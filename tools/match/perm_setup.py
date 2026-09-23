"""Set up a decomp-permuter directory for one function of a tw2004 unit.
    python tools/match/perm_setup.py <Unit> <fn> [suffix]    e.g.  ... Ball fn_80050D34
Creates build/perm/<fn>/ in this checkout with base.c, target.o, compile.sh, settings.toml.
Run:  timeout 2700 python C:/dev/tools/decomp-permuter/permuter.py build/perm/<fn> -j6 --best-only
"""
import os, pathlib, re, subprocess, sys

ROOT = pathlib.Path(__file__).resolve().parents[2].as_posix()   # the checkout this script lives in
HERE = pathlib.Path(__file__).resolve().parent.as_posix()
unit, fn = sys.argv[1], sys.argv[2]
out = f'{ROOT}/build/perm/{fn}' + (sys.argv[3] if len(sys.argv) > 3 else '')
os.makedirs(out, exist_ok=True)

CC = ROOT + '/build/compilers/GC/2.5/mwcceppc.exe'
CFLAGS = ['-nodefaults', '-proc', 'gekko', '-align', 'powerpc', '-enum', 'int', '-fp', 'hardware',
          '-Cpp_exceptions', 'off', '-O4,p', '-inline', 'smart', '-pragma', 'cats off', '-pragma',
          'warn_notinlined off', '-maxerrors', '1', '-nosyspath', '-RTTI', 'off', '-fp_contract', 'on',
          '-str', 'reuse', '-common', 'on', '-use_lmw_stmw', 'on', '-multibyte', '-lang=c']
DEFS = ['-DBUILD_VERSION=0', '-DVERSION_GW4E69', '-DVERSION=0', '-DNDEBUG=1']


def bracket_end(s, i):
    lvl = 0
    while True:
        if s[i] == '{':
            lvl += 1
        elif s[i] == '}':
            lvl -= 1
            if lvl == 0:
                return i
        i += 1


# 1. preprocessed source, other functions reduced to prototypes (inline helpers kept whole)
pp = subprocess.run([CC] + CFLAGS + ['-i', 'include', '-i', 'build/GW4E69/include'] + DEFS
                    + ['-EP', f'src/{unit}.c'], cwd=ROOT, capture_output=True, text=True, check=True).stdout
pp = '\n'.join(l for l in pp.splitlines() if not l.startswith('#pragma'))
res, pos = [], 0
hdr = re.compile(r'^([^\n;{}#]*?\b(\w+)\s*\([^;{}]*\))\s*\{', re.M)
while True:
    m = hdr.search(pp, pos)
    if not m:
        res.append(pp[pos:])
        break
    end = bracket_end(pp, m.end() - 1)
    head, name = m.group(1), m.group(2)
    if name in ('if', 'for', 'while', 'switch'):      # not a function
        res.append(pp[pos:end + 1]); pos = end + 1; continue
    res.append(pp[pos:m.start()])
    if name == fn or re.search(r'\binline\b', head):
        res.append(pp[m.start():end + 1])
    else:
        head = re.sub(r'^\s*asm\s+', '', head)
        head = re.sub(r'\bregister\s+', '', head)
        res.append(head + ';')
    pos = end + 1
open(out + '/base.c', 'w', encoding='utf-8', newline='\n').write(''.join(res))

# 2. target.o: the function's asm, assembled on its own
asm = open(f'{ROOT}/build/GW4E69/asm/{unit}.s', encoding='utf-8').read()
m = re.search(r'^\.fn ' + re.escape(fn) + r',.*?^\.endfn ' + re.escape(fn) + r'$', asm, re.M | re.S)
open(out + '/target.s', 'w', encoding='utf-8', newline='\n').write(
    '.include "macros.inc"\n.text\n.balign 4\n' + m.group(0) + '\n')
subprocess.run([ROOT + '/build/binutils/powerpc-eabi-as.exe', '-mgekko', '-I', ROOT + '/build/GW4E69/include',
                out + '/target.s', '-o', out + '/target.o'], check=True)

# 3. compile.sh and settings.toml
flags = ' '.join("'%s'" % f for f in CFLAGS)
open(out + '/compile.sh', 'w', newline='\n').write(
    '#!/bin/bash\n"%s" %s -c "$1" -o "$3" >/dev/null 2>&1\n' % (CC, flags))
open(out + '/settings.toml', 'w', newline='\n').write(
    'func_name = "%s"\ncompiler_type = "mwcc"\nobjdump_command = "python %s/perm_objdump.py %s"\n' % (fn, HERE, fn))
print('ready:', out)
