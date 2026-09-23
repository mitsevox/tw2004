"""Improve a function's declaration order by moving one declaration line at a time.

    python tools/match/declclimb.py <Unit> <fn> <number of declaration lines>

For functions with too many declarations for declperm.py (N! orders). Starting from the current
order, it tries moving each of the first N lines after the function's opening line to every other
position, keeps the first move that raises the score, and repeats until no single move helps or
the function is exact. The best order found is left in src/<Unit>.c. It finds a local best, not
the global one; run declperm.py on a few of the lines afterwards if needed.
"""
import pathlib, re, sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from trial import Trial

unit, fn, n = sys.argv[1], sys.argv[2], int(sys.argv[3])
t = Trial(unit)
base = t.base
m = re.search(r'^[A-Za-z_][^\n;]*\b' + re.escape(fn) + r'\([^;{]*\)\s*\{\n', base, re.M)
start = m.end()
lines = base[start:].split('\n')
decl, rest = lines[:n], '\n'.join(lines[n:])


def build(d):
    return base[:start] + '\n'.join(d) + '\n' + rest


def score(d):
    t._write(build(d))
    s = t.score(fn)
    try:
        return float(s)
    except ValueError:
        return -1.0


cur = score(decl)
print('start', cur, flush=True)
improved = True
while improved and cur < 100:
    improved = False
    for a in range(n):
        for c in range(n):
            if a == c:
                continue
            d = list(decl)
            d.insert(c, d.pop(a))
            s = score(d)
            if s > cur:
                cur, decl, improved = s, d, True
                print('better', cur, decl[c].strip(), '-> line', c + 1, flush=True)
                break
        if improved:
            break
t._write(build(decl))
print(fn, 'final', cur)
print('\n'.join(decl))
