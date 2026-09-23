"""Create source units from the file map (config/GW4E69/filemap.json, see docs/sourcefiles.md).

    python tools/match/mapunits.py --dry-run [--exclude A.c,B.c]   list what it would do
    python tools/match/mapunits.py [--only File.c] [--exclude ...]  create the units
    python tools/match/mapunits.py --check                          verify the units already made

A file qualifies when its name is strong, it has a certain core (text.core), no unit other than
sweeps overlaps the core, and no sweep crosses the core's edge. Each unit covers exactly the core:
a .text range in splits.txt, Object(NonMatching, ...) in configure.py (among the named units, before
the first one that starts after it), and src/<File>.c with its header line. The sweep units inside
the core are folded in unchanged, inside the sweep block (sweepblock.py); identical declarations are
merged and a function declared twice keeps its typed prototype. A declaration that clashes (the same
name with two different types) stops the file. Sweep files are removed only after their code is
found verbatim in the new source. Then rebuild: main.dol must stay OK and no function's score may
change (docs/workflow.md, "Creating units from the file map")."""
import json, pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
sys.path.insert(0, str(ROOT / 'tools/match'))
import sweepblock                                    # noqa: E402

SPLITS = ROOT / 'config/GW4E69/splits.txt'
CONFIGURE = ROOT / 'configure.py'
MAP = ROOT / 'config/GW4E69/filemap.json'
UNIT_HEAD = re.compile(r'\n(?=\S[^\n]*:\n)')


def read(p):
    return p.read_bytes().decode('utf-8')


def write(p, text):
    p.write_bytes(text.encode('utf-8'))


def split_blocks(sp):
    """splits.txt as blocks: [(unit or None, text range or None, block text)]."""
    out = []
    for b in UNIT_HEAD.split(sp):
        m = re.match(r'(\S[^\n]*):\n', b)
        t = re.search(r'\.text\s+start:0x([0-9A-Fa-f]+) end:0x([0-9A-Fa-f]+)', b)
        rng = (int(t.group(1), 16), int(t.group(2), 16)) if t else None
        out.append((m.group(1) if m else None, rng, b))
    return out


def is_sweep(unit):
    return re.search(r'(^|/)sweep_[0-9A-F]{8}\.c$', unit) is not None


def header_text(f, has_sweeps):
    ev = [e for e in f['name_evidence']]
    what = "EA's name, from its asserts" if 'assert' in ev else "strong name (%s)" % ', '.join(ev)
    if '2002 tree' in ev:
        what += "; also in EA's 2002 source tree"
    if 'TW06' in ev:
        what += '; TW06'
    body = ('not yet decompiled; the sweep code below is the matched small functions.' if has_sweeps
            else 'not yet decompiled.')
    words, lines, cur = ('// %s (%s): %s' % (f['name'], what, body)).split(' '), [], ''
    for w in words:
        if cur and len(cur) + 1 + len(w) > 100:
            lines.append(cur)
            cur = '// ' + w
        else:
            cur = cur + ' ' + w if cur else w
    return '\n'.join(lines + [cur]) + '\n'


def candidates(blocks, exclude):
    """(file, core lo, core hi, sweeps inside) for every qualifying file; and the reasons others fail."""
    units = [(u, r) for u, r, _ in blocks if u and r]
    names = {u for u, _ in units}
    good, skipped = [], []
    for f in json.loads(read(MAP))['files']:
        t = f.get('text') or {}
        if f.get('region') != 'EA' or f.get('name_confidence') != 'strong' or not t.get('core'):
            continue
        name = f['name']
        lo, hi = int(t['core'][0], 16), int(t['core'][1], 16)
        if name in names:
            continue                                   # already a unit
        if name in exclude:
            skipped.append((name, 'excluded'))
            continue
        if lo >= hi:
            skipped.append((name, 'empty core'))
            continue
        named = [u for u, (a, b) in units if not is_sweep(u) and a < hi and b > lo]
        cross = [u for u, (a, b) in units if is_sweep(u) and a < hi and b > lo and not (lo <= a and b <= hi)]
        if named:
            skipped.append((name, 'overlaps ' + ', '.join(named)))
            continue
        if cross:
            skipped.append((name, 'sweep crosses the core edge: ' + ', '.join(cross)))
            continue
        inside = sorted((u for u, (a, b) in units if is_sweep(u) and lo <= a and b <= hi),
                        key=lambda u: dict(units)[u][0])
        good.append((f, lo, hi, inside))
    return good, skipped


