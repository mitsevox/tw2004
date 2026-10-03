"""Pair our unnamed functions (fn_XXXXXXXX) with EA's names in the reference builds: a SURVEY, it
renames nothing.
    python tools/naming/pairnames.py                      summary (counts per confidence)
        [--per-unit]                                     ... and per unit
        [--globals]                                      ... and the lbl_ variables (globals_survey)
        [--tsv <out.tsv> [--only-proposals]]             one row per fn_ function (or per proposal)
    python tools/naming/pairnames.py --explain <fn|addr>  every candidate of one function, with its score
    python tools/naming/pairnames.py --units              the unit -> reference file mapping and why
    python tools/naming/pairnames.py --fields             struct fields: placeholder names (nC38, f2C)
        against the TW06 reference layouts (pair_fields)
    python tools/naming/pairnames.py --holdout 0.5 [--seed N]
        precision test: hides that share of the functions already named after a reference name,
        re-runs the pairing, and reports how often each confidence level gets the hidden name back
Run after a build (it reads the split objects in build/GW4E69/obj, like callgraph.py). ~30 s.

References (reference/, derived inventories only):
  tw07   TW07 PS3 DWARF, cu/<file>.txt: every function per EA source file, in source order, with its
         PS3 size, parameters (C++ `this` included), locals and the functions it inlines.
  tw06p  TW06 PS2 EA_DASH.ELF stabs, functions.cpp: EA's shared packages (SharedFileIO, TagFile,
         ChecksumCRC32, TibExt, ...), per source file in address order, with sizes and parameters.
Hints taken as extra evidence (never alone): docs/evidence/tw06-names.md suggestions not yet applied
(TW06 Xbox PDB / PS2 map pairing, code E2), reference/tw07-ps3/pairs.tsv (the PC pairing
that saw TW07's call graph and strings), reference/007eon-ps2/shared_functions.tsv.

Method, per unit (our .o = one EA source file, functions in source order):
 1. Anchors: our functions that already carry a reference name (exact, `::` -> `_`). They give the
    unit's reference file(s) (with the unit's file name and any `<file>.c` string in the unit) and
    fix positions: CodeWarrior emits a file's functions in source order, so a function between two
    anchors pairs with a reference function between the same two anchors.
 2. Every other function of the unit is scored against every free function of those files:
      sig   parameter count and kinds (float / pointer / integer, byte and halfword widths count;
            `this` may be dropped or kept). sig = all agree, sigw = an integer width differs,
            sig~ = one kind differs, sig! = the count differs
      ret   return kind
      size  size ratio against the reference's median ratio (PS3 ~1.6x GameCube, measured on the
            anchors; TW06 PS2's EA_DASH copies are often stubs, so no size line there);
            inl-only = TW07 inlined every call, no size
      inl   callees of ours that the reference function inlines (by name, or by an earlier pairing)
      nbr   a paired caller of ours whose reference function inlines this candidate
      E1    one of our strings contains the candidate's name (EA's own text)
      p07   pairs.tsv (PC pairing with TW07 call graph and strings) agrees; E2 tw06-names.md agrees
    then a monotone alignment (dynamic programming) inside each gap between anchors picks the set of
    pairs with the best total score. Margin = how much the gap's best total drops when that pair is
    forbidden (score and position together). The unit's own ends count as anchors (pos=both) when
    a string in the unit names the file, or the unit's file name matches and an anchor of that file
    confirms it, or (coh) the whole unit aligns with the file: 5+ pairs, 70% with agreeing
    signatures, covering 40% of the unit. runN = the pair sits in N consecutive pairs on both sides
    (anchors included) with agreeing signatures: the order itself is then the evidence.
 3. Confidence:
      A  E1 names it; or bracketed on both sides, signature exact, size plausible, margin >= 3 and
         (a 1:1 gap, an independent line: inl / nbr / p07 / E2 / 007, or margin >= 5); or a B inside
         a run of 6+ with an exact signature and a plausible size. Never A on an inline-only
         reference function.
      B  bracketed, signature not contradicted, size plausible, margin >= 1.5; or one-sided bracket
         with an agreeing signature and margin >= 2.5; in both cases with a second line (run of 4+,
         a gap of at most 3 of ours, an independent line or margin >= 3), else C (code `weak`). A
         bracketed C inside a run of 4+ is raised to B.
      C  anything else the alignment picked (provisional)
    A and B pairs become anchors for the next round (up to --rounds, default 6); a reference name
    claimed twice keeps the stronger claim only (the other gets `dup`, C).
 4. Units with no reference file of their own take the files of their paired callers/callees
    (neighbour vote); their pairs are capped at C unless bracketed.
Measured (2026-09-27, --holdout 0.5, three seeds): A 99%, B 91-96%, C 64-70% get the hidden name
back; a hand check of 30 A/B pairs on real fn_ functions: docs/evidence/notes/2026-09-27-name-pairing.md.
Codes in the output follow docs/style.md: E2b = TW07 name, E2 = TW06 name, E1 = EA's text, then the
machine lines above. Every proposal is a CANDIDATE for the audit process (docs/style.md "Where names
and comments come from"), never a rename.
Not scored: IStudio (UIS*, UIStudio), the MAD decoder (maddec*, madidct) and runtime/MSL units (lane n2 names them from
Madden 2003 / NFSMW); they are counted separately (--all includes them)."""
import collections, json, math, pathlib, random, re, statistics, sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import callgraph as cg  # noqa: E402

ROOT = cg.ROOT
REF = ROOT / 'reference'
SKIP = re.compile(r'^(UIS\w*|UIStudio|maddeca?|madidct|runtime/.*|src/MSL_C/.*)$')

# ---------------------------------------------------------------------------------------- types
FLOAT = re.compile(r'\b(float|double|f32|f64|real32|real64)\b')


def kind(t):
    """'f' float, 'p' pointer / array / reference / vector typedef, 'i' integer / enum / bool,
    'v' void, 's' struct by value, '?' unknown."""
    t = re.sub(r'\b(const|volatile|static|inline|register|struct|enum|union|unsigned|signed)\b', ' ',
               t or '').strip()
    if not t:
        return '?'
    if '*' in t or '&' in t or '[' in t:
        return 'p'
    if re.match(r'^void$', t):
        return 'v'
    if FLOAT.search(t):
        return 'f'
    if re.search(r'\b(vec\d?\w*|Vec\w*|mat\d\w*|Mtx\w*|vec4flt|Matrix\w*|Quat\w*|va_list)\b', t):
        return 'p'
    # integer widths: 'b' byte, 'h' halfword, 'B' bool (a byte in TW07's C++, a word or a byte in
    # 2003's C), 'i' word (int, long, enums, EA's typedefs)
    if re.search(r'^(bool)$', t):
        return 'B'
    if re.search(r'^(char|u8|s8|uint8|int8|uint8_t|int8_t|byte|char8_t|BOOL8|u_char)$', t):
        return 'b'
    if re.search(r'^(short|short int|u16|s16|uint16|int16|uint16_t|int16_t|wchar_t|char16_t)$', t):
        return 'h'
    if re.search(r'\b(int|long|short|char|bool|BOOL|u?int\d+(_t)?|[su]\d+|uint|byte|size_t|char8_t|'
                 r'wchar_t|\w+_t|\w+_e|Club|Lie|Bool)\b', t) or t.endswith('_t'):
        return 'i'
    return '?'


