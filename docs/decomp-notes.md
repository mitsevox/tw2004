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

- **[verified] CodeWarrior -O4,p loop shapes.** A byte/halfword copy loop written as
  `while (n > 7) { eight explicit copies through temporaries; p += 8; n -= 8; }` followed by
  `while (n--) *d++ = *s++;` comes out as: count = n >> 3 into `mtctr`, the block unrolled 2x with
  all eight loads before the stores, `n += -(count << 3)` after, then the remainder loop
  auto-unrolled 8x. Interleaved load/store pairs mean the copies did *not* go through
  temporaries (the compiler assumed aliasing). A `do { } while (--n)` is *not* unrolled at all.
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
  rule was found: `s32` is `long` in `game_types.h`.)
- **[verified] What else forces an in-place `+= const`:** the post-add value flowing into a phi
  with another definition of the same variable - a loop that decrements it, or a redefinition in
  one branch plus a use after the join. Even a *dead* decrement inside a later loop does it (the
  compiler deletes the decrement but has already given up folding). A dead write outside a loop
  does not. This costs a copy, though: the first definition is computed into a scratch register
  and moved (`extrwi r3; mr r28, r3`), so it is not what the original did here.
- **[verified] Float compares.** `if (a < b) return;` gives `fcmpo; blt`; `if (a >= b) return;`
  gives `fcmpo; cror eq,gt,eq; beq` (the NaN-safe form). When the original has a plain `bge`,
  the source was `if (a < b) { ...rest... }` - a block, not an early return.
- **[verified] A value the compiler CSEs into a callee-saved float register** (e.g. `100 - skill`
  used three times, first computed *after* a call) was a named local in the source, assigned
  right after the call whose result it is combined with: `r = Rand_Float(0); miss = 100 - skill;
  err = limit * (miss * r) / 100;`. Computing it before the call moves the `fsubs`.
- **[verified] `a*a + b*b`** becomes `fmuls` + `fmadds` only when written with the right-hand
  square first in evaluation order, i.e. `dx*dx + dz*dz` where `dz` was computed first; a fused
  `x + y*z` shows up as `fmadds` where the original has separate `fmuls`/`fadds` - use a
  temporary for the product.
- **[verified] A float parameter reused as the running value.** When the original's product
  lands in a different callee-saved register than ours and the operands of `fmuls` are swapped
  (`f1, f0` vs `f0, f1`), the source overwrote the parameter: `fSeconds *= 60.0f; do { ...
  fSeconds -= fTick; } while (fSeconds > 0)` rather than a new `fLeft` local. (`Ball_SimSeconds`.)
- **[verified] An unexplained `mr r3, r4` before the first call** means an unused first
  parameter: the function takes something in r3 it never reads (`fn_80051124(Ball*, f32, f32*)`).
- **[verified] A constant compared both ways as `x < c && x > -c` with `fneg`** comes from a local:
  `eps = 1e-6f; if (y < eps && y > -eps)`. Writing the literal `-1e-6f` loads a second constant.
- **[verified] `if (f)` on a float** compiles to `fcmpu cr0, fX, f0` (variable first) against a
  loaded 0.0; `if (f != 0.0f)` and `if (0.0f != f)` both give the constant first. Same test,
  different operand order. (`fn_800512BC`.)
- **[verified] A clamp whose result lands straight in the argument register** (`lfs f1, lo;
  fcmpo; bge; b call`) is an inline function returning early: `static inline f32 Clamp(x, lo,
  hi) { if (x < lo) return lo; if (x > hi) return hi; return x; }` used as the call's argument.
- **[verified] Float locals coalesce by live range, not by name.** Two slopes computed in two
  halves of a function got different registers until they were two variables declared in the
  right place; the product `a *= t; b = p * (k * a)` in place of `p * (k * (a * t))` fixed the
  register numbers of a multiply chain.
- **[verified] `a + b*c` where the original fuses the *other* product.** CodeWarrior evaluates
  the heavier operand first and fuses the last product into `fmadds`; swapping the operands in
  the source changes nothing. To fuse `spin * (...)` and keep `k * speed` as the addend, compute
  the addend into a local first: `t = k * speed; x = spin * (...) + t;` (`Ball_FlightStep`).
