"""Check a batch of proposed function names against the call graph before they are applied.
    python tools/match/namecheck.py <batch.tsv>
batch.tsv (tab-separated, a header row is allowed):
    address, current name, new name, tier, evidence codes, evidence, purpose
Exit 1 if any row fails; each failure says why. Run on the current build (`ninja`).

Hard errors (always):
  tier      must be T1, T2 or T3.
  suffix    a T3 (provisional) name ends with its address, `Name_80012CB4`, so it can always be traced
            to the disassembly and is visibly unconfirmed; T1/T2 names carry no address.
Flags (a reviewer may keep the row by writing `namecheck-ok: <why>` in its evidence):
  prefix    the name's system prefix (text before the first `_`) must be the unit's name, the prefix
            of a named function in the same unit, of a named caller or callee, or of another row of
            the batch for the same unit whose codes include E1 or E2 (EA's text or a confirmed TW06
            name establishes a prefix). A `Render_` name for code only career-mode code calls fails.
  domain    a name that claims a subsystem (Render/Draw/Texture..., Audio/Sound/Voice...,
            Card/MemCard, Pad/Controller/Rumble, DVD/Disc) must reach it: the function, its callees
            two calls deep, or the globals and strings they use must touch that subsystem."""
import collections, pathlib, re, sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import callgraph                                     # noqa: E402

ROOT = callgraph.ROOT
DOMAINS = {
    'graphics': ({'render', 'draw', 'tex', 'texture', 'shader', 'gfx', 'mesh', 'vtx', 'vertex',
                  'sprite', 'polygon'},
                 r'^(GX|GD|PSMTX|C_MTX|MTX|VI)|Tex|Shader|Render|Draw|Gfx|LLObj|^RC_|^FO_|Mesh|Vtx'),
    'audio': ({'audio', 'sound', 'snd', 'voice', 'music', 'sfx'},
              r'^(AX|AI|DSP|ARQ|MIX|SP)|hlaud|Aud|Snd|Audio|Sound|Voice|Music'),
    'memory card': ({'card', 'memcard'}, r'^CARD|^MC_|Card'),
    'controller': ({'pad', 'controller', 'rumble'}, r'^PAD|Pad|Controller|Rumble'),
    'disc': ({'dvd', 'disc'}, r'^DVD|Disc'),
}
HEX_SUFFIX = re.compile(r'_([0-9A-F]{8})$')


def words(name):
    """UFont_DrawString -> {'ufont', 'draw', 'string'}"""
    out = set()
    for part in name.split('_'):
        out.update(w.lower() for w in re.findall(r'[A-Z]+(?![a-z])|[A-Z]?[a-z]+|\d+', part))
    return out


def prefix(name):
    name = HEX_SUFFIX.sub('', name)
    return name.split('_')[0] if '_' in name else name


def load(path):
    rows = []
    for l in pathlib.Path(path).read_text(encoding='utf-8-sig').splitlines():
        f = [x.strip() for x in l.split('\t')]
        if len(f) < 3 or not f[0] or f[0].startswith('#') or f[0].lower() == 'address':
            continue
        f += [''] * (7 - len(f))
        rows.append(dict(addr=int(f[0].removeprefix('0x'), 16), cur=f[1], new=f[2], tier=f[3],
                         codes=f[4], evidence=f[5]))
    return rows


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    rows = load(sys.argv[1])
    unit_of, calls, refs, callers = {}, {}, {}, collections.defaultdict(set)
    for o in callgraph.current_objects():
        funcs, c, r = callgraph.read_obj(o)
        u = o.relative_to(callgraph.OBJ).with_suffix('').as_posix()
        for f, _, _ in funcs:
            unit_of[f] = u
        for f, cs in c.items():
            calls[f] = cs
            for x in cs:
                callers[x].add(f)
        refs.update(r)
    by_unit = collections.defaultdict(set)
    for f, u in unit_of.items():
        by_unit[u].add(f)

    def named(f):
        return not re.match(r'^(fn|lbl|func)_[0-9A-Fa-f]+$', f)

    def reach(f):
        seen, frontier, out = {f}, [f], set()
        for _ in range(2):
            nxt = []
            for g in frontier:
                for c in calls.get(g, []):
                    out.add(c)
                    if c not in seen:
                        seen.add(c)
                        nxt.append(c)
            frontier = nxt
        for g in seen:
            for r in refs.get(g, []):
                out.add(r[1] if isinstance(r, tuple) else r)
        return out

    established = collections.defaultdict(set)     # unit -> prefixes set up by E1/E2 rows
    for r in rows:
        u = unit_of.get(r['cur'])
        if u and re.search(r'\bE[12]\b', r['codes']):
            established[u].add(prefix(r['new']))
    errors, flags = [], []
    for r in rows:
        tag = '%08X %s -> %s' % (r['addr'], r['cur'], r['new'])
        if r['tier'] not in ('T1', 'T2', 'T3'):
            errors.append('%s: tier "%s" is not T1/T2/T3' % (tag, r['tier']))
        m = HEX_SUFFIX.search(r['new'])
        if r['tier'] == 'T3' and (not m or int(m.group(1), 16) != r['addr']):
            errors.append('%s: a T3 name must end with _%08X' % (tag, r['addr']))
        if r['tier'] in ('T1', 'T2') and m:
            errors.append('%s: only T3 names carry the address' % tag)
        u = unit_of.get(r['cur'])
        if u is None:
            errors.append('%s: %s is not a function in the current build' % (tag, r['cur']))
            continue
        ok = 'namecheck-ok:' in r['evidence']
        p = prefix(r['new'])
        near = {prefix(f) for f in by_unit[u] | callers.get(r['cur'], set()) | set(calls.get(r['cur'], []))
                if named(f) and f != r['cur']}
        if p != u.split('/')[-1] and p not in near and p not in established[u] and not ok:
            flags.append('%s: prefix %s is not the unit (%s) nor used by its unit, callers or callees (%s)'
                         % (tag, p, u, ', '.join(sorted(near)) or 'none named'))
            w = words(r['new'])
        else:
            # an established prefix is not a claim of its own (CameraController_ is the camera's)
            w = words(HEX_SUFFIX.sub('', r['new'])[len(p):])
        for dom, (vocab, marker) in DOMAINS.items():
            if w & vocab and not ok:
                hits = [x for x in reach(r['cur']) | {u.split('/')[-1]} if re.search(marker, x)]
                if not hits:
                    flags.append('%s: the name says %s but nothing it reaches (2 calls deep) touches %s'
                                 % (tag, '/'.join(sorted(w & vocab)), dom))
    for e in errors:
        print('ERROR', e)
    for f in flags:
        print('FLAG ', f)
    print('%d rows: %d errors, %d flags' % (len(rows), len(errors), len(flags)))
    return 1 if errors or flags else 0


if __name__ == '__main__':
    sys.exit(main())
