# Gemini Round 4 Findings & Decompilation Analysis (2026-09-26)

## 1. Summary of Targets Investigated

During Round 4, investigations focused on four high-leverage near-miss functions to advance progress toward 100% unit matches:
1. `src/GoStaticCam.c: StaticCam_GetFlybyInformation` (99.95%, only scheduling in 1 block differs out of 313 instructions)
2. `src/startUp.c: fn_800B0748` (99.44%, final blocker for 103/103 in `startUp.c`)
3. `src/CamSpline.c: fn_800C7A9C` (98.5%, final blocker for 8/8 in `CamSpline.c`)
4. `src/MC.c: fn_8009F8C8` (down to 18 diffs, final blocker for 52/52 in `MC.c`)

---

## 2. Infrastructure Improvements: `mwccdbg.py` on macOS

- Updated `tools/match/mwccdbg.py` to ensure `/opt/homebrew/bin` is in the environment's `PATH`. On macOS, this allows `retrowin32` and other emulator dependencies installed via Homebrew to be found cleanly.
- Updated intermediate dump pattern matching to search dynamically for `*before-regalloc.txt` to handle varying compiler pipeline dump filenames.

---

## 3. Deep Dive: `GoStaticCam.c: StaticCam_GetFlybyInformation`

### Context
- Total length: 313 instructions.
- Target ROM vs current baseline: 313 instructions vs 313 instructions.
- All 313 instructions have matching opcodes and register allocations everywhere outside of a 12-instruction window (lines 262–274).

### Exact Instruction Diff Breakdown
In the second interpolation branch (`else if (fDist < fTarget)`):

**Original Target (Left):**
```assembly
260: stw      r28, 0x8(r1)        ; pCam
261: addi     r3, r7, 0x20        ; pPrev->v20
262: addi     r6, r10, 0x20       ; pAfter->v20
263: stw      r29, 0xc(r1)        ; pSub
264: addi     r4, r31, 0x20       ; pShot->v20
265: fmadds   f31, f2, f0, f31    ; fT updated directly in f31!
266: stw      r30, 0x10(r1)       ; pFov
267: addi     r5, r9, 0x20        ; pNext->v20
268: addi     r7, r7, 0x30        ; pPrev->v30
269: lfs      f2, 0x78(r9)        ; pNext->f78
270: addi     r8, r31, 0x30       ; pShot->v30
271: fmr      f3, f31             ; fT passed to float arg 3
272: lfs      f1, 0x78(r31)       ; pShot->f78
273: addi     r9, r9, 0x30        ; pNext->v30
274: addi     r10, r10, 0x30      ; pAfter->v30
275: bl       fn_800C7480
```

**Current Build (Right):**
```assembly
260: stw      r28, 0x8(r1)        ; pCam
261: addi     r3, r7, 0x20        ; pPrev->v20
262: addi     r4, r31, 0x20       ; pShot->v20
263: stw      r29, 0xc(r1)        ; pSub
264: addi     r5, r9, 0x20        ; pNext->v20
265: stw      r30, 0x10(r1)       ; pFov
266: fmadds   f31, f2, f0, f31    ; fT
267: addi     r6, r10, 0x20       ; pAfter->v20
268: addi     r7, r7, 0x30        ; pPrev->v30
269: lfs      f1, 0x78(r31)       ; pShot->f78
270: addi     r8, r31, 0x30       ; pShot->v30
271: fmr      f3, f31             ; fT
272: lfs      f2, 0x78(r9)        ; pNext->f78
273: addi     r9, r9, 0x30        ; pNext->v30
274: addi     r10, r10, 0x30      ; pAfter->v30
275: bl       fn_800C7480
```

### Key Technical Findings
1. Notice that the multiset of instructions between lines 260 and 274 is 100% identical: exactly the same registers, same immediate offsets, and same operations.
2. In Branch 1 (`if (fDist > fTarget)`), MWCC's scheduler automatically produces the order `pPrev`, `pAfter`, `pShot`, `pNext` and `lfs f2` (`pNext->f78`) before `lfs f1` (`pShot->f78`) because of latency hiding during single-precision floating point divide (`fdivs`).
3. In Branch 2, `fdivs` is at index 259. `fmadds` at line 265 in Left is scheduled exactly 5 cycles after `fdivs`.
4. The target updates `fT` directly into `f31`, which is then copied to `f3` (`fmr f3, f31`) at line 271 for the `fn_800C7480` function call. However, writing `fT += ...` or `fT = fT + ...` alters the dependency graph earlier in the block, causing the scheduler to evaluate `r4` (`pShot`) and `r5` (`pNext`) ahead of `r6` (`pAfter`).

---

## 4. Deep Dive: `startUp.c: fn_800B0748`

### Context
- Last function needed to achieve 100% completion (103/103) in `src/startUp.c`.
- Issue involves bitmask calculation:
  `lbl_802820F0 &= ~(uBit << (uAddr / 0xFE00));`

### Compiler Mechanism via `mwccdbg`
- MWCC assigns GPRs in descending order of virtual register index (`r46 -> r32`).
- Using an in-place shift assignment `uBit <<= uAddr / 0xFE00` causes the destination to be bound to `r4` (`uBit`), generating `slw r4, r4, r0` instead of `slw r0, r4, r0`.
- Using a single expression `lbl_802820F0 &= ~(uBit << (uAddr / 0xFE00))` produces the exact instruction `slw r0, r4, r0`. However, because `lbl_802820F0` is parsed and evaluated first, its virtual register receives a lower index than the division's multiplication constant (`mulhwu`), causing `r3` and `r5` to swap roles.
- Named temporary variables in C are never assigned to `r0` by MWCC (only rvalues receive `r0`). The solution requires an expression structure that triggers the division before referencing `lbl_802820F0` without assigning the intermediate shift to a named variable.

---

## 5. Deep Dive: `CamSpline.c: fn_800C7A9C`

### Context
- Target for closing `src/CamSpline.c` to 8/8 functions (100%).
- 10 instruction differences.

### Findings
- The differences originate from a fake workaround in `CamSpline.c`:
  `static const f32 aStep[1] = {128.0f};`
- This workaround hoists `128.0f` into `f27` before the double conversion constant `0x4330000000000000`, causing registers `f27` and `f28` to swap throughout the function.
- Inlining `128.0f` directly into `nStep / 128.0f` causes MWCC to perform strength reduction into `nStep * (1.0f / 128.0f)` (`fmuls`), whereas the original code requires `fdivs`.
- Preserving `fdivs` while avoiding premature hoisting of `128.0f` before `0x4330...` requires either non-constant propagation or a struct-based lookup.

---

## 6. Deep Dive: `MC.c: fn_8009F8C8`

### Context
- Target for closing `src/MC.c` to 52/52 functions (100%).

### Findings
- Systematic sweep of loop indices across all 3 nested loops proved that configuration `(18, 'k', 'nHole', 'nHole', 'k')` drops the differences to 18.
- The remaining 18 differences are entirely register allocation contention between `r22` and `r25` for pointer accumulation vs inner loop counter variables.
