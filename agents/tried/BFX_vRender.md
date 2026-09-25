# BFX_vRender (goballfx.c, 0x80093AE0)

Status: OPEN, 90.12766% on 2026-09-25.

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

### Independent follow-up on color reuse and UV timing (2026-09-25, ChatGPT round 3)

Using the updated 90.12766% real-unit source as quicktrial baseline (31 aligned,
27 positional), put the row-pointer assignment *inside* the red, green, blue,
or alpha load expression. Scores were 125, 32, 31, and 31 aligned respectively;
putting UV-pointer assignment inside the first UV store also stayed 31. These
expression boundaries did not delay the pUV `lis` or reuse the target's alpha
row register.

Tested a flat `u8*` view of the existing color table so the red load could
retain a separate table base and `nPlayer*4` offset, as in target `lbzx`.
Using that view for red only, red+green, all channels, or red plus a flat row
pointer scored 45, 47, 95, and 95 aligned. The extra pointer arithmetic
regressed the codegen. A `const u8*` source-row pointer scored 39. The MWCC
build does not accept C99 `restrict` on source/color/UV pointer declarations
(all syntax errors), so it is not a portable lever with this compiler.

Making `desc.pUV` the early owner of the UV pointer, and using that field for
all UV stores, scored 75 aligned. The analogous color descriptor-field form
scored 75, and both together scored 146. This changed stack traffic rather
than obtaining the target's later UV address setup. No tracked source change
was retained; the current C already performs the target calls, color loads,
16 byte stores, and UV stores in the same order. The remaining diff is
register/scheduling plus the separate `aIndex` data relocation.

### Targeted live-range follow-up (2026-09-25, ChatGPT round 3)

The target computes the UV address at instructions 51–55, between the first red load and remaining color loads, then holds it in `r30`; ours emits UV `lis` at 41 but otherwise computes the same address and holds it in `r30`. To test whether C-level UV lifetime caused that scheduler choice, moved the `pUV` assignment after color stores 0/1/2/3/4/7/8/11/12/15. Baseline stayed 32 for every position; combined with the blue/alpha row pointer and with the alpha-only row pointer, every position stayed 31. CodeWarrior hoists the independent address computation regardless of the source placement.

`register` hints on pUV, pColour, each color byte, all bytes, and the parameter (also `const` parameter) each remained 32; they do not affect this allocator/scheduler path. A typed pointer to a `[4]` row (`&table[nPlayer]` or `table + nPlayer`) gave 125 when used for all channels, 32 when introduced after red, 31 after green or blue, and 32 after alpha—same as a plain byte row pointer. Four equivalent UV address spellings (`&array[n][0]`, `*(array+n)`, flat base plus `n*8`, and `(array+n)[0]`) all remained 32. No tracked source edit. The remaining discrepancy seems to require a different upstream live-range constraint or original source construct, not merely later placement/scope of the UV local.

### Post-main structural pass (2026-09-25, ChatGPT round 3)

Rebuilt the scratch baseline after main was pulled: quicktrial aligned still 32, and the target/ours are identical through the first 38 instructions except the earlier `aIndex` relocation. The target's next color/UV cluster starts with `lis r5` for pColour at instruction 39, computes the color row in `r4`, loads red into `r5`, then starts pUV at 51. Ours uses `lis r6` at 39, starts pUV at 41, computes the color row in `r5`, loads red into `r6`, and holds alpha in `r0`. The later color store mismatches are consequences of these live ranges, not extra/missing stores.

- Tested a semantically equivalent inverted early guard (`if (sentinel == fGround) return`) both with and without an enclosing block: 32 -> 32. The target branch shape was already correct.
- Materialized the four color bytes as C89 block-scoped initialized locals at four positions from the first pPos setup through pColour assignment: 32 -> 38, 38, 38, 32. The block begins before the color loads and ends before the later render calls. This neither delays pUV nor reproduces row-register reuse.
- Moved `pUV` declaration into an inner scope beginning at the first render call, pPos setup, last pPos write, or pColour assignment: 32 -> 32 each. Moving `pColour` declaration alone into an inner scope at the first three positions: 32 -> 58 each; moving both pointers there: 32 -> 58 each. CodeWarrior is sensitive to pColour's local scope but it is not the original shape.

No new candidate improved the quicktrial baseline from this family. I then checked the earlier 31-diff mixed row-pointer candidate in the real unit: `r/g` direct, `pSrc = lbl_80189CB0[nPlayer]`, `b/a` through `pSrc[2]/[3]`. It improves real `BFX_vRender` 89.67376% -> 90.12766%, and goballfx 91.68% -> 92.045715%, with 3/4 functions exact. This safe, labeled `// fake match:` source edit is retained. `ninja` succeeds, `ninja build/GW4E69/main.dol` reports no work (linked DOL unchanged because this unit is not yet exact), lint reports 0 findings, and `git diff --check` passes.

With the new 31-diff baseline, I tried color row pointers offset to elements 0/1/2/3 (then indexing back to blue/alpha), and alpha-only pointers at each offset. All eight compile to the same 31-diff result: CodeWarrior canonicalizes the row address. Using typed pointer-to-array destinations for pColour and/or pUV, with type-correct dereferencing in stores and descriptor fields, gives 31 in all four combinations. These do not affect the remaining green/alpha registers or the early UV `lis`.

I also limited the source-row pointer's lifetime with four distinct nested-block forms (initialized versus assigned local; blue/alpha together versus only one byte). All compile to the same 31-diff result, so lexical lifetime does not force the target's alpha-in-row-register reuse. As a type/layout check, a scratch-only declaration of `lbl_80189CB0` as `GXColor[6]` with direct `.r/.g/.b/.a` fields scored 32; taking a `GXColor*` row pointer scored 125. This gives no evidence that the shared `u8[6][4]` declaration is the cause, and no header was changed.

### Isolated compiler-version/flag diagnostic (2026-09-25, ChatGPT round 3)

Compiled the current `base.c` through each installed compiler without editing the
tracked source or shared build configuration. The GC/1.3.2, 2.0, 2.0p1, 2.5,
2.6, and 2.7 builds with the unit's `-O4,p -inline smart` flags all produced
the **same 141-instruction stream** (SHA-256 prefix `0c4a0dcf03c4` after
symbol normalization), scoring 31 aligned / 27 positional diffs. In particular,
all emit the early pUV `lis r3` at instruction 41 and the same color-row/register
schedule; none reproduces the target's pUV `lis` at 51 or alpha in `r4`.

The GC/2.0, 2.0p1, 2.5, and 2.6 object files were byte-identical in this
scratch compilation. GC/1.3.2 and 2.7 object hashes differed, but their
normalized instruction streams did not; those differences need not be code
and are not evidence for changing the unit compiler. On both 2.0 and 2.5,
`-O4,s` regressed 31 -> 42 aligned diffs and shortened the routine to 135
instructions; `-O3` regressed 31 -> 179 aligned diffs, also 135 instructions.
Changing `-inline smart` to `-inline off` on either 2.0 or 2.5 left the
instruction stream and 31-diff score unchanged. No compiler/flag variant
tested improves the color/UV cluster; retain the configured GC/2.5 `-O4,p`.
- 2026-09-25 n-misc (quicktrial aligned, base 31): EA reads r with lbzx (table base + n*4)
  and g/b/a through the row pointer. All 24 orders of {pColour, r (direct), pSrc + g/b/a via pSrc,
  pUV}: best 32. No change.

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
