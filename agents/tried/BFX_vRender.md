# BFX_vRender (goballfx.c, 0x80093AE0)

Status: SOLVED 2026-09-27 (b2, third agent): 100%, goballfx linked. Fix: `pPos = lbl_801D95C8[nPlayer];`
(plain; the earlier `(s64)` product is gone) plus three labelled fake-match assignments that leave
pPos unchanged and only add dead sign extensions (srawi) of a dead 64-bit high word: after store [3]
`pPos = (f32*)(s32)((u64)(u32)pPos | (((u64)(s64)(s32)lbl_801D9578[nPlayer] >> 32 |
(u64)(u32)lbl_801D94D8[nPlayer]) << 32));` and the same with `nPlayer * 4 | (s32)((s64)nPlayer >> 32)`
/ `nPlayer`, and after UV store [0] the same with `nPlayer` / `nPlayer`. See the first attempt below.

Read all of this before working on the function. Do not repeat an attempt listed here
unless you combine it with something new. Before you stop, add every attempt under
"Attempts" (what, score before -> after). When it is exact: Status SOLVED, the fix, the commit.

## Attempts

- 2026-09-27 b2 (third agent; quicktrial aligned, 16 at start; scripts or*.py, combo*.py, whatif*.py,
  ph_model.py, degturn.py in the lane scratch). **Mechanism facts (dumps):** the register
  allocator's liveness skips a dead instruction entirely (its uses do not extend anything) EXCEPT
  `srawi` (it sets XER[CA], so it is never "dead"): a dead srawi interferes with everything live
  across it AND keeps its source live up to it; `or`/`rlwinm`/`mulhw`/`li` with a dead result get 0
  neighbours (GameMode22's OR works through GLOBAL liveness across its loop, not inside a block).
  So a dead `or` feeding a srawi IS live: `srawi(a | b)` keeps both a and b live to the srawi, and
  both are deleted after allocation. Placing it: `pPos = (f32*)(s32)((u64)(u32)pPos | (((u64)(s64)(V)
  >> 32 | (u64)(u32)(P)) << 32));` (low word = pPos, logic unchanged; the (s64)(V) is srawi V).
  Frontend CSE temps (@N) are coloured after all backend temps, so a `(s64)nPlayer` shared with the
  pPos product (hi word = srawi nPlayer = "X0") is coloured late, after the bytes: the only way to
  get a value into r9 for n*32.
  Results: (1) V = `(s32)lbl_801D9578[nPlayer]`, P = `lbl_801D94D8[nPlayer]`, after store [3]: 16 -> 9
  (the red/UV instruction order is EA's). (2) + `(u8)(s32)(s64)(s32)` on store [2]'s byte and
  V = `nPlayer * 4`, P = row[0] after store [15] + `lbl_80189CB0[(s32)(s64)nPlayer][0]` on store 12:
  3 (only n*32 r7 vs r10 left). (3) chain V = `(nPlayer * 4 | (s32)lbl_801D94D8[nPlayer]) |
  (s32)((s64)nPlayer >> 32)` (keeps n*4 and X0 live past the UV add): EVERY volatile register is
  EA's (blue r8, n*32 r10, X0 in r9 and deleted) but the saved registers permute (pBall r28, pPos
  r29, pColour r31): 43. Minimal form (m2: C1 of (1) + V = `nPlayer * 4 | (s32)((u64)(s64)nPlayer
  >> 32)`, no store round trips): 43, same. Why (rasim replay + what-if): pColour and pUV must leave
  the graph in the FIRST simplify sweep (<= 28 neighbours at their turn) so pPos/nPlayer/pBall are
  coloured first; the dead-op temps live across pUV's def push pUV to 30 (needs 2 fewer: e.g. the
  chain's or+srawi or C1's srawi scheduled before the UV add). Tried for that: chain duplicated
  (no frontend CSE of or-expressions), chain held in locals (copy-propagated away), chain reading
  the UV base instead of the row (still scheduled after the UV add), 256 chain spellings (atoms
  n*4, UV row/base, X0 signed/unsigned, n*32, bytes; `|`/`+`): best 40/43; desc.pColour/pUV
  spellings and desc statement orders (720 sampled): 43+.
  **The fix (exact, e3 in scratch early.py):** drop the `(s64)` from the pPos product (plain
  `lbl_801D95C8[nPlayer]`), put the chain statement (signed form `(s32)((s64)nPlayer >> 32)`)
  right after C1 (so it is early in block order and the scheduler issues its or/srawi before the UV
  add: they stop being pUV's neighbours), and give `(s64)nPlayer` its second occurrence late with
  a third dead statement after UV store [0] (V = P = `nPlayer`), which keeps X0 a late-coloured
  frontend temp (@ numbers follow the order of each expression's SECOND occurrence). Every register
  is EA's; the only listed difference left was the aIndex `@13+4` relocation, which links fine.
  Each statement is needed (dropping any one: 3-11 wrong registers). Metric used for the search:
  aligned diff where register-only differences count as distinct renamings (regmetric.py).

- 2026-09-27 b2 (second agent; quicktrial aligned). **Colours copied straight between the globals,
  as sibling GoObjShadow fn_80093DB8 writes its arrays: `lbl_801D9578[nPlayer][k] =
  lbl_80189CB0[nPlayer][k & 3]` x16, no rgba / pColour locals, UV direct, `desc.pColour =
  lbl_801D9578[nPlayer]`, pPos with the s64 product: 20 -> 16, real 90.13 -> 97.80 (applied).**
  The four bytes become frontend CSE temps (@11-14) coloured after the n*4 / n*16 temps, which
  fixes table base r6, n*4 r7, pColour hi r5, n*16 r8, red r5, green r6, alpha r4. Left: blue r7
  (EA r8), n*32 r8 (EA r10), and 4 instructions of the red/pColour/UV cluster in a different order.
  Same as a `for (i = 0; i < 4; i++)` loop with `[i * 4 + k]` (16); `i += 4` loop 69; pColour local
  51+; UV before the colour stores 47-48; pPos plain 25, `(u64)` 22, `(f32*)base + (s64)n * 12` 39.
  64-bit index spellings on the UV row / pColour / colour row (`(s64)n * 32`, `<< 5`, `(u64)`,
  `[(s64)n]`, per-site or all sites) on the 20 base: 25-80. One `(u32)(s64)(s32)nPlayer` round trip
  on any single `[nPlayer]` index, or `(u8)(s64)(s32)` on one loaded byte, on the 16 base, UV block
  at 4 places: 16 best (none), the rest 47-120 (breaks the frontend CSE).
  On the 16 base, also no gain: GXColor-typed source and/or destination tables (16 each); a subset
  of the bytes in locals, every declaration order (20-30); per-function pragmas (scheduling off/
  once/604/7400, opt_propagation/common_subs/dead_assignments off 32-79, opt_lifetimes/
  loop_invariants/strength_reduction/dead_code/unroll off and optimization_level 3: all 16,
  level 2 54, level 1 331, peephole off 114); 64-bit spellings (`(s64)n << k`, `* N`, `(u64)`,
  `(s32)(...)`, `[(s64)n]`, round trip) on the UV / pColour / colour-table row at all, first,
  second or last use (22-107).
  **Why the last two registers are hard (model, scratch ea_*.py, s750f.py, s750p.py):** the
  allocator colours in descending vreg order, lowest free register. On this base the order is
  backend temps, then O4 (n*4) @9, O16 (n*16) @10, R/G/B/A @11-14, O32 (n*32) @15. For blue to take
  r8, a value in r7 must be live at blue's load and coloured before it; the only r7 value in EA's
  code is n*4, whose last use is red's lbzx, which precedes blue's load in every schedule we get.
  For n*32 to take r10, r7, r8 and r9 must be live somewhere in its range; nothing in EA's code is
  ever in r9, so EA's pre-allocation code had a value there that vanished (a dead srawi). An
  exhaustive replay (current pre-RA order, all positions and colouring slots): no single point
  phantom, no two point phantoms (also with any one instruction moved) give EA's registers; two
  phantoms WITH live ranges would (12k placements), but a dead srawi is a point: its consumer
  (e.g. the dead `rlwinm hi,5` of `(s64)n << 5`) is deleted before liveness, so it only interferes
  with values live across its def (checked: dump of `(s64)nPlayer << 5` on one UV store, srawi
  r39 @15 has 7 neighbours, none defined after it). A dead srawi DOES extend the live range of the
  value it reads (a round trip on n*4 keeps n*4 live to the srawi): the model finds blue right with
  one such srawi on n*4 placed after blue's load, but n*32 still needs a point phantom in r9 inside
  its short range while r0, r3..r8 are live, which none of the searched schedules has. With a free
  pre-RA order the nearest solutions move the pPos `add` after the colour/UV address work (a
  schedule the first pass never makes: the add is on the critical path). The first-pass model
  (sched750 + an FPU unit, FP ops complete after 3 cycles, alias-aware memory edges: constant-pool
  loads free, distinct globals unordered, pBall loads 'unknown'; the dump's B8 + B9-before-the-call
  are one block to the scheduler) reproduces every GPR instruction of B8's real first pass on 7
  dumps (only the stb / UV-stfs interleave differs). In that model dead srawis land at the block's
  end (low height), extending their sources to the end.
  What WOULD give EA's registers on today's pre-RA order (model, ea_src.py / ea_src3.py): (a) three
  dead srawis inside n*32's range right after its rlwinm, two reading n*4 and one reading the UV
  `lis` (or the pColour `addi`), the first coloured after the bytes and before n*32 (it takes r9);
  or (b) `li r3,0x70` moved up to just after the UV `addi`, then two srawis: one reading any
  live-through value inside n*32's range (r9) and a later one reading n*4. Neither is reachable:
  in the first-pass model every single srawi (any source, any input position: 24 distinct
  schedules) and every pair (1,152 distinct schedules, ea_pipe4.py) leaves blue and n*32 wrong,
  and the srawis always issue after the UV `add`. A real `(s64)(s32)` round trip on the n*4 offset
  breaks the CSE of the colour row (flat / row-pointer spellings with the round trip: 63-76).
  Checked for decomp-notes (dead srawi deletion): in the dump of `r = lbl_80189CB0[(s64)nPlayer][0]`
  (scratch dump_t1) `srawi r0,r30,31` is the block's last GPR def (nothing later in B8 writes r0;
  B9 is the call) and backend-14 (post-RA peephole) has no srawi: the "later overwrite in the same
  block" rule is incomplete; a register dead at the block's end (here r0 before a call) also
  lets it go.
  Next step if anyone continues: find a C form whose first pass issues `li r3,0x70` (the call
  argument) or a round trip's srawi before the UV `add` (s750p.py predicts a variant's first pass
  from a mwccdbg dump in seconds; ea_dump.py / ea_pipe3.py score the allocation).
