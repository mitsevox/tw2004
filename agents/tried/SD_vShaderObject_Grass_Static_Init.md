# SD_vShaderObject_Grass_Static_Init (GoShaderObject_Grass_Gc.c, 0x80120304)

Status: OPEN, 82.05% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

## Collected from the notes and docs (2026-09-25)

Nothing recorded.
- 2026-09-26 round 3 (r3-render; written by the orchestrator from the lane report, the disk was full): nBits as s32/int 284, s16 285, u32 292 (base 288). Orig divides by 32 signed; its frame is 0x100 vs our 0x110.
