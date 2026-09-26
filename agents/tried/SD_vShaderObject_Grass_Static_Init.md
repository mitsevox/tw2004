# SD_vShaderObject_Grass_Static_Init (GoShaderObject_Grass_Gc.c, 0x80120304)

Status: OPEN, 82.05% on 2026-09-25.

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

## Collected from the notes and docs (2026-09-25)

Nothing recorded.
- 2026-09-26 round 3 (r3-render; written by the orchestrator from the lane report, the disk was full): nBits as s32/int 284, s16 285, u32 292 (base 288). Orig divides by 32 signed; its frame is 0x100 vs our 0x110.
