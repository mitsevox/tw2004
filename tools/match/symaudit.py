"""Check our declarations of the game's globals against config/GW4E69/symbols.txt.
    python tools/match/symaudit.py            list every problem
    python tools/match/symaudit.py --all      also list partial views (declared smaller than the object)
    python tools/match/symaudit.py --count    one line: "<sda> <too-big> <scope>" (used by merges)
Run it after a build: the scope check reads the built objects.

Size (src/*.c, game code). The compiler decides how to address a global from its declared size:
8 bytes or less is small data, reached through r13/r2 in one instruction; anything larger (or of
unknown size) is built with lis/addi. So a declaration whose size is on the wrong side of 8 makes
every use of the global compile differently from the original. The declared size is taken from the
compiler itself (sizeof, in a copy of each unit), and compared with the object's size and section:
  sda       the object is in .sdata/.sbss/.sdata2/.sbss2 but the declaration is larger than 8 bytes
            or unsized, or the other way round;
  too-big   the declaration is larger than the object (it overlaps whatever follows it);
  partial   the declaration is smaller than the object (only with --all: normal while a struct is
            still being worked out, but the goal is every global at its true size).
An `extern` with `// fake match: <why>` on its line or the line before is skipped: some originals
really did declare a global unsized (Swing.c's lbl_80281B40 is reached with lis/addi although it
lives in .sdata).

Scope (every built object). symbols.txt knows the scope of the SDK's symbols and a few others
(the game's own functions and data carry none, so their `static` cannot be checked this way):
  scope     we define a symbol `static` that the original has global, or the other way round."""
import collections, concurrent.futures, pathlib, re, struct, subprocess, sys, tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
sys.path.insert(0, str(ROOT / 'tools/match'))
from lint import CFLAGS                              # noqa: E402  (same flags as lint's compiles)

CC = ROOT / 'build/compilers/GC/2.5/mwcceppc.exe'
NM = ROOT / 'build/binutils/powerpc-eabi-nm.exe'
SMALL = ('.sdata', '.sbss', '.sdata2', '.sbss2')
SYM = re.compile(r'^(\S+) = (\.\w+):0x([0-9A-Fa-f]+); // type:(\w+)(?: size:0x([0-9A-Fa-f]+))?(.*)$')


def symbols():
    """name -> list of (section, size, scope); a name can repeat (file-local symbols)."""
    out = collections.defaultdict(list)
    for l in (ROOT / 'config/GW4E69/symbols.txt').read_text(encoding='utf-8').splitlines():
        m = SYM.match(l)
        if m:
            scope = re.search(r'scope:(\w+)', m.group(6))
            out[m.group(1)].append((m.group(2), int(m.group(5) or '0', 16), m.group(4),
                                    scope.group(1) if scope else None))
    return out


