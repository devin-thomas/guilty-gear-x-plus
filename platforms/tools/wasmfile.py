"""Minimal WebAssembly reader/writer for the port's build tools.

It reads what clang and wasm-ld emit for the Core (MVP sections, the "linking"
symbol table and "reloc.*" sections) and can patch bytes in place. It is not a
general validator.
"""
import struct

SEC_CUSTOM, SEC_TYPE, SEC_IMPORT, SEC_FUNCTION, SEC_TABLE, SEC_MEMORY, SEC_GLOBAL, \
    SEC_EXPORT, SEC_START, SEC_ELEM, SEC_CODE, SEC_DATA, SEC_DATACOUNT = range(13)

VALTYPES = {0x7f: 'i32', 0x7e: 'i64', 0x7d: 'f32', 0x7c: 'f64', 0x7b: 'v128',
            0x70: 'funcref', 0x6f: 'externref'}

# linking section
WASM_SYMBOL_TABLE = 8
SYM_FUNCTION, SYM_DATA, SYM_GLOBAL, SYM_SECTION, SYM_TAG, SYM_TABLE = range(6)
FLAG_BINDING_WEAK = 0x1
FLAG_BINDING_LOCAL = 0x2
FLAG_UNDEFINED = 0x10
FLAG_EXPLICIT_NAME = 0x40
FLAG_ABSOLUTE = 0x200

# relocation types (llvm/BinaryFormat/WasmRelocs.def)
R_FUNCTION_INDEX_LEB = 0
R_TABLE_INDEX_SLEB = 1
R_TABLE_INDEX_I32 = 2
R_MEMORY_ADDR_LEB = 3
R_MEMORY_ADDR_SLEB = 4
R_MEMORY_ADDR_I32 = 5
R_TYPE_INDEX_LEB = 6
R_GLOBAL_INDEX_LEB = 7
R_FUNCTION_OFFSET_I32 = 8
R_SECTION_OFFSET_I32 = 9
R_TAG_INDEX_LEB = 10
R_MEMORY_ADDR_REL_SLEB = 11
R_TABLE_INDEX_REL_SLEB = 12
R_GLOBAL_INDEX_I32 = 13
R_MEMORY_ADDR_LEB64 = 14
R_MEMORY_ADDR_SLEB64 = 15
R_MEMORY_ADDR_I64 = 16
R_MEMORY_ADDR_REL_SLEB64 = 17
R_TABLE_INDEX_SLEB64 = 18
R_TABLE_INDEX_I64 = 19
R_TABLE_NUMBER_LEB = 20
R_MEMORY_ADDR_TLS_SLEB = 21
R_FUNCTION_OFFSET_I64 = 22
R_MEMORY_ADDR_LOCREL_I32 = 23
R_TABLE_INDEX_REL_SLEB64 = 24
R_MEMORY_ADDR_TLS_SLEB64 = 25
R_FUNCTION_INDEX_I32 = 26
HAS_ADDEND = {R_MEMORY_ADDR_LEB, R_MEMORY_ADDR_SLEB, R_MEMORY_ADDR_I32, R_FUNCTION_OFFSET_I32,
              R_SECTION_OFFSET_I32, R_MEMORY_ADDR_REL_SLEB, R_MEMORY_ADDR_LEB64,
              R_MEMORY_ADDR_SLEB64, R_MEMORY_ADDR_I64, R_MEMORY_ADDR_REL_SLEB64,
              R_MEMORY_ADDR_TLS_SLEB, R_FUNCTION_OFFSET_I64, R_MEMORY_ADDR_LOCREL_I32,
              R_MEMORY_ADDR_TLS_SLEB64}
MEMORY_RELOCS = {R_MEMORY_ADDR_LEB, R_MEMORY_ADDR_SLEB, R_MEMORY_ADDR_I32}
TABLE_RELOCS = {R_TABLE_INDEX_SLEB, R_TABLE_INDEX_I32}


