"""A function's C as a blind auditor must see it (name and comment audit, docs/style.md "Names").
    python tools/match/blindview.py <function name or address> [--uses]
    python tools/match/blindview.py --type <struct/union/enum name>   a type's definition, comments removed
Prints the definition with: every comment removed; the function's own name and every game-code
function name that has not passed the audit (config/GW4E69/audit.tsv, tier T1 or T2) replaced by
its address (fn_XXXXXXXX). SDK and C library names stay: that code is out of the audit's scope.
Globals, types and struct fields keep their names: treat them as claims, not facts.
--uses also prints every line (with one line either side) that calls or mentions the function,
in any game file, blinded the same way.
Blind is the point: a reader who sees the current name or comment is primed by it."""
import collections, json, pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
AUDIT = ROOT / 'config/GW4E69/audit.tsv'
IDENT = re.compile(r'\b[A-Za-z_][A-Za-z0-9_]*\b')


def strip_comments(t):
    """Remove // and /* */ comments, keep strings and character constants, keep line breaks."""
    out, i, n = [], 0, len(t)
    while i < n:
        c = t[i]
        if c in '"\'':
            j = i + 1
            while j < n and t[j] != c:
                j += 2 if t[j] == '\\' else 1
            out.append(t[i:j + 1])
            i = j + 1
        elif t.startswith('//', i):
            j = t.find('\n', i)
            i = n if j < 0 else j
        elif t.startswith('/*', i):
            j = t.find('*/', i + 2)
            j = n if j < 0 else j + 2
            out.append('\n' * t.count('\n', i, j))
            i = j
        else:
            out.append(c)
            i += 1
    return ''.join(out)


def game_functions():
    """name -> address for every function in a game unit (report.json category 'game')."""
    r = json.loads((ROOT / 'build/GW4E69/report.json').read_text())
    out = {}
    for u in r['units']:
        if 'game' not in u.get('metadata', {}).get('progress_categories', []):
            continue
        for f in u.get('functions', []):
            a = f.get('metadata', {}).get('virtual_address')
            if a:
                out[f['name']] = int(a)
    return out


def audited():
    """names that passed the audit at T1/T2 (their current names)."""
    ok = set()
    if AUDIT.exists():
        cols = None
        for l in AUDIT.read_text(encoding='utf-8').splitlines():
            f = l.split('\t')
            if l.startswith('#') or not l.strip():
                continue
            if f[0] == 'address':
                cols = {c: i for i, c in enumerate(f)}
                continue
            if cols and f[cols['tier']] in ('T1', 'T2'):
                ok.add(f[cols['name_after']])
    return ok


class Blinder:
    def __init__(self):
        self.game = game_functions()
        self.ok = audited()

    def hide(self, name):
        a = self.game.get(name)
        return a is not None and name not in self.ok and not re.fullmatch(r'fn_[0-9A-F]{8}', name)

    def __call__(self, text, own=None):
        code = strip_comments(text)

        def sub(m):
            w = m.group(0)
            before = code[max(0, m.start() - 2):m.start()]
            if before.endswith('.') or before.endswith('->'):
                return w                              # a struct member that shares the spelling
            if w == own or self.hide(w):
                return 'fn_%08X' % self.game[w]
            return w
        return IDENT.sub(sub, code)


def game_sources():
    return [p for p in sorted((ROOT / 'src').rglob('*.c')) if 'dolphin' not in p.parts]


def find_definition(name):
    pat = re.compile(r'^[A-Za-z_][^;{}()]*\b%s\s*\(' % re.escape(name), re.M)
    for p in game_sources():
        t = p.read_text(encoding='utf-8', errors='replace')
        for m in pat.finditer(t):
            k = t.find('{', m.end())
            semi = t.find(';', m.end())
            if k < 0 or (0 <= semi < k):
                continue                                  # a prototype, not the definition
            depth, j = 0, k
            s = strip_comments(t[k:])
            for j, ch in enumerate(s):
                depth += (ch == '{') - (ch == '}')
                if depth == 0:
                    break
            return p, t[:m.start()].count('\n') + 1, t[m.start():m.start()] + s[:0] + strip_comments(t[m.start():k]) + s[:j + 1]
    return None, 0, None


def find_type(name):
    """A struct/union/enum definition (typedef'd or tagged) by name, from include/ and src/."""
    pats = [re.compile(r'typedef\s+(struct|union|enum)\s*\w*\s*\{'), re.compile(r'\b(struct|union|enum)\s+%s\s*\{' % re.escape(name))]
    for p in sorted((ROOT / 'include').rglob('*.h')) + game_sources():
        t = strip_comments(p.read_text(encoding='utf-8', errors='replace'))
        for pat in pats:
            for m in pat.finditer(t):
                k = t.index('{', m.start())
                depth = 0
                for j in range(k, len(t)):
                    depth += (t[j] == '{') - (t[j] == '}')
                    if depth == 0:
                        break
                tail = re.match(r'\s*(\w+)\s*;', t[j + 1:])
                if pat is pats[1] or (tail and tail.group(1) == name):
                    end = j + 1 + (tail.end() if tail else 0)
                    return p, t[m.start():end]
    return None, None


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    b = Blinder()
    arg = sys.argv[1]
    if arg == '--type':
        p, body = find_type(sys.argv[2])
        if body is None:
            sys.exit('no definition of type %s' % sys.argv[2])
        print('// %s (blind view)' % p.relative_to(ROOT).as_posix())
        print(re.sub(r'\n\s*\n+', '\n', b(body)))
        return
    if re.fullmatch(r'(0x)?[0-9A-Fa-f]{8}', arg):
        a = int(arg.removeprefix('0x'), 16)
        arg = next((n for n, x in b.game.items() if x == a), 'fn_%08X' % a)
    p, line, body = find_definition(arg)
    if body is None:
        sys.exit('no definition of %s in the game sources' % arg)
    print('// %s:%d (blind view)' % (p.relative_to(ROOT).as_posix(), line))
    print(re.sub(r'\n\s*\n+', '\n', b(body, own=arg)))
    if '--uses' in sys.argv:
        print('\n// uses:')
        for q in game_sources():
            lines = q.read_text(encoding='utf-8', errors='replace').split('\n')
            for i, l in enumerate(lines):
                code = strip_comments(l)
                if re.search(r'\b%s\b' % re.escape(arg), code) and not (q == p and i + 1 == line):
                    ctx = '\n'.join(lines[max(0, i - 1):i + 2])
                    print('// %s:%d\n%s' % (q.relative_to(ROOT).as_posix(), i + 1, b(ctx, own=arg).strip('\n')))


if __name__ == '__main__':
    main()
