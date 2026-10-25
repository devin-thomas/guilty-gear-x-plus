#!/usr/bin/env python3
"""Split a C file into its top-level declarations and function definitions.

split(text) -> (prelude, funcs): prelude is every top-level chunk that is not
a function body (preprocessor lines, typedefs, prototypes, externs, variable
definitions), in order; funcs is [(name, text)] for each function definition.
Comments, strings and character literals are skipped when counting braces.
"""
import re
import sys


def _chunks(text):
    """Yield top-level chunks: preprocessor lines, ';'-terminated declarations
    and brace-terminated function bodies."""
    i, n, start, depth = 0, len(text), 0, 0
    seen_brace_at0 = False
    while i < n:
        c = text[i]
        if depth == 0 and c == '#' and \
                re.sub(r'/\*.*?\*/|//[^\n]*', '', text[start:i], flags=re.S).strip() == '':
            j = text.find('\n', i)
            while j != -1 and text[j - 1] == '\\':
                j = text.find('\n', j + 1)
            j = n if j == -1 else j + 1
            yield text[start:j]
            i = start = j
            continue
        if text.startswith('/*', i):
            j = text.find('*/', i + 2)
            i = n if j == -1 else j + 2
            continue
        if text.startswith('//', i):
            j = text.find('\n', i)
            i = n if j == -1 else j
            continue
        if c in '"\'':
            j = i + 1
            while j < n and text[j] != c:
                j += 2 if text[j] == '\\' else 1
            i = j + 1
            continue
        if c == '{':
            if depth == 0:
                head = re.sub(r'/\*.*?\*/|//[^\n]*', '', text[start:i], flags=re.S).strip()
                seen_brace_at0 = head.endswith(')') and '=' not in head.split('(')[0]
            depth += 1
        elif c == '}':
            depth -= 1
            if depth == 0 and seen_brace_at0:
                yield text[start:i + 1]
                start = i + 1
                seen_brace_at0 = False
        elif c == ';' and depth == 0:
            yield text[start:i + 1]
            start = i + 1
        i += 1
    if text[start:].strip():
        yield text[start:]


def split(text):
    prelude, funcs = [], []
    for ch in _chunks(text):
        body = re.sub(r'/\*.*?\*/|//[^\n]*', '', ch, flags=re.S)
        brace = body.find('{')
        if brace != -1 and body[:brace].rstrip().endswith(')') and body.rstrip().endswith('}') \
                and not body.lstrip().startswith(('typedef', 'struct', 'union', 'enum')):
            head = body[:brace]
            depth, k = 0, len(head.rstrip()) - 1
            while k >= 0:                        # find the '(' matching the last ')'
                depth += {')': 1, '(': -1}.get(head[k], 0)
                if depth == 0:
                    break
                k -= 1
            m = re.search(r'(\w+)\s*$', head[:k])
            funcs.append((m.group(1), ch))
        else:
            prelude.append(ch)
    return prelude, funcs


def _top_eq(text):
    """Index of the first '=' outside (), [] and {}, or -1."""
    depth = 0
    for i, c in enumerate(text):
        if c in '([{':
            depth += 1
        elif c in ')]}':
            depth -= 1
        elif c == '=' and depth == 0:
            return i
    return -1


def data_definition(chunk):
    """For a file-scope variable definition with an initializer, return
    (name, extern_declaration); otherwise None."""
    body = re.sub(r'/\*.*?\*/|//[^\n]*', '', chunk, flags=re.S).strip()
    if not body.endswith(';') or body.startswith(('#', 'typedef', 'extern')):
        return None
    eq = _top_eq(body)
    if eq == -1:
        return None
    decl = body[:eq].strip()
    if re.match(r'(struct|union|enum)\s+\w+\s*\{', decl):
        return None
    bare = re.sub(r'\([^()]*\)\s*$', '', decl)            # a function pointer's parameter list
    m = re.search(r'(\w+)\s*(?:\[[^\]]*\]\s*)*\)?\s*(?:\[[^\]]*\]\s*)*$', bare)
    if not m:
        return None
    return m.group(1), 'extern ' + re.sub(r'^static\s+', '', decl) + ';'


def prototype(ftext):
    """'int f(int a) { ... }' -> 'int f(int a);' (comments dropped)."""
    body = re.sub(r'/\*.*?\*/|//[^\n]*', '', ftext, flags=re.S)
    return body[:body.find('{')].strip() + ';'


if __name__ == '__main__':
    pre, fs = split(open(sys.argv[1]).read())
    print(f'{len(pre)} prelude chunks; functions: {[n for n, _ in fs]}')
