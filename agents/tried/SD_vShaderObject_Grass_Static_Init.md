# SD_vShaderObject_Grass_Static_Init (GoShaderObject_Grass_Gc.c, 0x80120304)

Status: SOLVED 2026-09-27 (lane b3): exact, unit linked. Fix: the 2026-09-27 b3 levers below, then
the row-setup statement order (a labelled fake match) found by search; see the last attempt.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)
- 2026-09-26 r4-render (aligned, base 288): EA's three f32[3] arrays are all real (afPoint at
  0x28, afNew 0x1c, afClip 0x10 in EA's frame); afClip folded into afNew gives the 0x100 frame
  but scores 314 (into afPoint 318), so the extra 0x10 is spill slots (ours spills nSet and
  more to 0x70-0x8c), not a missing local. Not pursued further.
- 2026-09-26 r4-render: random dependency-keeping moves of the 17 row-setup statements (pCur ..
  pNewEnd, 5 min): aligned 288 -> 272, but real 82.05 -> 73.55 (reverted).
- 2026-09-27 b3: 82.05 -> 94.26 (quicktrial aligned 288 -> 46; commit "SD_vShaderObject_Grass_Static_Init
  82.05 -> 94.26"). Levers, in order of effect:
  * `int nSet` (was s32 = long): passing a long to fn_80120C2C's int parameter made the frontend
    hoist `(int)nSet` into three loop-invariant copies (@108/@111/@113, spilled); EA keeps nSet in
    r29 (265 -> 231).
  * `fHi = 2.5f + pDesc->fX` (not `+ fLo`): EA's `lfs f1; fmr f31,f1; fadds f30,f0,f1` (-> 225).
  * The merge `while`: `while (pCur != pEnd && afPoint[a] <= afNew[a]) { if (afPoint[a] == afNew[a])
    { if (*pCur > *pOld) break; if (*pCur == *pOld && uStep > *pOldStep) break; } ... }` puts EA's
    fcmpu / tie tests at the loop top (-19). `s32 nBits`: EA divides signed (srawi/addze).
  * Declaration order from `rasim.py search` (pass-2 targets read from EA's asm: nSet r29, pCur r28,
    pNew r27, pOld r26, pEnd r25, pNewStep r24, pOldStep r23, bFirst r22, nCount r21, nBit r20,
    uStep r19, bStart r18, pBits r17, pVerts r16, pTriFlags r15), spilled locals after it in EA's
    stack-slot order (nRow i pNewEnd pNext nOldRows nNewRows nCur nOther nPrev nNewBit nBitBase) (-> 96).
  * `pRows += 2 * (...)` instead of `pRows = (u16*)((u32*)pRows + ...)`: the frontend splits the
    cast form into a new variable @128 (r5, then r18); EA keeps one r18 (-> 89).
  * `u32 uHead`; `nOldRows = (uHead >> 8) & 0xFF; pEnd = pCur + nOldRows; ... nNewRows = uHead & 0xFF;
    pNext = pEnd + nNewRows;`: EA's extrwi / rlwinm 25,23,30 / clrlwi / clrlslwi, and nOldRows /
    nNewRows stay real variables spilled in pass 1 at 0x64 / 0x60 (with `(u8)` casts the backend CSEs
    them into temps and copy-propagates the variables away; u16 uHead gives srawi) (-> 56).
  * `nCount++; nBit++;` in that order (-> 48); `nBitBase += nOldRows + nNewRows;` gives EA's two
    `add r3,r0,r3` (-> 46).
  Left (all in one block, 0x220-0x2bc, the row setup before the first fn_8001E9CC): the order of
  that block, and EA has one reload of nOther fewer. Mechanism found: the post-RA CSE pass
  (backend-21) turns a spill reload into a register reuse when the stored register is still
  intact; EA's `subfic r0` (nOther) survives to `slwi r7,r0,7`, ours is clobbered because uHead
  takes r0 (EA: r5). uHead has 36 pass-2 neighbours (> 28, coloured early, first free volatile r0);
  its live range ends at nNewRows' clrlwi, which the first scheduling pass always puts last (a leaf).
  rasim search (declaration order) cannot give uHead r5; rasim --key what-ifs for uHead and the
  SD_gpGrassTypeData temp @118 do not either. Tried without gain (aligned 46 unless noted): the 17
  row-setup statements hill-climbed (2 x 10 min, best 46); `a200[nOther = 1 - nOther]`, `a000[...]`
  embedded forms; pNewStep before pNew; `a200[1 - nOther]` before the update (55); a temp local
  (75); pNext as `&pEnd[n]`, `pNewEnd + n` (46), `pCur + nOldRows + nNewRows` (87); nNewRows first;
  nNewRows/nOldRows as u8 (81/98), u16 (81), u32 (46/60); `#pragma scheduling once` (108) / off (211).
  Tooling: tools/match/sched750.py's first-pass model (with `bl` treated as a branch) reproduces this
  block's first pass exactly; its last pass does not (calls/spill slots are not modelled).
- 2026-09-27 b3: 94.26 -> 100, EXACT. What was left was register colouring in the row-setup block:
  EA's SD_gpGrassTypeData temp (the frontend's @118, feeding a000/a200) is in r0 and uHead in r5, so
  the SD temp must be coloured before uHead, i.e. have > 28 pass-2 neighbours (ours 26) = a longer
  live range in the first scheduling pass. Enumerated source orders of the 17 row-setup statements
  through sched750's first-pass model (with `bl` as a branch; 7628 distinct first-pass orders from
  30000 random topological orders), ranked them by the SD temp's live length, compiled the top 40
  (best 14), then a quicktrial hill-climb from that order reached 0:
  `nCur = 1 - nCur; uHead = *pRows; nOther = 1 - nOther; pNew = ...a000[nOther]; nBit = nBitBase;
  pCur = pRows + 1; nPrev = nCount; pOld = ...a000[nCur]; pNewStep = ...a200[nOther]; pOldStep =
  ...a200[nCur]; nCount = 0; nOldRows = ...; pEnd = pCur + nOldRows; nNewRows = ...; nNewBit = ...;
  pNewEnd = pEnd; pNext = pEnd + nNewRows;` (labelled fake match; same statements, every dependency
  kept, so the same values). Unit linked: .sdata2 0x80284AD0-0x80284B00 with a
  GoShaderObject_Grass_Gc_StrippedFn stand-in (1.0f first in EA's pool), and the unit's data by hand:
  .bss 0x802607D0-0x80260CB8 (SD_gGrassTypeData, TW06 name, + the five lbl_ arrays) and .sdata
  0x80281908-0x80281910 (SD_gpGrassTypeData = &SD_gGrassTypeData).
  Also: the scratch copy of sched750 needed store/load edges only between the same stack slot
  (spill slots do not alias) to reproduce this dump's last-pass blocks (91 -> 93 of 96).

## Collected from the notes and docs (2026-09-25)

Nothing recorded.
- 2026-09-26 round 3 (r3-render; written by the orchestrator from the lane report, the disk was full): nBits as s32/int 284, s16 285, u32 292 (base 288). Orig divides by 32 signed; its frame is 0x100 vs our 0x110.
- 2026-09-26, r6-assert: dead asserts (agents/findings/2026-09-26-dead-asserts.md): not swept. Frame: on GC/2.5 a dead or unused buffer takes no stack (unused, write-only, used only under if (0) / static const 0 / goto, empty inline: all frames unchanged), so dead debug code cannot explain a frame difference either way.
