#!/usr/bin/env python3
"""Give every piece of a compiled C file's data its own section.

gcc emits a file's literals, switch tables and statics into a few shared
sections (.rdata, .data, .sdata, .sbss, .bss). In the original game those
sections hold the whole translation unit, so the pieces one function uses
sit between other functions' pieces, at their own alignment phases. A
section compiled from one function cannot always be placed as one block.

split(asm) cuts gcc's assembly at every data label into fragments, one
section each (.rodata.F<k>, .sdata.F<k>, ...), so the verifier and the build
can place each fragment at its own original address.

GAS treats only sections named exactly .sdata/.sbss as $gp-relative, and the
1999 GAS has no %gp_rel operator. So small-data fragments go to a companion
file under global names unique to the compiled file (`tag`), and the code
sees them as `.extern name, 4`, which gives the same $gp-relative
instructions. They stay global: GAS and ld -r fold the object's gp0 into
$gp-relative references to local symbols only. verify.compile_c merges the two
objects with ld -r and keeps the result only if every function is the same
instruction stream as gcc's own assembly gives (relocated fields masked,
relocation kinds equal); otherwise it uses the plain object.

.lit4 is GAS's pool for li.s constants (one entry per li.s, never shared);
each li.s that GAS would serve from it becomes `l.s $fN, <literal>` with the
literal as its own .lit4 fragment, so a function's literals can be placed
even when the original pool holds other functions' entries between them.
li.d into a general register is handled the same way: its double becomes an
.rodata fragment loaded through $at. .lit8 stays whole.
"""
import re
import struct

from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection

KIND = {'.rdata': '.rodata', '.rodata': '.rodata', '.data': '.data', '.sdata': '.sdata',
        '.sbss': '.sbss', '.bss': '.bss'}
SMALL = {'.sdata', '.sbss', '.lit4'}
NOBITS = {'.sbss', '.bss'}
TAG = '__rd'
LABEL = re.compile(r'^\s*([A-Za-z_.$][\w.$]*):(.*)$')
CONTENT = re.compile(r'^\s*\.(word|half|byte|ascii|asciz|asciiz|string|space|skip|zero|float|double|dword|'
                     r'quad|octa|2byte|4byte|8byte|gpword)\b', re.I)
PRE = re.compile(r'^\s*\.(align|p2align|balign|type|size|globl|global|local|weak)\b')
SYMTOK = re.compile(r'(?<![\w.$])([A-Za-z_.$][\w.$]*)')
FRAG = re.compile(r'^(\.rodata|\.data|\.sdata|\.sbss|\.bss|\.lit4)\.F\d+$')


def kind_of(name):
    """Data kind of an object section: '.rodata.F3' -> '.rodata', '.sdata' -> '.sdata',
    anything else (code, -fdata-sections names, metadata) -> None."""
    m = FRAG.match(name)
    if m:
        return m.group(1)
    return name if name in ('.rodata', '.data', '.sdata', '.sbss', '.bss', '.lit4', '.lit8') else None


def _section(line):
    """None if not a section directive; else ('previous',) or (key, directive)
    where key is a data kind ('.rodata', ...) or ('sec', name)."""
    s = line.split('#', 1)[0].strip()
    m = re.match(r'^\.(rdata|data|sdata|sbss|bss|text)\s*$', s)
    if m:
        n = '.' + m.group(1)
        return (KIND.get(n, n), '\t' + n if n == '.text' else None)
    if s == '.previous':
        return ('previous',)
    m = re.match(r'^\.section\s+([^\s,]+)', s)
    if m:
        n = m.group(1).strip('"')
        return (KIND[n], None) if n in KIND else (n, line)
    return None