- 2026-09-27 b2 (quicktrial aligned; current source 31, all four colour bytes read direct
  `lbl_80189CB0[nPlayer][k]` = "DIRECT" 32). Scripts in the lane scratch (bfx*.py, climb*.py,
  phsim*.py). Two real levers found, one wall:
  1. **The schedule is fixed by a dead 64-bit high word on pPos**: `pPos = (f32*)((u8*)lbl_801D95C8
     + (s64)nPlayer * 48);` with DIRECT reads -> 26, and every remaining difference is a register:
     the instruction ORDER is EA's (pUV `lis` late, colour loads interleaved, lbzx for red). The
     `li 0x30; mulhw; li 0` of the high word joins the pre-RA schedule and is gone before
     allocation (0 neighbours). Spellings `nPlayer * (s64)48`, `(s64)nPlayer * (s64)sizeof(row)`,
     `* 48u`, `* 12 * 4` all the same; `lbl_801D95C8[(s64)nPlayer]` 42 (srawi, pUV to r27); s64 on
     pColour / pUV / the colour index alone: 42-46. The same s64 on the committed pSrc form: 27.
  2. **pUV written straight into the global, as GoObjShadow fn_80093DB8 does**
     (`lbl_801D94D8[nPlayer][k] = ..`, `desc.pUV = lbl_801D94D8[nPlayer]`, no pUV local) on top of
     1: 20. n*32 becomes a frontend temp (coloured late); row, b, a get EA's r4/r8/r4. pColour
     direct as well: 29 (34 with pPos too); pPos direct would need the s64 at each store (not tried).
  3. **The wall: EA's n*32 is r10 and r9 is never used anywhere in the function.** In every graph we
     produce (mwccdbg + rasim-style replays of the 26 and 20 variants), n*32's neighbours in EA's
     colours hold r0,r3,r4,r5,r6,r8 (+r7 if its rlwinm is scheduled before the lbzx), so r10 needs a
     neighbour holding r9 that leaves no instruction: a dead value allocated, then deleted.
     Replays with one or two phantom neighbours (any live set) plus renumbering reach 1 wrong
     (n*32 only), never 0. Dead `srawi`s from `(s64)(s32)` round trips (on a loaded byte or a
     pointer) ARE deleted after allocation here, even in r0 with no later write in the block
     (dump of the g/a variant), so they work as phantoms; but a srawi takes the lowest free register at
     its def, and r9 needs r0,r3..r8 all live at one point. None of our pre-RA schedules has such a
     point. Lead: before pPos's `add` (pPos lo r0, mulli r5, pUV lis r3, table r4/r6, off r7, n*16 r8)
     would give it, if the pre-RA scheduler put the colour-table/pColour address work ahead of that add.
  Other results: statement order of pColour=/rgba/pUV= (1,000+ placements): 32 on DIRECT, 26 on 1,
  20 on 2 (CW reorders freely); 24 rgba orders x declaration orders: no gain; colour loop forms 26-30;
  local uv table + loop 111; per-function pragmas on the 26/20/12 bases (scheduling once/750/7400/603,
  opt_propagation/common_subs/dead_assignments/lifetimes/loop_invariants/strength_reduction off,
  peephole off, optimization_level 3): never better, `scheduling once` 142; pSrc row forms on 2: 27-28;
  64-bit adds (`(s64)(s32)base + ..`) leave addc/adde (53-90). Random climbs over (s64) / (s64)(s32) /
  (s64)(s8) round trips on the 7 index/value sites + declaration + rgba order (7 climbs, ~10k compiles)
  stall at 12 aligned (every byte store right; left: table base r4 vs r6, off r8 vs r7, n*16 r6 vs r8,
  row r7 vs r4, n*32 r4 vs r10, pUV's add one slot early), with 4-6 casts: not a credible fake, not
  applied. pPos written direct with the s64 at every store 55 (68 with a plain desc.pPos); direct
  without s64 32. Nothing changed in src.

- 2026-09-26 e-render: permuter 20 min -j 3 (7.1k iterations; machine load ~27): best 1340,
  1385, 1445: all `volatile int` temps for one colour byte (not usable). Nothing else.

- 2026-09-26 e-render (aligned, base 31): red read through a flat index the frontend might not
  match with the row (`lbl_80189CB0[0][nPlayer * 4]`, `((u8*)lbl_80189CB0)[nPlayer * 4]` /
  `[nPlayer << 2]` / `* sizeof(row)`, `*((u8*)tab + nPlayer * 4)`, through `(u32)` arithmetic)
  x g/b/a (row pointer for all three, g direct + row for b/a, all direct) x red before/after:
  best 31 (current); still CSE'd.

- 2026-09-26 r6-misc (quicktrial aligned, base 31): void* casts on the row (`pSrc = (u8*)(void*)
  lbl_80189CB0[nPlayer]` for all four 125, red direct + void* row for g/b/a 32, red through a
  void* row and g/b/a direct 32, `*(u8*)(void*)row` for red 32/31); red read at every statement
  position from before the first call to after pColour (direct g/b/a 34-79, pSrc b/a 37-80). The
  frontend still CSEs red's address with the row.

- 2026-09-26 r5-render: frontend-02 shows the frontend CSE temp @19 (`lbl_80189CB0 +
  nPlayer*4`) formed at red's read and reused for b/a (and the backend CSE folds g's add into it),
  so red's address has several uses and never becomes EA's `lbzx`. EA needs red's add to stay
  single-use until peephole-forward (pass 01, before CSE). Red spelled `[0]`, `*row`, flat
  `((u8*)tab)[n * 4]`, `pSrc[0]`, `*(u8*)&row` x g/b/a via pSrc or indexed x red first / after
  `pSrc = row` / last: all 32 (best 32 of 30 variants).

- 2026-09-26 r5-render (aligned, base 32): EA reads red with `lbzx r5,r6,r7` (table base +
  nPlayer*4, its own address) and g/b/a with lbz 1/2/3 off `add r4,r6,r7`, i.e. red's address is
  not CSE'd with the row. Tried: red direct + pSrc for g/b/a (both orders) 32, `r = *row` 32,
  `pSrc = &row[1]` for g/b/a 27 (real not checked: the colour loads' registers still differ),
  `pSrc = &row[2]` after g 31, all four direct 32, pColour after the reads 38, `*pSrc++` 125.
  (The aIndex `@4+0x4` vs lbl_80283C4C difference is only the target's label split.)

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
- 2026-09-26 r4-render (aligned, base 31): random dependency-keeping moves of the 45 statements
  from `pPos = ..` to the last UV store (5 min climb): 28, but only as a scramble of the pPos /
  colour / UV stores (not EA-like; not applied, real not measured).

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
