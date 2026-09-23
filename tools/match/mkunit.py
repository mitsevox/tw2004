"""Create a NonMatching unit over [lo, hi), folding in the sweep units inside it; or widen one.

    python tools/match/mkunit.py GameManager.c 0x800DCA94 0x800DFC18 [after=Gimme.c]
    python tools/match/mkunit.py --extend uiTransform.c 0x80092CE8 0x80093560
Writes splits.txt / configure.py edits and prints the sweep sources to merge (it does not delete
their files; that is done after the merged unit is verified, by fold.py).

--extend widens an existing unit's .text range to cover [lo, hi) as well (its other sections and
its configure.py line stay as they are). Only sweep units may lie in the added range, whole; the
new range must touch or overlap the old one. Then fold the printed sweeps in with fold.py, which
appends their code to the unit's source instead of replacing it.
"""
import re, sys, pathlib
ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
SPLITS = ROOT / 'config/GW4E69/splits.txt'
TEXT = re.compile(r'\.text\s+start:0x([0-9A-F]+) end:0x([0-9A-F]+)')


def blocks():
    return re.split(r'\n(?=\S[^\n]*:\n)', SPLITS.read_text(encoding='utf-8'))


def extend(name, lo, hi):
    bl = blocks()
    mine = [b for b in bl if b.startswith(name + ':\n')]
    if len(mine) != 1:
        sys.exit('%s: no unit of that name in splits.txt' % name)
    t = TEXT.search(mine[0])
    if not t:
        sys.exit('%s has no .text range' % name)
    old_lo, old_hi = int(t.group(1), 16), int(t.group(2), 16)
    if hi < old_lo or lo > old_hi:
        sys.exit('%x-%x does not touch %s (%x-%x)' % (lo, hi, name, old_lo, old_hi))
    new_lo, new_hi = min(lo, old_lo), max(hi, old_hi)
    out, absorbed = [], []
    for b in bl:
        m = re.match(r'(\S[^\n]*):\n', b)
        t = TEXT.search(b)
        if m and m.group(1) == name:
            b = TEXT.sub('.text       start:0x%08X end:0x%08X' % (new_lo, new_hi), b, count=1)
        elif m and t:
            s, e = int(t.group(1), 16), int(t.group(2), 16)
            if s < new_hi and e > new_lo:
                if not (new_lo <= s and e <= new_hi) or b.count('start:') != 1 \
                        or not m.group(1).startswith('unsorted/sweep_'):
                    sys.exit('%x-%x would take in %s (%x-%x): only whole sweep units can be absorbed'
                             % (new_lo, new_hi, m.group(1), s, e))
                absorbed.append(m.group(1))
                continue
        out.append(b)
    SPLITS.write_text('\n'.join(out), encoding='utf-8', newline='\n')
    cp = (ROOT / 'configure.py').read_text(encoding='utf-8')
    for u in absorbed:
        cp, n = re.subn(r'\n[ \t]*Object\(\w+, "%s"\),' % re.escape(u), '', cp)
        assert n == 1, u
    (ROOT / 'configure.py').write_text(cp, encoding='utf-8', newline='\n')
    print('%s: .text 0x%08X-0x%08X (was 0x%08X-0x%08X)' % (name, new_lo, new_hi, old_lo, old_hi))
    print('absorbed', absorbed)


def create(name, lo, hi, after):
    bl = blocks()
    if any(b.startswith(name + ':\n') for b in bl):
        sys.exit('%s is already a unit: use --extend to widen it' % name)
    out, absorbed, inserted = [], [], False
    for b in bl:
        m = re.match(r'(\S[^\n]*):\n', b)
        t = TEXT.search(b)
        if m and t:
            s, e = int(t.group(1), 16), int(t.group(2), 16)
            if lo <= s and e <= hi:
                assert b.count('start:') == 1, 'unit has data too: ' + m.group(1)
                absorbed.append(m.group(1))
                if not inserted:
                    out.append('%s:\n\t.text       start:0x%08X end:0x%08X\n' % (name, lo, hi))
                    inserted = True
                continue
            if s < hi and e > lo:
                sys.exit('overlaps partially: %s %x-%x' % (m.group(1), s, e))
            if not inserted and s >= hi:
                out.append('%s:\n\t.text       start:0x%08X end:0x%08X\n' % (name, lo, hi))
                inserted = True
        out.append(b)
    SPLITS.write_text('\n'.join(out), encoding='utf-8', newline='\n')

    cp = (ROOT / 'configure.py').read_text(encoding='utf-8')
    for u in absorbed:
        cp, n = re.subn(r'\n[ \t]*Object\(\w+, "%s"\),' % re.escape(u), '', cp)
        assert n == 1, u
    cp, n = re.subn(r'(\n([ \t]*)Object\(\w+, "%s"\),)' % re.escape(after),
                    r'\1\n\2Object(NonMatching, "%s"),' % name, cp)
    assert n == 1
    (ROOT / 'configure.py').write_text(cp, encoding='utf-8', newline='\n')
    print('absorbed', absorbed)


if __name__ == '__main__':
    if sys.argv[1] == '--extend':
        extend(sys.argv[2], int(sys.argv[3], 16), int(sys.argv[4], 16))
    else:
        create(sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16),
               sys.argv[4] if len(sys.argv) > 4 else 'Gimme.c')
