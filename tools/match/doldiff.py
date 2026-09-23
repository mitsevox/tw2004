"""Compare two DOLs section by section and print the first differing address ranges.
    python tools/match/doldiff.py orig/GW4E69/sys/main.dol build/GW4E69/main.dol [--all | --max N]
Shows 20 differences per section by default; --max N changes the limit, --all shows every one."""
import struct, sys
args, limit, rest = [], 20, sys.argv[1:]
while rest:
    x = rest.pop(0)
    if x == '--all': limit = None
    elif x == '--max' and rest: limit = int(rest.pop(0))
    else: args.append(x)
if len(args) != 2: sys.exit(__doc__)
def load(p):
    d = open(p, 'rb').read()
    off = struct.unpack('>18I', d[0:72]); addr = struct.unpack('>18I', d[72:144]); size = struct.unpack('>18I', d[144:216])
    return d, [(off[i], addr[i], size[i]) for i in range(18) if size[i]]
a, sa = load(args[0]); b, sb = load(args[1])
for (oa, aa, za), (ob, ab, zb) in zip(sa, sb):
    if (aa, za) != (ab, zb): print('section layout differs', hex(aa), hex(za), hex(ab), hex(zb))
    n = 0; i = 0; m = min(za, zb)
    while i < m and (limit is None or n < limit):
        if a[oa + i] != b[ob + i]:
            j = i
            while j < m and a[oa + j] != b[ob + j]: j += 1
            print('diff %08X-%08X  orig %s  ours %s' % (aa + i, aa + j, a[oa+i:oa+min(j,i+16)].hex(), b[ob+i:ob+min(j,i+16)].hex()))
            n += 1; i = j
        else: i += 1
    if limit is not None and n == limit and i < m:
        print('(section %08X: stopped at %d differences; --all shows the rest)' % (aa, limit))