def split_params(s):
    s = re.sub(r'/\*.*?\*/', '', s).strip()
    if s in ('', 'void'):
        return []
    out, depth, cur = [], 0, ''
    for c in s:
        if c in '(<[':
            depth += 1
        elif c in ')>]':
            depth -= 1
        if c == ',' and depth == 0:
            out.append(cur.strip())
            cur = ''
        else:
            cur += c
    if cur.strip():
        out.append(cur.strip())
    return [p for p in out if p != '...']


def c_candidate(name):
    """False for names our C cannot have: constructors, destructors, operators, new/delete."""
    parts = [p for p in re.sub(r'<.*>', '', name).split('::') if p]
    last = parts[-1] if parts else ''
    return bool(last) and not last.startswith('~') and not last.startswith('operator') \
        and last not in ('new', 'delete') and not (len(parts) >= 2 and parts[-2] == last)


def cname(name):
    """Reference name as it is written in our C: namespaces dropped, `Class::Method` ->
    `Class_Method`."""
    name = re.sub(r'<.*>', '', name)
    parts = [p for p in name.split('::') if p]
    if len(parts) > 2:
        parts = parts[-2:]
    return '_'.join(parts)


class RefFunc:
    __slots__ = ('ref', 'file', 'order', 'line', 'name', 'cname', 'size', 'ret', 'params', 'this',
                 'locals', 'inlines', 'key')

    def __init__(self, **kw):
        for k in self.__slots__:
            setattr(self, k, kw.get(k))
        self.key = '%s:%s:%s' % (self.ref, self.file, self.order)

    def sig(self):
        return '%s %s(%s)' % (self.ret, self.name, ', '.join(self.params if not self.this
                                                              else ['this'] + self.params))


def load_tw07():
    funcs = []
    head = re.compile(r'^ +(\d+) +(inline-only|([0-9A-F]+) +([0-9A-F]+)) +(.*)$')
    for p in sorted((REF / 'tw07-ps3/cu').glob('*.txt')):
        order, cur = 0, None
        for l in p.read_text(encoding='utf-8', errors='replace').splitlines():
            m = head.match(l)
            if m:
                decl = m.group(5)
                d = re.match(r'^(static\s+)?(.*?)\s*\b([\w:~<>,]+)\((.*)\)\s*$', decl)
                if not d or '<' in d.group(3):
                    cur = None
                    continue
                params = split_params(d.group(4))
                this = bool(params) and re.search(r'\bthis$', params[0]) is not None
                if this:
                    params = params[1:]
                cur = RefFunc(ref='tw07', file=p.name[:-4], order=order, line=int(m.group(1)),
                              name=d.group(3), cname=cname(d.group(3)),
                              size=int(m.group(4), 16) if m.group(3) else None,
                              ret=d.group(2).strip(), params=params, this=this, locals=[], inlines=[])
                if c_candidate(d.group(3)):
                    funcs.append(cur)
                    order += 1
                continue
            if cur is None:
                continue
            s = l.strip()
            if s.startswith('local '):
                cur.locals.append(s[6:].strip())
            elif s.startswith('inlines '):
                cur.inlines += [cname(x.strip()) for x in s[8:].split(',') if x.strip()]
    return funcs


def load_tw06p():
    funcs, cur, order = [], None, 0
    # a function with locals opens a block ("{" alone; the locals follow on their own lines)
    line = re.compile(r'^/\* ([0-9a-f]+) ([0-9a-f]+) \*/ (.*?)\b([\w:~]+)\((.*)\) \{\}?\s*$')
    for l in (REF / 'tw06-ps2/functions.cpp').read_text(encoding='utf-8', errors='replace').splitlines():
        m = re.match(r'// FILE -- (.*)', l)
        if m:
            cur, order = re.split(r'[\\/]', m.group(1).strip())[-1], 0
            continue
        m = line.match(l)
        if not m or cur is None or not c_candidate(m.group(4)) or ' type_info function' in l:
            continue
        params = split_params(m.group(5))
        this = bool(params) and re.search(r'\bthis$', params[0]) is not None
        if this:
            params = params[1:]
        params = [p for p in params if not p.endswith('__in_chrg')]
        funcs.append(RefFunc(ref='tw06p', file=cur, order=order, line=int(m.group(1), 16),
                             name=m.group(4), cname=cname(m.group(4)), size=int(m.group(2), 16),
                             ret=m.group(3).strip(), params=params, this=this, locals=[], inlines=[]))
        order += 1
    return funcs


# ------------------------------------------------------------------------------------ our side
class Our:
    __slots__ = ('name', 'unit', 'addr', 'size', 'calls', 'strs', 'data', 'ret', 'params', 'callers',
                 'hidden')

    def __init__(self, **kw):
        for k in self.__slots__:
            setattr(self, k, kw.get(k))

    @property
    def unnamed(self):
        return self.name.startswith('fn_')


def c_signatures():
    """name -> (return type, [parameter declarations]) from every definition in src/."""
    out = {}
    pat = re.compile(r'^(?:asm\s+)?([A-Za-z_][\w \*]*?[\s\*])([A-Za-z_]\w*)\s*\(([^;{}]*?)\)\s*\{',
                     re.M | re.S)
    for p in (ROOT / 'src').rglob('*.c'):
        t = p.read_text(encoding='utf-8', errors='replace')
        t = re.sub(r'/\*.*?\*/', ' ', t, flags=re.S)
        t = re.sub(r'//[^\n]*', '', t)
        for m in pat.finditer(t):
            ret = m.group(1).strip()
            if ret.split()[0] in ('if', 'else', 'return', 'while', 'for', 'switch', 'do'):
                continue
            out.setdefault(m.group(2), (ret, split_params(' '.join(m.group(3).split()))))
    return out


