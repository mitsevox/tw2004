"""Before linking a unit as Matching: list the data sections our object emits, and for each local
constant the original symbol at the same relocation, its address and size, and which units use it.
    python tools/match/datamap.py <Unit> [<Unit> ...]      (unit names without .c, e.g. GameMode17)
See docs/workflow.md, "Finishing a unit"."""
import glob, os, pathlib, re, subprocess, sys
ROOT = pathlib.Path(__file__).resolve().parents[2].as_posix()   # the checkout this script lives in
RE = ROOT + '/build/binutils/powerpc-eabi-readelf.exe'
syms = {}
for l in open(ROOT + '/config/GW4E69/symbols.txt'):
    m = re.match(r'(\S+) = (\.\w+):0x([0-9A-F]+);.*?size:0x([0-9A-F]+)', l)
    if m: syms[m.group(1)] = (m.group(2), int(m.group(3), 16), int(m.group(4), 16))
asm = {f: open(f).read() for f in glob.glob(ROOT + '/build/GW4E69/asm/*.s') if 'auto_' not in f}
def relocs(o):
    out = subprocess.run([RE, '-r', '-W', o], capture_output=True, text=True).stdout
    sec = None; r = {}
    for l in out.splitlines():
        m = re.match(r"Relocation section '\.rela(\S+)'", l)
        if m: sec = m.group(1); continue
        p = l.split()
        if len(p) >= 5 and re.match(r'^[0-9a-f]{8}$', p[0]):
            r[(sec, int(p[0], 16))] = (p[4], p[6] if len(p) > 6 else '0')
    return r
def sections(o):
    out = subprocess.run([RE, '-S', '-W', o], capture_output=True, text=True).stdout
    res = []
    for l in out.splitlines():
        m = re.match(r'\s*\[\s*\d+\]\s+(\.\S+)\s+\S+\s+\S+\s+\S+\s+([0-9a-f]+)\s+\S+\s+(\S*A\S*)', l)
        if m and m.group(1) != '.text': res.append((m.group(1), int(m.group(2), 16)))
    return res
def symsec(o):
    out = subprocess.run([RE, '-s', '-W', o], capture_output=True, text=True).stdout
    return out
for u in sys.argv[1:]:
    src = f'{ROOT}/build/GW4E69/src/{u}.o'; org = f'{ROOT}/build/GW4E69/obj/{u}.o'
    print('==', u, sections(src))
    rs, ro = relocs(src), relocs(org)
    seen = {}
    for k, (name, add) in rs.items():
        if name.startswith('@') or name.startswith('.') :
            o = ro.get(k)
            seen.setdefault((name, add), set()).add(o[0] if o else None)
    for (name, add), os_ in sorted(seen.items()):
        for on in os_:
            s = syms.get(on)
            users = sorted(os.path.basename(f)[:-2] for f, t in asm.items() if on and re.search(r'\b' + re.escape(on) + r'\b', t))
            print('  ', name, '+' + add, '->', on, s and (s[0], hex(s[1]), hex(s[2])), 'used by', users)
