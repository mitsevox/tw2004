# nm-a lane notes (golfer-01), 2026-09-25

## hlaudtrackseq (LINKED: .text, .bss 0x801F1880-0x801F18B8, .sdata2 0x80283FD8-0x80283FF8)
- fn_800AA9EC: sweep variant (identity s8 inline on n65-1, u8 i after nChannel) -> 2 (one extra mr).
  New sweep: nothing better. Hand sweep of inline signatures: `static inline u8 Read(s8 n)` on n65
  alone, `Read(n65) - 1` -> EXACT. u8 i still has to follow nChannel (original order: 18).
- fn_800AA744: 2 diffs (lwz pTmpl scheduled after the param mr's). New sweep: nothing. Tried:
  separate assign, inline returning pTmpl, bank local, n3 local: all 2. Permuter found score 0 with
  `pEvent->n3 & 0xFFu`; `(u8)pEvent->n3` is exact too (also (int), (s16), (u32)). EXACT.
- fn_800AAAA4: 2 diffs (orig clrlwi r7,n before apTracks load, mulli from r7). Tried: n as
  int/u32/s32 (worse), casts on aTracks/apTracks index, `aTracks + n`, identity inlines on n: all 2.
  New sweep: 2. Permuter: `n = (unsigned int)pEvent->n3` score 0; `(u8)pEvent->n3` EXACT.
  Finding: a redundant (u8) cast on a u8 field read changes scheduling (two functions here).

## char Character_PlaceFeetOnGround: 7 -> 3 (identity inline on pPos, fn_800187CC_Read)
- Left: orig loads fLow into f1 then `fmr f2,f1` (fY) and compares f1; ours loads straight into f2.
- Tried (all 3 or worse): ternary / if-assign / if-else / >= form / temp local / identity inline on
  fLow (else arm, both) / inline Pick(a,b) and Pick(a,p) / (f32) casts / double constant / `>`
  form / else-if chains / nested ternary. New leversweep (156 levers): 3. Permuter 15 min: none.

## SkinPart fn_800CE8C0: 26 -> EXACT
- Sweep variant (pSkin, pDesc after m) -> 10; new sweep: 10. pDesc in an inner block: 10.
  Identity inline on `pDesc = Read(apSkins[i]->pModel->pDesc)` inside the condition + the decl move
  -> EXACT (original decl order with inline: 16).

## SkinPart fn_800CE52C: 14 -> 5 (identity inline on nIndices, pDesc declared last)
- Left: pIndex / nIndices swapped (r31/r26).
- Tried: every position pair of nIndices/pIndex x 6 inline forms; inline types (s32/int/u32/long/
  s16/u16 in/out); inline on p28/p20/p44/pDesc/n4/n44/nFirst/pDesc-assign; `for (...; j++,
  pIndex++)` (6); int nIndices (15); index local k, SkinDesc14* local; decl climb (5); param n as
  s32/u32/u8/s16/u16. Order from the original's registers (pIndex, nFirst, i, j, nIndices, pDesc
  first, no inline) = 6, only the n*8 temp and pIndex swapped (r30/r31). Permuter 15 min x2: none.
