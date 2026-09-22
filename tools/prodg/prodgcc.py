#!/usr/bin/env python3
"""
Compile one C file with SN Systems ProDG (GCC 2.95 for GameCube) for the dtk build.

    python tools/prodg/prodgcc.py <compilers_dir> <version> [gcc flags...] -c <src.c> -o <out_dir>

The ProDG driver (ngccc.exe) refuses to run without an installed sn.ini, so this runs the three
stages itself: cpp.exe -> cc1.exe -> NgcAs.exe. The object lands at <out_dir>/<stem>.o.
"""
import os, subprocess, sys
args = sys.argv[1:]
compilers, version = args[0], args[1]; rest = args[2:]
src = rest[rest.index('-c') + 1]; out_dir = rest[rest.index('-o') + 1]
flags = []
skip = False
for i, a in enumerate(rest):
    if skip: skip = False; continue
    if a in ('-c', '-o'): skip = True; continue
    if a.startswith('-lang'): continue                       # CodeWarrior flag added by the generator
    if a in ('-I', '-D', '-U') and i + 1 < len(rest): flags.append(a + rest[i + 1]); skip = True; continue
    flags.append(a)
d = os.path.join(compilers, version)
stem = os.path.splitext(os.path.basename(src))[0]
os.makedirs(out_dir, exist_ok=True)
i_file = os.path.join(out_dir, stem + '.i'); s_file = os.path.join(out_dir, stem + '.s'); o_file = os.path.join(out_dir, stem + '.o')
cpp_flags = [f for f in flags if f.startswith(('-I', '-D', '-U', '-nostdinc'))]
cc1_flags = [f for f in flags if not f.startswith(('-I', '-D', '-U', '-nostdinc'))]
def run(cmd):
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode:
        sys.stderr.write(' '.join(cmd) + '\n' + r.stdout + r.stderr); sys.exit(r.returncode)
run([os.path.join(d, 'cpp.exe' if os.path.exists(os.path.join(d, 'cpp.exe')) else 'CPP.exe'), *cpp_flags, src, i_file])
run([os.path.join(d, 'cc1.exe'), *cc1_flags, '-quiet', i_file, '-o', s_file])
run([os.path.join(d, 'NgcAs.exe'), s_file, '-o', o_file])
