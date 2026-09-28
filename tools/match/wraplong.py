"""Wrap C code lines longer than lint's 110 columns at an argument comma (a rename that makes names
longer pushes call lines over). The continuation lines up under the open parenthesis (or 8 past the
statement's indent when that is too deep). Comment-only lines, preprocessor lines and lines holding a
string or a // comment are left alone (reported instead).
    python tools/match/wraplong.py <file:line> ...        e.g. from `lint.py --diff main`
    python tools/match/wraplong.py --from-lint [--diff REV]   every long-line finding of lint --diff REV
                                                          (default main; name.py passes HEAD)
Wrapping moves the following lines down, so a __LINE__ constant below it changes: rebuild and check
main.dol: OK."""
import pathlib, re, subprocess, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
LIMIT = 110


def split(line):
    """[first, rest] or None. Break points: after an argument comma, before a binary operator, or
    (last resort) right after an open parenthesis. The shallowest one (fewest open parentheses)
    that keeps the first part <= LIMIT and shortens the line wins, the rightmost of those, so
    `a && f(x, y)` breaks at the && and not inside f's arguments. Nothing inside a string or
    character literal counts."""
    base = len(line) - len(line.lstrip())
    stack, cands, quote, i, square = [], [], None, 0, 0
    while i < len(line):
        c = line[i]
        if quote:
            if c == '\\':
                i += 2
                continue
            if c == quote:
                quote = None
        elif c in '"\'':
            quote = c
        elif line.startswith('//', i):
            break
        elif c == '[':
            square += 1
        elif c == ']':
            square = max(0, square - 1)
        elif c == '(':
            if base < i < LIMIT - 1 and line[i + 1:i + 2] not in (')', ''):
                cands.append((len(stack) + 2, i, 'paren', None))
            stack.append(i)
        elif c == ')':
            if stack:
                stack.pop()
        elif c == ',' and stack and i + 1 < LIMIT and line[i + 1:i + 2] == ' ':
            cands.append((len(stack) + 4 * square, i, 'comma', stack[-1]))
        elif c == ' ' and base < i < LIMIT:
            m = re.match(r' (&&|\|\||<<|>>|[-+*/%&|^]|==|!=|<=|>=|<|>|=) ', line[i:])
            if m:
                # an assignment's = is the last resort: `a->b\n = f(...)` strands the target alone
                rank = len(stack) + (3 if m.group(1) == '=' else 0) + 4 * square   # never inside [ ]
                cands.append((rank, i, 'op', stack[-1] if stack else None))
        i += 1

    def parts(c):
        _, i, kind, paren = c
        indent = paren + 1 if paren is not None and paren + 1 <= 60 else base + 8
        if kind == 'comma':
            return [line[:i + 1].rstrip(), ' ' * indent + line[i + 1:].lstrip()]
        if kind == 'paren':
            return [line[:i + 1], ' ' * indent + line[i + 1:].lstrip()]
        return [line[:i].rstrip(), ' ' * indent + line[i + 1:]]

    cands = [c for c in cands if len(parts(c)[1]) < len(line)]     # a break must shorten the line
    if not cands:
        return None
    depth = min(c[0] for c in cands)
    return parts(max((c for c in cands if c[0] == depth), key=lambda c: c[1]))


def wrap(line):
    out = [line]
    while len(out[-1]) > LIMIT:
        parts = split(out[-1])
        if parts is None or len(parts[1]) >= len(out[-1]):
            return None
        out[-1:] = parts
    return out


def main():
    args = sys.argv[1:]
    if '--from-lint' in args:
        rev = args[args.index('--diff') + 1] if '--diff' in args else 'main'
        res = subprocess.run([sys.executable, str(ROOT / 'tools/match/lint.py'), '--diff', rev],
                             capture_output=True, text=True, cwd=ROOT).stdout
        args = [m.group(1) for m in re.finditer(r'^(\S+?:\d+): long-line', res, re.M)]
        if not args:
            return
    if not args:
        sys.exit(__doc__)
    by_file = {}
    for a in args:
        f, n = a.rsplit(':', 1)
        by_file.setdefault(f, []).append(int(n))
    for f, lines in by_file.items():
        p = ROOT / 'src' / f if not (ROOT / f).exists() else ROOT / f
        text = p.read_bytes().decode('utf-8', 'surrogateescape')     # SJIS files round-trip
        eol = '\r\n' if '\r\n' in text else '\n'
        src = text.split(eol)
        for n in sorted(lines, reverse=True):
            l = src[n - 1]
            s = l.lstrip()
            if s.startswith('//') and ' ' in s[3:LIMIT - (len(l) - len(s))]:
                # a whole-line comment: move the words past the limit to a new comment line
                pre = l[:len(l) - len(s)] + '// '
                if s.startswith(('// port:', '//       ')):
                    pre += '      '                     # a port: note's lines stay under its text
                cut = l.rfind(' ', 0, 101)                    # docs/style.md: comments wrap at 100
                if cut > len(pre):
                    rest = l[cut + 1:]
                    nxt = src[n] if n < len(src) else ''
                    if nxt.startswith(pre) and not nxt[len(pre):].startswith((' ', '-', '*')):
                        # the paragraph goes on: the spilled words join its next line (which
                        # lint then reports again if it grew too long; name.py runs this 3 times)
                        src[n - 1:n + 1] = [l[:cut].rstrip(), pre + rest + ' ' + nxt[len(pre):]]
                    else:
                        src[n - 1:n] = [l[:cut].rstrip(), pre + rest]
                    continue
            if s.startswith(('//', '#', '/*', '*')) or re.search(r'\S\s*//', l):
                print(f'{f}:{n}: left alone (comment or preprocessor line, or a trailing comment)')
                continue
            w = wrap(l)
            if w is None:
                print(f'{f}:{n}: no break point found')
                continue
            src[n - 1:n] = w
        p.write_bytes(eol.join(src).encode('utf-8', 'surrogateescape'))


if __name__ == '__main__':
    main()
