#!/usr/bin/env python3
"""
Derive data ranges for SDK units whose data is only partly named, using the other project's object
layout: a unit's data section keeps the same symbol order and sizes, so one known symbol anchors
the whole section. Writes the results into build/sdk_import/sdk_data_ok.json (extending it).
"""
import struct, re, json, collections, os
ROOT = 'C:/dev/tw2004'; V = 'GW4E69'; IMP = ROOT + '/build/sdk_import'; OBJ = 'C:/dev/prime/build/GM8E01_00/obj'
exec(open(ROOT + '/tools/research/sdk/sdk_data_units.py').read().split("units=json.load")[0])   # parse_syms, tw, etc.
units = json.load(open(IMP + '/sdk_units.json')); ok = json.load(open(IMP + '/sdk_data_ok.json')); done = {u['unit'] for u in ok}
anon = json.load(open(IMP + '/anon_map.json')); anon_by_unit = collections.defaultdict(dict)
for lbl, (rel, n) in anon.items(): anon_by_unit[rel][n] = lbl
twaddr = {}
for line in open(ROOT + f'/config/{V}/symbols.txt'):
    m = re.match(r'^(\S+) = \.(\w+):0x([0-9A-Fa-f]+); // type:(\w+) size:0x([0-9A-Fa-f]+)', line)
    if m: twaddr[m.group(1)] = (m.group(2), int(m.group(3), 16), int(m.group(5), 16))
def secsizes(p):
    d = open(p, 'rb').read(); shoff = struct.unpack('>I', d[0x20:0x24])[0]; n, idx = struct.unpack('>HH', d[0x30:0x34])
    sh = [struct.unpack('>10I', d[shoff + i * 40:shoff + i * 40 + 40]) for i in range(n)]; st = sh[idx]
    return {d[st[4] + s[0]:d.index(b'\0', st[4] + s[0])].decode(): s[5] for s in sh}
added = 0; renames = {}
for u in units:
    if u['status'] != 'FULL' or not u['data'] or u['unit'] in done: continue
    syms = parse_syms(OBJ + '/' + u['unit']); sizes = secsizes(OBJ + '/' + u['unit'])
    bysec = collections.defaultdict(list)
    for name, sec, val, sz, kind in syms:
        if kind == 1: bysec[sec].append((val, sz, name))
    ranges = {}; bad = None
    for sec, items in bysec.items():
        bases = set()
        for val, sz, name in items:
            lbl = anon_by_unit.get(u['unit'], {}).get(name) if name.startswith('@') else name
            t = twaddr.get(lbl) if lbl else None
            if t and t[0] == sec.lstrip('.'): bases.add(t[1] - val)
        if len(bases) != 1: bad = f'{sec}: {len(bases)} candidate bases'; break
        base = bases.pop(); ranges[sec.lstrip('.')] = (base, base + sizes[sec], len(items))
    if bad: print(f'   {u["unit"]:34} skip: {bad}'); continue
    ok.append({'unit': u['unit'], 'lo': u['lo'], 'hi': u['hi'], 'data': {k: list(v) for k, v in ranges.items()}}); added += 1
    # name the symbols this layout places (named ones only; lbl_ at the computed address -> Prime name)
    for sec, items in bysec.items():
        base = ranges[sec.lstrip('.')][0]
        for val, sz, name in items:
            if name.startswith('@') or not re.match(r'^[A-Za-z_]\w*$', name) or name in twaddr: continue
            lbl = 'lbl_%08X' % (base + val)
            if lbl in twaddr: renames[lbl] = name
    print(f'OK {u["unit"]:34}', {k: (hex(a), hex(b)) for k, (a, b, n) in ranges.items()})
json.dump(ok, open(IMP + '/sdk_data_ok.json', 'w'), indent=0); print(added, 'units added to sdk_data_ok.json')
sp = ROOT + f'/config/{V}/symbols.txt'; t = open(sp, newline='').read(); n = 0
for lbl, name in renames.items():
    t2 = re.sub(r'^' + lbl + r' = ', name + ' = ', t, count=1, flags=re.M)
    if t2 != t: t = t2; n += 1
open(sp, 'w', newline='').write(t); print('renamed', n, 'data symbols:', list(renames.values())[:8])
