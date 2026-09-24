"""One-screen status of every agent worktree.
    python tools/agents/status.py
For each worktree under <main checkout>-agents: branch, commits not yet in main, commits main has that
the branch lacks, the last commit's age, and matched bytes ahead of main (from each last built report)."""
import json, os, subprocess, time
import paths

MAIN = paths.MAIN_S
ROOT = paths.WORKTREES_S


def git(*a):
    return subprocess.run(['git', '-C', MAIN] + list(a), capture_output=True, text=True).stdout.strip()


def units(path):
    try:
        r = json.load(open(path + '/build/GW4E69/report.json'))
    except Exception:
        return None
    return {u['name']: int(u['measures'].get('matched_code', 0) or 0) for u in r['units']}


main_units = units(MAIN) or {}
m = json.load(open(MAIN + '/build/GW4E69/report.json'))['measures']
print('main %s: exact %d, matched %.2f%%, linked code %.2f%%, linked data %.2f%%' % (
    git('log', '--oneline', '-1', '--format=%h'), m['matched_functions'], m['matched_code_percent'],
    m['complete_code_percent'], m['complete_data_percent']))
print('%-12s %9s %9s %10s %12s' % ('agent', 'unmerged', 'behind', 'last', 'ahead (B)'))
for name in sorted(os.listdir(ROOT)):
    br = f'agent/{name}'
    if not git('rev-parse', '--verify', '-q', br):
        continue
    ahead = git('rev-list', '--count', f'main..{br}')
    behind = git('rev-list', '--count', f'{br}..main')
    ts = int(git('log', '-1', '--format=%ct', br) or 0)
    age = '%dm ago' % ((time.time() - ts) / 60) if ts else '-'
    u = units(f'{ROOT}/{name}')
    d = sum(max(0, v - main_units.get(k, 0)) for k, v in u.items()) if u else 0
    print('%-12s %9s %9s %10s %12d' % (name, ahead, behind, age, d))
