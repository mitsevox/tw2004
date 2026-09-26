# TW2004 decomp: start here

A matching decompilation of Tiger Woods PGA Tour 2004 (GameCube, GW4E69). The goal is C source
that can be ported to PC and modded; the byte match (`build/GW4E69/main.dol: OK`) is the proof the
C is right, not the goal. Fidelity and parity outrank speed.

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
- **Names and comments need evidence** (docs/style.md "Where names and comments come from"): EA's
  text, a TW06/TW07 name the code confirms, or two blind readers + a reconciler (agents/roles/audit.md).
  Matching lanes write ONLY matching notes (`fake match:`, `EA bug:`, `port:`, register / data-order
  / section notes), never rename, never change an existing comment.
- **The audit baseline** (git tag `audit-baseline-1`): every game function's name and comments were
  audited to 100% on 2026-09-24. `python tools/match/auditbaseline.py` sorts code into audited /
  draft / changed / new / headers. At 100% match the code gets partitioned with it.
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

## After 100% match (parked on purpose)

- Partition code into fully / semi / not audited (auditbaseline.py), re-check draft-audited comments.
- Phase 3 audit: struct field comments in the 69 game headers; names of globals, types, fields,
  parameters were never audited.
- **docs/gameplay.md needs a full cleanup**: its behaviour claims were written during matching and
  never audited (1 in 5 code comments from that era were wrong).
- Port-hazards doc (EA bugs sorted by effect; raw material in agents/findings/).
- Misfiled units and `src/unsorted/` placement (agents/state.md follow-ups).
- **Apply the EA names found after the baseline** (the audit gate blocks renames until then): the
  MAD video decoder in rcmp_mad_codec.c (14 functions, 5 globals, TW06 PDB + NFSMW decomp agree:
  `agents/findings/2026-09-25-mad-names.tsv`) and ska_shared's SKAUtil_EulerAnglesToQTs8 /
  SKA_LoadFromMem (TW07 + 007 EoN: `docs/reference-builds/007eon-ps2`). Log them in
  `config/GW4E69/name_sources.tsv` with `tools/match/rename.py`.
