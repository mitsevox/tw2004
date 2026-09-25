"""Sweep combinations of the known matching levers over near-miss functions. CPU only; src/ is never
written, every result is a candidate for a lane to apply and check.

    python tools/match/leversweep.py <Unit> <fn> [--minutes 6] [-j N]
    python tools/match/leversweep.py --from-report [--min 95] [--skip Unit,Unit] [--minutes 6] [-j N]

--from-report takes every non-exact function of report.json at or above --min percent, closest first
(rebuild the report first: `ninja build/GW4E69/report.json`). Each function gets its permuter
directory (perm_setup.py, from the current src/) and --minutes of sweeping on -j worker processes
(default: all cores). An overnight run on 20 cores: `--from-report --min 95 -j 20` (80 functions,
about 8 hours on 2026-09-25). The time per function is sized for 20 cores: on 4 cores, 6 minutes
does not reach every pair of a function with ~170 levers (leversweep_selftest.py: GoDynObj).

The levers (docs/decomp-notes.md, "New from the first cloud lanes"), each tried at every site:
  safe    move a declaration; int/s32/long or u32/unsigned int respelled (the same type on the
          GameCube); an identity inline on an assignment's value (`p = __lv(&x[i])`: moved a pointer
          to EA's register in three functions); `x = a * b * c` split into `x = a; x *= b; ...`
          (also +); a `for` loop written as `init; while (cond) { ...; step; }`.
  review  a local's signedness changed (int <-> u32); a (u32) cast on an array index (both change
          the meaning for negative values: a person checks the value range before using one); a
          do/while written as a while (changes it when the loop can run zero times); moving a
          declaration whose initialiser calls a function (reorders the calls).
An identity inline is a fake match by nature: EA had a real helper or macro there, or none. A hit
that uses one says so in its report; look for EA's helper in TW07 first.
First every lever alone, then pairs (the fixes that need two changes at once: GameMode26, GoDynObj),
then triples built on the best pairs, until the time limit. A function stops at its first exact
variant. Scores are quicktrial's (differing instructions, lined up); a 0 is only called exact when
the positional compare (branch targets included) is 0 as well.

Output in build/leversweep/: <fn>.txt (base score, the best variants with their levers and the
changed lines) and summary.tsv (one row per function). Applying a hit: make the same change in
src/<Unit>.c, confirm it in the real build (the snapshot masks symbol names), check the meaning,
and label it: an identity inline becomes `static inline T fn_<caller address>_Read(T v)` with a
`// fake match:` comment (agents/brief.md)."""
import difflib, json, multiprocessing, os, pathlib, re, subprocess, sys, time

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(HERE))
import quicktrial as qt                                  # noqa: E402

OUT = ROOT / 'build' / 'leversweep'
KEYWORDS = {'return', 'goto', 'break', 'continue', 'case', 'else', 'do', 'if', 'while', 'for',
            'switch', 'default', 'sizeof'}
SIGNED = {'int', 's32', 'long', 'signed int', 'signed long'}
UNSIGNED = {'u32', 'unsigned int', 'unsigned long', 'unsigned'}
INDEXABLE = SIGNED | {'s16', 'short', 's8', 'char', 'signed char'}
DECL = re.compile(r'^(\s*)((?:(?:const|volatile|register|unsigned|signed|struct|union|enum)\s+)*'
                  r'[A-Za-z_]\w*(?:\s+(?:int|long|short|char))?)(\s+\**\s*|\s*\*+\s*)([A-Za-z_]\w*)'
                  r'((?:\[[^\]]*\])*)(\s*=\s*[^;]*)?;\s*$')
MULTI = re.compile(r'^\s*[A-Za-z_][\w ]*[\s\*]+[A-Za-z_]\w*(?:\[[^\]]*\])*\s*,[^;]*;\s*$')


# ---- the function inside base.c ------------------------------------------------------------------

