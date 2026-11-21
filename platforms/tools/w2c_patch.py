#!/usr/bin/env python3
"""w2c_patch.py <w2c dir>: call_indirect as the EE does it, and traps that say why.

The game calls many functions through pointers whose C type disagrees with
the function: a handler table typed `void (*)(CHARACTER_WORK *)` holds
handlers that return int, or that take fewer arguments than the call
passes. On the EE that is harmless (the caller ignores v0; the callee never
reads the extra argument registers). WebAssembly checks the exact signature
and traps.

This rewrites wasm2c's CHECK_CALL_INDIRECT so a call also goes through when
the target's signature differs from the caller's in only those two ways:
the caller ignores the target's result, or passes more arguments than the
target takes, with the target's own parameters matching. The call then runs
through the caller's C function type; on every supported ABI (x86-64
System V and Windows, AArch64) the extra argument registers are ignored and
the unused return register is never read. Every other mismatch (too few
arguments, a void target where the caller uses the result, a different
parameter type) still traps, after reporting the index to the host
(ggx_core_bad_indirect in core_host.c).

Each wasm2c function type is a 32-byte tag the generated code compares by
address or content. The script reads every table entry's tag and the C
signature of the wrapper it points to, and every call site's tag and C
function type, so it knows the signature behind each tag without assuming
how wasm2c numbers them.

It also makes every store mark its 4 KiB page in ggx_core_dirty, which is
how core_state.c knows which pages a snapshot must copy and the State Hash
must rehash. With GGX_STATE_SCAN defined, every load marks ggx_core_read
too, for the poison/scan test. And it checks that the instance struct
holds only the memory, the function table and the stack pointer: a new
mutable global would be Core state outside the regions. Idempotent.
"""
import re
import sys
from pathlib import Path

MARK = '/* ggx: w2c_patch.py */'

# {RefFunc, w2c_ggx_t3, (wasm_rt_function_ptr_t)wrap_w2c_ggx_Name, {NULL}, 0},
REF = re.compile(r'\{RefFunc, (w2c_\w+_t\d+), \(wasm_rt_function_ptr_t\)(\w+),')
# static u32 wrap_w2c_ggx_Name(void *instance, u32 var_p0, ...) {
WRAP = re.compile(r'^static (\w+) (wrap_w2c_\w+)\(void\s*\*\s*\w+((?:,\s*\w+ \w+)*)\)\s*\{', re.M)
# CALL_INDIRECT(table, u32 (*)(void*, u32, u32), w2c_ggx_t5, ...
CALL = re.compile(r'CALL_INDIRECT\([^,]+, (\w+) \(\*\)\(void\*((?:, \w+)*)\), (w2c_\w+_t\d+),')


def kinds(params):
    return tuple(p.strip().split()[0] for p in params.split(',') if p.strip())


STATE_MARK = '/* ggx: w2c_patch.py state marks */'
STATE_DECLS = STATE_MARK + r'''
extern u8 ggx_core_dirty[];
extern u8 ggx_core_read[];
/* a store marks the page of its first byte, and the next page if its last
   byte is there */
#define GGX_STORE_MARK(addr, n)                                \
  do {                                                         \
    ggx_core_dirty[(u32)(addr) >> 12] = 1;                     \
    if (UNLIKELY(((u32)(addr) & 0xfff) > 0x1000 - (n)))        \
      ggx_core_dirty[((u32)(addr) >> 12) + 1] = 1;             \
  } while (0)
#ifdef GGX_STATE_SCAN
/* the first read of each page goes to the host, which reports any outside
   the regions with the Core function that made it */
void ggx_core_first_read(u32 addr);
#define GGX_LOAD_MARK(addr, n)                                 \
  do {                                                         \
    if (UNLIKELY(!ggx_core_read[(u32)(addr) >> 12]))           \
      ggx_core_first_read((u32)(addr));                        \
    if (UNLIKELY(!ggx_core_read[(u32)((addr) + (n) - 1) >> 12])) \
      ggx_core_first_read((u32)((addr) + (n) - 1));            \
  } while (0)
#else
#define GGX_LOAD_MARK(addr, n) ((void)0)
#endif
static inline void ggx_mark_range(u8 *map, u64 a, u64 n) {
  if (n)
    memset(map + ((u32)a >> 12), 1, ((u32)(a + n - 1) >> 12) - ((u32)a >> 12) + 1);
}
#ifdef GGX_STATE_SCAN
#define GGX_LOAD_RANGE(a, n) ggx_mark_range(ggx_core_read, a, n)
#else
#define GGX_LOAD_RANGE(a, n) ((void)0)
#endif
'''

# (what wasm2c 1.0.41 writes, what it becomes); each must occur exactly once
STATE_EDITS = [
    ('#include "ggx_core.h"\n', '#include "ggx_core.h"\n' + STATE_DECLS),
    # DEFINE_LOAD
    ('    wasm_rt_memcpy(&result, MEM_ADDR_MEMOP(mem, addr, sizeof(t1)),     \\\n'
     '                   sizeof(t1));',
     '    wasm_rt_memcpy(&result, MEM_ADDR_MEMOP(mem, addr, sizeof(t1)),     \\\n'
     '                   sizeof(t1)); GGX_LOAD_MARK(addr, sizeof(t1));'),
    # DEFINE_STORE
    ('    wasm_rt_memcpy(MEM_ADDR_MEMOP(mem, addr, sizeof(t1)), &wrapped,    \\\n'
     '                   sizeof(t1));',
     '    wasm_rt_memcpy(MEM_ADDR_MEMOP(mem, addr, sizeof(t1)), &wrapped,    \\\n'
     '                   sizeof(t1)); GGX_STORE_MARK(addr, sizeof(t1));'),
    ('  memset(MEM_ADDR(mem, d, n), val, n);\n',
     '  memset(MEM_ADDR(mem, d, n), val, n);\n  ggx_mark_range(ggx_core_dirty, d, n);\n'),
    ('  memmove(MEM_ADDR(dest, dest_addr, n), MEM_ADDR(src, src_addr, n), n);\n',
     '  memmove(MEM_ADDR(dest, dest_addr, n), MEM_ADDR(src, src_addr, n), n);\n'
     '  ggx_mark_range(ggx_core_dirty, dest_addr, n);\n  GGX_LOAD_RANGE(src_addr, n);\n'),
    ('  LOAD_DATA((*dest), dest_addr, src + src_addr, n);\n',
     '  LOAD_DATA((*dest), dest_addr, src + src_addr, n);\n  ggx_mark_range(ggx_core_dirty, dest_addr, n);\n'),
]

