"""Create a NonMatching unit over [lo, hi), folding in the sweep units inside it.

    python tools/match/mkunit.py GameManager.c 0x800DCA94 0x800DFC18 [after=Gimme.c]
Writes splits.txt / configure.py edits and prints the sweep sources to merge (it does not delete
their files; that is done after the merged unit is verified).
"""
import re, sys, pathlib
ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
name, lo, hi = sys.argv[1], int(sys.argv[2], 16), int(sys.argv[3], 16)
after = sys.argv[4] if len(sys.argv) > 4 else 'Gimme.c'

sp = (ROOT / 'config/GW4E69/splits.txt').read_text(encoding='utf-8')
blocks = re.split(r'\n(?=\S[^\n]*:\n)', sp)
out, absorbed, inserted = [], [], False
for b in blocks:
    m = re.match(r'(\S[^\n]*):\n', b)
    t = re.search(r'\.text\s+start:0x([0-9A-F]+) end:0x([0-9A-F]+)', b)
    if m and t and m.group(1) != name:
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
(ROOT / 'config/GW4E69/splits.txt').write_text('\n'.join(out), encoding='utf-8', newline='\n')

cp = (ROOT / 'configure.py').read_text(encoding='utf-8')
for u in absorbed:
    cp, n = re.subn(r'\n[ \t]*Object\(\w+, "%s"\),' % re.escape(u), '', cp)
    assert n == 1, u
cp, n = re.subn(r'(\n([ \t]*)Object\(\w+, "%s"\),)' % re.escape(after),
                r'\1\n\2Object(NonMatching, "%s"),' % name, cp)
assert n == 1
(ROOT / 'configure.py').write_text(cp, encoding='utf-8', newline='\n')
print('absorbed', absorbed)
