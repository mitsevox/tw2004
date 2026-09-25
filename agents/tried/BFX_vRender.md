# BFX_vRender (goballfx.c, 0x80093AE0)

Status: OPEN, 89.67% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

### 2026-09-25, ChatGPT round 3

Baseline in the real `main/goballfx` unit: 89.67376% for `BFX_vRender` (unit 91.68%, 3/4 exact). The matching scratch `quicktrial.py --aligned` baseline is 32 differing instructions out of 141. No tracked source edit was made in this pass; all experiments below were compiled with the unit's CodeWarrior flags in ignored `build/perm/BFX_vRender` scratch.

- Tested pUV pointer/index identity helpers dependent on one of `r/g/b/a`: all 32 -> 32. They did not postpone the early `lis r3`.
- Put pUV initialization and subsequent drawing into a nested scope beginning before `r`, after `r`, after `g`, after `b`, or after `a`: 32 -> 51, 44, 32, 32, 32 respectively. The scopes did not reproduce the target's register reuse.
- Jointly varied the first color-table row-pointer creation before/after each channel, `table[nPlayer]` versus `&table[nPlayer][0]`, pUV assignment position, and row-pointer declaration placement (250 variants): best 32 -> 31, when red/green are direct-indexed and blue/alpha use a row pointer. No zero-score variant.
- Tried direct destination-array color writes, direct UV-array writes, and the above mixed source pointer in eight combinations: scores 32, 31, 44, 43, 32, 31, 37, 37. Moving the pColour/pUV assignments jointly with the mixed source pointer (50 variants) also bottomed out at 31.
- Checked the TW07 paired function's `PlayerNumber_t` enum as a scratch parameter/local type: 32 -> 32; PS3's body uses a substantially different rendering interface, so it did not supply a GameCube ordering clue.
- Tried a local `GXColor` aggregate with field assignments/readback: 32 -> 32. Source pointer only for green or only for alpha: 32 -> 31. pUV immediately after red plus a source pointer for blue/alpha: 32 -> 31. None changes the core target/ours scheduling divergence.
- The second `aIndex` data load differs by relocation (`SYM@sda21` target versus `@4+0x4@sda21` ours). Splitting its initializer into two initialized elements plus two assignments, or all four assignments, worsened 32 -> 40. The original initializer remains best.

The first substantive divergence is the color/pUV setup at instruction 39: target keeps `lis` for pUV until instruction 51, forms the color row in `r4`, and loads alpha into that same `r4`; ours loads pUV at 41, forms the row in `r5`, and loads alpha into `r0`. Downstream 16 byte stores match in shape but retain those different source registers. This looks like source-lifetime/CodeWarrior instruction scheduling, not a different color algorithm. The safe structural families tested here only move the scratch diff by one instruction; preserve the clearer current source until a larger real-unit improvement or exact variant is found. `ninja` completed and `lint.py --diff main` reported 0 findings; no DOL/source change to verify.

### Targeted live-range follow-up (2026-09-25, ChatGPT round 3)

The target computes the UV address at instructions 51–55, between the first red load and remaining color loads, then holds it in `r30`; ours emits UV `lis` at 41 but otherwise computes the same address and holds it in `r30`. To test whether C-level UV lifetime caused that scheduler choice, moved the `pUV` assignment after color stores 0/1/2/3/4/7/8/11/12/15. Baseline stayed 32 for every position; combined with the blue/alpha row pointer and with the alpha-only row pointer, every position stayed 31. CodeWarrior hoists the independent address computation regardless of the source placement.

`register` hints on pUV, pColour, each color byte, all bytes, and the parameter (also `const` parameter) each remained 32; they do not affect this allocator/scheduler path. A typed pointer to a `[4]` row (`&table[nPlayer]` or `table + nPlayer`) gave 125 when used for all channels, 32 when introduced after red, 31 after green or blue, and 32 after alpha—same as a plain byte row pointer. Four equivalent UV address spellings (`&array[n][0]`, `*(array+n)`, flat base plus `n*8`, and `(array+n)[0]`) all remained 32. No tracked source edit. The remaining discrepancy seems to require a different upstream live-range constraint or original source construct, not merely later placement/scope of the UV local.

## Collected from the notes and docs (2026-09-25)

### agents/notes/cloud-2026-09-24-round1.txt

```
- goballfx BFX_vRender (32; EA computes pUV after the red load, alpha last into the row register): pUV at
  18 positions 30-39; pColour after each colour load 31-132; 24 orders of r/g/b/a decls and loads x 2 pUV
  places 30+; int/s32/u32/s8/char and a u8* row pointer 30+; direct array references 32-38; block moves
  32-83; colour stores as loops 32-157; declaration climb. All compilers 32.
```

### agents/notes/map-09-notes_w6.txt

```
goballfx BFX_vRender 89.1 -> 89.7 (applied): fY local dropped (0.01f + fGround written at each use), half size
  as its own local fSize = 0.02f (fixes every float register and the fadds operand order), pColour set before
  the colour reads. Left: colour byte registers (orig r5/r6/r8/r4, row pointer reused for a) and the pUV
  lis scheduled early. Tried: colour statement orders (all 15 keeping r,g,b,a order), pSrc row pointer (worse),
  4x colour loop (same), no `a` local (same), rgba int/order, pos decl order, decl climb 12 (no gain).
  Also the aIndex initializer: orig loads a second .sdata symbol for {2,3}, ours @N+4 (data labels only?).
```
