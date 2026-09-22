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
- **[verified] CodeWarrior always folds `n += 3` into the later uses** (`addi r5, rN, 3` at a call,
  `addi r0, rN, 3; cmpwi r0, 8`, ...) even with five uses and even after the `if` that follows; a
  select `n = (c ? a : b) + 3` produces one materialised add but the pre-add value in a scratch
  register. `UStream_Decompress` has an in-place `addi r28, r28, 3` we could not reproduce; the
  function is left at 98.8% with a comment. Open question.
- **[verified] Register order for callee-saved locals** follows declaration order (first declared
  gets r31). Parameters used as working pointers come after the locals; to make `pEnd` r31 and the
  destination r30, declare `pEnd` first and copy the parameters into locals declared after it.
  A `u8 v = (u8)value` local declared *before* the pointer local gets the `clrlwi` in place.
- **[verified] A function that returns its pointer argument** keeps r3 untouched and works on a
  copy in another register (`mr r6, r3`); a `void` function advances r3 directly.
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
