"""Simulate MWCC's instruction scheduler (GC/2.6, -proc gekko = the PPC750 model) on a mwccdbg dump:
predict a block's order before and after register allocation, and search for the one dead value
that would turn it into a target (EA's) order.

    python tools/matching/sched750.py predict  <dump dir> <block>            # e.g. build/mwccdbg/fn_X B23
    python tools/matching/sched750.py deadsearch <dump dir> <block> <target.txt> [--two]
    python tools/matching/sched750.py validate <dump dir> [<dump dir> ...]

<dump dir> is `python tools/matching/mwccdbg.py src/<Unit>.c <fn>` output; <block> is its block name
(B23). `predict` runs the first scheduling pass on the block as it enters it (the dump before the
first after-scheduling file) and the last pass on the allocated block (the dump before the last
after-scheduling file) and prints each next to the compiler's real output. `deadsearch` takes the
target final order (one `op args` per line in the dumps' notation, physical registers, e.g.
`add r6,r3,r0`, taken from EA's asm), keeps the current registers, and tries: a dead `srawi` of each
value the block defines (what a `(u32)(s64)(s32)x` round trip on x leaves; it is placed right after
x's definition and deleted after allocation) and an extra copy (`mr`, deleted by coalescing); with
--two, pairs of them. It lists the variants whose first pass + mapping to the current registers +
last pass give the target. The registers are held fixed: a variant also changes live ranges, so
confirm it with a build (and rasim.py). `validate` counts the blocks of the dumps whose real
schedules the model reproduces.

Transcribed from mwcceppc.exe (GC/2.6) by lane b8, 2026-09-27: scheduleblock 0x507d80, the pick
0x507f50, findsuccessors 0x508090, addsuccessor 0x508480; the 750 MachineInfo at 0x5d2c70 (issue
width 2) with latency 0x579180, can_issue 0x578e90, issue 0x578e10, advance_clock 0x578820 and the
timing table 0x5d2c98 (7 bytes per opcode: unit, latency, cycles in the first and second stage).
- Dependences are built bottom-up from per-register def/use lists that are never cleared inside a
  block: a def gets a true edge (its latency) to EVERY later use of that register and a 0-latency
  edge to every later def; a use gets a 0-latency edge to every later def. Loads are unordered
  among themselves; leaves get a 0-latency edge to the block's branch. Height = max(own latency,
  edge latency + successor height); "urgent" = max height - height <= current cycle.
- The ready nodes stay in block order. The pick: urgent beats not urgent, then the node whose
  issue would make more successors ready, then the greater height, then (first pass only) a
  smaller opcode-info byte (offset 9), else the earlier node. Two issues per cycle.
- 750 units: loads/stores LSU (2 stages, latency 2); add/rlwinm/subf/mr/srawi/li either integer
  unit (IU1 first), latency 1; cmp latency 3; mulli IU1 only for 3 cycles, mulhw/mullw 5.
- Undocumented integer-unit rule: when only one integer unit is free, an integer op cannot issue
  if it reads or writes the register defined by the op in the other unit or by an integer op that
  completed in the previous cycle.
- Assumption: a completion queue of 6 entries retiring two finished ops per cycle (the exe sets 8
  free entries; 6 is what reproduces the dumps, the exact retire timing is not transcribed).
- The first pass reads a copy's source instead of its destination (the dumps show `cmpl r39`
  where the input had `mr r45,r39; cmpl r45`); the model does the same before scheduling.
Validation: 2146 of 2240 integer-only blocks of the round-5 batch dumps (both passes; `validate`
on the main checkout's build/mwccdbg/*/) reproduced exactly; fn_800949D0's block B23 in both
passes; deadsearch on its old dump finds the one lever that closed it (a dead srawi of n). FPU pipelines, calls and record forms are not
modelled (such blocks are skipped). Found: fn_800949D0 exact from the deadsearch lead (a dead
srawi of n moved the whole first pass; docs/decomp-notes.md "New from round 7")."""
import itertools, os, re, sys

# opcode -> (unit, latency, cycles in stage 1, cycles in stage 2); unit 0 BPU, 1 IU1, 2 any IU, 3 LSU
T = {}
LOADS = {'lhz', 'lwz', 'lwzx', 'lbz', 'lha', 'lhzx', 'lbzx', 'lhax', 'lfs', 'lfd', 'lfsx', 'lfdx', 'lwzu'}
STORES = {'stw', 'sth', 'stb', 'stwx', 'sthx', 'stbx', 'stfs', 'stfd'}
BRANCH = {'bt', 'bf', 'b', 'blr', 'bctr'}
for _op in LOADS | STORES:
    T[_op] = (3, 2, 1, 1)
