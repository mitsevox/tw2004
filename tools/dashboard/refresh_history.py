#!/usr/bin/env python3
"""
Write build/dashboard_history.json: matched and linked bytes and named-function count at every commit.

Two kinds of numbers per commit:
- linked_code / linked_data: the bytes of the units marked Matching in configure.py, summed from
  splits.txt at that commit (code = .text/.init, data = every other section, .bss included). This
  is the same sum objdiff reports as complete_code / complete_data, so it is exact for every commit
  without rebuilding anything.
- matched_code (and, for newer commits, matched_data): objdiff's numbers from
  build/<VERSION>/report.json, cached per commit in build/dashboard_reports.json. The hook records
  HEAD's report; tools/dashboard/backfill_history.py rebuilds older commits. A commit with no cached
  report carries the previous commit's matched_code forward (or its linked_code, if that is higher)
  and has "report": false.

Each record also has complete_code / complete_data: the report's values when cached, otherwise the
linked_* sums above.

    python tools/dashboard/refresh_history.py

Used by backfill_history.py and pages.py seed (the public page's history; docs/infrastructure.md)
and by the local server. Commits are immutable, so the per-commit git reads are cached in the history file itself and only
new commits are read.
"""
import json, os, re, subprocess
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
VERSION = 'GW4E69'
OUT = os.path.join(ROOT, 'build', 'dashboard_history.json')
REPORTS = os.path.join(ROOT, 'build', 'dashboard_reports.json')
REPORT = os.path.join(ROOT, 'build', VERSION, 'report.json')
CODE_SECTIONS = ('.text', '.init')
MEASURES = ('matched_code', 'complete_code', 'matched_data', 'complete_data')


def git(*a):
    return subprocess.run(['git', '--no-pager', *a], cwd=ROOT, capture_output=True, text=True, stdin=subprocess.DEVNULL).stdout


def read_commit(h):
    """linked_code, linked_data and named at commit h, from configure.py, splits.txt and symbols.txt."""
    units = set(re.findall(r'Object\(Matching, "([^"]+)"\)', git('show', f'{h}:configure.py')))
    code = data = 0
    cur = None
    for s in git('show', f'{h}:config/{VERSION}/splits.txt').splitlines():
        m = re.match(r'^(\S.*?):\s*$', s)
        if m:
            cur = m.group(1)
            continue
        m = re.match(r'\s+(\.\w+)\s+start:0x([0-9A-Fa-f]+)\s+end:0x([0-9A-Fa-f]+)', s)
        if m and cur in units:
            size = int(m.group(3), 16) - int(m.group(2), 16)
            if m.group(1) in CODE_SECTIONS:
                code += size
            else:
                data += size
    syms = git('show', f'{h}:config/{VERSION}/symbols.txt')
    named = sum(1 for l in syms.splitlines() if re.match(r'^(?!fn_)\S+ = \.(?:text|init):0x[0-9A-Fa-f]+; // type:function', l))
    return {'linked_code': code, 'linked_data': data, 'named': named}


def cached_report(v):
    """A dashboard_reports.json entry: a plain int (older entries: matched_code only) or a dict."""
    if isinstance(v, dict):
        return v
    return {'matched_code': v} if isinstance(v, int) else None


try:
    reports = json.load(open(REPORTS))
except Exception:
    reports = {}
try:
    # Earlier output: reuse the git reads of commits already seen (records with linked_data).
    seen = {r['commit']: r for r in json.load(open(OUT)) if 'linked_data' in r}
except Exception:
    seen = {}

head = git('rev-parse', 'HEAD').strip()
if head and os.path.exists(REPORT) and os.environ.get('DASHBOARD_NO_HEAD_REPORT') is None:
    try:
        m = json.load(open(REPORT))['measures']
        reports[head] = {k: int(m.get(k, 0)) for k in MEASURES}
    except Exception:
        pass

out = []
for line in git('log', '--reverse', '--format=%H|%ct|%s').splitlines():
    h, ts, subject = line.split('|', 2)
    old = seen.get(h[:7])
    fixed = {k: old[k] for k in ('linked_code', 'linked_data', 'named')} if old else read_commit(h)
    rec = {'commit': h[:7], 'time': int(ts), 'subject': subject, **fixed}
    rep = cached_report(reports.get(h))
    # No report for this commit (not built, or it did not build): carry the last real value forward.
    last = out[-1]['matched_code'] if out else 0
    rec['report'] = rep is not None
    rec['matched_code'] = rep['matched_code'] if rep else max(last, fixed['linked_code'])
    rec['complete_code'] = rep.get('complete_code', fixed['linked_code']) if rep else fixed['linked_code']
    rec['complete_data'] = rep.get('complete_data', fixed['linked_data']) if rep else fixed['linked_data']
    if rep and 'matched_data' in rep:
        rec['matched_data'] = rep['matched_data']
    out.append(rec)

if not out:
    raise SystemExit('refresh_history: git log returned nothing, history left as it was')
os.makedirs(os.path.dirname(OUT), exist_ok=True)
json.dump(reports, open(REPORTS, 'w'), indent=0)
json.dump(out, open(OUT, 'w'), indent=0)
print(f'wrote {OUT}: {len(out)} commits, latest {out[-1]["matched_code"]:,} matched, '
      f'{out[-1]["complete_code"]:,} linked code bytes')