def locate(src, fn):
    """(header start, body start, body end) of fn's definition in src: body_start is just after
    the opening brace, body_end the index of the closing one."""
    m = re.search(r'^[A-Za-z_][^\n;]*\b' + re.escape(fn) + r'\s*\([^;{]*\)\s*\{', src, re.M)
    if not m:
        return None
    i, depth, q = m.end(), 1, None
    while i < len(src) and depth:
        c = src[i]
        if q:
            if c == '\\':
                i += 1
            elif c == q:
                q = None
        elif c in '"\'':
            q = c
        elif c == '{':
            depth += 1
        elif c == '}':
            depth -= 1
        i += 1
    return m.start(), m.end(), i - 1


class Func:
    """fn's body as a declaration block (a list of lines) and the rest, plus helpers that go in
    front of the function. Levers edit a copy of this; render() puts it back into base.c."""

    def __init__(self, src, fn):
        self.src, self.fn = src, fn
        self.head, self.bstart, self.bend = locate(src, fn)
        lines = src[self.bstart:self.bend].split('\n')
        k = 0
        while k < len(lines) and (not lines[k].strip() or self._is_decl(lines[k])):
            k += 1
        while k and not lines[k - 1].strip():     # a blank line after the block stays in the rest
            k -= 1
        self.decls = [l for l in lines[:k]]
        self.rest = '\n'.join(lines[k:])
        self.helpers = []

    @staticmethod
    def _is_decl(line):
        m = DECL.match(line)
        if m:
            return m.group(2).split()[0] not in KEYWORDS and m.group(4) not in KEYWORDS
        return bool(MULTI.match(line)) and line.split()[0] not in KEYWORDS

    def copy(self):
        f = Func.__new__(Func)
        f.src, f.fn, f.head, f.bstart, f.bend = self.src, self.fn, self.head, self.bstart, self.bend
        f.decls, f.rest, f.helpers = self.decls[:], self.rest, self.helpers[:]
        return f

    def render(self):
        body = '\n'.join(self.decls) + ('\n' if self.decls else '') + self.rest
        return (self.src[:self.head] + ''.join(h + '\n' for h in self.helpers)
                + self.src[self.head:self.bstart] + body + self.src[self.bend:])

    def locals(self):
        """name -> (index in decls, type, stars, array suffix, initialiser) for single declarators."""
        out = {}
        for i, l in enumerate(self.decls):
            m = DECL.match(l)
            if m and not self._multi(l):
                out[m.group(4)] = (i, ' '.join(m.group(2).split()), m.group(3).strip(), m.group(5),
                                   m.group(6))
        return out

    @staticmethod
    def _multi(l):
        return ',' in l.split('=')[0]


# ---- the levers -----------------------------------------------------------------------------------
# A lever is (key, label, kind, apply); apply(Func) edits it in place and returns False when it no
# longer applies (another lever changed its site). Two levers with the same key never combine.

def _nth(pattern, text, n):
    ms = list(re.finditer(pattern, text, re.M))
    return ms[n] if n < len(ms) else None


