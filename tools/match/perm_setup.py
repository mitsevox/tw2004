"""Set up a decomp-permuter directory for one function of a tw2004 unit.
    python tools/match/perm_setup.py <Unit> <fn> [suffix]    e.g.  ... Ball fn_80050D34
Creates build/perm/<fn>/ in this checkout with base.c, target.o, compile.sh, settings.toml.
Run:  timeout 2700 python C:/dev/tools/decomp-permuter/permuter.py build/perm/<fn> -j6 --best-only
"""
import os, pathlib, re, shlex, subprocess, sys

ROOT = pathlib.Path(__file__).resolve().parents[2].as_posix()   # the checkout this script lives in
HERE = pathlib.Path(__file__).resolve().parent.as_posix()
unit, fn = sys.argv[1], sys.argv[2]
out = f'{ROOT}/build/perm/{fn}' + (sys.argv[3] if len(sys.argv) > 3 else '')
os.makedirs(out, exist_ok=True)

def unit_flags(unit):
    """The compiler and cflags build.ninja uses for this unit's object (per-object extra_cflags included)."""
    nj = re.sub(r'\$\n\s*', '', open(ROOT + '/build.ninja', encoding='utf-8').read())
    obj = 'build\\GW4E69\\src\\%s.o:' % unit.replace('/', '\\')
    i = nj.index('build ' + obj)
    block = nj[i:nj.find('\nbuild ', i + 1)]
    ver = re.search(r'^\s*mw_version = (\S+)', block, re.M).group(1).replace('\\', '/')
    flags = shlex.split(re.sub(r'\s+', ' ', re.search(r'^\s*cflags = (.*)$', block, re.M).group(1)))
    return ROOT + '/build/compilers/%s/mwcceppc.exe' % ver, flags


CC, ALL = unit_flags(unit)
CFLAGS, DEFS, k = [], [], 0             # compile.sh gets no -i/-D: base.c is already preprocessed
while k < len(ALL):
    if ALL[k] in ('-i', '-I', '-ir'):
        k += 2
        continue
    (DEFS if ALL[k].startswith('-D') else CFLAGS).append(ALL[k])
    k += 1


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
