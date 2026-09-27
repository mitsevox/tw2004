# FO_spLoadFontFromStream (LLFont.c, 0x800107F4)

Status: OPEN, 87.02% on 2026-09-26 (r6-misc).

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

- 2026-09-27 b11 (quicktrial with a register-blind score: instruction shapes only, then aligned):
  KEPT, each checked on its own (noreg/aligned/raw from 77/361/521 on the r6 base):
  the texel loop as ONE 4-step loop `for (k = 0; k < 4; k++) { ...; pDst++; nX++; }` (CW unrolls
  it by 2: EA's `li 2; mtctr` with the dead `addi k,1` and nX++ per copy); `(*pSrc >> 4) & 0xF`
  (EA's `rlwimi ..,28,28,31`, no srawi; `/ 16`, `(u32)`, `(*pSrc & 0xF0) >> 4` same);
  `aCode[0] + ((aCode[1] << 8) & 0xFF00u)` (EA's clrlslwi); fX/fY `aX[0] + ((aX[1] << 8) &
  0xFF00u)` unsigned (EA converts with the unsigned magic, no clrlwi 16) and the uv block as
  `fX = ..; fX1 = fX + uWidth; fY = ..; fY1 = fY + uHeight; fX /= nRowBytes * 2; fX1 /= ..; fY /=
  nTexHeight; fY1 /= ..;` then the four stores (EA loads uWidth/uHeight before any store; stack
  slot order fX, uWidth, fY, uHeight); `pSrc = pData + 16; pSrc += w / 2 * nY + nByte;` (EA's add
  order); `s32 i` (EA's unfolded `li r0,0; cmpwi r0,0x100` 256-loop guard; long counter);
  **`u8* pData` parameter used as the byte cursor itself** (`pData += uBitmap`; engine.h prototype
  void* -> u8*, UFont.c passes a void*): EA's pData r28 separate from pFile r30, aligned 249 ->
  247, raw 240 -> 210 (a void* param with `(u8*)pData` casts: 240); compare through
  `((LLFontFile*)pData)->n0C` (EA's `lwz r0,0xc(r3)`); `pRec = (u8*)(pFont + 1); pRecs = pRec;
  pRec += n * sizeof(LLGlyphRec); pGlyphs = pRec` (EA's `add r4,r4,r0` into the same register);
  `x / 512.0f` not `x * (1.0f / 512.0f)` (EA's fmuls operand order: value first); declaration
  order from rasim (pFont r31, pFile r30, nTexHeight r29, pData r28). 87.02 -> 95.12 (report).
  No gain: nX/k as `i`/`nPalette`/each other (webs split; `i` for nX adds a hoisted copy);
  nX/k/nByte/nRow as s32/u32/short/u8 (int/s32 mismatch between nX and nByte adds a copy);
  `nByte + k >= w` without nX (no strength reduction, k used); pFile as the parameter with a u8*
  copy (coalesced); pGlyphs via `(pFont->pRecs = ..) + n`, `&pGlyphRec[n]`.
  Open: the texel loop's registers. EA colours nX (r3) and k (r7) before the loop temps (which
  take r23/r24/r26); ours: nX and k have 27 neighbours (<= 28) so they leave the graph in the
  first sweep and are coloured after the temps (r22/r23, temps r11/r12).
- 2026-09-27 b11, later: rasim what-if (dummy neighbours) showed nX and k need 2 more neighbours
  (29: then the loop temps get EA's r23/r24/r26 exactly). Found both, same code: `*pSrc++ & 0xF`
  for uFirst (a mask temp the post-RA peephole folds into the rlwimi, +1) and the tile offsets
  written from the loop counters, no nByte/nY: `pSrc += w / 2 * (nTileRow * 8) + nTileCol * 4;`
  and `nX = nTileCol * 4;` (the backend's induction temps, +1). 95.38 -> 95.91; quicktrial
  declaration climb 135 -> 105 aligned (95.95). No gain / changes code: `% 16`, `(s16)` casts on
  nWidth or nX, `(u8)(*pSrc >> 4)`, `*pSrc >> 4` with & 0xF on uFirst (srawi stays), copies of
  pSrc/pDst inside the k loop (propagated away), `nX += 1`, `++k`, `nX++` in the compare,
  swapped if/else (layout), block-scope nX/k (no change), GC/1.3.2/2.0/2.6/2.7 (same), 2.0p1/3.0 worse.
- 2026-09-27 b11, last (commit e8491f4, 99.49%): KEPT `if (nTileCol * 4 + k >= w)` with no nX
  (the backend's induction temp is EA's r3 `mr r3,r4`; 95.95 -> 96.25), then the declaration order
  from rasim (texel loop registers exact, 96.57), then **mask-first byte pairs in the uv block**
  `aX[0] + ((aX[1] & 0xFFu) << 8)` (the shift-then-mask form leaves a dead `rlwinm` that register
  allocation deletes, which makes the post-RA scheduler reschedule the loop-2 block; EA's order is
  our pre-RA order) 96.57 -> 99.49. Split webs (a local reused in two loops) get @ vregs just above
  the locals in declaration order; rasim does not renumber them when it permutes declarations.
  LEFT (36 differing instructions, all registers): loop 1 (EA nPalette r3, i r4, IVs r7/r8,
  cursor r3; ours i r3, nPalette r4, IVs r6/r7, cursor r8) and the entry copies (EA `mr r27,r4;
  mr r30,r3; mr r28,r3`, ours `mr r28,r3; mr r27,r4; mr r30,r28`: the post-RA peephole rewrites a
  load's base through the copy but never a `mr` source). Best lead (not applied, names): palette
  in the texel counter `k` and the loop-1 cursor in `pSrc` (both become split webs): nPalette r3,
  aligned 36 -> 31 (build/perm base). A real-compile declaration search on that variant was
  started (scratch qsearch.py). No gain on loop 1: aCode spellings, a local for the code, a
  `k` copy, `pGlyphRec = pRecs + i`, fresh counters per loop (the 256 guard needs `i` shared by
  all three loops).

- 2026-09-26 r6-misc: WHY the srawi forms: mwccdbg shows each peephole-forward pass folds only
  ONE `rlwinm; srawi` pair per basic block, the last one in the block (backend-01/-09/-13 fold
  n0C, nGlyphs, uVersion in turn; n00 is never reached). EA's header swaps all keep srawi, so
  EA's block had three more signed candidates after n0C: uGlyphs, u18 and uBitmap were signed.
  KEPT: those three fields s32 in engine.h (84.75 -> 86.57; the whole swap block now has EA's
  instruction forms), and the size `sizeof(LLFont) + n * sizeof(LLGlyphRec) + n *
  sizeof(LLGlyph)` (86.57 -> 87.02; quicktrial 363 -> 359, 8 other term orders 359-363).
  No gain: `if (((LLFontFile*)pData)->n0C > 100)`, pBytes through void*, pBytes/pFile copies
  swapped or chained (363-364); `s32 i` 360 (EA's first 256-loop keeps the `li r0,0; cmpwi
  r0,0x100` guard, i.e. a long counter, but not kept alone); pGlyphs from `pFont->pRecs +
  n` / `&pFont->pRecs[n]` (same). Left: pFile/pBytes kept apart in EA (r30/r28, ours both
  coalesced into r31: the backend copy-propagation pattern of UISScreen), the texel loop's
  shape (EA: `li r26,2; mtctr` inner k loop with rlwimi byte merges) and `aCode[1] << 8`
  (EA keeps a clrlslwi).

- 2026-09-26 r5-render: reading only (no variant): EA's n00 and n0C swaps both use the signed
  `rlwinm 0,8,15` + `srawi 8` form and the n0C one reloads `lwz r5,0xc(r30)` after the compare's
  `lwz r0,0xc(r3)`; ours emits the unsigned form for n0C (and extrwi for the u16 fields), i.e.
  our frontend knows those values are non-negative (n0C > 100 in the branch, u16 by type) and EA's
  did not; EA also keeps the compare on the parameter r3 and pFile in r30, pBytes r28.

- 2026-09-25, n-ll (quicktrial aligned, base 407). Reading: the target keeps pFile (r30) and pBytes
  (r28) apart (ours coalesces both into r31); its 16-bit swaps are `rlwinm 16,23` + `srawi 8` (ours
  folds to extrwi); its n0C swap is the signed form like n00 (ours gets the unsigned form, as if CW
  knew n0C > 100). GC/1.3.2, 2.0, 2.6, 2.7: 407; 2.0p1: 415. uVersion swap spellings ((s32),
  (int)mask, (s16)x, both (s16), `/ 256`, terms swapped, `(x >> 8) & 0xFF`, 0xFFFFFF00): 399-407,
  none gives the srawi. Swap16/Swap32 static inlines with int/s32/u32/u16/s16 parameters for the
  16-bit fields and n00/n0C: 402-431 (CW propagates through the inline). No change kept.

## Collected from the notes and docs (2026-09-25)

Nothing recorded.
