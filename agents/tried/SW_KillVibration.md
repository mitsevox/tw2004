# SW_KillVibration (Swing.c, 0x80059EB4)

Status: OPEN, 92.75% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

- 2026-09-25, ChatGPT: initialized `Player*` earlier, reordered pointer declarations and
  assignments, used direct frame-field store, and applied `register` to each pointer: 4 differing
  instructions -> 4 (early Player initialization worsened to 12, moving its declaration to 7).
  No source change.

## Collected from the notes and docs (2026-09-25)

### agents/notes/cloud-2026-09-24-round1.txt

```
- Swing SW_KillVibration (4): 864-variant sweep of pFrames/pController from p or gPlayers[], argument
  spellings, 6 assignment orders, bVibrating/nVibrateCount through p or gPlayers[]; Player* copy pX with
  pController = &pX->nController, pSw->nVibrateCount, int temp (164 orders): all 4 (copy-propagated).
  TW07's shape (pCore, pSw, temp, static inline GetSwingInfo): 7-16. EA keeps pFrames as p+0x834 in p's
  register and pController as mr + add: looks strength-reduced; no C form found.
```

### agents/notes/map-03-notes_w8.txt

```
SW_KillVibration: TW07-shaped forms (int temp = nController; pointers from gPlayers[] directly;
  two Player* copies): all worse.
Swing SW_KillVibration: body in a one-pass for loop (5 forms, 25-30), p->nController first call +
  pController/pFrames in 6 orders (21-35): no.
Swing SW_KillVibration: inline helpers returning &p->nController / &p->swing.nVibrateCount (by p or
  by index, 5 mixes): worse (21 -> 23/24 quicktrial). Decl climb: no.
```

### agents/notes/map-06-notes_w7.txt

```
- SW_KillVibration (EA: pController = copy of p then +0xa08; first call loads 0xa08(p)): p->nController in the
  first call, gPlayers[] forms, statement orders: 4-19, none better than base 4.
```

### agents/notes/map-10-notes_w5.txt

```
- Swing_RumbleOff: pointer order/const/decl permutations, p-> forms; TW07 SW_KillVibration has int temp +
  pCoreShotInfo + pSwingInfo.
```
