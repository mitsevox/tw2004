#!/usr/bin/env python3
"""
The public progress page's history (GitHub Pages, fed by CI): one record per build of main.

    python tools/dashboard/pages.py append <history.json> <report.json> <sha> <time> <subject>
    python tools/dashboard/pages.py seed <history.json> [<dashboard_history.json>]

append: adds (or replaces, for the same sha) the record of one CI build: sha, date, exact
functions, matched / linked code and data, all from objdiff's report.json. seed: writes a first
history.json from build/dashboard_history.json (refresh_history.py + backfill_history.py), keeping
main's first-parent commits only. Only report.json numbers and commit subjects are written: no
game data. CI keeps the file on the data-only branch pages-history (see .github/workflows/build.yml).
"""
import json, os, subprocess, sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
KEYS = ('matched_code', 'complete_code', 'matched_data', 'complete_data', 'total_code', 'total_data',
        'matched_functions', 'total_functions')


def _num(v):
    try:
        return int(v)
    except (TypeError, ValueError):
        try:
            return int(float(v))
        except (TypeError, ValueError):
            return 0


def record(report_path, sha, time, subject):
    m = json.load(open(report_path))['measures']
    rec = {'commit': sha[:7], 'sha': sha, 'time': int(time), 'subject': subject, 'report': True}
    rec.update({k: _num(m.get(k)) for k in KEYS})
    rec['exact'] = rec['matched_functions']
    return rec


def load(path):
    try:
        h = json.load(open(path))
        return h if isinstance(h, list) else []
    except (OSError, ValueError):
        return []


def append(history_path, rec):
    h = [r for r in load(history_path) if r.get('sha', r.get('commit')) not in (rec['sha'], rec['commit'])]
    h.append(rec)
    h.sort(key=lambda r: r.get('time', 0))
    json.dump(h, open(history_path, 'w'), indent=0)
    return h


def seed(out_path, src_path):
    src = load(src_path)
    if not src:
        raise SystemExit('pages.py seed: %s is empty; run refresh_history.py (and backfill_history.py) first' % src_path)
    main = subprocess.run(['git', 'rev-list', '--first-parent', 'HEAD'], cwd=ROOT, capture_output=True,
                          text=True).stdout.split()
    keep = {c[:7] for c in main}
    out = []
    for r in src:
        if r.get('commit') not in keep:
            continue
        rec = {k: r[k] for k in ('commit', 'time', 'subject', 'report', 'matched_code', 'complete_code',
                                 'matched_data', 'complete_data', 'linked_code', 'linked_data') if k in r}
        out.append(rec)
    json.dump(out, open(out_path, 'w'), indent=0)
    print('pages.py seed: wrote %s, %d commits' % (out_path, len(out)))


if __name__ == '__main__':
    a = sys.argv[1:]
    if len(a) == 6 and a[0] == 'append':
        h = append(a[1], record(a[2], a[3], a[4], a[5]))
        print('pages.py append: %s now %d records, latest %s' % (a[1], len(h), a[3][:7]))
    elif len(a) in (2, 3) and a[0] == 'seed':
        seed(a[1], a[2] if len(a) == 3 else os.path.join(ROOT, 'build', 'dashboard_history.json'))
    else:
        raise SystemExit(__doc__)