for _op in ('rlwinm', 'add', 'subf', 'mr', 'srawi', 'li', 'addi', 'extsh', 'extsb', 'neg', 'or', 'and',
            'addis', 'lis', 'rlwimi', 'slw', 'srw', 'xor', 'ori', 'subfic', 'nor', 'addic', 'addze',
            'subfe', 'adde', 'addc', 'subfc', 'sraw', 'cntlzw'):
    T[_op] = (2, 1, 1, 0)
for _op in ('cmp', 'cmpl', 'cmpi', 'cmpli'):
    T[_op] = (2, 3, 1, 0)
T['mulli'] = (1, 3, 3, 0)
for _op in ('mulhw', 'mullw'):
    T[_op] = (1, 5, 5, 0)
T['mulhwu'] = (1, 6, 5, 0)
for _op in BRANCH:
    T[_op] = (0, 0, 0, 0)
# opcode-info byte 9 (first-pass tie-break)
PROP = {'lhz': 3, 'lwz': 3, 'lwzx': 3, 'lbz': 3, 'lha': 3, 'rlwinm': 2, 'mulli': 2, 'add': 2, 'subf': 2,
        'cmp': 1, 'cmpl': 1, 'cmpi': 1, 'cmpli': 1, 'mr': 0, 'srawi': 2, 'li': 4, 'addi': 2, 'extsh': 2,
        'extsb': 2, 'mulhw': 2, 'mullw': 2, 'mulhwu': 2, 'bt': 0, 'bf': 0}
CQ = 6


def regs_of(s):
    return re.findall(r'\b([rf]\d+|cr\d)\b', s)


def operands(op, args):
    """(reg, is_def) in operand order; r0 as a load/addi base and r2/r13 are not registers here."""
    a = args.split('(')[0]
    rs = regs_of(a)
    if op in BRANCH or op in STORES:
        return [(r, False) for r in rs if r not in ('r2', 'r13')]
    if not rs:
        return []
    parts = [p.strip() for p in a.split(',')]
    out = [(rs[0], True)]
    for r in rs[1:]:
        if r == 'r0' and (op in LOADS or op in ('addi', 'addis')) and len(parts) > 1 and parts[1] == 'r0':
            continue
        out.append((r, False))
    return [(r, d) for r, d in out if r not in ('r2', 'r13')]


class Node:
    pass


def build(ins):
    nodes = []
    for k, (op, args) in enumerate(ins):
        x = Node()
        x.k, x.op, x.args, x.ops = k, op, args, operands(op, args)
        x.lat = x.h = T[op][1]
        x.succ, x.npred, x.early = {}, 0, 0
        nodes.append(x)
    defs, uses, loads, stores = {}, {}, [], []
    branch, maxh = None, 0

    def edge(a, b, true):
        if a is b:
            return
        lat = a.lat if true else 0
        if b.k in a.succ:
            if a.succ[b.k] < lat:
                a.succ[b.k] = lat
                a.h = max(a.h, lat + b.h)
            return
        a.succ[b.k] = lat
        b.npred += 1
        a.h = max(a.h, lat + b.h)
    for x in reversed(nodes):
        for r, d in x.ops:
            if d:
                for y in uses.get(r, []):
                    edge(x, y, True)
                for y in defs.get(r, []):
                    edge(x, y, False)
                defs.setdefault(r, []).insert(0, x)
            else:
                for y in defs.get(r, []):
                    edge(x, y, False)
                uses.setdefault(r, []).insert(0, x)
        if x.op in LOADS:
            for y in stores:
                edge(x, y, True)
            loads.insert(0, x)
        elif x.op in STORES:
            for y in loads + stores:
                edge(x, y, True)
            stores.insert(0, x)
        if not x.succ and branch is not None:
            edge(x, branch, False)
        if x.op in BRANCH:
            branch = x
        maxh = max(maxh, x.h)
    for x in nodes:
        x.slack = maxh - x.h
    return nodes


