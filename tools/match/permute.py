"""Run decomp-permuter on one function with a hard time limit.
    python tools/match/permute.py <Unit> <fn> [--minutes 20] [-j 4] [--max-jobs N]
-j is capped at 4 because the machine is usually shared; --max-jobs N raises the cap (an idle
many-core machine: --max-jobs 18 -j 18).
Sets up build/perm/<fn> (perm_setup.py), runs the permuter, and stops it after the time limit,
killing its whole process tree (Git Bash's `timeout` does not stop it on Windows). Then lists the
best outputs found. Read the diff in the best output and apply the idea by hand."""
import glob, os, pathlib, subprocess, sys, time

ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
sys.path.insert(0, str(ROOT / 'tools/match'))
from hosttools import NEW_PROCESS_GROUP, kill_orphan_workers, stop_tree  # noqa: E402
# decomp-permuter checkout: TW_PERMUTER, else tools/decomp-permuter beside the main checkout
# (a worktree sits one level deeper, in <parent>/tw2004-agents/<lane>)
PERMUTER = os.environ.get('TW_PERMUTER') or next(
    (str(p) for p in (ROOT.parent / 'tools/decomp-permuter/permuter.py',
                      ROOT.parent.parent / 'tools/decomp-permuter/permuter.py') if p.exists()),
    str(ROOT.parent / 'tools/decomp-permuter/permuter.py'))
args = sys.argv[1:]
minutes, jobs, max_jobs = 20, 4, 4
if '--minutes' in args:
    k = args.index('--minutes'); minutes = float(args[k + 1]); del args[k:k + 2]
if '--max-jobs' in args:
    k = args.index('--max-jobs'); max_jobs = int(args[k + 1]); del args[k:k + 2]
if '-j' in args:
    k = args.index('-j'); jobs = int(args[k + 1]); del args[k:k + 2]
unit, fn = args[0], args[1]
jobs = min(jobs, max_jobs)           # the machine is shared (docs/workflow.md)

kill_orphan_workers()                # leftovers of earlier runs
subprocess.run([sys.executable, str(ROOT / 'tools/match/perm_setup.py'), unit, fn], check=True)
out = ROOT / 'build/perm' / fn
log = open(out / 'permuter.log', 'w')   # its progress line gives the iteration count
p = subprocess.Popen([sys.executable, PERMUTER, str(out), '-j%d' % jobs, '--best-only'],
                     stdout=log, stderr=subprocess.STDOUT, **NEW_PROCESS_GROUP)
timed_out = False
try:
    p.wait(timeout=minutes * 60)
except subprocess.TimeoutExpired:
    timed_out = True
finally:                             # whatever ended the wait, the whole tree goes
    if p.poll() is None:
        stop_tree(p)
    kill_orphan_workers()
    log.close()
print('permuter stopped after', 'the time limit' if timed_out else 'finishing', f'({minutes} min)')
progress = [l for l in (out / 'permuter.log').read_text(errors='replace').replace('\r', '\n').split('\n')
            if l.startswith('iteration ')]
if progress:
    print(' ', progress[-1].strip())
best = sorted(glob.glob(str(out / 'output-*')), key=lambda d: int(d.split('output-')[1].split('-')[0]))
if not best:
    print('no improvement found')
for d in best[:5]:
    print(' ', os.path.relpath(d, ROOT), '(score', d.split('output-')[1].split('-')[0] + ', 0 = exact)')
