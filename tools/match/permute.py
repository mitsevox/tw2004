"""Run decomp-permuter on one function with a hard time limit.
    python tools/match/permute.py <Unit> <fn> [--minutes 20] [-j 4]
Sets up build/perm/<fn> (perm_setup.py), runs the permuter, and stops it after the time limit,
killing its whole process tree (Git Bash's `timeout` does not stop it on Windows). Then lists the
best outputs found. Read the diff in the best output and apply the idea by hand."""
import glob, os, pathlib, subprocess, sys, time

ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
PERMUTER = 'C:/dev/tools/decomp-permuter/permuter.py'
args = sys.argv[1:]
minutes, jobs = 20, 4
if '--minutes' in args:
    k = args.index('--minutes'); minutes = float(args[k + 1]); del args[k:k + 2]
if '-j' in args:
    k = args.index('-j'); jobs = int(args[k + 1]); del args[k:k + 2]
unit, fn = args[0], args[1]
jobs = min(jobs, 4)                  # the machine is shared (docs/workflow.md)

subprocess.run([sys.executable, str(ROOT / 'tools/match/perm_setup.py'), unit, fn], check=True)
out = ROOT / 'build/perm' / fn
p = subprocess.Popen([sys.executable, PERMUTER, str(out), '-j%d' % jobs, '--best-only'],
                     stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
try:
    p.wait(timeout=minutes * 60)
except subprocess.TimeoutExpired:
    if os.name == 'nt':
        subprocess.run(['taskkill', '/PID', str(p.pid), '/T', '/F'], capture_output=True)
    else:
        p.kill()
    p.wait()
print('permuter stopped after', 'the time limit' if p.returncode else 'finishing', f'({minutes} min)')
best = sorted(glob.glob(str(out / 'output-*')), key=lambda d: int(d.split('output-')[1].split('-')[0]))
if not best:
    print('no improvement found')
for d in best[:5]:
    print(' ', os.path.relpath(d, ROOT), '(score', d.split('output-')[1].split('-')[0] + ', 0 = exact)')
