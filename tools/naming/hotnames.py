"""Which unnamed functions to name first: the ones called from the most places.
    python tools/naming/hotnames.py                 coverage + the top 60 unnamed functions
    python tools/naming/hotnames.py --top N         the top N
    python tools/naming/hotnames.py --unit NAME     only functions defined in that unit
    python tools/naming/hotnames.py --tsv           every unnamed function, tab-separated
    python tools/naming/hotnames.py --unit X --todo the file's functions not yet reviewed, in address order
                                                   (a file larger than one batch continues where the last stopped)
    python tools/naming/hotnames.py --units --log   ... and append the totals to
                                                   config/GW4E69/readability_progress.tsv (after every round)
    python tools/naming/hotnames.py --units         per source file: named, commented, done (the
                                                   naming lanes' territory map; worst files first)
A function is done when it has a real name and, if its body is longer than 3 lines, a comment
right above its definition (a short getter or setter reads from its name alone). "reviewed" counts
the functions a naming lane has read and logged (config/GW4E69/review.tsv, written by name.py):
the plan's measure that a comment was checked, not just present.
Coverage = share of all call sites (bl/b to a function, counted in the original's split objects)
whose target has a real name rather than fn_XXXXXXXX. Naming one function called from 300 places
makes 300 lines readable, so this is the number the naming work moves. For each unnamed function:
call sites, distinct callers, its unit, and n1's EA-name suggestion (docs/notes/
2026-09-27-name-pairing.tsv, confidence A/B/C) when there is one.
Run after a build (`ninja` makes the split objects)."""
import collections, pathlib, re, struct, sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from callgraph import ROOT, R_PPC_REL24, current_objects, sections  # noqa: E402

PLACEHOLDER = re.compile(r'^fn_[0-9A-Fa-f]{8}$')
PAIRS = ROOT / 'docs/notes/2026-09-27-name-pairing.tsv'


def call_sites(path):
    """(defined, sites): defined = function names in the object's .text; sites = Counter of call
    targets by name, one per REL24 relocation (every bl/b to a symbol)."""
    b = path.read_bytes()
    secs = sections(b)
    symtab = next(s for _, s in secs if s[1] == 2)
    strtab = secs[symtab[6]][1][4]
    syms = []
    for off in range(symtab[4], symtab[4] + symtab[5], 16):
        st_name, _, _, info, _, shndx = struct.unpack_from('>IIIBBH', b, off)
        syms.append((b[strtab + st_name:b.index(b'\0', strtab + st_name)].decode(), info & 0xF, shndx))
    text = {i for i, (n, _) in enumerate(secs) if n == '.text'}
    defined = {n for n, t, sh in syms if t == 2 and sh in text}
    sites = collections.Counter()
    for _, s in secs:
        if s[1] != 4 or s[7] not in text:
            continue
        for off in range(s[4], s[4] + s[5], 12):
            r_off, r_info, _ = struct.unpack_from('>IIi', b, off)
            if (r_info & 0xFF) == R_PPC_REL24:
                name = syms[r_info >> 8][0]
                if name:
                    sites[name] += 1
    return defined, sites


DEF = re.compile(r'^(?:asm |static |inline )*[A-Za-z_][^;=(]*?\b([A-Za-z_]\w*)\s*\([^;]*\{\s*$')


# The first line of a definition or prototype at column 0: a signature split over several lines, and
# a function returning a pointer to an array (`f32 (*Fn(Character* pChar))[4] {`), count too.
DEF_START = re.compile(r'^(?:asm |static |inline )*[A-Za-z_][\w \t*]*?(?:\(\s*\*\s*)?\b([A-Za-z_]\w*)\s*\(')


def find_defs(lines):
    """[(first line, name, line with the opening brace)] for each function definition in lines."""
    out = []
    for i, l in enumerate(lines):
        m = DEF_START.match(l)
        if not m or m.group(1) in ('if', 'while', 'for', 'switch', 'return', 'sizeof'):
            continue
        j = i
        while j < len(lines) and j < i + 8 and '{' not in lines[j] and ';' not in lines[j]:
            j += 1
        if j < len(lines) and '{' in lines[j] and ';' not in lines[j].split('{')[0]:
            out.append((i, m.group(1), j))
    return out


