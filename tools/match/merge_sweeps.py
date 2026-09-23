"""Merge sweep files into one unit source: includes, then declarations, then bodies.
    python tools/match/merge_sweeps.py <Name>.c <sweep1.c> ...   (paths relative to src/)
A new unit's source is written from the sweeps. An existing source (a unit widened with
`mkunit.py --extend`) keeps every line it has: the sweeps' code is added in a marked sweep block
(sweepblock.py) before its first function or after its last one, whichever keeps the functions in
address order, and only the includes and declarations it lacks are added. Sweeps that would land
between its functions stop the merge with nothing written."""
import re, sys, pathlib
ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
sys.path.insert(0, str(ROOT / 'tools/match'))
import sweepblock                                    # noqa: E402

DEF = re.compile(r'^[A-Za-z_][\w \*]*?\b(\w+)\s*\([^;]*$')     # a function header at column 0


def parse(files):
    decls, bodies, includes = [], [], []
    for f in files:
        s = (ROOT / 'src' / f).read_text(encoding='utf-8')
        cur = []
        depth = 0
        for ln in s.split('\n'):
            if depth == 0:
                if ln.startswith('//') or ln.strip() == '':
                    if cur: cur.append(ln)
                    continue
                if ln.startswith('#include'):
                    if ln not in includes: includes.append(ln)
                    continue
                if ln.rstrip().endswith(';') and '{' not in ln:
                    if ln not in decls: decls.append(ln)
                    continue
            cur.append(ln)
            depth += ln.count('{') - ln.count('}')
            if depth == 0 and ln.startswith('}'):
                bodies.append('\n'.join(cur).strip('\n'))
                cur = []
        assert depth == 0 and not ''.join(cur).strip(), f
    return includes, decls, bodies


def addresses():
    out = {}
    for l in (ROOT / 'config/GW4E69/symbols.txt').read_text(encoding='utf-8').splitlines():
        m = re.match(r'^(\w+) = \.text:0x([0-9A-Fa-f]+);', l)
        if m:
            out.setdefault(m.group(1), int(m.group(2), 16))
    return out


def defined(text):
    """Names of the functions defined in text (header at column 0, body brace on it or below)."""
    lines, out = text.split('\n'), []
    for i, l in enumerate(lines):
        m = DEF.match(l)
        if m and not l.startswith(('typedef', 'return', 'extern', 'static inline')):
            rest = ' '.join(x.strip() for x in lines[i:i + 4])
            if re.match(r'^[^;{]*\)\s*\{', rest):
                out.append((i, m.group(1)))
    return out


def append(path, includes, decls, bodies):
    text = path.read_text(encoding='utf-8')
    lines = text.rstrip('\n').split('\n')
    addr = addresses()
    mine = [(i, addr[n]) for i, n in defined(text) if n in addr]
    new = [addr[n] for b in bodies for _, n in defined(b) if n in addr]
    if not new:
        sys.exit('no sweep function found in symbols.txt')
    missing = [inc for inc in includes if inc not in lines]
    decls = [d for d in decls if d not in lines]
    block = [sweepblock.BEGIN, ''] + decls + [''] + '\n\n'.join(bodies).split('\n') + ['', sweepblock.END]
    if not mine or min(new) > max(a for _, a in mine):
        lines = lines + [''] + block                 # after the last function
    elif max(new) < min(a for _, a in mine):
        at = min(i for i, _ in mine)                 # before the first function and its comment
        while at > 0 and lines[at - 1].startswith('//'):
            at -= 1
        lines = lines[:at] + block + [''] + lines[at:]
    else:
        sys.exit('the sweeps fall between %s\'s functions: merge them by hand' % path.name)
    if missing:
        last = max((i for i, l in enumerate(lines) if l.startswith('#include')), default=0)
        lines = lines[:last + 1] + missing + lines[last + 1:]
    out = '\n'.join(lines) + '\n'
    for b in bodies:                                 # every body verbatim, every old line kept
        assert b in out
    assert all(l in out for l in text.split('\n'))
    path.write_text(out, encoding='utf-8', newline='\n')
    print('appended to %s: %d includes, %d decls, %d bodies' % (path.name, len(missing), len(decls),
                                                              len(bodies)))


def main():
    out_name, files = sys.argv[1], sys.argv[2:]
    includes, decls, bodies = parse(files)
    path = ROOT / 'src' / out_name
    if path.exists() and path.read_text(encoding='utf-8').strip():
        append(path, includes, decls, bodies)
        return
    text = '\n'.join(includes) + '\n\n' + '\n'.join(decls) + '\n\n' + '\n\n'.join(bodies) + '\n'
    path.write_text(text, encoding='utf-8', newline='\n')
    print(len(decls), 'decls', len(bodies), 'bodies')


if __name__ == '__main__':
    main()
