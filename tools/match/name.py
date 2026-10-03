"""Apply a batch of function names in one step, or nothing at all.
    python tools/match/name.py <batch.tsv> --by "<who>"      apply, build, report
    python tools/match/name.py <batch.tsv> --check           validate only, change nothing
batch.tsv, tab-separated, one function per line (# lines skipped):
    address  current_name  new_name  tier  codes  evidence  purpose  [comment]
  tier: T1 verified, T2 strong (EA's name from TW06/TW07/another EA build the code confirms),
        T3 read from the code (docs/style.md "Names"); codes as in config/GW4E69/name_sources.tsv
        (E1 EA text, E2 TW06/TW07 name, E3 wrapper, E4 named data, E5 named neighbours, E6 the code);
  evidence: what in the code or the reference shows it; purpose: one line, what the function does;
  comment (optional): KEEP (read, the existing comment is right: logged as reviewed), NONE (read,
        a short function whose name says it all: logged as reviewed, needs no comment), or the function's whole comment, written for a reader of the code (what it
        does in the game, units, what 0/NULL mean; read from the code, never contradicting it).
        It goes above the definition, wrapped at 100 columns, REPLACING an existing // comment:
        rewrite a comment that is wrong, stale or vague. Keep every `fake match:`, `port:` and
        `EA bug:` label of the old comment (correct its text if stale): name.py refuses a row
        that drops one.
Steps, in order; the first failure stops it and puts every touched file back:
  1. every row checked: EA style name (System_Verb: `Mem_set`, `RenderState_SetDepthFunc`), tier,
     codes, evidence and purpose present; then rename.py --dry-run (current name at that address,
     new name unused anywhere);
  2. rename.py: symbols.txt, src/, include/ (code and comments);
  3. the old names in the Markdown docs (not the dated records: docs/journal.md, docs/notes/,
     docs/reference-builds/);
  4. the comments of column 8 above their definitions;
  5. wraplong.py: lines the longer names pushed past lint's 110 columns are rewrapped;
  6. one row per name appended to config/GW4E69/name_sources.tsv;
  7. configure + ninja, and the DOL check must print main.dol: OK;
  8. report: call-site coverage before -> after (hotnames.py) and lint findings left on the lines
     the batch changed.
A row whose new_name equals current_name is comment-only: column 8 is added (tier, codes and
evidence still say where the comment comes from), nothing is renamed and no name_sources row is
written. Use it for functions that already have a name; a fn_XXXXXXXX placeholder is refused:
every function read gets a name (an empty or stripped one after where it is called and what for).
Needs a clean tree under src/, include/, config/ and docs/ (commit or stash first)."""
import datetime, pathlib, re, subprocess, sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import hotnames                                              # noqa: E402  (find_defs)

ROOT = pathlib.Path(__file__).resolve().parents[2]
PY = sys.executable
SOURCES = ROOT / 'config/GW4E69/name_sources.tsv'
REVIEW = ROOT / 'config/GW4E69/review.tsv'
NAME = re.compile(r'^[A-Z][A-Za-z0-9]*(_[A-Za-z0-9]+)+$')
CODES = re.compile(r'^E[1-6][a-z]?(\([^)]*\))?(\+E[1-6][a-z]?(\([^)]*\))?)*$')
PATHS = ['src', 'include', 'config', 'docs']
SKIP_MD = ('docs/journal.md', 'docs/reference-builds/', 'docs/notes/')


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
        # EA's own name, spelled as EA did: from a related build (E2) or EA's text in this one (E1)
        ea_own = tier in ('T1', 'T2') and ('E1' in codes or 'E2' in codes)
        if cur == new and re.fullmatch(r'fn_[0-9A-Fa-f]{8}', cur):
            errors.append(f'line {n}: {cur} keeps its placeholder name: every function read gets a name '
                          '(an empty or stripped one is named from where it is called and what for)')
        elif cur == new:
            pass                                            # comment-only row: the name is not new
        elif not NAME.match(new) and not (ea_own and re.fullmatch(r'[A-Za-z_]\w*', new)):
            errors.append(f'line {n}: {new!r} is not EA style System_Verb (Mem_set, RenderState_SetDepthFunc);'
                          ' EA\'s own name (T1/T2 with E1 or E2) may be spelled as EA did')
        if cur != new and re.search(r'_[0-9A-Fa-f]{8}$|maybe|guess|unk|Unknown', new):
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
        if f == 'docs/tw06-names.md':
            # a record of TW06's names: only the "Now" column (the 2nd) follows our renames
            k = 0
            out = []
            for line in s.split('\n'):
                c = line.split('|')
                if len(c) > 3 and c[2].strip().startswith('`'):
                    c[2], j = rx.subn(lambda m: names[m.group(1)], c[2])
                    k += j
                    line = '|'.join(c)
                out.append(line)
            t = '\n'.join(out)
        else:
            t, k = rx.subn(lambda m: names[m.group(1)], s)
        if k:
            p.write_text(t, encoding='utf-8')
            n += k
    return n


