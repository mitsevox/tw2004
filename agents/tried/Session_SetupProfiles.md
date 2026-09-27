# Session_SetupProfiles (Code8002EE1C.c, 0x8002EF64)

Status: SOLVED 2026-09-26 (e-link2, 8d1ca71): SaveProfile.n54C2 is s8 in EA's struct (include/game/save.h,
was u8). Unit Code8002EE1C linked (.sdata2 0x80282F20-0x80282F30, Code8002EE1C_StrippedFn for the
1.0f that opens the pool).

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

- 2026-09-26, e-link2: SOLVED. mwcc-debugger: our registers were EA's, and EA's final order was our
  order BEFORE the post-regalloc scheduling pass. That pass only reschedules blocks regalloc touched:
  the block's `.n2 = gpSaveData[..].n54C2` store (s8 <- u8) left a dead `extsb` (peephole-forward
  moved the stb onto the lbzx), regalloc deleted it and so cleared the block's "scheduled" flag
  (`{400c}` -> `{4004}`), and the block was rescheduled. With `#pragma peephole off` the copy block
  matched (15 aligned, other diffs elsewhere), which pointed at it. n54C2 declared s8 (no
  conversion, no extsb): 14 -> 0, 95.84 -> 100. Also tried: `(u8)`/`(s8)` casts on the load 14,
  `*(s8*)&` 0 (not used: the header type is the EA form; FE_MessageTable already reads it `(s8)`).
  `#pragma scheduling 601/602/603/604/750/7400/off` 14-77; per-function pragma sweep 14-136;
  permuter 20 min (base 470): nothing before the fix was found.

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
Code8002EE1C Session_SetupProfiles
base 14, 11 levers, 371 variants (11 singles) in 4 s; best 14

14 [safe]
  - for loop #1 (`for (i = 0; i < pSession->nNumPlayers; i++) {`) as a while loop

14 [safe]
  - for loop #2 (`for (j = 0; j < 6; j++) {`) as a while loop

14 [safe]
  - for loop #3 (`for (j = 0; j < 6; j++) {`) as a while loop

14 [safe]
  - move `Session* pSession = &gSession;` to line 1 of the declarations
```

## Collected from the notes and docs (2026-09-25)

### agents/notes/cloud-2026-09-24-round1.txt

```
- Golfer Session_SetupProfiles 95.84 (name-copy scheduling only): manual unroll, j down, do-while,
  (s32) index, pSession->nGolfer[i], u64 temp, index local, s32 pairs, u64* source, f64 copy, nGolfer
  first, i/j/nGolfer types (150): no.
```

### agents/notes/map-02-notes_w6.txt

```
- Golfer Session_SetupProfiles (unrolled name copy scheduling): SaveProfile* / u64* locals, (gp + n)->,
  pSession->, count-down loop: none better than 14.
```

### agents/notes/map-03-notes_w8.txt

```
Session_SetupProfiles: per-site gSession./pSession-> sweep: no.
```

### agents/notes/map-10-notes_w4.txt

```
  Session_SetupProfiles, Ter_SearchForDropLocation, Ball_SetLie, Ball_FlightStep, fn_8003D414 (fnmsubs order),
```

### agents/findings/cleanup-audit-2026-09-23.md

```
- gszEmpty (Golfer.c:1752) unsized -> can't match (orig sda21 at 8002EEFC; Session_SetupProfiles 70.9%): size it.
  lbl_80281B40 (Swing.c:403) unsized is DELIBERATE: original uses lis/@ha (fn_8005CC18 100%) -> label it fake
  match. (So symaudit's sda finding for lbl_80281B40 is a correct flag of a deliberate trick: needs a label.)
```
- 2026-09-26, r6-assert: dead asserts (agents/findings/2026-09-26-dead-asserts.md): an empty `if (x) { } else { }` (the only assert form that leaves any trace on GC/2.5) after every statement of the function, quicktrial aligned: gPlayers[0].nController as the condition: 14. No source change.