- **[verified] One result variable for every return.** A lookup whose float registers would
  not settle matched once both returns went through one local: `p = k * (i - 1); p = k * t + p;
  return p; ... p = 1.1f; return p;` (`fn_80050D34`, found by the permuter).
- **[verified] An inline accessor changes integer registers.** Reading `pRow->fDist[i]`
  directly swapped two integer registers; the same reads through a one-line
  `static inline f32 ClubRow_Dist(ClubRow*, int)` matched (`fn_80050F88`, found by the permuter).
  When only integer registers around an array read are off, try an accessor.
- **[verified] A squared distance written inline.** `(a[0] - b[0]) * (a[0] - b[0]) + ...` matched
  where every spelling with a named difference was one register off (`fn_80053E98`).
- **[verified] Repeated small expressions can be inline helpers.** `(f32)(1 - pBall->n84)`, used
  twice, matched as a one-line `static inline` (`Ball_SpinKeep`, `fn_800539F8`); a value scaled
  in place (`x = pBall->fSpinX; x *= k;`) matched where `pBall->fSpinX * k` did not.
- **[verified] An exact unit can still break the linked build.** objdiff compares functions; the
  DOL check also needs the data layout. A unit whose C makes the compiler emit its own data (the
  8-byte int-to-float constant `0x4330000080000000` in `.sdata2`, a string literal, a static)
  adds that data to the link and shifts the original's, so `main.dol` fails although every
  function is 100%. Check each new unit's object for data sections before marking it Matching.
- **Permuter results need a human.** Most of its "wins" are nonsense (`if (!x && !x) {}`,
  `vPin[(long long)1]`, dummy variables) that happen to nudge the allocator. Use them as hints
  for what to change and look for a plausible spelling that gives the same code; never commit
  the nonsense.
- **[verified] `x >= c` in an `&&` chain gives `cror; bne`; the original's plain `blt` is
  `!(x < c)`.** Same for `if (!(h2 < -60000.0f) && ...)`. (`fn_8005418C`, `fn_80055324`.)
- **[verified] Two early returns that became one `bne; b`** are one `||` test:
  `if (pBall->bHoled || pBall->nState == 2) return;`.
- **[verified] A local reused as a temporary** can be the reason for a register mismatch: the
  original stored a sqrt into the distance local before scaling it (`Ball_Tick`).
- **[verified] `abs()` on an int** is emitted inline as `srawi t,v,31; xor; subf` (no call).
- **[verified] The `lwzu`/`lfsu` idiom is a repeated field access, not a pointer local.** When the
  same `gPlayers[n].field` is read again after a call, CodeWarrior makes a pointer to the field
  itself and folds the first read into `lwzu rD, off(rP)`; later reads are `lwz rD, 0(rP)`. An
  explicit `s32* p = &gPlayers[n].field; ... *p` local blocks that shape (`addi rP; lwz` from the
  base). Write the field access out each time. Applied to 33 functions of `Swing.c` in one sweep
  (`C:\dev\scratch\tw\ptr_sweep.py`), 10 became exact. Exception: pointers that are indexed
  (`pViews[k]`) or passed as pointers stay pointers.
- **[verified] 64-bit arguments skip r4.** `fn(handle, 0, k)` sites where the original sets r5 and
  r6 but never r4 are `fn(handle, (unsigned long long)k)`: a 64-bit integer goes in an aligned
  register pair (r5:r6). The animation-event lookups (`fn_8005CB78`, `fn_80048574`,
  `fn_80062BB0`, `fn_80062B98`) take the 64-bit event id that `fn_8000BEE4` hashes.
- **[verified] `(fn() & uMask)` operand order.** `and. r0, r3, rM` (call result first) comes from
  the mask call inline: `if (fn_800136DC(x) & fn_800142AC(k, m))`. A `uMask` local assigned
  first gives `and. r0, rM, r3`, and swapping the operands in the source changes nothing. Applied
  to every single-use mask in Swing.c (States 04/05/08/09/10 exact, 06/12/14/22 closer).
- **[verified] A loop over the players with a separate base and offset register** (`addi rB,
  gPlayers@l` before the loop, `add rP, rB, rOff` inside, `addi rOff, rOff, 0xEF8`) comes from
  byte arithmetic: `(Player*)((u8*)gPlayers + i * sizeof(Player))` (the `PLAYER(i)` macro in
  `golfer.h`). Every other spelling (`&gPlayers[i]`, `gPlayers + i`, a local base pointer, an
  unsized array, other counter types) walks one pointer instead. Found with ten-line test
  functions (2026-09-23): `GM_RestartHole` exact, `fn_800E1074` 85 -> 98%, `GM_GolferConcede_Hole`
  85 -> 99.6%. Not every player loop uses it (`GM_CheckForAIConcede` got worse).
