#!/usr/bin/env python3
"""
Name static (local) SDK functions whose names are shared across files (OnReset, AlarmHandler...),
which match_sdk_names.py skipped. For each partially matched unit, the unmatched functions are
compared by bytes (relocation-masked) at the exact addresses implied by the unit's layout.
Applies scope:local renames to symbols.txt.
"""
import struct, re, json, glob, collections
ROOT = 'C:/dev/tw2004'; V = 'GW4E69'; OBJ = 'C:/dev/prime/build/GM8E01_00/obj'; IMP = ROOT + '/build/sdk_import'
src = open(ROOT + '/tools/research/match_sdk_names.py').read()
exec(src[src.index('def parse(p):'):src.index('# TW\n')])                   # parse(): (name, code, mask, calls, bind)
td = open(ROOT + '/orig/GW4E69/sys/main.dol', 'rb').read() if False else open('C:/dev/scratch/tw/main.dol', 'rb').read()
toffs = struct.unpack('>7I', td[0:0x1C]); tad = struct.unpack('>7I', td[0x48:0x64]); tsz = struct.unpack('>7I', td[0x90:0xAC])
def tw(addr, size):
    for o, a, s in zip(toffs, tad, tsz):
        if s and a <= addr and addr + size <= a + s: return td[o + addr - a:o + addr - a + size]
sp = ROOT + f'/config/{V}/symbols.txt'; text = open(sp, newline='').read(); nl = '\r\n' if '\r\n' in text else '\n'
twf = {}
for l in text.split(nl):
    m = re.match(r'^(\S+) = \.(text|init):0x([0-9A-Fa-f]+); // type:function size:0x([0-9A-Fa-f]+)', l)
    if m: twf[int(m.group(3), 16)] = (m.group(1), int(m.group(4), 16))
units = json.load(open(IMP + '/sdk_units.json'))
apply = lambda b, m: bytes(x & y for x, y in zip(b, m))
ren = {}
for u in units:
    if u['status'] != 'CONTIG': continue
    funcs = parse(OBJ + '/' + u['unit'])
    # layout: Prime object function order == binary order; walk TW functions in [lo,hi) and beyond
    # anchor: first matched function
    names = [f[0] for f in funcs]
    known = [(a, n) for a, (n, s) in twf.items() if n in names and u['lo'] <= a < u['hi']]
    if not known: continue
    a0, n0 = min(known); i0 = names.index(n0)
    # walk forward and backward from the anchor through consecutive TW functions
    order = sorted(twf)
    pos = order.index(a0)
    for direction, rng in ((1, range(i0 + 1, len(funcs))), (-1, range(i0 - 1, -1, -1))):
        p = pos
        for i in rng:
            p += direction
            if p < 0 or p >= len(order): break
            a = order[p]; n, s = twf[a]; name, code, mask, calls, bind = funcs[i]
            if s != len(code): break
            if n == name: continue
            if not n.startswith('fn_'): break
            if apply(tw(a, s), mask) == apply(code, mask): ren[a] = (name, bind)
            else: break
print(len(ren), 'byte-verified local functions to name:', sorted(set(v[0] for v in ren.values())))
lines = text.split(nl); n = 0
for i, l in enumerate(lines):
    m = re.match(r'^(fn_[0-9A-F]{8}) = (\.(?:text|init):0x([0-9A-Fa-f]+); // .*)$', l)
    if m and int(m.group(3), 16) in ren:
        name, bind = ren[int(m.group(3), 16)]; rest = re.sub(r' scope:\w+', '', m.group(2))
        lines[i] = f'{name} = {rest} scope:{"local" if bind == 0 else "global"}'; n += 1
open(sp, 'w', newline='').write(nl.join(lines)); print('applied', n)
