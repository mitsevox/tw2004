"""Try every order of a function's leading declaration lines and keep the best.

    python tools/match/declperm.py <unit> <src.c> <function> <number of declaration lines>
The declaration lines are the first N lines after the function's opening line.
"""
import itertools, json, pathlib, re, subprocess, sys
unit, src, fn, n = sys.argv[1], sys.argv[2], sys.argv[3], int(sys.argv[4])
TW = str(pathlib.Path(__file__).resolve().parents[2])   # the checkout this script lives in
base = open(src, encoding='utf-8').read()
m = re.search(r'^[A-Za-z_][^\n;]*\b' + re.escape(fn) + r'\([^;{]*\)\s*\{', base, re.M)
start = base.index('\n', m.start()) + 1
lines = base[start:].split('\n')
decl = lines[:n]
rest = '\n'.join(lines[n:])


def score():
    subprocess.run('ninja build/GW4E69/report.json', shell=True, cwd=TW, capture_output=True)
    r = json.load(open(TW + '/build/GW4E69/report.json'))
    u = [u for u in r['units'] if u['name'] == 'main/' + unit][0]
    return [f.get('fuzzy_match_percent') or 0 for f in u['functions'] if f['name'] == fn][0]


best = None
for perm in itertools.permutations(decl):
    s = base[:start] + '\n'.join(perm) + '\n' + rest
    open(src, 'w', encoding='utf-8', newline='\n').write(s)
    pc = score()
    if best is None or pc > best[1]:
        best = (perm, pc)
    if pc == 100:
        break
s = base[:start] + '\n'.join(best[0]) + '\n' + rest
open(src, 'w', encoding='utf-8', newline='\n').write(s)
print(fn, 'best', best[1])
print('\n'.join(best[0]))
