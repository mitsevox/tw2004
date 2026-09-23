"""Merge sweep files into one unit source: includes, then declarations, then bodies.
    python tools/match/merge_sweeps.py <Name>.c <sweep1.c> ...   (paths relative to src/)"""
import re, sys, pathlib
ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
files = sys.argv[2:]
out_name = sys.argv[1]
decls, bodies, includes = [], [], []
for f in files:
    s = (ROOT / 'src' / f).read_text(encoding='utf-8')
    lines = s.split('\n')
    i = 0
    cur = []
    depth = 0
    for ln in lines:
        if depth == 0:
            if ln.startswith('//') or ln.strip() == '':
                if cur: cur.append(ln)
                continue
            if ln.startswith('#include'):
                if ln not in includes: includes.append(ln)
                continue
            if ln.rstrip().endswith(';') and '{' not in ln:
                if ln not in decls: decls.append(ln)
                continue
        cur.append(ln)
        depth += ln.count('{') - ln.count('}')
        if depth == 0 and ln.startswith('}'):
            bodies.append('\n'.join(cur).strip('\n'))
            cur = []
    assert depth == 0 and not ''.join(cur).strip(), f
text = '\n'.join(includes) + '\n\n' + '\n'.join(decls) + '\n\n' + '\n\n'.join(bodies) + '\n'
(ROOT / 'src' / out_name).write_text(text, encoding='utf-8', newline='\n')
print(len(decls), 'decls', len(bodies), 'bodies')
