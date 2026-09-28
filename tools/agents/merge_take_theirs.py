# take_theirs.py <conflict file> <dest>: resolves every diff3 conflict with the lane's (theirs) side.
import sys
L = open(sys.argv[1]).read().split('\n'); out = []; st = 0
for l in L:
    if st == 0 and l.startswith('<<<<<<< '): st = 1; continue
    if st == 1 and l.startswith('||||||| '): st = 2; continue
    if st in (1, 2) and l == '=======': st = 3; continue
    if st == 3 and l.startswith('>>>>>>> '): st = 0; continue
    if st in (0, 3): out.append(l)
open(sys.argv[2], 'w').write('\n'.join(out))
