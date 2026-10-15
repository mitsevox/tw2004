"""Check a batch of proposed function names against the call graph before they are applied.
    python tools/naming/namecheck.py <batch.tsv>
batch.tsv: name.py's batch (tab-separated: address, current name, new name[, comment]).
Run on the current build (`ninja`). Exit 1 on an error; flags are for the reviewer to weigh.

Errors:
  missing   the current name is not a function in the current build.
  format    the new name is a placeholder or carries an address (`fn_`, `lbl_`, `_80082E40`).
Flags:
  prefix    the name's system prefix (text before the first `_`) is not the unit's name, nor the
            prefix of a named function in the same unit, of a named caller or callee, or of
            another row of the batch for the same unit. A `Render_` name for code only
            career-mode code calls is flagged.
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
        rows.append(dict(addr=int(f[0].removeprefix('0x'), 16), cur=f[1], new=f[2]))
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

    batch = collections.defaultdict(list)          # unit -> rows of the batch in it
    for r in rows:
        if r['cur'] in unit_of:
            batch[unit_of[r['cur']]].append(r)
    errors, flags = [], []
    for r in rows:
        tag = '%08X %s -> %s' % (r['addr'], r['cur'], r['new'])
        if re.match(r'^(fn|lbl|func)_', r['new']) or re.search(r'[0-9A-F]{8}', r['new']):
            errors.append('%s: a name carries no placeholder or address' % tag)
        u = unit_of.get(r['cur'])
        if u is None:
            errors.append('%s: %s is not a function in the current build' % (tag, r['cur']))
            continue
        p = prefix(r['new'])
        near = {prefix(f) for f in by_unit[u] | callers.get(r['cur'], set()) | set(calls.get(r['cur'], []))
                if named(f) and f != r['cur']}
        near |= {prefix(o['new']) for o in batch[u] if o is not r}
        if p != u.split('/')[-1] and p not in near:
            flags.append('%s: prefix %s is not the unit (%s) nor used by its unit, callers or callees (%s)'
                         % (tag, p, u, ', '.join(sorted(near)) or 'none named'))
            w = words(r['new'])
        else:
            # an established prefix is not a claim of its own (CameraController_ is the camera's)
            w = words(HEX_SUFFIX.sub('', r['new'])[len(p):])
        for dom, (vocab, marker) in DOMAINS.items():
            if w & vocab:
                hits = [x for x in reach(r['cur']) | {u.split('/')[-1]} if re.search(marker, x)]
                if not hits:
                    flags.append('%s: the name says %s but nothing it reaches (2 calls deep) touches %s'
                                 % (tag, '/'.join(sorted(w & vocab)), dom))
    for e in errors:
        print('ERROR', e)
    for f in flags:
        print('FLAG ', f)
    print('%d rows: %d errors, %d flags' % (len(rows), len(errors), len(flags)))
    return 1 if errors else 0


if __name__ == '__main__':
    sys.exit(main())
