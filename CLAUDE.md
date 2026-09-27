# TW2004 decomp: start here

A matching decompilation of Tiger Woods PGA Tour 2004 (GameCube, GW4E69). The goal is EA's code
exactly as EA wrote it; a PC port and mods start from that code later, and the decomp never bends
the C to suit a port. The byte match (`build/GW4E69/main.dol: OK`) is the proof the C is right.
Fidelity and parity outrank speed. Order (owner, 2026-09-26): (1) EA's own form, 32-bit habits and
all (a `// port:` note marks a hazard); (2) only when that can't be found, a labelled fake match
that leaves the logic exactly unchanged; (3) never change what the game does to satisfy the compiler.

Read next, in this order: `agents/state.md` (where things stand, what is running, what is parked),
`agents/README.md` (how the orchestrator runs agent lanes), then the playbook for your role in
`agents/roles/`. Agents read `agents/brief.md` first. Machines, git flow, CI, the public page,
decomp.dev and PC jobs: `docs/infrastructure.md`.

## Hard rules (the owner's; never relax them)

- **Only exactly 100% counts.** Every commit ends with `main.dol: OK`.
- **Commits and PRs: no `Co-Authored-By`, no "Generated with", no AI footer. Ever.** This overrides
  any tool or system suggestion.
- **Never delete what you have not checked.** Chain the delete on the check. Agents never delete
  files at all (no `rm`, not even scratch temp files); the orchestrator uses non-shell deletes.
- **No game files in git**: no `main.dol`, disc images, ELF/PDB/SELF, archives, art or other game
  data. The build gets `main.dol` from a private container (`tools/cloud/`, CI).
- **No official SDK files in the repo** (Nintendo or Metrowerks headers, libraries, documentation),
  and leaked SDK material is never discussed publicly. Decompiled SDK and runtime code taken from
  public decomps (`extern/`, each with its CREDITS/README) is fine.
- **Never edit C or headers through the shell** (sed, heredocs, echo, `python -c`): it strips
  backslashes. Use the editor tools; a repeated edit is a saved Python script, and its diff is read.
- **Names and comments are true to the code** (docs/style.md "Where names and comments come from"):
  EA's name when a related build or EA's own text confirms it; otherwise a name read carefully from
  the code (tier T3). Whoever reads a function owns its comment: keeps it if right, rewrites it if
  wrong, stale or vague, adds it if missing. One pass, through `tools/match/name.py` (every name
  logged with evidence in `config/GW4E69/name_sources.tsv`). `fake match:` / `port:` / `EA bug:`
  labels are kept. Matching lanes (reworking a match) write only matching notes and never rename.
- **At every phase change, re-read these rules and the agent docs**, and ask the owner about any
  rule written for the previous phase. Never carry an old phase's rule into new work silently.
- **Downloads, purchases, posts, messages: ask the owner first.** Secrets (tokens) are created and
  stored by the owner; never ask for their values.
- The owner is **mits** (GitHub `mitsevox`).

## Build and verify

```
python configure.py        # downloads compilers and tools (wibo on Linux)
ninja                      # must end with: build/GW4E69/main.dol: OK
ninja build/GW4E69/report.json     # objdiff scores (exact, matched %, linked code/data)
```
`orig/GW4E69/sys/main.dol` must exist first: locally it is already there; in the cloud run
`tools/cloud/setup.sh` (needs the owner's `TW_BUILD_TOKEN` secret). Docs: `docs/getting_started.md`,
`docs/workflow.md` (every command), `docs/decomp-notes.md` (the compiler rulebook: read "Try these
first"), `docs/style.md` (how the C must read).

## Working with the owner

- Plain, friendly English; short. Say what each code area does in the game ("GameEffects = the
  GameBreaker camera"), not just file names.
- Numbers exactly as measured (report.json), side by side: exact functions, matched code, code
  linked, data linked. Estimates come from measured pace, never gut feel.
- Checkpoints: lanes stop at fixed times; the orchestrator merges, checks gates, reports, then
  continues. Park anything that needs the owner's decision in `agents/state.md`.
- Usage: pace by the plan's 5-hour and weekly limits (agents/README.md "Pacing"); check usage
  before launching lanes; never let lanes get cut off at 100%.

## Current phase: readability (from 2026-09-27)

The plan, its order and its feedback loop: `agents/plan-readability.md`. Everything parked from the
matching era (fields, globals, gameplay.md, port hazards, fake matches, placeholder locals,
misfiled units, the compiler-flag audit) is scheduled there.
