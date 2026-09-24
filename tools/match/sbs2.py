"""Normalised objdiff instruction diff: pool/label symbols are masked and branch targets are shown
as offsets from the function start (+0x..), so only real differences show, including a branch that
goes somewhere else.   python tools/match/sbs2.py main/<Unit> <fn> [sbs]   (sbs = side-by-side)"""
import json, pathlib, subprocess, sys, difflib, re
ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
from hosttools import OBJDIFF_CLI                    # noqa: E402
unit, fn = sys.argv[1], sys.argv[2]; mode = sys.argv[3] if len(sys.argv) > 3 else 'diff'
d = json.loads(subprocess.run([OBJDIFF_CLI, 'diff', '-p', str(ROOT), '-u', unit, fn, '-o', '-', '--format', 'json'],
                              capture_output=True, text=True).stdout)
def norm(ins, start):
    s = ins.get('formatted', '?')
    if 'branch_dest' in ins:     # a local branch: its target relative to the function start
        s = re.sub(r'0x[0-9a-f]+$', '+0x%x' % (int(ins['branch_dest']) - start), s)
    s = re.sub(r'(@\d+|lbl_[0-9A-Fa-f]+|\w+)@(sda21|ha|l)', r'SYM@\2', s)
    return s
def get(side):
    for s in d[side]['symbols']:
        if s['name'] == fn:
            start = int(s.get('address', 0))
            return [norm(i.get('instruction', {}), start) for i in s['instructions']]
    return []
L, R = get('left'), get('right')
if mode == 'sbs':
    for i in range(max(len(L), len(R))):
        l = L[i] if i < len(L) else ''; r = R[i] if i < len(R) else ''
        print(str(i).rjust(3), ('  ' if l == r else '* ') + l.ljust(34), r)
else:
    for tag, i1, i2, j1, j2 in difflib.SequenceMatcher(None, L, R, autojunk=False).get_opcodes():
        if tag == 'equal': continue
        print(f'{tag} @{i1}: orig={L[i1:i2][:12]}\n        ours={R[j1:j2][:12]}')
print(len(L), len(R), 'instructions')
