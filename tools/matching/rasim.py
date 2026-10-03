"""Replay MWCC's register allocator from a mwccdbg dump, and search declaration orders against it.

    python tools/matching/rasim.py <dump dir> replay [--fpr] [--pass N] [-n 20]
    python tools/matching/rasim.py <dump dir> search <lo> <hi> [--fpr] [--pass N] [--summary] [name=reg ...]
                                [--iter 3000] [--phantoms r45,r119] [--key r97=66.5 ...]

<dump dir> is build/mwccdbg/<fn> (tools/matching/mwccdbg.py). `replay` rebuilds the priority list and
the colouring and checks both against the dump. The model (Chaitin as MWCC does it):
- simplify: sweep over the variables in vreg order; a variable with at most K remaining neighbours
  (K = 28 for GPRs, 31 for FPRs; physical registers and coalesced variables always count, see
  below) leaves the graph and goes on the priority list; repeat the sweep from the first vreg.
- the spill-cost choice: when a whole sweep removes nothing (every variable has more than K
  neighbours: common in big functions, where the long-lived variables form one clique), the
  variable with the lowest cost / remaining-neighbour count leaves the graph (cost is the dump's
  `cost:`; a tie goes to the LAST such variable in sweep order), then sweeping starts again.
- colouring: highest priority first, the first free volatile register or an already used saved
  one, else a new saved one from r31/f31 down; none left = spilled.
- coalesced variables (flag fCoalesced) never leave the graph: they stay a neighbour of everything
  they touched for the whole pass (the MWCC coalescing bug the mwcc-debugger README describes).
The final registers come from the LAST pass: after a spill the allocator reruns (pass 2) on a new
graph, usually without the coalesced neighbours of pass 1. --pass defaults to the last pass in
the dump. Note mwccdbg's summary.txt maps EA's registers through the final object, so its "EA"
column is only meaningful in the last pass's section.

`search` permutes the vregs lo..hi (the locals, whose numbers follow their declaration order:
the first declared local gets the highest number; frontend temps @N sit above the locals, the
highest @ number lowest; backend temps above those in code order) and keeps the order that gives
the most target registers: targets as name=r29 / r45=r30, or --summary for EA's registers from
mwccdbg's summary.txt (a majority vote over matching instructions: noisy for temps). What-ifs for
shapes the C does not produce yet: --key r97=66.5 gives vreg 97 a different sweep position,
--phantoms r45,r119 adds pass 1's coalesced variables as permanent neighbours to this pass.
Then write the declarations in that order and confirm with a real build.

Found and verified by lane r6-misc (2026-09-26): the replay reproduced all 122 GPRs of LLDynTex
fn_8010A930 and all 98 FPRs of Particle fn_80094534; the search took fn_8010A930 from 30 to 4
aligned differences where objdiff-score climbs had stalled (then exact). Lane r7-golfer
(2026-09-26) added the spill-cost choice, the tie rule and the pass selection: Golfer
AI_ChooseTarget, both GPR passes (169 and 176 variables, five spill-cost choices in pass 1, one in
pass 2) replay with 0 differences in order and registers, as do all 80 GPR and 34 FPR
allocations of the round-5 batch dumps and pass 1 of its six spilling functions."""
import os, random, re, sys


def passes(d, cls):
    return sorted(int(m.group(1)) for f in os.listdir(d)
                  for m in [re.match(r'regalloc-%s-pass-(\d+)-all\.txt$' % cls, f)] if m)


def load(d, fpr, pas=None):
    c = 'f' if fpr else 'r'
    cls = 'fpr' if fpr else 'gpr'
    if not passes(d, cls):
        sys.exit('no %s allocation in %s (the function has no %s variables)' % (cls, d, cls))
    pas = pas or passes(d, cls)[-1]
    allt = open(os.path.join(d, 'regalloc-%s-pass-%d-all.txt' % (cls, pas))).read()
    asg = open(os.path.join(d, 'regalloc-%s-pass-%d-assigned.txt' % (cls, pas))).read()
    ents = re.findall(r'^%s(\d+) -> (?:%s(\d+)|none) ?(\S*)\n  flags:([^\n]*)\n  cost: (\d+)\n'
                      r'  neighbors: (\d+)(?: \(([^)]*)\))?' % (c, c), allt, re.M)
    nb, name, reg, cost, flags = {}, {}, {}, {}, {}
    for vr, r, nm, fl, co, _, ns in ents:
        v = int(vr)
        nb[v] = set(int(x[1:]) for x in ns.split()) if ns else set()
        name[v] = nm
        reg[v] = int(r) if r else None
        cost[v] = int(co)
        flags[v] = fl.split()
    order = [int(m.group(1)) for m in re.finditer(r'^%s(\d+) -> ' % c, asg, re.M)]
    return nb, name, reg, cost, flags, order, pas


