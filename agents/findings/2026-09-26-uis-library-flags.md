# UIS library flags and the kept-copy pattern (r6-uis, 2026-09-26)

The UIS units (UISEvent, UIStudio, UISApi, UISScreen) are EA Tiburon's IStudio UI library
(`Code/Tiburon/IStudio/` in TW2005's strings), so they were probably built with ONE library-wide flag
set. Measured here: which flag dimensions move these four units at all.

## Method

`/home/user/scratch/tw/agents/r6-uis/libflags.py` (scratch, not in the repo): compiles the four
sources with the base cflags from build.ninja plus one extra flag set, then runs `objdiff-cli report
generate` on a temporary objdiff.json holding only these four units (the same numbers as
build/GW4E69/report.json; `objdiff-cli diff` alone scores differently, e.g. fn_80165D90 99.55 there,
100 in the report). About 2 s per flag set. Baseline (the per-file flags in configure.py): 33 exact
functions of 60, size-weighted fuzzy 96.8146.

## Result 1: one library-wide set costs nothing

`-pragma "pool_data on" -inline auto,deferred -str reuse,readonly` (UISEvent's current set) on all
four units: 33 exact, 96.8146, every function's score identical to the per-file flags. Confirmed in
the real build (configure.py edited, report.json per unit: UISEvent 97.83 / UIStudio 97.99 / UISApi
95.86 / UISScreen 95.68 fuzzy, data 100% each, all unchanged; main.dol OK), then reverted: a tie, not a
win, so configure.py is left for the orchestrator. The deferred emit order does not matter for
UISApi/UIStudio (address-order sources give byte-identical code).

## Result 2: no flag, pragma or compiler moves the kept copies

Each set below is the library set plus one change (exact / weighted, functions lost):

| change | exact | weighted | notes |
|---|---|---|---|
| -O4 / -opt level=4,speed | 33 | 96.8146 | same |
| -O3,p / -O3 / `#pragma optimization_level 3` | 31 | 96.6788 | loses fn_80165C74, fn_80168FC8; only fn_80165670 better (96.95 -> 98.05) |
| -O4,s | 14 | 85.54 | |
| -O2 | 16 | 78.64 | |
| -opt nopeephole | 11 | 89.37 | fn_8016C614 90.21 -> 95.62 |
| -opt noschedule | 10 | 71.91 | |
| -opt nolifetimes | 32 | 95.86 | loses fn_80168FC8 |
| -opt nopropagation (or pragma opt_propagation off) | 20 | 95.20 | |
| -opt nocse (or pragma opt_common_subs off) | 28 | 95.08 | fn_8016C614 +1.5, fn_8016B844 +0.4 |
| -opt nostrength | 17 | 92.78 | |
| -opt noloop / pragma opt_loop_invariants off | 27 | 95.19 | |
| -opt nodeadstore / pragma opt_dead_assignments off | 33 | 96.78 | |
| pragma opt_dead_code off, opt_findoptimalunrollfactor off, opt_classresults off, defer_codegen on | 33 | 96.8146 | same |
| pragma opt_unroll_loops off / ppc_unroll_speculative off | 32 | 94.85 / 93.82 | loses fn_8016BEDC |
| pragma global_optimizer off | 16 | 86.29 | |
| pragma optimize_for_size on | 14 | 85.54 | |
| -inline auto / all,deferred / auto,deferred,level=8 | 33 | 96.81 | same |
| -inline smart / deferred (no auto) | 29 | 90.85 | |
| no pool_data | 33 | 96.62 | |
| -str reuse / reuse,pool / reuse,pool,readonly | 33 | 96.81 | |
| -str noreuse | 31 | 96.45 | |
| -fp_contract off | 33 | 96.80 | |
| -enum min, -proc 750, -char signed, -sym on, -lang c++ | 33 | 96.8146 | same (`-lang c++` after `-lang=c` has no effect) |
| -char unsigned | 33 | 96.37 | |
| -use_lmw_stmw off | 26 | 95.22 | |
| GC 1.3.2 / 2.0 / 2.6 / 2.7 | 33 | 96.8146 | identical |
| GC 2.0p1 | 30 | 96.20 | |
| GC 3.0a3 | 10 | 71.57 | |
| GC 1.3 | 26 | 92.60 | |
| GC 1.0 / 1.1 / 1.2.5 | 11 | 70.10-70.13 | |

## What the kept copies are (mwcc-debugger)

- EA's `mr r29,r7; lwz r3,0x0(r7)` (UISScreen fn_8016ABBC/fn_8016AD54) is the post-regalloc
  peephole ("after-peephole") rewriting the first load after a KEPT copy to the copy's source
  (verified on Ball Physics_StopBall, exact). So EA's IR kept `mr pNode,p` through every
  copy-propagation pass. Ours deletes it in the first copy-prop pass.
- The first copy-prop pass handles virtual-to-virtual copies only; copies from a physical register
  (incoming parameters, call results) are propagated by the later passes (the one after
  loop-transforms, the O4-only one after load-deletion). EA keeps several of those too:
  fn_8016AD54's `mr r0,r3; cmpwi r0,-1`, fn_80168F5C's `mr r0,r5`, fn_80165670's `mr r25,r4`,
  fn_80169858's `mr r0,r3; mr r25,r0`, fn_8016C6C4/fn_8016C614's late parameter mask.
- A copy survives only when its destination has a second definition reaching the same uses (a
  phi): `pNode = p ? p : 0` keeps pNode in its own register (mechanism only, it changes the code);
  EASBStorage fn_8012BA58 (exact) keeps `mr r6,r3` exactly this way (eError assigned on two paths).
- fn_80165670 is the one function where O3 helps: loop code motion hoists the inline's `mr
  r72,uA; mr r74,uB` into the preheader and only O4 has a copy-prop pass after load-deletion to
  fold them; at O3 the whole UISEvent_NoneWaiting loop matches EA.

- A second definition is enough to rebuild EA's picture in fn_8016ABBC: `UISNode* pNode = (nKind ==
  8) ? p : p;` and the same for pGroup leave p in r7 with no entry copy (as EA) and pNode/pGroup in
  their own saved registers; only the ?:'s own branch and a loop-register rotation remain (21).
  EA's asm has no such branch, so its second definition (if that is what it is) costs no code.
- Near-misses that read as the same family (a parameter or call-result copy EA keeps and we
  propagate): fn_80165B90 (EA extends nB before nA: nA's entry copy kept would delay its extsh),
  fn_80168F5C / fn_801694A0 (uScreen's `mr r0,r5` / `mr r8,r5`), fn_8016C6C4 / fn_8016C614 (the
  last parameter's mask after `li i`), fn_80165670 (`mr r25,r4`), fn_80169858 (`mr r0,r3; mr
  r25,r0`), fn_8016AD54 (`mr r0,r3; cmpwi r0,-1`), fn_8016ABBC / fn_8016AD54 / fn_8016B4D4 /
  fn_8016AEEC / fn_8016A2D4 (the per-case copies). `-pragma "register_coloring off"` and unknown
  `-opt` keywords are accepted and change nothing; `-opt [no]prop` is the only switch and it
  covers constant AND copy propagation (EA's code clearly has constant propagation).

## Source forms tried for the copy (all no change, quicktrial aligned)

fn_8016ABBC (16): case body in `do { ... } while (0)`, in `if (1) { }`, a block with the copy
declared then assigned; a union `{ void* p; UISNode* pNode; UISGroup* pGroup; }` local; a one-member
struct; a 1-element array; `void** ppv = &p`; `(u8*)p + 0`; `&((UISNode*)p)[0]`; K&R definition;
`register` p; the body in a static inline worker called by a thin wrapper (the recursion inlines
into it: 45-53). fn_8016AD54 (38): the result compare through `if ((n = f()) != -1)`, a
block-local, a second local, reversed compare, casts.

## New lever (verified): a dead value that lives until register allocation

UISEvent fn_80165B90 99.64 -> 100 (commit "fn_80165B90 exact"): passing `(s64)nArgs` to the
inline's s32 parameter leaves the high word's `srawi rX,rN,31` in the code; no pass before
register allocation deletes it (the post-regalloc peephole does), so it occupies a register during
allocation, the nA/nB extsh temps get different registers and the final scheduler emits them in
EA's order. Found by the permuter (15 variants, all a 64-bit round trip on nType or nArgs).
Logic unchanged (s32 -> s64 -> s32), labelled fake match. A sweep wrapping every rvalue
identifier in `(s64)` found nothing in ten other UIS/uiProcessInterface near-misses.

## Not found

What in EA's source kept these copies. Everything points at a second definition (a phi) of the
copy's destination in EA's code, or at copy-propagation seeing a physical source; no spelling of
the current C produces either without changing the code.