def split(asm, tag=TAG):
    """Return (code_s, data_s, nfrag): the assembly with each data label in its
    own section, the moved small-data fragments ('' if none), and the number
    of fragments (0: nothing to do, use the assembly as it is)."""
    out = []                     # (section key, line) for code; ('frag', i) for fragments
    frags = []                   # dict(kind, lines, labels, content)
    directive = {'.text': '\t.text'}
    cur, prev = '.text', '.text'
    frag = None
    globls, textlabels = set(), set()
    deferred = []                # li.d literal fragments, placed after the code

    def close():
        nonlocal frag
        if frag is None:
            return
        if not frag['labels'] and not frag['content']:
            pass                     # only directives (a trailing .align): nothing to place
        elif not frag['labels'] and frags and out and out[-1] == ('frag', len(frags) - 1) \
                and frags[-1]['kind'] == frag['kind']:
            frags[-1]['lines'] += frag['lines']             # unlabelled continuation
        else:
            frags.append(frag)
            out.append(('frag', len(frags) - 1))
        frag = None

    for l in asm.splitlines():
        m = re.match(r'^\s*\.(globl|global)\s+([A-Za-z_.$][\w.$]*)', l)
        if m:
            globls.add(m.group(2))
        sec = _section(l)
        if sec is not None:
            close()
            if sec[0] == 'previous':
                cur, prev = prev, cur
            else:
                cur, prev = sec[0], cur
                if sec[1] is not None and cur not in directive:
                    directive[cur] = sec[1]
            if cur in KIND.values():
                frag = dict(kind=cur, lines=[], labels=[], content=False)
            continue
        if cur not in KIND.values():
            lm = LABEL.match(l)
            if lm and cur.startswith('.text'):
                textlabels.add(lm.group(1))
            lit = LI_S.match(l)
            if lit and _literal(lit.group(3)):
                # the .lit4 entry GAS would make for this li.s, as a fragment
                name = f'$LIT{len(frags)}'
                frags.append(dict(kind='.lit4', labels=[name], content=True,
                                  lines=['\t.align\t2', f'{name}:', f'\t.float\t{lit.group(3)}']))
                out.append(('frag', len(frags) - 1))
                l = f'{lit.group(1)}l.s\t{lit.group(2)},{name}'
            litd = LI_D.match(l)
            if litd and not _simple_double(litd.group(3)):
                # li.d into a GPR: GAS appends the double to the end of .rdata at
                # this point of the file and loads it through $at. Give it its own
                # fragment so it can sit between other functions' data, as it does
                # in the original whole-TU .rodata.
                name = f'$LITD{len(frags)}'
                frags.append(dict(kind='.rodata', labels=[name], content=True,
                                  lines=['\t.align\t3', f'{name}:', f'\t.double\t{litd.group(3)}']))
                # emitted after all the code: a section switch here would make GAS
                # flush pending hazard nops (mips_emit_delays) into the function
                deferred.append(len(frags) - 1)
                ind, reg = litd.group(1), litd.group(2)
                for x in (f'lui\t$1,%hi({name})', f'ld\t{reg},%lo({name})($1)'):
                    out.append((cur, f'{ind}{x}'))
                continue
            out.append((cur, l))
            continue
        lm = LABEL.match(l)
        if lm:
            if frag['content'] or frag['labels']:
                # the directives after the last content line belong to this label
                k = len(frag['lines'])
                while k > 0 and (PRE.match(frag['lines'][k - 1]) or not frag['lines'][k - 1].strip()):
                    k -= 1
                carry, kind = frag['lines'][k:], frag['kind']
                frag['lines'] = frag['lines'][:k]
                close()
                frag = dict(kind=kind, lines=carry, labels=[], content=False)
            frag['labels'].append(lm.group(1))
            if lm.group(2).strip():
                frag['content'] = True
        elif CONTENT.match(l):
            frag['content'] = True
        frag['lines'].append(l)
    close()
    out += [('frag', i) for i in deferred]
    if not frags:
        return asm, '', 0

    home = {n: i for i, f in enumerate(frags) for n in f['labels']}
    moved = {i for i, f in enumerate(frags) if f['kind'] in SMALL}
    cross = set()
    for i in moved:
        cross.update(frags[i]['labels'])
        for l in frags[i]['lines']:
            for t in SYMTOK.findall(_code(l)):
                if t in textlabels:
                    raise ValueError(f'small-data fragment refers to code label {t}')
                if t in home and home[t] not in moved:
                    cross.add(t)
    rename = {n: f'{tag}{home[n]}.{n.lstrip("$.")}' for n in cross if n not in globls}

    def sub(l):
        if not rename:
            return l
        body, sep, comment = l.partition('#')
        parts = body.split('"')                     # leave string contents alone
        parts[::2] = [SYMTOK.sub(lambda m: rename.get(m.group(1), m.group(1)), p) for p in parts[::2]]
        return '"'.join(parts) + sep + comment

    def emit(i):
        f = frags[i]
        flags = '"aw",@nobits' if f['kind'] in NOBITS else \
            ('"a",@progbits' if f['kind'] == '.rodata' else '"aw",@progbits')
        res = [f'\t.section {f["kind"]}.F{i},{flags}']
        for l in f['lines']:
            lm = LABEL.match(l)
            if lm and lm.group(1) in cross:
                res.append(f'\t.globl\t{rename.get(lm.group(1), lm.group(1))}')
            res.append(sub(l))
        return res

    code, data, last = [], [], '.text'
    for key, v in out:
        if key == 'frag':
            if v in moved:
                data += emit(v)
            else:
                code += emit(v)
                last = None
            continue
        if key != last:
            code.append(directive.get(key, f'\t.section {key}'))
            last = key
        code.append(sub(v))
    for i in sorted(moved):
        for n in frags[i]['labels']:
            code.append(f'\t.extern\t{rename.get(n, n)}, 4')
    return '\n'.join(code) + '\n', ('\n'.join(data) + '\n' if data else ''), len(frags)