def merge_sweeps(sweeps):
    """Includes, declarations (merged) and bodies of the sweep sources, in address order."""
    includes, decls, bodies = [], [], []
    for s in sweeps:
        cur, depth = [], 0
        for ln in read(ROOT / 'src' / s).split('\n'):
            if depth == 0:
                if ln.startswith('//') or ln.strip() == '':
                    if cur:
                        cur.append(ln)
                    continue
                if ln.startswith('#include'):
                    if ln not in includes:
                        includes.append(ln)
                    continue
                if ln.rstrip().endswith(';') and '{' not in ln:
                    if ln not in decls:
                        decls.append(ln)
                    continue
            cur.append(ln)
            depth += ln.count('{') - ln.count('}')
            if depth == 0 and ln.startswith('}'):
                bodies.append('\n'.join(cur).strip('\n'))
                cur = []
        if depth != 0 or ''.join(cur).strip():
            raise ValueError('cannot parse ' + s)
    keep, funcs, data = [], {}, {}
    for d in decls:
        m = re.match(r'^(?:extern\s+)?[A-Za-z_][\w \*]*?\b(\w+)\((.*)\);$', d)
        if m:                                          # a function: keep the first typed prototype
            nm, args = m.group(1), m.group(2).strip()
            if nm in funcs:
                i, old = funcs[nm]
                if old == '' and args != '':
                    keep[i] = d
                    funcs[nm] = (i, args)
                elif args not in ('', old):
                    raise ValueError('%s is declared with two parameter lists' % nm)
                continue
            funcs[nm] = (len(keep), args)
        else:
            m = re.match(r'^extern\s+(.*?)\b(\w+)\s*(\[[^\]]*\])?\s*;$', d)
            if m:                                      # data: one type per name
                if m.group(2) in data:
                    raise ValueError('%s is declared as %r and %r' % (m.group(2), data[m.group(2)], d))
                data[m.group(2)] = d
        keep.append(d)
    return includes, keep, bodies


def make_unit(blocks, f, lo, hi, sweeps):
    name = f['name']
    includes, decls, bodies = merge_sweeps(sweeps)
    text = header_text(f, bool(sweeps))
    if sweeps:
        text += ('\n' + '\n'.join(includes) + '\n\n' + sweepblock.BEGIN + '\n\n' + '\n'.join(decls) + '\n\n'
                 + '\n\n'.join(bodies) + '\n\n' + sweepblock.END + '\n')
    for b in bodies:                                   # every body verbatim in the new source
        assert b in text
    # splits.txt: the unit's block where the first absorbed sweep (or the next unit) was
    out, inserted = [], False
    new_block = '%s:\n\t.text       start:0x%08X end:0x%08X\n' % (name, lo, hi)
    for u, r, b in blocks:
        if u in sweeps:
            if not inserted:
                out.append(new_block)
                inserted = True
            continue
        if not inserted and u and r and r[0] >= hi:
            out.append(new_block)
            inserted = True
        out.append(b)
    assert inserted
    # configure.py: the sweeps' lines go; the unit goes among the named units of the "unsorted" lib
    starts = {u: r[0] for u, r, _ in blocks if u and r}
    cp = read(CONFIGURE)
    for s in sweeps:
        cp, n = re.subn(r'\n[ \t]*Object\(\w+, "%s"\),' % re.escape(s), '', cp)
        assert n == 1, s
    lines = cp.split('\n')
    lib = [i for i, l in enumerate(lines) if '"lib": "unsorted"' in l][0]
    first = [i for i in range(lib, len(lines)) if 'Object(' in lines[i]][0]
    named, i = [], first
    while True:
        m = re.search(r'Object\(\w+, "([^"]+)"\)', lines[i])
        if not m or is_sweep(m.group(1)):
            break
        named.append((i, m.group(1)))
        i += 1
    pos = named[-1][0] + 1
    for k, u in named:
        if starts.get(u, 0) > lo:
            pos = k
            break
    indent = re.match(r'[ \t]*', lines[first]).group(0)
    lines.insert(pos, '%sObject(NonMatching, "%s"),' % (indent, name))
    write(ROOT / 'src' / name, text)
    write(SPLITS, '\n'.join(out))
    write(CONFIGURE, '\n'.join(lines))
    for s in sweeps:
        (ROOT / 'src' / s).unlink()
    return len(decls), len(bodies)