def load_ours():
    units = collections.OrderedDict()
    callers = collections.defaultdict(set)
    addrs = cg.symbol_addresses()
    sigs = c_signatures()
    allf = {}
    for u in json.loads((ROOT / 'objdiff.json').read_text())['units']:
        if 'game' not in u['metadata'].get('progress_categories', []) or not u.get('target_path'):
            continue
        path = ROOT / u['target_path']
        if not path.exists():
            continue
        name = u['name'].split('/', 1)[1]
        funcs, calls, refs = cg.read_obj(path)
        lst = []
        for fn, off, size in funcs:
            a = addrs.get(fn, [])
            ret, params = sigs.get(fn, (None, None))
            o = Our(name=fn, unit=name, addr=a[0] if len(a) == 1 else None, size=size,
                    calls=list(calls.get(fn, [])), strs=[r[1] for r in refs.get(fn, []) if isinstance(r, tuple)],
                    data=[r for r in refs.get(fn, []) if not isinstance(r, tuple)],
                    ret=ret, params=params, callers=set(), hidden=None)
            lst.append(o)
            allf[fn] = o
            for c in o.calls:
                callers[c].add(fn)
        units[name] = sorted(lst, key=lambda o: (o.addr or 0))
    for n, o in allf.items():
        o.callers = callers.get(n, set())
    return units, allf


def load_hints():
    """address -> list of (source, reference name, strength)."""
    h = collections.defaultdict(list)
    doc = ROOT / 'docs/evidence/tw06-names.md'
    for l in doc.read_text(encoding='utf-8').splitlines():
        m = re.match(r'^\| `([0-9A-Fa-f]{8})` \| `([^`]*)` \| `([^`]*)` \| (\w+) \| (.*) \|$', l)
        if m:
            h[int(m.group(1), 16)].append(('tw06', cname(m.group(3)), m.group(4)))
    p = REF / 'tw07-ps3/pairs.tsv'
    for l in p.read_text(encoding='utf-8').splitlines()[1:]:
        c = l.split('\t')
        if len(c) >= 8:
            h[int(c[0], 16)].append(('p07', cname(c[3]), c[7]))
    p = REF / '007eon-ps2/shared_functions.tsv'
    for l in p.read_text(encoding='utf-8').splitlines()[1:]:
        c = l.split('\t')
        if len(c) >= 8 and c[6].startswith('fn_'):
            h[int(c[6][3:11], 16)].append(('007', cname(c[1]), c[7]))
    return h


# ------------------------------------------------------------------------------------- scoring
def our_kinds(o):
    if o.params is None:
        return None
    return [kind(re.sub(r'\b\w+\s*$', '', p) if re.search(r'[\w\*]\s+\**\w+$', p) else p)
            for p in o.params]


def our_ret(o):
    return None if o.ret is None else kind(re.sub(r'\b(asm|static|inline)\b', '', o.ret))


def ref_kinds(r):
    ks = []
    for p in r.params:
        t = re.sub(r'\s*\b\w+\s*$', '', p) if re.search(r'[\w\*&\]]\s+[\w]+$', p) else p
        if re.match(r'^[\w\s]*\[\d*\]\s*\w*$', p) or re.search(r'\[\d*\]', p):
            ks.append('p')
        else:
            ks.append(kind(t))
    return ks


INTS = {'i', 'b', 'h', 'B', 's'}


def compatible(a, b):
    """Same kind of register (integer / float / pointer); widths may differ (soft())."""
    return a == b or a == '?' or b == '?' or {a, b} <= INTS


def soft(a, b):
    """0 same width, 1 integer widths differ (EA changed a type between 2003 and 2006)."""
    if a == b or '?' in (a, b) or 'B' in (a, b) or 's' in (a, b):
        return 0
    return 1 if {a, b} <= INTS else 0


class Scorer:
    def __init__(self, ratio):
        self.ratio = ratio          # reference -> median log(ref size / our size)
        self.pair_of = {}           # our name -> RefFunc (fixed pairs, for inl / nbr)
        self.hints = {}

    def sig(self, o, r):
        ok = our_kinds(o)
        if ok is None:
            return 0.0, None
        rk = ref_kinds(r)
        opts = [rk] + ([['p'] + rk] if r.this else [])
        best = None
        for k in opts:
            if len(k) == len(ok):
                mism = sum(1 for a, b in zip(ok, k) if not compatible(a, b))
                sw = sum(soft(a, b) for a, b in zip(ok, k))
                narrow = sum(1 for a, b in zip(ok, k) if a == b and a in 'bh')
                if mism == 0:
                    s = 1.5 + 0.75 * min(len(k), 4) - 0.5 * sw + min(1.5, 0.5 * narrow)
                    code = 'sig' if sw == 0 else 'sigw'
                else:
                    s = 1.0 if mism == 1 else 0.0
                    code = 'sig~' if mism == 1 else 'sig?'
            else:
                d = abs(len(k) - len(ok))
                s, code = -min(4.0, 1.5 * d), 'sig!'
            if best is None or s > best[0]:
                best = (s, code)
        return best

    def ret(self, o, r):
        a, b = our_ret(o), kind(re.sub(r'\bstatic\b', '', r.ret))
        if a is None or b == '?' or a == '?':
            return 0.0
        if a == b or {a, b} <= INTS:
            return 0.5
        if 'v' in (a, b):
            return -1.0
        if 'f' in (a, b):
            return -1.0
        return -0.25

    def size(self, o, r):
        if r.size is None:
            return -0.5, 'inl-only'
        if not o.size or r.ref == 'tw06p':
            # EA_DASH's copies of the shared packages are often stubs: sizes say nothing
            return 0.0, None
        d = abs(math.log(r.size / o.size) - self.ratio[r.ref])
        if d < 0.35:
            return 1.5, 'size'
        if d < 0.7:
            return 0.5, 'size~'
        if d < 1.2:
            return -0.5, 'size?'
        return -2.0, 'size!'

    def score(self, o, r):
        """(score, codes) for our function o as reference function r."""
        codes, s = [], 0.0
        a, c = self.sig(o, r)
        s += a
        if c:
            codes.append(c)
        s += self.ret(o, r)
        a, c = self.size(o, r)
        s += a
        if c:
            codes.append(c)
        called = set(o.calls) | {self.pair_of[x].cname for x in o.calls if x in self.pair_of}
        k = len(called & set(r.inlines))
        if k:
            s += min(4.5, 1.5 * k)
            codes.append('inl%d' % k)
        nb = [x for x in o.callers if x in self.pair_of and r.cname in self.pair_of[x].inlines]
        if nb:
            s += 2.0
            codes.append('nbr')
        if any(r.cname in t or r.name in t for t in o.strs if len(r.cname) >= 6):
            s += 8.0
            codes.append('E1')
        for src, nm, st in self.hints.get(o.addr, ()):
            if nm == r.cname:
                if src == 'p07':
                    s += {'high': 2.5, 'med': 1.5}.get(st, 0.5)
                    codes.append('p07' + st[0])
                elif src == 'tw06':
                    s += 2.0
                    codes.append('E2')
                elif src == '007':
                    s += 2.0
                    codes.append('007')
        return s, codes


