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
p = subprocess.Popen([sys.executable, PERMUTER, str(out), '-j%d' % jobs, '--best-only'],
                     stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
try:
    p.wait(timeout=minutes * 60)
except subprocess.TimeoutExpired:
    pass
finally:                             # whatever ended the wait, the whole tree goes
    if p.poll() is None:
        if os.name == 'nt':
            subprocess.run(['taskkill', '/PID', str(p.pid), '/T', '/F'], capture_output=True)
        else:
            p.kill()
        p.wait()
    kill_orphan_workers()
print('permuter stopped after', 'the time limit' if p.returncode else 'finishing', f'({minutes} min)')
best = sorted(glob.glob(str(out / 'output-*')), key=lambda d: int(d.split('output-')[1].split('-')[0]))
if not best:
    print('no improvement found')
for d in best[:5]:
    print(' ', os.path.relpath(d, ROOT), '(score', d.split('output-')[1].split('-')[0] + ', 0 = exact)')
