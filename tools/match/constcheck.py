"""Compare the VALUES of the constants and data our matched functions load with the original's.
    python tools/match/constcheck.py                 summary by unit, then every mismatch
    python tools/match/constcheck.py --unit Ball     one unit only (Ball, Ball.c or main/Ball)
    python tools/match/constcheck.py --count         one number: the mismatches (for merges)
    python tools/match/constcheck.py --all-fns       also functions below 100% (paired by offset, so a
                                                     moved instruction can pair the wrong constants)
Run it after a build (it reads build/GW4E69/report.json and the objects).

Why: objdiff masks relocations, so `lfs f1, @24@sda21(r2)` scores 100% whatever float @24 holds.
A literal a few bits off (0.0138889f where the original has 1/72) is invisible to the score and
to the DOL check until the unit is linked, but a port inherits it. Linked units are safe: the DOL
hash proves their data.

How: for each function objdiff scores 100%, the relocations of our object (build/GW4E69/src/X.o)
and of the original's split object (build/GW4E69/obj/X.o) line up offset by offset. For each one
that points at data, the bytes at symbol + addend are read on both sides and compared:
  - a load or store (lfs/lwz/stfs 4 bytes, lfd/stfd 8, lhz 2, lbz 1, psq_l 4 or 8) reads its width;
  - an address (addi/ori with @l or @sda21) compares the whole object from the addend on (a string
    up to its NUL).
A symbol the object does not define is looked up among the other objects' globals: ours in
build/GW4E69/src, falling back to the original's (then it is the original's data, equal by
construction); the original's in build/GW4E69/obj. .bss/.sbss read as zeros.

Mismatches are grouped as float (read by a float instruction), string, and data (the rest). For a
float the original's value is also tried as a/q (a an integer or a short decimal, q <= 1000), times
1, PI, PI/180 (golfer.h's DEG) or 180/PI, evaluated in double and in float; the simplest exact
reproduction is suggested (`= 1/72?`), since that is usually how EA wrote it. A literal typed out
to a few digits (0.0138889f) is the usual culprit: write the expression instead (1.0f / 72.0f)."""
import collections, json, math, pathlib, struct, sys

ROOT = pathlib.Path(__file__).resolve().parents[2]   # the checkout this script lives in
BUILD = ROOT / 'build/GW4E69'

R_PPC_ADDR16_LO, R_PPC_ADDR16_HI, R_PPC_ADDR16_HA, R_PPC_EMB_SDA21 = 4, 5, 6, 109
DATA_RELOCS = (R_PPC_ADDR16_LO, R_PPC_EMB_SDA21)     # @ha/@hi halves are covered by their @l
SHT_SYMTAB, SHT_RELA, SHT_NOBITS, SHF_EXECINSTR = 2, 4, 8, 4
STT_SECTION, STT_FUNC = 3, 2