def levers(f):
    out = []
    loc = f.locals()
    names = [l for l in f.decls if l.strip()]
    order = [n for n, v in sorted(loc.items(), key=lambda kv: kv[1][0])]

    # declaration moves (every declaration line to every other position)
    for i, line in enumerate(f.decls):
        if not line.strip():
            continue
        for j in range(len(f.decls)):
            if j == i or (j == i + 1):
                continue
            def mv(g, line=line, j=j):
                if line not in g.decls:
                    return False
                i2 = g.decls.index(line)
                g.decls.insert(j if j < i2 else j - 1, g.decls.pop(i2))
                return True
            # an initialiser that calls something runs in declaration order: moving it is review
            kind = 'review' if '=' in line and '(' in line.split('=', 1)[1] else 'safe'
            out.append((('move', line.strip()), 'move `%s` to line %d of the declarations'
                        % (line.strip(), j + 1), kind, mv))

    for name, (i, typ, stars, arr, init) in loc.items():
        # type respelled within one family (safe) or across signedness (review)
        if not stars and not arr:
            fam = SIGNED if typ in SIGNED else UNSIGNED if typ in UNSIGNED else None
            if fam:
                alts = [(t, 'safe') for t in ('int', 's32') if t != typ and fam is SIGNED] + \
                       [(t, 'safe') for t in ('u32', 'unsigned int') if t != typ and fam is UNSIGNED] + \
                       [(t, 'review') for t in (('u32',) if fam is SIGNED else ('int', 's32'))]
                for t, kind in alts:
                    def ty(g, name=name, t=t):
                        for k, l in enumerate(g.decls):
                            m = DECL.match(l)
                            if m and m.group(4) == name and not g._multi(l):
                                g.decls[k] = l[:m.start(2)] + t + l[m.end(2):]
                                return True
                        return False
                    out.append((('type', name), 'type of %s: %s -> %s' % (name, typ, t), kind, ty))

        # identity inline on each assignment to a plain local
        if not arr:
            T = typ + (' ' + stars if stars else '')
            pat = r'^(\s*)' + re.escape(name) + r'\s*=\s*([^=;][^;]*);'
            for n, m in enumerate(re.finditer(pat, f.rest, re.M)):
                helper = 'static inline %s __lv_%s_%d(%s v) { return v; }' % (T, name, n, T)
                def ident(g, name=name, n=n, pat=pat, helper=helper):
                    mm = _nth(pat, g.rest, n)
                    if not mm:
                        return False
                    g.rest = (g.rest[:mm.start()] + '%s%s = __lv_%s_%d(%s);'
                              % (mm.group(1), name, name, n, mm.group(2).strip()) + g.rest[mm.end():])
                    g.helpers.append(helper)
                    return True
                stmt = m.group(0).strip()
                out.append((('id', name, n), 'identity inline on `%s`' % (stmt[:60]), 'safe', ident))
                # `x = p[i]` through an accessor inline (p, i) -> p[i]: fixed ShaderObjectsData
                acc = re.match(r'^([A-Za-z_][\w.>-]*)\[([^\[\]]+)\]$', m.group(2).strip())
                if acc and not stars:
                    helper2 = 'static inline %s __lv_%s_%d_rd(%s* p, int i) { return p[i]; }' % (
                        T, name, n, T)
                    def accessor(g, name=name, n=n, pat=pat, helper2=helper2):
                        mm = _nth(pat, g.rest, n)
                        a = mm and re.match(r'^([A-Za-z_][\w.>-]*)\[([^\[\]]+)\]$', mm.group(2).strip())
                        if not a:
                            return False
                        g.rest = (g.rest[:mm.start()] + '%s%s = __lv_%s_%d_rd(%s, %s);'
                                  % (mm.group(1), name, name, n, a.group(1), a.group(2))
                                  + g.rest[mm.end():])
                        g.helpers.append(helper2)
                        return True
                    out.append((('id', name, n), 'accessor inline on `%s`' % (stmt[:60]), 'safe',
                                accessor))

        # `x = a * b * c` -> `x = a; x *= b; x *= c;` (and +)
        pat = r'^(\s*)' + re.escape(name) + r' = ([^;]+);'
        for n, m in enumerate(re.finditer(pat, f.rest, re.M)):
            rhs = m.group(2)
            for op in ('*', '+'):
                parts = _top_split(rhs, ' %s ' % op)
                if len(parts) < 2 or re.search(r'\b%s\b' % re.escape(name), rhs):
                    continue
                if any(o in p for p in parts for o in (' + ', ' - ', ' * ', ' / ') if o != ' %s ' % op
                       and _top_split(p, o) != [p]):
                    continue
                def split(g, name=name, n=n, pat=pat, op=op):
                    mm = _nth(pat, g.rest, n)
                    if not mm:
                        return False
                    ps = _top_split(mm.group(2), ' %s ' % op)
                    ind = mm.group(1)
                    txt = '%s%s = %s;' % (ind, name, ps[0]) + ''.join(
                        '\n%s%s %s= %s;' % (ind.lstrip('\n'), name, op, p) for p in ps[1:])
                    g.rest = g.rest[:mm.start()] + txt + g.rest[mm.end():]
                    return True
                out.append((('split', name, n), 'split `%s = %s` into %s= steps' % (name, rhs[:50], op),
                            'safe', split))

    # (u32) cast on an index (review)
    for name, (i, typ, stars, arr, init) in loc.items():
        if not stars and not arr and typ in INDEXABLE and re.search(r'\[\s*%s\s*\]' % re.escape(name), f.rest):
            def cast(g, name=name):
                new = re.sub(r'\[\s*%s\s*\]' % re.escape(name), '[(u32)%s]' % name, g.rest)
                if new == g.rest:
                    return False
                g.rest = new
                return True
            out.append((('cast', name), '(u32) cast on every [%s] index' % name, 'review', cast))

    # a for loop as a while loop (no `continue` in its body: it would skip the step)
    for n, m in enumerate(re.finditer(r'\bfor \(([^;()]+); ([^;]+); ([^)]+)\) \{', f.rest)):
        def fw(g, n=n):
            mm = _nth(r'\bfor \(([^;()]+); ([^;]+); ([^)]+)\) \{', g.rest, n)
            if not mm:
                return False
            end = _block_end(g.rest, mm.end())
            body = g.rest[mm.end():end]
            if 'continue' in body:
                return False
            line_start = g.rest.rfind('\n', 0, mm.start()) + 1
            ind = g.rest[line_start:mm.start()]
            g.rest = (g.rest[:mm.start()] + '%s;\n%swhile (%s) {' % (mm.group(1), ind, mm.group(2))
                      + body.rstrip(' ') + '    %s;\n%s' % (mm.group(3), ind) + g.rest[end:])
            return True
        out.append((('for', n), 'for loop #%d (`%s`) as a while loop' % (n + 1, m.group(0)[:50]),
                    'safe', fw))

    # a do/while loop as a while loop (review: a do runs its body once before the first test)
    for n, m in enumerate(re.finditer(r'\bdo \{', f.rest)):
        def dw(g, n=n):
            mm = _nth(r'\bdo \{', g.rest, n)
            if not mm:
                return False
            end = _block_end(g.rest, mm.end())
            tail = re.match(r'\}\s*while \(([^;]*)\);', g.rest[end:])
            if not tail:
                return False
            g.rest = (g.rest[:mm.start()] + 'while (%s) {' % tail.group(1) + g.rest[mm.end():end]
                      + '}' + g.rest[end + tail.end():])
            return True
        out.append((('do', n), 'do/while loop #%d as a while loop' % (n + 1), 'review', dw))
    return out


