# merge_pick.py <conflict file> <dest> <choices>: resolves the diff3 conflicts in order, one letter
# each: o = ours (main), t = theirs (the lane). E.g. `ooott`.
import sys
L = open(sys.argv[1]).read().split('\n'); pick = sys.argv[3]; out = []; st = 0; k = -1; ours = []; theirs = []
for l in L:
    if st == 0 and l.startswith('<<<<<<< '): st = 1; k += 1; ours, theirs = [], []; continue
    if st == 1 and l.startswith('||||||| '): st = 2; continue
    if st in (1, 2) and l == '=======': st = 3; continue
    if st == 3 and l.startswith('>>>>>>> '):
        out += ours if pick[k] == 'o' else theirs; st = 0; continue
    if st == 1: ours.append(l)
    elif st == 3: theirs.append(l)
    elif st == 0: out.append(l)
assert k + 1 == len(pick), f'{k + 1} conflicts, {len(pick)} choices'
open(sys.argv[2], 'w').write('\n'.join(out))
