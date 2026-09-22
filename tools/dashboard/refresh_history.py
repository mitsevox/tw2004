#!/usr/bin/env python3
"""
Write build/dashboard_history.json: matched code bytes and named-function count at every commit.

    python tools/dashboard/refresh_history.py

Installed as .git/hooks/post-commit by tools/dashboard/install_hook.py, so the dashboard's history
updates itself. Runs git here (from a shell that has a console) so the dashboard server never has to.
"""
import json, os, re, subprocess
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
VERSION = 'GW4E69'
OUT = os.path.join(ROOT, 'build', 'dashboard_history.json')

def git(*a):
    return subprocess.run(['git', '--no-pager', *a], cwd=ROOT, capture_output=True, text=True, stdin=subprocess.DEVNULL).stdout

out = []
for line in git('log', '--reverse', '--format=%H|%ct|%s').splitlines():
    h, ts, subject = line.split('|', 2)
    units = set(re.findall(r'Object\(Matching, "([^"]+)"\)', git('show', f'{h}:configure.py')))
    matched = 0; cur = None
    for s in git('show', f'{h}:config/{VERSION}/splits.txt').splitlines():
        m = re.match(r'^(\S.*?):\s*$', s)
        if m: cur = m.group(1); continue
        m = re.match(r'\s+\.(?:text|init)\s+start:0x([0-9A-Fa-f]+)\s+end:0x([0-9A-Fa-f]+)', s)
        if m and cur in units: matched += int(m.group(2), 16) - int(m.group(1), 16)
    syms = git('show', f'{h}:config/{VERSION}/symbols.txt')
    named = sum(1 for l in syms.splitlines() if re.match(r'^(?!fn_)\S+ = \.(?:text|init):0x[0-9A-Fa-f]+; // type:function', l))
    out.append({'commit': h[:7], 'time': int(ts), 'subject': subject, 'matched_code': matched, 'named': named})
os.makedirs(os.path.dirname(OUT), exist_ok=True)
json.dump(out, open(OUT, 'w'), indent=0)
print(f'wrote {OUT}: {len(out)} commits, latest {out[-1]["matched_code"]:,} bytes')
