"""Apply a batch of function names in one step, or nothing at all.
    python tools/match/name.py <batch.tsv> --by "<who>"      apply, build, report
    python tools/match/name.py <batch.tsv> --check           validate only, change nothing
batch.tsv, tab-separated, one function per line (# lines skipped):
    address  current_name  new_name  tier  codes  evidence  purpose  [comment]
  tier: T1 verified, T2 strong (EA's name from TW06/TW07/another EA build the code confirms),
        T3 read from the code (docs/style.md "Names"); codes as in config/GW4E69/name_sources.tsv
        (E1 EA text, E2 TW06/TW07 name, E3 wrapper, E4 named data, E5 named neighbours, E6 the code);
  evidence: what in the code or the reference shows it; purpose: one line, what the function does;
  comment (optional): the comment for a function that has none, written for a reader of the code
        (what it does in the game, not how; read from the code, never contradicting it). It goes
        above the definition, wrapped at 100 columns. A function that already has a comment keeps
        it: a wrong one is reported, not overwritten here.
Steps, in order; the first failure stops it and puts every touched file back:
  1. every row checked: EA style name (System_Verb: `Mem_set`, `RenderState_SetDepthFunc`), tier,
     codes, evidence and purpose present; then rename.py --dry-run (current name at that address,
     new name unused anywhere);
  2. rename.py: symbols.txt, src/, include/ (code and comments);
  3. the old names in the Markdown docs (not the dated records: docs/journal.md, agents/findings/,
     docs/reference-builds/);
  4. the comments of column 8 above their definitions;
  5. wraplong.py: lines the longer names pushed past lint's 110 columns are rewrapped;
  6. one row per name appended to config/GW4E69/name_sources.tsv;
  7. configure + ninja, and the DOL check must print main.dol: OK;
  8. report: call-site coverage before -> after (hotnames.py) and lint findings left on the lines
     the batch changed.
Needs a clean tree under src/, include/, config/ and docs/ (commit or stash first)."""
import datetime, pathlib, re, subprocess, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
PY = sys.executable
SOURCES = ROOT / 'config/GW4E69/name_sources.tsv'
NAME = re.compile(r'^[A-Z][A-Za-z0-9]*(_[A-Za-z0-9]+)+$')
CODES = re.compile(r'^E[1-6][a-z]?(\([^)]*\))?(\+E[1-6][a-z]?(\([^)]*\))?)*$')
PATHS = ['src', 'include', 'config', 'docs', 'agents']
SKIP_MD = ('docs/journal.md', 'docs/reference-builds/', 'agents/findings/')


def run(cmd, **kw):
    return subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True, **kw)


def coverage():
    m = re.search(r'= ([\d.]+)%', run([PY, 'tools/match/hotnames.py', '--top', '0']).stdout)
    return float(m.group(1)) if m else None


def load(path):
    rows, errors = [], []
    for n, l in enumerate(pathlib.Path(path).read_text(encoding='utf-8').splitlines(), 1):
        if not l.strip() or l.startswith('#'):
            continue
        c = l.split('\t')
        if len(c) not in (7, 8):
            errors.append(f'line {n}: {len(c)} columns, need 7 or 8 '
                          '(address current new tier codes evidence purpose [comment])')
            continue
        addr, cur, new, tier, codes, ev, purpose = (x.strip() for x in c[:7])
        comment = c[7].strip() if len(c) == 8 else ''
        if comment.startswith('//') or '*/' in comment:
            errors.append(f'line {n}: write the comment as plain text (name.py adds the //)')
        if not re.fullmatch(r'[0-9A-Fa-f]{8}', addr):
            errors.append(f'line {n}: address {addr!r} is not 8 hex digits')
        if not NAME.match(new):
            errors.append(f'line {n}: {new!r} is not EA style System_Verb (Mem_set, RenderState_SetDepthFunc)')
        if re.search(r'_[0-9A-Fa-f]{8}$|maybe|guess|unk|Unknown', new):
            errors.append(f'line {n}: {new!r} carries an address or a confidence marker; the tier says that')
        if tier not in ('T1', 'T2', 'T3'):
            errors.append(f'line {n}: tier {tier!r} is not T1, T2 or T3')
        if not CODES.match(codes):
            errors.append(f'line {n}: codes {codes!r} not like E6 or E2+E4')
        if len(ev) < 10:
            errors.append(f'line {n}: evidence is missing or too short')
        if len(purpose) < 5:
            errors.append(f'line {n}: purpose is missing')
        rows.append((addr.upper(), cur, new, tier, codes, ev, purpose, comment))
    return rows, errors


def restore(reason):
    changed = run(['git', 'diff', '--name-only', '--'] + PATHS).stdout.split()
    if changed:
        run(['git', 'checkout', '--'] + changed)
    sys.exit(f'name.py: {reason}\nnothing applied: {len(changed)} touched file(s) put back.')


def update_markdown(rows):
    names = {cur: new for _, cur, new, *_ in rows}
    rx = re.compile(r'\b(' + '|'.join(map(re.escape, names)) + r')\b')
    files = run(['git', 'ls-files', '*.md']).stdout.split()
    n = 0
    for f in files:
        if f.startswith(SKIP_MD):
            continue
        p = ROOT / f
        s = p.read_text(encoding='utf-8')
        t, k = rx.subn(lambda m: names[m.group(1)], s)
        if k:
            p.write_text(t, encoding='utf-8')
            n += k
    return n


