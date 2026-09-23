Decompilation notes: CodeWarrior and GameCube
=============================================

General lessons that apply to any CodeWarrior / GameCube matching decompilation.
Game-specific knowledge goes in [`tw2004-notes.md`](tw2004-notes.md).

Each entry says where it came from:

- **[verified]** - we tested it ourselves and saw the result.
- **[observed]** - we saw it happen but have not tested the rule in isolation.
- **[other project]** - learned while working on another decomp (Metroid Prime, CodeWarrior 1.3.2, C++).
  May not carry over unchanged.

Add to this file whenever something costs more than a few minutes to figure out.

How matching works
------------------

- The goal is byte-identical output. The build compiles our C, links it in place of the original
  machine code, rebuilds the whole game, and compares the SHA-1 with the retail `main.dol`.
- If the bytes match, behavior is identical by definition. We can never know whether the C is
  what the original programmers typed; many spellings compile to the same bytes.
- Anything not yet decompiled is linked from the original binary, so any function can be worked
  on at any time. A roadmap is a priority order, not a dependency chain.
- The build re-verifies every matched function on every run. A wrong setting cannot silently
  corrupt anything; it shows up as a function that stops matching.

Evidence and false positives
----------------------------

- **[verified] The wrong compiler can produce the right bytes.** MSL `mem_funcs.c` matched this
  game only under a compiler built two years after the game shipped. Different source plus a
  different compiler landed on the same bytes. If a result is impossible (dates, versions),
  treat it as a red flag, not a puzzle to explain away.
- **[verified] Most code cannot tell compilers apart.** Across 1,765 functions, only about 4%
  compiled differently between neighbouring CodeWarrior versions. A match on a simple function
  is weak evidence. Only functions where candidates disagree carry information.
- **Rules for a fair compiler test:** write the natural C once; compile it unchanged on every
  candidate; do not tune the C per compiler; count only functions where the candidates differ;
  test several different kinds of code.
- **[verified] Calibrate a scanner before trusting "zero hits".** Run any pattern search on code
  you compiled yourself with each candidate first. Our first fingerprint scanner had a bug that
  made it blind; calibration caught it. Zero hits can also mean the programmers simply did not
  write that kind of code.
- Keep a list of things that looked like evidence but were not (see `compiler.md`).

CodeWarrior versions (GameCube)
-------------------------------

| Package | Compiler | Built |
|---------|----------|-------|
| GC/1.2.5 | not checked | not checked |
| GC/1.3.2 | 2.4.2 build 81 | May 2002 |
| GC/2.0 | 2.4.7 build 92 | Sep 2002 |
| GC/2.5 | 2.4.7 build 105 | Feb 2003 |
| GC/2.6 | 2.4.7 build 107 | Jul 2003 |
| GC/2.7 | 2.4.7 build 108 | Jul 2004 |
| GC/3.0a3+ | 4.1 | Dec 2005 onward |

- **[verified]** `mwcceppc.exe -version` prints the real build and date. Check dates against the
  game's release date.
- **[verified] Function opening (prologue).** GC/1.2.5 and older: `mflr r0; stw r0,4(r1); stwu r1,-X(r1)`.
  GC/1.3.2 and newer: `stwu r1,-X(r1); mflr r0; stw r0,X+4(r1)`. Counting styles across a binary
  separates "1.2.5 or older" from "1.3.2 or newer" without writing any C.
- **[verified] Float copies.** Copying three floats: GC/1.3.2 loads and stores one at a time;
  GC/2.0 - 2.7 interleave (load, load, store, load, store, store); GC/3.0 loads all then stores all.
  The interleave needs a `const` source: `Vec3Copy(f32* pSrc, f32* pDst)` gave load/store in turns
  and the linked DOL failed; `(const f32* pSrc, f32* pDst)` matched (code_800082F8 `Vec3Copy`).
- **[verified] Null pointer compare.** GC/2.x and older use an unsigned compare (`cmplwi`), GC/3.0 a signed one (`cmpwi`).
- **[verified] GC/2.0 vs 2.5+.** Differ when a byte-sized value is masked and then used as an
  index, and in bit-field packing. GC/2.5, 2.6 and 2.7 never differed from each other in our tests.
- **[verified] No help:** switch statements, 64-bit integer math, simple compare loops.
- **[verified]** Under `-proc gekko`, every version from GC/1.3.2 up saves callee-saved float
  registers twice in the prologue (`stfd` plus `psq_st`). This is not vector math.
- **[verified]** `-O4,p` (speed) and `-O4,s` (size) produce different code for loops, identical
  code for straight-line functions.
- **[verified]** CodeWarrior does not generate paired-single (vector) math from plain C. Real
  paired-single math in a binary is hand-written assembly or intrinsics.

Reading compiler output
-----------------------

Rules for reading CodeWarrior output while matching, grouped by topic. Start with the checklist.

### Try these first

The fixes that come up most often. Each points to its full entry below.