class Reader:
    def __init__(self, data, pos=0, end=None):
        self.data, self.pos = data, pos
        self.end = len(data) if end is None else end

    def u8(self):
        b = self.data[self.pos]
        self.pos += 1
        return b

    def uleb(self):
        result = shift = 0
        while True:
            b = self.u8()
            result |= (b & 0x7f) << shift
            shift += 7
            if not b & 0x80:
                return result

    def sleb(self):
        result = shift = 0
        while True:
            b = self.u8()
            result |= (b & 0x7f) << shift
            shift += 7
            if not b & 0x80:
                if b & 0x40:
                    result -= 1 << shift
                return result

    def bytes(self, n):
        b = self.data[self.pos:self.pos + n]
        self.pos += n
        return b

    def name(self):
        return self.bytes(self.uleb()).decode('utf-8')

    def u32(self):
        v = struct.unpack_from('<I', self.data, self.pos)[0]
        self.pos += 4
        return v

    def done(self):
        return self.pos >= self.end


def uleb_bytes(v, width=None):
    out = bytearray()
    while True:
        b = v & 0x7f
        v >>= 7
        if v or (width and len(out) + 1 < width):
            out.append(b | 0x80)
        else:
            out.append(b)
            break
    if width and len(out) != width:
        raise ValueError('value does not fit the padded LEB')
    return bytes(out)


def sleb_bytes(v, width=None):
    out = bytearray()
    while True:
        b = v & 0x7f
        v >>= 7
        last = (v == 0 and not b & 0x40) or (v == -1 and b & 0x40)
        if last and not (width and len(out) + 1 < width):
            out.append(b)
            break
        out.append(b | 0x80)
    if width and len(out) != width:
        raise ValueError('value does not fit the padded LEB')
    return bytes(out)


def section_bytes(sid, payload):
    return bytes([sid]) + uleb_bytes(len(payload)) + payload


class Section:
    def __init__(self, sid, start, end, name=None):
        self.id, self.start, self.end, self.name = sid, start, end, name


class Symbol:
    def __init__(self, kind, flags, name, index=None, segment=None, offset=None, size=None):
        self.kind, self.flags, self.name = kind, flags, name
        self.index, self.segment, self.offset, self.size = index, segment, offset, size

    @property
    def undefined(self):
        return bool(self.flags & FLAG_UNDEFINED)

    @property
    def local(self):
        return bool(self.flags & FLAG_BINDING_LOCAL)


