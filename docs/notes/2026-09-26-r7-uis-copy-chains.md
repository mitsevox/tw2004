# How MWCC's backend removes copies, and what EA's kept copies need (r7-uis, 2026-09-26)

Follow-up to `2026-09-26-uis-library-flags.md` (the UIS "kept copy" pattern: UISScreen
fn_8016ABBC, fn_8016AD54, fn_8016AEEC, fn_8016A2D4, fn_8016B4D4; UIStudio fn_80168918). Measured
with mwccdbg on small test files and on base.c variants (scripts in the lane scratch folder
`/home/user/scratch/tw/agents/r7-uis/`: toy*.c, toyrun.sh, t_*.py). No source form was found; what
follows is the mechanism, so the next lane does not repeat the dead ends.

## 1. Each copy-propagation pass removes one link of a copy chain [verified]

The pass list (mwcc-debugger's GC/2.6 table, `mwcc_debugger.py` lines 269-292) has up to eight
copy-propagation passes; a dump appears only when the pass before it changed something, so the
dumped list differs per function (fn_8016ABBC: 03, 05, 08, 12; fn_8016A2D4: 03, 05, 08).
A copy `mr rD,rS` is NOT propagated in a pass while rD is itself the source of another copy.
Copies are visited in code order, so a chain `rA <- rB <- rC <- rD` loses only its LAST link per
pass. Test (toy4.c t22, a value hoisted out of three nested loops: the frontend makes one copy per
loop level): backend-00 `mr r39,r50; mr r41,r39; mr r44,r41`; pass 02 deletes r44, pass 05 r41,
pass 08 r39. With four levels (toy5.c t24) one link reaches register allocation.

## 2. Surviving copy-propagation is not enough: the coalescer merges non-interfering copies [verified]

In t24 the surviving `mr r40,r55` is still gone from the final code: register allocation coalesces
a copy whose two registers do not interfere. EA's kept copies are all copies whose two sides DO
interfere at that point, or whose source was first coalesced with a physical register:
fn_8016ABBC's p is coalesced with the incoming r7 (p dies at the case copies), and then pNode
(live across calls) cannot join r7. In our build even a pNode copy that survives (under
`opt_propagation off`, r5-uisscreen) is merged with p first, so p lives in a saved register.

## 3. Copies that are never propagated [verified]

- A copy into a physical register (a call argument, a return value): its source copy stays too.
  toy.c t2: `N* q = p;` plus `g(a, 1, q)` after the loop gives EA's exact case shape
  (`mr r29,r5; lwz r0,0(r5)`, p never saved). EA's pNode is not passed anywhere, so this is not
  EA's form (also noted by r4-uisscreen on fn_8016AD54).
- A copy whose destination has another definition (a phi): the IV copies CSE makes from two
  `li 0` (`li r29,0; mr r31,r29`, fn_8016ABBC case 7) survive pass 12 in our build as in EA.
- A copy from a physical register that is redefined later (argument registers are rewritten
  by every call's argument setup).

## 4. Copies that appear late [verified]

- Constant propagation (pass 06) rewrites a mask it proves redundant into `mr` (fn_8016ABBC
  `rlwinm r56,r45,0,16,31` -> `mr r56,r45`; fn_8016AEEC's hoisted (short) conversion of bLast
  `rlwinm r50,r56,0,24,31` -> `mr r50,r56`). The copy-propagation after load deletion (08)
  deletes it; EA's `mr r30,r27` in fn_8016AEEC is exactly this copy kept.
- Load deletion (07) turns a reload of a stack variable into a copy from the register that was
  stored. The permuter (15 min, fn_8016ABBC) found `void** q = &p;` reading p through q in one case:
  p goes to the stack (`stw r7,0xc(r1)`, not in EA) and the deleted loads leave `mr r30,r7;
  lwz r3,0(r7)`, EA's exact case shape (a physical source redefined by the loop's calls, so never
  propagated). Wrong because of the store: proof that EA's case copies come straight from r7.

## 5. What this means for EA's source

For each kept copy, EA's IR at the last copy-propagation pass still had the copy's destination
feeding another copy, or a second definition, and its source was coalesced with the parameter
register before the destination could merge with it. The ?: form (`(nKind == 8) ? p : p`,
r6-uis: 21 aligned) gives the second definition at the cost of a branch. Not reproduced without
extra code; tried this round, all no better (quicktrial aligned, base 16 unless noted):
- p reused as the child pointer in the loops (`p = pNode->ppGroups[i]; fn(.., p, 0)`, in the
  argument `p = ...`, either or both cases): the frontend deletes or folds the store (16);
  with `#pragma opt_dead_assignments off` pNode's copy is kept in case 8 but p then takes a saved
  register (19);
- inline helpers for the pInfo test taking pNode / pInfo / void*: 26-62; inline identity
  accessors (`static inline UISNode* ..(void* p) { UISNode* q = p; return q; }`, nested up to 4
  deep): the frontend flattens them to one extra link, removed by pass 05 (16);
- case bodies as static helpers taking `s32* pn`: the recursion inlines into them (134);
- a latch re-definition `pNode = p` in the loop (17, extra mr), `for (pNode = p, i = 0; ...)` (16);
- no-op self assignments (`| 0`, `& ~0`, `<< 0`, `* 1`, `- 0`, `^ 0`): folded by the frontend (16);
- function-level pNode/pGroup reused for the recursive arguments in the other case: the frontend
  still folds the single-use assignment (16).
- fn_8016AD54: function return type x nFound type (s32/int/long/u32/short x s32/int/u32/short/u16):
  38 at best (base).
- fn_8016B4D4 (with fn_8016C6C4's definition in base.c so it inlines, base 12): the nScreens read
  swapped after the call, twice, inside the if condition (`nIndex < (nScreens = ...)`), the field
  in the if: 12-13. EA's `lwz r8,0x34(r3)` is the inlined loop's bound; `mr r28,r8` after the loop
  is nScreens taken from it by CSE and kept (section 1/2); ours deletes it in pass 02.