# ------------------------------------------------------------------------------------ pairing
INDEPENDENT = ('inl', 'nbr', 'p07h', 'p07m', 'E2', '007', 'E1')
ERA = re.compile(r'Apt|APT_|PS3|Ps3|Xenon|Singleton')


def monotone_chain(pairs):
    """Longest chain of (i, j) increasing in both (the anchors that agree on order)."""
    pairs = sorted(pairs)
    best = []
    for k, (i, j) in enumerate(pairs):
        prev = max((c for c in best if c[-1][0] < i and c[-1][1] < j), key=len, default=[])
        best.append(prev + [(i, j)])
    return max(best, key=len, default=[])


def align(cands, na, nb):
    """Max-weight monotone matching: cands = {(i, j): score>0} on na x nb."""
    if not cands:
        return []
    dp = [[0.0] * (nb + 1) for _ in range(na + 1)]
    for i in range(1, na + 1):
        row, prev = dp[i], dp[i - 1]
        for j in range(1, nb + 1):
            v = max(prev[j], row[j - 1])
            w = cands.get((i - 1, j - 1))
            if w is not None and prev[j - 1] + w > v:
                v = prev[j - 1] + w
            row[j] = v
    out, i, j = [], na, nb
    while i > 0 and j > 0:
        w = cands.get((i - 1, j - 1))
        if w is not None and abs(dp[i][j] - (dp[i - 1][j - 1] + w)) < 1e-9:
            out.append((i - 1, j - 1))
            i, j = i - 1, j - 1
        elif dp[i][j] == dp[i - 1][j]:
            i -= 1
        else:
            j -= 1
    return out[::-1]


