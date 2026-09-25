# PictInt_Decode (LLPictInt.c, 0x8005620C)

Status: OPEN, 93.08% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

## Collected from the notes and docs (2026-09-25)

### agents/notes/cloud-2026-09-24-round1.txt

```
- LLPictInt PictInt_Decode 92.54 -> 93.08 (merged): height swap `(((u16)h >> 8) & 0xFF) |
  (((u16)h & 0xFF) << 8)` gives EA's clrlslwi 24,8 + rlwimi. Width (EA: extrwi then rlwimi 8,16,23):
  5 x 6 term spellings x both orders x none/(s16)/(u16); static inline Swap16 (u16/s16/int/u32 x 6
  bodies); u8/u32 casts: no. stwbrx order: u32 local, __stwbrx(v, pFile, 0xC), (u8*)pFile + 0xC: no.
  Loop mullw r9 vs r0: pointer-add args, operand orders, (y*w)/4, y/4*w: no.
```

### agents/notes/map-10-notes_w4.txt

```
- PictInt_Decode: orig byte swaps are NOT the sthbrx idiom (masked rlwimi 16-23 / clrlslwi); 60+ forms tried.
```
