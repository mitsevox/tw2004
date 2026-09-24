Code style
==========

How the C in `src/` is written. Matching decides *what* the compiler must produce; this file
decides how the source reads, so every unit looks like one author wrote it. Model files:
`src/GameTargets.c` and `src/GameModeReplay.c`.

The goal is source that can be ported to PC and modded. The byte match proves the C is right; it
is not the goal. So the C must mean what the original did, on any compiler: prototypes that match
their definitions, no undefined behaviour, real fields and types instead of offsets, pointers kept
as pointers, sizes written as `sizeof`, and one shared definition of shared data (see
"Portability").

`python tools/match/lint.py <files>` checks the mechanical rules below, and compiles each file
with the game's compiler to catch two kinds of undefined behaviour (see "Odd code vs wrong
code"). Run it before every commit.

Files
-----

- **Header comment.** Every unit starts with `// <File>.c (our name): <what it does>`, two to four
  lines in plain English: which game mode or system, the main data it works on. "(our name)"
  marks names we chose; EA's file names are unknown. A unit named after its TW06 class says so
  instead: `// GameModeStableford.c (TW06's GameModeStableford): game mode 18, ...`.
- **File names**, strongest evidence first: EA's own name from an assert string in the binary
  (`GoGolfCam.c`, header "(EA's name, from its asserts)"); a TW06 source file or class name when
  the code proves the match (`GoTerrainCollision.c`, `GameModeStableford.c`); otherwise our own
  (`GameMode8.c`, "(our name)").
- **Order inside a file:** includes, then types local to the file, then `extern` data, then
  prototypes, then functions in address order (the order is fixed by the binary).
- **Includes:** `golfer.h` (players, sessions, shared game types), `ball.h`, `physics.h` as needed.
  No includes of other units' `.c` files.

Names
-----

- **Functions and globals:** use a real name only with the evidence described in "Where names and
  comments come from" below. Otherwise keep the address name (`fn_800FA518`, `lbl_80211D38`). Do
  not invent names from guesses.
- **Renaming a global or function** is done in `config/GW4E69/symbols.txt`, so every file and the
  assembly see the new name. Never alias with `#define NICE_NAME lbl_XXXXXXXX`.
- **Style of names:** EA's own. Functions `System_Verb` (`View_SetCamera`, `Session_SetNumPlayers`)
  or `fn_XXXXXXXX`. Globals `gName`. Locals and fields in Hungarian with a type prefix:
  `n` integer, `f` float, `b` flag, `u` unsigned bit mask, `p` pointer, `a` array, `pfn` function
  pointer, `sz` string. Parameters that are a player index are `nPlayer`, a hole `nHole`, a
  view `nView`.
- **Struct fields whose meaning is unknown** keep their type prefix plus the hex offset: `nC38`,
  `fF08`, `aCD4[20]`. Padding and fields never read are `unkXXX`. Every field in a shared struct
  has its offset in a trailing comment (`// 0xC38`).