class Elf:
    """Just enough of a big-endian ELF32 relocatable object."""

    def __init__(self, path):
        self.path = path
        b = self.b = path.read_bytes()
        shoff, = struct.unpack_from('>I', b, 0x20)
        shentsize, shnum, shstrndx = struct.unpack_from('>HHH', b, 0x2E)
        raw = [struct.unpack_from('>IIIIIIIIII', b, shoff + i * shentsize) for i in range(shnum)]
        strtab = raw[shstrndx]
        self.secs = []
        for s in raw:
            self.secs.append({'name': self._str(strtab[4], s[0]), 'type': s[1], 'flags': s[2],
                              'off': s[4], 'size': s[5], 'link': s[6], 'info': s[7]})
        self.syms, self.globals = [], {}
        for s in self.secs:
            if s['type'] != SHT_SYMTAB:
                continue
            stroff = self.secs[s['link']]['off']
            for off in range(s['off'], s['off'] + s['size'], 16):
                name, value, size, info, _, shndx = struct.unpack_from('>IIIBBH', b, off)
                sym = {'name': self._str(stroff, name), 'value': value, 'size': size,
                       'type': info & 15, 'bind': info >> 4, 'shndx': shndx}
                self.syms.append(sym)
                if sym['bind'] != 0 and 0 < shndx < len(self.secs) and sym['name']:
                    self.globals.setdefault(sym['name'], sym)
        self._next = {}

    def _str(self, base, off):
        end = self.b.index(b'\0', base + off)
        return self.b[base + off:end].decode('latin-1')

    def relocs(self, secidx):
        """offset -> (type, symbol, addend) for the relocations applied to section secidx."""
        out = {}
        for s in self.secs:
            if s['type'] == SHT_RELA and s['info'] == secidx:
                for off in range(s['off'], s['off'] + s['size'], 12):
                    r_off, info, addend = struct.unpack_from('>IIi', self.b, off)
                    out[r_off] = (info & 0xFF, self.syms[info >> 8], addend)
        return out

    def func(self, name):
        for s in self.syms:
            if s['name'] == name and s['type'] == STT_FUNC and 0 < s['shndx'] < len(self.secs):
                return s
        return None

    def word(self, secidx, off):
        return struct.unpack_from('>I', self.b, self.secs[secidx]['off'] + off)[0]

    def extent(self, sym):
        """Size of a symbol; for a size-0 label (or a section symbol), up to the next symbol."""
        if sym['size'] and sym['type'] != STT_SECTION:
            return sym['size']
        idx = sym['shndx']
        if idx not in self._next:
            self._next[idx] = sorted({s['value'] for s in self.syms if s['shndx'] == idx}
                                     | {self.secs[idx]['size']})
        if sym['type'] == STT_SECTION:
            return self.secs[idx]['size']
        return next((v for v in self._next[idx] if v > sym['value']), self.secs[idx]['size']) - sym['value']

    def read(self, sym, addend):
        """(section name, bytes from symbol + addend to the end of the symbol) or None."""
        idx = sym['shndx']
        if not 0 < idx < len(self.secs):
            return None
        sec = self.secs[idx]
        if sec['flags'] & SHF_EXECINSTR:
            return None
        start = sym['value'] + addend
        n = max(0, sym['value'] + self.extent(sym) - start)
        if sec['type'] == SHT_NOBITS:
            return sec['name'], bytes(n)
        return sec['name'], self.b[sec['off'] + start:sec['off'] + start + n]


class Index:
    """Global data symbols of every object under a folder, loaded on first use."""

    def __init__(self, folder, fallback=None):
        self.folder, self.fallback, self.map = folder, fallback, None

    def lookup(self, name):
        if self.map is None:
            self.map = {}
            for p in sorted(self.folder.rglob('*.o')):
                try:
                    elf = Elf(p)
                except (struct.error, ValueError, IndexError):
                    continue
                for n, s in elf.globals.items():
                    self.map.setdefault(n, (elf, s))
        hit = self.map.get(name)
        if hit is None and self.fallback:
            return self.fallback.lookup(name)
        return hit


ORIG = Index(BUILD / 'obj')
OURS = Index(BUILD / 'src', ORIG)
_cache = {}


def load(path):
    if path not in _cache:
        _cache[path] = Elf(path)
    return _cache[path]


def resolve(elf, sym, index):
    """The object and symbol that define sym (a local, or a global from another object)."""
    if 0 < sym['shndx'] < len(elf.secs):
        return elf, sym
    hit = index.lookup(sym['name'])
    return hit if hit else (None, None)


def access(insn):
    """(width, is_float) for a load/store, or (None, False) for an address computation."""
    op = insn >> 26
    widths = {32: 4, 33: 4, 34: 1, 35: 1, 36: 4, 37: 4, 38: 1, 39: 1, 40: 2, 41: 2, 42: 2, 43: 2,
              44: 2, 45: 2}
    if op in widths:
        return widths[op], False
    if op in (48, 49, 52, 53):
        return 4, True
    if op in (50, 51, 54, 55):
        return 8, True
    if op in (56, 57, 60, 61):                       # psq_l/psq_st: W=1 is one value
        return (4 if (insn >> 15) & 1 else 8), True
    return None, False


