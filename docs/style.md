Code style
==========

How the C in `src/` reads. The match decides what the compiler must produce; this page decides how
the source reads, so the whole game looks like one author wrote it. That author is EA: the goal is
EA's code as EA wrote it, 32-bit habits included (pointers in `int`, literal sizes, type puns).

`python tools/quality/lint.py <files>` checks the mechanical rules below and compiles each file
to catch undefined behaviour (see "Odd code vs wrong code").

Files
-----

- **Header comment.** Every file opens with two to four lines saying what it does in the game:
  which system or game mode, and the main data it works on. When the file name is EA's, say so in
  a word: `// GoGolfCam.c (EA's name): ...`.
- **File names**, strongest evidence first: EA's own (an assert string in the binary, or the file
  in a related EA build, `reference/`), then a TW06 class name the code confirms, then our own.
- **Order inside a file:** includes, types local to the file, data, prototypes of static
  functions, then functions in address order (the binary fixes the order).

Names
-----

- **Every function and global has a name.** No `fn_XXXXXXXX` or `lbl_XXXXXXXX` survives review.
- **EA's name first**, spelled as EA did, when EA's text in the binary or a related build that the
  code confirms gives it (tiers T1/T2). Otherwise a name read carefully from the code (T3). A name
  never contradicts the code.
- Every name is logged with its evidence in `config/GW4E69/name_sources.tsv`, through
  `tools/naming/name.py` (functions) or `tools/naming/rename.py` (globals). Renames go through
  `config/GW4E69/symbols.txt`, so the C and the assembly agree; never alias with `#define`.
- **EA's naming style.** Functions `System_Verb` (`View_SetCamera`, `Session_SetNumPlayers`),
  globals `gName`, locals and fields in Hungarian: `n` integer, `f` float, `b` flag, `u` unsigned
  bit mask, `p` pointer, `a` array, `pfn` function pointer, `sz` string. A player index is
  `nPlayer`, a hole `nHole`, a view `nView`.
- **Struct fields** are named from how the code uses them. A field nobody has worked out yet keeps
  its type prefix and hex offset (`nC38`, `fF08`); padding and fields never read are `unkXXX`.
  Fields of shared structs carry their offset in a trailing comment (`// 0xC38`).
- **Macros** are for real abstractions (`PLAYER(i)`, a table's row count), next to what they
  describe. Not for shortening a name.

Comments
--------

A comment explains how the code works and what it does in the game ("the winner takes the whole
pool"), in plain words, where the code alone doesn't say it. That's all a comment is for.

- No matching notes, offset narration, address lists, provenance essays or status reports.
  Where a name came from belongs in `name_sources.tsv`, not above the function.
- No commented-out code and no m2c leftovers (`temp_r3`, `var_r31`, `M2C_ERROR`).
- A comment is a claim: check it against the code. A wrong comment is worse than none.
- One label is allowed: `// fake match: <why>`, on code written only to make the compiler match
  (below), so it can be found and fixed later. Bugs in EA's own code are worth knowing, but they
  are listed in
  [`evidence/notes/2026-09-28-ea-bug-register.md`](evidence/notes/2026-09-28-ea-bug-register.md),
  not in the source.

Data and types
--------------

- **One declaration per global, one definition per type.** A global's `extern` lives in a header
  with one type everywhere; a struct used by two files is defined once, in a header. No `typedef`
  or `extern` in a `.c` file. `python tools/quality/typeaudit.py` lists the exceptions.
- **Struct fields, never raw offsets.** `gPlayers[n].nC38`, not `*(s32*)((u8*)p + 0xC38)`. If the
  field is missing, add it to the struct in the right header.
- Player access the way EA wrote it, `gPlayers[nPlayer].field` each time; see
  [`compiler/decomp-notes.md`](compiler/decomp-notes.md), "Structs, arrays and pointers", for when
  `PLAYER(i)` or a local pointer is needed instead.
- A table gets a `typedef struct` for one row, in the header of the system that owns it.
- Every shared struct with a proven size gets `LAYOUT_ASSERT(Type, size)` after its definition
  (`platform.h`; checked only in the GameCube build).

Prototypes
----------

- **A function called from another file** is declared once, in the header of the system it belongs
  to, with the definition's exact types. A prototype inside a `.c` file is only for that file's
  own static functions.
- **Headers:** `engine.h` (memory, math, streams, events, sound), `camera.h` (views and cameras),
  `character.h` (the golfer's character and animation), `physics.h` (lies, shot kinds, clubs),
  `ball.h` (the ball, surfaces, the hole's terrain), `golfer.h` (players and golfers), `game.h`
  (game manager, rounds, modes, golfer states, sessions), `game/save.h`, `game/earnings.h`,
  `game/modes/*.h`, plus one header per subsystem (`lighting.h`, `fog.h`, `holeloader.h`...). A
  header includes only headers below it (`game_types.h` < `platform.h` < `engine.h` < `camera.h`,
  `character.h`; `physics.h`, `ball.h` < `golfer.h` < `game.h`).
- CodeWarrior rejects a second prototype whose parameter types differ (even `int` vs `s32`). When
  one call needs another argument type to match, cast at the call site.

Fake matches
------------

Code that exists only to make the compiler emit the original instructions is marked, so a reader
knows it's deliberate and someone can come back for it:

```c
    int n;          // fake match: a copy of nPlayer, for the original register order
```

- Anything a person wouldn't naturally write gets the label: a redundant copy, an odd type, a
  useless cast, a strange statement order, a `goto`.
- Normal EA habits are not fake matches: repeated `gPlayers[n].field`, `while` loops, the
  `gPlayers[(u32)i]` index, `int` vs `s32` for a local.
- A fake match never changes what the game does. Inline `asm` is only for code EA wrote in asm,
  never to force a match.

Odd code vs wrong code
----------------------

A matching decomp may look strange. It may not be wrong.

**Tolerated when the match needs it:** unused variables or parameters, odd casts and signed or
unsigned comparisons, redundant assignments, strange control flow or declaration order,
non-idiomatic pointer arithmetic.

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
- Test `__MWERKS__` only around CodeWarrior syntax (`asm`, intrinsics). The Common library is built
  with ProDG, which doesn't define it.
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
