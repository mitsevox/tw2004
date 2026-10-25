# Docs

## Start here

- [getting_started.md](getting_started.md): set up and build the project.
- [style.md](style.md): how the C must read (names, comments, types, fake matches), and what an audit pass fixes. Progress: [audit.tsv](audit.tsv).
- [audit-process.md](audit-process.md): how the audit runs (writer, reviewer, landing, big
  files, parallel tasks), with the writer and reviewer briefs in [audit-prompts/](audit-prompts/).
  The task list: [audit-plan.tsv](audit-plan.tsv).
- [workflow.md](workflow.md): the commands to build, score, name and check a change.
- [infrastructure.md](infrastructure.md): where things run, CI, the progress page, decomp.dev.
- [../src/README.md](../src/README.md): the code map, what each source file does in the game.

## The compiler: [`compiler/`](compiler/)

- [decomp-notes.md](compiler/decomp-notes.md): what C produces what MWCC output. Read "Try these
  first" before matching anything.
- [compiler.md](compiler/compiler.md): which compiler built `main.dol` and how we know.

## The game: [`game/`](game/)

- [tw2004-notes.md](game/tw2004-notes.md): the binary (memory map, SDK, source file names,
  save files, cheat codes, disc loading).
- [formats/](game/formats/README.md): the data file formats on disc and their byte order.
- [ea-bugs.md](game/ea-bugs.md): bugs in EA's code, confirmed and possible.
- [gameplay.md](game/gameplay.md): the shot, AI and physics logic in plain English. Not yet
  checked against the finished code.
- [hypotheses.md](game/hypotheses.md): behaviour seen in play, still to confirm in the code.

## Where names come from: [`evidence/`](evidence/)

- [tw06-names.md](evidence/tw06-names.md): TW06 function names paired to ours.
- [name-pairing.md](evidence/name-pairing.md): EA name proposals from TW07 and TW06, not yet applied.
- [mad-names.md](evidence/mad-names.md): names for EA's MAD movie decoder, from two builds.
- [filemap.md](evidence/filemap.md): EA's `__FILE__` names pinned to functions.
- [../reference/](../reference/README.md): the other EA builds whose symbols the names come from.

## Decomp-toolkit docs: [`dtk/`](dtk/)

[splits.md](dtk/splits.md), [symbols.md](dtk/symbols.md), [comment_section.md](dtk/comment_section.md),
[common_bss.md](dtk/common_bss.md), [dependencies.md](dtk/dependencies.md): the standard dtk-template
references for the config files and linker details.
