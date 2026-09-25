# BFX_vRender (goballfx.c, 0x80093AE0)

Status: OPEN, 89.67% on 2026-09-25.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

(add yours here: date, lane, what, score)

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
