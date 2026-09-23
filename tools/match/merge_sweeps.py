"""Merge sweep files into one unit source: includes, then declarations, then bodies.
    python tools/match/merge_sweeps.py <Name>.c <sweep1.c> ...   (paths relative to src/)
A new unit's source is written from the sweeps. An existing source (a unit widened with
`mkunit.py --extend`) keeps every line it has: each sweep function goes where its address
(symbols.txt) puts it, before, between or after the unit's functions, in marked sweep blocks
(sweepblock.py; one per gap, or straight into an existing block when the gap is inside one), and
only the includes and declarations it lacks are added (the declarations at the top of the first
block). A unit whose own functions are not in address order stops the merge with nothing written."""
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


INCLUDE = re.compile(r'^#include\s+"([^"]+)"')


def included(lines):
    """Every header the lines include, directly or through other headers (include/ and the
    build's generated include/)."""
    seen, todo = set(), [m.group(1) for m in map(INCLUDE.match, lines) if m]
    while todo:
        h = todo.pop()
        if h in seen:
            continue
        seen.add(h)
        for d in (ROOT / 'include', ROOT / 'build/GW4E69/include'):
            f = d / h
            if f.is_file():
                todo += [m.group(1) for m in map(INCLUDE.match,
                         f.read_text(encoding='utf-8', errors='replace').split('\n')) if m]
                break
    return seen


def needed_includes(lines, includes):
    """The sweeps' includes the unit lacks. A header the unit already gets through its own
    includes is not added again, and neither is game_types.h when the unit has any include (every
    game header brings it)."""
    have = included(lines)
    has_any = any(INCLUDE.match(l) for l in lines)
    out = []
    for inc in includes:
        m = INCLUDE.match(inc)
        if inc in lines or (m and m.group(1) in have) \
                or (has_any and m and m.group(1) == 'game_types.h'):
            continue
        out.append(inc)
    return out


def body_addr(body, addr):
    return next((addr[n] for _, n in defined(body) if n in addr), None)


def by_address(bodies, addr):
    """[(address, body)] sorted by address; a body not in symbols.txt goes with the next one."""
    keyed, pending = [], []
    for b in bodies:
        a = body_addr(b, addr)
        pending.append(b)
        if a is not None:
            keyed += [(a, x) for x in pending]
            pending = []
    if not keyed:
        sys.exit('no sweep function found in symbols.txt')
    keyed += [(keyed[-1][0], x) for x in pending]
    return sorted(keyed, key=lambda x: x[0])         # stable: unknown bodies stay with theirs


def append(path, includes, decls, bodies):
    """Put each sweep function where its address says, between the unit's functions: bodies that
    fall in the same gap go together in one sweep block (or straight into an existing block when
    the gap is inside one); the new declarations go at the top of the first such block."""
    text = path.read_text(encoding='utf-8')
    lines = text.rstrip('\n').split('\n')
    addr = addresses()
    mine = [(i, addr[n]) for i, n in defined(text) if n in addr]
    if [a for _, a in mine] != sorted(a for _, a in mine):
        sys.exit('%s\'s functions are not in address order: merge by hand' % path.name)
    missing = needed_includes(lines, includes)
    keyed = by_address(bodies, addr)
    have = {a for _, a in mine}
    clash = [hex(a) for a, _ in keyed if a in have]
    if clash:
        sys.exit('%s already defines the functions at %s' % (path.name, clash))
    decls = [d for d in decls if d not in lines]
    inside = sweepblock.lines_in_blocks(lines)       # 1-based

    def slot_line(k):
        """0-based line to insert at for slot k (before the unit's k-th function)."""
        if k == len(mine):
            return len(lines)
        at = mine[k][0]                              # before the function and its comment
        while at > 0 and lines[at - 1].startswith('//') and lines[at - 1].strip() != sweepblock.END:
            at -= 1
        return at

    groups = {}                                      # insert line -> bodies
    for a, b in keyed:
        k = sum(1 for _, x in mine if x < a)
        groups.setdefault(slot_line(k), []).append(b)
    first = min(groups)
    for at in sorted(groups, reverse=True):          # bottom up, so line numbers stay valid
        code = '\n\n'.join(groups[at]).split('\n')
        if at == first and decls:
            code = decls + [''] + code
        in_block = (0 < at < len(lines) and at in inside and (at + 1) in inside   # between two
                    and lines[at - 1].strip() != sweepblock.END              # lines of one block
                    and lines[at].strip() != sweepblock.BEGIN)
        if in_block:
            chunk = code + ['']
        elif at == len(lines):
            chunk = [''] + [sweepblock.BEGIN, ''] + code + ['', sweepblock.END]
        else:
            chunk = [sweepblock.BEGIN, ''] + code + ['', sweepblock.END, '']
        lines = lines[:at] + chunk + lines[at:]
    if missing:
        last = max((i for i, l in enumerate(lines) if l.startswith('#include')), default=0)
        lines = lines[:last + 1] + missing + lines[last + 1:]
    out = '\n'.join(lines) + '\n'
    for b in bodies:                                 # every body verbatim, every old line kept
        assert b in out
    assert all(l in out for l in text.split('\n'))
    order = [addr[n] for _, n in defined(out) if n in addr]
    assert order == sorted(order), 'functions out of address order after the merge'
    path.write_text(out, encoding='utf-8', newline='\n')
    print('merged into %s: %d includes, %d decls, %d bodies in %d place(s)'
          % (path.name, len(missing), len(decls), len(bodies), len(groups)))


def main():
    out_name, files = sys.argv[1], sys.argv[2:]
    includes, decls, bodies = parse(files)
    path = ROOT / 'src' / out_name
    if path.exists() and path.read_text(encoding='utf-8').strip():
        append(path, includes, decls, bodies)
        return
    bodies = [b for _, b in by_address(bodies, addresses())]
    text ='\n'.join(includes) + '\n\n' + '\n'.join(decls) + '\n\n' + '\n\n'.join(bodies) + '\n'
    path.write_text(text, encoding='utf-8', newline='\n')
    print(len(decls), 'decls', len(bodies), 'bodies')


if __name__ == '__main__':
    main()