LI_S = re.compile(r'^(\s*)li\.s\s+(\$f\d+)\s*,\s*([-+0-9.eE]+)\s*(#.*)?$')
LI_D = re.compile(r'^(\s*)li\.d\s+(\$\d+)\s*,\s*([-+0-9.eE]+)\s*(#.*)?$')


def _literal(text):
    """Whether GAS puts li.s's constant in .lit4: it loads it with lui/ori
    instead when either 16-bit half of the single-precision bits is zero."""
    bits = struct.unpack('<I', struct.pack('<f', float(text)))[0]
    return bool(bits & 0xffff) and bool(bits >> 16)


def _simple_double(text):
    """Whether GAS builds li.d's 64-bit value with instructions instead of a literal:
    when each 32-bit half of the double has a zero 16-bit half (mips_ip, 'F')."""
    bits = struct.unpack('<Q', struct.pack('<d', float(text)))[0]
    lo, hi = bits & 0xffffffff, bits >> 32
    return (lo & 0xffff == 0 or lo >> 16 == 0) and (hi & 0xffff == 0 or hi >> 16 == 0)


def _code(line):
    """The part of an assembly line outside comments and string literals."""
    body = line.partition('#')[0]
    return ' '.join(body.split('"')[::2])


# relocation kinds that may stand for each other (a section-relative and a
# symbol-relative reference, a .lit4 literal and a $gp-relative symbol)
RCLASS = {4: 'j', 5: 'hi', 6: 'lo', 7: 'gp', 8: 'gp', 2: 'w'}
RMASK = {'j': 0x03ffffff, 'hi': 0xffff, 'lo': 0xffff, 'gp': 0xffff, 'w': 0xffffffff}


def code_shape(path):
    """{text section: [(word with relocated field masked, relocation class)]}."""
    with open(path, 'rb') as f:
        e = ELFFile(f)
        secs = {i: s for i, s in enumerate(e.iter_sections())}
        rel = {}
        for s in e.iter_sections():
            if isinstance(s, RelocationSection):
                tgt = secs[s['sh_info']].name
                for r in s.iter_relocations():
                    rel.setdefault(tgt, {})[r['r_offset']] = RCLASS.get(r['r_info_type'], r['r_info_type'])
        out = {}
        for s in e.iter_sections():
            if s.name.startswith('.text') and s['sh_size']:
                b = s.data()
                ws = struct.unpack('<%dI' % (len(b) // 4), b[:len(b) // 4 * 4])
                rs = rel.get(s.name, {})
                out[s.name] = [(w & ~RMASK.get(rs.get(4 * i), 0) & 0xffffffff, rs.get(4 * i))
                               for i, w in enumerate(ws)]
        return out
