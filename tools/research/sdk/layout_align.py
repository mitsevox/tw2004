"""Align each reference object's function list against the game's functions around its exact matches
(longest common subsequence on function size, exact matches pinned), and propose names for unnamed
game functions that align to a reference function of identical size.

    python tools/research/sdk/layout_align.py <obj_dir> [<out.json>]

<obj_dir> is a tree of split or library objects (as for match_sdk_names.py). Proposals of 12 bytes
or less are unreliable (tiny setters collide); apply only the larger ones, then re-run: every
applied name is a new anchor. Does not edit symbols.txt.
"""
import json, re, glob, os, collections, sys
SYMS = 'C:/dev/tw2004/config/GW4E69/symbols.txt'
exec(open('C:/dev/tw2004/tools/research/sdk/sdk_units.py').read().split("tw={}")[0])
OBJ = sys.argv[1]; OUT = sys.argv[2] if len(sys.argv) > 2 else 'layout_props.json'
by_addr = {}; by_name = collections.defaultdict(list)
for l in open(SYMS):
    m = re.match(r'^(\S+) = \.(text|init):0x([0-9A-Fa-f]+); // type:function size:0x([0-9A-Fa-f]+)', l)
    if m: by_addr[int(m.group(3), 16)] = (m.group(1), int(m.group(4), 16)); by_name[m.group(1)].append(int(m.group(3), 16))
addrs = sorted(by_addr); pos = {a: i for i, a in enumerate(addrs)}
def lcs(A, B, pinned):
    # A: game [(addr,name,size)], B: ref [(name,size)]; pinned: set of (i,j) forced pairs
    n, m = len(A), len(B); dp = [[0] * (m + 1) for _ in range(n + 1)]
    def ok(i, j):
        if (i, j) in pinned: return True
        if any(pi == i or pj == j for pi, pj in pinned): return False
        return A[i][2] == B[j][1]
    for i in range(n - 1, -1, -1):
        for j in range(m - 1, -1, -1):
            dp[i][j] = max(dp[i + 1][j], dp[i][j + 1], dp[i + 1][j + 1] + (1 if ok(i, j) else 0))
    i = j = 0; pairs = []
    while i < n and j < m:
        if ok(i, j) and dp[i][j] == dp[i + 1][j + 1] + 1: pairs.append((i, j)); i += 1; j += 1
        elif dp[i + 1][j] >= dp[i][j + 1]: i += 1
        else: j += 1
    return pairs
props = {}; report = []
for o in sorted(glob.glob(OBJ + '/**/*.o', recursive=True)):
    funcs, _ = parse(o); rel = os.path.relpath(o, OBJ).replace(chr(92), '/')
    if not funcs: continue
    B = [(n, sz) for n, sz in funcs]
    anchors = [(by_name[n][0], k) for k, n in enumerate(B) for n in [n[0]] if len(by_name.get(n, [])) == 1 and by_addr[by_name[n][0]][0] == n]
    anchors = sorted(a for a in anchors if 0x8012E000 <= a[0] < 0x8017E000)
    if not anchors: continue
    lo = pos[anchors[0][0]] - anchors[0][1]; hi = pos[anchors[-1][0]] + (len(B) - anchors[-1][1])
    lo = max(0, lo - 2); hi = min(len(addrs), hi + 2)
    A = [(a, by_addr[a][0], by_addr[a][1]) for a in addrs[lo:hi]]
    ai = {a: i for i, (a, n, s) in enumerate(A)}
    pinned = {(ai[a], k) for a, k in anchors}
    pairs = lcs(A, B, pinned)
    got = 0
    for i, j in pairs:
        a, cur, sz = A[i]; n, rsz = B[j]
        if cur.startswith('fn_') and sz == rsz:
            props[hex(a)] = (rel, n, sz); got += 1
    report.append((rel, len(B), len(anchors), got))
json.dump(props, open(OUT, 'w'), indent=0)
print('proposals:', len(props), 'bytes', sum(v[2] for v in props.values()))
for rel, nb, na, got in report:
    if got: print(f'  {rel:36s} ref {nb:3d} fns, anchors {na:3d}, new {got}')
for a, (rel, n, sz) in sorted(props.items(), key=lambda kv: int(kv[0], 16)):
    print(f'{a} {n:36s} {sz:5d}  {rel}')
