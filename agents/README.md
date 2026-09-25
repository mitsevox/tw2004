# Running agent lanes (the orchestrator's playbook)

One session, the **orchestrator**, runs parallel agent **lanes**, reviews and merges their work, and
keeps the docs current. Lanes do the matching / auditing; the orchestrator keeps quality uniform and
its own context small. Start of a session (or after a compaction): read `../CLAUDE.md`, then
`state.md`, then `python tools/agents/status.py`. If state.md says PAUSED, wait for the owner.

## Layout

| Where | What |
|---|---|
| `agents/state.md` | current state only: numbers, running lanes, parked decisions, follow-ups |
| `agents/brief.md` | what every lane reads first (rules, environment, reporting) |
| `agents/roles/matching.md` | a matching / link lane |
| `agents/roles/audit.md` | the blind name-and-comment audit (lane 1, lane 2, reconciler) |
| `agents/roles/data.md` | attaching orphan data to its owning unit |
| `agents/tried/` | **the tried-ledger: one file per function not yet exact** (every attempt, scores, sweep results); lanes read it first and add to it before stopping (`tools/agents/triedledger.py` starts files for new functions, never overwrites) |
| `agents/notes/` | older per-lane notes (history; their content is collected into `agents/tried/`) |
| `agents/findings/` | audit reports (EA bugs, misfiled units), the orphan-data map, older findings |
| `tools/agents/` | `new_agent.py`, `merge.py`, `audit_apply.py`, `review.py`, `status.py`, `remain.py`, `phase2b.py` |
| `docs/reference-builds/` | TW06 (PS2/Xbox) and TW07 (PS3 debug build) inventories; `tw07-ps3/cu/` = EA's functions per source file with parameters and locals; `tw07-ps3/pairs.tsv` = our address -> TW07 function |

Paths (`tools/agents/paths.py`): the main checkout, worktrees in `<parent>/tw2004-agents/<lane>`
(branch `agent/<lane>`), scratch in `<parent>/scratch/tw/agents/<lane>` (never committed). Override
with `TW_MAIN`, `TW_WORKTREES`, `TW_SCRATCH`, `TW_PERMUTER`.

## Starting a lane

1. `python tools/agents/new_agent.py <lane>`: worktree + branch + scratch folder, configured and
   built (it shares the main checkout's compilers; never build a new worktree by hand).
2. Launch the agent (background), prompt = "Read agents/brief.md and agents/roles/<role>.md
   completely and follow them" + lane name, worktree, scratch folder, the units (in priority order),
   and a CHECKPOINT time. Reuse a finished lane's worktree for the next assignment.
3. Record it in `state.md` (lane, units, checkpoint).

Give each lane its own files (units). Shared headers: add, never change what others use; a change
that touches many lanes' files (a prototype's parameter order, a struct layout) is scheduled for one
lane while the others avoid those call sites. Rotate unit lists between rounds (fresh eyes), and
tell lanes to read `agents/tried/<fn>.md` for what was already tried. When merging, check the lane
added its attempts to the ledger files of the functions it worked on, and that solved ones say SOLVED.

## Per report

1. Read the diff (`git -C <worktree> diff main...agent/<lane>`): style, fake-match labels, UB,
   shared-file edits, surprising claims (verify them against the asm yourself).
2. `python tools/agents/merge.py <lane> ["message"]`, one lane at a time. It merges without
   committing and runs the gates: `main.dol: OK`, `ninja all_source`, exact functions by address (a
   rename is not a loss), lint on merged lines, typeaudit / symaudit counts, constcheck (constant
   values), stripped backslashes, and the **audit baseline** (a renamed function is refused; removed
   or changed comment lines are printed: they must be matching notes only). It commits and pushes
   only if all pass, else undoes the merge. A conflict goes back to the lane (`git merge main`).
3. Verified findings go into `docs/decomp-notes.md` (a docs commit), and into the next prompts.
4. Tell the owner what landed, in game terms, with the numbers side by side.

## Gates and invariants

- `main.dol: OK` after every merge; only exact 100% counts; no function loses its exact match.
- Parity counts never rise: `typeaudit.py --count`, `symaudit.py --count`, whole-file `lint.py`.
- A unit is DONE when every function is exact, its data is in C and it is linked (Matching).
  Linking a unit also attaches its orphan `.bss` (roles/data.md).
- `python tools/match/auditbaseline.py` after merges: "changed" rows must be explainable (added
  matching notes, code changed by a matching fix). Never rewrite `config/GW4E69/audit_baseline.tsv`
  by hand; `--rehash` only when the fingerprint method itself changes.

## Pacing (measure, don't guess)

- Check plan usage before launching. Stop launching Opus lanes at ~88% of the 5-hour window and
  never let lanes run into 100%. Weekly usage is the other wall; the owner sets the stop point.
- Measured 2026-09-24: 4 Opus lanes for ~1 hour cost ~2% of the weekly limit and ~10% of a 5-hour
  window; 14 lanes burned ~40% of a 5-hour window per hour. Size the lane count to the window.
- Replace a lane with a fresh agent past ~350K tokens (long contexts slip on rules).
- The permuter is CPU-heavy: `tools/match/permute.py`, `-j 2`, one run per lane, time-boxed.

## Decisions already made (apply consistently)

- File names: EA's (asserts) > TW06/TW07 source name proven by the code > our own ("(our name)").
- Prototypes live in headers (game.h, engine.h, golfer.h, ball.h); one declaration per global with
  its true type; no `extern`/`typedef` in .c files; cast at an odd call site.
- Game code builds with `-pragma "pool_data off"` (EA's setting); per-unit exceptions in configure.py.
- Units are finished when linked, not when 100%: prefer finishing and linking over opening new units.
- Never `git stash` (shared by all worktrees) and never check out another branch in a worktree.
