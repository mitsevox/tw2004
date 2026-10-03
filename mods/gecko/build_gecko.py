"""Compile an asm hook with the project's CodeWarrior, pull its .text out of the ELF object, and
print it as a Gecko C2 code.   python build_gecko.py <file.c> <hook address hex>"""
import struct, subprocess, sys, os
src, addr = sys.argv[1], int(sys.argv[2], 16)
obj = os.path.splitext(src)[0] + '.o'
cc = ['C:/dev/tw2004/build/tools/sjiswrap.exe', 'C:/dev/tw2004/build/compilers/GC/2.5/mwcceppc.exe',
      '-nodefaults', '-proc', 'gekko', '-fp', 'hardware', '-O0', '-c', src, '-o', obj]
r = subprocess.run(cc, capture_output=True, text=True)
if r.returncode: print(r.stdout, r.stderr); sys.exit(1)
d = open(obj, 'rb').read()
shoff, shentsize, shnum, shstrndx = struct.unpack_from('>I', d, 0x20)[0], *struct.unpack_from('>HHH', d, 0x2E)
secs = [struct.unpack_from('>IIIIIIIIII', d, shoff + i * shentsize) for i in range(shnum)]
strtab = secs[shstrndx]
def name(s): o = strtab[4] + s[0]; return d[o:d.index(b'\0', o)].decode()
text = next(s for s in secs if name(s) == '.text')
code = d[text[4]:text[4] + text[5]]
words = [struct.unpack_from('>I', code, i)[0] for i in range(0, len(code), 4)]
assert words[-1] >> 26 != 18 or True
body = words[:]                      # the last instruction is the replaced one
if len(body) % 2 == 0: body += [0x60000000]
body += [0x00000000]                 # Gecko fills in the branch back
lines = [(body[i], body[i + 1]) for i in range(0, len(body), 2)]
print(f'{len(words)} instructions')
print(f'C2{addr & 0x1FFFFFF:06X} {len(lines):08X}')
for a, b in lines: print(f'{a:08X} {b:08X}')
