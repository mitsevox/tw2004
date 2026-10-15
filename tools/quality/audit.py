"""The audit table: which game source files have had their audit pass (docs/style.md, "The audit").
    python tools/quality/audit.py            progress: files done out of the total, per folder
    python tools/quality/audit.py --check    fail if the table and src/ disagree, or a done file
                                             has a lint finding (CI runs this after the build)
    python tools/quality/audit.py --sync     add new game files as open, drop rows of files that
                                             are gone (after a file is merged, split or moved)
docs/audit.tsv, tab-separated: file (under src/), state (open or done), commit (the one that
finished it, empty while open). A file is done when lint has nothing to say about it and a
reviewer who did not write the change has passed it. The SDK's own files (src/dolphin/) are not
listed."""
import collections, pathlib, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools' / 'quality'))
import lint                                          # noqa: E402

TABLE = ROOT / 'docs' / 'audit.tsv'
HEAD = 'file\tstate\tcommit'
SKIP = ('dolphin/',)


def game_files():
    return sorted(p.relative_to(ROOT / 'src').as_posix() for p in (ROOT / 'src').rglob('*.c')
                  if not p.relative_to(ROOT / 'src').as_posix().startswith(SKIP))


def load():
    rows, errors = {}, []
    for n, l in enumerate(TABLE.read_text(encoding='utf-8').splitlines(), 1):
        if not l.strip() or l.startswith('#') or l == HEAD:
            continue
        f = l.split('\t')
        if len(f) != 3 or f[1] not in ('open', 'done') or (f[1] == 'done') != bool(f[2]):
            errors.append('docs/audit.tsv:%d: need file, open or done, and the commit of a done row' % n)
            continue
        if f[0] in rows:
            errors.append('docs/audit.tsv:%d: %s is listed twice' % (n, f[0]))
        rows[f[0]] = (f[1], f[2])
    return rows, errors


def save(rows):
    out = [HEAD] + ['%s\t%s\t%s' % (f, s, c) for f, (s, c) in sorted(rows.items())]
    TABLE.write_text('\n'.join(out) + '\n', encoding='utf-8', newline='\n')


def main():
    args = sys.argv[1:]
    files = game_files()
    if not TABLE.exists():
        save({f: ('open', '') for f in files})
    rows, errors = load()
    if '--sync' in args:
        new = [f for f in files if f not in rows]
        gone = [f for f in rows if f not in files]
        for f in new:
            rows[f] = ('open', '')
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
        protos = lint.header_protos()
        for f, (state, _) in sorted(rows.items()):
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
        done = sum(s == 'done' for s, _ in rows.values())
        print('audit: %d of %d files done, %d problem(s)' % (done, len(rows), len(errors)))
        return 1 if errors else 0
    for e in errors:
        print(e)
    per = collections.defaultdict(lambda: [0, 0])
    for f, (state, _) in rows.items():
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
