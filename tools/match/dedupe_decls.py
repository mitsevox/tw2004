"""Drop duplicate top-level function prototypes in a merged sweep file.
    python tools/match/dedupe_decls.py src/<Name>.c [--dry-run]

When a name is declared more than once (column-0 lines only), one declaration is kept, at the
position of the FIRST one, so a call between the first and a later declaration never loses its
prototype. Its text is the first fully typed prototype (parameters or `(void)`), preferred over an
empty `()` one. Declarations that really disagree (another return type, other parameter types)
are left alone and reported: that is a conflict for a person to fix, not a duplicate.

Also warns about any deduped function that is used above its (kept) declaration.
Exits 2 when there are conflicts, 0 otherwise. --dry-run prints what it would do, writes nothing."""
import re, sys

PROTO = re.compile(r'^((?:(?:extern|static)\s+)?[A-Za-z_][\w \*]*?)\b(\w+)\((.*)\);\s*(//.*)?$')
BUILTIN = {'int', 'char', 'short', 'long', 'float', 'double', 'signed', 'unsigned', 'void',
           'const', 'volatile', 's8', 'u8', 's16', 'u16', 's32', 'u32', 's64', 'u64', 'f32',
           'f64', 'BOOL', 'bool', 'size_t', 'vu8', 'vu16', 'vu32', 'vs8', 'vs16', 'vs32'}


def split_params(p):
    out, depth, cur = [], 0, ''
    for ch in p:
        if ch == ',' and depth == 0:
            out.append(cur); cur = ''
            continue
        depth += ch in '([' and 1 or ch in ')]' and -1 or 0
        cur += ch
    return out + [cur]


def norm_type(t):
    t = re.sub(r'\s*\*\s*', '*', ' '.join(t.split()))
    return re.sub(r'\b(extern|static)\s+', '', t).strip()


def norm_param(p):
    """A parameter's type without its name: `Player* pPlayer` -> `Player*`, `u8` stays."""
    p = norm_type(p)
    m = re.match(r'^(.*?[\w\*])\s*\b([A-Za-z_]\w*)$', p)
    if m and '(' not in p and m.group(2) not in BUILTIN and re.search(r'\w', m.group(1)) \
            and not (m.group(1) in ('unsigned', 'signed') and m.group(2) in BUILTIN):
        return m.group(1).strip()
    return p


def signature(ret, params):
    """(return type, parameter types or None for an empty `()`)"""
    params = params.strip()
    if params == '':
        return norm_type(ret), None
    if params == 'void':
        return norm_type(ret), ()
    return norm_type(ret), tuple(norm_param(x) for x in split_params(params))


def dedupe(text):
    """-> (new text, dropped lines, conflicts, warnings)"""
    lines = text.split('\n')
    decl = {}
    for i, l in enumerate(lines):
        if l.startswith((' ', '\t', '#', '//', 'typedef', 'return')):
            continue
        m = PROTO.match(l)
        if m:
            decl.setdefault(m.group(2), []).append((i, m.group(1), m.group(3)))
    replace, drop, conflicts, warnings = {}, set(), [], []
    for name, lst in decl.items():
        if len(lst) < 2:
            continue
        sigs = [signature(r, p) for _, r, p in lst]
        rets = {s[0] for s in sigs}
        typed = {s[1] for s in sigs if s[1] is not None}
        if len(rets) > 1 or len(typed) > 1:
            conflicts.append((name, [(i + 1, lines[i]) for i, _, _ in lst]))
            continue
        first = lst[0][0]
        best = next((i for (i, _, _), s in zip(lst, sigs) if s[1] is not None), first)
        if any('static' in r.split() for _, r, _ in lst) and 'static' not in lst[0][1].split():
            best = next(i for i, r, _ in lst if 'static' in r.split())
        if best != first:
            replace[first] = lines[best]
        drop.update(i for i, _, _ in lst[1:])
        use = re.compile(r'\b%s\b' % re.escape(name))
        for j in range(first):
            if use.search(lines[j]) and not lines[j].lstrip().startswith('//'):
                warnings.append('%s is used on line %d, above its declaration on line %d'
                                % (name, j + 1, first + 1))
                break
    out = [replace.get(i, l) for i, l in enumerate(lines) if i not in drop]
    dropped = [(i + 1, lines[i]) for i in sorted(drop)]
    kept = [(i + 1, lines[i], replace[i]) for i in sorted(replace)]
    return '\n'.join(out), dropped, kept, conflicts, warnings


def main():
    args = [a for a in sys.argv[1:] if a != '--dry-run']
    dry = '--dry-run' in sys.argv
    p = args[0]
    s = open(p, encoding='utf-8').read()
    out, dropped, kept, conflicts, warnings = dedupe(s)
    print('dropping', [l for _, l in dropped])
    for ln, old, new in kept:
        print('line %d: %r -> %r (the typed prototype, at the first position)' % (ln, old, new))
    for w in warnings:
        print('WARNING:', w)
    for name, where in conflicts:
        print('CONFLICT: %s is declared differently, left as is:' % name)
        for ln, l in where:
            print('    %s:%d: %s' % (p, ln, l))
    if not dry and out != s:
        open(p, 'w', encoding='utf-8', newline='\n').write(out)
    sys.exit(2 if conflicts else 0)


if __name__ == '__main__':
    main()
