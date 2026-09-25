"""What kind of difference each non-exact function has, to pick work with real logic in it.

    python tools/agents/diffkinds.py [--kind LOGIC|CONST|REG] [--unit NAME] [--max-attempts N]

Needs a current build/GW4E69/report.json (`ninja build/GW4E69/report.json`). For every game
function below 100% it asks objdiff for the instruction diff and sorts the differing instructions:
  REG    only register numbers differ (same instructions): register-allocation roulette
  CONST  some immediate or offset differs, no instruction added or removed: a type, constant or
         struct detail
  LOGIC  instructions missing, extra or different: the C's structure is wrong; the best odds
A function's kind is LOGIC if any instruction is structural, else CONST if any constant differs.
Columns: match %, bytes, differing instructions per class (reg / const / structural), attempts
logged in agents/tried/<fn>.md, and how many functions its unit still has left (1 = it links).
Sorted: LOGIC first, then fewest attempts, fewest left in the unit, most structural."""
import collections, json, os, pathlib, re, subprocess, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/match'))
from hosttools import OBJDIFF_CLI  # noqa: E402

REG = re.compile(r'^(r\d+|f\d+|cr\d)$')
ORDER = {'LOGIC': 0, 'CONST': 1, 'REG': 2, '?': 3}


def kinds(unit, fn):
    p = subprocess.run([OBJDIFF_CLI, 'diff', '-p', str(ROOT), '-u', unit, fn, '-o', '-', '--format', 'json'],
                       capture_output=True, text=True, cwd=ROOT)
    try:
        d = json.loads(p.stdout)
    except ValueError:
        return None
    left = next((s for s in d['left']['symbols'] if s.get('name') == fn), None)
    right = next((s for s in d['right']['symbols'] if s.get('name') == fn), None)
    if not left or not right:
        return None
    reg = const = struct = 0
    for a, b in zip(left.get('instructions', []), right.get('instructions', [])):
        k = a.get('diff_kind', 'NONE')
        if k == 'NONE':
            continue
        if k != 'DIFF_ARG_MISMATCH':
            struct += 1
            continue
        ta = re.split(r'[\s,()]+', a.get('instruction', {}).get('formatted', ''))[1:]
        tb = re.split(r'[\s,()]+', b.get('instruction', {}).get('formatted', ''))[1:]
        diffs = [(x, y) for x, y in zip(ta, tb) if x != y]
        if diffs and all(REG.match(x) and REG.match(y) for x, y in diffs):
            reg += 1
        else:
            const += 1
    return reg, const, struct


def attempts(fn):
    p = ROOT / 'agents/tried' / (fn + '.md')
    if not p.exists():
        return 0
    t = p.read_text(encoding='utf-8')
    return len(re.findall(r'^- 20\d\d-', t, re.M)) + len(re.findall(r'^### ', t, re.M))


def main():
    a = sys.argv[1:]
    want = a[a.index('--kind') + 1].upper() if '--kind' in a else None
    only = a[a.index('--unit') + 1] if '--unit' in a else None
    maxatt = int(a[a.index('--max-attempts') + 1]) if '--max-attempts' in a else None
    rep = json.load(open(ROOT / 'build/GW4E69/report.json'))
    todo = []
    for u in rep['units']:
        if u.get('metadata', {}).get('auto_generated'):
            continue
        for f in u.get('functions', []):
            if f.get('fuzzy_match_percent', 0) < 100:
                todo.append((u['name'], f))
    left = collections.Counter(u for u, f in todo)
    rows = []
    for unit, f in todo:
        name = unit.split('/')[-1]
        if only and name != only:
            continue
        k = kinds(unit, f['name'])
        reg, const, struct = k or (0, 0, 0)
        kind = '?' if k is None else 'LOGIC' if struct else 'CONST' if const else 'REG'
        att = attempts(f['name'])
        if (want and kind != want) or (maxatt is not None and att > maxatt):
            continue
        rows.append((kind, name, f['name'], f.get('fuzzy_match_percent', 0), int(f.get('size', 0)),
                     reg, const, struct, att, left[unit]))
    rows.sort(key=lambda r: (ORDER[r[0]], r[8], r[9], -r[7]))
    c = collections.Counter(r[0] for r in rows)
    print('LOGIC %d  CONST %d  REG %d%s' % (c['LOGIC'], c['CONST'], c['REG'], ('  ? %d' % c['?']) if c['?'] else ''))
    print('%-5s %-24s %-30s %6s %6s  %4s %5s %6s  %4s %4s' % ('kind', 'unit', 'function', 'match', 'bytes',
                                                               'reg', 'const', 'struct', 'att', 'left'))
    for r in rows:
        print('%-5s %-24s %-30s %5.1f%% %6d  %4d %5d %6d  %4d %4d' % (r[0], r[1][:24], r[2][:30], r[3], r[4],
                                                                       r[5], r[6], r[7], r[8], r[9]))


if __name__ == '__main__':
    main()
