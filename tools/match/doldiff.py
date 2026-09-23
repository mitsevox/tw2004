"""Compare two DOLs section by section and print the first differing address ranges.
    python tools/match/doldiff.py orig/GW4E69/sys/main.dol build/GW4E69/main.dol"""
import struct, sys
def load(p):
    d = open(p, 'rb').read()
    off = struct.unpack('>18I', d[0:72]); addr = struct.unpack('>18I', d[72:144]); size = struct.unpack('>18I', d[144:216])
    return d, [(off[i], addr[i], size[i]) for i in range(18) if size[i]]
a, sa = load(sys.argv[1]); b, sb = load(sys.argv[2])
for (oa, aa, za), (ob, ab, zb) in zip(sa, sb):
    if (aa, za) != (ab, zb): print('section layout differs', hex(aa), hex(za), hex(ab), hex(zb))
    n = 0; i = 0; m = min(za, zb)
    while i < m and n < 20:
        if a[oa + i] != b[ob + i]:
            j = i
            while j < m and a[oa + j] != b[ob + j]: j += 1
            print('diff %08X-%08X  orig %s  ours %s' % (aa + i, aa + j, a[oa+i:oa+min(j,i+16)].hex(), b[ob+i:ob+min(j,i+16)].hex()))
            n += 1; i = j
        else: i += 1
