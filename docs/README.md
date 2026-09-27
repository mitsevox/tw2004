# Docs

What each file is for, and how far to trust it. **Current** = kept up to date; **reference** =
correct for what it covers but no longer edited; **unaudited** = written during matching, claims not
yet checked against the code (verify before relying on one); **history** = a record, not guidance.

## Start here

| File | What | Status |
|---|---|---|
| [getting_started.md](getting_started.md) | set up and build the project | current |
| [workflow.md](workflow.md) | every command: build, objdiff, the matching and naming tools | current |
| [style.md](style.md) | how the C must read; where names and comments come from | current |
| [decomp-notes.md](decomp-notes.md) | the compiler rulebook: what C produces what MWCC output ("Try these first") | current |
| [infrastructure.md](infrastructure.md) | machines, git flow, CI, the public page, PC jobs | current |

## The game

| File | What | Status |
|---|---|---|
| [tw2004-notes.md](tw2004-notes.md) | everything learned about this game's code (subsystems, conventions) | reference |
| [gameplay.md](gameplay.md) | gameplay logic read from the code | unaudited (full cleanup parked, CLAUDE.md) |
| [hypotheses.md](hypotheses.md) | gameplay guesses still to confirm; overlaps gameplay.md, to be merged into it | unaudited |
| [formats/](formats/README.md) | the game's data file formats (containers, textures, game data) | reference |
| [format-byteorder.md](format-byteorder.md) | byte order of those formats | reference |
| [../src/README.md](../src/README.md) | the code map: 20 subsystems, what each source file does | current |

## Evidence behind names and splits

| File | What | Status |
|---|---|---|
| [reference-builds/](reference-builds/README.md) | other EA builds with symbols (TW07 PS3, TW06, Madden 2003 PS2, ...) used for names | reference |
| [tw06-names.md](tw06-names.md) | TW06 function names paired to ours | reference |
| [sourcefiles.md](sourcefiles.md) | EA's original source files in link order, with evidence | reference (pre-split; src/README.md is current) |
| [filemap.md](filemap.md) | EA's `__FILE__` names pinned to functions (generated) | reference (pre-split) |
| [compiler.md](compiler.md) | which compiler built main.dol and how we know | reference |

## Decomp-toolkit template docs

[splits.md](splits.md), [symbols.md](symbols.md), [comment_section.md](comment_section.md),
[common_bss.md](common_bss.md), [dependencies.md](dependencies.md): the standard dtk-template
references for the config files and linker details.

## Other

| File | What | Status |
|---|---|---|
| [journal.md](journal.md) | the project's history, dated | history |
| [publish/](publish/) | the owner's drafts for posts outside the project | owner's |