def _top_split(expr, sep):
    """expr split at sep where the parenthesis/bracket depth is 0."""
    parts, depth, last, i = [], 0, 0, 0
    while i < len(expr):
        c = expr[i]
        if c in '([':
            depth += 1
        elif c in ')]':
            depth -= 1
        elif depth == 0 and expr.startswith(sep, i):
            parts.append(expr[last:i].strip())
            last = i + len(sep)
            i = last
            continue
        i += 1
    parts.append(expr[last:].strip())
    return parts


def _block_end(text, start):
    """Index of the `}` closing the block that begins at start (just after its `{`)."""
    depth, i = 1, start
    while i < len(text):
        if text[i] == '{':
            depth += 1
        elif text[i] == '}':
            depth -= 1
            if not depth:
                return i
        i += 1
    return len(text)


# ---- scoring in worker processes -----------------------------------------------------------------

_W = {}


def _init(fn):
    _W['fn'] = fn
    _W['dir'] = pathlib.Path(os.environ.get('TMP') or os.environ.get('TEMP') or '/tmp') / (
        'leversweep_%d' % os.getpid())
    _W['dir'].mkdir(exist_ok=True)       # one folder per worker, reused: nothing piles up


def _score(job):
    """(index, lined-up score, positional score or None). 9999 = did not compile."""
    idx, src = job
    fn, d = _W['fn'], _W['dir']
    if fn not in qt._target:
        qt._target[fn] = qt._dis(fn, os.path.join(qt._perm(fn), 'target.o'))
    c, o = d / 'v.c', d / 'v.o'
    c.write_text(src, encoding='utf-8')
    if o.exists():
        o.write_bytes(b'')               # a failed compile must not score the previous object
    subprocess.run(qt._compiler(fn) + ['-c', str(c), '-o', str(o)], capture_output=True, text=True)
    if not o.exists() or o.stat().st_size == 0:
        return idx, 9999, None
    a, b = qt._target[fn], qt._dis(fn, str(o))
    ab, bb = qt._branchless(a), qt._branchless(b)
    ops = difflib.SequenceMatcher(None, ab, bb, autojunk=False).get_opcodes()
    s = sum(max(i2 - i1, j2 - j1) for t, i1, i2, j1, j2 in ops if t != 'equal')
    pos = None
    if s == 0:
        pos = sum(1 for x, y in zip(a, b) if x != y) + abs(len(a) - len(b))
    return idx, s, pos