class Pairing:
    def __init__(self, units, allf, refs, hints, include_skipped=False):
        self.units, self.allf, self.hints = units, allf, hints
        self.refs = refs
        self.by_file = collections.defaultdict(list)
        self.by_cname = collections.defaultdict(list)
        for r in refs:
            self.by_file[(r.ref, r.file)].append(r)
            self.by_cname[r.cname].append(r)
        self.include_skipped = include_skipped
        self.fixed = {}         # our name -> (RefFunc, how) : anchors and accepted pairs
        self.props = {}         # our name -> proposal dict
        self.unit_files = {}
        self.ratio = self.size_ratios()
        self.scorer = Scorer(self.ratio)
        self.scorer.hints = hints

    def active_units(self):
        return [u for u in self.units if self.include_skipped or not SKIP.match(u)]

    def size_ratios(self):
        rs = collections.defaultdict(list)
        for u, fs in self.units.items():
            for o in fs:
                if o.unnamed or not o.size:
                    continue
                cand = [r for r in self.by_cname.get(o.name, ()) if r.size]
                if len(cand) == 1:
                    rs[cand[0].ref].append(math.log(cand[0].size / o.size))
        return {k: statistics.median(v) for k, v in rs.items()} | {k: 0.0 for k in ('tw07', 'tw06p') if k not in rs}

    def anchors(self):
        """Our named functions whose name is a reference name (one reference function, or the one
        in a file the unit's other anchors agree on)."""
        for u in self.active_units():
            for o in self.units[u]:
                if o.unnamed:
                    continue
                c = self.by_cname.get(o.name, [])
                if len(c) == 1:
                    self.fixed[o.name] = (c[0], 'anchor')
        for u in self.active_units():
            votes = collections.Counter(self.fixed[o.name][0].file for o in self.units[u] if o.name in self.fixed)
            for o in self.units[u]:
                if o.unnamed or o.name in self.fixed:
                    continue
                c = [r for r in self.by_cname.get(o.name, []) if r.size is not None] or self.by_cname.get(o.name, [])
                if len(c) > 1:
                    c.sort(key=lambda r: -votes.get(r.file, 0))
                    if votes.get(c[0].file, 0) > votes.get(c[1].file, 0) or len({r.file for r in c}) == 1:
                        self.fixed[o.name] = (c[0], 'anchor')

    def map_units(self, final=False):
        files = collections.defaultdict(collections.Counter)
        why = collections.defaultdict(list)
        stems = collections.defaultdict(set)
        for (ref, f) in self.by_file:
            stems[f.split('.')[0].lower()].add((ref, f))
        for u in self.active_units():
            st = pathlib.PurePosixPath(u).name.lower()
            for k in sorted(stems.get(st, ())):
                files[u][k] += 3
                why[u].append('stem:%s' % k[1])
            for o in self.units[u]:
                if o.name in self.fixed:
                    r = self.fixed[o.name][0]
                    files[u][(r.ref, r.file)] += 2 if self.fixed[o.name][1] == 'anchor' else 1
                    why[u].append('anchor:%s' % r.file)
                for src, nm, st in self.hints.get(o.addr, ()):
                    if src == 'p07' and st in ('high', 'med') and o.unnamed:
                        for r in self.by_cname.get(nm, ()):
                            if r.ref == 'tw07':
                                files[u][(r.ref, r.file)] += 1
                                why[u].append('p07:%s' % r.file)
                for t in o.strs:
                    for m in re.finditer(r'([\w]+)\.(c|cpp)\b', t, re.I):
                        for k in sorted(stems.get(m.group(1).lower(), ())):
                            files[u][k] += 3
                            why[u].append('str:%s' % k[1])
        # neighbour vote for units with nothing of their own
        for u in self.active_units():
            if files[u]:
                continue
            nv = collections.Counter()
            for o in self.units[u]:
                for x in list(o.calls) + sorted(o.callers):
                    if x in self.fixed:
                        r = self.fixed[x][0]
                        nv[(r.ref, r.file)] += 0.5
            for k, v in nv.most_common(3):
                if v >= 1.5:
                    files[u][k] = v
                    why[u].append('nbr:%s' % k[1])
        self.unit_files = {u: {k: v for k, v in files[u].items() if v >= 2 or 'nbr:%s' % k[1] in why[u]}
                           for u in self.active_units()}
        self.unit_why = why

    def run(self, rounds=6):
        self.anchors()
        accepted = {}
        for rnd in range(rounds):
            self.map_units()
            self.scorer.pair_of = {n: v[0] for n, v in self.fixed.items()}
            props = self.one_round()
            new = 0
            for n, p in sorted(props.items(), key=lambda kv: -kv[1]['score']):
                if p['conf'] in ('A', 'B') and n not in self.fixed:
                    self.fixed[n] = (p['ref'], 'round%d' % (rnd + 1))
                    p['round'] = rnd + 1
                    accepted[n] = p
                    new += 1
            last = props
            if not new:
                break
        # accepted pairs (A/B, each fixed in the round it was found) plus the last round's
        # provisional ones
        self.props = dict(accepted)
        for n, p in last.items():
            if n not in self.props and p['conf'] == 'C':
                self.props[n] = p
        return self.props

    def file_pairs(self, u, fk, taken, edge, neighbour_only, extra_code=None):
        """Proposals for unit u against reference file fk, gap by gap between the anchors."""
        seq, rseq = self.units[u], self.by_file[fk]
        pos = {r.key: j for j, r in enumerate(rseq)}
        fixed = [(i, pos[self.fixed[o.name][0].key]) for i, o in enumerate(seq)
                 if o.name in self.fixed and self.fixed[o.name][0].key in pos]
        chain = monotone_chain(fixed)
        bounds = [(-1, -1, edge)] + [(i, j, True) for i, j in chain] + [(len(seq), len(rseq), edge)]
        out = []
        for (i0, j0, b0), (i1, j1, b1) in zip(bounds, bounds[1:]):
            ours = [i for i in range(i0 + 1, i1) if seq[i].name not in self.fixed]
            theirs = [j for j in range(j0 + 1, j1) if rseq[j].key not in taken]
            if not ours or not theirs:
                continue
            sc = {}
            for a, i in enumerate(ours):
                for b, j in enumerate(theirs):
                    s, codes = self.scorer.score(seq[i], rseq[j])
                    if s > 0:
                        sc[(a, b)] = (s, codes)
            weights = {k: v[0] for k, v in sc.items()}
            chosen = align(weights, len(ours), len(theirs))
            total = sum(weights[k] for k in chosen)
            with_code = [j for j in theirs if rseq[j].size is not None]
            for a, b in chosen:
                o, r = seq[ours[a]], rseq[theirs[b]]
                s, codes = sc[(a, b)]
                # margin: how much the best alignment loses when this pair is forbidden
                # (feature score and position together)
                w2 = dict(weights)
                del w2[(a, b)]
                margin = total - sum(w2[k] for k in align(w2, len(ours), len(theirs)))
                both = b0 and b1
                one = b0 or b1
                gap11 = len(ours) == 1 and len(with_code) == 1 and r.size is not None
                sig_exact = 'sig' in codes
                sig_ok = sig_exact or 'sigw' in codes
                sig_bad = 'sig!' in codes
                size_bad = 'size!' in codes
                indep = [c for c in codes if c.startswith(INDEPENDENT)]
                if 'E1' in codes or (both and sig_exact and not size_bad and margin >= 3
                                     and (gap11 or indep or margin >= 5)):
                    conf = 'A'
                elif (both and not sig_bad and not size_bad and margin >= 1.5) \
                        or (one and sig_ok and margin >= 2.5 and not size_bad):
                    conf = 'B'
                else:
                    conf = 'C'
                if neighbour_only and conf != 'C' and not both and 'E1' not in codes:
                    conf = 'C'
                # TW07 inlined every call of it: it may still have code here, but the
                # size line is missing; never A without EA's text
                if r.size is None and conf == 'A' and 'E1' not in codes:
                    conf = 'B'
                pos_code = 'pos=' + ('1:1' if gap11 else ('both' if both else ('one' if one else 'none')))
                out.append(dict(our=o, ref=r, score=round(s, 2), margin=round(margin, 2), conf=conf,
                                codes=codes + [pos_code] + ([extra_code] if extra_code else []),
                                gap=(len(ours), len(theirs)), unit=u, round=None, ij=(ours[a], theirs[b]),
                                both=both))
        # runs: consecutive functions on both sides (ours i, i+1, ... with theirs j, j+1, ...,
        # anchors included). Inside a run of 4+ with agreeing signatures the order itself is the
        # evidence, even where identical signatures leave the score margin small
        sigok = {p['ij'] for p in out if ('sig' in p['codes'] or 'sigw' in p['codes'])
                 and 'size!' not in p['codes']} | {(i, j) for i, j in chain}
        for p in out:
            i, j = p['ij']
            lo = 0
            while (i - lo - 1, j - lo - 1) in sigok:
                lo += 1
            hi = 0
            while (i + hi + 1, j + hi + 1) in sigok:
                hi += 1
            run = lo + hi + 1 if p['ij'] in sigok else 1
            p['run'] = run
            # B needs a second line besides the score: a run, a small gap, an independent line
            # or a wide margin (the hand check found B's errors in wide gaps with none of these)
            indep = [c for c in p['codes'] if c.startswith(INDEPENDENT)]
            if p['conf'] == 'B' and run < 4 and not indep and p['margin'] < 3 and p['gap'][0] > 3:
                p['conf'] = 'C'
                p['codes'].append('weak')
            # an inline-only reference function (no code in TW07) is B only inside a run (the hand
            # check found fn_80012E00, 1/(f08-f04), paired with the inline-only FO_vFreeFont)
            if p['conf'] == 'B' and p['ref'].size is None and run < 4:
                p['conf'] = 'C'
                p['codes'].append('weak')
            if run >= 4:
                p['codes'].append('run%d' % run)
                if p['conf'] == 'C' and p['both'] and 'sig!' not in p['codes'] and 'size!' not in p['codes']:
                    p['conf'] = 'B'
                if p['conf'] == 'B' and p['both'] and 'sig' in p['codes'] and run >= 6 \
                        and p['ref'].size is not None and 'size?' not in p['codes'] and 'size!' not in p['codes']:
                    p['conf'] = 'A'
        for p in out:
            # era drift: a name that names a system TW2004 does not have (TW07's Apt Flash UI,
            # the PS3) can be the right position but not a name for this game's code
            if ERA.search(p['ref'].name) and p['conf'] in ('A', 'B'):
                p['conf'] = 'C'
                p['codes'].append('era')
        return out, bool(fixed)

    def one_round(self):
        taken = {v[0].key: n for n, v in self.fixed.items()}
        props = {}
        for u in self.active_units():
            why = self.unit_why.get(u, [])
            neighbour_only = bool(why) and all(w.startswith('nbr:') for w in why)
            for fk in self.unit_files.get(u, {}):
                # the unit's own ends count as brackets when the unit IS this file: a string in it
                # names the file, or its file name matches and an anchor of that file confirms it
                # (a file name alone is not enough: our unit names are partly our own, e.g.
                # startUp is an audio driver)
                named = 'str:' + fk[1] in why or 'stem:' + fk[1] in why
                out, has_anchor = self.file_pairs(u, fk, taken, 'str:' + fk[1] in why, neighbour_only)
                if named and not has_anchor and 'str:' + fk[1] not in why:
                    # no anchor, file name only: accept the ends as brackets when the whole unit
                    # aligns coherently with the file (most pairs with an agreeing signature and
                    # size, in order): 'coh'
                    good = [p for p in out if ('sig' in p['codes'] or 'sigw' in p['codes'])
                            and 'size!' not in p['codes']]
                    n = len(self.units[u])
                    if len(out) >= 5 and len(good) >= 0.7 * len(out) and len(out) >= 0.4 * n:
                        out, _ = self.file_pairs(u, fk, taken, True, neighbour_only, 'coh')
                elif named and has_anchor:
                    out, _ = self.file_pairs(u, fk, taken, True, neighbour_only)
                for p in out:
                    old = props.get(p['our'].name)
                    if old is None or (old['conf'], -old['score']) > (p['conf'], -p['score']):
                        props[p['our'].name] = p
        # a reference function claimed by two of ours: keep the stronger, drop the other to C
        claim = collections.defaultdict(list)
        for n, p in props.items():
            claim[p['ref'].key].append(p)
        for k, ps in claim.items():
            if len(ps) > 1:
                ps.sort(key=lambda p: (p['conf'], -p['score']))
                for p in ps[1:]:
                    p['conf'] = 'C'
                    p['codes'] = p['codes'] + ['dup']
        return props


