"""Cross-file type consistency audit for src/*.c.
    python tools/match/typeaudit.py            list every problem
    python tools/match/typeaudit.py --count    one line: "<conflicts> <duplicates> <externs> <typedefs>"
                                               (used by merges: none of the four may rise)
Reports two kinds of disparity that per-file matching can introduce:
  - a global declared with different types in different files (e.g. `u8*` in one, a struct in
    another): one object must have one declaration, in a header;
  - a struct/union/enum type defined locally in more than one file: two private views of the same
    data drift apart; the type belongs in a header, defined once.
It also counts `extern` declarations and `typedef`s written in .c files at all: both belong in a
header (docs/style.md), so those two counts only go down.
Sweep files (src/unsorted) are skipped. The goal is zero of all four; merges may not add any.

    python tools/match/typeaudit.py --layout           struct types with the same layout under
    python tools/match/typeaudit.py --layout --count   different names (one number; not a gate yet)
Two names for one layout (TourStats in save.h, PgaStatCounts in pgatoursim.h) are the same drift
as a type defined twice, only harder to see. Offsets and sizes are measured by the compiler
(headers, and each .c's own types); fields named unk*/pad* are ignored, and an array's elements
count for overlap but not as agreement (u32 a[20] fits any run of words)."""
import collections, pathlib, re, struct, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
sys.path.insert(0, str(ROOT / 'tools/match'))
import sweepblock                                    # noqa: E402


def audit():
    ext = collections.defaultdict(dict)
    tdefs = collections.defaultdict(list)
    raw = collections.Counter()      # 'extern' / 'typedef' lines in .c files
    for f in sorted((ROOT / 'src').glob('*.c')):
        # a marked block of uncleaned sweep code is counted by lint.py, not here (sweepblock.py)
        s = sweepblock.strip_blocks(f.read_text(encoding='utf-8', errors='replace'))
        for m in re.finditer(r'^extern\s+([\w \*]+?)\s*\b(\w+)\s*(\[[^\]]*\])?\s*;', s, re.M):
            t = ' '.join((m.group(1) + ('[]' if m.group(3) else '')).replace('*', ' * ').split())
            ext[m.group(2)][f.name] = t
        for m in re.finditer(r'typedef\s+(?:struct|union|enum)\s*\w*\s*\{.*?\}\s*(\w+)\s*;', s, re.S):
            tdefs[m.group(1)].append(f.name)
        raw['extern'] += len(re.findall(r'^\s*extern\b', s, re.M))
        raw['typedef'] += len(re.findall(r'^\s*typedef\b', s, re.M))
    conflicts = {n: d for n, d in ext.items() if len(set(d.values())) > 1}
    dups = {n: fs for n, fs in tdefs.items() if len(fs) > 1}
    return conflicts, dups, raw


PAD = re.compile(r'^(unk|pad|_)', re.I)              # filler: says nothing about the layout
MIN_SAME, MIN_SAME_IDENTICAL = 8, 5                  # fields in common before a pair is reported
from hosttools import mwcc                           # noqa: E402
CC = mwcc()


def struct_defs(text):
    """(C name, [(field, type, is_array)]) for each struct defined with a body in text."""
    text = re.sub(r'/\*.*?\*/', '', re.sub(r'//[^\n]*', '', text), flags=re.S)
    out = []
    for m in re.finditer(r'\b(typedef\s+)?struct\s*(\w*)\s*\{', text):
        depth, i = 1, m.end()
        while depth and i < len(text):
            depth += {'{': 1, '}': -1}.get(text[i], 0)
            i += 1
        body, tail = text[m.end():i - 1], text[i:i + 200]
        if m.group(1):
            n = re.match(r'\s*(\w+)\s*;', tail)
            if not n:
                continue
            name = n.group(1)
        elif m.group(2) and re.match(r'\s*;', tail):
            name = 'struct ' + m.group(2)
        else:
            continue                                  # an anonymous or nested member struct
        fields, depth, cur = [], 0, ''
        for ch in body:
            depth += {'{': 1, '}': -1}.get(ch, 0)
            if ch == ';' and depth == 0:
                fields += member(cur.strip())
                cur = ''
            else:
                cur += ch
        out.append((name, fields))
    return out


