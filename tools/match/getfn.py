"""python tools/match/getfn.py <m2c.c> fn1 [fn2 ...]: print just those function definitions, minus m2c's
temp/var declaration lines."""
import re, sys
src = open(sys.argv[1], encoding='utf-8').read()
for fn in sys.argv[2:]:
    m = re.search(r'^[^\n;]*\b' + re.escape(fn) + r'\([^;\n]*\) \{\n', src, re.M)
    if not m:
        print('// not found', fn)
        continue
    end = src.index('\n}\n', m.end()) + 3
    body = src[m.start():end]
    out = [l for l in body.split('\n')
           if not re.match(r'^    [\w?]+\*? \*?(temp|var|sp)\w*;$', l) and not re.match(r'^    \?\* ', l)]
    print('\n'.join(out))
