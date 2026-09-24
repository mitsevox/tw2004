"""Orchestrator check of one agent branch before merging.
    python review.py <agent-name>
Builds the worktree, checks the DOL, lints the branch's lines, lists commits and shared-file edits,
and compares exact functions / matched / linked code against main's last report."""
import json, subprocess, sys
import paths

name = sys.argv[1]
WT = f'{paths.WORKTREES_S}/{name}'
MAIN = paths.MAIN_S
SHARED = ['include/', 'config/GW4E69/symbols.txt', 'config/GW4E69/splits.txt', 'configure.py', 'docs/']


def run(cmd, cwd=WT):
    return subprocess.run(cmd, shell=True, cwd=cwd, capture_output=True, text=True).stdout


def measures(root):
    m = json.load(open(root + '/build/GW4E69/report.json'))['measures']
    return m['matched_functions'], int(m['matched_code']), int(m['complete_code']), int(m['complete_data'])


print('== commits')
print(run('git log --oneline main..HEAD').strip() or '(none)')
files = run('git diff --name-only main...HEAD').split()
print('== files:', len(files))
for f in files:
    tag = '  SHARED' if any(f.startswith(s) for s in SHARED) else ''
    print('  ', f + tag)
run('rm -f build/GW4E69/ok')
b = run('ninja')
print('== DOL:', 'OK' if 'main.dol: OK' in b else 'FAILED\n' + b[-1500:])
run('ninja build/GW4E69/report.json')
print('== lint (branch lines):', run('python tools/match/lint.py --diff main').strip().splitlines()[-1])
lint = run('python tools/match/lint.py --diff main').strip().splitlines()
for l in lint[:-1][:20]:
    print('  ', l)
def units(root):
    r = json.load(open(root + '/build/GW4E69/report.json'))
    return {u['name'][5:]: u['measures'] for u in r['units']}


ua, um = units(WT), units(MAIN)
cfg = open(WT + '/configure.py', encoding='utf-8').read()
print('== touched units (main -> branch): exact fns, matched B, linked')
for f in files:
    if f.startswith('src/') and f.endswith('.c'):
        n = f[4:-2]
        a, m = ua.get(n, {}), um.get(n, {})
        lk = 'Matching' if f'Object(Matching, "{n}.c")' in cfg else 'NonMatching'
        print('   %-22s %s/%s -> %s/%s  %s -> %s B  %s' % (
            n, m.get('matched_functions', '-'), m.get('total_functions', '-'),
            a.get('matched_functions', '-'), a.get('total_functions', '-'),
            m.get('matched_code', '-'), a.get('matched_code', '-'), lk))