def is_string(data):
    nul = data.find(b'\0')
    return nul > 0 and all(32 <= c < 127 or c in (9, 10, 13) for c in data[:nul])


def f32(x):
    return struct.unpack('>f', struct.pack('>f', x))[0]


PI_F = f32(3.14159265)                               # golfer.h's PI
# (format, double scale, float scale, extra cost): DEG(x) is x * (PI / 180.0f) in golfer.h
SCALES = (('{}', 1.0, 1.0, 0), ('PI*{}', math.pi, PI_F, 50),
          ('DEG({})', math.pi / 180, f32(PI_F / f32(180.0)), 50),
          ('{}*180/PI', 180 / math.pi, f32(f32(180.0) / PI_F), 50))


def fraction(bits, width):
    """The simplest a/q (a an integer or a decimal of up to 3 places, q <= 1000), optionally times
    PI, PI/180 or 180/PI, that reproduces the value exactly, evaluated in double or in float the
    way a `0.924f / 36.0f` would be; e.g. '1/72', 'PI*1/6', '0.924/36'. None if nothing short fits."""
    v = struct.unpack('>f' if width == 4 else '>d', bits)[0]
    if not math.isfinite(v) or v == 0 or v == int(v):
        return None

    def exact(x):
        try:
            return struct.pack('>f' if width == 4 else '>d', x) == bits
        except OverflowError:
            return False

    best = None
    for fmt, scale, fscale, extra in SCALES:
        for k in range(4):
            for q in range(1, 1001):
                p = round(v / scale * q * 10 ** k)
                if p == 0 or (k and p % 10 == 0):
                    continue
                cost = abs(p) + q + extra + 100 * k
                if cost > 1000 or (best and cost >= best[0]):
                    continue
                a = p / 10 ** k
                # double, then float arithmetic (a double can hold a float-precision value too)
                tries = [a / q * scale, a * scale / q,
                         f32(f32(f32(a) / f32(q)) * fscale), f32(f32(f32(a) * fscale) / f32(q))]
                if any(exact(x) for x in tries):
                    num = f'{p}' if k == 0 else f'{a:.{k}f}'
                    best = (cost, fmt.format(num if q == 1 else f'{num}/{q}'))
    return best[1] if best else None


def show(data, width, flt):
    h = '0x' + data.hex().upper()
    if flt and width == 4:
        return f'{h} ({struct.unpack(">f", data)[0]:.9g})'
    if flt and width == 8 and len(data) == 8:
        return f'{h} ({struct.unpack(">d", data)[0]:.17g})'
    if is_string(data):
        return repr(data[:data.find(b"\0")].decode("latin-1"))
    return h if len(data) <= 32 else h[:66] + f'... ({len(data)} bytes)'


