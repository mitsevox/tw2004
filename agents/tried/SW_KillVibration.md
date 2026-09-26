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
- 2026-09-25, ChatGPT subagent: `Player*` identity inlines at seven combinations of the player,
  frame and controller address reads; `pFrames[0]`/`*(pFrames + 0)` final stores; byte-pointer
  derivations through real struct members; explicit controller-value temporary with pointer
  assignments before, between and after the first call; pointer initializers inside the pad branch:
  all 4 -> 4. A short `leversweep.py` combination run (two workers, completed in ~17 seconds)
  also found no improvement (best 4). No source change.
- 2026-09-25, ChatGPT subagent (second pass): inspected exact liveness: target copies the player
  address `r29` to `r30`, loads the first controller value from `r29+0xA08`, then advances `r29`
  to the frame field and `r30` to the controller field before the first rumble call; ours folds
  the frame offset into the final store and advances the controller address before the load.
  New tests: inline value-read helpers (`s32*`, `Player*`, index), C89 block-initialized pointers
  with qualifiers, `__restrict` pointer locals, chained final stores, and explicitly staged
  member-derived byte pointers: best still 4 (staged byte pointers were 13). No source change.
- 2026-09-25, ChatGPT subagent (second pass continued): varied the player-base local between
  `void*`, `const void*`, and `const Player*`, then recovered real fields through typed pointer
  casts at each address-taking site. All compiling variants stayed at 4. This did not recover
  the target's delayed pair of pointer additions. No source change.
- 2026-09-25, ChatGPT subagent (third pass): whole-block control-flow rewrites with a guard
  and early return (`!HasPad`, `HasPad == 0`, braced form), a scoped active block, and a
  `goto` active label all stayed at 4; a `switch` guard worsened to 6. Static inline blocks
  for the two rumble calls, the two final clears, or the whole active tail, with either
  original guard or early return, all stayed at 4. No source change.
- 2026-09-25, n-const: SwingData* through `static inline KV_Swing(Player*)` with `pSwing->nVibrateCount = 0` 4; int temp = p->nController first 19; pointers set then first call on `p->nController` 19; `p = q = &gPlayers[n]` with pController from q 19; `gPlayers + nPlayer` with pFrames after the first call 4; inline `KV_Off(Player*, nPlayer)` holding the whole tail 8; inline taking the two pointers (from p or gPlayers[]) 19/20; `&KV_P(nPlayer)->field` for both pointers 7; inline for the two calls with `q->nController` first 19. Compilers 1.3.2 / 2.0 / 2.0p1 / 2.6 / 2.7: all 4.
- 2026-09-25, n-const (2): pointers made multiply-assigned so they are not folded: `= 0` initialisers 4; `p = gPlayers; p += nPlayer` 12; `p = gPlayers; p = &p[nPlayer]` 4; pFrames = `(s32*)&p->swing` then `&((SwingData*)pFrames)->nVibrateCount` 7 (keeps both pointers live and the first call reads 0xa08(p) as EA does, but adds 0x3D4 + 0x460 separately); pController assigned again between the calls 19. No pointer locals (p->field / two Player* copies p, q in 3 mixes, SwingData* pSw) 19; pSw + pController 7.
- 2026-09-26, r5-game (mwcc-debugger): registers already agree (@782 r31, pController r30, p r29);
  frontend-01 (after its AST optimisations) no longer has pFrames at all: the frontend substitutes
  the single-use pointer, so backend-00 stores `stw r44,r33,0x834` through p. EA's C must keep
  pFrames past the frontend. pFrames as int* / u32* / void* (cast at the store), pController as
  int*, both int*, a SwingData* pSw for the store: 4 (aligned). No source change.

## Structural constraint from the third pass

The target keeps a frame pointer live before the first rumble call and writes through it at the
end. Our C exposes only one use of that pointer, the final store, so CodeWarrior safely folds the
frame offset into that store. Rewriting the guard or moving these effects into inlined blocks did
not prevent the fold. An added observable read/write or non-inlined call would change behavior or
the instruction stream; only a semantics-preserving source shape that makes the compiler keep
the early pointer is acceptable.

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
- 2026-09-26, r6-assert: dead asserts (agents/findings/2026-09-26-dead-asserts.md): an empty `if (x) { } else { }` (the only assert form that leaves any trace on GC/2.5) after every statement of the function, quicktrial aligned: an empty if/else after the pointer setup keeps pFrames past the frontend (the `addi rX,base,0x834` before the first call, as EA) but with other registers: 4 -> 7 for every condition and position; with it, 4896 variants (pointer spellings from p or gPlayers[], first call through pController / p / gPlayers[], the flag store through p or gPlayers[], both assignment orders, all declaration orders, the assert on pFrames / pController / both / p at 4 positions): best 4. No source change.
