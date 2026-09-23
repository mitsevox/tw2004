"""After mkunit.py: fold the sweep files it listed into the new unit, and commit.
    python tools/match/fold.py <Name> <sweep1.c> ... [--push]
Sweep names as mkunit printed them (e.g. unsorted/sweep_X.c). Runs configure, merges and dedupes,
builds, and checks that every folded function is exact. Only then does it git rm the sweeps and
commit. Nothing is deleted unless the DOL is OK and every folded function matches. Pushes only
with --push (agents working in a worktree leave pushing to the orchestrator)."""
import json, pathlib, re, subprocess, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
HERE = pathlib.Path(__file__).resolve().parent
args = [a for a in sys.argv[1:] if a != '--push']
push = '--push' in sys.argv
name, sweeps = args[0], args[1:]


def run(cmd):
    return subprocess.run(cmd, shell=True, cwd=ROOT, capture_output=True, text=True)


run('python configure.py')
print(run(f'python "{HERE / "merge_sweeps.py"}" {name}.c ' + ' '.join(sweeps)).stdout.strip())
print(run(f'python "{HERE / "dedupe_decls.py"}" src/{name}.c').stdout.strip())
run('rm -f build/GW4E69/ok')
b = run('ninja')
if 'main.dol: OK' not in b.stdout:
    print('BUILD NOT OK'); print(b.stdout[-2000:]); sys.exit(1)
run('ninja build/GW4E69/report.json')
r = json.load(open(ROOT / 'build/GW4E69/report.json'))
u = [u for u in r['units'] if u['name'] == 'main/' + name][0]
ex = {f['name'] for f in u['functions'] if f.get('fuzzy_match_percent') == 100}
src = (ROOT / 'src' / f'{name}.c').read_text(encoding='utf-8')
defs = re.findall(r'^\w[\w \*]*\b(fn_\w+)\([^)]*\)\s*\{', src, re.M)
miss = [d for d in defs if d not in ex]
print(len(defs), 'defined, not exact:', miss, 'total fns', len(u['functions']))
if miss or not defs:
    print('NOT deleting sweeps'); sys.exit(1)
print(run('git rm -q ' + ' '.join('src/' + s for s in sweeps)).stderr.strip())
print(run(f'git add src/{name}.c config/GW4E69/splits.txt configure.py').stderr.strip())
lo = re.search(r'start:(0x\w+) end:(0x\w+)',
               (ROOT / 'config/GW4E69/splits.txt').read_text().split(name + '.c:')[1])
msg = f'{name}.c: new unit ({lo.group(1)}-{lo.group(2)}), {len(sweeps)} sweep files folded in'
print(run(f'git commit -q -m "{msg}"').stderr.strip())
if push:
    print(run('git push -q').stderr.strip()[-200:])
print(run('git log --oneline -1').stdout.strip())
