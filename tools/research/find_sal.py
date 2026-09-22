"""Find an embedded animation library (SAL layout) inside a blob and write it out as its own file.
    python find_sal.py <blob> <out.bin>"""
import struct, sys
d = open(sys.argv[1], 'rb').read()
def i32(o): return struct.unpack_from('<i', d, o)[0]
best = None
cands = []
for p in range(0, len(d) - 0x148, 4):
    tsize, nidx, nrec = i32(p + 0x110), i32(p + 0x114), i32(p + 0x118)
    if not (0 < nrec < 2000 and 0 < nidx < 4000 and 0 < tsize < 200000): continue
    rec0 = p + ((0x148 + 2 * nidx + 15) & ~15)
    end = rec0 + 0x24 * nrec + tsize
    if end > len(d): continue
    name = d[rec0:rec0 + 16].split(b'\0')[0]
    if not (4 <= len(name) <= 15 and name.isascii() and name[:1].isalpha()): continue
    g = [i32(p + 4 * k) for k in range(21)]
    if not all(x == -1 or 0 <= x < tsize for x in g): continue
    tree = rec0 + 0x24 * nrec
    ok = True
    for x in g:
        if x >= 0 and not (0 <= struct.unpack_from('<h', d, tree + x)[0] < tsize or struct.unpack_from('<h', d, tree + x)[0] == -1): ok = False
    if not ok: continue
    cands.append((p, end))
if not cands: sys.exit('no library found')
p, end = cands[-1] if len(sys.argv) > 3 else cands[0]
print('candidates', [hex(c[0]) for c in cands])
open(sys.argv[2], 'wb').write(d[p:end])
print(f'library at {p:#x}..{end:#x} ({end - p} bytes), {i32(p + 0x118)} clips')
