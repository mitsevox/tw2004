"""Walk an animation library (SAL object, little-endian) the way fn_80025640 does and list the
clips for a group across every style / x / y key.
    python sal_dump.py <SAL.bin> <group> [group ...]
Layout (from the loader fn_80025F38 and its swap tables):
  0x000 s32 group[64]   byte offset of each group's node in the tree, -1 none
  0x10C s32 default leaf; 0x110 tree bytes; 0x114 index count; 0x118 record count
  0x148 s16 index[count] -> record; align 16; record[n] 0x24 (char name[16], ...); tree
Tree nodes (s16): group node: [0] default leaf, [1+style] style node (8 styles)
  style node: [x] x node (6 values)
  x node: [0]?, [1] default leaf, [2+y] leaf (11 values)
  leaf: [0] clip count, [1] first index, then a u32 used-mask"""
import struct, sys, collections
d = open(sys.argv[1], 'rb').read()
def i32(o): return struct.unpack_from('<i', d, o)[0]
def i16(o): return struct.unpack_from('<h', d, o)[0]
groups = [i32(4 * g) for g in range(64)]
dflt, tsize, nidx, nrec = i32(0x10C), i32(0x110), i32(0x114), i32(0x118)
idx = [i16(0x148 + 2 * k) for k in range(nidx)]
rec0 = (0x148 + 2 * nidx + 15) & ~15
names = [d[rec0 + 0x24 * r: rec0 + 0x24 * r + 16].split(b'\0')[0].decode('latin1') for r in range(nrec)]
tree = rec0 + 0x24 * nrec
assert tree + tsize == len(d), (hex(tree), tsize, len(d))
def leaf(off):
    n, first = i16(tree + off), i16(tree + off + 2)
    return [names[idx[first + k]] for k in range(n)]
def lookup(g, style, x, y):
    """returns (leaf offset, how) like fn_80025640"""
    node = groups[g]
    if node < 0: return None, 'no group'
    s = i16(tree + node + 2 + style * 2)
    if s < 0 and style != 0: s = i16(tree + node + 2)
    if s < 0:
        l = i16(tree + node)
        return (l, 'group default') if l >= 0 else ((dflt, 'lib default') if dflt >= 0 else (None, 'none'))
    xo = i16(tree + s + 2 * x)
    if xo < 0:
        l = i16(tree + node)
        return (l, 'group default') if l >= 0 else ((dflt, 'lib default') if dflt >= 0 else (None, 'none'))
    l = i16(tree + xo + 4 + 2 * y)
    if l >= 0: return l, 'exact'
    for alt, how in ((i16(tree + xo + 2), 'x default'), (i16(tree + node), 'group default'), (dflt, 'lib default')):
        if alt >= 0: return alt, how
    return None, 'none'
print(f'{nrec} clips, {nidx} index entries, tree {tsize} bytes, groups {[g for g in range(64) if groups[g] >= 0]}')
for g in map(int, sys.argv[2:]):
    print(f'== group {g}')
    seen = collections.OrderedDict()
    for style in range(8):
        for x in range(6):
            for y in range(11):
                l, how = lookup(g, style, x, y)
                if l is None: continue
                seen.setdefault((style, l, how), []).append((x, y))
    for (style, l, how), keys in seen.items():
        xs = sorted({k[0] for k in keys}); ys = sorted({k[1] for k in keys})
        print(f'  style {style}  x{xs} y{ys} [{how}]  -> {leaf(l)}')