- **[verified] Three identical branches after one compare** (`cmpw; blt; blt; blt`) are
  `(a >= b && a >= b && a >= b)`: a comparison macro written for several fields that are all
  the same value here (`RECORD_AT_LEAST` in `GameManager.c`).
- **[verified] GC/2.0, 2.5 and 2.6 compile the game units identically; GC/2.0p1 is worse**
  (Golfer 70 -> 67, Ball 63 -> 46, Swing 139 -> 133). Tested on every game unit, 2026-09-23.
- **[verified] The right side of a comparison is evaluated first.** `f(0) < f(1)` calls `f(1)`
  first. So the call order in the original tells you how the comparison was written: m2c's
  `t = f(1); if (f(0) < t)` is the source `f(0) < f(1)` (GameModeAlternateShot, 2026-09-23).
  Additions go left to right, so `a = f(x) + f(y)` calls `f(x)` first.
- **[verified] `bne next; b exit` over an early return** (instead of one `beq exit`) comes from
  structure the optimiser doesn't collapse: `switch (f()) { case 0: break; default: return; }`
  (fn_800E7828, GameModeBattle/BestBall/FourBall EndGame, a `Game_GetCourse() != 7` test), or
  `if (x == 0) { return -1; } else { return y; }` with an explicit `else` (fn_800ECA34). A plain
  `if (x) return;` always collapses. The `if (!gpGame->b285) return;` in GameEffects
  (DB30C/DBA50) is still unexplained: a switch gives `cmpwi`, the original has `cmplwi`.
- **[verified] `x ? 0 : 2` compiles branch-free** (cntlzw/neg/andc); EA's code has
  `a = 2; if (x == 0) a = 0;`. A flag from `(a || b) ? 1 : 0` adds a `neg/or/srwi` normalising
  step that a plain `a || b` does not.
- **[verified] Taking a parameter's address puts it on the stack at the parameter's slot**;
  copying it to a local first gives a different slot order (fn_800E4164, fn_800E53F0).
- **[verified] The PLAYER(i) byte-offset form also applies to plain arrays**: EA's loops over
  `lbl_8020315C[i]` next to `gPlayers[i]` index both by byte offset
  (`*(u32*)((u8*)arr + i * sizeof(u32))`); `arr[i]` walks a pointer (GameModeBattle 800C).
- **[verified] GetHonors shape (all four match modes):** one variable for "best" and "other",
  the pin read as `pCourse->pin[nHole].x` (not through a pointer), and the ball through the
  Player (`*(f32*)(PLAYER(i)->ball + 8)`), not a Ball* cast. GameModeMatch GetHonors 85 -> 100%.
- **[verified] Statement order among plain stores matters**: `a[i] = 1; x = 0; y = 0;` and
  `x = 0; a[i] = 1; y = 0;` give different register use (fn_800E4238). And a call made for an
  argument that was never used (`fn_800E5DA0(lbl)` vs `fn_800E5DA0()`) changes the code.
- **[verified] `x / 2.0f` becomes `x * 0.5f`** with the constant loaded first (`lfs f0, 0.5;
  lfs f2, x`). Writing `x * 0.5f` loads them the other way round. (STATEFUNC_ShowYardageUpdate.)
- **[verified] `!(a >= b || c > d)` vs `a < b && c <= d`.** The original's float compares follow
  the source's operator: `>=` gives `cror eq,gt,eq; beq`, `<` gives `bge`, `> d` taken-branch-away
  gives a bare `ble`, `<= d` gives `cror eq,lt,eq`. Match the operator, not just the logic.
- **[verified] Short string literals live in `.sdata`.** A literal of 8 bytes or less (`"tball"`,
  `"Glove"`, `"Swing.c"`) is addressed `li rX, sym@sda21`; longer ones `lis/addi`. When the original
  has an sda21 string, write the literal, not `extern char lbl_...[]` (Swing_LoadTuning exact).
