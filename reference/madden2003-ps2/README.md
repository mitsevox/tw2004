# Madden NFL 2003, PlayStation 2 prototype (EA Tiburon, 2002-06-06)

Derived evidence only. The executable stays outside the repo (on the research machine at
`/home/user/refs/madden03/`); nothing here is code bytes or game data.

- Source: `https://debugging.games/_files/PlayStation 2/[PS2] Madden NFL 2003 (USA) [SLUS-20529] [2002-06-06].7z`
  (downloaded with the owner's approval, 2026-09-27).
- `SLUS_205.29`: MIPS ELF (ee-gcc 2.9x; the objects name `GNU as v2.9-ee-991111b`), optimized,
  not stripped, 58,607,700 bytes,
  SHA-256 `8af61dbc3bfa6aa3cedda7fff695d524ac91af955777c2039d26db99d6c5fb29`.
  It has a `.symtab` and a 53 MB `.mdebug` section: ECOFF symbolic debug info whose local symbols
  carry gcc's STABS (function signatures, parameters, locals with register / stack slots, lexical
  blocks, every struct / union / enum / typedef of each compiled `.i` file, line labels).

## Why it matters

EA Tiburon's UI script library ("UI Studio", `IStudio` in TW2005's paths) is in it with full
debug info: `../../../Source/Common/UIStudio\` holds six files. TW2004's `src/UISEvent.c`,
`UIStudio.c`, `UISApi.c` and `UISScreen.c` are the same library a year later (same struct sizes
and offsets throughout, same functions, a few additions), so this gives EA's own names, types,
parameter names and local declarations for nearly every one of our 60 UIS functions.

## Files

| file | what |
|---|---|
| `UIStudio.c.txt` | 40 functions (screens: load / unload / activate, the `_Parse*` walkers, `PatchScrData`, init) |
| `UISEvent.c.txt` | 5 functions (rate functions) |
| `UISStack.c.txt` | `_UISPatchFncPC` and `UISStackProcess` (the script interpreter), with the opcode enum |
| `UISUtils.c.txt` | 23 functions (find screen / event PC, `UISExecuteFnc`, `UISSprintf` and its writers, hints) |
| `UISError.c.txt` | `UISRegisterRuntimeErrorFnc` and the error globals |
| `UISActionProcess.c.txt` | 7 functions (the "thread action" queue: our event stack) |
| `opcodes.md` | `UISStackProcess`: each opcode's case and the variables EA declared in that case's block |
| `pairing.md` | EA name <-> our function, with evidence; the leads for the four open functions |
| `glue/UISObj.c.txt`, `glue/UISCallback.c.txt` | Madden's game-side glue (how a game registers its callbacks with the library); for the callback signatures only |

78 library functions in all (7 + 1 + 5 + 2 + 40 + 23). Madden's other `UIS*` files
(`UISTibPlayer.c`, `UISTibModel.c`, `UISmodelobj.c`, ...) are football-specific display code and
were not extracted. No `TibExt`, `Eassdk` or `IStudio` paths exist in this build.

Each `.c.txt` lists the functions in source order: return type, name, parameters (with where gcc
kept each one), then the locals **in declaration order**, nested by lexical block, each with its
storage (`reg s0`, `stack -176`, `static`); `first line` is the source line of the function's
first statement. Then every struct / union / enum / typedef those functions use, with member
offsets. Notes on reading it:

- Register / stack slots are gcc's (MIPS), not CodeWarrior's: they say what is a separate variable,
  not how CW allocates. A local on the stack is not necessarily address-taken (gcc spills).
- gcc lists a block's variables before the block's `N_LBRAC`; the tool nests them accordingly.
  A block with no variables (`{ }`) is a scope EA wrote (or a statement-expression macro) with no
  declarations of its own.
- Function-pointer typedefs show no parameter list: STABS does not record one.
- `(none: optimized out)`: gcc kept no location for the variable.

## How it was extracted

`python tools/ref/mdebug.py <elf> cfile --file 'Common/UIStudio' --types '^_?UIS' --out <dir>`
(and `--file 'Objects\\(UISObj|UISCallback)\.c'` for `glue/`). `tools/ref/mdebug.py` (removed 2026-10-03; in git history) is a small
ECOFF reader: HDRR at the section's file offset, FDRs (0x48 bytes), local symbols (0x0C), strings;
a stab is a local symbol whose index is 0x8F300 + the stab code in an FDR that starts with
`@stabs`. Its other commands: `files`, `raw` (every symbol of a file), `type <name>` (one type
expanded), `externs`. `opcodes.md` came from a scratch script that reads `UISStackProcess`'s jump
table (0x50ECE0, 120 entries) and maps each target into the lexical blocks.