class M750:
    def __init__(s):
        s.st, s.cq, s.done1, s.done2 = {}, [], None, None

    @staticmethod
    def dep(x, other):
        if other is None or not other.ops or not other.ops[0][1]:
            return False
        return any(r == other.ops[0][0] for r, d in x.ops)

    def can_issue(s, x):
        if len(s.cq) >= CQ:
            return False
        unit = T[x.op][0]
        if unit == 2:
            f1, f2 = 1 not in s.st, 2 not in s.st
            if not (f1 or f2):
                return False
            if f1 and f2:
                return True
            other = s.st[2][0] if f1 else s.st[1][0]
            if s.dep(x, other) or s.dep(x, s.done1) or s.dep(x, s.done2):
                return False
        elif unit in s.st:
            return False
        return not (x.op in STORES and 4 in s.st and s.st[4][0].op in STORES)

    def issue(s, x):
        s.cq.append([x, False])
        unit = T[x.op][0]
        if unit == 2 and 1 not in s.st:
            unit = 1
        s.st[unit] = [x, T[x.op][2]]

    def advance(s):
        s.done1 = s.done2 = None
        for k in s.st:
            if s.st[k][1]:
                s.st[k][1] -= 1
        for _ in range(2):
            if s.cq and s.cq[0][1]:
                s.cq.pop(0)
        for k in (1, 4, 0, 2):
            if k in s.st and s.st[k][1] == 0:
                x = s.st.pop(k)[0]
                for e in s.cq:
                    if e[0] is x:
                        e[1] = True
                if k == 1:
                    s.done1 = x
                if k == 2:
                    s.done2 = x
        if 3 in s.st and s.st[3][1] == 0 and 4 not in s.st:
            x = s.st.pop(3)[0]
            s.st[4] = [x, T[x.op][3]]


def schedule(ins, first_pass=False):
    """Indexes of `ins` in the order the scheduler emits them."""
    nodes = build(ins)
    lst, m, cyc, out = list(nodes), M750(), 0, []

    def frees(x):
        return sum(1 for k in x.succ if nodes[k].npred == 1)

    def better(best, x):
        bu, nu = best.slack <= cyc, x.slack <= cyc
        if bu != nu:
            return nu
        cb, cn = frees(best), frees(x)
        if cb != cn:
            return cn > cb
        if best.h != x.h:
            return x.h > best.h
        return first_pass and PROP.get(best.op, 9) > PROP.get(x.op, 9)
    while lst and cyc < 5000:
        for _ in range(2):
            best = None
            for x in lst:
                if x.npred == 0 and x.early <= cyc and m.can_issue(x) and (best is None or better(best, x)):
                    best = x
            if best is None:
                break
            for k, lat in best.succ.items():
                nodes[k].npred -= 1
                nodes[k].early = max(nodes[k].early, cyc + lat)
            out.append(best.k)
            m.issue(best)
            lst.remove(best)
        m.advance()
        cyc += 1
    return out


def forward_copies(ins):
    """First pass: later reads of a copy's destination read its source."""
    out = list(ins)
    for k, (op, args) in enumerate(ins):
        if op != 'mr':
            continue
        d, s = regs_of(args)[:2]
        for j in range(k + 1, len(out)):
            op2, a2 = out[j]
            ops = operands(op2, a2)
            if any(r == d for r, isdef in ops if not isdef):
                first = 0 if op2 in BRANCH or op2 in STORES else 1
                parts = a2.split(',')
                out[j] = (op2, ','.join(parts[:first] + [re.sub(r'\b%s\b' % d, s, p) for p in parts[first:]]))
            if any(r in (d, s) for r, isdef in ops if isdef):
                break
    return out


def parse_blocks(path):
    blocks, cur = {}, None
    for line in open(path):
        m = re.match(r'^(B\d+):', line)
        if m:
            cur = blocks.setdefault(m.group(1), [])
            continue
        m = re.match(r'^\s+\d+\s+(\S+)\s*(.*)$', line)
        if m and cur is not None:
            cur.append((m.group(1), m.group(2).strip()))
    return blocks


def dumps(d):
    """(first-pass input, first-pass output, before-regalloc, after-regalloc, last input, last output)"""
    fl = sorted(os.listdir(d))
    sch = [f for f in fl if 'after-scheduling' in f]
    pick = lambda f: os.path.join(d, f)
    return (pick(fl[fl.index(sch[0]) - 1]), pick(sch[0]), pick(next(f for f in fl if 'before-regalloc' in f)),
            pick(next(f for f in fl if 'after-regalloc' in f)), pick(fl[fl.index(sch[-1]) - 1]), pick(sch[-1]))


def show(title, ins, order, real):
    got = [ins[k] for k in order]
    print('%s: %s' % (title, 'MATCH' if got == real else 'DIFF'))
    for g, w in zip(got, real):
        print('  %s %-34s %s' % (' ' if g == w else '*', '%s %s' % w, '%s %s' % g))