def wrap_comment(text, indent=''):
    lines, cur = [], ''
    for w in text.split():
        if cur and len(indent) + 3 + len(cur) + 1 + len(w) > 100:
            lines.append(cur)
            cur = w
        else:
            cur = f'{cur} {w}' if cur else w
    return [f'{indent}// {l}' for l in lines + [cur]]


def add_comments(rows):
    """Insert column 8 above each definition that has no comment. Returns (added, skipped)."""
    added, skipped = 0, []
    want = {new: com for _, _, new, *rest in rows for com in [rest[-1]] if com}
    if not want:
        return 0, []
    files = [p for d in ('src',) for p in (ROOT / d).rglob('*.c')]
    found = set()
    for p in files:
        raw = p.read_bytes().decode('utf-8', 'surrogateescape')
        eol = '\r\n' if '\r\n' in raw else '\n'
        lines = raw.split(eol)
        changed = False
        for i in range(len(lines) - 1, -1, -1):
            l = lines[i]
            m = re.match(r'^(?:asm |static |inline )*[A-Za-z_][^;=(]*?\b([A-Za-z_]\w*)\s*\([^;]*$', l)
            if not m or m.group(1) not in want:
                continue
            j = i + 1
            while j < len(lines) and '{' not in lines[j - 1] and not lines[j - 1].rstrip().endswith(';'):
                j += 1
            if lines[j - 1].rstrip().endswith(';'):
                continue                                    # a prototype, not the definition
            name = m.group(1)
            found.add(name)
            top = i
            while top > 0 and lines[top - 1].startswith('#if'):
                top -= 1
            if top > 0 and lines[top - 1].lstrip().startswith(('//', '/*', '*')):
                skipped.append(name)
                continue
            lines[top:top] = wrap_comment(want[name])
            added += 1
            changed = True
        if changed:
            p.write_bytes(eol.join(lines).encode('utf-8', 'surrogateescape'))
    skipped += [f'{n} (definition not found)' for n in want if n not in found]
    return added, skipped


def main():
    args = sys.argv[1:]
    if not args or args[0].startswith('-'):
        sys.exit(__doc__)
    rows, errors = load(args[0])
    if not rows and not errors:
        errors.append('no rows')
    dup = {r[2] for r in rows if [x[2] for x in rows].count(r[2]) > 1}
    errors += [f'{d}: used twice in the batch' for d in dup]
    tmp = ROOT / 'build/name_batch.tsv'
    tmp.parent.mkdir(exist_ok=True)
    tmp.write_text(''.join(f'{r[0]}\t{r[1]}\t{r[2]}\n' for r in rows), encoding='utf-8')
    dry = run([PY, 'tools/match/rename.py', str(tmp), '--dry-run'])
    if dry.returncode:
        errors.append(dry.stdout.strip() + dry.stderr.strip())
    if errors:
        sys.exit('name.py: batch refused, nothing changed:\n  ' + '\n  '.join(errors))
    if '--check' in args:
        print(f'name.py: {len(rows)} name(s) pass the checks ({dry.stdout.strip()})')
        return
    if '--by' not in args:
        sys.exit('name.py: say who proposed the batch: --by "<lane or model>"')
    by = args[args.index('--by') + 1]
    if run(['git', 'status', '--porcelain', '--'] + PATHS).stdout.strip():
        sys.exit('name.py: src/, include/, config/, docs/ or agents/ has uncommitted changes; commit them first')

    before = coverage()
    r = run([PY, 'tools/match/rename.py', str(tmp)])
    if r.returncode:
        restore('rename.py failed:\n' + r.stdout + r.stderr)
    md = update_markdown(rows)
    ncom, kept = add_comments(rows)
    for _ in range(3):
        run([PY, 'tools/match/wraplong.py', '--from-lint', '--diff', 'HEAD'])
    today = datetime.date.today().isoformat()
    with open(SOURCES, 'a', encoding='utf-8') as f:
        for a, cur, new, tier, codes, ev, purpose, _ in rows:
            f.write('\t'.join([a, new, cur, tier, codes, ev, purpose, by, '', today]) + '\n')
    if run([PY, 'configure.py']).returncode:
        restore('configure.py failed')
    b = run(['ninja'])
    check = run(run(['ninja', '-t', 'commands', 'build/GW4E69/ok']).stdout.strip().splitlines()[-1],
                shell=True)
    if b.returncode or 'OK' not in check.stdout + check.stderr:
        tail = '\n'.join((b.stdout + b.stderr).splitlines()[-15:])
        restore('build or DOL check failed:\n' + tail + '\n' + check.stdout + check.stderr)
    after = coverage()
    lint = run([PY, 'tools/match/lint.py', '--diff', 'HEAD']).stdout
    left = [l for l in lint.splitlines() if re.match(r'^\S+:\d+: ', l)]
    print(f'name.py: {len(rows)} name(s) applied ({r.stdout.strip()}); {md} Markdown mention(s) updated')
    print(f'comments added: {ncom}' + (f'; kept the existing comment of: {", ".join(kept)}' if kept else ''))
    print('main.dol: OK')
    print(f'call sites to named functions: {before:.2f}% -> {after:.2f}%')
    if left:
        print(f'lint findings on the changed lines ({len(left)}; fix by hand, long comments mostly):')
        print('  ' + '\n  '.join(left[:20]))
    print('next: read `git diff`, then commit.')


if __name__ == '__main__':
    main()
