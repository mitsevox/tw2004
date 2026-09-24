"""Remaining audit batches: every game function not yet in config/GW4E69/audit.tsv (exact or not),
inside the given address ranges (ranges no matching lane is editing). Units stay whole (~200 fns).
    python tools/agents/phase2b.py <label prefix> <lo-hi> [<lo-hi> ...]
-> <scratch>/phase2/batch_<prefix>NN.tsv (address, unit, exact 1/0) + plan_<prefix>.txt"""
import json, pathlib, sys
import paths

R = paths.MAIN
OUT = paths.SCRATCH / 'phase2'
prefix = sys.argv[1]
ranges = [tuple(int(x, 16) for x in r.split('-')) for r in sys.argv[2:]]
done = set()
for l in (R / 'config/GW4E69/audit.tsv').read_text(encoding='utf-8').splitlines():
    f = l.split('\t')
    if f[0] and not l.startswith('#') and f[0] != 'address':
        done.add(f[0].upper())
rep = json.loads((R / 'build/GW4E69/report.json').read_text())
units = []
for u in rep['units']:
    md = u.get('metadata', {})
    if 'game' not in md.get('progress_categories', []):
        continue
    fs = []
    for f in u.get('functions', []):
        a = f.get('metadata', {}).get('virtual_address')
        if not a:
            continue
        a = int(a)
        if '%08X' % a in done or not any(lo <= a < hi for lo, hi in ranges):
            continue
        fs.append(('%08X' % a, 1 if f.get('fuzzy_match_percent') == 100 else 0))
    if fs:
        units.append((u['name'].split('/', 1)[1], sorted(fs)))
units.sort(key=lambda x: x[1][0][0])
batches, cur, n = [], [], 0
for name, fs in units:
    if n and n + len(fs) > 220:
        batches.append(cur); cur, n = [], 0
    cur.append((name, fs)); n += len(fs)
if cur:
    batches.append(cur)
plan = []
for i, b in enumerate(batches):
    lab = '%s%02d' % (prefix, i + 1)
    rows = [(a, name, ex) for name, fs in b for a, ex in fs]
    (OUT / ('batch_%s.tsv' % lab)).write_text('address\tunit\texact\n' + ''.join('%s\t%s\t%d\n' % r for r in rows), encoding='utf-8')
    plan.append('%s\t%d fns (%d not exact)\t%s' % (lab, len(rows), sum(1 for r in rows if not r[2]), ', '.join(nm for nm, _ in b)))
(OUT / ('plan_%s.txt' % prefix)).write_text('\n'.join(plan) + '\n', encoding='utf-8')
print('\n'.join(p[:170] for p in plan))
print('total', sum(len(fs) for _, fs in units))
