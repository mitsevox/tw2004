#!/usr/bin/env python3
"""
Compile one C file with SN Systems ProDG (GCC 2.95 for GameCube) for the dtk build.

    python tools/build/prodg/prodgcc.py <compilers_dir> <version> [gcc flags...] -c <src.c> -o <out_dir>

The ProDG driver (ngccc.exe) refuses to run without an installed sn.ini, so this runs the three
stages itself: cpp.exe -> cc1.exe -> NgcAs.exe. The object lands at <out_dir>/<stem>.o.
"""
import os, subprocess, sys
args = sys.argv[1:]
wrapper = []
if args and args[0] == '--wrapper':     # e.g. wibo or wine: the ProDG tools are Windows programs
    wrapper = [args[1]]; args = args[2:]
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
# cpp.exe predefines nothing on its own (the driver passes its version through the specs), so
# give it the compiler's version the way ngccc would.
cpp_flags = ['-D__GNUC__=2', '-D__GNUC_MINOR__=95'] + [f for f in flags if f.startswith(('-I', '-D', '-U', '-nostdinc'))]
as_flags = [x for f in flags if f.startswith('-Wa,') for x in f[4:].split(',')]   # -Wa,<opt> goes to NgcAs, as with gcc
cc1_flags = [f for f in flags if not f.startswith(('-I', '-D', '-U', '-nostdinc', '-Wa,'))]
def strip_empty_sections(path):
    """NgcAs emits empty .data/.bss/.sbss/.sdata sections. The CodeWarrior linker still rounds the
    output section up for them (it shifted .sbss by 2 bytes), so remove them from the section table
    entirely and renumber every section reference (symbols, sh_link, sh_info)."""
    import struct
    d = bytearray(open(path, 'rb').read())
    shoff = struct.unpack('>I', d[0x20:0x24])[0]; shentsize, shnum, shstrndx = struct.unpack('>HHH', d[0x2E:0x34])
    hdrs = [bytes(d[shoff + i * shentsize: shoff + (i + 1) * shentsize]) for i in range(shnum)]
    def fld(h, o): return struct.unpack('>I', h[o:o + 4])[0]
    drop = {i for i, h in enumerate(hdrs) if i and fld(h, 4) in (1, 8) and fld(h, 20) == 0}
    if not drop:
        return
    newidx = {}; kept = []
    for i, h in enumerate(hdrs):
        if i in drop: continue
        newidx[i] = len(kept); kept.append(bytearray(h))
    def remap(i): return newidx.get(i, 0)
    for h in kept:
        typ = fld(h, 4)
        link, info = fld(h, 24), fld(h, 28)
        h[24:28] = struct.pack('>I', remap(link) if link else 0)
        if typ in (2, 4, 9):                      # SYMTAB link/info and RELA/REL info are section refs
            h[28:32] = struct.pack('>I', remap(info) if (typ != 2) else info)
    # symbols: renumber st_shndx (drop section symbols of removed sections by pointing them at UNDEF)
    for i, h in enumerate(hdrs):
        if fld(h, 4) == 2:
            off, size, ent = fld(h, 16), fld(h, 20), fld(h, 36)
            for k in range(size // ent):
                so = off + k * ent
                shndx = struct.unpack('>H', d[so + 14:so + 16])[0]
                if shndx and shndx < 0xFF00:
                    d[so + 14:so + 16] = struct.pack('>H', remap(shndx))
    table = b''.join(kept)
    d[shoff:shoff + shnum * shentsize] = table + bytes((shnum - len(kept)) * shentsize)
    d[0x30:0x32] = struct.pack('>H', len(kept))
    d[0x32:0x34] = struct.pack('>H', remap(shstrndx))
    open(path, 'wb').write(d)


def size_string_labels(path):
    """gcc names each string literal with a .LC label that has no size or type. Give every .LC label
    in a data section the object type and a size (a string's length with its NUL, otherwise up to
    the next label or the section end), so that the strings show up as data objects like
    CodeWarrior's."""
    import struct
    d = bytearray(open(path, 'rb').read())
    shoff = struct.unpack('>I', d[0x20:0x24])[0]; shentsize, shnum = struct.unpack('>HH', d[0x2E:0x32])
    sh = [struct.unpack('>10I', d[shoff + i * shentsize: shoff + i * shentsize + 40]) for i in range(shnum)]
    for symtab in [h for h in sh if h[1] == 2]:
        strtab = sh[symtab[6]]
        off, size, ent = symtab[4], symtab[5], symtab[9]
        syms = []
        for k in range(size // ent):
            so = off + k * ent
            name_off, value, ssize, info, other, shndx = struct.unpack('>IIIBBH', d[so:so + 16])
            n0 = strtab[4] + name_off
            name = bytes(d[n0:d.index(0, n0)]).decode('latin-1')
            syms.append((so, name, value, ssize, info, shndx))
        for so, name, value, ssize, info, shndx in syms:
            if not name.startswith('.LC') or ssize or not 0 < shndx < len(sh) or sh[shndx][1] != 1 or sh[shndx][2] & 4:
                continue
            ends = [v for _, n, v, _, i, x in syms if x == shndx and v > value and (i & 0xF) != 3]
            end = min(ends) if ends else sh[shndx][5]
            body = bytes(d[sh[shndx][4] + value: sh[shndx][4] + end])
            nul = body.find(0)
            if 0 < nul and not any(body[nul:]) and all(32 <= c < 127 or c in (9, 10, 13) for c in body[:nul]):
                end = value + nul + 1          # a string: its bytes and the NUL, not the alignment padding
            struct.pack_into('>IB', d, so + 8, end - value, (info & 0xF0) | 1)
    open(path, 'wb').write(d)


def run(cmd):
    cmd = wrapper + cmd
    r = subprocess.run(cmd, capture_output=True, text=True)
    if r.returncode:
        sys.stderr.write(' '.join(cmd) + '\n' + r.stdout + r.stderr); sys.exit(r.returncode)
def write_depfile(path):
    """<stem>.d for ninja (deps = gcc), built from the preprocessor's line markers, so that an edited
    header rebuilds this unit. GCC 2.95's cpp has no -MD of the form ninja needs."""
    import re
    seen = []
    for l in open(i_file, errors='replace'):
        m = re.match(r'# \d+ "([^"<>]+)"', l)
        if m:
            p = m.group(1).replace('\\', '/')
            if p not in seen and os.path.isfile(p):
                seen.append(p)
    with open(path, 'w') as f:
        f.write('%s: %s\n' % (o_file.replace('\\', '/'), ' '.join(p.replace(' ', '\\ ') for p in seen)))


run([os.path.join(d, 'cpp.exe' if os.path.exists(os.path.join(d, 'cpp.exe')) else 'CPP.exe'), *cpp_flags, src, i_file])
write_depfile(os.path.join(out_dir, stem + '.d'))
run([os.path.join(d, 'cc1.exe'), *cc1_flags, '-quiet', i_file, '-o', s_file])
run([os.path.join(d, 'NgcAs.exe'), *as_flags, s_file, '-o', o_file])
strip_empty_sections(o_file)
size_string_labels(o_file)