# The instance struct core_state.c expects: the env pointer, and the Core's
# only mutable state outside its memory.
INSTANCE_FIELDS = ['struct w2c_env* w2c_env_instance', 'u32 w2c_0x5F_stack_pointer0x2E1',
                   'wasm_rt_memory_t w2c_memory', 'wasm_rt_funcref_table_t w2c_0x5F_indirect_function_table']


def patch_state(d, impl):
    header = next(p for p in d.glob('*.h') if not p.name.endswith('-impl.h'))
    m = re.search(r'typedef struct w2c_\w+ \{(.*?)\} w2c_\w+;', header.read_text(), re.S)
    fields = [f.strip() for f in m.group(1).split(';') if f.strip()] if m else None
    if fields != INSTANCE_FIELDS:
        raise SystemExit(f'{header}: the Core instance holds {fields}, not {INSTANCE_FIELDS}; '
                         'a new member is Core state that core_state.c must save and hash')
    s = impl.read_text()
    if STATE_MARK in s:
        return
    for old, new in STATE_EDITS:
        if s.count(old) != 1:
            raise SystemExit(f'{impl}: expected one {old.strip()[:60]!r}; '
                             'this wasm2c is not the one w2c_patch.py knows')
        s = s.replace(old, new)
    impl.write_text(s, newline='\n')


def main():
    d = Path(sys.argv[1])
    impl = next(d.glob('*-impl.h'))
    patch_state(d, impl)
    text = ''.join(p.read_text() for p in sorted(d.glob('*.c')))
    wraps = {name: (ret, kinds(params)) for ret, name, params in WRAP.findall(text)}
    sig = {}
    for tag, wrap in REF.findall(text):
        if wrap in wraps:
            sig[tag] = wraps[wrap]
    for ret, params, tag in CALL.findall(text):
        sig.setdefault(tag, (ret, kinds(params)))
    if not sig:
        raise SystemExit(f'{d}: no table entries or call sites found; this wasm2c output is unfamiliar')

    rows = []
    for tag, (ret, params) in sorted(sig.items(), key=lambda kv: int(kv[0].rsplit('_t', 1)[1])):
        rows.append(f'  {{ &{tag}, "{ret}", "{",".join(params)}" }},')
    patch = MARK + '''
void ggx_core_bad_indirect(u32 index, int missing);

/* a type tag is a `const char *const` under GCC and clang and a char array
   elsewhere (FUNC_TYPE_EXTERN_T); the table holds its address either way */
#if defined(__GNUC__) || defined(__clang__)
#define GGX_TAG(p) (*(const char *const *)(p))
#else
#define GGX_TAG(p) ((const char *)(p))
#endif
struct ggx_func_sig { const void *type; const char *ret; const char *params; };
static const struct ggx_func_sig ggx_func_sigs[] = {
''' + '\n'.join(rows) + '''
};

static const struct ggx_func_sig *ggx_sig_of(const char *type)
{
  size_t i;
  for (i = 0; i < sizeof(ggx_func_sigs) / sizeof(ggx_func_sigs[0]); i++)
    if (func_types_eq(type, GGX_TAG(ggx_func_sigs[i].type)))
      return &ggx_func_sigs[i];
  return NULL;
}

/* the EE-tolerated mismatches: an ignored result, extra arguments */
static int ggx_call_compatible(const char *caller, const char *callee)
{
  const struct ggx_func_sig *c = ggx_sig_of(caller), *e = ggx_sig_of(callee);
  size_t n;
  if (!c || !e)
    return 0;
  if (strcmp(c->ret, "void") && strcmp(c->ret, e->ret))
    return 0;
  n = strlen(e->params);
  return !strncmp(c->params, e->params, n) && (c->params[n] == 0 || c->params[n] == ',' || n == 0);
}

#ifdef CHECK_CALL_INDIRECT
#undef CHECK_CALL_INDIRECT
#define CHECK_CALL_INDIRECT(table, ft, x)                                                   \\
  (LIKELY((x) < table.size && table.data[x].func &&                                         \\
          (func_types_eq(ft, table.data[x].func_type) ||                                    \\
           ggx_call_compatible(ft, table.data[x].func_type))) ||                            \\
   (ggx_core_bad_indirect((x), !((x) < table.size && table.data[x].func)), TRAP(CALL_INDIRECT)))
#endif
'''
    s = impl.read_text()
    if 'define CHECK_CALL_INDIRECT' not in s:
        raise SystemExit(f'{impl}: no CHECK_CALL_INDIRECT; this wasm2c is not the one w2c_patch.py knows')
    if MARK in s:
        s = s[:s.index(MARK)]
    impl.write_text(s + patch, newline='\n')


if __name__ == '__main__':
    main()