# ---- one function ---------------------------------------------------------------------------------

def sweep(unit, fn, minutes, jobs, setup=True):
    OUT.mkdir(parents=True, exist_ok=True)
    if setup:
        r = subprocess.run([sys.executable, str(HERE / 'perm_setup.py'), unit, fn], cwd=ROOT,
                           capture_output=True, text=True)
        if r.returncode:
            return _write(unit, fn, None, [], 0, 0, 'perm_setup failed: ' + (r.stderr or r.stdout)[-300:])
    src = qt.base(fn)
    if not locate(src, fn):
        return _write(unit, fn, None, [], 0, 0, 'function not found in base.c')
    f = Func(src, fn)
    L = levers(f)
    t0, deadline = time.time(), time.time() + minutes * 60
    results = []                                         # (score, pos, [lever indexes])
    tried = set()

    def variant(combo):
        g = f.copy()
        for k in combo:
            if not L[k][3](g):
                return None
        return g.render()

    def run(combos, pool):
        batch = []
        for combo in combos:
            key = tuple(sorted(combo))
            if key in tried or len({L[k][0] for k in combo}) < len(combo):
                continue
            tried.add(key)
            s = variant(combo)
            if s is not None:
                batch.append((len(results) + len(batch), s, list(combo)))
        if not batch:
            return False
        order = {b[0]: b[2] for b in batch}
        for idx, sc, pos in pool.imap_unordered(_score, [(b[0], b[1]) for b in batch], chunksize=4):
            results.append((sc, pos, order[idx]))
            if sc == 0 and pos == 0:
                return True
            if time.time() > deadline:
                pool.terminate()
                return True
        return False

    with multiprocessing.Pool(jobs, _init, (fn,)) as pool:
        base_score = pool.apply(_score, ((0, src),))[1]
        if base_score == 0:
            return _write(unit, fn, 0, [], 0, time.time() - t0, 'already exact in the snapshot')
        done = run([[k] for k in range(len(L))], pool)
        single = {r[2][0]: r[0] for r in results}
        n1 = len(results)
        ks = sorted(single, key=lambda k: single[k])

        def beam(size, beam_width=40):
            """The best combinations of size-1 levers, each extended by every lever."""
            top = sorted((r for r in results if len(r[2]) == size - 1 and r[0] < 9999),
                         key=lambda r: r[0])[:beam_width]
            return [[*r[2], k] for r in top for k in ks if k not in r[2]]

        # Two searches take turns, one batch each, so neither starves the other: deeper along the
        # best branches (fixes of three changes: PlaceBall) and every pair (two changes that each
        # do nothing alone: GoDynObj).
        pairs = sorted(([a, b] for i, a in enumerate(ks) for b in ks[i + 1:] if L[a][0] != L[b][0]),
                       key=lambda p: single[p[0]] + single[p[1]])
        deep, level = [], 1
        while not done and time.time() < deadline and (pairs or level < 5):
            if not deep and level < 5:
                level += 1
                deep = beam(level)
            for queue in (deep, pairs):
                if queue and not done and time.time() < deadline:
                    batch = queue[:400]
                    del queue[:400]
                    done = run(batch, pool)
    return _write(unit, fn, base_score, results, len(L), time.time() - t0, None, f, L, n1)


