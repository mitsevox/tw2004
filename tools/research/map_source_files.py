#!/usr/bin/env python3
"""
Pin leaked source file names (assert __FILE__ strings) to the functions that reference them.

    python tools/research/map_source_files.py            # prints the table
    python tools/research/map_source_files.py --json out.json

Reads the dtk disassembly in build/GW4E69/asm. Data-section .string objects whose text ends in
.c or .h are collected, then every code reference (lbl_XXXXXXXX@ha/@l/@sda21) is attributed to
the enclosing function.
"""
import re, glob, os, sys, json, collections
ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
ASM = os.path.join(ROOT, 'build', 'GW4E69', 'asm')
strings = {}
for f in glob.glob(os.path.join(ASM, 'auto_0[4-9]_*.s')) + glob.glob(os.path.join(ASM, 'auto_1*_*.s')):
    cur = None
    for line in open(f, errors='replace'):
        m = re.match(r'\.obj (\S+),', line)
        if m: cur = m.group(1); continue
        m = re.match(r'\s+\.string "(.*)"', line)
        if m and cur and re.search(r'\.[ch]$', m.group(1)): strings[cur] = m.group(1)
refs = collections.defaultdict(set)
for f in glob.glob(os.path.join(ASM, 'auto_0[0-1]_*.s')) + glob.glob(os.path.join(ASM, '*', '*.s')):
    cur = addr = None
    for line in open(f, errors='replace'):
        m = re.match(r'\.fn (\S+),', line)
        if m: cur = m.group(1); continue
        m = re.match(r'/\* ([0-9A-F]{8}) ', line)
        if m: addr = int(m.group(1), 16)
        for lab in re.findall(r'(lbl_[0-9A-F]{8})@', line):
            if lab in strings and cur: refs[lab].add((addr, cur))
byfile = collections.defaultdict(set)
for lab, s in strings.items():
    for a, fn in refs.get(lab, ()): byfile[s.split('/')[-1]].add((a, fn))
rows = sorted((min(a for a, _ in v), max(a for a, _ in v), k, sorted(v)) for k, v in byfile.items())
if '--json' in sys.argv:
    out = sys.argv[sys.argv.index('--json') + 1]
    json.dump({k: [{'addr': a, 'fn': fn} for a, fn in v] for _, _, k, v in rows}, open(out, 'w'), indent=1)
    print('wrote', out)
else:
    for lo, hi, k, v in rows: print(f'0x{lo:08X}  0x{hi:08X}  {len(v):3}  {k}')
    print(len(rows), 'files;', len(strings) - len(rows), 'unreferenced')
