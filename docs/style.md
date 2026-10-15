Code style and audit
===================

How the C in `src/` reads, and what an audit pass checks. The match decides what the compiler must
produce; this page decides how the source reads. The goal is code as close to EA's own source as
the match allows: a reader who knows the game can follow each file without knowing how it was
decompiled. EA's 32-bit habits stay (pointers in `int`, literal sizes, type puns).

Every change ends with `build/GW4E69/main.dol: OK`. `python tools/quality/lint.py <files>` checks
the mechanical rules and compiles each file to catch undefined behaviour.

The first half of this page is the conventions: what correct code looks like. The second half,
"The audit", is what fails a pass and what it becomes.

Files
-----

- **Header comment.** Every file opens with one to three lines saying what it does in the game:
  which system or game mode, and the main data it works on.
- **File names**, strongest evidence first: EA's own (an assert string in the binary, or the file
  in a related EA build, `reference/`), then a TW06 class name the code confirms, then our own.
- **Order inside a file:** includes, types local to the file, data, prototypes of static
  functions, then functions in address order (the binary fixes the order).

Names
-----

- **Everything is named:** functions, globals, struct fields, parameters and locals. No
  `fn_XXXXXXXX` or `lbl_XXXXXXXX` survives an audit.
- **A name says what the code does.** Where EA's own name is known (its text in the binary, or a
  related EA build in `reference/`), use it as EA spelled it; otherwise name it from a careful
  reading of the code. Either way the name is ours to stand behind: it is not cited anywhere, and
  it never claims more than the code shows.
- Renames go through `tools/naming/name.py` (functions) or `tools/naming/rename.py` (globals),
  which change `config/GW4E69/symbols.txt`, the C and the headers together, so the C and the
  assembly agree; never alias with `#define`.
- **EA's naming style.** Functions `System_Verb` (`View_SetCamera`, `Session_SetNumPlayers`),
  globals `gName`, locals and fields in Hungarian: `n` integer, `f` float, `b` flag, `u` unsigned
  bit mask, `p` pointer, `a` array, `pfn` function pointer, `sz` string. A player index is
  `nPlayer`, a hole `nHole`, a view `nView`.
- **Struct fields** are named for what they hold, worked out from every place that uses them. Only
  bytes no code ever reads or writes (padding, unused space) are `unkXXX`. Fields of shared structs
  carry their offset in a trailing comment (`// 0xC38`).