def comment_state(path):
    """name -> (body lines, has a comment above) for each function defined in a source file. With
    an asm version and an #else plain-C copy, the comment above either one counts."""
    lines = path.read_bytes().decode('utf-8', 'surrogateescape').split('\n')
    out = {}
    for i, name, k in find_defs(lines):
        j = k + 1
        while j < len(lines) and not lines[j].startswith('}'):
            j += 1
        t = i
        while t > 0 and lines[t - 1].startswith('#if'):
            t -= 1
        has = t > 0 and lines[t - 1].lstrip().startswith(('//', '/*', '*'))
        prev = out.get(name, (0, False))
        out[name] = (max(j - k - 1, prev[0]), has or prev[1])
    return out


def reviewed_names():
    """Names of the functions a naming lane has read (config/GW4E69/review.tsv, by address)."""
    log = ROOT / 'config/GW4E69/review.tsv'
    if not log.exists():
        return set()
    addrs = {l.split('\t')[0].upper() for l in log.read_text(encoding='utf-8').splitlines()
             if l and l[0] != '#' and not l.startswith('address')}
    names = set()
    for l in (ROOT / 'config/GW4E69/symbols.txt').read_text(encoding='utf-8').splitlines():
        m = re.match(r'^(\S+) = \.text:0x([0-9A-Fa-f]+);', l)
        # a function still called fn_XXXXXXXX is not through the pass, whatever review.tsv says
        if m and m.group(2).upper() in addrs and not PLACEHOLDER.match(m.group(1)):
            names.add(m.group(1))
    return names


def units_report(unit_of, sites, log=False):
    seen = reviewed_names()
    by_unit = collections.defaultdict(list)
    for n, u in unit_of.items():
        by_unit[u].append(n)
    rows = []
    for u, fns in by_unit.items():
        src = ROOT / 'src' / (u + '.c')
        if not src.exists():
            continue
        state = comment_state(src)
        named = sum(1 for n in fns if not PLACEHOLDER.match(n))
        need = [n for n in fns if state.get(n, (0, False))[0] > 3]
        commented = sum(1 for n in need if state[n][1])
        done = sum(1 for n in fns if not PLACEHOLDER.match(n)
                   and (state.get(n, (0, False))[0] <= 3 or state[n][1]))
        calls = sum(sites[n] for n in fns if PLACEHOLDER.match(n))
        rev = sum(1 for n in fns if n in seen)
        rows.append((done / len(fns), u, len(fns), named, len(need), commented, done, calls, rev))
    tot = [sum(r[i] for r in rows) for i in (2, 3, 4, 5, 6, 8)]
    print(f'all files: {tot[0]} functions, named {tot[1]} ({100 * tot[1] / tot[0]:.1f}%), '
          f'commented {tot[3]}/{tot[2]} that need one ({100 * tot[3] / max(tot[2], 1):.1f}%), '
          f'done {tot[4]} ({100 * tot[4] / tot[0]:.1f}%), reviewed by a naming lane {tot[5]} '
          f'({100 * tot[5] / tot[0]:.1f}%)')
    passed = sorted(r[1] for r in rows if r[8] == r[2])
    print(f'files through the full pass (every function reviewed): {len(passed)} of {len(rows)}'
          + (': ' + ', '.join(passed) if passed else ''))
    if log:
        import datetime, subprocess
        sha = subprocess.run(['git', 'rev-parse', '--short', 'HEAD'], cwd=ROOT, capture_output=True,
                             text=True).stdout.strip()
        total = sum(sites.values())
        cov = 100 * sum(v for n, v in sites.items() if not PLACEHOLDER.match(n)) / total
        f = ROOT / 'config/GW4E69/readability_progress.tsv'
        new = not f.exists()
        with open(f, 'a', encoding='utf-8') as out:
            if new:
                out.write('# Readability pass progress (hotnames.py --units --log, after every round)\n'
                          'date\tcommit\tfunctions\tnamed\tcommented\tneed_comment\tdone\treviewed\t'
                          'files_passed\tfiles\tcall_coverage%\n')
            out.write('\t'.join(map(str, [datetime.datetime.now().strftime('%Y-%m-%d %H:%M'), sha,
                                            tot[0], tot[1], tot[3], tot[2], tot[4], tot[5], len(passed),
                                            len(rows), f'{cov:.2f}'])) + '\n')
        print(f'logged to {f.relative_to(ROOT)}')
    print(f'\n{"done%":>6} {"fns":>5} {"named":>5} {"cmt":>9} {"reviewed":>8} {"unnamed calls":>13}  file')
    for d, u, n, named, need, cmt, done, calls, rev in sorted(rows):
        print(f'{100 * d:6.1f} {n:5} {named:5} {cmt:4}/{need:<4} {rev:8} {calls:13}  {u}')