def compare(ours, orig, fn, all_fns):
    """Mismatches in one function: (offset, kind, ours text, orig text)."""
    fa, fb = ours.func(fn), orig.func(fn)
    if not fa or not fb:
        return []
    ra, rb = ours.relocs(fa['shndx']), orig.relocs(fb['shndx'])
    out = []
    size = min(fa['size'], fb['size'])
    # @sda21 sits on the instruction, @l on its low half (instruction + 2)
    for rel in sorted({o - fa['value'] for o in ra if 0 <= o - fa['value'] < size}):
        a, b = ra.get(fa['value'] + rel), rb.get(fb['value'] + rel)
        if not a or not b or a[0] != b[0] or a[0] not in DATA_RELOCS:
            continue
        insn = rel & ~3
        insn_a, insn_b = ours.word(fa['shndx'], fa['value'] + insn), orig.word(fb['shndx'], fb['value'] + insn)
        if all_fns and insn_a >> 26 != insn_b >> 26:
            continue
        ea, sa = resolve(ours, a[1], OURS)
        eb, sb = resolve(orig, b[1], ORIG)
        if not ea or not eb:
            continue
        da, db = ea.read(sa, a[2]), eb.read(sb, b[2])
        if not da or not db:
            continue
        width, flt = access(insn_b)
        da, db = da[1], db[1]
        if width:
            da, db = da[:width], db[:width]
        elif is_string(db) or is_string(da):
            da, db = da[:da.find(b'\0') + 1 or None], db[:db.find(b'\0') + 1 or None]
        else:
            n = min(len(da), len(db))
            da, db = da[:n], db[:n]
        if da == db:
            continue
        kind = 'float' if flt else 'string' if (is_string(da) or is_string(db)) and not width else 'data'
        a_txt, b_txt = show(da, width, flt), show(db, width, flt)
        if flt and len(db) in (4, 8):
            guess = fraction(db, len(db))
            if guess:
                b_txt = b_txt[:-1] + f' = {guess}?)'
        out.append((insn, kind, a_txt, b_txt))
    return out


def units(wanted):
    report = json.loads((BUILD / 'report.json').read_text())
    objd = {u['name']: u for u in json.loads((ROOT / 'objdiff.json').read_text())['units']}
    for u in report['units']:
        cfg = objd.get(u['name'], {})
        if not cfg.get('base_path') or not cfg.get('target_path'):
            continue
        src = u.get('metadata', {}).get('source_path') or cfg['metadata'].get('source_path', '')
        label = src[4:] if src.startswith('src/') else src
        short = u['name'].split('/', 1)[-1]
        if wanted and not ({short, label, u['name'], src, label.rsplit('.', 1)[0]} & wanted):
            continue
        yield label, ROOT / cfg['base_path'], ROOT / cfg['target_path'], u.get('functions', [])


def main():
    args = sys.argv[1:]
    count = '--count' in args
    all_fns = '--all-fns' in args
    wanted = set()
    it = iter(args)
    for a in it:
        if a == '--unit':
            wanted.add(next(it))
        elif not a.startswith('--'):
            wanted.add(a)
    results = collections.OrderedDict()
    for label, ours_p, orig_p, fns in units(wanted):
        if not ours_p.exists() or not orig_p.exists():
            continue
        ours, orig = load(ours_p), load(orig_p)
        found = []
        for f in fns:
            if f.get('fuzzy_match_percent', 0) < 100.0 and not all_fns:
                continue
            for m in compare(ours, orig, f['name'], all_fns):
                found.append((f['name'],) + m)
        results[label] = found
    total = sum(len(v) for v in results.values())
    if count:
        print(total)
        return
    kinds = collections.Counter(m[2] for v in results.values() for m in v)
    print(f'{total} mismatches in {sum(1 for v in results.values() if v)} of {len(results)} units '
          f'({kinds["float"]} float, {kinds["string"]} string, {kinds["data"]} data)')
    for label, found in sorted(results.items(), key=lambda kv: -len(kv[1])):
        if found:
            c = collections.Counter(m[2] for m in found)
            distinct = len({(m[2], m[3], m[4]) for m in found})
            print(f'  {label:40} {len(found):4}  (float {c["float"]}, string {c["string"]}, '
                  f'data {c["data"]}; {distinct} distinct)')
    for kind in ('float', 'string', 'data'):
        rows = [(label, m) for label, found in results.items() for m in found if m[2] == kind]
        if rows:
            print(f'\n{kind} mismatches ({len(rows)}):')
            for label, (fn, rel, _, a, b) in rows:
                print(f'{label} {fn} +0x{rel:X}: ours {a} orig {b}')
    sys.exit(1 if total else 0)


if __name__ == '__main__':
    main()
