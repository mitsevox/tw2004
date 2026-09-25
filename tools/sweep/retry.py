"""Second pass over sweep functions that compiled but were not exact.

Compiles a function's C on its own and compares it to the original instructions, masking the
fields the linker fills in (relocations). Fast enough to try many small variants per function.

    python retry.py score            -> distribution of how far off the skipped m2c functions are
    python retry.py try [maxdiff]    -> apply mechanical variants, write retry_hits.json
"""
import os, re, sys, json, struct, subprocess, tempfile, itertools
from concurrent.futures import ThreadPoolExecutor

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import sweep_m2c
from paths import ROOT, HERE, WORK, MWCC_CMD  # noqa: E402

SKIP = WORK + '/sweep_skip.json'
HITS = HERE + '/retry_hits.json'
MWCC = MWCC_CMD + ['-nodefaults', '-proc', 'gekko', '-align', 'powerpc',
        '-enum', 'int', '-fp', 'hardware', '-Cpp_exceptions', 'off', '-O4,p', '-inline', 'smart', '-maxerrors', '1',
        '-nosyspath', '-RTTI', 'off', '-fp_contract', 'on', '-str', 'reuse', '-common', 'on', '-use_lmw_stmw', 'on',
        '-lang=c', '-i', ROOT + '/include']


def elf_sections(d):
    shoff = struct.unpack('>I', d[0x20:0x24])[0]
    shentsize, shnum, shstrndx = struct.unpack('>HHH', d[0x2E:0x34])
    secs = []
    for i in range(shnum):
        h = d[shoff + i * shentsize: shoff + (i + 1) * shentsize]
        nm, typ, flags, addr, off, size, link, info, align, entsize = struct.unpack('>IIIIIIIIII', h)
        secs.append(dict(nm=nm, typ=typ, addr=addr, off=off, size=size, link=link, info=info, entsize=entsize))
    so = secs[shstrndx]['off']
    for s in secs:
        s['name'] = d[so + s['nm']:d.index(b'\0', so + s['nm'])].decode()
    return secs


