"""After mkunit.py: fold the sweep files it listed into the unit, and commit.
    python tools/match/fold.py <Name> <sweep1.c> ... [--push]
Sweep names as mkunit printed them (e.g. unsorted/sweep_X.c). Runs configure, merges and dedupes,
builds, and checks that every folded function is exact. Only then does it git rm the sweeps and
commit. Nothing is deleted unless the DOL is OK and every folded function matches. Pushes only
with --push (agents working in a worktree leave pushing to the orchestrator).

For a unit that already has a source (widened with `mkunit.py --extend`), the sweeps' code is
appended in a sweep block and the rest of the file is kept (merge_sweeps.py); if the build or the
check fails, the source is put back as it was."""
import json, pathlib, re, subprocess, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
HERE = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from merge_sweeps import defined                     # noqa: E402
args = [a for a in sys.argv[1:] if a != '--push']
push = '--push' in sys.argv
name, sweeps = args[0], args[1:]
path = ROOT / 'src' / f'{name}.c'
before = path.read_text(encoding='utf-8') if path.exists() else None
extending = bool(before and before.strip())


def run(cmd):
    return subprocess.run(cmd, shell=True, cwd=ROOT, capture_output=True, text=True)


def fail(msg):
    print(msg)
    if extending:
        path.write_text(before, encoding='utf-8', newline='\n')
        print(f'src/{name}.c put back as it was')
    sys.exit(1)


folded = [n for s in sweeps for _, n in defined((ROOT / 'src' / s).read_text(encoding='utf-8'))]
run('python configure.py')
m = run(f'python "{HERE / "merge_sweeps.py"}" {name}.c ' + ' '.join(sweeps))
print(m.stdout.strip())
if m.returncode:
    fail('merge failed: ' + (m.stderr.strip() or m.stdout.strip())[-500:])
print(run(f'python "{HERE / "dedupe_decls.py"}" src/{name}.c').stdout.strip())
run('rm -f build/GW4E69/ok')
b = run('ninja')
if 'main.dol: OK' not in b.stdout:
    print(b.stdout[-2000:])
    fail('BUILD NOT OK')
run('ninja build/GW4E69/report.json')
r = json.load(open(ROOT / 'build/GW4E69/report.json'))
u = [u for u in r['units'] if u['name'] == 'main/' + name][0]
ex = {f['name'] for f in u['functions'] if f.get('fuzzy_match_percent') == 100}
miss = [d for d in folded if d not in ex]
print(len(folded), 'folded, not exact:', miss, 'total fns', len(u['functions']))
if miss or not folded:
    fail('NOT deleting sweeps')
rm = run('git rm -q ' + ' '.join('src/' + s for s in sweeps))
if rm.returncode != 0:
    # e.g. a sweep file has local changes: someone edited it, so it is not deleted unchecked
    print('git rm failed, nothing committed:', rm.stderr.strip()); sys.exit(1)
print(run(f'git add src/{name}.c config/GW4E69/splits.txt configure.py').stderr.strip())
lo = re.search(r'start:(0x\w+) end:(0x\w+)',
               (ROOT / 'config/GW4E69/splits.txt').read_text().split(name + '.c:')[1])
what = 'widened to' if extending else 'new unit'
msg = f'{name}.c: {what} ({lo.group(1)}-{lo.group(2)}), {len(sweeps)} sweep files folded in'
print(run(f'git commit -q -m "{msg}"').stderr.strip())
if push:
    print(run('git push -q').stderr.strip()[-200:])
print(run('git log --oneline -1').stdout.strip())
