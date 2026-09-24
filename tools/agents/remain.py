"""What is left to match and link: per game unit, non-exact functions and bytes."""
import json, pathlib
import paths
R = paths.MAIN
rep = json.loads((R / 'build/GW4E69/report.json').read_text())
m = rep['measures']
print('overall: exact fns %s/%s  matched %.2f%%  linked(complete code) %.2f%%  units %s/%s' % (
    m.get('matched_functions'), m.get('total_functions'), m.get('matched_code_percent', 0),
    m.get('complete_code_percent', 0), m.get('complete_units'), m.get('total_units')))
rows = []
for u in rep['units']:
    md = u.get('metadata', {})
    cats = md.get('progress_categories', [])
    um = u.get('measures', {})
    complete = md.get('complete', False)
    fns = u.get('functions', [])
    bad = [f for f in fns if f.get('fuzzy_match_percent', 0) != 100]
    tot = int(um.get('total_code', 0))
    rows.append((u['name'].split('/', 1)[1], cats, complete, len(fns), bad, tot))
for cat in ('game', 'sdk'):
    rs = [r for r in rows if cat in r[1]]
    lk = [r for r in rs if r[2]]
    code = sum(r[5] for r in rs); lcode = sum(r[5] for r in lk)
    ready = [r for r in rs if not r[2] and not r[4]]
    print('%s: units %d, linked %d (%d KB of %d KB), all-exact-but-unlinked %d' % (
        cat, len(rs), len(lk), lcode // 1024, code // 1024, len(ready)))
    left = sorted([r for r in rs if r[4]], key=lambda r: len(r[4]))
    print('  units with non-exact fns: %d, non-exact fns: %d, bytes %d KB' % (
        len(left), sum(len(r[4]) for r in left), sum(int(f.get('size', 0)) for r in left for f in r[4]) // 1024))
    hist = {}
    for r in left:
        k = len(r[4]) if len(r[4]) < 5 else '5+'
        hist[k] = hist.get(k, 0) + 1
    print('  units by fns left:', hist)
    for r in left:
        print('   %-28s %3d/%3d left  %s' % (r[0][:28], len(r[4]), r[3], ' '.join(
            '%s:%.0f' % (f['name'][:22], f.get('fuzzy_match_percent', 0)) for f in r[4][:6])))