- **Values with a meaning** (camera modes, game types, message IDs, flag bits, a table's size)
  get an enum or define: EA's when known, otherwise ours. Plain arithmetic constants stay numbers.
- **Flags:** a field of bits is `uFlags` (`uXxxFlags` when a struct has several). Its bits are
  defines next to the struct, named for the struct: `PLAYER_IS_HUMAN` for
  `Player.uFlags`. A table's size is `SYSTEM_NUM_THINGS`.
- **Macros** are for real abstractions (`PLAYER(i)`, a table's row count), next to what they
  describe. Not for shortening a name.

Comments
--------

A comment only ever helps a reader understand the code: what it does, what it references, what it
means in the game ("a player who can't beat double bogey is done with the hole"). It says what the
code alone can't, and every claim in it is checked against the code. A wrong comment is worse than
none.

A comment never talks about anything else: not how the code was found, matched, named or split,
not other builds, not tools, not history. The one exception is the `fake match:` tag (below),
because it marks a spot to come back to.

Bugs in EA's own code are listed in [`game/ea-bugs.md`](game/ea-bugs.md), not in the source.

Data and types
--------------

- **One declaration per global, one definition per type.** A global's `extern` lives in a header
  with one type everywhere; a struct used by two files is defined once, in a header. No `typedef`
  or `extern` in a `.c` file. `python tools/quality/typeaudit.py` lists the exceptions.
- **Struct fields, never raw offsets.** `gPlayers[n].nScore`, not `*(s32*)((u8*)p + 0xC38)`. If the
  field is missing, add it to the struct in the right header.
- Player access the way EA wrote it, `gPlayers[nPlayer].field` each time; see
  [`compiler/decomp-notes.md`](compiler/decomp-notes.md), "Structs, arrays and pointers", for when
  `PLAYER(i)` or a local pointer is needed instead.
- A table gets a `typedef struct` for one row, in the header of the system that owns it.
- Every shared struct with a proven size gets `LAYOUT_ASSERT(Type, size)` after its definition
  (`platform.h`; checked only in the GameCube build).
- **A helper used by several files** is defined once, in the header where EA kept it, not copied
  per file.

Prototypes
----------

- **A function called from another file** is declared once, in the header of the system it belongs
  to, with the definition's exact types. A prototype inside a `.c` file is only for that file's
  own static functions.
- **A function only its own file calls is `static`**, with `scope:local` on its line in
  `symbols.txt`. Never give it a header prototype just to quiet lint.
- **A function is never called through a cast of itself.** When callers pass something other than
  what the definition takes, the definition (and its one prototype) is wrong: fix it to take what
  the callers pass.
- **Headers:** `engine.h` (memory, math, streams, events, sound), `camera.h` (views and cameras),
  `character.h` (the golfer's character and animation), `physics.h` (lies, shot kinds, clubs),
  `ball.h` (the ball, surfaces, the hole's terrain), `golfer.h` (players and golfers), `game.h`
  (game manager, rounds, modes, golfer states, sessions), `game/save.h`, `game/earnings.h`,
  `game/modes/*.h`, plus one header per subsystem (`lighting.h`, `fog.h`, `holeloader.h`...). A
  header includes only headers below it (`game_types.h` < `platform.h` < `engine.h` < `camera.h`,
  `character.h`; `physics.h`, `ball.h` < `golfer.h` < `game.h`).
- CodeWarrior rejects a second prototype whose parameter types differ (even `int` vs `s32`). When
  one call needs another argument type to match, cast that argument at the call site (never the
  function).

Fake matches
------------

A fake match is a compiler trick: code that exists only to steer the compiler, which no programmer
would write for the program's sake. A filler function that only orders constants, a dead store, an
empty statement or `volatile` that only moves registers, an `asm` block or `#pragma` that pins a
register. It is tagged, in one line naming the trick, so it can be found and fixed later:

```c
static f32 Grass_OrderConstants(f32 x) {   // fake match: never called, orders the float constants
```

Plain, plausible C is not a fake match, even when it looks odd: an extra cast, a copy into a local,
a `switch` where an `if` would do, a declaration order, repeated `gPlayers[n].field`. We can't know
EA didn't write it, so it gets no tag and no comment justifying it.

A fake match never changes what the game does. A pass never adds a new trick: it removes one, or
leaves it tagged. Inline `asm` is for code EA wrote in asm (the paired-single math helpers).

Odd code vs wrong code
----------------------

A matching decomp may look strange. It may not be wrong.

**Fine as it is:** unused variables or parameters, odd casts and signed or unsigned comparisons,
redundant assignments, strange control flow or declaration order.

**Fixed even when the function matches**, because matching bytes don't make the C right:

- a non-void function that can reach its end without `return` (`lint.py`: `ub-missing-return`)
- reading a variable before it is set (`ub-uninitialized`)
- indexing past an array's declared size: the declaration is wrong, fix the size
- a struct field whose type or offset disagrees with how the code uses it
- a call with no prototype in scope, or through a prototype that disagrees with the definition
- a call that only works because a register still holds a value from the call before it
- shifts by the type's width or more, signed overflow the code relies on, a variable modified
  twice without a sequence point

When the bytes prove EA wrote the bug, keep it (the match requires it) and add it to the bug
register.

Types and platform
------------------

- `game_types.h`: `s32`/`u32` are `long` in the GameCube build (CodeWarrior treats `int` and `long`
  differently, and some functions only match with `long`).
- `platform.h` (included by `engine.h`): the C library prototypes, declared once, and
  `LAYOUT_ASSERT`.
- Test `__MWERKS__` only around CodeWarrior syntax (`asm`, intrinsics) in files the Common library
  shares with ProDG, which doesn't define it. Never to keep a second, plain-C version of a function.
- Data read from the disc or the memory card is big-endian: read it through the loader's
  byte-order helpers.

Formatting
----------

- 4 spaces, no tabs. Opening brace on the same line, functions included. Braces on every
  `if`/`for`/`while` body, except a one-line early exit (`if (nPlayer < 0) return 0;`, `break;`,
  `continue;`).
- `if (a == 5)`, `for (i = 0; i < n; i++)`: spaces around binary operators and after keywords, none
  inside parentheses.
- Lines up to about 110 columns. LF line endings, no trailing whitespace, a final newline.
- Declarations at the top of the block (C89, as the compiler needs).

The audit
=========

An audit pass takes a file to done. **Done means done:** every function, global, field, parameter
and local is understood and named, every comment is checked against the code, and the only thing
left open is a tagged compiler trick. Names never claim more than the code shows, but the work to
understand the code is always done: it's all there.

**What a pass covers.** A file's pass owns everything the file defines, wherever it shows up: its
functions' signatures and every call to them (a wrong cast in a caller's file is fixed in this
pass), its globals and their declarations, and the struct fields its code reads, writes or
passes on (their names, types and every use of them, in any file). It also fixes what the file reads from elsewhere when
the file can't read right without it, such as a header field it uses. Other files are changed
only where they meet this file; they stay `open` until their own pass. A new define replaces
every test of that field across the repo, so no file keeps a bare bit the header names. Renaming
or moving another file (to free EA's name for this one) doesn't audit it: its old text stays for
its own pass. Anything else the pass finds outside that scope (a tool bug, another file's stale
comment) goes in a commit of its own, or is left for that file's pass.

Progress lives in [`audit.tsv`](audit.tsv), one row per game source file: `open`, or `done` with
the commit that holds the pass and the reviewer who passed it. The author of a pass leaves the
row `open`; the reviewer, who did not write it, marks it done. CI fails if a done file picks up a
lint finding, so done stays true. `python tools/quality/audit.py` shows the count, and
`--sync` updates the rows after files are merged, split or moved (a merged file is done only when
all of it is).

Each item below fails a file. "Becomes" is the fix.

Comments
--------

| Fails | Example | Becomes |
|---|---|---|
| **A1.** A header that tells the decomp's history: splits, dates, address ranges, section ownership, compiler flags | "Split 2026-09-27 ... This part (0x8002D8A8-0x8002DB80) owns .sdata 0x802810B0" | 1 to 3 lines on what the file does in the game |
| **A2.** Where a name came from: TW06, TW07, PDB, DWARF, asserts, "EA's name" | "gpGame->pfnGetCurrentLead (TW06: GetCurrentLead)." | Deleted |
| **A3.** Matching notes: registers, scores, compiler behaviour, "the original does X, ours Y" | "keeps pData in r30 (ours: r29): 95%." | Deleted; a real trick gets a one-line `fake match:` |
| **A4.** Addresses, offsets or raw field names narrated in prose | "the .sdata2 0x80283E90-0x80283EC0 and the .data ..." | The fields and globals get names; the comment goes |
| **A5.** A comment that restates the code, or only points at another name ("(Player.fSpeed)") instead of saying what the value means | "// Turns the IK on or off." over `gSkelIKEnabled = bOn;` | Deleted, or says what the value means |
| **A6.** A comment that's wrong, stale or a guess. A claim about what another function does is checked by reading that function | "most are not decompiled yet"; "frees the ball" over a call that only clears its flag | Fixed against the code, or deleted |
| **A7.** `port:` and `EA bug:` notes | "// port: untested, the plain-C version ..." | `port:` deleted; an EA bug goes to `game/ea-bugs.md` |

Code that isn't EA's
--------------------

| Fails | Becomes |
|---|---|
| **B1.** A plain-C copy of a function for a port (`#ifdef __MWERKS__ ... #else`) | Only the code EA shipped |
| **B2.** An invented filler function | Removed if the match holds without it; otherwise tagged as a trick |
| **B3.** `asm` or a `#pragma` that only pins the compiler | C, or tagged as a trick |
| **B4.** Per-file copies of one helper (`Ball_Vec3Add`, `BreakLine_Vec3Add`) | One definition, in the header where EA kept it |
| **B5.** A compiler trick with no tag, or a tag (and justification) on plain C | A trick gets a one-line `fake match:`; plain C loses the tag and the comment |

Names
-----

| Fails | Becomes |
|---|---|
| **C1.** A `fn_`/`lbl_` name, or a hybrid (`fn_800C8C70_Read`) | A real name |
| **C2.** A name that claims more than the code shows, or reuses a name another struct or function already uses for something else | A name the code supports, used for one thing |
| **C3.** A field named by its offset (`p->nC38`, `fF08`) that any code touches | A name for what it holds |
| **C4.** Locals and parameters like `a`, `b`, `p`, `t0`, `temp_r3` | Hungarian names for what they hold; a parameter the function never reads takes the name of the matching parameter of the function its callers mirror |
| **C5.** A value with a meaning left as a bare number: a flag bit, a group of bits, a count or table size | EA's enum or define, or ours, used everywhere the value is |

Structure
---------

| Fails | Becomes |
|---|---|
| **D1.** Another file's prototype declared in a `.c` file; a non-static prototype for a function only this file calls; a call through a cast of the function | One prototype, in the owning header; `static`; a definition that takes what the callers pass |
| **D2.** Raw pointer arithmetic (`*(s32*)((u8*)p + 0x10)`) | Struct fields |
| **D3.** A file that isn't one EA source file (`CodeXXXXXXXX.c`, `unsorted/sweep_*`), a boundary that isn't proven, or our name on a file whose EA name is known | Its functions in the EA file they belong to, under EA's name; the proof of each boundary goes in the commit message. "Nothing else claims these functions" is not a proof |
| **D4.** Undefined behaviour the compiler can see, not listed in the bug register | Fixed, or listed when EA wrote it |

Not failures
------------

- EA's own habits, even ugly ones: repeated `gPlayers[n].field`, `while` loops, `int` vs `s32`.
- `asm` EA really wrote.
- Plain C that looks odd but is real C a programmer could write, untagged.
- A `fake match:` tag on a real compiler trick, one line naming it.

Who checks what
---------------

- **Tools:** the build (`main.dol: OK`), and `lint.py` for C1 (`placeholder-name`), D1
  (`foreign-prototype`, `should-be-static`, `cast-call`, `dup-prototype`, `proto-mismatch`), D2 (`raw-offset`), D4 (the `ub-` rules),
  A7 (`old-label`), and comments that name another build, a register or an
  address (`comment-other-build`, `comment-register`, `comment-address`: the plain cases of A2-A4).
  It also counts the sweep code still waiting to be cleaned up.
- **A reviewer:** everything that needs judgment (A1, A5, A6, B2-B5, C2-C5, D3). The reviewer is
  never the person who wrote the change, and checks every surviving comment and name against the
  code. `python tools/quality/lint.py --diff main` shows what the pass added to every file it
  touched, not only the audited one; the pass adds no finding anywhere. The reviewer then marks
  the row done, in a commit of its own.
