"""Evidence for naming a unit's functions, from the original's split objects (build/GW4E69/obj).
    python tools/match/callgraph.py <unit>           e.g. GoCamera, unsorted/sweep_8010FF5C
    python tools/match/callgraph.py <unit> --unnamed only the functions still named fn_XXXXXXXX
For every function of the unit, leaves first (a function comes after everything it calls inside the
unit, so callee names are settled before their callers are read): its address and size, what it
calls, who calls it (in any unit), the globals it reads or writes, the text of any string literal it
uses, and any TW06 name suggestion from docs/tw06-names.md that has not been applied.
Run after a build (`ninja` makes the split objects)."""
import bisect, collections, json, pathlib, re, struct, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]
OBJ = ROOT / 'build/GW4E69/obj'
R_PPC_REL24 = 10


def sections(b):
    shoff, = struct.unpack_from('>I', b, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from('>HHH', b, 0x2E)
    secs = [struct.unpack_from('>IIIIIIIIII', b, shoff + i * shentsize) for i in range(shnum)]
    names = secs[shstrndx][4]
    return [(b[names + s[0]:b.index(b'\0', names + s[0])].decode(), s) for s in secs]


def read_obj(path):
    """(functions, calls, refs): functions = [(name, offset, size)] in .text; calls/refs =
    {function: [symbol name or ("str", text)]} in order of appearance."""
    b = path.read_bytes()
    secs = sections(b)
    symtab = next(s for _, s in secs if s[1] == 2)
    strtab = secs[symtab[6]][1][4]
    syms = []
    for off in range(symtab[4], symtab[4] + symtab[5], 16):
        st_name, value, size, info, _, shndx = struct.unpack_from('>IIIBBH', b, off)
        syms.append((b[strtab + st_name:b.index(b'\0', strtab + st_name)].decode(), value, size, info & 0xF, shndx))
    text = [i for i, (n, _) in enumerate(secs) if n == '.text']
    funcs = sorted((s[1], s[2], s[0], s[4]) for s in syms if s[3] == 2 and s[4] in text)

    def string_at(shndx, off):
        n, s = secs[shndx]
        if s[1] != 1 or n.startswith('.text'):
            return None
        raw = b[s[4] + off:s[4] + s[5]]
        end = raw.find(b'\0')
        t = raw[:end if end >= 0 else 0]
        if len(t) >= 2 and all(32 <= c < 127 or c in (9, 10) for c in t):
            return t.decode().replace('\n', '\\n')
        return None

    calls, refs = collections.defaultdict(list), collections.defaultdict(list)
    for n, s in secs:
        if s[1] != 4 or s[7] not in text:           # SHT_RELA against a .text section
            continue
        tsec = s[7]
        mine = [f for f in funcs if f[3] == tsec]
        starts = [f[0] for f in mine]
        for off in range(s[4], s[4] + s[5], 12):
            r_off, r_info, addend = struct.unpack_from('>IIi', b, off)
            i = bisect.bisect_right(starts, r_off) - 1
            if i < 0 or r_off >= mine[i][0] + max(mine[i][1], 4):
                continue
            owner = mine[i]
            name, value, _, typ, shndx = syms[r_info >> 8]
            if (r_info & 0xFF) == R_PPC_REL24:
                if name and name not in calls[owner[2]]:
                    calls[owner[2]].append(name)
                continue
            if shndx and shndx < len(secs):
                st = string_at(shndx, (value if typ != 3 else 0) + addend)
                if st is not None:
                    if ('str', st) not in refs[owner[2]]:
                        refs[owner[2]].append(('str', st))
                    continue
            if name and not name.startswith('@') and name not in refs[owner[2]]:
                refs[owner[2]].append(name)
    return [(f[2], f[0], f[1]) for f in funcs], calls, refs


def symbol_addresses():
    out = collections.defaultdict(list)
    for l in (ROOT / 'config/GW4E69/symbols.txt').read_text(encoding='utf-8').splitlines():
        m = re.match(r'^(\S+) = \.text:0x([0-9A-Fa-f]+);', l)
        if m:
            out[m.group(1)].append(int(m.group(2), 16))
    return out


def tw06_hints():
    """address -> 'tier: name (evidence)' for suggestions in tw06-names.md that were not applied."""
    hints = {}
    doc = ROOT / 'docs/tw06-names.md'
    if doc.exists():
        for l in doc.read_text(encoding='utf-8').splitlines():
            m = re.match(r'^\| `([0-9A-Fa-f]{8})` \| `([^`]*)` \| `([^`]*)` \| (\w+) \| (.*) \|$', l)
            if m and m.group(2).startswith('fn_'):
                hints[int(m.group(1), 16)] = '%s: %s (%s)' % (m.group(4), m.group(3), m.group(5).strip())
    return hints


def main():
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    if not args:
        sys.exit(__doc__)
    unit = args[0].replace('\\', '/').removesuffix('.c').removesuffix('.o')
    target = OBJ / (unit + '.o')
    if not target.exists():
        sys.exit('%s not found: give the unit as configure.py names it, after a build' % target.relative_to(ROOT))
    callers = collections.defaultdict(set)
    # only the current units (objdiff.json); build/GW4E69/obj keeps objects of splits since replaced
    current = [ROOT / u['target_path'] for u in json.loads((ROOT / 'objdiff.json').read_text())['units']
               if u.get('target_path')]
    for o in current:
        if not o.exists():
            continue
        _, calls, _ = read_obj(o)
        u = o.relative_to(OBJ).with_suffix('').as_posix()
        for f, cs in calls.items():
            for c in cs:
                callers[c].add('%s (%s)' % (f, u) if u != unit else f)
    funcs, calls, refs = read_obj(target)
    names = {f[0] for f in funcs}
    addr = symbol_addresses()
    hints = tw06_hints()
    order, seen = [], set()

    def visit(f, stack):
        if f in seen or f in stack:
            return
        stack.add(f)
        for c in calls.get(f, ()):
            if c in names:
                visit(c, stack)
        stack.discard(f)
        seen.add(f)
        order.append(f)
    for f, _, _ in funcs:
        visit(f, set())
    size = {f[0]: f[2] for f in funcs}
    only_unnamed = '--unnamed' in sys.argv
    print('%s: %d functions, %d still fn_ (leaves first)\n' % (
        unit, len(funcs), sum(1 for f in funcs if f[0].startswith('fn_'))))
    for f in order:
        if only_unnamed and not f.startswith('fn_'):
            continue
        a = addr.get(f, [])
        print('%s  %s  size 0x%X' % (f, '0x%08X' % a[0] if len(a) == 1 else '?', size[f]))
        cs = calls.get(f, [])
        print('  calls:     %s' % (', '.join(cs) or '-'))
        by = sorted(callers.get(f, ()))
        print('  called by: %s' % (', '.join(by[:12]) + (' ... +%d more' % (len(by) - 12) if len(by) > 12 else '') or '-'))
        data = [r for r in refs.get(f, []) if not isinstance(r, tuple)]
        strs = [r[1] for r in refs.get(f, []) if isinstance(r, tuple)]
        if data:
            print('  data:      %s' % ', '.join(data))
        if strs:
            print('  strings:   %s' % ', '.join('"%s"' % s for s in strs))
        if len(a) == 1 and a[0] in hints:
            print('  tw06:      %s' % hints[a[0]])
        print()


if __name__ == '__main__':
    main()