class Module:
    """A parsed module. Offsets are into self.data (a bytearray)."""

    def __init__(self, data):
        self.data = bytearray(data)
        if self.data[:8] != b'\0asm\1\0\0\0':
            raise ValueError('not a wasm module')
        self.sections = []
        r = Reader(self.data, 8)
        while not r.done():
            sid = r.u8()
            size = r.uleb()
            start = r.pos
            name = None
            if sid == SEC_CUSTOM:
                name = Reader(self.data, start).name()
            self.sections.append(Section(sid, start, start + size, name))
            r.pos = start + size
        self._parse()

    def section(self, sid, name=None):
        for s in self.sections:
            if s.id == sid and (name is None or s.name == name):
                return s
        return None

    def _parse(self):
        d = self.data
        self.types, self.imports, self.func_types, self.func_imports = [], [], [], []
        self.num_imported_funcs = self.num_imported_globals = 0
        s = self.section(SEC_TYPE)
        if s:
            r = Reader(d, s.start, s.end)
            for _ in range(r.uleb()):
                assert r.u8() == 0x60
                params = tuple(VALTYPES[r.u8()] for _ in range(r.uleb()))
                results = tuple(VALTYPES[r.u8()] for _ in range(r.uleb()))
                self.types.append((params, results))
        s = self.section(SEC_IMPORT)
        if s:
            r = Reader(d, s.start, s.end)
            for _ in range(r.uleb()):
                mod, field, kind = r.name(), r.name(), r.u8()
                if kind == 0:
                    t = r.uleb()
                    self.imports.append((mod, field, kind, t))
                    self.func_imports.append((mod, field))
                    self.func_types.append(t)
                    self.num_imported_funcs += 1
                elif kind == 1:
                    r.u8()
                    self._limits(r)
                    self.imports.append((mod, field, kind, None))
                elif kind == 2:
                    self._limits(r)
                    self.imports.append((mod, field, kind, None))
                elif kind == 3:
                    r.u8()
                    r.u8()
                    self.imports.append((mod, field, kind, None))
                    self.num_imported_globals += 1
                else:
                    raise ValueError(f'import kind {kind}')
        s = self.section(SEC_FUNCTION)
        if s:
            r = Reader(d, s.start, s.end)
            self.func_types += [r.uleb() for _ in range(r.uleb())]
        self.symbols = []
        s = self.section(SEC_CUSTOM, 'linking')
        if s:
            self._parse_linking(s)
        self.relocs = {}
        for s in self.sections:
            if s.id == SEC_CUSTOM and s.name.startswith('reloc.'):
                r = Reader(d, s.start, s.end)
                r.name()
                target = r.uleb()
                entries = []
                for _ in range(r.uleb()):
                    rtype = r.u8()
                    offset = r.uleb()
                    index = r.uleb()
                    addend = r.sleb() if rtype in HAS_ADDEND else 0
                    entries.append((rtype, offset, index, addend))
                self.relocs[target] = entries

    @staticmethod
    def _limits(r):
        flags = r.u8()
        lo = r.uleb()
        hi = r.uleb() if flags & 1 else None
        return flags, lo, hi

    def _parse_linking(self, s):
        r = Reader(self.data, s.start, s.end)
        r.name()
        assert r.uleb() == 2, 'linking metadata version'
        while not r.done():
            kind = r.u8()
            size = r.uleb()
            end = r.pos + size
            if kind == WASM_SYMBOL_TABLE:
                for _ in range(r.uleb()):
                    sk = r.u8()
                    flags = r.uleb()
                    if sk in (SYM_FUNCTION, SYM_GLOBAL, SYM_TAG, SYM_TABLE):
                        index = r.uleb()
                        name = None
                        if not flags & FLAG_UNDEFINED or flags & FLAG_EXPLICIT_NAME:
                            name = r.name()
                        elif sk == SYM_FUNCTION:
                            name = self.func_imports[index][1]
                        self.symbols.append(Symbol(sk, flags, name, index=index))
                    elif sk == SYM_DATA:
                        name = r.name()
                        if not flags & FLAG_UNDEFINED:
                            seg, off, sz = r.uleb(), r.uleb(), r.uleb()
                            self.symbols.append(Symbol(sk, flags, name, segment=seg, offset=off, size=sz))
                        else:
                            self.symbols.append(Symbol(sk, flags, name))
                    elif sk == SYM_SECTION:
                        self.symbols.append(Symbol(sk, flags, None, index=r.uleb()))
                    else:
                        raise ValueError(f'symbol kind {sk}')
            r.pos = end

    def function_names(self):
        """{function index: name} from the "name" section (subsection 1)."""
        names = {}
        s = self.section(SEC_CUSTOM, 'name')
        if s is None:
            return names
        r = Reader(self.data, s.start, s.end)
        r.name()
        while not r.done():
            sub, size = r.u8(), r.uleb()
            end = r.pos + size
            if sub == 1:
                for _ in range(r.uleb()):
                    idx = r.uleb()
                    names[idx] = r.name()
            r.pos = end
        return names

    def signature(self, func_index):
        return self.types[self.func_types[func_index]]

    def section_index(self, sec):
        return self.sections.index(sec)


def fmt_sig(sig):
    params, results = sig
    return '(' + ', '.join(params) + ') -> ' + (', '.join(results) if results else 'void')
