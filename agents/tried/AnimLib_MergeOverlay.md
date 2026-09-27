# AnimLib_MergeOverlay (skalib.c, 0x80024B18)

Status: SOLVED 2026-09-27 (b6): 100%. EA form for the loops (i is also the overlay search
counter; `lbl_801C6008[k].n04 += nCopied` with no pUsed local; one `Clip* pHdr` walk), plus
labelled fakes: the u32 round-up scratch uAl, u32 strides with a signed halving, `nRet +=
0x2800`, and a register note for the block-level n50Al. Commit on agent/b6 (see below).

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

- 2026-09-27 b6 (continued). 96.26 -> 97.36 -> 100.
  * nRet: `nRet += 0x2800;` (nRet is still 0 there). With `=`, the first scheduling pass issues
    the li at cycle 1 (it wins the tie with the Mem_cpy size's `li r5` by source order), the li's
    spill temp overlaps nClips and the slwi takes r0; the frontend keeps `nRet = nRet + 0x2800`
    (it does not fold nRet's 0 after the loop), the backend addi is scheduled late and then
    constant-propagated into EA's `li r0,0x2800` after the slwi. 97.36.
  * preheader: EA's offset IV is a copy of i (`lwz r31,0x38(r1)`) because EA's record loop is a
    LATER web of i: EA uses i as the overlay search counter too (no j). The frontend keeps a
    variable's name for its first web only; the record loop's web becomes a frontend temp, and
    CSE then gives the frontend IV (`@IV = 0`) a copy of that temp's `li 0`. And EA has no pUsed
    local: `lbl_801C6008[k].n04 += nCopied;` (the hoisted address is a temp too, so its spill
    slot 0x3c comes after i's). Exact.
  Tried before finding it (no change, all `li r31,0`): i = 0 as a statement anywhere before the
  loop, at the top, at the join of both branches (mulli from i's reload), `i = nRet`, `k - k`,
  `k ^ k`, `k & 0`, `(s64)0`, 0u/0L, u32/s16/s32 i, comma for-inits, while/do forms, an explicit
  byte-offset local (`o = i`), (u64) identities before the loop (the OR form moves pEnd's load:
  aligned 37 -> 18, not the IV), scheduling pragmas (7400: 32; once/603/604/off worse).
  nRet also not: `(s64)` round trips on the Mem_cpy arguments / nSize / pLib, a pChar local,
  volatile nRet (worse), nRet as u32/int, 10 << 10, 0x2800u.
  Kept (each checked by reverting it on the exact version): uAl scratch (in place: 11 aligned),
  u32 uAl (s32: 174), u32 strides (s32: 166), n50Al in the loop block (at the top: 28), `+=`
  (13), s declared before p and bFound (9), `pSlot = &lbl_801C6068[k]` in the s loop (14),
  one pHdr instead of pEnd + pHdr (200).

- 2026-09-27 b6. 87.73 -> 96.26. Harness: build/perm snapshot + quicktrial compile, three scores
  (position diffs, aligned diffs, aligned with registers masked = "noreg"); base 409 / 276 / 65.
  Structure first (noreg), then registers (mwccdbg last pass + rasim replay, trace of the simplify
  sweeps / spill-cost choices). What worked, in order (noreg after each):
  * nCopied u32 (EA's `pBank->uId += nCopied` is addc/addze: zero-extended) and nHdr u32 (an s32
    nHdr leaves a conversion copy `mr` before nCopied = nHdr): 65 -> 32.
  * `pUsed` before `pBank` (EA computes k*0x18 first).
  * nSize as `sizeof(AnimLib) + pSrc->nTreeSize + pSrc->nClips * 4` (EA: add nTreeSize,nClips*4
    then addi 0x148; the other spellings reassociate the constant first), computed before
    `pNew = pOv->pChar->pLib`; no pChar local: EA reloads pOv->pChar (lwz 0xc(r28)) at every use.
  * final player loop: `pSlot = &lbl_801C6068[k];` inside the s loop (EA keeps &slot[k] in r7 and
    reads nOverlays at 0x14c from it; indexing lbl_801C6068[k] hoists the 0x14c address): 40 -> 33.
    Its volatile registers (s r9, p r10, bFound r11): declare s before p before bFound.
  * The four round-ups to 32 (stride1, stride2, n4C, n50): EA keeps the result in a scratch and
    copies it into the variable (`mr r0,r3 .. mr r23,r0`, `mr r0,r17 .. stw r0,0x24`,
    `mr r4,r16 .. mr r14,r4`). What gives it: one scratch variable, `t = e; if (e & 31) t = ((e >>
    5) + 1) << 5; dst = t;` (tested and rounded from the size itself, not from t). The scratch
    must be u32 (an s32 scratch: n50Al's copy is propagated away and n50Al coalesced; f / uAram /
    uAramStart as the scratch: the variable's later web becomes a frontend temp and takes a high
    register, since the frontend keeps a variable's name only for its FIRST web). A fresh u32 local
    (uAl) is right: aligned 167 -> 64.
  * the ARAM size sum: `nSize = pHdr->n38 + pHdr->n04;` before n4C, `GoARAM_Alloc(n4CAl + n50Al +
    nSize)` (the frontend turns ((a+b)+c) into a+(b+c): EA's add order T+n then n4CAl+..).
  * spill set: EA spills n4CAl (0x24) and keeps n50Al in r14; pass-1 tie (same cost/degree) goes
    to the higher vreg, so n50Al must be declared after n4CAl, but the slot order (n4CAl 0x24,
    uAramStart, k, pLibFile, nRet, i, pUsed = reverse declaration order) needs n4CAl declared
    last: n50Al is declared in the loop body block (block locals number after all others).
  * nStride1/nStride2 u32 (+ `(s32)nStride1 / 2`, EA's halving is signed): as s32, the frontend
    hoists `(u32)nStride1` for GoARAM_CopyToAram as a loop temp (@122) that jumps to the top of
    the priority list. No pHdr/pEnd pair: one `Clip* pHdr` walked by `(u8*)pHdr + nCopied` (a
    separate pHdr is one more long-lived neighbour; with both fixes nSlot drops to r15 as in EA).
  Not better / no change (all on the way, noreg): inline Align helpers (int/s32/u32, in place, r
  local, ternary, two returns: the frontend folds them all to the same code), an ALN ternary
  macro, `(u32)`/`(s16)` casts on the macro, assignment expressions inside the Alloc sum, n or
  f as the scratch (f: loops become temps; n: clobbers the sum), a separate pEnd with
  `((Clip*)pEnd)->` (same as pHdr only), i as long/s16, `[(u32)i]`, `pRecords + i`, byte-offset
  indexing, `i = 0` moved before the if / to the top / to the join after both branches (join:
  mulli r31,i,0x24), `i = nRet`, `i = j - j`, wrapping the body in `if` instead of `continue`,
  per-function pragmas (optimization_level 1/2/3, opt_propagation / strength_reduction /
  loop_invariants / common_subs / lifetimes / dead_assignments off: 63-156 vs 63).
  nRet = 0x2800 placement (before/after nSize/pNew, top of block, u32/int nRet, `nRet = 0;` as a
  statement): no change; after Mem_cpy / at the block end moves the store out of the block.
  Where it stands: EA's pre-RA order has `li nRet` after the pLib load (its li r0 reuses nClips's
  r0 after the slwi); ours issues it 4th, and the post-RA pass then gives nClips r5 / slwi r0.
  The preheader needs `mr vOff, vI` (EA): our frontend emits the IV init as `li 0` whenever i's
  start is known; CSE never merges it with i's `li 0`. A `(u64)` OR-swap on pSlot->pEnd (the
  UISEvent lever) moves pEnd's load later (aligned 50 -> 31) but not the IV init.

## Collected from the notes and docs (2026-09-25)

### docs/decomp-notes.md

```
  not a declaration at the top (`AnimLib_MergeOverlay`).
```

### docs/journal.md

```
  (`AnimLib_MergeOverlay`, `AnimLib_PlanBank`, `AnimLib_WalkPair`). Scratch tools in `C:\dev\scratch\tw\`:
```