def evidence_codes(p):
    r = p['ref']
    base = ['E2b' if r.ref == 'tw07' else 'E2']
    return base + p['codes']


def load_refs():
    return load_tw07() + load_tw06p()


def build(include_skipped=False, hide=None):
    units, allf = load_ours()
    if hide:
        for n in hide:
            o = allf[n]
            o.hidden = o.name
            o.name = 'fn_hidden_' + o.name
        allf = {o.name: o for fs in units.values() for o in fs}
    return Pairing(units, allf, load_refs(), load_hints(), include_skipped)


def summary(P, props):
    act = P.active_units()
    unnamed = [o for u in act for o in P.units[u] if o.unnamed]
    skipped = [(u, sum(1 for o in P.units[u] if o.unnamed)) for u in P.units if SKIP.match(u)]
    lv = collections.Counter(p['conf'] for n, p in props.items() if P.allf[n].unnamed)
    print('reference size ratios (median log ref/ours): %s' % ', '.join('%s x%.2f' % (k, math.exp(v)) for k, v in P.ratio.items()))
    print('anchors (named functions carrying a reference name): %d' % sum(1 for v in P.fixed.values() if v[1] == 'anchor'))
    print('units scored: %d, with a reference file: %d' % (len(act), sum(1 for u in act if P.unit_files.get(u))))
    print('fn_ functions in scored units: %d (%d in units with a reference file of their own, %d via '
          'neighbours only, %d in units with none)' % (
              len(unnamed),
              sum(1 for o in unnamed if P.unit_files.get(o.unit) and not all(w.startswith('nbr:') for w in P.unit_why.get(o.unit, []))),
              sum(1 for o in unnamed if P.unit_files.get(o.unit) and all(w.startswith('nbr:') for w in P.unit_why.get(o.unit, []))),
              sum(1 for o in unnamed if not P.unit_files.get(o.unit))))
    for k in 'ABC':
        print('  %s: %d' % (k, lv.get(k, 0)))
    print('  none: %d' % (len(unnamed) - sum(lv.values())))
    print('skipped units (lane n2): %s' % ', '.join('%s %d' % s for s in skipped))


def per_unit(P, props):
    rows = []
    for u in P.active_units():
        fs = [o for o in P.units[u] if o.unnamed]
        if not fs:
            continue
        c = collections.Counter(props[o.name]['conf'] for o in fs if o.name in props)
        rows.append((u, len(fs), c.get('A', 0), c.get('B', 0), c.get('C', 0),
                     ' '.join(sorted(f for _, f in P.unit_files.get(u, {})))))
    return rows


def write_tsv(P, props, path, only_proposals=False):
    """One row per fn_ function of the scored units (--only-proposals: rows with a candidate).
    confidence A / B / C as in the module docstring; tier = what the audit would start from
    (A: T1 candidate, B: T2 candidate, C: provisional); never a rename by itself."""
    with open(path, 'w', encoding='utf-8', newline='\n') as f:
        f.write('address\tunit\tcurrent_name\tproposed\tsource\tref_file\tref_line\tref_signature\t'
                'our_signature\tcodes\tconfidence\tscore\tmargin\tgap\tround\n')
        for u in P.active_units():
            for o in P.units[u]:
                if not o.unnamed:
                    continue
                p = props.get(o.name)
                oursig = '%s(%s)' % (o.ret or '?', ', '.join(o.params) if o.params is not None else '?')
                if p is None:
                    if only_proposals:
                        continue
                    f.write('%08X\t%s\t%s\t-\t-\t-\t-\t-\t%s\t-\tnone\t\t\t\t\n' % (o.addr or 0, u, o.name, oursig))
                    continue
                r = p['ref']
                f.write('%08X\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%.2f\t%.2f\t%d:%d\t%s\n' % (
                    o.addr or 0, u, o.name, r.cname, 'TW07 PS3' if r.ref == 'tw07' else 'TW06 PS2 EA_DASH',
                    r.file, r.line if r.ref == 'tw07' else '%X' % r.line, r.sig(), oursig,
                    ','.join(evidence_codes(p)), p['conf'], p['score'], p['margin'], p['gap'][0], p['gap'][1],
                    p['round'] or ''))


def holdout(frac, seed, rounds):
    P0 = build()
    P0.anchors()
    names = sorted(n for n, v in P0.fixed.items())
    random.Random(seed).shuffle(names)
    hide = set(names[:int(len(names) * frac)])
    P = build(hide=hide)
    props = P.run(rounds)
    res = collections.defaultdict(lambda: [0, 0])
    wrong = []
    for n, p in props.items():
        o = P.allf[n]
        if o.hidden:
            ok = p['ref'].cname == o.hidden
            res[p['conf']][0 if ok else 1] += 1
            if not ok and p['conf'] in 'AB':
                wrong.append((p['conf'], o.hidden, p['ref'].cname, ','.join(p['codes'])))
    found = sum(v[0] + v[1] for v in res.values())
    print('hidden %d of %d anchors; %d got a proposal' % (len(hide), len(names), found))
    for k in 'ABC':
        ok, bad = res[k]
        if ok + bad:
            print('  %s: %d right, %d wrong (precision %.1f%%)' % (k, ok, bad, 100.0 * ok / (ok + bad)))
    for w in wrong[:40]:
        print('  wrong %s: hidden %s -> proposed %s (%s)' % w)