def member(stmt):
    """[(name, type, is_array)] declared by one member statement; bit-fields are left out."""
    if '{' in stmt:                                   # a nested struct/union with a member name
        n = re.search(r'\}\s*(\w+)\s*(\[.*\])?\s*$', stmt)
        return [(n.group(1), None, bool(n.group(2)))] if n else []
    if re.search(r'[^:]:[^:]', stmt) or stmt.startswith(('typedef', '#')):
        return []
    fp = re.search(r'\(\s*\*\s*(\w+)\s*\)', stmt)
    if fp:
        return [(fp.group(1), None, False)]
    parts = stmt.split(',')
    first = re.match(r'^(.*?)(\**)\s*(\w+)\s*((\[[^\]]*\])*)$', parts[0].strip())
    if not first:
        return []
    base = ' '.join(first.group(1).split())
    out = [(first.group(3), None if first.group(2) else base, bool(first.group(4)))]
    for p in parts[1:]:
        d = re.match(r'^(\**)\s*(\w+)\s*((\[[^\]]*\])*)$', p.strip())
        if d:
            out.append((d.group(2), None if d.group(1) else base, bool(d.group(3))))
    return out


def measure(prefix, structs, workdir):
    """{C name: (size, [(field, offset, size, element size)])} from the compiler itself: the
    structs' offsets and sizes are written into a table after `prefix` and read back from the
    object. A struct the compiler rejects (a member name we misread) is dropped and retried."""
    import subprocess
    sys.path.insert(0, str(ROOT / 'tools/match'))
    from constcheck import Elf
    from lint import CFLAGS
    flags = [f for f in CFLAGS if f not in ('-w', 'all')] + ['-w', 'off', '-maxerrors', '1000']
    todo = dict(structs)
    while todo:
        names = sorted(todo)
        rows, where = [], {}
        for k, name in enumerate(names):
            items = ['sizeof(%s)' % name]
            for f, _, arr in todo[name]:
                acc = '((%s*)0)->%s' % (name, f)
                items += ['(unsigned int)&%s' % acc, 'sizeof(%s)' % acc,
                          'sizeof(%s[0])' % acc if arr else 'sizeof(%s)' % acc]
            where[len(prefix.split('\n')) + len(rows) + 1] = name
            rows.append('const unsigned int LAYOUT_%d[] = {%s};' % (k, ', '.join(items)))
        src, obj = workdir / 'layout.c', workdir / 'layout.o'
        src.write_text(prefix + '\n' + '\n'.join(rows) + '\n', encoding='utf-8')
        r = subprocess.run(CC + flags + ['-c', str(src), '-o', str(obj)], cwd=ROOT,
                           capture_output=True, text=True)
        if r.returncode == 0:
            break
        bad = {where.get(int(m.group(1))) for m in
               re.finditer(r'layout\.c:(\d+): (?!warning)', r.stdout + r.stderr)} - {None}
        if not bad:
            return {}
        for b in bad:
            del todo[b]
    else:
        return {}
    elf = Elf(obj)
    out = {}
    for k, name in enumerate(names):
        sym = elf.globals.get('LAYOUT_%d' % k)
        sec = elf.secs[sym['shndx']]
        vals = struct.unpack_from('>%dI' % (sym['size'] // 4), elf.b, sec['off'] + sym['value'])
        fields = [(f[0], vals[1 + 3 * j], vals[2 + 3 * j], vals[3 + 3 * j]) for j, f in enumerate(todo[name])]
        out[name] = (vals[0], fields)
    return out


def layout_dups():
    """Pairs of struct types with the same size whose fields agree wherever both name one (padding
    `unk`/`pad` members say nothing) and whose kinds agree (int, float, pointer), sharing at least
    MIN_SAME named fields (MIN_SAME_IDENTICAL when the layouts are the same throughout) and over
    half of the smaller one's: likely one piece of data under two names. Offsets and sizes come
    from the compiler (measure). Returns (pairs, names that could not be measured)."""
    import tempfile
    skip = ('asm_types.h', '__ppc_eabi_linker.h', 'NMWException.h', 'fdlibm.h', 'types.h', 'GameVersions.h')
    headers = sorted(p for p in (ROOT / 'include').rglob('*.h') if p.name not in skip)   # not the SDK's
    hdefs, types = [], {}
    for h in headers:
        for name, fields in struct_defs(h.read_text(encoding='utf-8', errors='replace')):
            hdefs.append((name, fields))
            types[name] = (h.relative_to(ROOT / 'include').as_posix(), fields)
    prefix = '\n'.join('#include "%s"' % h.relative_to(ROOT / 'include').as_posix() for h in headers)
    with tempfile.TemporaryDirectory() as tmp:
        tmp = pathlib.Path(tmp)
        sizes = {n: (types[n][0], s) for n, s in measure(prefix, dict(hdefs), tmp).items()}
        for f in sorted((ROOT / 'src').glob('*.c')):
            text = sweepblock.strip_blocks(f.read_text(encoding='utf-8', errors='replace'))
            local = struct_defs(text)
            if local:
                for n, s in measure(text, dict(local), tmp).items():
                    sizes.setdefault(n, (f.name, s))
                    types.setdefault(n, (f.name, dict(local)[n]))
    ftype = {n: {f: t for f, t, _ in fl} for n, (_, fl) in types.items()}

    def kind(t):
        if t is None:
            return 'pointer'
        if re.search(r'\b(f32|f64|float|double)\b', t):
            return 'float'
        if re.search(r'\b(struct|union)\b', t) or t in types:
            return t                                  # a struct we could not flatten: by name
        return 'int'                                  # signedness and spelling ignored

    def leaves(name, base=0, depth=0):
        """{offset: (size, kind, in an array)} of the named leaf fields, nested structs and arrays
        flattened."""
        out = {}
        for f, off, size, elem in sizes[name][1][1]:
            if PAD.match(f):
                continue
            t = ftype.get(name, {}).get(f)
            for k in range(min(size // elem if elem else 1, 256)):
                o = base + off + k * elem
                if t in sizes and depth < 4 and sizes[t][1][0] == elem:
                    out.update(leaves(t, o, depth + 1))
                else:
                    out[o] = (elem, kind(t), elem != size)
        return out

    flat = {n: leaves(n) for n in sizes}
    pairs = []
    names = sorted(sizes)
    for i, a in enumerate(names):
        for b in names[i + 1:]:
            if sizes[a][1][0] != sizes[b][1][0] or sizes[a][1][0] < 8:
                continue
            A, B = flat[a], flat[b]
            # agreement counts named fields; an array's elements (u32 a[20]) fit any run of words
            same = [o for o in A if o in B and A[o][:2] == B[o][:2] and not (A[o][2] and B[o][2])]
            # a handful of plain fields (three floats, four words) is too common a shape to mean
            # anything; ask for more agreement unless one layout is exactly the other
            ident = {o: v[:2] for o, v in A.items()} == {o: v[:2] for o, v in B.items()}
            need = MIN_SAME_IDENTICAL if ident else MIN_SAME
            if len(same) < need or len(same) * 2 < min(len(A), len(B)):
                continue
            spans = sorted([(o, o + v[0], 'a', v[1]) for o, v in A.items()]
                           + [(o, o + v[0], 'b', v[1]) for o, v in B.items()])
            clash = False
            for j, x in enumerate(spans):             # fields that overlap unequally or differ in kind
                for y in spans[j + 1:]:
                    if y[0] >= x[1]:
                        break
                    if x[2] != y[2] and (x[:2] != y[:2] or x[3] != y[3]):
                        clash = True
            if not clash:
                pairs.append((a, sizes[a][0], b, sizes[b][0], sizes[a][1][0], len(same), ident))
    return pairs, sorted('%s (%s)' % (n, types[n][0]) for n in types if n not in sizes)


if '--layout' in sys.argv:
    pairs, skipped = layout_dups()
    if '--count' in sys.argv:
        print(len(pairs))
        sys.exit(0)
    print('struct types with the same layout under different names: %d pairs' % len(pairs))
    for a, fa, b, fb, size, same, ident in pairs:
        print('  %s (%s) = %s (%s): 0x%X bytes, %d fields agree%s'
              % (a, fa, b, fb, size, same, ', identical' if ident else ''))
    if skipped:
        print('not measured (the compiler rejected the table for them): ' + ', '.join(skipped))
    sys.exit(0)

conflicts, dups, raw = audit()
if '--count' in sys.argv:
    print(len(conflicts), len(dups), raw['extern'], raw['typedef'])
    sys.exit(0)
print('globals declared with different types:', len(conflicts))
for n, d in sorted(conflicts.items()):
    print('  %s: %s' % (n, '; '.join('%s %s' % (k, v) for k, v in sorted(d.items()))))
print('types defined in more than one file:', len(dups))
for n, fs in sorted(dups.items()):
    print('  %s: %s' % (n, ', '.join(fs)))
print('extern declarations in .c files: %d, typedefs in .c files: %d (both belong in headers)'
      % (raw['extern'], raw['typedef']))
sys.exit(1 if conflicts or dups else 0)
