#!/usr/bin/env python3
"""
Backfill build/dashboard_reports.json (objdiff matched code per commit) by rebuilding old commits
in a throwaway worktree, then refresh the dashboard history.

    python tools/dashboard/backfill_history.py <oldest-commit> [<newest-commit>]

Builds every commit in oldest..newest (inclusive) that has no cached report yet, oldest first.
The worktree lives in ../tw2004_backfill next to the repo and reuses this checkout's
build/compilers, build/tools and orig/ so nothing is downloaded. A commit whose build fails is
skipped (the history falls back to its linked_code). Progress goes to build/backfill.log.
"""
import json, os, shutil, subprocess, sys, time
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
VERSION = 'GW4E69'
WT = os.path.join(os.path.dirname(ROOT), os.path.basename(ROOT) + '_backfill')
REPORTS = os.path.join(ROOT, 'build', 'dashboard_reports.json')
LOG = os.path.join(ROOT, 'build', 'backfill.log')

def log(msg):
    with open(LOG, 'a') as f:
        f.write(time.strftime('%H:%M:%S ') + msg + '\n')

def git(*a, cwd=ROOT):
    return subprocess.run(['git', '--no-pager', *a], cwd=cwd, capture_output=True, text=True, stdin=subprocess.DEVNULL)

oldest = sys.argv[1]; newest = sys.argv[2] if len(sys.argv) > 2 else 'HEAD'
commits = git('rev-list', '--reverse', f'{oldest}^..{newest}').stdout.split()
reports = json.load(open(REPORTS)) if os.path.exists(REPORTS) else {}
todo = [c for c in commits if c not in reports]
log(f'backfill {len(todo)} of {len(commits)} commits')

if not os.path.isdir(WT):
    r = git('worktree', 'add', '--detach', WT, todo[0] if todo else newest)
    log('worktree: ' + (r.stdout + r.stderr).strip())
for d in ('compilers', 'tools'):
    src = os.path.join(ROOT, 'build', d); dst = os.path.join(WT, 'build', d)
    if os.path.isdir(src) and not os.path.isdir(dst):
        shutil.copytree(src, dst)
dol = os.path.join('orig', VERSION, 'sys', 'main.dol')
if not os.path.exists(os.path.join(WT, dol)):
    os.makedirs(os.path.dirname(os.path.join(WT, dol)), exist_ok=True)
    shutil.copy2(os.path.join(ROOT, dol), os.path.join(WT, dol))

for i, c in enumerate(todo):
    t0 = time.time()
    git('checkout', '-q', '--detach', c, cwd=WT)
    git('clean', '-qfd', '-e', 'build', '-e', 'orig', cwd=WT)
    ok = subprocess.run([sys.executable, 'configure.py'], cwd=WT, capture_output=True, text=True, stdin=subprocess.DEVNULL).returncode == 0
    if ok:
        ok = subprocess.run(['ninja'], cwd=WT, capture_output=True, text=True, stdin=subprocess.DEVNULL).returncode == 0
    rep = os.path.join(WT, 'build', VERSION, 'report.json')
    if ok and os.path.exists(rep):
        try:
            m = int(json.load(open(rep))['measures']['matched_code'])
        except Exception as e:
            ok = False; m = None
    if ok:
        reports = json.load(open(REPORTS)) if os.path.exists(REPORTS) else {}
        reports[c] = m
        json.dump(reports, open(REPORTS, 'w'), indent=0)
        log(f'[{i + 1}/{len(todo)}] {c[:7]} {m:,} bytes ({time.time() - t0:.0f}s)')
    else:
        log(f'[{i + 1}/{len(todo)}] {c[:7]} BUILD FAILED, skipped ({time.time() - t0:.0f}s)')
    subprocess.run([sys.executable, os.path.join(ROOT, 'tools', 'dashboard', 'refresh_history.py')],
                   cwd=ROOT, capture_output=True, text=True, stdin=subprocess.DEVNULL,
                   env={**os.environ, 'DASHBOARD_NO_HEAD_REPORT': '1'})
log('done')
