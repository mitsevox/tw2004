# SW_vImpact (Swing.c, 0x8005AEE0)

Status: OPEN, 99.81% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

## Lever sweep, 2026-09-24 (the PC, levers before 543bf7b)

```
Swing SW_vImpact
base 6, 37 levers, 2483 variants (37 singles) in 25 s; best 6

6 [safe]
  - move `f32 fAim;` to line 1 of the declarations

6 [safe]
  - move `f32 fAim;` to line 2 of the declarations

6 [safe]
  - move `f32 fAim;` to line 3 of the declarations

6 [safe]
  - move `f32* pLaunchB;` to line 5 of the declarations
```

## Collected from the notes and docs (2026-09-25)

### agents/notes/cloud-2026-09-24-round1.txt

```
- Swing SW_vImpact (6): pLaunchB assignment at 15 positions 7; SwingData* pSw local over 127 mixes of
  the 7 swing.* sites 7; f32* pPower 23; physics argument through p-> 44; no pLaunchB 41.
```

### agents/notes/map-03-notes_w8.txt

```
Swing SW_vImpact permuter 14 min -j2 (at 99.81): no improvement found.
Swing SW_vImpact 99.2 -> 99.81: `pLaunchA = gPlayers[nPlayer].vLaunchA;` (not p->), decl pLaunchA
  before pLaunchB (w8/sites.py per-site p->/gPlayers[]/PLAYER() sweep). Left: pLaunchB r27 vs r29 and
  the fShotPower stfsu temp r29 vs r27. Tried: Physics arg via p-> (98.3), pLaunchB via gPlayers[]
  (same), via PLAYER() (95.9).
Swing SW_vImpact: all-gPlayers / all-p / PLAYER() / p = gPlayers; p += n / p set late: all 23 (no).
```

### agents/notes/map-06-notes_w7.txt

```
- SW_vImpact (23, EA p = r31 and the gPlayers offset r30): decl climb, p initialiser in 3 places,
  gPlayers + nPlayer: none.
```