def check():
    """The units made from the map: one splits block over the core, one configure line, no sweep left
    inside, each sweep function defined once in src/."""
    sp, cp = read(SPLITS), read(CONFIGURE)
    blocks = split_blocks(sp)
    units = [(u, r) for u, r, _ in blocks if u and r]
    problems, n = [], 0
    defs = {}
    for p in (ROOT / 'src').rglob('*.c'):
        for m in re.finditer(r'^\w[\w \*]*\b(fn_[0-9A-F]{8})\([^;{]*\)\s*\{', read(p), re.M):
            defs.setdefault(m.group(1), []).append(str(p.relative_to(ROOT)))
    for f in json.loads(read(MAP))['files']:
        t = f.get('text') or {}
        mine = [r for u, r in units if u == f['name']]
        if not mine or not t.get('core'):
            continue
        n += 1
        lo, hi = int(t['core'][0], 16), int(t['core'][1], 16)
        if len(mine) != 1:
            problems.append('%s: %d splits blocks' % (f['name'], len(mine)))
        if cp.count('"%s"' % f['name']) != 1:
            problems.append('%s: %d configure.py lines' % (f['name'], cp.count('"%s"' % f['name'])))
        left = [u for u, (a, b) in units if is_sweep(u) and a < hi and b > lo]
        if left:
            problems.append('%s: sweeps still inside: %s' % (f['name'], ', '.join(left)))
    for u, _ in units:                                 # configure.py and splits.txt agree on sweeps
        if is_sweep(u) and cp.count('"%s"' % u) != 1:
            problems.append('%s: in splits.txt, %d configure.py lines' % (u, cp.count('"%s"' % u)))
        if is_sweep(u) and not (ROOT / 'src' / u).exists():
            problems.append('%s: in splits.txt, no source' % u)
    split_units = {u for u, _ in units}
    for m in re.finditer(r'Object\(\w+, "([^"]+)"\)', cp):
        if is_sweep(m.group(1)) and m.group(1) not in split_units:
            problems.append('%s: in configure.py, not in splits.txt' % m.group(1))
    for p in (ROOT / 'src/unsorted').rglob('sweep_*.c'):
        if str(p.relative_to(ROOT / 'src')).replace('\\', '/') not in split_units:
            problems.append('%s: source left with no unit' % p.relative_to(ROOT))
    for fn, where in defs.items():
        if len(where) > 1:
            problems.append('%s defined in %s' % (fn, ', '.join(where)))
    print('%d units from the map checked, %d problems' % (n, len(problems)))
    for p in problems:
        print('  ' + p)
    return not problems


def main():
    args = sys.argv[1:]
    if '--check' in args:
        sys.exit(0 if check() else 1)
    exclude = set()
    if '--exclude' in args:
        exclude = set(args[args.index('--exclude') + 1].split(','))
    only = args[args.index('--only') + 1] if '--only' in args else None
    blocks = split_blocks(read(SPLITS))
    good, skipped = candidates(blocks, exclude)
    if only:
        good = [g for g in good if g[0]['name'] == only]
        if not good:
            sys.exit('%s does not qualify: %s' % (only, dict(skipped).get(only, 'not in the map as strong, or '
                                                                           'already a unit')))
    if '--dry-run' in args:
        total = 0
        for f, lo, hi, sweeps in good:
            sweep_bytes = sum(r[1] - r[0] for u, r, _ in blocks if u in sweeps)
            total += hi - lo - sweep_bytes
            print('%-44s 0x%08X-0x%08X  asm %6d bytes  sweeps %3d  (%s)' % (
                f['name'], lo, hi, hi - lo - sweep_bytes, len(sweeps), ', '.join(f['name_evidence'])))
        print('%d files, %d bytes of asm-only code' % (len(good), total))
        for name, why in skipped:
            print('skipped %s: %s' % (name, why))
        return
    for f, lo, hi, sweeps in good:
        blocks = split_blocks(read(SPLITS))
        try:
            nd, nb = make_unit(blocks, f, lo, hi, sweeps)
        except (ValueError, AssertionError) as e:
            print('%s: not made: %s' % (f['name'], e))
            continue
        print('%s: 0x%08X-0x%08X, %d sweeps folded (%d declarations, %d functions)' % (
            f['name'], lo, hi, len(sweeps), nd, nb))
    check()


if __name__ == '__main__':
    main()
