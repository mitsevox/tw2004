"""List a unit's functions that are not exact.
    python tools/match/unitfns.py main/<Unit> [all]
Prints the unit score, then every function under 100%. Pass `all` to also list functions with no
score (not compiled yet, shown as None), which otherwise stay hidden. Run ninja
build/GW4E69/report.json first."""
import json, pathlib, sys
ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
r = json.load(open(ROOT / 'build/GW4E69/report.json'))
for u in r['units']:
    if u['name'] == sys.argv[1]:
        m = u['measures']; print('unit', m.get('fuzzy_match_percent'), m.get('matched_functions'), '/', m.get('total_functions'))
        for f in u['functions']:
            p = f.get('fuzzy_match_percent')
            if (p is not None and p < 100) or (len(sys.argv) > 2 and p is None): print(f"  {f['name']:24s} {p}")
