#!/usr/bin/env python3
"""
Add every fully-matched SDK unit that is not yet in the build to splits.txt and configure.py.

    python tools/research/sdk/add_units.py          # after sdk_units.py / name_data.py / sdk_data_units.py

Code-only units come from build/sdk_import/sdk_units.json (status FULL, no data); data-carrying
units from sdk_data_ok.json. Data range ends are padded to the next symbol in that section.
Overlapping data ranges are resolved by keeping the first owner in link order.
"""
import json, re, collections, os
ROOT = 'C:/dev/tw2004'; V = 'GW4E69'; IMP = ROOT + '/build/sdk_import'
units = json.load(open(IMP + '/sdk_units.json')); ok = json.load(open(IMP + '/sdk_data_ok.json'))
cands = {}
for u in units:
    if u['status'] == 'FULL' and not u['data']: cands[u['unit']] = {'lo': u['lo'], 'hi': u['hi'], 'data': {}}
for u in ok: cands[u['unit']] = {'lo': u['lo'], 'hi': u['hi'], 'data': u['data']}
sp = ROOT + f'/config/{V}/splits.txt'; s = open(sp, newline='').read(); nl = '\r\n' if '\r\n' in s else '\n'
have = set(re.findall(r'^(\S.*?):\s*$', s, re.M))
syms = collections.defaultdict(list)
for line in open(ROOT + f'/config/{V}/symbols.txt'):
    m = re.match(r'^(\S+) = \.(\w+):0x([0-9A-Fa-f]+); // type:(\w+) size:0x([0-9A-Fa-f]+)', line)
    if m: syms[m.group(2)].append((int(m.group(3), 16), int(m.group(5), 16)))
for k in syms: syms[k].sort()
def next_start(sec, addr):
    for a, sz in syms[sec]:
        if a >= addr: return a
    return addr
add = ''; groups = collections.OrderedDict(); n = 0
for rel, u in sorted(cands.items(), key=lambda kv: kv[1]['lo']):
    src = rel[:-2] + '.c'
    if src in have: continue
    parts = rel.split('/')
    lib = 'MSL_C.PPCEABI.bare.H' if parts[0] == 'runtime' else (parts[1] if len(parts) == 3 else parts[1][:-2])
    groups.setdefault(lib, []).append(src); n += 1
    sec = '.init' if u['lo'] < 0x80005520 else '.text'
    add += nl + src + ':' + nl + f'\t{sec:<11} start:0x{u["lo"]:08X} end:0x{u["hi"]:08X}' + nl
    for dsec, (lo, hi, cnt) in u['data'].items():
        add += f'\t.{dsec:<10} start:0x{lo:08X} end:0x{next_start(dsec, hi):08X}' + nl
open(sp, 'w', newline='').write(s.rstrip('\r\n') + nl + add)
# overlaps
s = open(sp, newline='').read(); lines = s.split(nl); cur = None; seen = collections.defaultdict(list); drop = set(); order = {}
for i, l in enumerate(lines):
    m = re.match(r'^(\S.*?):\s*$', l)
    if m: cur = m.group(1); continue
    m = re.match(r'\s+\.(\w+)\s+start:0x([0-9A-Fa-f]+)\s+end:0x([0-9A-Fa-f]+)', l)
    if m and cur and cur != 'Sections':
        sec, a, b = m.group(1), int(m.group(2), 16), int(m.group(3), 16)
        if sec in ('text', 'init'): order[cur] = a
        for a2, b2, u2, i2 in seen[sec]:
            if a < b2 and a2 < b:
                loser = i if order.get(cur, 0) > order.get(u2, 0) else i2
                drop.add(loser); print(f'overlap .{sec}: {cur} vs {u2} -> dropped from {cur if loser == i else u2}')
        seen[sec].append((a, b, cur, i))
open(sp, 'w', newline='').write(nl.join(l for i, l in enumerate(lines) if i not in drop))
cp = ROOT + '/configure.py'; c = open(cp, newline='').read()
for lib, srcs in groups.items():
    key = f'        "lib": "{lib}",\n'
    objs = ''.join(f'            Object(Matching, "{x}"),\n' for x in srcs)
    if key in c:
        i = c.index(key); j = c.index('        "objects": [\n', i) + len('        "objects": [\n'); c = c[:j] + objs + c[j:]
    else:
        rt = lib.startswith('MSL')
        c = c.replace('config.libs = [\n', 'config.libs = [\n' + f'''    {{
        "lib": "{lib}",
        "mw_version": "{'GC/1.3' if rt else 'GC/1.2.5n'}",
        "cflags": {'cflags_runtime' if rt else 'cflags_sdk'},
        "progress_category": "sdk",
        "src_dir": "extern/sdk",
        "objects": [
''' + objs + '''        ],
    },
''', 1)
open(cp, 'w', newline='').write(c)
print(f'added {n} units:', {k: len(v) for k, v in groups.items()})
