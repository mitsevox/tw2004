#!/usr/bin/env python3
"""
Add SDK/MSL units from extern/sdk to splits.txt and configure.py as NonMatching (SDK Level 1).
    python tools/research/sdk/add_units_nonmatching.py <spec.json>
spec.json: [{"src": "dolphin/PPCArch.c", "lib": "os", "sections": {"text": ["801393D0", "801394C0"]}}, ...]
Section bounds are hex strings (end exclusive). A lib that is not in configure.py yet gets a new
SDK block (GC/1.2.5n + cflags_sdk, or GC/1.3 + cflags_runtime for MSL/Runtime libs).
"""
import json, re, sys
ROOT = 'C:/dev/tw2004'; V = 'GW4E69'
spec = json.load(open(sys.argv[1]))
sp = ROOT + f'/config/{V}/splits.txt'; s = open(sp, newline='').read(); nl = '\r\n' if '\r\n' in s else '\n'
have = set(re.findall(r'^(\S.*?):\s*$', s, re.M))
ranges = []; cur = None
for l in s.split(nl):
    m = re.match(r'^(\S.*?):\s*$', l)
    if m: cur = m.group(1); continue
    m = re.match(r'\s+\.(\w+)\s+start:0x([0-9A-Fa-f]+)\s+end:0x([0-9A-Fa-f]+)', l)
    if m and cur and cur != 'Sections': ranges.append((m.group(1), int(m.group(2), 16), int(m.group(3), 16), cur))
add = ''; libs = {}
for u in spec:
    src = u['src']
    if src in have: print('already present:', src); continue
    secs = [(k, int(a, 16), int(b, 16)) for k, (a, b) in u['sections'].items()]
    for sec, lo, hi in secs:
        for sec2, a, b, owner in ranges:
            if sec2 == sec and lo < b and a < hi:
                sys.exit(f'OVERLAP .{sec} {src} [{lo:#x},{hi:#x}) with {owner} [{a:#x},{b:#x})')
    add += nl + src + ':' + nl
    for sec, lo, hi in secs:
        add += f'\t.{sec:<11}start:0x{lo:08X} end:0x{hi:08X}' + nl
    libs.setdefault(u['lib'], []).append(src)
open(sp, 'w', newline='').write(s.rstrip('\r\n') + nl + add)
cp = ROOT + '/configure.py'; c = open(cp, newline='').read(); cnl = '\r\n' if '\r\n' in c else '\n'
for lib, srcs in libs.items():
    key = f'        "lib": "{lib}",{cnl}'
    objs = ''.join(f'            Object(NonMatching, "{x}"),{cnl}' for x in srcs)
    if key in c:
        i = c.index(key); j = c.index(f'        "objects": [{cnl}', i) + len(f'        "objects": [{cnl}'); c = c[:j] + objs + c[j:]
    else:
        rt = lib.startswith('MSL') or lib.startswith('Runtime')
        block = (f'    {{{cnl}        "lib": "{lib}",{cnl}        "mw_version": "{"GC/1.3" if rt else "GC/1.2.5n"}",{cnl}'
                 f'        "cflags": {"cflags_runtime" if rt else "cflags_sdk"},{cnl}        "progress_category": "sdk",{cnl}'
                 f'        "src_dir": "extern/sdk",{cnl}        "objects": [{cnl}' + objs + f'        ],{cnl}    }},{cnl}')
        anchor = f'config.libs = [{cnl}'; assert anchor in c
        c = c.replace(anchor, anchor + block, 1)
open(cp, 'w', newline='').write(c)
print('added:', {k: len(v) for k, v in libs.items()})