def _write(unit, fn, base_score, results, nlev, secs, note, f=None, L=None, n1=0):
    lines = ['%s %s' % (unit, fn)]
    best = sorted(results, key=lambda r: (r[0], r[1] if r[1] is not None else 9999, len(r[2])))
    exact = [r for r in best if r[0] == 0 and r[1] == 0]
    if note:
        lines.append(note)
    else:
        lines.append('base %s, %d levers, %d variants (%d singles) in %.0f s; best %s%s'
                     % (base_score, nlev, len(results), n1, secs, best[0][0] if best else '-',
                        ' EXACT' if exact else ''))
        for sc, pos, combo in best[:12]:
            kinds = {L[k][2] for k in combo}
            lines.append('\n%s%s [%s]' % (sc, ' (exact)' if sc == 0 and pos == 0 else '',
                                         'review' if 'review' in kinds else 'safe'))
            lines += ['  - ' + L[k][1] for k in combo]
            if any(L[k][0][0] == 'id' for k in combo):
                lines.append('  fake match: EA did not write an identity wrapper. Look in TW07 '
                             '(docs/reference-builds/tw07-ps3/cu/) for a real helper or macro at this '
                             'spot and use it; otherwise name it fn_<caller address>_Read with a '
                             '`// fake match:` comment.')
        if best and best[0][0] < base_score:
            g = f.copy()
            for k in best[0][2]:
                L[k][3](g)
            a = f.render()[f.head:].split('\n')
            b = g.render()[f.head:].split('\n')
            lines.append('\nchanged lines of the best variant (base.c):')
            lines += [l for l in difflib.unified_diff(a, b, lineterm='', n=1)][2:80]
    (OUT / ('%s.txt' % fn)).write_text('\n'.join(lines) + '\n', encoding='utf-8')
    top = best[0] if best else None
    row = '\t'.join(str(x) for x in (
        unit, fn, base_score if base_score is not None else '-', top[0] if top else '-',
        'EXACT' if exact else '', ('review' if top and any(L[k][2] == 'review' for k in top[2]) else 'safe')
        if top else '', ' + '.join(L[k][1] for k in top[2]) if top else (note or '')))
    with open(OUT / 'summary.tsv', 'a', encoding='utf-8') as s:
        s.write(row + '\n')
    print(row, flush=True)
    return row


def from_report(min_pct, skip):
    r = json.load(open(ROOT / 'build/GW4E69/report.json'))
    todo = []
    for u in r['units']:
        unit = u['name'].split('/', 1)[1]
        if unit in skip or '/' in unit:
            continue
        for fnc in u.get('functions', []):
            p = fnc.get('fuzzy_match_percent') or 0
            if min_pct <= p < 100:
                todo.append((p, unit, fnc['name']))
    return [(u, n) for p, u, n in sorted(todo, reverse=True)]


def main():
    argv = sys.argv[1:]

    def opt(name, default, conv):
        if name in argv:
            k = argv.index(name)
            v = conv(argv[k + 1])
            del argv[k:k + 2]
            return v
        return default
    minutes = opt('--minutes', 6.0, float)
    jobs = opt('-j', os.cpu_count() or 4, int)
    min_pct = opt('--min', 95.0, float)
    skip = set(opt('--skip', '', str).split(',')) - {''}
    if '--from-report' in argv:
        todo = from_report(min_pct, skip)
        print('%d functions, about %.1f hours at %.1f min each' % (len(todo), len(todo) * minutes / 60,
                                                                   minutes), flush=True)
    else:
        todo = [(argv[0], argv[1])]
    for unit, fn in todo:
        try:
            sweep(unit, fn, minutes, jobs)
        except Exception as e:                           # one bad function never stops the night
            _write(unit, fn, None, [], 0, 0, 'error: %r' % (e,))


if __name__ == '__main__':
    main()