class Alloc:
    def __init__(self, d, fpr, pas=None):
        self.fpr = fpr
        self.c = 'f' if fpr else 'r'
        self.nb, self.name, self.reg, self.cost, self.flags, order, self.pas = load(d, fpr, pas)
        self.live = set(v for v in order if 'fCoalesced' not in self.flags.get(v, []))
        self.dump_order = [v for v in order if v in self.live]
        self.k = 31 if fpr else 28
        self.vol = list(range(0, 14)) if fpr else [0] + list(range(3, 13))

    def add_phantoms(self, d, vregs):
        """What-if: pass 1's coalesced variables as permanent neighbours of this pass's graph."""
        nb1 = load(d, self.fpr, 1)[0]
        for ph in vregs:
            self.live.discard(ph)
            self.nb[ph] = set(nb1[ph])
            for n in nb1[ph]:
                if n in self.live:
                    self.nb[n].add(ph)

    def run(self, sweep):
        """(priority list highest first, {vreg: register or None}) for this sweep order."""
        removed, pri = set(), []
        deg = lambda v: sum(1 for n in self.nb[v] if n not in self.live or n not in removed)
        while len(removed) < len(sweep):
            prog = False
            for v in sweep:
                if v not in removed and deg(v) <= self.k:
                    removed.add(v); pri.append(v); prog = True
            if not prog:     # the spill-cost choice: lowest cost / neighbours, a tie goes to the last
                best, bc = None, None
                for v in sweep:
                    if v not in removed:
                        c = self.cost[v] / max(deg(v), 1)
                        if bc is None or c <= bc:
                            best, bc = v, c
                removed.add(best); pri.append(best)
        pri.reverse()
        col, used = {}, []
        for v in pri:
            taken = set(col[n] for n in self.nb[v] if n in col) | set(n for n in self.nb[v] if n < 32)
            # coalesced (phantom) neighbours keep the register they were merged into
            taken |= set(self.reg[n] for n in self.nb[v] if n >= 32 and n not in self.live and self.reg.get(n))
            r = next((x for x in self.vol + sorted(used) if x not in taken), None)
            if r is None:
                r = next((x for x in range(31, 13, -1) if x not in taken and x not in used), None)
                if r is not None:
                    used.append(r)
            col[v] = r
        return pri, col

    def label(self, v):
        return '%s%d%s' % (self.c, v, (' ' + self.name[v]) if self.name.get(v) else '')

    def reg_s(self, r):
        return '%s%d' % (self.c, r) if r is not None else 'none'


def main():
    a = sys.argv[1:]
    if len(a) < 2:
        sys.exit(__doc__)
    fpr = '--fpr' in a
    opt = lambda k, d: a[a.index(k) + 1] if k in a else d
    al = Alloc(a[0], fpr, int(opt('--pass', '0')) or None)
    if '--phantoms' in a:
        al.add_phantoms(a[0], [int(x.lstrip('rf')) for x in opt('--phantoms', '').split(',') if x])
    key = {}
    for i, t in enumerate(a):
        if a[i - 1] == '--key' and '=' in t:
            k, x = t.split('=')
            key[int(k.lstrip('rf'))] = float(x)
    base = sorted(al.live, key=lambda v: key.get(v, v))
    if a[1] == 'replay':
        pri, col = al.run(base)
        bad = [v for v in pri if col[v] != al.reg[v]]
        print('pass %d replay: %d of %d registers differ from the dump; priority order %s' % (
            al.pas, len(bad), len(pri), 'the same' if pri == al.dump_order else 'differs'))
        for v in pri[:int(opt('-n', '20'))]:
            print('  %-22s -> %s%s' % (al.label(v), al.reg_s(col[v]),
                                       '' if col[v] == al.reg[v] else '   (dump %s)' % al.reg_s(al.reg[v])))
        return
    if a[1] != 'search':
        sys.exit(__doc__)
    lo, hi = int(a[2]), int(a[3])
    target = {}
    if '--summary' in a:
        txt = open(os.path.join(a[0], 'summary.txt')).read()
        sec = re.split(r'^== ', txt, flags=re.M)
        sec = [s for s in sec if s.startswith('regalloc-%s-pass-%d-' % ('fpr' if fpr else 'gpr', al.pas))]
        for m in re.finditer(r'^\s+%s(\d+)\s+-> %s(\d+)\s+!?EA %s(\d+)' % ((al.c,) * 3), sec[0] if sec else '', re.M):
            target[int(m.group(1))] = int(m.group(3))
    byname = {v: k for k, v in al.name.items() if v}
    for t in a[4:]:
        if '=' in t and not t.startswith('-') and a[a.index(t) - 1] != '--key':
            k, r = t.split('=')
            v = int(k[1:]) if re.match(r'[rf]\d+$', k) else byname[k]
            target[v] = int(r.lstrip('rf'))
    if not target:
        sys.exit('no targets: give name=reg pairs or --summary')
    locs = [v for v in base if lo <= v <= hi]
    rest = [v for v in base if not lo <= v <= hi]

    slots = sorted(locs)

    def mk(p):
        """Sweep order with the locals in order p (p[0] takes the lowest slot)."""
        pos = dict(zip(p, slots))
        return sorted(rest + list(p), key=lambda v: pos[v] if v in pos else key.get(v, v))
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
    print('best: %d wrong. Declaration (vreg) order, lowest vreg (last declared) first:' % best)
    print('  ' + ', '.join(al.label(v) for v in bestp))
    for v, r in sorted(target.items()):
        if col[v] != r:
            print('  still wrong: %s wants %s%d, gets %s' % (al.label(v), al.c, r, al.reg_s(col[v])))


if __name__ == '__main__':
    main()
