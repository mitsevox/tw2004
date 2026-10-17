"""The audit table: which game source files have had their audit pass (docs/style.md, "The audit").
    python tools/quality/audit.py            progress: files done out of the total, per folder
    python tools/quality/audit.py --check    fail if the table and src/ disagree, a done file has a
                                             lint finding, or the plan and the table disagree (CI
                                             runs this after the build)
    python tools/quality/audit.py --sync     add new game files as open, drop rows of files that
                                             are gone (after a file is merged, split or moved)
docs/audit.tsv, tab-separated: file (under src/), state (open or done), commit (the one holding
the pass), reviewer (who passed it); commit and reviewer stay empty while open. The author of a
pass leaves the row open: the reviewer, who did not write it, marks it done. The SDK's own files
(src/dolphin/, and the SDK and runtime scraps in SDK_SCRAPS) are not listed.
docs/audit-plan.tsv, tab-separated: order, area, file, task, scope, state. Every task the audit
has left, in the order they run: a 'pass' over a whole file; for a big file, 'chunk i/n' passes
over groups of its functions, then a 'whole-file review' that alone takes the file to done; a
'fold' that proves which file an unsorted scrap belongs to and folds it in. Each task is one
writer and one reviewer, and the reviewer sets its state to done."""
import collections, pathlib, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools' / 'quality'))
import lint                                          # noqa: E402

TABLE = ROOT / 'docs' / 'audit.tsv'
HEAD = 'file\tstate\tcommit\treviewer'
PLAN = ROOT / 'docs' / 'audit-plan.tsv'
PLAN_HEAD = 'order\tarea\tfile\ttask\tscope\tstate'
SKIP = ('dolphin/',)
# Not EA code: debugger stubs, Metrowerks critical sections and MSL stubs in the SDK and runtime
# address range (src/README.md, "Unplaced"). They belong to the libraries in extern/.
SDK_SCRAPS = ('unsorted/sweep_801338E0.c', 'unsorted/sweep_8013B3C4.c', 'unsorted/sweep_8014CA80.c',
              'unsorted/sweep_80155F40.c', 'unsorted/sweep_801654D0.c', 'unsorted/sweep_801654F4.c',
              'unsorted/sweep_80165524.c')


def game_files():
    return sorted(f for f in (p.relative_to(ROOT / 'src').as_posix()
                              for p in (ROOT / 'src').rglob('*.c'))
                  if not f.startswith(SKIP) and f not in SDK_SCRAPS)


def load():
    rows, errors = {}, []
    for n, l in enumerate(TABLE.read_text(encoding='utf-8').splitlines(), 1):
        if not l.strip() or l.startswith('#') or l == HEAD:
            continue
        f = l.split('\t')
        if (len(f) != 4 or f[1] not in ('open', 'done')
                or (f[1] == 'done') != bool(f[2]) or (f[1] == 'done') != bool(f[3])):
            errors.append('docs/audit.tsv:%d: need file, open or done, and a done row\'s commit and '
                          'reviewer' % n)
            continue
        if f[0] in rows:
            errors.append('docs/audit.tsv:%d: %s is listed twice' % (n, f[0]))
        rows[f[0]] = (f[1], f[2], f[3])
    return rows, errors