def elf_array(path, name):
    """The 32-bit words of the initialised array `name` in a big-endian ELF object."""
    b = path.read_bytes()
    shoff, = struct.unpack_from('>I', b, 0x20)
    shentsize, shnum = struct.unpack_from('>HH', b, 0x2E)
    secs = [struct.unpack_from('>IIIIIIIIII', b, shoff + i * shentsize) for i in range(shnum)]
    symtab = next(s for s in secs if s[1] == 2)                  # SHT_SYMTAB
    strtab = secs[symtab[6]]                                     # sh_link
    for off in range(symtab[4], symtab[4] + symtab[5], 16):
        st_name, value, size, _, _, shndx = struct.unpack_from('>IIIBBH', b, off)
        end = b.index(b'\0', strtab[4] + st_name)
        if b[strtab[4] + st_name:end].decode() == name:
            data = secs[shndx][4] + value
            return list(struct.unpack_from('>%dI' % (size // 4), b, data))
    raise KeyError(name)


def declared_sizes(src, names):
    """name -> sizeof(name) as the compiler sees it in src, or None when the declaration has no
    size (an incomplete array or struct). Names with no file-scope declaration are left out."""
    names = sorted(names)
    unsized = set()
    with tempfile.TemporaryDirectory() as tmp:
        wrap, obj = pathlib.Path(tmp, 'symaudit.c'), pathlib.Path(tmp, 'symaudit.o')
        for _ in range(4):                       # each round drops the names the compiler rejected
            if not names:
                return {n: None for n in unsized}
            wrap.write_text('#include "%s"\nunsigned long symaudit_sizes[] = {\n%s};\n' % (
                src.resolve().as_posix(), ''.join('    sizeof(%s),\n' % n for n in names)))
            r = subprocess.run([str(CC)] + CFLAGS + ['-maxerrors', '1000', '-c', str(wrap), '-o', str(obj)],
                               cwd=ROOT, capture_output=True, text=True)
            bad = {}
            for l in (r.stdout + r.stderr).splitlines():
                m = re.match(r'(.*?):(\d+): (?!warning:)(.*)', l)
                if m and pathlib.Path(m.group(1)).name == wrap.name and 3 <= int(m.group(2)) < 3 + len(names):
                    bad[names[int(m.group(2)) - 3]] = m.group(3)
            if r.returncode == 0:
                return dict(zip(names, elf_array(obj, 'symaudit_sizes')), **{n: None for n in unsized})
            if not bad:
                sys.exit('%s does not compile:\n%s' % (src.name, r.stdout + r.stderr))
            unsized |= {n for n, e in bad.items() if 'undefined identifier' not in e}
            names = [n for n in names if n not in bad]
    sys.exit('%s: could not settle the declared sizes' % src.name)


def size_findings(syms):
    objects = {n: e for n, es in syms.items() for e in es if e[2] == 'object' and e[1]}
    work = {f: set(re.findall(r'\b[A-Za-z_]\w*\b', f.read_text(encoding='utf-8', errors='replace')))
            & objects.keys() for f in sorted((ROOT / 'src').glob('*.c'))}
    with concurrent.futures.ThreadPoolExecutor(8) as pool:
        sizes = dict(zip(work, pool.map(lambda f: declared_sizes(f, work[f]), work)))
    hits = collections.defaultdict(list)                # (kind, name, declared) -> files
    for f, got in sizes.items():
        text = f.read_text(encoding='utf-8', errors='replace')
        for n, d in got.items():
            # a declaration that differs on purpose says so: `// fake match:` on its line or the one before
            if re.search(r'fake match[^\n]*\n[^\n]*\bextern\b[^\n;]*\b%s\b|\bextern\b[^\n;]*\b%s\b[^\n]*fake match'
                         % (n, n), text):
                continue
            sec, size = objects[n][0], objects[n][1]
            if (sec in SMALL) != (d is not None and d <= 8):
                kind = 'sda'
            elif d is not None and d > size:
                kind = 'too-big'
            elif d is not None and d < size:
                kind = 'partial'
            else:
                continue
            hits[kind, n, d].append(f.name)
    return [(k, n, '%s declared %s, the object is 0x%X bytes in %s (%s)' % (
        n, 'unsized' if d is None else '0x%X bytes' % d, objects[n][1], objects[n][0], ', '.join(fs)))
        for (k, n, d), fs in sorted(hits.items())]


def scope_findings(syms):
    hits = []
    for o in sorted((ROOT / 'build/GW4E69/src').rglob('*.o')):
        if 'unsorted' in o.parts:
            continue                        # sweeps are placeholders, not our declarations
        out = subprocess.run([str(NM), str(o)], capture_output=True, text=True).stdout
        for l in out.splitlines():
            m = re.match(r'^[0-9a-f]{8} ([A-Za-z]) (\S+)$', l)
            if not m or m.group(1) in 'Uu' or re.search(r'[@$.]', m.group(2)):
                continue
            ours = 'local' if m.group(1).islower() else 'global'
            theirs = {e[3] for e in syms.get(m.group(2), ()) if e[3] in ('local', 'global')}
            if theirs and ours not in theirs:
                hits.append(('scope', m.group(2), '%s is %s in %s but %s in the original' % (
                    m.group(2), ours, o.relative_to(ROOT / 'build/GW4E69').as_posix(), '/'.join(sorted(theirs)))))
    return hits


def main():
    syms = symbols()
    hits = size_findings(syms) + scope_findings(syms)
    if '--count' in sys.argv:
        print(*(sum(1 for h in hits if h[0] == k) for k in ('sda', 'too-big', 'scope')))
        return 0
    shown = [h for h in hits if h[0] != 'partial' or '--all' in sys.argv]
    for kind, _, msg in shown:
        print('%s: %s' % (kind, msg))
    counts = collections.Counter(h[0] for h in hits)
    print(', '.join('%s %d' % (k, counts[k]) for k in ('sda', 'too-big', 'scope', 'partial')))
    return 1 if any(h[0] != 'partial' for h in hits) else 0


if __name__ == '__main__':
    sys.exit(main())
