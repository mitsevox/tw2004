# Contributing to the TW2004 decomp

A matching decompilation of Tiger Woods PGA Tour 2004 (GameCube, GW4E69). Build with
`python configure.py && ninja`; every commit must end with `build/GW4E69/main.dol: OK`. The build
needs `orig/GW4E69/sys/main.dol`; a cloud session gets it with `tools/cloud/setup.sh`.
How the C must read: [`docs/style.md`](docs/style.md). Compiler behaviour:
[`docs/compiler/decomp-notes.md`](docs/compiler/decomp-notes.md).

Rules:

- Write what EA wrote. A fake match is a last resort, marked `// fake match:`, and never changes
  what the game does.
- Every function and global gets a name, logged with its evidence in
  `config/GW4E69/name_sources.tsv` (`tools/naming/name.py`).
- Comments explain what the code does in the game. No `port:` notes; EA bugs go in
  `docs/evidence/notes/2026-09-28-ea-bug-register.md`, not in the code.
- No game files or official SDK files in git. `extern/` holds only public decompilations.
- Edit C and headers with an editor or a saved script, never with sed or heredocs (they mangle
  backslashes).
- Never delete anything you have not checked first.
- Ask the owner (mits, `mitsevox`) before downloads, purchases or posting anywhere. Tokens are
  the owner's; never ask for their values.