def check_plan(rows):
    """The plan agrees with the table: every open file has its tasks, and a file is done exactly
    when its last task (its pass, whole-file review or fold) is done."""
    errors, tasks = [], collections.defaultdict(list)
    for n, l in enumerate(PLAN.read_text(encoding='utf-8').splitlines(), 1):
        if not l.strip() or l == PLAN_HEAD:
            continue
        f = l.split('\t')
        if (len(f) != 6 or not f[0].isdigit() or f[5] not in ('open', 'done')
                or not (f[3] in ('pass', 'whole-file review', 'fold') or f[3].startswith('chunk '))):
            errors.append('docs/audit-plan.tsv:%d: need order, area, file, task (pass, chunk i/n, '
                          'whole-file review or fold), scope, open or done' % n)
            continue
        tasks[f[2]].append((n, f[3], f[5]))
    for f, ts in sorted(tasks.items()):
        kinds = [t for _, t, _ in ts]
        chunks = [(n, s) for n, t, s in ts if t.startswith('chunk ')]
        last = [(n, t, s) for n, t, s in ts if not t.startswith('chunk ')]
        if len(last) != 1 or (chunks and last[0][1] != 'whole-file review'):
            errors.append('docs/audit-plan.tsv: %s needs one pass, fold, or chunks and one '
                          'whole-file review (has %s)' % (f, ', '.join(kinds)))
            continue
        n, kind, state = last[0]
        if state == 'done' and any(s == 'open' for _, s in chunks):
            errors.append('docs/audit-plan.tsv:%d: %s\'s whole-file review is done but a chunk is '
                          'open' % (n, f))
        if kind == 'fold':
            if state == 'open' and f not in rows:
                errors.append('docs/audit-plan.tsv:%d: %s is folded away; mark its fold done'
                              % (n, f))
            continue
        if f not in rows:
            errors.append('docs/audit-plan.tsv:%d: %s is not in docs/audit.tsv (renamed? update '
                          'the plan)' % (n, f))
        elif (rows[f][0] == 'done') != (state == 'done'):
            errors.append('docs/audit-plan.tsv:%d: %s is %s in docs/audit.tsv but its %s is %s'
                          % (n, f, rows[f][0], kind, state))
    errors += ['%s is open in docs/audit.tsv but has no task in docs/audit-plan.tsv' % f
               for f, r in sorted(rows.items()) if r[0] == 'open' and f not in tasks]
    return errors


def save(rows):
    out = [HEAD] + ['%s\t%s\t%s\t%s' % ((f,) + r) for f, r in sorted(rows.items())]
    TABLE.write_text('\n'.join(out) + '\n', encoding='utf-8', newline='\n')


def main():
    args = sys.argv[1:]
    files = game_files()
    if not TABLE.exists():
        save({f: ('open', '', '') for f in files})
    rows, errors = load()
    if '--sync' in args:
        new = [f for f in files if f not in rows]
        gone = [f for f in rows if f not in files]
        for f in new:
            rows[f] = ('open', '', '')
        for f in gone:
            print('dropped %s (%s)' % (f, rows.pop(f)[0]))
        print('added %d file(s) as open' % len(new))
        save(rows)
        return 0
    if '--check' in args:
        errors += ['%s is a game file missing from docs/audit.tsv (audit.py --sync)' % f
                   for f in files if f not in rows]
        errors += ['docs/audit.tsv lists %s, which is not in src/ (audit.py --sync)' % f
                   for f in rows if f not in files]
        errors += check_plan(rows)
        protos = lint.header_protos()
        for f, (state, _, _) in sorted(rows.items()):
            if state != 'done' or f not in files:
                continue
            path = ROOT / 'src' / f
            hits = lint.lint(path, protos)
            if lint.SWEEP_DEBT.get(path.name):
                hits.append((1, 'sweep-code', 'still holds sweep code'))
            errors += ['src/%s:%d: %s: %s (the file is marked done)' % (f, ln, n, msg[:100])
                       for ln, n, msg in hits]
        for e in errors:
            print(e)
        done = sum(r[0] == 'done' for r in rows.values())
        print('audit: %d of %d files done, %d problem(s)' % (done, len(rows), len(errors)))
        return 1 if errors else 0
    for e in errors:
        print(e)
    per = collections.defaultdict(lambda: [0, 0])
    for f, (state, _, _) in rows.items():
        d = f.rsplit('/', 1)[0] if '/' in f else '.'
        per[d][1] += 1
        per[d][0] += state == 'done'
    for d, (k, n) in sorted(per.items()):
        print('%-28s %4d / %4d' % ('src/' + d if d != '.' else 'src/', k, n))
    done = sum(k for k, _ in per.values())
    total = sum(n for _, n in per.values())
    print('%-28s %4d / %4d  (%.1f%%)' % ('total', done, total, 100.0 * done / total if total else 0))
    return 0


if __name__ == '__main__':
    sys.exit(main())