- **Macros** are for real abstractions (`PLAYER(i)`, a table's row count), in the header next to
  the thing they describe. Not for shortening a name.

Where names and comments come from
----------------------------------

The name and comment audit (`config/GW4E69/audit.tsv`, from 2026-09-24) found about 7% of our
function names and about 1 in 5 of our function comments wrong: swapped cases, claims the code
does not show, text copied between similar game modes, stale status notes. They had been written
during matching, where nothing checks them. So a name or a behaviour comment in EA game code needs
one of two things before it goes in:

- **Hard evidence:** EA's own text in the binary (a string that names the job), or EA's name from
  a related build that the code confirms (TW06, TW07; `docs/reference-builds/`). Say which.
- **Two independent readers:** two blind reads of the function (neither sees the current name or
  comments) that agree on what it does, reconciled against the code (`tools/match/blindview.py`,
  the audit ledger). A name that rests only on our own deduction is provisional:
  `fn_<address>_<Guess>`.

Matching work writes only matching notes (`fake match:`, `EA bug:`, `port:`, register, data-order
and section notes) and keeps `fn_`/`lbl_` names. Behaviour comments and names come from the audit
process above, never as a side effect of making bytes match.

**The audit baseline** (git tag `audit-baseline-1`, `config/GW4E69/audit_baseline.tsv`): the
function audit reached 100% (6,610 functions) on 2026-09-24, and the baseline fingerprints each
function's name, comments and code as audited. `python tools/match/auditbaseline.py` sorts the
game code into:

- **audited:** unchanged since the audit, which read byte-matching C (6,422 functions at the tag).
- **draft:** audited, but from our non-exact draft C (188 functions at the tag). Once it matches,
  re-check its comments against the final code.
- **changed:** its name, comments or code changed after the tag; `git diff audit-baseline-1 --
  <file>` shows what. A new matching note is fine, anything else needs the audit process.
- **new:** a function the baseline does not have.
- **headers:** struct field comments in game headers (phase 3, not audited).

Never audited: the names of globals, types, struct fields and parameters. Treat them as claims.
Never rewrite the baseline file by hand; a later audit milestone writes a new one with a new tag.

Data access
-----------

- **One declaration per global, one definition per type.** A global's `extern` lives in a header,
  with one type everywhere; a struct used by two files is defined once, in a header. A private
  local view of shared data (a partial struct, a `u8*` where others see a struct) is how files
  drift apart: `python tools/match/typeaudit.py` lists them, and merges may not add any.

- **Use struct fields, never raw offsets.** `gPlayers[n].nC38`, not `*(s32*)((u8*)p + 0xC38)`. If
  the field is missing, add it to the struct (with its offset comment) in the right header.
- Write player access the way EA did, `gPlayers[nPlayer].field` each time; see
  `decomp-notes.md`, "Structs, arrays and pointers", for when `PLAYER(i)` or a local pointer is
  needed instead.
- Tables get a `typedef struct` describing one row, in the header of the system that owns the
  table (no `typedef` or `extern` in a `.c` file; `typeaudit.py` counts them and merges may not
  add any).

Prototypes
----------

- **A function used by more than one file** is declared once, in the header of the system it
  belongs to.
- **A prototype local to a file** is for functions only that file calls.
- **Headers:** `game.h` (game manager, rounds, modes, golfer states, sessions), `engine.h`
  (memory, math, streams, events, sound), `golfer.h` (players and golfers), `ball.h` (the ball,
  surfaces and the hole's terrain; `golfer.h` includes it), `camera.h` (views and cameras),
  `character.h` (the golfer's character object and animation), `physics.h` (TW06's lie, shot-kind
  and club enums), `platform.h` (the C library and layout checks; see Portability),
  `game/save.h` (the save profile), `game/earnings.h` (the prize table), `game/modes/*.h` (data a
  family of game modes shares: `challenge.h`, `pgatour.h`, `rte.h`). A header includes only the
  headers below it (`game_types.h` < `platform.h` < `engine.h` < `camera.h`, `character.h`;
  `physics.h`, `ball.h` < `golfer.h` < `game.h`).
- **No per-file re-declaration with other types.** CodeWarrior rejects a second prototype whose
  parameter types differ (even `int` vs `s32`) as "identifier redeclared". When one file's calls
  need a different argument type to match, cast at the call site (`fn((u16)x)`) and say why.

Matching tricks
---------------

Code that exists only to make the compiler emit the original instructions is marked, so a
reader knows it is deliberate:

```c
    int n;          // fake match: a copy of nPlayer, for the original register order
```

- Use `// fake match: <why>` for anything a person would not naturally write (a redundant copy
  of a variable, an odd type, a useless cast, a strange statement order).
- Rules that explain *normal* EA style are not fake matches and need no comment: repeated
  `gPlayers[n].field`, `while` loops, the `gPlayers[(u32)i]` index, and choosing `int` or
  `s32`/`long` for a local (both are natural; the choice changes the code, see decomp-notes).
- No `goto` unless the control flow cannot be matched without it; mark it as a fake match.

Odd code vs wrong code
----------------------

A matching decomp is allowed to look strange. It is not allowed to be wrong. Two different things:

**Tolerated when the match needs it** (style complaints, not bugs; mark the unnatural ones as a
fake match):

- unused variables or parameters (EA's signatures are fixed by the callers)
- suspicious casts, odd signed/unsigned comparisons
- seemingly redundant assignments, or assignments that exist only for register allocation
- weird control flow, and code that looks unreachable but is there because of how the compiler
  lays out branches
- non-idiomatic pointer arithmetic
- declarations in strange places or orders, because moving them changes the match

**Investigated even when the function matches** (undefined behaviour or a real correctness
problem; the current compiler producing the right bytes does not make the C right):

- a non-void function that can reach its end without `return` (`lint.py`: `ub-missing-return`)
- reading a variable before it is set (`lint.py`: `ub-uninitialized`)
- indexing past an array's declared size: the declaration is wrong, fix the size (the replay
  array `nDE4` was declared `[18]` until code was found using 40 entries)
- a struct field whose type or offset disagrees with how the code uses it
- calling a function with no prototype in scope, or through one whose return type or parameter
  count disagrees with the definition. A deliberate per-file parameter type (see Prototypes) is
  the only exception, and it carries a comment
- shifts by the type's width or more, signed overflow the code relies on, a variable modified
  twice without a sequence point (`i++ + i++`)

When the original itself has the bug (the bytes prove EA wrote it), keep it, since the match
requires it, and say so: `// EA bug: reads nC38 of player 5, one past the last player`.

Portability
-----------

A port rebuilds this C with another compiler, likely 64-bit and little-endian. `lint.py` flags
the patterns that break there:

- `port-ptr-int`: a pointer cast to an integer (`(int)pBall`). Keep pointers as pointer types;
  a handle that is really a pointer gets the pointer's type.
- `port-literal-size`: a copy or clear whose size is a number (`Mem_cpy(a, b, 0xBC)`). Write
  `sizeof(Ball)`: the size of a struct holding pointers changes on a 64-bit machine.
- `port-frame-rate`: the frame rate as a bare number (`59.94f`). Use `FRAME_RATE` or `FRAME_TIME`
  (engine.h); CodeWarrior folds `1.0f / FRAME_RATE` to the same constant as `0.016683351f`.
- `port-asm-no-fallback`: CodeWarrior-only code with no plain-C version. An `asm` function sits
  under `#ifdef __MWERKS__` with an `#else` that computes the same result in C (marked untested
  until a port runs it; the asm body stays as it is). A compiler intrinsic (`__cntlzw`) needs a C
  fallback in game_types.h or platform.h, in the `TW_PORT` branch.

The base types and the platform layer:

- `game_types.h`: for the GameCube build `s32`/`u32` stay `long` (CodeWarrior treats `int` and
  `long` differently, and some functions only match with `long`); a port defines `TW_PORT` and gets
  `<stdint.h>` types. `uptr` is an integer as wide as a pointer: use it, not `u32`, for address
  arithmetic (alignment, an offset stored in a pointer field), e.g. `(uptr)p & 15`.
- `platform.h` (included by engine.h): the C library prototypes, declared once (a port gets the
  standard headers instead), and `LAYOUT_ASSERT(Type, size)`, a zero-code size check. Every shared
  struct with a proven size gets one after its definition. The sizes are the GameCube's 32-bit
  layouts, checked only when `TW_GC_LAYOUT` is defined (the GameCube build), so a 64-bit port knows
  which structs its layout must not rely on.
- Test `__MWERKS__` only for CodeWarrior syntax (`asm`, intrinsics). For what differs between the
  GameCube build and a port, test `TW_PORT`: the Common library is built with ProDG, which defines
  no `__MWERKS__`.

When the original's code can only be matched with one of these, keep it and say why with
`// port: <why>` on the line or the line before, so a porter knows to look there. Data read from
the disc or the memory card is big-endian and laid out for a 32-bit machine: read it through the
loader's byte-order helpers, never by laying a struct over the buffer in new code.

Formatting
----------

- 4 spaces, no tabs. Opening brace on the same line, functions included
  (`void fn_800F125C(void) {`). Always braces on `if`/`for`/`while` bodies, except a one-line
  early exit: `if (nPlayer < 0) return 0;` (also `break;`, `continue;`).
- `if (a == 5)`, `for (i = 0; i < n; i++)`: spaces around binary operators and after keywords,
  none inside parentheses.
- Lines up to about 110 columns. LF line endings, no trailing whitespace, a final newline.
- Declarations at the top of the block (C89, as the compiler needs), one per line when they carry
  a comment.
- Comments explain *why* or what the game does ("the winner gets the whole pool"), not what the
  C says. No commented-out code; no m2c leftovers (`temp_r3`, `var_r31`, `M2C_ERROR`, `?` types).

Sweep files
-----------

`src/unsorted/sweep_*.c` are machine-generated placeholders and are exempt from this file until
they are folded into a named unit. Once folded, the code follows these rules like any other.