def wrap_comment(text, indent=''):
    """// lines of at most 100 columns; each fake match: / port: / EA bug: label starts a line. A
    `port:` note's continuation lines are indented under its text ("//       "), so a porter sees
    where the note ends."""
    out = []
    for part in re.split(r'\s+(?=(?:fake match:|port:|EA bug:))', text.strip()):
        lines, cur = [], ''
        cont = '//       ' if part.startswith('port:') else '// '
        for w in part.split():
            pre = cont if lines else '// '
            if cur and len(indent) + len(pre) + len(cur) + 1 + len(w) > 100:
                lines.append(cur)
                cur = w
            else:
                cur = f'{cur} {w}' if cur else w
        lines.append(cur)
        out += [f'{indent}{"// " if k == 0 else cont}{l}' for k, l in enumerate(lines)]
    return out


LABELS = ('fake match:', 'port:', 'EA bug:')
REWRITTEN = set()


def add_comments(rows):
    """Column 8 becomes the comment above each definition: added where there is none, and REPLACING
    the existing // block where there is one (owner, 2026-09-27: naming lanes own the comments of
    the functions they touch). A replacement must keep every matching label of the old block
    (fake match:, port:, EA bug:; the text after it may be corrected). Only the first definition in
    a file is commented (an #else plain-C copy keeps its own note). Returns (added, replaced, errors)."""
    added, replaced, errors = 0, 0, []
    want = {new: com for _, _, new, *rest in rows for com in [rest[-1]] if com and com not in ('KEEP', 'NONE')}
    none = {new for _, _, new, *rest in rows if rest[-1] == 'NONE'}
    if not want and not none:
        return 0, 0, []
    found = set()
    for p in (ROOT / 'src').rglob('*.c'):
        raw = p.read_bytes().decode('utf-8', 'surrogateescape')
        eol = '\r\n' if '\r\n' in raw else '\n'
        lines = raw.split(eol)
        defs = []
        for i, name, k in hotnames.find_defs(lines):
            if name in none:
                j = k + 1
                while j < len(lines) and not lines[j].startswith('}'):
                    j += 1
                if j - k - 1 > 3:
                    errors.append(f'{name}: NONE, but its body is {j - k - 1} lines: every function over '
                                  '3 lines gets a comment (write one, or KEEP the one it has)')
                continue
            if name not in want or name in found:
                continue
            found.add(name)
            defs.append((i, name))
        for i, name in reversed(defs):
            top = i
            while top > 0 and lines[top - 1].startswith('#if'):
                top -= 1
            first = top
            while first > 0 and lines[first - 1].lstrip().startswith('//'):
                first -= 1
            if first > 0 and lines[first - 1].rstrip().endswith('*/'):
                errors.append(f'{name}: has a /* */ comment; edit it by hand')
                continue
            old = ' '.join(l.strip()[2:].strip() for l in lines[first:top])
            lost = [lab for lab in LABELS if old.count(lab) > want[name].count(lab)]
            if lost:
                errors.append(f'{name}: the new comment drops {", ".join(lost)} from the old one: '
                              f'keep the label (correct its text if it is stale). Old: {old[:200]}')
                continue
            lines[first:top] = wrap_comment(want[name])
            if first < top:
                replaced += 1
                REWRITTEN.add(name)
            else:
                added += 1
        if defs:
            p.write_bytes(eol.join(lines).encode('utf-8', 'surrogateescape'))
    errors += [f'{n}: definition not found' for n in want if n not in found]
    return added, replaced, errors


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
    renames = [r for r in rows if r[1] != r[2]]
    errors += [f'{r[2]}: comment-only row without a comment' for r in rows if r[1] == r[2] and not r[7]]
    tmp.write_text(''.join(f'{r[0]}\t{r[1]}\t{r[2]}\n' for r in renames), encoding='utf-8')
    dry = run([PY, 'tools/match/rename.py', str(tmp), '--dry-run']) if renames else None
    if dry is not None and dry.returncode:
        errors.append(dry.stdout.strip() + dry.stderr.strip())
    if errors:
        sys.exit('name.py: batch refused, nothing changed:\n  ' + '\n  '.join(errors))
    if '--check' in args:
        print(f'name.py: {len(rows)} row(s) pass the checks'
              + (f' ({dry.stdout.strip()})' if dry is not None else ' (comments only)'))
        return
    if '--by' not in args:
        sys.exit('name.py: say who proposed the batch: --by "<lane or model>"')
    by = args[args.index('--by') + 1]
    if run(['git', 'status', '--porcelain', '--'] + PATHS).stdout.strip():
        sys.exit('name.py: src/, include/, config/ or docs/ has uncommitted changes; commit them first')

    before = coverage()
    r = run([PY, 'tools/match/rename.py', str(tmp)]) if renames else None
    if r is not None and r.returncode:
        restore('rename.py failed:\n' + r.stdout + r.stderr)
    md = update_markdown(renames) if renames else 0
    ncom, nrep, cerr = add_comments(rows)
    rewritten = REWRITTEN
    if cerr:
        restore('comment rows refused:\n  ' + '\n  '.join(cerr))
    for _ in range(10):         # a reflowed comment spills into its next line: repeat until stable
        before_wrap = run(['git', 'diff', '--stat']).stdout + run(['git', 'diff']).stdout[-4000:]
        run([PY, 'tools/match/wraplong.py', '--from-lint', '--diff', 'HEAD'])
        if run(['git', 'diff', '--stat']).stdout + run(['git', 'diff']).stdout[-4000:] == before_wrap:
            break
    today = datetime.date.today().isoformat()
    with open(SOURCES, 'a', encoding='utf-8') as f:
        for a, cur, new, tier, codes, ev, purpose, _ in renames:
            f.write('\t'.join([a, new, cur, tier, codes, ev, purpose, by, '', today]) + '\n')
    if run([PY, 'configure.py']).returncode:
        restore('configure.py failed')
    b = run(['ninja'])
    check = run(run(['ninja', '-t', 'commands', 'build/GW4E69/ok']).stdout.strip().splitlines()[-1],
                shell=True)
    if b.returncode or 'OK' not in check.stdout + check.stderr:
        tail = '\n'.join((b.stdout + b.stderr).splitlines()[-15:])
        restore('build or DOL check failed:\n' + tail + '\n' + check.stdout + check.stderr)
    # the review log: every function this batch read, and what happened to its comment
    new_log = not REVIEW.exists()
    with open(REVIEW, 'a', encoding='utf-8') as f:
        if new_log:
            f.write('# Every function a naming lane has read (name.py): address, name, comment action\n'
                    '# (keep: read and right; rewrite; add; none: needs none), who, date.\n'
                    'address\tname\tcomment\tby\tdate\n')
        for a, cur, new, *_rest, com in rows:
            act = 'keep' if com == 'KEEP' else 'none' if com == 'NONE' else ('none' if not com else ('rewrite' if new in rewritten else 'add'))
            f.write('\t'.join([a, new, act, by, today]) + '\n')
    after = coverage()
    lint = run([PY, 'tools/match/lint.py', '--diff', 'HEAD']).stdout
    left = [l for l in lint.splitlines() if re.match(r'^\S+:\d+: ', l)]
    print(f'name.py: {len(renames)} name(s) applied' + (f' ({r.stdout.strip()})' if r else '')
          + f'; {md} Markdown mention(s) updated')
    print(f'comments added: {ncom}; replaced: {nrep}')
    print('main.dol: OK')
    print(f'call sites to named functions: {before:.2f}% -> {after:.2f}%')
    if left:
        print(f'lint findings on the changed lines ({len(left)}; fix by hand, long comments mostly):')
        print('  ' + '\n  '.join(left[:20]))
    print('next: read `git diff`, then commit.')


if __name__ == '__main__':
    main()