- **[verified] No automatic inlining in game code.** Calling `GOLFERSTATE_Kill()` from a later
  function stays a `bl`; where the original has the body pasted in, write the body out.
- **[verified] A signed array element returned as `s8` needs no `extsb` in the callee** (the caller
  extends). A `u8` element returned as `s8` gets one. (`GOLFERSTATE_GetPreviousState`: `nState` is `s8[5]`, and
  `GOLFERSTATE_GetCurrentState` reads it as `(u8)` to keep its own shape.)
- **[verified] Struct array through a typed local.** `ShotObj* pObj = (ShotObj*)h; pObj->events[i].f`
  gives `add; lfs off(r)`; the same through raw pointer arithmetic gives `addi; lfsx`.
- **[verified] Clamp as nested ternaries.** `x = (x < lo) ? lo : ((x > hi) ? hi : x)` gives the
  original's `bge; b; ...; ble; b` layout (fn_8005AD20).
- **[observed] `mr r0, r3; ...; mr rN, r0` around a call's result** (the value passing through r0
  before its home register) is the mark of an inlined helper's return value. Seen in fn_8005A478 with
  the unexplained `beq L; b L` pairs there; a bool or void inline helper did not reproduce them.
- **[verified] Stack locals go in reverse declaration order** (the last declared gets the lowest
  `r1` offset). Twelve 16-byte vectors in fn_8005A0FC matched once declared highest-address first.
- **[verified] Declaration order picks the saved registers.** Two loop counters in fn_8005A0FC came
  out swapped (r27/r28) until their declarations were swapped. Try this first on any diff that is
  only a register permutation.
- **[verified] `beq next; b end` in a chain of tests is a one-case `switch`** with `default: return`.
  `if (fn() != 8) { switch (fn()) { case 9: break; default: return; } }` matched fn_8005A850's
  gate exactly; `if`, `||`, `&&` and `goto` spellings all fold to a single `bne end`.
- **[verified] Register numbering, declared-first-highest in some functions.** In fn_8005A850 the
  earliest-declared local took r30 and later ones descended; one `s8` (instead of `u8`) on a
  colour byte fixed the last permutation. When the lowest-first order does not help, try the reverse.
