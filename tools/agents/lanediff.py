"""Lane edits that did not reach main: the replay's last check.
    python tools/agents/lanediff.py <lane> [globals.tsv ...]
For every src/include file the lane changed, each line the lane added (its fork point -> its branch)
is looked for in main's working copy. Both sides first go through merge_norm.py (every old function
or global name read as today's); a line counts as present when its words (without the `//`
markers) appear in that order anywhere in main's file, so rewrapped comments and other lanes' work
in the same file do not show. What is printed is a lane line main does not have: a lint fix inside a
batch commit, a hand edit lost in a conflict, a comment another step overwrote. Carry each over or
say why not. Exit status 1 when any line is missing.
"""
import pathlib, subprocess, sys, tempfile

lane, *tsvs = sys.argv[1:]
base = subprocess.run(['git', 'merge-base', 'HEAD', 'agent/' + lane], capture_output=True, text=True).stdout.strip()
files = subprocess.run(['git', 'diff', '--name-only', base, 'agent/' + lane, '--', 'src', 'include'],
                       capture_output=True, text=True).stdout.split()
here = pathlib.Path(__file__).parent


def words(text):
    return ' '.join(w for w in text.split() if w != '//')


def normalized(data, tmp, name):
    p = pathlib.Path(tmp, name)
    p.write_bytes(data)
    subprocess.run([sys.executable, str(here / 'merge_norm.py'), str(p)] + tsvs, check=True)
    return p.read_text(encoding='utf-8', errors='surrogateescape')


total = 0
with tempfile.TemporaryDirectory() as tmp:
    for f in files:
        show = lambda rev: subprocess.run(['git', 'show', '%s:%s' % (rev, f)], capture_output=True).stdout
        old = normalized(show(base), tmp, 'base')
        new = normalized(show('agent/' + lane), tmp, 'lane')
        main = normalized(pathlib.Path(f).read_bytes() if pathlib.Path(f).exists() else b'', tmp, 'main')
        pathlib.Path(tmp, 'b').write_text(old, encoding='utf-8', errors='surrogateescape')
        pathlib.Path(tmp, 'l').write_text(new, encoding='utf-8', errors='surrogateescape')
        d = subprocess.run(['diff', '-U0', str(pathlib.Path(tmp, 'b')), str(pathlib.Path(tmp, 'l'))],
                           capture_output=True, text=True, errors='surrogateescape').stdout
        added = [l[1:] for l in d.splitlines() if l.startswith('+') and not l.startswith('+++')]
        hay = words(main)
        missing = [l for l in added if words(l) and words(l) not in hay]
        if missing:
            total += len(missing)
            print('== %s: %d lane lines not on main' % (f, len(missing)))
            for l in missing:
                print('  ' + l.rstrip()[:160])
print('%d lane lines missing from main, in %d files checked' % (total, len(files)))
sys.exit(1 if total else 0)
