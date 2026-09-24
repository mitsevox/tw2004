"""Prove that uncommitted changes touch only names and comment text (the name and comment audit).
    python tools/match/auditcheck.py [<base rev>]      default base: HEAD
Checks every changed file against the base:
  config/GW4E69/symbols.txt  only symbol names may change (same address and attributes per line);
  .c / .h under src/, include/  with the renames applied (read from the symbols.txt diff) and
                              comments stripped, the token stream must be identical: no code, type,
                              declaration or include may change;
                              a file that uses __LINE__ (directly or through a macro, e.g. SharedFileIO's
                              SFIO_ASSERT) must also keep its line count and every line's code, since
                              moving a line changes the constant compiled into the game;
  config/GW4E69/name_sources.tsv, config/GW4E69/audit.tsv  allowed (the audit's own records);
  anything else                 refused (configure.py, splits.txt, tools ...).
Exit 1 on any finding."""
import pathlib, re, subprocess, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/match'))
from blindview import strip_comments          # noqa: E402

ALLOWED = {'config/GW4E69/name_sources.tsv', 'config/GW4E69/audit.tsv'}
TOKEN = re.compile(r'"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'|[A-Za-z_]\w*|\d[\w.]*|\S')
SYM = re.compile(r'^(\S+) = (.*)$')


def git(*args):
    return subprocess.run(['git', *args], cwd=ROOT, capture_output=True, text=True, encoding='utf-8',
                          errors='surrogateescape')


def line_macros():
    """Macros whose body uses __LINE__ (a file using one bakes line numbers into its code)."""
    names = set()
    for p in list((ROOT / 'include').rglob('*.h')) + list((ROOT / 'src').rglob('*.h')):
        t = p.read_text(encoding='utf-8', errors='replace')
        for m in re.finditer(r'^\s*#\s*define\s+(\w+)(?:\([^)]*\))?((?:[^\n]*\\\n)*[^\n]*)$', t, re.M):
            if '__LINE__' in m.group(2):
                names.add(m.group(1))
    return names


def main():
    base = sys.argv[1] if len(sys.argv) > 1 else 'HEAD'
    changed = [l for l in git('diff', '--name-only', base).stdout.splitlines() if l]
    changed += [l for l in git('ls-files', '--others', '--exclude-standard').stdout.splitlines()
                if l.startswith(('src/', 'include/', 'config/'))]
    errors, renames = [], {}
    if 'config/GW4E69/symbols.txt' in changed:
        old = git('show', '%s:config/GW4E69/symbols.txt' % base).stdout.split('\n')
        new = (ROOT / 'config/GW4E69/symbols.txt').read_text(encoding='utf-8').split('\n')
        if len(old) != len(new):
            errors.append('symbols.txt: line count changed (%d -> %d)' % (len(old), len(new)))
        for i, (a, b) in enumerate(zip(old, new), 1):
            if a == b:
                continue
            ma, mb = SYM.match(a), SYM.match(b)
            if not ma or not mb or ma.group(2) != mb.group(2):
                errors.append('symbols.txt:%d: more than the name changed' % i)
            else:
                renames[ma.group(1)] = mb.group(1)
    rename = re.compile(r'\b(%s)\b' % '|'.join(map(re.escape, sorted(renames, key=len, reverse=True)))) if renames else None
    lm = line_macros()
    for f in changed:
        if f == 'config/GW4E69/symbols.txt' or f in ALLOWED:
            continue
        if not f.startswith(('src/', 'include/')) or not f.endswith(('.c', '.h', '.inc')):
            errors.append('%s: not a file the audit may change' % f)
            continue
        o = git('show', '%s:%s' % (base, f))
        if o.returncode:
            errors.append('%s: new file' % f)
            continue
        old = o.stdout
        new = (ROOT / f).read_text(encoding='utf-8', errors='surrogateescape') if (ROOT / f).exists() else ''
        if rename:
            old = rename.sub(lambda m: renames[m.group(1)], old)
        so, sn = strip_comments(old), strip_comments(new)
        if TOKEN.findall(so) != TOKEN.findall(sn):
            ot, nt = TOKEN.findall(so), TOKEN.findall(sn)
            k = next((i for i, (x, y) in enumerate(zip(ot, nt)) if x != y), min(len(ot), len(nt)))
            errors.append('%s: code changed, not only names/comments (first difference: %r -> %r)'
                          % (f, ' '.join(ot[max(0, k - 3):k + 3]), ' '.join(nt[max(0, k - 3):k + 3])))
            continue
        uses_line = '__LINE__' in so or any(re.search(r'\b%s\b' % m, so) for m in lm)
        if uses_line:
            lo, ln = so.split('\n'), sn.split('\n')
            if len(lo) != len(ln):
                errors.append('%s: uses __LINE__ and its line count changed (%d -> %d)' % (f, len(lo), len(ln)))
            else:
                bad = [i + 1 for i, (x, y) in enumerate(zip(lo, ln)) if TOKEN.findall(x) != TOKEN.findall(y)]
                if bad:
                    errors.append('%s: uses __LINE__ and code moved between lines (first at line %d)' % (f, bad[0]))
    for e in errors:
        print('ERROR', e)
    print('%d changed file(s), %d rename(s): %s' % (len(changed), len(renames), 'OK' if not errors else '%d error(s)' % len(errors)))
    return 1 if errors else 0


if __name__ == '__main__':
    sys.exit(main())
