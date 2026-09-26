"""Replay MWCC's register allocator from a mwccdbg dump, and search declaration orders against it.

    python tools/match/rasim.py <dump dir> replay [--fpr] [-n 20]
    python tools/match/rasim.py <dump dir> search <lo> <hi> [--fpr] [--summary] [name=reg ...] [--iter 3000]

<dump dir> is build/mwccdbg/<fn> (tools/match/mwccdbg.py). `replay` rebuilds the priority list
(sweeps over the variables in vreg order; a variable with at most K remaining neighbours leaves the
graph: K = 28 for GPRs, 31 for FPRs) and the colouring (first free volatile register or an already
used saved one, else a new saved one from r31/f31 down), and checks it against the dump. `search`
permutes the vregs lo..hi (the locals, whose numbers follow their declaration order) and keeps the
order that gives the most target registers: targets as name=r29 / r45=r30, or --summary for EA's
registers from mwccdbg's summary.txt (a majority vote over matching instructions: noisy for temps).
Then write the declarations in that order and confirm with a real build.

Found and verified by lane r6-misc (2026-09-26): the replay reproduced all 122 GPRs of LLDynTex
fn_8010A930 and all 98 FPRs of Particle fn_80094534; the search took fn_8010A930 from 30 to 4
aligned differences where objdiff-score climbs had stalled (then exact). Not modelled: spills and
the second pass after them, and coalescing beyond what the dump already shows."""
import os, random, re, sys


def load(d, fpr):
    c = 'f' if fpr else 'r'
    cls = 'fpr' if fpr else 'gpr'
    allt = open(os.path.join(d, 'regalloc-%s-pass-1-all.txt' % cls)).read()
    asg = open(os.path.join(d, 'regalloc-%s-pass-1-assigned.txt' % cls)).read()
    ents = re.findall(r'^%s(\d+) -> %s(\d+) ?(\S*)\n  flags:[^\n]*\n  cost: [^\n]*\n  neighbors: (\d+)(?: \(([^)]*)\))?'
                      % (c, c), allt, re.M)
    nb, name, reg = {}, {}, {}
    for vr, r, nm, _, ns in ents:
        nb[int(vr)] = set(int(x[1:]) for x in ns.split()) if ns else set()
        name[int(vr)] = nm
        reg[int(vr)] = int(r)
    order = [int(m.group(1)) for m in re.finditer(r'^%s(\d+) -> ' % c, asg, re.M)]
    return nb, name, reg, order


class Alloc:
    def __init__(self, d, fpr):
        self.fpr = fpr
        self.c = 'f' if fpr else 'r'
        self.nb, self.name, self.reg, self.dump_order = load(d, fpr)
        self.live = set(self.dump_order)
        self.k = 31 if fpr else 28
        self.vol = list(range(0, 14)) if fpr else [0] + list(range(3, 13))

    def run(self, sweep):
        """(priority list highest first, {vreg: register}) for this sweep order."""
        removed, pri = set(), []
        while len(removed) < len(sweep):
            prog = False
            for v in sweep:
                if v in removed:
                    continue
                if sum(1 for n in self.nb[v] if n not in self.live or n not in removed) <= self.k:
                    removed.add(v); pri.append(v); prog = True
            if not prog:     # every one has too many neighbours: the dump's spill choice is not modelled
                v = next(v for v in sweep if v not in removed)
                removed.add(v); pri.append(v)
        pri.reverse()
        col, used = {}, []
        for v in pri:
            taken = set(col[n] for n in self.nb[v] if n in col) | set(n for n in self.nb[v] if n < 32)
            # coalesced (phantom) neighbours keep the register they were merged into
            taken |= set(self.reg[n] for n in self.nb[v] if n >= 32 and n not in self.live and self.reg.get(n))
            r = next((x for x in self.vol + sorted(used) if x not in taken), None)
            if r is None:
                r = next(x for x in range(31, 13, -1) if x not in taken and x not in used)
                used.append(r)
            col[v] = r
        return pri, col

    def label(self, v):
        return '%s%d%s' % (self.c, v, (' ' + self.name[v]) if self.name.get(v) else '')


def main():
    a = sys.argv[1:]
    if len(a) < 2:
        sys.exit(__doc__)
    fpr = '--fpr' in a
    opt = lambda k, d: a[a.index(k) + 1] if k in a else d
    al = Alloc(a[0], fpr)
    base = sorted(al.live)
    if a[1] == 'replay':
        pri, col = al.run(base)
        bad = [v for v in pri if col[v] != al.reg[v]]
        print('replay: %d of %d registers differ from the dump; priority order %s' % (
            len(bad), len(pri), 'the same' if pri == al.dump_order else 'differs'))
        for v in pri[:int(opt('-n', '20'))]:
            print('  %-22s -> %s%d%s' % (al.label(v), al.c, col[v],
                                        '' if col[v] == al.reg[v] else '   (dump %s%d)' % (al.c, al.reg[v])))
        return
    if a[1] != 'search':
        sys.exit(__doc__)
    lo, hi = int(a[2]), int(a[3])
    target = {}
    if '--summary' in a:
        for m in re.finditer(r'^\s+%s(\d+)\s+-> %s(\d+)\s+!?EA %s(\d+)' % ((al.c,) * 3),
                             open(os.path.join(a[0], 'summary.txt')).read(), re.M):
            target[int(m.group(1))] = int(m.group(3))
    byname = {v: k for k, v in al.name.items() if v}
    for t in a[4:]:
        if '=' in t and not t.startswith('-'):
            k, r = t.split('=')
            v = int(k[1:]) if re.match(r'[rf]\d+$', k) else byname[k]
            target[v] = int(r.lstrip('rf'))
    if not target:
        sys.exit('no targets: give name=reg pairs or --summary')
    locs = [v for v in base if lo <= v <= hi]
    mk = lambda p: [v for v in base if v < lo] + list(p) + [v for v in base if v > hi]
    cost = lambda p: sum(1 for v, r in target.items() if al.run(mk(p))[1].get(v) != r)
    best, bestp = cost(locs), list(locs)
    print('current order: %d of %d targets wrong' % (best, len(target)))
    rnd = random.Random(int(opt('--seed', '1')))
    for t in range(int(opt('--iter', '3000'))):
        p = list(bestp)
        for _ in range(rnd.randint(1, 3)):
            i, j = rnd.sample(range(len(p)), 2)
            p[i], p[j] = p[j], p[i]
        c = cost(p)
        if c <= best:
            if c < best:
                print('  %d: %d wrong' % (t, c), flush=True)
            best, bestp = c, p
        if best == 0:
            break
    col = al.run(mk(bestp))[1]
    print('best: %d wrong. Declaration (vreg) order, first to last:' % best)
    print('  ' + ', '.join(al.label(v) for v in bestp))
    for v, r in sorted(target.items()):
        if col[v] != r:
            print('  still wrong: %s wants %s%d, gets %s%d' % (al.label(v), al.c, r, al.c, col[v]))


if __name__ == '__main__':
    main()
