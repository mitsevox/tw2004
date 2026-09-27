# fn_8016ABBC (UISScreen.c, 0x8016ABBC)

Status: SOLVED 2026-09-27 (lane b12), 96.23 -> 100, labelled fake match: fn_8016AD54's recipe
(pLoop8/pPrev8 copies carried round case 8's loop, pLoop7 round case 7's, one shared nCount for
nGroups/nEntries) plus the declaration order pGroup, pNode, i, nCount, pEntry, pPrev8, pLoop8,
pLoop7 (rasim.py search). No OR lever needed. Commit: see `git log -- agents/tried/fn_8016ABBC.md`.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

- 2026-09-27 b12 (quicktrial aligned, base 16; scripts /home/user/scratch/tw/agents/b12/o1.py,
  qt.py, mk.py): OR lever (`s32 q = (s32)p; pNode = (UISNode*)(q | (u32)p)`, same for pGroup):
  `void* q`/`T* q` copies fold (16); integer q of another type survives as `or r30,r7,r7` (15, EA's
  shape, registers rotated). With function-level decls + one shared nCount + first read through p,
  rasim order (pGroup, pNode, i, nCount, pEntry): 4, all registers right, only `lwz r3,0(r7)` is
  scheduled before the `or` in both cases (pre-RA pass: the load is independent and on the
  critical path). Load read through pNode: 2 (`lwz r3,0(r29)`: the post-RA peephole rewrites the
  first load after a kept copy only for a real mr, not for an or). `#pragma scheduling once/off`:
  19-34. Volatile first pInfo read: 8. Dead srawi deadsearch (sched750 --two) on the block: none.
  Double 64-bit shift chain as pNode's init keeps a real `mr r28,r7` (17) but its dead srawis sit in
  the block and make the final pass reschedule it (lwz first). Observation: block flag 0x8 in the
  dumps looks like "scheduled, unchanged": the post-RA pass reschedules only blocks changed after
  allocation (srawi deletion, coalesced copies), and the peephole's load rewrite alone seems not to
  count; EA's `mr; lwz 0(r7)` order needs a real mr with nothing else deleted from the block.
  Shift chain on p at the function top: 42-101. Then fn_8016AD54's closing recipe (pLoop8/pPrev8,
  pLoop7, shared nCount): 6 (case 7 exact, case 8 pNode/i swapped); rasim.py search 38..45 ->
  order pGroup, pNode, i, nCount, pEntry, pPrev8, pLoop8, pLoop7: 0. Exact in the build (report 100).

- 2026-09-26 r6-uis (quicktrial aligned, base 16), mwcc-debugger: EA's `mr r29,r7; lwz r3,0x0(r7)`
  is NOT a partial copy propagation: it is the post-regalloc peephole (backend "after-peephole")
  rewriting the first load after a kept copy to the copy's source. Verified on Ball
  Physics_StopBall (exact): backend-09 `lbz r34,r32,0x98` after `mr r32,r3`, after-peephole
  `lbz r0,0x98(r3)`. So EA's IR kept `mr pNode,p` through every copy-propagation pass AND
  pNode was not coalesced with p (p colours r7). Ours: backend-03 (first copy-prop) deletes it.
  Only a second definition of pNode that reaches the same uses keeps the copy
  (`pNode = p ? p : 0` gives pNode its own register, r30: mechanism only, changes code). Tried,
  all 16: p reassigned as the call argument in either/both cases (the front end folds it away),
  one function-level pNode/void* q assigned in both cases (the back end splits them into two
  vregs, still propagated), pNode through a one-member struct / a 1-element array / `void** ppv
  = &p` / `(u8*)p + 0` / `&((UISNode*)p)[0]` / `(u8*)p - (u8*)0`; `*(UISNode**)&p` 14 (p goes to
  the stack, wrong). `#pragma opt_propagation off` + first read through p: 17 (the back end's
  copy-prop passes still run: the pragma does not reach them). Case bodies as static inline
  helpers (inlined): the inlined parameter copies are propagated the same way (worse, 60+).
  Unit/function flags on base.c: -opt nopropagation 17 (the copy survives but is coalesced and
  the IV inits turn into `slwi`), nocse 17, noloop 16, nopeephole 24, nostrength 29;
  optimization_level 3 16, 2 38, optimize_for_size 29; every GC compiler x {-O4,p; -O3,p;
  -O4,s}: 16 at best. The same kept-copy family appears across UIS (see fn_80165670: some of it
  is O3-shaped).