def main():
    args = sys.argv[1:]
    top = int(args[args.index('--top') + 1]) if '--top' in args else 60
    only = args[args.index('--unit') + 1] if '--unit' in args else None
    unit_of, sites, fan_in = {}, collections.Counter(), collections.Counter()
    for p in current_objects():
        unit = str(p.relative_to(ROOT / 'build/GW4E69/obj')).removesuffix('.o')
        defined, s = call_sites(p)
        for n in defined:
            unit_of[n] = unit
        sites.update(s)
        for n in s:
            fan_in[n] += 1                      # number of units that call it
    pairs = {}
    if PAIRS.exists():
        for l in PAIRS.read_text(encoding='utf-8').splitlines()[1:]:
            c = l.split('\t')
            if len(c) > 11 and c[3]:
                pairs[c[2]] = '%s (%s)' % (c[3], c[10])
    total = sum(sites.values())
    named = sum(v for n, v in sites.items() if not PLACEHOLDER.match(n))
    unnamed_fns = [n for n in unit_of if PLACEHOLDER.match(n)]
    rows = sorted(((sites[n], fan_in[n], n) for n in unnamed_fns
                   if only is None or unit_of[n] == only), reverse=True)
    if '--units' in args:
        units_report(unit_of, sites, log='--log' in args)
        return
    if '--todo' in args:
        if not only:
            sys.exit('--todo needs --unit NAME')
        seen = reviewed_names()
        addr = {}
        for l in (ROOT / 'config/GW4E69/symbols.txt').read_text(encoding='utf-8').splitlines():
            m = re.match(r'^(\S+) = \.text:0x([0-9A-Fa-f]+);', l)
            if m:
                addr[m.group(1)] = int(m.group(2), 16)
        state = comment_state(ROOT / 'src' / (only + '.c')) if (ROOT / 'src' / (only + '.c')).exists() else {}
        fns = sorted((addr.get(n, 0), n) for n, u in unit_of.items() if u == only)
        todo = [(a, n) for a, n in fns if n not in seen]
        print(f'{only}: {len(fns)} functions, {len(fns) - len(todo)} reviewed, {len(todo)} to go '
              f'(address order; take them from the top)')
        for a, n in todo:
            lines, has = state.get(n, (0, False))
            print(f'  {a:08X}  {n:<40} {lines:4} lines  {"comment" if has else "no comment"}  '
                  f'{sites[n]} call sites')
        return
    if '--tsv' in args:
        print('calls\tunits_calling\tname\tunit\tn1_suggestion')
        for c, f, n in rows:
            print(f'{c}\t{f}\t{n}\t{unit_of[n]}\t{pairs.get(n, "")}')
        return
    print(f'call sites to named functions: {named}/{total} = {100 * named / total:.2f}%')
    print(f'functions still fn_: {len(unnamed_fns)}/{len(unit_of)}; '
          f'uncalled: {sum(1 for n in unnamed_fns if not sites[n])}')
    acc = named
    print(f'\n{"calls":>5} {"units":>5}  {"cum%":>6}  {"name":<14} {"unit":<32} n1 suggestion')
    for c, f, n in rows[:top]:
        acc += c
        print(f'{c:5} {f:5}  {100 * acc / total:6.2f}  {n:<14} {unit_of[n]:<32} {pairs.get(n, "")}')


if __name__ == '__main__':
    main()