- **Every float constant: read its hex from the original's data and write the exact expression
  EA wrote.** EA writes fractions and unit conversions, not decimals: `1.0f / 72.0f` (0x3C638E39),
  `59.94f / 60.0f` (0x3F7FBE76), `0.92f / 36.0f`, `1.0f / 65536.0f`, `DEG(x)`, `PI / 180.0f`. A
  decimal like `0.0138889f` is a few bits off. objdiff masks constant values, so the function
  still reads 100% while the value is wrong: a real parity bug in a port, and a common reason a
  unit won't link. Decimals are right only when the hex round-trips exactly (0.5, 0.25, 20.0).
  `tools/match/constcheck.py` (when present) lists every mismatch and suggests the fraction. How
  the expression is written can also change the code (`x * (1.0f/65536.0f)` gives `fmuls`,
  `x / 65536.0f` gives `fdivs`). See [Floating point](#floating-point).

- Only register numbers differ: swap the declaration order, then try moving initializers out of
  the declarations. See [Registers](#registers-declaration-order-and-the-stack).
- Register numbers still off after reordering: make the loop counter `long`. See
  [Registers](#registers-declaration-order-and-the-stack).
- An unexpected `clrlwi`/`extsb` before a call: cast at the call site, or change the prototype's
  parameter type in this file. See [Calls](#function-calls-and-parameters).
- Only stack offsets differ: swap the declarations (stack locals go in reverse declaration
  order). See [Registers](#registers-declaration-order-and-the-stack).
- A `+= const` is folded in one build and in place in the other: switch the local between `int`
  and `s32`. See [Types](#types-casts-and-sign-extension).
- `lwzu`/`stbu`/`lbz 0(rN)` on a player field: write `gPlayers[n].field` out each time, no
  pointer local. See [Structs](#structs-arrays-and-pointers).
- A player loop with separate base and offset registers: use the `PLAYER(i)` byte-offset form.
  See [Structs](#structs-arrays-and-pointers).
- A loop over a global array: index it (`gTable[i].x`), don't walk a pointer. See
  [Structs](#structs-arrays-and-pointers).
- Runs of identical stores: write the loop, not the stores. See [Loops](#loops-and-unrolling).
- `bne`/`beq next; b end` where one branch was expected: a one-case `switch` with
  `default: return`. See [Returns](#returns-early-exits-and-switch).
- Several tests branching to one shared return: combine them, `if (a || b || c) return;`. See
  [Returns](#returns-early-exits-and-switch).
- A float compare with the wrong branch shape (`cror` vs a plain `blt`/`bge`): match the source's
  operator, e.g. `!(x < c)`. See [Compares](#compares-and-conditions).
- The wrong product fused into `fmadds`: compute the addend into a local first. See
  [Floating point](#floating-point).
- Integer registers off around an array read, or a small expression repeated: try a one-line
  `static inline` helper. See [Inlining](#inlining-and-inline-helpers).
- The original has a helper's body pasted in: write the body out (no automatic inlining in game
  code). See [Inlining](#inlining-and-inline-helpers).
- Call order looks wrong around a comparison: the right side is evaluated first. See
  [Evaluation order](#evaluation-and-statement-order).
- Every function at 100% but the DOL hash fails: check the unit's data sections, `.sdata2`
  constants and `splits.txt`. See [Data](#data-constants-and-symbols).

### Loops and unrolling

- **[verified] Two tests on players n and n + 1 can be a two-pass loop.** When the second
  address is `off + 0xEF8` added to the first, the source was `for (i = 0; i < 2; i++)` with an
  `s32 i` (unrolled); `int i` recomputes `(n + 1) * 0xEF8` (91.5%). Golfer `Team_IsAllCPU`,
  `Team_IsAllHuman`: 78.95% -> 100.
- **[verified] CodeWarrior -O4,p loop shapes.** A byte/halfword copy loop written as
  `while (n > 7) { eight explicit copies through temporaries; p += 8; n -= 8; }` followed by
  `while (n--) *d++ = *s++;` comes out as: count = n >> 3 into `mtctr`, the block unrolled 2x with
  all eight loads before the stores, `n += -(count << 3)` after, then the remainder loop
  auto-unrolled 8x. Interleaved load/store pairs mean the copies did *not* go through
  temporaries (the compiler assumed aliasing). A `do { } while (--n)` is *not* unrolled at all.
- **[verified] Unrolled loops: the guard tells you the counter's type and the bound's form.** CW
  unrolls counted loops and puts a guard in front. With an `int` counter and a constant bound the
  guard folds away; declared `s32` (a `long`) it stays as `li rX, 0; cmpwi rX, N; bge`
  (skalib.c `AnimLib_BuildCb`, `Skalib_Init`). With a variable bound, `for (i = 0; i < p->n; i++)`
  inside an `if` followed by more code gives `cmpw i, n; bge`, while the original's
  `cmpwi n, 0; ble` came from the bound held in a local or written `p->n > i`
  (`Skalib_SetBudgets`, `AnimLib_FreeWorkCopies`). Tested in isolation in
  `C:\dev\scratch\tw\looptest\`.
- **[verified] Runs of identical stores are unrolled loops.** CW at -O4 fully unrolls small
  constant-count loops (10 link checks, 12 attribute stores, 19 flag bytes - the last as 18
  stores plus a one-iteration remainder loop) and unrolls a 34-iteration loop by two. Writing
  the loop, not the stores, is what matches (`AI_TargetsLoad`, `Golfer_TableSetup` exact).
- **[verified] `while (i < N) a[i++] = v;` and `for` unroll differently.** Clearing an 18-entry
  array (GameMode2 `fn_800F8B08`), the `while (h < 18)` form unrolls by 9 and the `for` form by
  6; count the stores per iteration to tell which one EA wrote. Likewise a five-player reset in GameMode16
  `fn_800F57C8` matched only as `i = 0; while (i < 5) gPlayers[i++].nDC0 = 20;`, not as a `for`
  or a pointer walk.

### Registers, declaration order and the stack

- **[verified] A value computed before a call and kept in a saved register gets its own local.**
  `nMoney *= 2;` puts the shift after the following call (58.5%); `s32 nPaid = nMoney * 2;` is
  exact (Earnings `fn_800D39B4`). Reusing locals for a second value swaps float registers
  (Earnings `fn_800D6A70`, 99.9% until separate locals).
- **[verified] Locals take the higher volatile registers in declaration order; compiler
  temporaries take the lower ones.** In GetHonors' sort all 5040 orders of 7 variables gave two
  outcomes: only the relative order of `nScore` and `nHigh` mattered.
- **[verified] Declaration order picks the saved registers.** Register order for callee-saved
  locals follows declaration order (first declared gets r31). Two loop counters in fn_8005A0FC
  came out swapped (r27/r28) until their declarations were swapped. Try this first on any diff
  that is only a register permutation. Parameters used as working pointers come after the
  locals; to make `pEnd` r31 and the destination r30, declare `pEnd` first and copy the
  parameters into locals declared after it. A `u8 v = (u8)value` local declared *before* the
  pointer local gets the `clrlwi` in place.
- **[verified] Register numbering, declared-first-highest in some functions.** In fn_8005A850 the
  earliest-declared local took r30 and later ones descended; one `s8` (instead of `u8`) on a
  colour byte fixed the last permutation. When the lowest-first order does not help, try the reverse.
- **[verified] The declaration-order rule for callee-saved registers does not hold in
  `AI_ChooseTarget`** (0x8002C2DC, 20 live variables, a loop with eight calls): no ordering tried
  reproduced the original assignment, and hand-hoisting values into named locals made it worse.
  The function is left at 92.7% with only register numbers differing. Something else (spill
  cost, loop weight) drives allocation once a function is this large.
- **[verified] An initializer in the declaration changes register assignment.** `f32* p = &x;`
  at the top of the function got r28 while the same pointer declared bare and assigned on the
  next line got r31 (the original). When a function is otherwise exact but the callee-saved
  numbers are off, try moving initializers out of the declarations (and vice versa).
- **[verified] A parameter can be copied to a local.** `ClipBank_Load` kept its data pointer in the
  lowest saved register only when the parameter was copied into a local declared last.
- **[verified] One variable, two jobs.** When the original reuses one FPR for two unrelated values
  (a distance, later a blend step), the source reused one local. A local reused as a temporary
  can be the reason for a register mismatch: the original stored a sqrt into the distance local
  before scaling it (`Ball_Tick`).
- **[verified] Stack locals go in reverse declaration order** (the last declared gets the lowest
  `r1` offset), so the order locals are declared in changes where they sit on the stack.
  Symptom: a function at ~99% where the only differences are stack offsets swapped between two
  variables. Fix: swap the declarations. Twelve 16-byte vectors in fn_8005A0FC matched once
  declared highest-address first.
- **[verified] Taking a parameter's address puts it on the stack at the parameter's slot**;
  copying it to a local first gives a different slot order (fn_800E4164, fn_800E53F0).
- **[verified] An initialiser placed after early exits is in an inner block.** A local array copied
  from `.rodata` only after the function's first checks means `if (ok) { u32 aPad[4] = {0}; ... }`,
  not a declaration at the top (`AnimLib_MergeOverlay`).
- **[other project]** Register choice rules seen on CodeWarrior 1.3.2: a temporary goes in the
  lowest free register; a variable that is modified in place keeps its own register; a value
  used once gets folded into the instruction that uses it. A single wrong register with all else
  right can be extremely hard to fix and may need a different way of expressing the same logic.
- **[verified] A `long` loop counter can fix the register order.** `fn_800F6ED4` (GameMode13)
  stayed at the same score with every declaration order; the permuter found the fix: the inner
  counter declared `long j` instead of `int j`. Try `long`/`s32` on counters when register
  numbers are off and reordering does nothing.

### Evaluation and statement order

- **[verified] A three-term sum is reordered by the compiler.** `return c + a*10000 + b*1000000;` adds
  the products first in every term order; `int n = c; n += a * 10000; n += b * 1000000; return n;`
  matched (FE_Manager `fn_80078604`).
- **[verified] Call results as arguments.** `f(g(), p->x, p->y)` loads the other arguments first and
  keeps them in saved registers; `t = g(); f(t, p->x, p->y);` calls first (FE_LogoDesign
  `fn_8010FAF4`, 76.2 -> 100).
- **[verified] Shift the parameter once.** `(s16)(n >> 1)` written at three uses scored 90.7;
  `n >>= 1;` then plain uses gave the original's `extsh` then `srawi`/`extsh.` (startUp `fn_800AFF9C`).
- **[verified] A pointer local to a global struct**, `T* p = &gX; p->a++; ...`, can be what the
  original did even though the address could be reused anyway (PGATour `fn_800EE2C8`, 80.5 -> 100
  with its other locals).

- **[verified] The right side of a comparison is evaluated first.** `f(0) < f(1)` calls `f(1)`
  first. So the call order in the original tells you how the comparison was written: m2c's
  `t = f(1); if (f(0) < t)` is the source `f(0) < f(1)` (GameModeAlternateShot, 2026-09-23).
  Additions go left to right, so `a = f(x) + f(y)` calls `f(x)` first.
- **[verified] `a()->arr[b()].f`** evaluates `b()` first and scales the index before calling
  `a()`; splitting `b()` into a local first moves the shift after the call.
- **[verified] `(fn() & uMask)` operand order.** `and. r0, r3, rM` (call result first) comes from
  the mask call inline: `if (fn_800136DC(x) & fn_800142AC(k, m))`. A `uMask` local assigned
  first gives `and. r0, rM, r3`, and swapping the operands in the source changes nothing. Applied
  to every single-use mask in Swing.c (States 04/05/08/09/10 exact, 06/12/14/22 closer).
- **[verified] Statement order among plain stores matters**: `a[i] = 1; x = 0; y = 0;` and
  `x = 0; a[i] = 1; y = 0;` give different register use (fn_800E4238). And a call made for an
  argument that was never used (`fn_800E5DA0(lbl)` vs `fn_800E5DA0()`) changes the code.
- **[verified] Chained assignment stores backwards.** `a[0] = a[1] = a[2] = 0` stores 2, 1, 0;
  the original wrote four statements in order.
- **[verified] ...but a chain gets its zero register first.** A run of zero stores whose only
  difference is the loop's volatile registers (the zero in `r7` instead of `r4`, pointers shifted
  by one) matched as one chain written last field first, so the stores still come out in field
  order: GameMode2 `fn_800F8880`, 96.9% -> 100 (in field order the chain scores 94.8). The
  permuter's 30 exact variants all made the zero one shared value. It is per function, not EA
  style: the same seven stores in `fn_800F8B08` and GameModeMatch `fn_800EA548` match only as
  separate statements. Mark the chain as a fake match.
- **[verified] Nested call arguments are evaluated last argument first.** In
  `f(g(), Game_CurHoleIndex())` the hole index is fetched before `g()`. Writing it into a local
  first keeps the same call order but a different saved register (GameMode2 `fn_800F8EDC`,
  99.93% -> 100 with the call inline).
- **[verified] A call result used once stays inline.** `n = f(); x = 0; y = 0; p->a += n;` and
  `p->a += f(); x = 0; y = 0;` schedule the same, but the local changes the scratch registers
  (GameMode2 `fn_800F8EDC`, 99.56% -> 99.93).
- **[verified] A table read before its range check was a local.** When the original loads
  `table[n]` before testing `n`, the source read it into a local at the top:
  `u16 nSound = lbl_80192BA8[nEvent]; if (nEvent >= 37 || nSound == 0xFFFF) return;` (GameMode8
  `fn_800FAA70`, 71.7% -> 93.9, then exact with the or-chain). Check the table is big enough for
  every index the callers pass; if not, it is an EA bug and gets a comment.

### Structs, arrays and pointers

- **[verified] The `(u32)` index habit applies to other global arrays too:** a loop over
  `gSwingStacks` with a separate base and 6-byte offset is `gSwingStacks[(u32)i]` (Swing
  `fn_8005CD94` 84.2% -> 100, `GOLFERSTATE_Update` 86.6% -> 100).
- **[verified] Other global arrays follow the gPlayers rule.** `gSwingStacks[n].nState[
  gSwingStacks[n].nTop]` written out each time, not `SwingStack* p` / `s8* pTop` locals (Swing
  `GOLFERSTATE_Pop` 98.53% -> 100; the permuter found it).
- **[verified] A load the original does before a store to the same struct was a local.**
  `n = p->n60; p->f68 = 0.0f; fn(n, p);` keeps the load first; reading `p->n60` in the call
  moves it after the store (GoGolfCam `fn_800C0880`, 94.6% -> 100).
- **[verified] A typed struct-pointer global indexes differently from a cast byte pointer.**
  `extern Profile* gpSaveData; gpSaveData[n].f` gives `addis base; add; lwz off`;
  `((Profile*)u8ptr)[n].f` gives `addis idx; addi; lwzx` (GameMode23 `fn_800F0428` 83.75% -> 100).
- **[verified] `a[x - 1]` folds the -1 into the displacement; `n = x - 1; a[n]` keeps a `subi`.**
  Per function: GameMode23 `fn_800EE064` needs the local, its neighbour `fn_800EFA9C` does not.
- **[verified] Pointer-to-index with `mulhwu` is a byte difference divided by `sizeof`.**
  `p - base` divides signed (`mulhw; srawi`); `((u8*)p - (u8*)base) / sizeof(T)` divides
  unsigned (`mulhwu; srwi.`), because `sizeof` is unsigned (GoTerrainCollision `fn_80050BEC`,
  82.69% -> 100).
- **[verified] In a leaf loop, reading a field each time instead of a local copy moves the
  volatile registers**, though the field is still loaded once (GoTerrainCollision
  `Ter_CalcLowestPlayableWorldHeight`, 97.21% -> 98.69%, then exact by declaration order).
- **[verified] `a[k] = x; k++;` and `a[k++] = x;` compile differently.** The split form gives
  walking pointers (`&a[k]` stepped by `addi 4`, and `&a[0]` kept for a later loop); `k++` in the
  index gives `stwx` with a scaled index; a `*p++ = x` walk gives one pointer. GameMode0
  `fn_800FF894`: the split form took it from 94.9% to 99.6%.
- **[verified] The `lwzu`/`lfsu` idiom is a repeated field access, not a pointer local.** When the
  same `gPlayers[n].field` is read again after a call, CodeWarrior makes a pointer to the field
  itself and folds the first read into `lwzu rD, off(rP)`; later reads are `lwz rD, 0(rP)`. An
  explicit `s32* p = &gPlayers[n].field; ... *p` local blocks that shape (`addi rP; lwz` from the
  base). Write the field access out each time. Applied to 33 functions of `Swing.c` in one sweep
  (`C:\dev\scratch\tw\ptr_sweep.py`), 10 became exact. Exception: pointers that are indexed
  (`pViews[k]`) or passed as pointers stay pointers. Likewise `lwzu` / `stbu` / `lbz 0(rN)` on a
  player field means EA repeated `gPlayers[n].field`; a local `Player* p` or `s8* pField` gives
  `lwz 0xOFF(rN)` instead, and the add comes out in a different place (GameMode10 fn_800F1ABC,
  fn_800F1B60, fn_800F21B4).
- **[verified] One function can use both.** When the original keeps `&gPlayers[n]` in one saved
  register for the first statements and computes the player offset afresh after the calls, the
  source used a `Player* p` for the first part and `gPlayers[n].field` at the end (GameMode8
  `fn_800FA9E0`, 77% -> 100; `p` throughout gives 83.7%).
- **[verified] `Player* p = &gPlayers[n]` vs `gPlayers[n].field`** pick different address shapes:
  the pointer form gives `mulli r5; addi r0, rB, sym@l; add r3, r0, r5`; direct indexing gives
  `mulli r0; addi r3, r3, sym@l; add r3, r3, r0`. Match whichever the original has per function.
- **[verified] Reading a field several times vs once.** If the original loads a byte once and keeps
  it in a register, the source used a local variable. Re-reading `src->index` in each statement
  produces a reload each time. The other way round: a load repeated inside a loop (`lwz` of the
  same field every iteration) means the source read the field through a pointer or by name each
  time; copying it into a local hoists it, which the original did not do.
- **[verified] Extra spilled address temps mean array indexing, not a pointer.** Four more stack
  words holding `&table[n].field` addresses went away only when `pStats->x` was written
  `lbl_801C6008[nSlot].x` (`AnimLib_PlanBank`, frame 0x140 -> 0x150 like the original).
- **[verified] Struct array through a typed local.** `ShotObj* pObj = (ShotObj*)h; pObj->events[i].f`
  gives `add; lfs off(r)`; the same through raw pointer arithmetic gives `addi; lfsx`.
- **[verified] Check struct sizes against the multiply.** Array indexing shows the element size:
  `slwi r,r,4` is 16 bytes, `mulli r,r,0x44` is 0x44 bytes. If ours has `mulli` by a different
  number, the struct is the wrong size (we had three function pointers in a two-pointer entry).
- **[verified] Three equality tests on one field: pointer vs indexed decides the range trick.**
  `if (gPlayers[n].nLie == 6 || ... == 7 || ... == 8)` became `subi; cmplwi 1; ble` (a 6..7 range)
  plus one compare; the same chain through `Player* p` (`p->nLie == 6 || ...`) gave the original's
  three plain `cmpwi/beq` (`Lie_AllowsFullSwing`, exact). A `switch` with the three cases was
  wrong both ways. The other direction: `pShot->p40->bAB == 6 || == 8 || == 9 || == 10` on a
  field read through a pointer gave the original's one `subi 8; clrlwi; cmplwi 2; ble` (8..10);
  copied into a `u8` local first, the same chain gave an 8..9 range plus a compare with 10
  (GoGolfCam `GolfCamera_InitZoomToAimCamera`). Which spelling makes the range depends on how the
  value is read, so try the field, the local and the pointer form.
- **[verified] An address used twice: keep the index, not a pointer.** `f32* pPin =
  &pCourse->pin[Game_CurrentPinSet()].x;` used twice swapped two registers in every declaration
  order; `nPinSet = Game_CurrentPinSet();` with `&pCourse->pin[nPinSet].x` written at each use
  matched (GoGolfCam `fn_800C0414`, 98.52% -> 100).
- **[verified] A loop over a global array: index it, don't walk a pointer.** `T* p = gTable;
  for (...; p++)` gives `addi r0, r3, gTable@l; mr r31, r0` for the pointer; `gTable[i].x` in the
  loop body (CW strength-reduces it to the same walking pointer) gives the original's direct
  `addi r31, r3, gTable@l`. Two functions went 93/95 -> 100 (`AI_TargetsClear`,
  `Golfer_TableByteSwap`). A local set at the top of each iteration, `pSlot = &gTable[i];`, is
  as good as indexing and gives the same direct `addi` (skalib `AnimLib_FreeCopies`, 96.0 -> 100;
  fully indexed was worse there).
- **[verified] A loop over the players with a separate base and offset register** (`addi rB,
  gPlayers@l` before the loop, `add rP, rB, rOff` inside, `addi rOff, rOff, 0xEF8`) comes from
  byte arithmetic: `(Player*)((u8*)gPlayers + i * sizeof(Player))` (the `PLAYER(i)` macro in
  `golfer.h`). Every other spelling (`&gPlayers[i]`, `gPlayers + i`, a local base pointer, an
  unsized array, other counter types) walks one pointer instead. Found with ten-line test
  functions (2026-09-23): `GM_RestartHole` exact, `fn_800E1074` 85 -> 98%, `GM_GolferConcede_Hole`
  85 -> 99.6%. Not every player loop uses it (`GM_CheckForAIConcede` got worse).
- **[verified] The PLAYER(i) byte-offset form also applies to plain arrays**: EA's loops over
  `lbl_8020315C[i]` next to `gPlayers[i]` index both by byte offset
  (`*(u32*)((u8*)arr + i * sizeof(u32))`); `arr[i]` walks a pointer (GameModeBattle 800C).
- **[verified] `addi r0, rOff, 0x1E4; lwzx r0, rBase, r0`** (the field offset added to the player
  offset, then an indexed load from a base that already holds `gPlayers + h*4`) comes from an
  unsigned player index: `gPlayers[(u32)i].nModePoints[h]` (GameModeMatch `fn_800E9F90`, found by
  the permuter as `i & 0xFFFFFFFFu`).
- **[verified] GetHonors shape (all four match modes):** one variable for "best" and "other",
  the pin read as `pCourse->pin[nHole].x` (not through a pointer), and the ball through the
  Player (`*(f32*)(PLAYER(i)->ball + 8)`), not a Ball* cast. GameModeMatch GetHonors 85 -> 100%.

### Types, casts and sign extension

- **[verified] `u32` bit-fields compile to EA's byte and halfword accesses** (`lbz`/`extrwi`/
  `rlwimi`/`stb`, and `lhz`/`sth` for a field that crosses a byte). A single `stw 0` over the whole
  word needs a union with a `u32` member (startUp voice flags). A test written on the shifted value,
  `if (p->nAttack * 16 == 0)`, gives an in-place `rlwinm.` mask test where `== 0` gives `extrwi.`
  (startUp `fn_800B0114`).
- **[verified] A `u8` return changes the epilogue order.** `return p != NULL;` as `int` restores r31
  after the `srwi`; as `u8` before it, like the original (startUp `fn_800AF9BC`).

- **[verified] `int` vs `s32` matters for parameters too.** PGATour `fn_800EE6A0(s32 nPlayer)` is
  exact; as `(int nPlayer)` it gives `addis r3, r4, 1; add r3, r3, r29` instead of the original's
  `addis r0, r4, 1; add r3, r0, r29` (99.81). Its neighbours were unaffected either way.

- **[verified] An enum-typed local holding 0 is not folded into an index multiply.** EA's
  `PlayerNumber_t nPlayer = PLR_1_e; gpSaveData[nPlayer]` gives `li rX, 0; mullw`; a literal 0,
  any integer local, a const global, an inline helper and `(Enum)0` all fold the multiply away
  (GameMode24 `fn_800F0820`, 82.9% -> 100; 12 times in GameMode23). EA style, no fake-match
  comment; the type is TW06's.
- **[verified] Two neighbouring words handled with 64-bit operations are one `u64`.** When the
  code ORs, ANDs and tests two adjacent words together (`and`/`xor`/`or.` on both halves, an AND
  with `li -1` for the upper word), declare one `u64` field. Player 0xC48/0xC4C as two `s32`s
  could not match in any statement order; as `u64 uC48` GameMode8 `fn_800FAAB8` went 93.3% ->
  99.2%, then exact with statement order.
- **[verified] Taking a field's address on a cast pointer reuses the base register.**
  `((Ball*)gPlayers[n].ball)->vPrev` gives `addi r4, r3, 0x10` where the original computes it
  afresh from `(f32*)(gPlayers[n].ball + 0x10)` (Swing `STATEFUNC_PreShotUpdate`, 96.14 -> 96.01).
- **[verified] The ball position is read as bytes of the Player, not through a `Ball*`.**
  `*(f32*)(gPlayers[n].ball + 0)` / `+ 8` matches; `((Ball*)gPlayers[n].ball)->vPos[0]` and
  `((f32*)gPlayers[n].ball)[0]` add an `addi r3, r3, 0xa90` pointer temp (GameMode8
  `fn_800FAD54`, 99.66%). Same rule as the GetHonors shape above, for `gPlayers[n]`. It is the
  one sanctioned raw offset until `Player.ball` gets a real type that matches.
- **[verified] `int` vs `long` changes the code.** For an `int` local CodeWarrior folds
  `n += 3` into every later use (`addi r5, rN, 3` at a call, `addi r0, rN, 3; cmpwi r0, 8`, ...),
  even when that costs instructions, and treats `n += *p` / `n = n + *p` as an in-place update.
  For a `long` (`s32`) local the promoted operand (`int`) needs an int-to-long conversion node,
  the statement is no longer the pure-`int` update pattern, and the adds stay in place:
  `addi rN, rN, 3` followed by plain `mr r5, rN` / `add rD, rD, rN` uses. `UStream_Decompress`
  went from 184/189 to exact by changing one declaration from `int nLen` to `s32 nLen`; nothing
  else in the function moved. Same size, same signedness, different type - so when a `+= const`
  is folded in our build but in place in the original (or the reverse), try the other of
  `int`/`long` before restructuring anything. The two are different types to the front end even
  though both are 32-bit. (What does *not* work: `register`, `#pragma optimization_level 3`,
  `optimize_for_size`, any placement of the add, a separate result variable, a ternary, a cast
  to `int` or `u8` - those are no-ops the front end drops - and every GC/2.x version behaves the
  same. A cast to `u32`/`s32` on one operand of `n = n + x` also blocks the fold, which is how the
  rule was found: `s32` is `long` in `game_types.h`.) The same choice decides loop hoisting: in
  Stableford's `fn_800FE8A8`, `int nPar` had `nPar + 2` hoisted into its own saved register before
  the player loop (one saved register more than the original); `s32 nPar` keeps the `addi` in the
  loop body. All 8 `int`/`s32` combinations of the other locals with `s32 nPar` are exact, all 7
  with `int nPar` stay at 75.4%. The `long j` counter in GameMode13 `fn_800F6ED4` is the same rule.
- **[verified] What else forces an in-place `+= const`:** the post-add value flowing into a phi
  with another definition of the same variable - a loop that decrements it, or a redefinition in
  one branch plus a use after the join. Even a *dead* decrement inside a later loop does it (the
  compiler deletes the decrement but has already given up folding). A dead write outside a loop
  does not. This costs a copy, though: the first definition is computed into a scratch register
  and moved (`extrwi r3; mr r28, r3`), so it is not what the original did here.
- **[verified] A signed array element returned as `s8` needs no `extsb` in the callee** (the caller
  extends). A `u8` element returned as `s8` gets one. (`GOLFERSTATE_GetPreviousState`: `nState` is `s8[5]`, and
  `GOLFERSTATE_GetCurrentState` reads it as `(u8)` to keep its own shape.)
- **[verified] `(s8)call()` into an `int` vs an `s8` local.** `extsb` register numbering followed
  `int a = call(); int b = load; s8 c = a;` (the cast as a separate s8 local after the load).
- **[verified] `u8` returned from an `int` local** gives `clrlwi r3, rX, 24` at the return; a `u8`
  local gives a plain `mr`.
- **[verified] Array index cast to `u32` moves the hoisting.** In the GameMode2 (Skins) honors
  loop (`fn_800F8278`), `gPlayers[(u32)i].field[h]` is what gives the original's base + h*4
  hoisted out of the loop; the plain `int` index computes it differently. The same `(u32)` index
  matched in GameModeMatch.c and in Stableford `fn_800FE8A8`: it is EA's habit (likely from a
  macro of theirs), so it is written without a fake-match comment.

### Function calls and parameters

- **[verified] A parameter's type moves saved registers, in the callee and in callers.** startUp
  `fn_800B044C`: `u32 uLen` -> `int nLen` (98.75 -> 100). PGATour `fn_800EF130`: the callee's
  prototype `fn_800EF0E0(PlayerNumber_t)` -> `(s32)` fixed the CALLER (98.65 -> 100; `int` did not).
- **[verified] A leftover argument can be an old value still in r4.** GameRound `fn_800E1074`:
  `fn_800D8D5C(i, 0)` was really `fn_800D8D5C(i)`; the callee sets r4 itself, and the original's r4 = 0
  was the zero shared by earlier stores (97.85 -> 100). Sign: a `li rX, 0` for an argument the
  original lacks, and the callee overwriting that register before reading it.

- **[verified] Pass the expression, not the variable just stored.** `p = x + n; f(p);` gives
  `add r30; mr r3, r30`; `p = x + n` used later with the call written `f(x + n)` gives the
  original's `add r3; mr r30, r3` (skalib `ClipBank_Load`, 99.76 -> 100).
- **[verified] Assign-then-fix each global in turn.** `a = x; a = align(a); b = y; b = align(b);`
  and `a = x; b = y; a = align(a); b = align(b);` schedule the same, but only the first gives the
  original's temporary registers (skalib `Skalib_Init`, 98.54 -> 100).
- **[verified] A wrong prototype can hide the real call shape and still score in the 80s-90s.**
  Check each prototype against the real definition, then check r3/r4 are set or kept live
  before each `bl`: `fn_80039344(View*, f32)` was really `(int nView, f32)` (Swing
  `STATEFUNC_GreenMorphExit` 94.8% -> 100); `GM_BumpBallForObstructions(void)` really takes
  `nPlayer` (`STATEFUNC_ShowYardageExit` 88.8% -> 100). The routine at 0x8000AE94 is
  `double fabs(double)` (it was first taken for `fabsf`): an `f32` declaration changes Swing's
  calls (`Swing_MisHitRumble` 99.5% -> 100 with `double`). The real `fabsf` is 0x8000AD9C, which
  calls it and rounds with `frsp`.
- **[verified] A callee that ignores r3, called while r3 still holds the caller's first
  parameter, takes that parameter.** `fn_8010069C()` -> `(nPlayer)` (Golfer
  `AI_FaceVector` 99.72% -> 100). Likewise a callee starting `clrlwi. r0, r3, 24` has a `u8`
  first parameter, and its callers pass a `u8` without `clrlwi` (Swing `fn_80045494/5558`).
- **[verified] `fn(15, (u8)a, b)` and `fn(15, a & 0xFF, b)` differ for an int parameter `a`.**
  The original's `mr r0, r3; clrlwi r4, r0, 24` comes from `a & 0xFF` (or a `u8` parameter passed
  on); `(u8)a` gives `clrlwi r4, r3, 24` (GameMode11 `fn_80101F40`, 57.9% -> 100).
- **[verified] A redeclaration with different parameter types is an error.** `void f(int, s32*);
  void f(int, int);` (also int vs s8, int vs long) gives "identifier redeclared", so a file that
  includes the header cannot declare its own variant; cast at the call site instead.
- **[verified] A function that leaves r3 alone before a call passes its own first parameter on**,
  and a `clrlwi` before the call gives that parameter's type. GameModeBattle `fn_800E7ABC`:
  `u8 fn(int a) { fn_800EA548(a); }` adds a `clrlwi`; `u8 fn(u8 bCheck)` is exact.
- **[verified] A `u16` return stored into an `s16` costs an `extsh` at the caller; `s16` does not**
  (GameMode5 `fn_800EC1E0`: `fn_800D2994` as `u16` drops 93.93 -> 92.96).
- **[verified] m2c turns a register holding half of a constant into an extra argument.** When the
  next call's argument register already holds `lis rN, 'PG'` for a later constant, m2c passes it
  on. `UStream_RegisterHandler('PGAc', fn, 'PG\0\0')` was really two arguments: GameMode23
  `fn_800EDE7C`, GameMode24 `fn_800F0518`, GameMode5 `fn_800EAE74` all went exact once it was
  dropped. Check each extra argument m2c shows against the callee's definition.
- **[verified] An unexplained `mr r3, r4` before the first call** means an unused first
  parameter: the function takes something in r3 it never reads (`fn_80051124(Ball*, f32, f32*)`).
- **[verified] 64-bit arguments skip r4.** `fn(handle, 0, k)` sites where the original sets r5 and
  r6 but never r4 are `fn(handle, (unsigned long long)k)`: a 64-bit integer goes in an aligned
  register pair (r5:r6). The animation-event lookups (`fn_8005CB78`, `fn_80048574`,
  `fn_80062BB0`, `fn_80062B98`) take the 64-bit event id that `fn_8000BEE4` hashes.
- **[verified] A function that returns its pointer argument** keeps r3 untouched and works on a
  copy in another register (`mr r6, r3`); a `void` function advances r3 directly.
- **[verified] The caller's cast and the prototype's type decide the extension.** A `clrlwi` on
  an argument before a call means the call site casts, e.g. `fn((u16)x)`: a `u16` parameter alone
  does not produce it. Whether the caller emits `extsb` depends on the prototype the translation
  unit sees (`s8` vs `int` parameter), so declare the prototype per file to match what that
  file's callers do (GameMode8, GameMode10).
- **[verified] Callbacks with another return type are assigned through a cast.** When a function
  stored in a callback slot returns a different type from the slot (a `u8` function in an `s32`
  slot), EA assigned it with a cast, e.g. `p->pfn = (u8 (*)(int, int))fn;` (GameMode15). Changing
  the function's own return type to fit the slot changes its code and loses the match.

### Compares and conditions

- **[verified] Nested `if`s vs one `&&`, and a flag vs a direct return.** `if (A) { if (B) x = 1; }`
  and `if (A && B) x = 1;` get different saved registers (PGATour `fn_800EE5B4`, 99.75 -> 100). A u8
  function's `bWin = 0; if (A && B) { if (C) bWin = 1; } return bWin;` was really
  `return A && B && C;` (`fn_800EE6A0`, 99.44 -> 100; no order of the flag form matched).

- **[verified] `return !(x == -1);` and `return x != -1;` end in a different instruction order**
  (GameMode11 `fn_80100798`, 93.9% -> 100).
- **[verified] A two-value choice `h = (n == 2) ? 6 : 7` compiles branch-free (`subi/nor/srawi`);**
  the original's `li 7; bne; li 6` is `h = 7; if (n == 2) h = 6;` (GameMode11 `fn_80100C08`).
- **[verified] A boolean chain assigned to an `int` keeps the original's register order where
  `if (...) b = 1;` does not**: `bDown = (A || B) && (C || D) && (E || F);` (GameMode8
  `fn_800FCC38`, 98.9% -> 100).
- **[verified] `n ? 0 : 1` and `n == 0` give the same instructions, different saved registers.**
  GameMode8 `fn_800FAAB8`: `nOther = nPlayer ? 0 : 1;` put `nOther` in the original's register
  (91.7% -> 93.1); `nOther = nPlayer == 0;` and `!nPlayer` (89.4%) did not.
- **[verified] Float compares.** `if (a < b) return;` gives `fcmpo; blt`; `if (a >= b) return;`
  gives `fcmpo; cror eq,gt,eq; beq` (the NaN-safe form). When the original has a plain `bge`,
  the source was `if (a < b) { ...rest... }` - a block, not an early return.
- **[verified] `!(a >= b || c > d)` vs `a < b && c <= d`.** The original's float compares follow
  the source's operator: `>=` gives `cror eq,gt,eq; beq`, `<` gives `bge`, `> d` taken-branch-away
  gives a bare `ble`, `<= d` gives `cror eq,lt,eq`. Match the operator, not just the logic.
- **[verified] `x >= c` in an `&&` chain gives `cror; bne`; the original's plain `blt` is
  `!(x < c)`.** Same for `if (!(h2 < -60000.0f) && ...)`. (`fn_8005418C`, `fn_80055324`.)
- **[verified] `if (f)` on a float** compiles to `fcmpu cr0, fX, f0` (variable first) against a
  loaded 0.0; `if (f != 0.0f)` and `if (0.0f != f)` both give the constant first. Same test,
  different operand order. (`fn_800512BC`.)
- **[verified] Three identical branches after one compare** (`cmpw; blt; blt; blt`) are
  `(a >= b && a >= b && a >= b)`: a comparison macro written for several fields that are all
  the same value here (`RECORD_AT_LEAST` in `GameManager.c`).
- **[verified] `x ? 0 : 2` compiles branch-free** (cntlzw/neg/andc); EA's code has
  `a = 2; if (x == 0) a = 0;`. A flag from `(a || b) ? 1 : 0` adds a `neg/or/srwi` normalising
  step that a plain `a || b` does not.
- **[verified] Clamp as nested ternaries.** `x = (x < lo) ? lo : ((x > hi) ? hi : x)` gives the
  original's `bge; b; ...; ble; b` layout (fn_8005AD20).
- **[verified] A shared tail** (`beq L; lfs; fmuls; L:` where ours has two copies of the
  multiply) is one `if` with the conditions merged: `if ((a && b) || (rand & 1)) x *= -1;`.
- **[observed] A redundant test is really in the source.** `if (a == 5 || a != 5 && x < y)`
  compiles to an extra compare and branch that looks like dead code (GameMode2 `fn_800F8624`,
  exact); when the original has a
  second test of the same variable right after the first, write the redundant form out.

### Returns, early exits and switch

- **[verified] A dispatch that tests the top case first can mean one more (empty) case.** With cases
  1 and 3 the compiler splits on a compare with 2; adding `case 4: break;` made it test 3 first,
  then 1, as the original does (FE_Manager `FE_GetBIOMovieName`, 97.6 -> 100). Check the asm for a
  compare with the extra value before calling it real; otherwise label it a fake match.

- **[verified] A byte test compiled `cmplwi; beq body; b end` is the last term of an or-chain of
  early exits**, not a switch: `if (a && b || Player_IsCPU(n) || gSession.nSplitScreen) return;`.
  A `(u32)` switch gives the branch shape with `cmpwi`; the or-chain gives the original's
  `cmplwi` (Golfer `Caddie_Update` 99.03% -> 100, `Caddie_Start` 97.86% -> 100).
- **[verified] An explicit `case 1: break;` leaves a second unconditional branch after the
  dispatch** (`b end; b end`); `default:` in any position does not (GameMode8 `fn_800FBD2C`).
- **[verified] Loop early exits: `for (...) { if (a[h] != 1) break; n++; }`** gives the
  original's test-at-the-bottom layout; `&& a[h] == 1` in the loop condition does not (GameMode8
  `fn_800FD1C0`, 92.5% -> 99.95%).
- **[verified] One shared `return` means one combined condition.** Four separate
  `if (...) return 2;` lines each get their own return sequence. If the original has several
  tests all branching to a single shared return, the source was `if (a || b || c || d) return 2;`.
- **[verified] The same rule from the other side: `bge skip; b end` at the last test.** Two early
  returns that became one `bne; b` are one `||` test:
  `if (pBall->bHoled || pBall->nState == 2) return;`. Three early returns written as separate
  `if (...) return;` lines gave `beq end; beq end; blt end`. The original had
  `beq end; beq end; bge body; b end` - the last term of an or-chain gets the inverted branch
  over an unconditional one. `if (a || b || c) return;` matched (`AI_ClubLonger`,
  `AI_ClubShorter`).
- **[verified] A branch over a branch is a one-case `switch`.** `bne next; b exit` over an early
  return, `beq next; b end` in a chain of tests, or `beq L1; b L2; L1:` (instead of one
  `beq exit`) comes from structure the optimiser doesn't collapse:
  `switch (f()) { case 0: break; default: return; }` (fn_800E7828,
  GameModeBattle/BestBall/FourBall EndGame, a `Game_GetCourse() != 7` test), or
  `if (x == 0) { return -1; } else { return y; }` with an explicit `else` (fn_800ECA34).
  `if (fn() != 8) { switch (fn()) { case 9: break; default: return; } }` matched fn_8005A850's
  gate exactly; `if`, `||`, `&&` and `goto` spellings all fold to a single `bne end`. A plain
  `if (x) return;` always collapses, and an `if (x == 0)` gives a single `bne`; an empty
  then-block is optimised away. The compare is `cmpwi` for a `u8` switch operand; the
  original's `cmplwi` is usually the or-chain rule above (a `cmplwi` in a branch-over-a-branch
  means the test is the last term of an `||` chain of early exits, and that chain can take in an
  enclosing `if`). GameEffects DB30C/DBA50: `if (A && !b && !c) { if (!gpGame->b285) return; ...}`
  had `cmplwi; bne body; b end`; `if (!A || b || c || !gpGame->b285) return; ...` matched both
  (99.49%/99.08% -> 100).
- **[verified] A one-case `switch`** on a call result gives `cmpwi; beq case; b default` and the
  default path returns the value still in r3; an `if (x != 8) return x;` gives a single `bne`.
- **[verified] A return through the common exit is a `goto`/single `return`.** An early
  `return nRet;` (known 0) compiles to `li r3, 0`; the original's `beq end` that reloads the
  variable at the shared epilogue came from `goto done;` with `done: return nRet;`.
- **[verified] `goto` is fine.** A repeated "fail early, otherwise keep testing" pattern matched
  first time written with `goto done;`. Do not assume it must be nested `if`/`else`.
- **[verified] One result variable for every return.** A lookup whose float registers would
  not settle matched once both returns went through one local: `p = k * (i - 1); p = k * t + p;
  return p; ... p = 1.1f; return p;` (`fn_80050D34`, found by the permuter).
- **[verified] A boolean result** `return a && b;` (u8 function) gives `li r5,0 ... mr r5,r3` where
  r3 still holds the constant 1 used in the test; nested ifs setting a flag do not.
- **[verified] A draft that "falls off the end" still has real returns.** m2c sometimes shows a
  non-void function whose last path returns nothing, because the value is already in `r3`. The
  original still returned it explicitly: `fn_800FA518` (GameMode8) matched as
  `if (gPlayers[nPlayer].nC38 <= 0) return gPlayers[nPlayer].nC38; return 1;`. Leaving the return
  out is undefined behaviour; `lint.py` reports it (`ub-missing-return`), and no unit has one.

### Inlining and inline helpers

- **[verified] A `const` on an inline helper's return type moves the caller's first loads.**
  FourBall `fn_800E8FC8`: `static inline int FourBall_TeamSecond(int)` -> `static inline const int`,
  94.39 -> 100 (`volatile` works the same; `s32`/`u32`/`long`/`short` go far worse). Label it a fake
  match. Getting repeated "pick the team's player" code through small helpers fixed the register
  numbers there (10 -> 4 differing) but made two neighbouring functions worse: decide per function.

- **[verified] An inline helper that reads a global itself, rather than being passed it,
  changes register choice** (GameMode11 `fn_80100C08`'s hint helper).
- **[verified] An inline helper that takes a value by pointer changes register choice.** The
  GetHonors sort append matched only as `static inline void AddIfScore(s32* aList, int* pnCount,
  ...) { if (...) { aList[*pnCount] = nPlayer; (*pnCount)++; } }` (Stableford `fn_800FE3FC`,
  Stroke `fn_800FF894`, 99.6% -> 100). GameMode12's used-up check likewise needed `s32* pPoints`
  copied into a local inside the helper. By value, or a plain append helper, stays at 99.6%.
- **[verified] No automatic inlining in game code.** Calling `GOLFERSTATE_Kill()` from a later
  function stays a `bl`; where the original has the body pasted in, write the body out.
- **[verified] Automatic inlining.** With `-inline auto` the compiler pastes small functions into
  callers that come after them in the same file. Symptom: a caller is much longer than the
  original and contains a copy of a helper's body; the original has a plain `bl helper`.
- **[other project]** `-inline deferred` makes functions come out in the reverse of source order.
- **[verified] A clamp whose result lands straight in the argument register** (`lfs f1, lo;
  fcmpo; bge; b call`) is an inline function returning early: `static inline f32 Clamp(x, lo,
  hi) { if (x < lo) return lo; if (x > hi) return hi; return x; }` used as the call's argument.
- **[verified] An inline accessor changes integer registers.** Reading `pRow->fDist[i]`
  directly swapped two integer registers; the same reads through a one-line
  `static inline f32 ClubRow_Dist(ClubRow*, int)` matched (`fn_80050F88`, found by the permuter).
  When only integer registers around an array read are off, try an accessor.
- **[verified] Repeated small expressions can be inline helpers.** `(f32)(1 - pBall->n84)`, used
  twice, matched as a one-line `static inline` (`Ball_SpinKeep`, `fn_800539F8`); a value scaled
  in place (`x = pBall->fSpinX; x *= k;`) matched where `pBall->fSpinX * k` did not.
- **[observed] `mr r0, r3; ...; mr rN, r0` around a call's result** (the value passing through r0
  before its home register) is the mark of an inlined helper's return value. Seen in fn_8005A478 with
  the unexplained `beq L; b L` pairs there; a bool or void inline helper did not reproduce them.
- **[verified] A store whose value is forwarded, done twice, can be an inline `return a = b;`.**
  GameMode10 fn_800F1424 stores `gReplayData.nPinSet` into the session in a loop and once more at
  the end. It only matched as `static inline s8 f(void) { return gSession.x = gReplayData.y; }`,
  called in the loop and once as a statement. The same code written inline, or as two statements
  in the helper, keeps the wrong registers.

### Floating point

- **[verified] `x += c` vs `x = x + c` swap the `fadds` operands**, and which one matches differs
  from function to function (GoGolfCam `GolfCamera_InitGreenZoomToAimCamera` needed `+=`; its
  `fn_800C1D3C` needed `x = x * c` rather than `*=`). Try both.

- **[verified] `x *= c` vs `x = x * c` on an address-taken array element.** `v[3] *= 2.0f` loads the
  element first; `v[3] = v[3] * 2.0f` loads the constant first, as the original did (GoGolfCam
  `fn_800C1D3C`, 4 differing -> 0).
- **[verified] An unwanted fused multiply-add goes away when one local holds the step.**
  `f74 += 0.01f * (fDist - f74)` fused; `fDist = 0.01f * (fDist - f74); f74 += fDist;` kept the
  original's separate `fmuls`/`fadds` (GoGolfCam `GolfCamera_ProcessPostShotCamera`, 99.07 -> 100).

- **[verified] A MIN-style ternary whose result lands in a scratch register is its own
  variable.** `r = a <= b ? a : b;` with `r` separate matched; writing it back into `a` let the
  compiler merge them (GameMode8 `fn_800FBD2C`).
- **[verified] A value the compiler CSEs into a callee-saved float register** (e.g. `100 - skill`
  used three times, first computed *after* a call) was a named local in the source, assigned
  right after the call whose result it is combined with: `r = Rand_Float(0); miss = 100 - skill;
  err = limit * (miss * r) / 100;`. Computing it before the call moves the `fsubs`.
- **[verified] Float locals coalesce by live range, not by name.** Two slopes computed in two
  halves of a function got different registers until they were two variables declared in the
  right place; the product `a *= t; b = p * (k * a)` in place of `p * (k * (a * t))` fixed the
  register numbers of a multiply chain. Integers too: one `s32 nCount` reused for two leaves
  became `nCountA`/`nCountB`, declared in that order (skalib `AnimLib_TrimCb`, 99.93 -> 100).
- **[verified] A float parameter reused as the running value.** When the original's product
  lands in a different callee-saved register than ours and the operands of `fmuls` are swapped
  (`f1, f0` vs `f0, f1`), the source overwrote the parameter: `fSeconds *= 60.0f; do { ...
  fSeconds -= fTick; } while (fSeconds > 0)` rather than a new `fLeft` local. (`Ball_SimSeconds`.)
- **[verified] `a*a + b*b`** becomes `fmuls` + `fmadds` only when written with the right-hand
  square first in evaluation order, i.e. `dx*dx + dz*dz` where `dz` was computed first; a fused
  `x + y*z` shows up as `fmadds` where the original has separate `fmuls`/`fadds` - use a
  temporary for the product.
- **[verified] `a + b*c` where the original fuses the *other* product.** CodeWarrior evaluates
  the heavier operand first and fuses the last product into `fmadds`; swapping the operands in
  the source changes nothing. To fuse `spin * (...)` and keep `k * speed` as the addend, compute
  the addend into a local first: `t = k * speed; x = spin * (...) + t;` (`Ball_FlightStep`).
- **[verified] A squared distance written inline.** `(a[0] - b[0]) * (a[0] - b[0]) + ...` matched
  where every spelling with a named difference was one register off (`fn_80053E98`).
- **[verified] Multiplication operand order.** With two variables, `a * b` and `b * a` produce
  `fmuls` with the operands in source order, so flip the C if only that differs. With a constant,
  flipping the C changed nothing: `x * 0.5f` and `0.5f * x` both came out constant-first.
- **[verified] Divide by a power of two becomes a multiply.** `x / 2.0f` compiles to `x * 0.5`,
  and the binary stores `0.5`, not `2.0`. It differs from a written `x * 0.5f` in two ways: the
  `fmuls` has the variable first, and the constant is loaded first (`lfs f0, 0.5; lfs f2, x`),
  where writing `x * 0.5f` loads them the other way round (STATEFUNC_ShowYardageUpdate). If a
  multiply by 0.5, 0.25 etc. has its operands the "wrong" way round, the source was probably a
  division.
- **[verified] A constant compared both ways as `x < c && x > -c` with `fneg`** comes from a local:
  `eps = 1e-6f; if (y < eps && y > -eps)`. Writing the literal `-1e-6f` loads a second constant.
- **[verified] A hoisted constant is a local.** `x * (1.0f / 128.0f)` with the constant loaded
  into the "wrong" FPR number: the original declared `f32 fInv = 1.0f / 128.0f;` at the top.

### Data, constants and symbols

- **[verified] Where EA's globals live: ordinary `.bss`, in link order, so game code builds with
  `-common off`.** The game's `.bss` (0x8019D540 to about 0x80261000) comes before the SDK's
  (0x802611A0 on) and follows the link order of the code that uses it; COMMON symbols would link
  after every `.bss`. With `pool_data off`, `-common` changes no function (all 7,647 scored the
  same both ways). So a shared uninitialised global is defined, non-static, in its owner file, with
  a plain `.bss` split (GameEffects `lbl_80202898`, DOL OK). Inside one object `.bss` is laid out in
  REVERSE definition order: define the file's globals last-address-first. `= {0}` moves a global to
  `.data`; don't. dtk's `common` split attribute is not for this (it turns every later `.bss` split
  into commons).

- **[observed] An all-zero small array in `.sdata` (not `.sbss`) was written with an initializer.**
  `u8 lbl_80281648[2] = {0, 0};` lands in `.sdata` and links (AlternateShot).
- **[verified] Base-last indexing of a big-struct global.** `gpSaveData[n].f` adds the base first;
  `((SaveProfile*)gpSaveData)[n].f`, an inline accessor returning `&gpSaveData[n]`, or a pointer local
  all give the original's `addis idx; addi; lhzx/stwx base` (PGATour `fn_800EE478`, 92.08 -> 100).

- **[verified] objdiff scores a switch 100% even when its jump table points at the wrong case
  bodies**: it masks relocations. GameMode11 `fn_80100328` had case labels off by one and read
  100%; only the linked DOL (`doldiff.py`) showed it. A function with a `switch` in a unit that is
  not linked yet is not proven: linking is the real check.
- **[verified] An object's `.data` is 8-aligned**, so a `.data` range in `splits.txt` must start on
  an 8-byte boundary; a misaligned start adds padding and shifts all later data. `datamap.py` only
  lists data our object emits; a file that owns tables we still declare `extern` needs its range
  widened to the file's whole data block (GameMode11: 0x80192D20, not 0x801930AC).
- **[verified] A dead-stripped function leaves its constants in the pool.** GameMode12's
  `.sdata2` has 0.0 and 0.5 early, where no remaining function uses them first; an unreferenced
  function using 0.0f then 0.5f at that point makes the unit link. Such a placeholder is a fake
  match with its body unknown: say so in its comment (style.md).
- **[verified] Float literals are pooled in `.sdata2` in the order they first appear in the
  source**; compiler-made constants (the int-to-float double) follow that function's literals.
  Folded or dead literals (`x * 1.0f`, `if (0)`, unused locals or inlines) get no slot.
- **[verified] A file-scope `const f32` is folded at every use and still emitted, so the value
  appears twice; a one-entry `const f32 x[1]` is loaded from the object instead.** GameMode8's lone
  1.0 at the start of its pool (`lbl_80284708`) is reproduced only by the array (a fake match: the
  bytes don't show what EA wrote).
- **[verified] A constant one bit off after linking can be a folded division.** GameMode8's
  0x3F7FBE76 is `59.94f / 60.0f`; the literal `0.999f` rounds to ...77.
- **[verified] An inline helper moves arithmetic after a call**: `n += SG_Score(t, s)` puts the
  multiply after the second call, where `n += s * 3 + t` computes it first (GameMode8
  `fn_800FDC5C`, 89.9% -> 98.2%).
- **[verified] An exact unit can still fail the link on function order.** objdiff scores each
  function by name, so a function defined out of address order reads 100% while the linked
  `.text` shifts. GameUI `fn_800E3ECC` was defined after `fn_800E3EE0`; moving it fixed the DOL.
- **[verified] A constant the original has twice means the original was two files.**
  CodeWarrior keeps one copy of each constant per file. GameMode10's code emits one int-to-float
  conversion double; the original has two (`lbl_80284688`, `lbl_802846A0`), each with the
  constants of one half of the unit, so the unit is two original files.
- **[verified] Constants shared with undecompiled neighbours mean the unit is a slice.** CharAnim
  uses three constants from a pool at 0x80283CD8 that neighbouring code also uses; it can link
  only once the unit is widened to own the whole pool.
- **[verified] An exact unit can still break the linked build.** objdiff compares functions; the
  DOL check also needs the data layout. A unit whose C makes the compiler emit its own data (the
  8-byte int-to-float constant `0x4330000080000000` in `.sdata2`, a string literal, a static)
  adds that data to the link and shifts the original's, so `main.dol` fails although every
  function is 100%. Check each new unit's object for data sections before marking it Matching.
- **[verified] Floating-point constants belong to a file.** Each source file gets its own copy of
  constants like `1.0f` in `.sdata2`. Symptom: every function reports 100% but the final hash
  check fails. Fix: add the constant's address range to the file's entry in `splits.txt`. First
  check that nothing outside the file references that address.
- **[observed]** Constants appear in `.sdata2` in the order the file first uses them. Neighbouring
  constants used by neighbouring functions suggest those functions share a source file.
- **[verified] Short string literals live in `.sdata`.** A literal of 8 bytes or less (`"tball"`,
  `"Glove"`, `"Swing.c"`) is addressed `li rX, sym@sda21`; longer ones `lis/addi`. When the original
  has an sda21 string, write the literal, not `extern char lbl_...[]` (Swing_LoadTuning exact).
- **[verified] Symbol names matter to objdiff.** Naming a global `gFoo` in C while
  `symbols.txt` still says `lbl_...` counts every reference as a mismatch; rename the data
  symbols (with `scope:local` for statics) at the same time as the code.
- **[verified] `volatile` shows up as reloads.** A global that is reloaded before every use in
  one function but loaded once in another is `volatile`. In UStream.c exactly the scalars the
  DVD-read callback writes (`gbReadPending`, `gpReadBuffer`, the ring indices it advances) are
  volatile; the main-thread-only indices are not. That is the author's intent, not noise.

### Other

- **Permuter results need a human.** Most of its "wins" are nonsense (`if (!x && !x) {}`,
  `vPin[(long long)1]`, dummy variables) that happen to nudge the allocator. Use them as hints
  for what to change and look for a plausible spelling that gives the same code; never commit
  the nonsense.
- **[verified] `abs()` on an int** is emitted inline as `srawi t,v,31; xor; subf` (no call).
- **[verified] GC/2.0, 2.5 and 2.6 compile the game units identically; GC/2.0p1 is worse**
  (Golfer 70 -> 67, Ball 63 -> 46, Swing 139 -> 133). Tested on every game unit, 2026-09-23.
- **[verified] GCC without optimization looks nothing like CodeWarrior:** frame pointer in `r31`
  (`mr r31, r1`), every variable written back to the stack after each statement, epilogue through
  `r11`. A binary can contain code from more than one compiler. See the
  [GCC appendix](#appendix-gcc-295-sn-prodg-not-used-by-tw2004-main-code) at the end of this file.

Working with decomp-toolkit (dtk)
---------------------------------

- A file entry in `splits.txt` must cover one contiguous address range per section. It may be a
  slice of the real original file; widen it as more neighbours are matched.
- The name of a C function must equal the symbol name in `symbols.txt` (`fn_80001234` is a legal C name).
- A unit marked `Matching` is linked from our C. If any part of it is wrong, the final hash check fails.
- `ninja build/<version>/report.json` then read `fuzzy_match_percent` per function to find which
  function is off.
- `objdiff-cli diff -p . -u main/<unit> <symbol> -o - --format json` prints a per-instruction diff.
  **In objdiff the left side is the original (target) and the right side is ours.** Easy to mislabel.
- Without `-o -`, `objdiff-cli diff` opens an interactive screen and hangs a script.
- dtk names SDK and runtime functions it recognizes automatically. Everything else is `fn_ADDRESS`.
- Strings in the binary are a free source of names: SDK build stamps, assert messages with source
  file names and paths.

Borrowing names from another project
------------------------------------

- **[verified]** Games built against the same SDK release contain byte-identical SDK functions. Find a
  decomp project with the same SDK build stamp (`<< Dolphin SDK - OS release build: ... >>`) and match
  its functions against yours to get names for free.
- Use the other project's split `.o` files, not its `main.dol`: they contain relocation records, so
  you can mask exactly the bytes the linker fills in and require everything else to be identical.
- Accept a name only when the match is unique in both directions. Small functions that differ only in
  a struct offset (getter families) are the main false-positive risk.
- Self-check two ways: names the analyzer already assigned must agree, and calls between matched
  functions must point at the expected names. We got 180/180 and 1,906/1,906.
- Static functions often share a name across files (`OnReset`, `AlarmHandler`). Skip or suffix them.

Bringing in another project's SDK source
----------------------------------------

- **[verified]** A unit can only be linked from source once *all* of its data is mapped too: every
  global, static and anonymous literal (`@N`) in each section. Code-only units are the easy first batch.
- **[verified]** Name data by aligning references: for each matched function, the sequence of symbol
  references in our disassembly lines up 1:1 with the sequence in the other project's split object.
  Require the same count, require each label to always map to the same name (we had 0 conflicts in
  635 functions). dtk quotes anonymous names (`"@104"@sda21`), so the regex must accept quotes.
- **[verified] `@N` names collide.** dtk names its own anonymous strings `@N`, and so does the other
  project's object. Never look up an `@N` name in symbols.txt; resolve it through the alignment map,
  per unit. One collision dropped an 8-byte constant and shifted every constant after it.
- **[verified] Pooled constants belong to one unit.** The linker merges identical read-only constants
  across objects; the binary holds one copy. Assign it to the unit that owns it in link order and let
  the other units' references resolve to it. Symptom: dtk reports "overlaps with previous split" or a
  "Cyclic dependency ... resolving link order" (data order contradicts text order).
- **[verified] Weak duplicates from inline functions.** An inline `sqrtf` in a header emits weak
  `_half`/`_three` constants into every object that uses it. The linker keeps the first, so the DOL is
  exact even though objdiff reports those objects' data sections as partially matched. The original
  binary kept four separate copies (one per link group), so do not try to merge them all into one.
- **[verified] Section-by-section DOL diff.** When the hash fails and every function reads 100%,
  compare the rebuilt DOL with the original section by section and print the first differing
  address; a constant-pool shift shows up as hundreds of `sda21` offset changes in the code.
- Per-file compiler settings: the SDK needs the other project's exact flags (`-inline auto` here),
  not the game's. Keep a separate `cflags_sdk`. Version conditionals in the borrowed source are
  satisfied by a small `GameVersions.h` plus `-DVERSION=...` rather than editing the source.

Comparing raw bytes yourself
----------------------------

- **[verified]** In an unlinked object file, a small-data access shows base register `r0` and offset
  0 (`lwz r5, 0(r0)`); the linked binary has `r13` or `r2` and a real offset. Mask this out, or a
  correct function looks one instruction short.
- Branch targets, `@ha`/`@l` pairs and small-data offsets are filled in by the linker. Compare
  instruction shape, not those bits.
- Tiny wrapper functions (`b otherFunction`) are identical after masking and will all "match" each
  other. Ignore them when searching one binary for another's functions.

Tool quirks on Windows
----------------------

- The Microsoft Store build of Python cannot see some files under `AppData\Local\Temp`. Keep scratch
  scripts somewhere plain such as `C:\dev\scratch`.
- Windows application control may block an old compiler executable the first time. Do not work
  around it; retry or skip that version.
- **[verified] Console programs from a windowless parent open Terminal windows.** With Windows
  Terminal as the default console host, every console program (git, a compiler, dtk) started by a
  process that has no console (pythonw, a scheduled task, a GUI app) gets its own Terminal window,
  and the window stays open after the program exits. `CREATE_NO_WINDOW` does not prevent it. The
  only reliable fix is for such a parent to launch nothing at all; do the work from a shell that has
  a console and hand results over through files. The dashboard reads `.git/HEAD` and a JSON file
  written by a post-commit hook for this reason.
- **[verified] Check the real process name before killing.** The Store Python runs as
  `pythonw3.13.exe`, not `pythonw`. `Get-Process pythonw` matches nothing, and a server you believe
  you restarted keeps running the old code while the new one fails to bind the port.
- Git warns about LF/CRLF on every commit. Harmless. `config/**/*.txt` must stay LF (set in `.gitattributes`).
- In shell heredocs a backslash inside Python source can get eaten. Use `chr(92)` or the Edit tool.

Good practice
-------------

- One batch of work, then report, then decide the next step. Do not loop on fixes silently.
- Show the plain-English logic and the C before building anything non-trivial.
- To learn what a function or field means, look at who calls it and what they do with the result.
- Name unknown things honestly: `unkOFFSET` for fields, `fn_ADDRESS` for functions, an `unsorted/`
  folder for files whose real name is unknown. Rename when there is evidence.
- Match the easy neighbours of a hard function first. They define the types the hard one uses.
- Never commit game data. Disc images and extracted assets are ignored by `.gitignore`.
- Write findings down here or in `tw2004-notes.md` the same day.

Appendix: GCC 2.95 (SN ProDG), not used by TW2004 main code
-----------------------------------------------------------

TW2004's main code is built with CodeWarrior, so none of this applies to it. These notes
are for code built with GCC 2.95 at -O0 (SN ProDG), kept for reference.

- **[verified]** Every variable is stored to the stack after each statement and reloaded before use;
  locals sit at increasing offsets in declaration order (first declared = lowest offset after the
  arguments). Swapping two declarations swaps their stack slots.
- **[verified] Stray `nop` before a `for` loop.** A `for` loop that follows early-return `if`s gets a
  `nop` emitted before its init statement. The same logic as `i = 0; while (i < n) { ...; i++; }`, or
  the `for` wrapped in its own `{ }` block, produces no nop. If a function is exactly one instruction
  too long and the extra is a `nop`, this is why.
- **[verified]** Static data with an initializer lands in `.data`; without one it is `.bss`. If the
  target has a `.data` object and ours does not, the original had an initializer.
- **[verified]** `__FILE__` / `__LINE__` passed to a call are reproduced with
  `#line <n> "<original path>"` on the line before the call. The path string then lands in `.rodata`
  and matches the assert string in the binary.
- **[verified]** The assembler (`NgcAs.exe`) leaves local branch displacements as relocations, so a
  raw byte comparison must mask conditional branches (`bc`, opcode 16) as well as `b`/`bl`.
- **[verified] Never round the match percentage.** A one-instruction difference in a 300-instruction
  function reads 99.98%. Treating >= 99.9% as "done" hid wrong struct offsets and off-by-one line
  numbers across 15 functions. The only acceptable number is exactly 100.
- **[verified] `crclr cr1eq` before a call means the callee had no prototype** (GCC's marker for a
  possibly-variadic call). Declare the function.
- **[verified] `x > 0` compiling to `cmpwi; beq` means `x` is unsigned.**
- **[verified] Local array initializers.** `u8 a[33] = {...}` and `u8 a[] = "..."` copy the same bytes
  but differ in two visible ways: an *unsized* array adds an `addi rX, r31, off` (address of the
  array) before the copy, and a *brace* initializer is emitted as unaligned `.byte`s while a string
  literal is 4-aligned in `.rodata`. Match both the copy prologue and the constant's address.
- **[verified] Casts vs masks.** `(u8)c` stored to a u16 gives `clrlwi; sth rX`; `c & 0xFF` gives
  `clrlwi; mr r0,rX; sth r0`. `(x << 8) & ~0xFF` gives `slwi; mr; clrrwi 8`. `while ((c = *p++) != 0)`
  tests the truncated assigned value; a separate `if (c == 0) break;` reloads it.
- **[verified] Dead jumps expose structure.** GCC -O0 emits every `break;` and every end-of-block
  jump, even after a `return`. An unreachable `b` right before the loop-back jump means the loop
  ended with `else break;`; a `b` after a `return` in a `default:` means `return; break;`.
- **[verified] A switch on an unsigned value** uses `cmplwi` in its binary search; on a signed
  one, `cmpwi`. The dispatch order of cases follows the source order of the case bodies.
- **[verified] Function-pointer call through two globals:** `g1->pfn(g2->field)` loads both
  globals first, then the function pointer into a spilled temp (8-aligned stack slot), then the
  argument. A local `pfn` variable adds an extra store/load pair.
- **[verified] `#line` numbering:** `#line N` applies to the *next* line. To make an assert on the
  first statement of a function report line L, put `#line L-2` before the function header
  (header, opening brace/declarations, assert).
- **[verified] Empty sections shift the link.** `NgcAs.exe` writes empty `.data`/`.bss`/`.sdata`/
  `.sbss` sections into every object. `mwldeppc` rounds the output section up when it meets one,
  even with the ALLOC flag cleared: our `.sbss` came out 2 bytes longer and the DOL hash failed while
  every function read 100%. `tools/prodg/prodgcc.py` deletes empty sections from the object.
- **[verified] A call result stored straight from `r3`.** `x = f()` normally goes `mr r0, r3; stw r0`.
  When the variable's type differs from the function's declared return type (an `int` result
  into a `u32`, or into a pointer) GCC stores `stw r3` directly. So a direct store means the
  types disagree; `mr r0, r3` means they agree. Fixing one side of a call changes the other file
  (`SFIOCreate` regressed when `TagFile_SetDescriptor` gained its real `int` return type).
- **[verified] GCC 2.95 reassociates `a - (b - C)` into `(a + C) - b`** (fold-const.c
  `split_tree` / `associate`), even at -O0, for every integer spelling: casts, `+ 0`, `- 0`, a
  comma, `?:`, unary plus, identity `& | ^ >> * /`. Nothing wrapped survives because `fold`
  strips no-op conversions first. If the binary computes `b - C` first and then `a - that`,
  the original went through something `fold` cannot split: a pointer (`(u8*)a - (b - C)` -
  typical of an alignment macro) or a GNU statement expression. Both reproduce it exactly.
- **[verified] A constant-true `if`.** `if (TRUE) { ...; return 0; } else { return 4; }` compiles
  to the then-block, a dead jump over the else, then the else body - with no compare at all.
  Symptom: an unreachable `return` or assert after a `return`, preceded by a dead `b` to the
  end of the chain. `TagFile_FreeBuffer` and the last branch of `TagFile_Update` have it.
- **[verified] Unused `static` variables are still emitted at -O0**, in declaration order, into
  `.sbss`/`.sdata` like any other. If a file's `.sbss` is bigger than its referenced globals,
  add a dummy static of the missing size; the DOL will not hash otherwise. Under `-common on`
  uninitialised non-static globals become COMMON symbols and land after all `.bss`; game code
  builds with `-common off` (see "Where EA's globals live" in Data, constants and symbols).
- **[verified] Comparison operand order.** `a == p->x` loads `p` first, then `a` into `r0`, then
  `p->x` into `r9`, and compares `r0, r9`. `p->x == a` gives `lwz r0, 0(r9)` then `lwz r9, a`.
  For two stack variables the left one is always `r0`. So the register order of a `cmpw`
  recovers which operand was written first.
- **[verified] `(*p)++` vs `*p = *p + 1` vs `*p += 1`.** The post-increment loads `p` three times
  (one dead load into `r0`); the other two load it twice.
- **[verified] Small globals vs a struct.** GCC puts objects of 8 bytes or less in `.sdata`/`.sbss`
  and addresses them with `@sda21`; a struct of four ints goes to `.data` with `lis`/`addi`. If the
  original uses `@sda21` for each field, they were separate variables.
- **[verified] Order of read-only data reveals declaration order.** A function-pointer table that
  precedes the `__FILE__` string in `.rodata` was defined above the functions (with prototypes).
- The `ngccc.exe` driver refuses to run without an installed `sn.ini`; run `cpp.exe`, `cc1.exe`
  and `NgcAs.exe` directly (see `tools/prodg/prodgcc.py`). `cc1` takes `-O0 -quiet in.i -o out.s`.
- The five ProDG builds in the compiler pack (3.5 - 3.9.3, all GCC 2.95.x) produce identical
  output for unoptimized code; the version cannot be identified from `-O0` functions.