def elf_symbols(d, secs):
    out = []
    for s in secs:
        if s['typ'] != 2:
            continue
        strs = secs[s['link']]['off']
        for i in range(s['size'] // 16):
            nm, val, size, info, other, shndx = struct.unpack('>IIIBBH', d[s['off'] + 16 * i: s['off'] + 16 * i + 16])
            name = d[strs + nm:d.index(b'\0', strs + nm)].decode()
            out.append(dict(name=name, value=val, size=size, shndx=shndx))
    return out


class Original:
    """Original instruction words, read from the linked (byte-identical) main.elf."""
    def __init__(self):
        self.d = open(ROOT + '/build/GW4E69/main.elf', 'rb').read()
        self.secs = [s for s in elf_sections(self.d) if s['addr'] and s['typ'] == 1]
        self.sizes = {}
        for line in open(ROOT + '/config/GW4E69/symbols.txt', encoding='utf-8'):
            m = re.match(r'(\S+) = \.\w+:0x([0-9A-F]+); // type:function size:0x([0-9A-F]+)', line)
            if m:
                a = int(m.group(2), 16)
                self.sizes[m.group(1)] = (a, int(m.group(3), 16))
                self.sizes['%s_%08X' % (m.group(1), a)] = (a, int(m.group(3), 16))

    def words(self, name):
        if name not in self.sizes:
            return None
        addr, size = self.sizes[name]
        for s in self.secs:
            if s['addr'] <= addr < s['addr'] + s['size']:
                o = s['off'] + addr - s['addr']
                return list(struct.unpack('>%dI' % (size // 4), self.d[o:o + size]))
        return None


def compiled_words(src):
    """-> {function: (words, {word index: mask})} for every function in the object, or None."""
    fd, cpath = tempfile.mkstemp(suffix='.c', dir=WORK + '/_retry')
    os.close(fd)
    opath = cpath[:-2] + '.o'
    try:
        open(cpath, 'w', encoding='utf-8').write(src)
        r = subprocess.run(MWCC + ['-c', cpath, '-o', opath], capture_output=True, text=True)
        if r.returncode or not os.path.exists(opath):
            return None
        d = open(opath, 'rb').read()
    finally:
        for p in (cpath, opath):
            if os.path.exists(p):
                os.remove(p)
    secs = elf_sections(d)
    text_i = next((i for i, s in enumerate(secs) if s['name'] == '.text'), None)
    if text_i is None:
        return {}
    for i, s in enumerate(secs):
        if s['typ'] in (1, 8) and s['size'] and s['name'] not in ('.text', '.comment', '.mwcats.text'):
            return 'data'
    text = secs[text_i]
    masks = {}
    for s in secs:
        if s['typ'] == 4 and s['info'] == text_i:
            for i in range(s['size'] // 12):
                off, info, add = struct.unpack('>IIi', d[s['off'] + 12 * i: s['off'] + 12 * i + 12])
                t = info & 0xFF
                m = {10: 0x03FFFFFC, 109: 0x001FFFFF}.get(t, 0x0000FFFF)
                masks[off // 4] = masks.get(off // 4, 0) | m
    out = {}
    for sym in elf_symbols(d, secs):
        if sym['shndx'] == text_i and sym['size'] and sym['name']:
            o = text['off'] + sym['value']
            ws = list(struct.unpack('>%dI' % (sym['size'] // 4), d[o:o + sym['size']]))
            out[sym['name']] = (ws, {k - sym['value'] // 4: v for k, v in masks.items()
                                     if sym['value'] // 4 <= k < (sym['value'] + sym['size']) // 4})
    return out


def diff_count(orig, got):
    ws, masks = got
    n = abs(len(orig) - len(ws))
    for i, (a, b) in enumerate(zip(orig, ws)):
        m = ~masks.get(i, 0) & 0xFFFFFFFF
        if (a & m) != (b & m):
            n += 1
    return n


def source(body, decls):
    return '#include "game_types.h"\n\n' + '\n'.join(decls) + '\n\n' + body + '\n'


def m2c_skipped():
    """Skipped functions whose skip entry is exactly the current m2c output."""
    skip = json.load(open(SKIP))
    out = {}
    for name, c in skip.items():
        r = sweep_m2c.clean(sweep_m2c._cache.get(name))
        if r and r[0] == c:
            out[name] = r
    return out


def score_one(orig, name, body, decls):
    ow = orig.words(name)
    if ow is None:
        return None
    got = compiled_words(source(body, decls))
    if not isinstance(got, dict) or name not in got:
        return None
    return diff_count(ow, got[name])


def cmd_score():
    os.makedirs(WORK + '/_retry', exist_ok=True)
    orig = Original()
    fns = m2c_skipped()
    with ThreadPoolExecutor(24) as ex:
        res = dict(zip(fns, ex.map(lambda n: score_one(orig, n, *fns[n]), fns)))
    json.dump(res, open(WORK + '/retry_scores.json', 'w'), indent=0)
    buckets = {}
    for v in res.values():
        k = 'fail' if v is None else ('0' if v == 0 else '1-2' if v <= 2 else '3-5' if v <= 5 else '6-10' if v <= 10 else '>10')
        buckets[k] = buckets.get(k, 0) + 1
    print(len(res), 'functions;', buckets)


if __name__ == '__main__':
    if sys.argv[1] == 'score':
        cmd_score()


# ---- variants -------------------------------------------------------------------------------

def calls_in(body):
    """[(start of args, end of args, callee)] for each call in the body (not the definition)."""
    out = []
    head_end = body.index('{')
    for m in re.finditer(r'(\b[A-Za-z_]\w*|\))\s*\(', body):
        name = m.group(1)
        if m.start() < head_end or name in ('if', 'while', 'for', 'switch', 'return', 'sizeof'):
            continue
        if name == ')':
            # a call through a function pointer: "(expr)(" or "temp()(": only accept "temp_rN("-style names
            continue
        i = m.end()
        depth, j = 1, i
        while j < len(body) and depth:
            depth += {'(': 1, ')': -1}.get(body[j], 0)
            j += 1
        # skip casts like (s32) and declarations: a call is followed by ; or an operator, never by an identifier
        out.append((i, j - 1, name))
    return out


def arg_pool(body):
    head = body[:body.index('{')]
    params = re.findall(r'\b(arg\d+)\b', head)
    temps = sorted(set(re.findall(r'\b((?:temp|var)_[rf]\d+(?:_\d+)?)\b', body)))
    conds = []
    for m in re.finditer(r'if \((.*)\) \{', body):
        for c in re.split(r'\s*&&\s*', m.group(1)):
            c = c.strip()
            while c.startswith('(') and c.endswith(')'):
                c = c[1:-1].strip()
            mm = re.match(r'(?:\((?:u32|s32|u8|s8|u16|s16|void\*)\)\s*)?(.+?)\s*(?:!=|==|>=|<=|>|<)\s*(?:0U?|NULL)$', c)
            if mm and len(mm.group(1)) < 120:
                conds.append(mm.group(1))
    pool = []
    for x in params + temps + conds:
        if x not in pool:
            pool.append(x)
    return pool[:8]


def unprototype(decls):
    return [re.sub(r'^(\S[^()]*\b\w+)\((.*)\);$', r'\1();', d) if '(' in d and not d.startswith('extern') else d
            for d in decls]


def void_to_u8(body, decls):
    return body.replace('void*', 'u8*').replace('void *', 'u8 *'), [d.replace('void*', 'u8*') for d in decls]


def fix_head(body):
    # keep the definition's own parameter types as void* (only locals/globals change)
    return body


def try_one(orig, name, body, decls, maxiter=4):
    ow = orig.words(name)
    if ow is None:
        return None

    def sc(b, d):
        got = compiled_words(source(b, d))
        if not isinstance(got, dict) or name not in got:
            return 999
        return diff_count(ow, got[name])

    bases = []
    for d0 in (decls, unprototype(decls)):
        bases.append((body, d0))
        b2, d2 = void_to_u8(body, d0)
        bases.append((b2, d2))
    best = None
    for b, d in bases:
        s = sc(b, d)
        if best is None or s < best[0]:
            best = (s, b, d)
    if best[0] == 0:
        return best
    if best[0] >= 999:
        return best
    # greedy: try every single edit, take the best, repeat
    for _ in range(maxiter):
        s0, b0, d0 = best
        for nb, nd in moves(b0, d0):
            s = sc(nb, nd)
            if s < best[0]:
                best = (s, nb, nd)
                if s == 0:
                    return best
        if best[0] >= s0:
            break
    return best


def moves(b0, d0):
    pool = arg_pool(b0)
    for (i, j, callee) in calls_in(b0):
        args = b0[i:j].strip()
        if args.count(',') >= 3:
            continue
        cands = list(pool) + [x + ', ' + y for x, y in itertools.permutations(pool[:5], 2)]
        for c in cands:
            if c in args:
                continue
            yield b0[:i] + (args + ', ' + c if args else c) + b0[j:], d0
            if args:
                yield b0[:i] + c + ', ' + args + b0[j:], d0
    # local and parameter types: narrow <-> int
    for m in re.finditer(r'(u8|s8|u16|s16|u32|s32)(\s+\**\s*)((?:temp|var)_\w+|arg\d+)', b0):
        for t in ('s32', 'u32', 'u8', 's8', 'u16', 's16'):
            if t != m.group(1):
                yield b0[:m.start()] + t + m.group(2) + m.group(3) + b0[m.end():], d0
    # return type
    m = re.match(r'(\w+\**)\s+(\w+)\(', b0)
    if m:
        for t in ('s32', 'u32', 'u8', 's8', 's16', 'u16', 'void'):
            if t != m.group(1):
                yield t + b0[len(m.group(1)):], d0


def _unused_old_greedy(best, sc, maxiter=4):
    for _ in range(maxiter):
        s0, b0, d0 = best
        pool = arg_pool(b0)
        improved = False
        for (i, j, callee) in calls_in(b0):
            args = b0[i:j].strip()
            n = 0 if not args else args.count(',') + 1   # rough; only used to cap
            if n >= 4:
                continue
            cands = []
            for x in pool:
                cands.append(x)
            for x, y in itertools.permutations(pool[:5], 2):
                cands.append(x + ', ' + y)
            for c in cands:
                if c in args:
                    continue
                nb = b0[:i] + (args + ', ' + c if args else c) + b0[j:]
                s = sc(nb, d0)
                if s < best[0]:
                    best = (s, nb, d0)
                    if s == 0:
                        return best
            if best[0] < s0:
                improved = True
                break
        if not improved:
            break
    return best


def cmd_try(maxdiff):
    os.makedirs(WORK + '/_retry', exist_ok=True)
    orig = Original()
    fns = m2c_skipped()
    scores = json.load(open(WORK + '/retry_scores.json'))
    done = set()
    for p in os.listdir(ROOT + '/src/unsorted'):
        pass
    names = [n for n in fns if scores.get(n) is None or 0 < scores[n] <= maxdiff]
    hits = json.load(open(HITS)) if os.path.exists(HITS) else {}
    names = [n for n in names if n not in hits]

    def run(n):
        try:
            return n, try_one(orig, n, *fns[n])
        except Exception as e:
            return n, None
    better = 0
    best = {}
    with ThreadPoolExecutor(24) as ex:
        for n, r in ex.map(run, names):
            if r and r[0] == 0:
                hits[n] = [r[1], r[2]]
            elif r and r[0] < (scores.get(n) or 999):
                better += 1
            if r and 0 < r[0] < 999:
                best[n] = [r[0], r[1], r[2]]
    json.dump(hits, open(HITS, 'w'), indent=0)
    json.dump(best, open(WORK + '/retry_best.json', 'w'), indent=0)
    print('tried', len(names), '- exact now:', len(hits), '- improved but not exact:', better)


if __name__ == '__main__' and sys.argv[1] == 'try':
    cmd_try(int(sys.argv[2]) if len(sys.argv) > 2 else 10)