- 2026-09-26 r6-uis, later (verified mechanism, not a fix): a SECOND definition of pNode/pGroup
  reaching their uses reproduces EA's whole register picture: `UISNode* pNode = (nKind == 8) ? p :
  p;` and `UISGroup* pGroup = (nKind == 7) ? p : p;` give p no entry copy at all (the NULL test
  and the case copies read r7, as EA) and pNode/pGroup their own saved registers; 21 aligned
  only because the ?: leaves its branch (`bne; mr r30,r7; b; mr r30,r7`) and the loop registers
  rotate. One side only: 19-20. So EA's pNode/pGroup had a second definition that costs no code;
  its source form is not found. Also no change (16): the case bodies in `do { } while (0)` /
  `if (1) { }` / a declare-then-assign block, a local union {void*, UISNode*, UISGroup*}, K&R
  definition, the body as a static inline worker behind a wrapper (45-53: the recursion inlines).
  Library-wide flag sweep: agents/findings/2026-09-26-uis-library-flags.md.

- 2026-09-25 n-uisscreen: (scores are aligned diff counts from the WHOLE unit compiled: base.c from perm_setup drops the auto-inlined callees, e.g. fn_8016C6C4 inside fn_8016B4D4). EA keeps p in r7 and copies it per case (not coalesced, same as fn_8016AD54, fn_8016AEEC's bLast, fn_8016B4D4's nScreens, fn_8016A2D4's uEvent: one cause suspected). Tried: explicit casts, `void* const p`, split declaration/assignment, case 8 body as a static inline helper (118: the recursion inlines), if/else chain (22), nKind int: all 16 or worse. GC 1.3.2-2.7: identical; 1.2.5n/3.0: worse; -O4/-O3/-O2/-O4,s: worse; -opt nocse/nopeephole/nopropagation: worse.
- 2026-09-25 n-uisscreen, more: p typed u32/s32/int/unsigned int/char*/u8*/UISNode* (header, casts at the calls and in the cases): all 16.
- 2026-09-26 r2-uisscreen (whole-unit aligned, base 16): identity inline on the p copies (node, group, both) 16; the first pInfo read through p (EA's `lwz r3,0(r7)` after `mr r29,r7`), once or in both reads, 16; function-level pNode/pGroup assigned in both cases (pGroup = ppGroups[i] in case 8, pNode = &pNodes[..] in case 7), all 6 declaration orders, 16; `register` / `const` pNode and pGroup, `register void* p` 16; each kind as a static inline helper with fn_8016ABBC under `#pragma auto_inline off` 31 (135 without the pragma). See fn_8016AD54 for the shared copy problem.
- 2026-09-26 r5-uisscreen, mwcc-debugger: `r35 -> r28 !EA r7 41 nb p`: EA never moves p out of r7 (its
  NULL test reads r7) and copies it per case (`mr r29,r7` / `mr r28,r7`) with the case's first load on
  r7; ours keeps p in r28 from the entry and folds the case copies. Same kept-copy shape as
  fn_8016AD54 (see there); no new C tried.
- 2026-09-26 r7-uis (quicktrial aligned, base 16; mechanism in agents/findings/2026-09-26-r7-uis-copy-chains.md): each backend
  copy-propagation pass deletes only the last link of a copy chain, and the register allocator
  then coalesces any surviving copy whose two sides do not interfere, so EA's case copies need a
  second definition of pNode/pGroup (or a feed into a never-propagated copy) all the way to
  register allocation. Tried, no gain: p reused as the loop's child pointer (`p = pNode->ppGroups[i];
  fn(.., p, 0)` / `p = ...` inside the argument, case 8, case 7, both): 16, the frontend folds or
  deletes the store; with `#pragma opt_dead_assignments off` 19 (pNode's copy kept in case 8, p
  then saved); the pInfo test as an inline taking pNode/pGroup, pInfo or void*: 26 / 62 (case 8
  only); an inline `ppGroups[i]` accessor 16; identity accessors `UISNode* q = p; return q;`
  (one to four nested levels: the frontend leaves one extra link, gone by pass 05) 16; case
  bodies as static helpers taking `s32* pn` (the recursion inlines) 134; `pNode = p` again in the
  loop latch 17 (an mr in the loop); `for (pNode = p, i = 0; ...)` 16; no-op self assignments
  (`|0`, `&0xFFFFFFFF`, `<<0`, `*1`, `-0`, `^0`) folded 16; function-level pNode/pGroup also used
  for the other case's recursive argument 16 (frontend folds). Permuter 15 min -j 2 (base 390):
  390 -> 185 with `void** q = &p;` read in case 7 (p goes to the stack: `stw r7,0xc(r1)` not in
  EA; load deletion then gives EA's exact `mr r30,r7; lwz r3,0(r7)`: proof the copies come
  straight from r7), 255 (10 aligned) only with `new_var = p;` placed inside the switch before the
  first case label, i.e. unreachable, so pNode reads an uninitialised variable: rejected.