def dead_free(ins):
    """The block without its dead srawis (a round trip's sign word: nothing in the block reads it)."""
    out = []
    for k, (op, a) in enumerate(ins):
        if op == 'srawi':
            d = regs_of(a)[0]
            if not any(r == d for op2, a2 in ins[k + 1:] for r, isdef in operands(op2, a2) if not isdef):
                continue
        out.append((op, a))
    return out


class Block:
    """One block of a dump: the first pass's input and the virtual -> physical register map."""
    def __init__(s, d, b):
        f0, f1, fb, fa, l0, l1 = dumps(d)
        s.pre_in = forward_copies(parse_blocks(f0)[b])
        s.pre_real, s.post_in, s.post_real = parse_blocks(f1)[b], parse_blocks(l0)[b], parse_blocks(l1)[b]
        before, after = parse_blocks(fb)[b], parse_blocks(fa)[b]
        s.vmap, j = {}, 0
        for op, a in before:          # copies deleted by allocation are missing after it
            if j < len(after) and after[j][0] == op:
                s.vmap.update(zip(regs_of(a), regs_of(after[j][1])))
                j += 1
            elif op == 'mr':
                dst, src = regs_of(a)[:2]
                s.vmap.setdefault(dst, s.vmap.get(src, src))

    def final(s, ins):
        """First pass, drop copies and dead srawis (deleted after allocation), map registers, last pass."""
        pre = [ins[k] for k in schedule(ins, True)]
        phys = []
        for op, a in dead_free(pre):
            if op == 'mr':
                continue
            phys.append((op, re.sub(r'\b([rf]\d+)\b', lambda m: s.vmap.get(m.group(1), m.group(1)), a)))
        return [phys[k] for k in schedule(phys)]


def predict(d, b):
    blk = Block(d, b)
    show('first pass (before allocation)', blk.pre_in, schedule(blk.pre_in, True), blk.pre_real)
    show('last pass (after allocation)', blk.post_in, schedule(blk.post_in), blk.post_real)


def deadsearch(d, b, target_file, two=False):
    blk = Block(d, b)
    target = [tuple(l.strip().split(None, 1)) for l in open(target_file) if l.strip()]
    base = dead_free(blk.pre_in)
    print('current (dead srawis removed) gives the target:', blk.final(base) == target)
    vals = sorted({regs_of(a)[0] for op, a in base if op not in BRANCH and not op.startswith('cmp')
                   and op not in STORES and regs_of(a)})
    cands = [('srawi', v) for v in vals] + [('mr', v) for v in vals]
    hits = 0
    for sel in itertools.chain(((c,) for c in cands), itertools.combinations(cands, 2) if two else ()):
        new = list(base)
        for q, (kind, v) in enumerate(sel):
            at = max([k + 1 for k, (op, a) in enumerate(new) if a.startswith(v + ',')] or [0])
            new.insert(at, ('srawi', 'r%d,%s,0x1f,zero' % (990 + q, v)) if kind == 'srawi'
                       else ('mr', 'r%d,%s' % (990 + q, v)))
        if new[-1][0] not in BRANCH and base[-1][0] in BRANCH:
            continue
        if blk.final(new) == target:
            hits += 1
            print('gives the target: ' + ' + '.join('dead %s of %s' % kv for kv in sel))
    print(hits, 'variant(s); registers were held fixed: confirm with a build (and rasim.py)')


def validate(dirs):
    tot = ok = 0
    for d in dirs:
        f0, f1, fb, fa, l0, l1 = dumps(d)
        for first, (fi, fo) in ((True, (f0, f1)), (False, (l0, l1))):
            bi, bo = parse_blocks(fi), parse_blocks(fo)
            for b, ins in bi.items():
                if b not in bo or len(ins) < 3 or any(op not in T for op, a in ins):
                    continue
                if first:
                    ins = forward_copies(ins)
                if sorted(x[0] for x in ins) != sorted(x[0] for x in bo[b]):
                    continue
                tot += 1
                got = [ins[k] for k in schedule(ins, first)]
                ok += [(g[0], g[1].split(',')[0]) for g in got] == [(w[0], w[1].split(',')[0]) for w in bo[b]]
    print('%d of %d blocks reproduced' % (ok, tot))


if __name__ == '__main__':
    a = sys.argv[1:]
    if not a or a[0] not in ('predict', 'deadsearch', 'validate'):
        sys.exit(__doc__)
    if a[0] == 'predict':
        predict(a[1], a[2])
    elif a[0] == 'deadsearch':
        deadsearch(a[1], a[2], a[3], '--two' in a)
    else:
        validate(a[1:])
