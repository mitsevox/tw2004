# Agent instructions

Read [CONTRIBUTING.md](CONTRIBUTING.md) before working: it defines the review tiers, the task loop, matching integrity, the evidence records and the hostile-review checklist. The [README](README.md) names the targets, the owner and the build commands.

- Keep original executables, disc images, assets and trial output out of public Git. Use the pinned, owner-approved build environment and do not alter supporting dependencies or their access controls; report dependency problems in an issue for the owner's decision.
- Work on a dedicated branch in your own checkout or worktree. Open a draft PR early with the unit names and target ids in its title; check open PRs before starting. Preserve unrelated changes and coordinate live overlaps on shared files.
- Keep recovered facts, hypotheses, function-level matches, combined-build results and review verdicts distinct. Report what was actually checked, on which revision and for which targets.
- Preserve useful partial reconstruction in normal source files as `NonMatching` objects so CI measures it. Original bytes stay in the verified linked build until promotion. Matching incompleteness alone is not a review failure.
- Record evidence for names, boundaries, types and compiler profiles in `config/<target id>/evidence.tsv` ([format](config/README.md)) in the same PR as the change it supports.
- State the PR's tier in its Scope line and complete the records that tier requires. An author cannot provide its own independent review. A `SHIP` verdict means that review passed; it does not approve a merge.
- **Only the project owner authorizes and performs merges into `main`.** No agent enables auto-merge, pushes to `main`, force-pushes `main` or another contributor's branch, or changes branch rules, repository settings, the build image or its digest. Push a work branch or open a PR only when authorized for the task, then leave it for the owner's decision.
- Address review feedback without weakening checks, hiding mismatches or changing project rules to accommodate a patch. Propose workflow improvements through a separate PR.
- Keep handoffs concise: changes, evidence, validation per target, unresolved findings and review state. Do not add running status diaries to the repository.

Before calling a shared change safe for every target, an agent has done four things, per active target:

1. Built every active target and confirmed `config/<target id>/build.sha1` on each.
2. Run objdiff per target for each touched unit and reported the `Matching` state and progress delta in the PR's Targets table.
3. Run `python3 tools/versions.py diff` on every file it added a `#if VERSION_` block to, and read the result.
4. Added evidence rows to each affected target's `evidence.tsv`.

## Project

The match is complete: every function is byte-exact and every unit links from C. Work now is the audit: making the code read the way EA wrote it. These rules come first in this repository; where they are stricter or more specific than the sections above, they win.

- **Build.** `python configure.py && ninja`; every commit must end with `build/GW4E69-rev0/main.dol: OK`. The build needs `orig/GW4E69-rev0/sys/main.dol`; a cloud session gets it with `tools/cloud/setup.sh`.
- **The audit is the review.** Passes run as [docs/audit-process.md](docs/audit-process.md) describes: one writer, one reviewer who did not write the pass, prompts in `docs/audit-prompts/`, state in `docs/audit.tsv` and `docs/audit-plan.tsv`. A reviewed pass is this repository's hostile record. PRs merge with a merge commit, so the pass commits the ledger records survive.
- **How the C must read:** [docs/style.md](docs/style.md). Compiler behaviour: [docs/compiler/decomp-notes.md](docs/compiler/decomp-notes.md).
- **Write what EA wrote.** A fake match is a last resort, marked `// fake match:`, and never changes what the game does. This is the owner's documented exception to the fake-match rule in CONTRIBUTING.md; the audit removes fake matches as it finds the real form.
- **Names.** Every function, global and field gets a name that says what the code does (`tools/naming/name.py`); EA's names come from `references/` first. Names are not cited, so `config/GW4E69-rev0/evidence.tsv` stays empty; `name_sources.tsv` is the frozen log of the earlier names.
- **Comments** explain what the code does in the game. No `port:` notes; EA bugs go in `docs/game/ea-bugs.md`, not in the code.
- **Files.** No game files or official SDK files in git. `extern/` holds only public decompilations. Edit C and headers with an editor or a saved script, never with sed or heredocs (they mangle backslashes). Never delete anything you have not checked first.
- **Ask the owner** (mits, `mitsevox`) before downloads, purchases or posting anywhere. Tokens are the owner's; never ask for their values.