def explain(P, props, who):
    o = P.allf.get(who) or next((x for x in P.allf.values() if x.addr and '%08X' % x.addr == who.upper().replace('0X', '').replace('FN_', '')), None)
    if o is None:
        sys.exit('unknown function %s' % who)
    print('%s %08X unit %s size 0x%X sig %s(%s)' % (o.name, o.addr or 0, o.unit, o.size, o.ret, o.params))
    print('  calls: %s' % ', '.join(o.calls))
    print('  strings: %s' % o.strs)
    print('  unit files: %s (%s)' % (P.unit_files.get(o.unit), ' '.join(P.unit_why.get(o.unit, []))))
    rows = []
    for fk in P.unit_files.get(o.unit, {}):
        for r in P.by_file[fk]:
            s, c = P.scorer.score(o, r)
            rows.append((s, r, c))
    for s, r, c in sorted(rows, key=lambda x: -x[0])[:15]:
        print('  %6.2f  %-40s %s:%s  %s' % (s, r.cname, r.file, r.line, ','.join(c)))
    if o.name in props:
        p = props[o.name]
        print('  PROPOSAL %s conf %s margin %.2f codes %s' % (p['ref'].cname, p['conf'], p['margin'], p['codes']))


# --------------------------------------------------------------------------------- struct fields
PLACEHOLDER = re.compile(r'^(n|f|p|b|u|a|v|s|c|w|h|d|m|pfn|sz|x|y|z|i|l|q|e)([0-9A-F]{1,5})$')


def load_ref_structs():
    """name -> (size, [(offset, type, name)]) from TW06 Xbox (resym) and TW06 PS2 (stdump)."""
    out = {}
    for p in sorted((REF / 'tw06-xbox/key-types').glob('*.hpp')):
        cur = None
        for l in p.read_text(encoding='utf-8', errors='replace').splitlines():
            m = re.match(r'^(?:struct|class|union) (\w+) \{ /\* Size=0x([0-9a-f]+) \*/', l)
            if m:
                cur = (m.group(1), int(m.group(2), 16), [])
                continue
            if cur and l.startswith('};'):
                out.setdefault(cur[0], ('tw06x', cur[1], cur[2]))
                cur = None
                continue
            m = re.match(r'^\s+/\* 0x([0-9a-f]+) \*/ (?:public: |private: |protected: )?(.*?)\s*(\w+)((?:\[\d+\])*)\s*(?::\s*\d+)?;', l)
            if cur and m:
                cur[2].append((int(m.group(1), 16), m.group(2) + m.group(4), m.group(3)))
    cur = None
    for l in (REF / 'tw06-ps2/types.hpp').read_text(encoding='utf-8', errors='replace').splitlines():
        m = re.match(r'^(?:typedef )?(?:struct|union|class) (\w+)? ?\{ // 0x([0-9a-f]+)', l)
        if m:
            cur = [m.group(1), int(m.group(2), 16), []]
            continue
        if cur and l.startswith('}'):
            n = cur[0] or (re.match(r'^\}\s*(\w+)', l) or [None, None])[1]
            if n:
                out.setdefault(n, ('tw06p', cur[1], cur[2]))
            cur = None
            continue
        m = re.match(r'^\t/\* 0x([0-9a-f]+) \*/ (.*?)\s*\**(\w+)((?:\[\d+\])*)\s*(?::\s*\d+)?;', l)
        if cur and m:
            cur[2].append((int(m.group(1), 16), m.group(2) + m.group(4), m.group(3)))
    return out


def load_our_structs():
    """[(header, struct name, size or None, preceding comment, [(offset, type, name, comment)])]"""
    sizes = {}
    for p in (ROOT / 'include').rglob('*.h'):
        for m in re.finditer(r'LAYOUT_ASSERT\((\w+),\s*(0x[0-9A-Fa-f]+|\d+)\)', p.read_text(errors='replace')):
            sizes[m.group(1)] = int(m.group(2), 0)
    out = []
    for p in sorted((ROOT / 'include').rglob('*.h')):
        lines = p.read_text(encoding='utf-8', errors='replace').splitlines()
        k = 0
        while k < len(lines):
            m = re.match(r'^(typedef\s+)?struct\s+(\w+)?\s*\{', lines[k])
            if not m:
                k += 1
                continue
            j = k - 1
            pre = []
            while j >= 0 and lines[j].lstrip().startswith('//'):
                pre.insert(0, lines[j].strip())
                j -= 1
            fields, depth, e = [], 1, k + 1
            while e < len(lines) and depth > 0:
                l = lines[e]
                depth += l.count('{') - l.count('}')
                if depth == 1 and l.lstrip().startswith('//'):
                    pre.append(l.strip())          # a comment line inside the body
                if depth == 1:
                    fm = re.match(r'^\s+([A-Za-z_][\w\s\*]*?[\s\*])(\w+)\s*((?:\[[^\]]*\])*)\s*(?::\s*\d+)?\s*;\s*(//\s*(.*))?$', l)
                    if fm:
                        com = fm.group(5) or ''
                        om = re.match(r'^0x([0-9A-Fa-f]+)', com)
                        ph = PLACEHOLDER.match(fm.group(2)) or re.match(r'^unk([0-9A-F]+)$', fm.group(2))
                        off = int(om.group(1), 16) if om else (int(ph.group(ph.lastindex), 16) if ph else None)
                        fields.append((off, fm.group(1).strip() + fm.group(3), fm.group(2), com))
                e += 1
            name = (re.match(r'^\}\s*(\w+)', lines[e - 1]) or [None, None])[1] or m.group(2)
            out.append((p.relative_to(ROOT).as_posix(), name, sizes.get(name), ' '.join(pre), fields))
            k = e
    return out


