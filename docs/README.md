# Docs

What each file is for, and how far to trust it. **Current** = kept up to date; **reference** =
correct for what it covers but no longer edited; **unaudited** = written during matching, claims not
yet checked against the code (verify before relying on one).

## Start here

| File | What | Status |
|---|---|---|
| [getting_started.md](getting_started.md) | set up and build the project | current |
| [style.md](style.md) | how the C must read; where names and comments come from | current |
| [infrastructure.md](infrastructure.md) | where things run, git flow, CI, the public page, decomp.dev | current |
| [workflow.md](workflow.md) | every command: build, objdiff, the matching and naming tools | reference (to be rewritten with the new workflow) |
| [../src/README.md](../src/README.md) | the code map: 20 subsystems, what each source file does | current |

## The compiler: [`compiler/`](compiler/)

| File | What | Status |
|---|---|---|
| [decomp-notes.md](compiler/decomp-notes.md) | the rulebook: what C produces what MWCC output ("Try these first") | reference |
| [compiler.md](compiler/compiler.md) | which compiler built main.dol and how we know | reference |

## The game: [`game/`](game/)

| File | What | Status |
|---|---|---|
| [tw2004-notes.md](game/tw2004-notes.md) | everything learned about this game's code (subsystems, conventions) | reference |
| [formats/](game/formats/README.md) | the game's data file formats (containers, textures, game data) | reference |
| [format-byteorder.md](game/format-byteorder.md) | byte order of those formats (a port's checklist) | reference |
| [gameplay.md](game/gameplay.md) | gameplay logic read from the code | unaudited |
| [hypotheses.md](game/hypotheses.md) | gameplay predictions from play, to confirm in the code | unaudited |

## Evidence behind names and splits: [`evidence/`](evidence/)

| File | What | Status |
|---|---|---|
| [tw06-names.md](evidence/tw06-names.md) | TW06 function names paired to ours | reference |
| [sourcefiles.md](evidence/sourcefiles.md) | EA's original source files in link order, with evidence | reference (pre-split; src/README.md is current) |
| [filemap.md](evidence/filemap.md) | EA's `__FILE__` names pinned to functions (generated) | reference (pre-split) |
| [notes/](evidence/notes/) | kept findings: EA bug register, misfiled units, TW07/TW06 name pairing, cited evidence | reference |
| [../reference/](../reference/README.md) | other EA builds' symbol dumps (TW07 PS3, TW06, Madden 2003 PS2, ...) used for names | reference |

## Decomp-toolkit template docs: [`dtk/`](dtk/)

[splits.md](dtk/splits.md), [symbols.md](dtk/symbols.md), [comment_section.md](dtk/comment_section.md),
[common_bss.md](dtk/common_bss.md), [dependencies.md](dtk/dependencies.md): the standard dtk-template
references for the config files and linker details.
