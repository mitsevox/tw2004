# FO_spLoadFontFromStream (LLFont.c, 0x800107F4)

Status: OPEN, 84.75% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

- 2026-09-25, n-ll (quicktrial aligned, base 407). Reading: the target keeps pFile (r30) and pBytes
  (r28) apart (ours coalesces both into r31); its 16-bit swaps are `rlwinm 16,23` + `srawi 8` (ours
  folds to extrwi); its n0C swap is the signed form like n00 (ours gets the unsigned form, as if CW
  knew n0C > 100). GC/1.3.2, 2.0, 2.6, 2.7: 407; 2.0p1: 415. uVersion swap spellings ((s32),
  (int)mask, (s16)x, both (s16), `/ 256`, terms swapped, `(x >> 8) & 0xFF`, 0xFFFFFF00): 399-407,
  none gives the srawi. Swap16/Swap32 static inlines with int/s32/u32/u16/s16 parameters for the
  16-bit fields and n00/n0C: 402-431 (CW propagates through the inline). No change kept.

## Collected from the notes and docs (2026-09-25)

Nothing recorded.