- **[verified] Unrolled loops: the guard tells you the counter's type and the bound's form.** CW
  unrolls counted loops and puts a guard in front. With an `int` counter and a constant bound the
  guard folds away; declared `s32` (a `long`) it stays as `li rX, 0; cmpwi rX, N; bge`
  (skalib.c `AnimLib_BuildCb`, `Skalib_Init`). With a variable bound, `for (i = 0; i < p->n; i++)`
  inside an `if` followed by more code gives `cmpw i, n; bge`, while the original's
  `cmpwi n, 0; ble` came from the bound held in a local or written `p->n > i`
  (`Skalib_SetBudgets`, `AnimLib_FreeWorkCopies`). Tested in isolation in
  `C:\dev\scratch\tw\looptest\`.
- **[verified] An initialiser placed after early exits is in an inner block.** A local array copied
  from `.rodata` only after the function's first checks means `if (ok) { u32 aPad[4] = {0}; ... }`,
  not a declaration at the top (`AnimLib_MergeOverlay`).
- **[verified] Extra spilled address temps mean array indexing, not a pointer.** Four more stack
  words holding `&table[n].field` addresses went away only when `pStats->x` was written
  `lbl_801C6008[nSlot].x` (`AnimLib_PlanBank`, frame 0x140 -> 0x150 like the original).
- **[verified] A return through the common exit is a `goto`/single `return`.** An early
  `return nRet;` (known 0) compiles to `li r3, 0`; the original's `beq end` that reloads the
  variable at the shared epilogue came from `goto done;` with `done: return nRet;`.
- **[verified] A parameter can be copied to a local.** `ClipBank_Load` kept its data pointer in the
  lowest saved register only when the parameter was copied into a local declared last.
- **[verified] One variable, two jobs.** When the original reuses one FPR for two unrelated values
  (a distance, later a blend step), the source reused one local.
- **[verified] Chained assignment stores backwards.** `a[0] = a[1] = a[2] = 0` stores 2, 1, 0;
  the original wrote four statements in order.
- **[verified] A hoisted constant is a local.** `x * (1.0f / 128.0f)` with the constant loaded
  into the "wrong" FPR number: the original declared `f32 fInv = 1.0f / 128.0f;` at the top.
- **[verified] `(s8)call()` into an `int` vs an `s8` local.** `extsb` register numbering followed
  `int a = call(); int b = load; s8 c = a;` (the cast as a separate s8 local after the load).
- **[verified] A branch over a branch (`beq L1; b L2; L1:`) is a `switch` with one `case 0:`.**
  An `if (x == 0)` gives a single `bne`; an empty then-block is optimised away. The compare is
  `cmpwi` for a `u8` switch operand; the original's `cmplwi` is still unexplained.
- **[verified] A shared tail** (`beq L; lfs; fmuls; L:` where ours has two copies of the
  multiply) is one `if` with the conditions merged: `if ((a && b) || (rand & 1)) x *= -1;`.
- **[verified] `Player* p = &gPlayers[n]` vs `gPlayers[n].field`** pick different address shapes:
  the pointer form gives `mulli r5; addi r0, rB, sym@l; add r3, r0, r5`; direct indexing gives
  `mulli r0; addi r3, r3, sym@l; add r3, r3, r0`. Match whichever the original has per function.
- **[verified] `u8` returned from an `int` local** gives `clrlwi r3, rX, 24` at the return; a `u8`
  local gives a plain `mr`.
- **[verified] The declaration-order rule for callee-saved registers does not hold in
  `AI_ChooseTarget`** (0x8002C2DC, 20 live variables, a loop with eight calls): no ordering tried
  reproduced the original assignment, and hand-hoisting values into named locals made it worse.
  The function is left at 92.7% with only register numbers differing. Something else (spill
  cost, loop weight) drives allocation once a function is this large.
- **[verified] An initializer in the declaration changes register assignment.** `f32* p = &x;`
  at the top of the function got r28 while the same pointer declared bare and assigned on the
  next line got r31 (the original). When a function is otherwise exact but the callee-saved
  numbers are off, try moving initializers out of the declarations (and vice versa).
- **[verified] `a()->arr[b()].f`** evaluates `b()` first and scales the index before calling
  `a()`; splitting `b()` into a local first moves the shift after the call.
- **[verified] A one-case `switch`** on a call result gives `cmpwi; beq case; b default` and the
  default path returns the value still in r3; an `if (x != 8) return x;` gives a single `bne`.
- **[verified] A boolean result** `return a && b;` (u8 function) gives `li r5,0 ... mr r5,r3` where
  r3 still holds the constant 1 used in the test; nested ifs setting a flag do not.
- **[verified] A load repeated inside a loop** (`lwz` of the same field every iteration) means the
  source read the field through a pointer or by name each time; copying it into a local hoists
  it, which the original did not do.
- **[verified] Register order for callee-saved locals** follows declaration order (first declared
  gets r31). Parameters used as working pointers come after the locals; to make `pEnd` r31 and the
  destination r30, declare `pEnd` first and copy the parameters into locals declared after it.
  A `u8 v = (u8)value` local declared *before* the pointer local gets the `clrlwi` in place.
- **[verified] A function that returns its pointer argument** keeps r3 untouched and works on a
  copy in another register (`mr r6, r3`); a `void` function advances r3 directly.
- **[verified] `volatile` shows up as reloads.** A global that is reloaded before every use in
  one function but loaded once in another is `volatile`. In UStream.c exactly the scalars the
  DVD-read callback writes (`gbReadPending`, `gpReadBuffer`, the ring indices it advances) are
  volatile; the main-thread-only indices are not. That is the author's intent, not noise.
- **[verified] Symbol names matter to objdiff.** Naming a global `gFoo` in C while
  `symbols.txt` still says `lbl_...` counts every reference as a mismatch; rename the data
  symbols (with `scope:local` for statics) at the same time as the code.
- **[verified] Local variables and the stack.** The order locals are declared in changes where
  they sit on the stack. Symptom: a function at ~99% where the only differences are stack offsets
  swapped between two variables. Fix: swap the declarations.
- **[verified] Multiplication operand order.** With two variables, `a * b` and `b * a` produce
  `fmuls` with the operands in source order, so flip the C if only that differs. With a constant,
  flipping the C changed nothing: `x * 0.5f` and `0.5f * x` both came out constant-first.
- **[verified] Divide by a power of two becomes a multiply.** `x / 2.0f` compiles to `x * 0.5`
  with the variable first, and the binary stores `0.5`, not `2.0`. If a multiply by 0.5, 0.25 etc.
  has its operands the "wrong" way round, the source was probably a division.
- **[verified] One shared `return` means one combined condition.** Four separate
  `if (...) return 2;` lines each get their own return sequence. If the original has several
  tests all branching to a single shared return, the source was `if (a || b || c || d) return 2;`.
- **[verified] The same rule from the other side: `bge skip; b end` at the last test.** Three
  early returns written as separate `if (...) return;` lines gave `beq end; beq end; blt end`.
  The original had `beq end; beq end; bge body; b end` - the last term of an or-chain gets the
  inverted branch over an unconditional one. `if (a || b || c) return;` matched
  (`AI_ClubLonger`, `AI_ClubShorter`).
- **[verified] Three equality tests on one field: pointer vs indexed decides the range trick.**
  `if (gPlayers[n].nLie == 6 || ... == 7 || ... == 8)` became `subi; cmplwi 1; ble` (a 6..7 range)
  plus one compare; the same chain through `Player* p` (`p->nLie == 6 || ...`) gave the original's
  three plain `cmpwi/beq` (`Lie_AllowsFullSwing`, exact). A `switch` with the three cases was
  wrong both ways.
- **[verified] A loop over a global array: index it, don't walk a pointer.** `T* p = gTable;
  for (...; p++)` gives `addi r0, r3, gTable@l; mr r31, r0` for the pointer; `gTable[i].x` in the
  loop body (CW strength-reduces it to the same walking pointer) gives the original's direct
  `addi r31, r3, gTable@l`. Two functions went 93/95 -> 100 (`AI_TargetsClear`,
  `Golfer_TableByteSwap`).
- **[verified] Runs of identical stores are unrolled loops.** CW at -O4 fully unrolls small
  constant-count loops (10 link checks, 12 attribute stores, 19 flag bytes - the last as 18
  stores plus a one-iteration remainder loop) and unrolls a 34-iteration loop by two. Writing
  the loop, not the stores, is what matches (`AI_TargetsLoad`, `Golfer_TableSetup` exact).
- **[verified] Check struct sizes against the multiply.** Array indexing shows the element size:
  `slwi r,r,4` is 16 bytes, `mulli r,r,0x44` is 0x44 bytes. If ours has `mulli` by a different
  number, the struct is the wrong size (we had three function pointers in a two-pointer entry).
- **[verified] Reading a field several times vs once.** If the original loads a byte once and keeps
  it in a register, the source used a local variable. Re-reading `src->index` in each statement
  produces a reload each time.
- **[verified] `goto` is fine.** A repeated "fail early, otherwise keep testing" pattern matched
  first time written with `goto done;`. Do not assume it must be nested `if`/`else`.
- **[verified] Automatic inlining.** With `-inline auto` the compiler pastes small functions into
  callers that come after them in the same file. Symptom: a caller is much longer than the
  original and contains a copy of a helper's body; the original has a plain `bl helper`.
- **[other project]** `-inline deferred` makes functions come out in the reverse of source order.
- **[verified] Floating-point constants belong to a file.** Each source file gets its own copy of
  constants like `1.0f` in `.sdata2`. Symptom: every function reports 100% but the final hash
  check fails. Fix: add the constant's address range to the file's entry in `splits.txt`. First
  check that nothing outside the file references that address.
- **[observed]** Constants appear in `.sdata2` in the order the file first uses them. Neighbouring
  constants used by neighbouring functions suggest those functions share a source file.
- **[other project]** Register choice rules seen on CodeWarrior 1.3.2: a temporary goes in the
  lowest free register; a variable that is modified in place keeps its own register; a value
  used once gets folded into the instruction that uses it. A single wrong register with all else
  right can be extremely hard to fix and may need a different way of expressing the same logic.
- **[verified] GCC without optimization looks nothing like CodeWarrior:** frame pointer in `r31`
  (`mr r31, r1`), every variable written back to the stack after each statement, epilogue through
  `r11`. A binary can contain code from more than one compiler.

GCC 2.95 (SN ProDG) at -O0
--------------------------

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
  add a dummy static of the missing size; the DOL will not hash otherwise. Uninitialised
  non-static globals become COMMON symbols instead and land elsewhere, so use `static` (or an
  explicit `= 0` for `.sdata`) to control the section.
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
