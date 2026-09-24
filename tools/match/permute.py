"""Run decomp-permuter on one function with a hard time limit.
    python tools/match/permute.py <Unit> <fn> [--minutes 20] [-j 4]
Sets up build/perm/<fn> (perm_setup.py), runs the permuter, and stops it after the time limit,
killing its whole process tree (Git Bash's `timeout` does not stop it on Windows). Then lists the
best outputs found. Read the diff in the best output and apply the idea by hand."""
import glob, os, pathlib, signal, subprocess, sys, time

ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
# decomp-permuter checkout: TW_PERMUTER, else tools/decomp-permuter beside the main checkout
# (a worktree sits one level deeper, in <parent>/tw2004-agents/<lane>)
PERMUTER = os.environ.get('TW_PERMUTER') or next(
    (str(p) for p in (ROOT.parent / 'tools/decomp-permuter/permuter.py',
                      ROOT.parent.parent / 'tools/decomp-permuter/permuter.py') if p.exists()),
    str(ROOT.parent / 'tools/decomp-permuter/permuter.py'))
args = sys.argv[1:]
minutes, jobs = 20, 4
if '--minutes' in args:
    k = args.index('--minutes'); minutes = float(args[k + 1]); del args[k:k + 2]
if '-j' in args:
    k = args.index('-j'); jobs = int(args[k + 1]); del args[k:k + 2]
unit, fn = args[0], args[1]
jobs = min(jobs, 4)                  # the machine is shared (docs/workflow.md)

def kill_orphan_workers():
    """Kill permuter pool workers whose parent is gone. Windows does not end a multiprocessing
    worker when its parent dies, so a permuter that finished, crashed or was stopped from outside
    this script leaves its pool behind (60 idle workers holding 2.9 GB were found on 2026-09-23)."""
    if os.name != 'nt':
        return
    ps = ("$p = Get-CimInstance Win32_Process; $alive = $p.ProcessId; "
          "$p | Where-Object { $_.CommandLine -match 'spawn_main\\(parent_pid=(\\d+)' -and "
          "-not ($alive -contains [int]$Matches[1]) } | "
          "ForEach-Object { Stop-Process -Id $_.ProcessId -Force -ErrorAction SilentlyContinue }")
    subprocess.run(['powershell', '-NoProfile', '-Command', ps], capture_output=True)


kill_orphan_workers()                # leftovers of earlier runs
subprocess.run([sys.executable, str(ROOT / 'tools/match/perm_setup.py'), unit, fn], check=True)
out = ROOT / 'build/perm' / fn
log = open(out / 'permuter.log', 'w')   # its progress line gives the iteration count
# on Linux the permuter gets its own process group: p.kill() alone left its pool workers running
p = subprocess.Popen([sys.executable, PERMUTER, str(out), '-j%d' % jobs, '--best-only'],
                     stdout=log, stderr=subprocess.STDOUT, start_new_session=os.name != 'nt')
timed_out = False
try:
    p.wait(timeout=minutes * 60)
except subprocess.TimeoutExpired:
    timed_out = True
finally:                             # whatever ended the wait, the whole tree goes
    if p.poll() is None:
        if os.name == 'nt':
            subprocess.run(['taskkill', '/PID', str(p.pid), '/T', '/F'], capture_output=True)
        else:
            os.killpg(p.pid, signal.SIGINT)      # the permuter's own clean exit
            try:
                p.wait(timeout=15)
            except subprocess.TimeoutExpired:
                os.killpg(p.pid, signal.SIGKILL)
        p.wait()
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
