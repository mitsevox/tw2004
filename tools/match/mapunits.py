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
change (docs/workflow.md, "Creating units from the file map").

The merged file can fail to compile where two sweeps saw a function differently: a call with fewer
arguments than the definition takes (m2c missed an argument passed through in r3/r4), or another
return type (the notes list these). Fix the call to pass what the function takes, with a cast where
the types differ; the code stays the same, which the unchanged scores confirm."""
import json, pathlib, re, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
sys.path.insert(0, str(ROOT / 'tools/match'))
import sweepblock                                    # noqa: E402

SPLITS = ROOT / 'config/GW4E69/splits.txt'
CONFIGURE = ROOT / 'configure.py'
MAP = ROOT / 'config/GW4E69/filemap.json'
NOTES = []                                           # what to look at if a new unit does not compile
SWEEP_CODE_OF = {}                                   # sweep -> its function bodies (merge_sweeps)
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


TYPE_WORDS = {'void', 'char', 'short', 'int', 'long', 'float', 'double', 'signed', 'unsigned', 'const', 'volatile',
              's8', 'u8', 's16', 'u16', 's32', 'u32', 's64', 'u64', 'f32', 'f64', 'BOOL', 'struct'}
NARROW = re.compile(r'\b(f32|f64|float|double|s8|u8|s16|u16|char|short)\b(?!\s*\*)')


def param_types(args):
    """A parameter list without the parameter names: 's32 a, u8* p' -> ('s32', 'u8*')."""
    args = args.strip()
    if args in ('', 'void'):
        return () if args == 'void' else None
    out = []
    for p in args.split(','):
        p = re.sub(r'\s+', ' ', p.strip())
        m = re.match(r'^(.*?[\s\*])(\w+)$', p)
        if m and m.group(2) not in TYPE_WORDS:
            p = m.group(1)
        out.append(re.sub(r'\s*\*', '*', p).strip())
    return tuple(out)


def choose_prototype(nm, versions, code, owner):
    """One prototype for a function the sweeps declared differently. The untyped `()` ones give way
    to a typed one; when the typed ones disagree, the defining sweep's own wins (its callers may then
    need a cast or the argument they left out); a function defined elsewhere whose versions take only
    ints and pointers is declared `ret name();`, which leaves every call as it was compiled."""
    typed = [(d, t) for d, t in versions if t is not None]
    rets = {re.sub(r'\s+', ' ', d[:d.index(nm + '(')].replace('extern', '')).strip() for d, _ in versions}
    if len(rets) > 1:
        NOTES.append('%s returns %s in different sweeps' % (nm, ' / '.join(sorted(rets))))
    if not typed:
        return versions[0][0]
    if len({t for _, t in typed}) == 1:
        return typed[0][0]
    defining = [d for d, t in typed
                if re.search(r'^[A-Za-z_][\w \*]*\b%s\([^;]*\)\s*\{' % nm, code, re.M) and
                re.search(r'^[A-Za-z_][\w \*]*\b%s\([^;]*\)\s*\{' % nm, SWEEP_CODE_OF[owner[d]], re.M)]
    if defining:
        NOTES.append("%s: sweeps disagree on its parameters; kept its definition's" % nm)
        return defining[0]
    if not any(NARROW.search(' '.join(t)) for _, t in typed):
        d = typed[0][0]
        NOTES.append('%s: sweeps disagree on its parameters; declared without them' % nm)
        return d[:d.index(nm + '(')] + nm + '();'
    raise ValueError('%s is declared with two parameter lists' % nm)


def merge_sweeps(sweeps):
    """Includes, declarations (merged) and bodies of the sweep sources, in address order."""
    includes, decls, bodies, owner, sweep_code = [], [], [], {}, {}
    for s in sweeps:
        cur, depth, first_body = [], 0, len(bodies)
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
                        owner[ln] = s
                    continue
            cur.append(ln)
            depth += ln.count('{') - ln.count('}')
            if depth == 0 and ln.startswith('}'):
                bodies.append('\n'.join(cur).strip('\n'))
                cur = []
        if depth != 0 or ''.join(cur).strip():
            raise ValueError('cannot parse ' + s)
        sweep_code[s] = '\n'.join(bodies[first_body:])
    SWEEP_CODE_OF.clear()
    SWEEP_CODE_OF.update(sweep_code)
    protos = {}                                        # function -> [(declaration, parameter types)]
    for d in decls:
        m = re.match(r'^(?:extern\s+)?[A-Za-z_][\w \*]*?\b(\w+)\((.*)\);$', d)
        if m:
            protos.setdefault(m.group(1), []).append((d, param_types(m.group(2))))
    code = '\n'.join(bodies)
    keep, done, data = [], set(), {}
    for d in decls:
        m = re.match(r'^(?:extern\s+)?[A-Za-z_][\w \*]*?\b(\w+)\((.*)\);$', d)
        if m:                                          # a function: one prototype for the unit
            nm = m.group(1)
            if nm in done:
                continue
            done.add(nm)
            keep.append(choose_prototype(nm, protos[nm], code, owner))
            continue
        else:
            m = re.match(r'^extern\s+(.*?)\b(\w+)\s*(\[[^\]]*\])?\s*;$', d)
            if m:                                      # data: one type per name
                nm = m.group(2)
                used = lambda x: re.search(r'\b%s\b' % nm, sweep_code[owner[x]]) is not None
                if nm in data:                         # a clash: a declaration its sweep never uses goes
                    i, old = data[nm]
                    if not used(d):
                        continue
                    if not used(old):
                        keep[i] = d
                        data[nm] = (i, d)
                        continue
                    raise ValueError('%s is declared as %r and %r' % (nm, old, d))
                data[nm] = (len(keep), d)
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
            for n in NOTES:
                print('  note: ' + n)
            del NOTES[:]
            continue
        print('%s: 0x%08X-0x%08X, %d sweeps folded (%d declarations, %d functions)' % (
            f['name'], lo, hi, len(sweeps), nd, nb))
        for n in NOTES:
            print('  note: ' + n)
        del NOTES[:]
    check()


if __name__ == '__main__':
    main()