- 2026-09-26 e-uisscreen (endgame; aligned counts on the WHOLE preprocessed unit, a scratch perm dir, same as the build): base 16. Second definitions that cost nothing, all folded (16): a
  constant condition (`s32/u8/int k = 1` or 0; `k ? p : p`, if/else), tautologies (`nKind ==
  nKind`, `p == p`, `n - n`, `bAll & 0`, `(u32)nKind < 0`, ...) ? p : p; `(p != 0) ? p : p` 23.
  Address-taken locals (&pNode, 1-element array, void* q + &q) 16. Per-function pragmas and
  scheduling models 16. NEW (the lever that closed fn_8016B4D4): a second local per case assigned
  from the case pointer before the loop and again right after its use in the loop, e.g. case 8
  `pLoop = pNode; for (...) { pG = pLoop->ppGroups[i]; pLoop = pNode; fn_8016ABBC(.., pG, 0); }`
  and case 7 `pLoop = pGroup; ... pEntry = &pLoop->pEntries[i]; pLoop = pGroup;`: EA's case copies
  from r7 come back (`mr r29,r7` / `mr r28,r7`, p stays in r7, no entry copy), aligned 10; with
  function-level declarations in the order rasim.py suggests (nEntries, pLoop8, i, nGroups, pNode,
  pEntry, pLoop7, pGroup) and a separate counter k for case 7 (declared second): 8. Left: case 8's
  ppGroups base goes to r4 with `li r6,7` before the lwzx (EA r6, the lwzx before `li r6,7`: the pG
  temp moves the load out of the argument list), and case 7's offset starts `li r31,0` (EA `mr
  r31,r29`, the CSE of the shared i's `li 0`, lost with the separate k). The reassignment after the
  call instead keeps EA's argument order but puts the second local in r3 (`mr r3,r31` twice): 11.
  Comma forms inside the arguments, `(pLoop = pNode)->`, for-increment: 13-16. The separate
  counter on case 8 instead (random order search, 150 orders) 8; pEntry loop-local with the shared i
  (sampled permutations of 7 declarations) 10: `mr r31,r29` is back but case 7's i/nEntries swap
  (r30/r29). The reassignment placed inside the call's argument list (comma in argument 1, 3, 5's
  index or 6): EA's exact argument order and case-8 registers, but the case copy is propagated
  again (14); through a UISGroup** temp (address or base, statement or comma) 8.
  Permuter 20 min from the 8 base: nothing better in aligned terms. Scratch:
  /home/user/scratch/tw/agents/e-uisscreen/t_abbc6..17.py, v_abbc_k.c (the 8 source).

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
UISScreen fn_8016ABBC
base 16, 6 levers, 23 variants (6 singles) in 2 s; best 16

16 [safe]
  - for loop #1 (`for (i = 0; i < nGroups; i++) {`) as a while loop

16 [safe]
  - for loop #2 (`for (i = 0; i < nEntries; i++) {`) as a while loop

16 [safe]
  - move `u32 i;` to line 1 of the declarations

16 [safe]
  - type of i: u32 -> unsigned int
```
- 2026-09-26 r4-uisscreen: the round 3 copy rules tested on fn_8016AD54 (same shape, see there):
  integer-typed copies of p, every p type x cast chain, p reassigned as the call argument, all
  merge; not repeated here.

## Collected from the notes and docs (2026-09-25)

### agents/notes/map-03-notes_w6.txt

```
- UISScreen fn_8016ABBC (orig keeps p in r7 and copies it per case: mr r29/r28,r7): pNode/pGroup at
  function top (4 orders), casts, split null tests: no.
```
- 2026-09-26 round 3 (r3-uisscreen; written by the orchestrator from the lane report, the disk was full): void* copies in the case ((UISNode*)(void*)p, also via u8*/char*/const void*), with the first pInfo read through p, copies at the function top, the parameter typed UISNode* in the header plus void* copies; per-function pragmas opt_lifetimes/opt_dead_assignments/opt_loop_invariants/peephole/opt_findoptimalunrollfactor off and optimization_level 3: all 16 (base). A case-helper inline 26. Observed in standalone tests: a copy q = p keeps its own register when q is passed to a call or the copy is long/unsigned; int->int and pointer->pointer copies merge; a copy only dereferenced in a loop merges.