def pair_fields():
    refs = load_ref_structs()
    rows, stats = [], collections.Counter()
    ph_total = 0
    for hdr, name, size, pre, fields in load_our_structs():
        phs = [f for f in fields if f[0] is not None and PLACEHOLDER.match(f[2])]
        ph_total += len(phs)
        if not phs:
            continue
        # which reference struct: named in the struct's comment after "TW06", or the same name
        how, ref = None, None
        for m in re.finditer(r"TW06(?::|'s)?\s+(\w+)", pre):
            if m.group(1) in refs:
                how, ref = 'explicit', m.group(1)
                break
        if ref is None:
            for tok in re.findall(r'\b([A-Za-z_]\w{7,})\b', pre):
                rs = refs.get(tok)
                if rs and tok != name and ('_' in tok or re.search(r'[a-z][A-Z]', tok)) \
                        and (size is None or 0.5 <= rs[1] / max(size, 1) <= 2.0):
                    how, ref = 'explicit', tok
                    break
        if ref is None and name in refs:
            how, ref = 'name', name
        if ref is None:
            continue
        stats['structs paired'] += 1
        stats['placeholders in paired structs'] += len(phs)
        src, rsize, rfields = refs[ref]
        byname = {n: (o, t) for o, t, n in rfields}
        anchors = []
        for off, t, n, com in fields:
            if off is None:
                continue
            m = re.search(r'TW06:\s*(\w+)', com)
            rn = m.group(1) if m and m.group(1) in byname else (n if n in byname else None)
            if rn:
                anchors.append((off, byname[rn][0], rn))
        anchors.sort()
        used = {a[2] for a in anchors}
        for off, t, n, com in phs:
            prev = [a for a in anchors if a[0] < off]
            nxt = [a for a in anchors if a[0] > off]
            d1 = prev[-1][1] - prev[-1][0] if prev else None
            d2 = nxt[0][1] - nxt[0][0] if nxt else None
            if d1 is not None and d1 == d2:
                d, pos = d1, 'both'
            elif d1 is not None or d2 is not None:
                d, pos = (d1 if d1 is not None else d2), 'one'
            elif size is not None and size == rsize:
                d, pos = 0, 'size'
            else:
                stats['no-anchor'] += 1
                continue
            cand = [(o, rt, rn) for o, rt, rn in rfields if o == off + d and rn not in used]
            if not cand:
                stats['no-field'] += 1
                continue
            o, rt, rn = cand[0]
            ok = compatible(kind(t), kind(rt)) and (('[' in t) == ('[' in rt) or kind(t) == kind(rt))
            if not ok:
                stats['kind-mismatch'] += 1
                continue
            conf = 'A' if pos == 'both' and how == 'explicit' else ('B' if pos in ('both', 'size') else 'C')
            rows.append((hdr, name, '0x%X' % off, n, rn, '%s %s' % (src, ref), '%s+0x%X' % (ref, o),
                         'E2,struct=%s,pos=%s,d=%+d' % (how, pos, d), conf))
    return rows, ph_total, stats


def globals_survey(P, props):
    """lbl_ variables (.data/.sdata/.bss/.sbss, strings left out): how many the reference
    inventories name (the TW06 PS2 per-file globals of EA's shared packages), and how many are used
    by a function that has a reference pair (their names would come with a TW07 globals export)."""
    syms = {}
    for l in (ROOT / 'config/GW4E69/symbols.txt').read_text(encoding='utf-8').splitlines():
        m = re.match(r'^(lbl_[0-9A-F]{8}) = (\.\w+):0x([0-9A-F]+); // (.*)$', l)
        if m and m.group(2) in ('.data', '.sdata', '.bss', '.sbss') and 'data:string' not in m.group(4):
            syms[m.group(1)] = m.group(2)
    users = collections.defaultdict(set)
    for n, o in P.allf.items():
        for d in o.data:
            if d in syms:
                users[d].add(n)
    paired = {n for n, v in P.fixed.items()} | {n for n, p in props.items() if p['conf'] in 'AB'}
    anchor = {n for n, v in P.fixed.items() if v[1] == 'anchor'}
    files = collections.Counter()
    by_paired = by_anchor = 0
    for g, us in users.items():
        if us & paired:
            by_paired += 1
            f = collections.Counter(P.fixed[u][0].file if u in P.fixed else props[u]['ref'].file for u in us & paired)
            files[f.most_common(1)[0][0]] += 1
        if us & anchor:
            by_anchor += 1
    # TW06 PS2 globals of the shared-package files our units share
    t6 = collections.defaultdict(list)
    cur = None
    for l in (REF / 'tw06-ps2/globals.cpp').read_text(encoding='utf-8', errors='replace').splitlines():
        m = re.match(r'// FILE -- (.*)', l)
        if m:
            cur = re.split(r'[\\/]', m.group(1).strip())[-1]
            continue
        m = re.match(r'^/\* (\w+) [0-9a-f]+ \*/ (?:static )?(.*?)\s*\**(\w+)(\[\d*\])*;', l)
        if m and cur:
            t6[cur].append(m.group(3))
    ours = set(re.findall(r'^(\w+) = ', (ROOT / 'config/GW4E69/symbols.txt').read_text(encoding='utf-8'), re.M))
    shared = {f: [(n, n in ours) for n in t6[f]] for f in ('SharedFileIO.c', 'TagFile.c', 'ChecksumCRC32.c',
                                                            'llSharedFileIO.c', 'TibExt.cpp')}
    return dict(total=len(syms), used=len(users), by_paired=by_paired, by_anchor=by_anchor,
                files=files, shared=shared)


def main():
    a = sys.argv[1:]
    opt = lambda k, d=None: a[a.index(k) + 1] if k in a else d
    rounds = int(opt('--rounds', 6))
    if '--holdout' in a:
        return holdout(float(opt('--holdout')), int(opt('--seed', 1)), rounds)
    if '--fields' in a:
        rows, total, stats = pair_fields()
        c = collections.Counter(r[-1] for r in rows)
        print('placeholder fields (prefix + hex offset, unk* padding excluded): %d' % total)
        print('proposals: A %d, B %d, C %d; skipped: %s' % (c['A'], c['B'], c['C'], dict(stats)))
        for r in rows:
            print('\t'.join(r))
        return
    P = build(include_skipped='--all' in a)
    props = P.run(rounds)
    if '--explain' in a:
        return explain(P, props, opt('--explain'))
    if '--units' in a:
        for u in P.active_units():
            print('%-40s %s  [%s]' % (u, ', '.join('%s:%s=%g' % (k[0], k[1], v) for k, v in P.unit_files.get(u, {}).items()),
                                      ' '.join(sorted(set(P.unit_why.get(u, []))))))
        return
    summary(P, props)
    if '--globals' in a:
        g = globals_survey(P, props)
        print('lbl_ variables (.data/.sdata/.bss/.sbss, no strings): %d, used by game code: %d' % (g['total'], g['used']))
        print('  used by a function with a reference name (anchor or A/B pair): %d (by an anchor: %d)' % (
            g['by_paired'], g['by_anchor']))
        print('  their TW07/TW06 files: %s' % ', '.join('%s %d' % kv for kv in g['files'].most_common(15)))
        for f, lst in g['shared'].items():
            print('  TW06 PS2 %s: %s' % (f, ', '.join('%s%s' % (n, '' if ok else ' (not in symbols.txt)') for n, ok in lst) or '-'))
    if '--per-unit' in a:
        for r in per_unit(P, props):
            print('  %-40s fn_ %3d  A %3d  B %3d  C %3d  %s' % r)
    if opt('--tsv'):
        write_tsv(P, props, opt('--tsv'), '--only-proposals' in a)


if __name__ == '__main__':
    main()
