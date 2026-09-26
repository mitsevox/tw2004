# Character_SetupForShot (char.c, 0x8001C860)

Status: OPEN, 99.90% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

- 2026-09-25 n-charskin: bStance/bPlace/bClipTime as u32: 9, s32: 6, u16: 37; the three
  flags read before pPlayer/pModel: 6; pBallPos from gPlayers directly: 11; nPlayer in a local: 6.

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
char Character_SetupForShot
base 6, 199 levers, 31039 variants (199 singles) in 318 s; best 6

6 [safe]
  - move `f32 vDir[4];` to line 1 of the declarations

6 [safe]
  - move `f32 vPos[4];` to line 8 of the declarations

6 [safe]
  - move `f32 vPos[4];` to line 9 of the declarations

6 [safe]
  - move `f32 vPos[4];` to line 10 of the declarations
```

## Collected from the notes and docs (2026-09-25)

### agents/notes/map-02-notes_w6.txt

```
- char Character_SetupForShot: all 360 orders of the 6 head statements: 16 stays (as map-09).
```

### agents/notes/map-06-notes_w6.txt

```
  fn_8000AE28 swap (45 calls): Character_SetupForShot 99.69 -> 99.90, nothing down.
```

### agents/notes/map-06-notes_w7.txt

```
  down; Ball_FlightStep 98.72 -> 100 (with if/else fDrag), Character_SetupForShot 99.69 -> 99.90.
```

### agents/notes/map-10-notes_w8.txt

```
- char Character_SetupForShot (6: nPlayer/u10 load regs swapped): gPlayers + n, (u32) index, pModel
  first, u10 through a u32 local: all 6.
```
- 2026-09-26 round 3 (r3-charskin; written by the orchestrator from the lane report, the disk was full): nPlayer/u10 locals in 80 type and order combinations, inline getters, the void* copy: best 6.
- 2026-09-26 r4-charskin: pBallPos from gPlayers again 11, a void* view of pChar for nPlayer 6, u10 read first / bClipTime first / pPlayer last / flags after pBallPos: 6 (identical code: the source order of the two loads does not reach the allocator), `? 1 : 0` 31, SetPosition on pPlayer->ball.vPos 14, pBallPos assigned in the call 6; GC/1.3.2-2.7: 6.
- 2026-09-26 r4-charskin: the three flags in one comma statement 6; inline getters for &gPlayers[nPlayer] (11) and for u10 (6), both 11.
- 2026-09-26 r4-charskin: pPlayer as integer address math (u32 / int / u8* spellings): 6.
- 2026-09-26 r4-charskin: every order (5040) of bStance, bPlace, bClipTime, pModel, pBallPos, pPlayer, pSkel: 6.
- 2026-09-26 PC declsearch (run 36221888505, iterated local search over the declaration order): best 6 (no better order than the current one), 11346 trials.
- 2026-09-26, r5-game (mwcc-debugger): the only '!' is `r51 -> r7 !EA r6 11 nb @1457`: @1457 is
  the frontend's CSE temp for pChar->u10 (shared by the three flag reads); nPlayer's load is the
  backend temp r52. All small temps are added in vreg order, so r52 is assigned before r51 and
  takes r6. EA needs the u10 load numbered above the nPlayer load: the u10 reads not
  frontend-CSEd (a backend temp made after the nPlayer load), or the nPlayer read made a frontend
  temp. Tried (quicktrial): `(*pChar).u10` on one read, (int)/(s32) casts on two reads,
  parenthesised reads, `&pPlayer->ball.vPos[0]`, pPlayer after the flags: 6; `x = u10; x &= mask`
  per flag 287; pBallPos from `gPlayers[pChar->nPlayer]` 11 (still swapped, plus pPlayer /
  pBallPos); an `int nPlayer` local (copy-propagated away, same dump): 6. pBallPos from
  `gPlayers[pChar->nPlayer]` with both gPlayers statements after the flag reads, in 4 orders: 11
  (the dump: the frontend now CSEs the whole &gPlayers[n] into @1458, but the nPlayer load itself
  stays a backend temp above @1457). No source change.
