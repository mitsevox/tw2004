# Running agent lanes (the orchestrator's playbook)

One session, the **orchestrator**, runs parallel agent **lanes**, reviews and merges their work, and
keeps the docs current. Lanes do the work (now: naming and comments); the orchestrator keeps quality uniform and
its own context small. Start of a session (or after a compaction): read `../CLAUDE.md`, then
`state.md`, then `python tools/agents/status.py`. If state.md says PAUSED, wait for the owner.

## Layout

| Where | What |
|---|---|
| `agents/state.md` | current state only: numbers, running lanes, parked decisions, follow-ups |
| `agents/brief.md` | what every lane reads first (rules, environment, reporting) |
| `agents/roles/matching.md` | a matching / link lane |
| `agents/roles/naming.md` | a naming lane: name and comment functions in one reading (`tools/match/name.py`) |
| `agents/findings/` | audit reports (EA bugs, misfiled units), name evidence, research that feeds naming and cleanup |

The matching-era records (the per-function tried-ledger `agents/tried/`, lane assignments
`agents/assign/`, the data-linking role and orphan-data map) were removed after 100%; they are in git
at commit `6839245`.
| `tools/agents/` | `new_agent.py`, `merge.py`, `review.py`, `status.py`, `remain.py` |
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

Every fact in a prompt comes from the repo or a reference build (owner, 2026-09-28): describe a file
by its own header comment, EA's text or a related build's file name, and cite it. Where nothing
says what a file does, the prompt says "purpose unknown: read it from the code". Never fill a gap
from memory (the "Road to the Emerald" gloss for RTE was invented; RTE is TW07's real-time events).

Give each lane its own files (units). Shared headers: add, never change what others use; a change
that touches many lanes' files (a prototype's parameter order, a struct layout) is scheduled for one
lane while the others avoid those call sites. Rotate unit lists between rounds (fresh eyes).

## Per report

1. Read the diff (`git -C <worktree> diff main...agent/<lane>`): style, fake-match labels, UB,
   shared-file edits, surprising claims (verify them against the asm yourself).
2. `python tools/agents/merge.py <lane> ["message"]`, one lane at a time. It merges without
   committing and runs the gates: `main.dol: OK`, `ninja all_source`, exact functions by address (a
   rename is not a loss), lint on merged lines, typeaudit / symaudit counts, constcheck (constant
   values), stripped backslashes, and renames (each renamed function needs its name_sources.tsv
   row). It commits and pushes
   only if all pass, else undoes the merge. A conflict goes back to the lane (`git merge main`).
3. Verified findings go into `docs/decomp-notes.md` (a docs commit), and into the next prompts.
4. Tell the owner what landed, in game terms, with the numbers side by side.

## Outside agents and PC jobs

- **Outside agents** (Gemini, ChatGPT/Codex on the owner's Mac) get a written assignment
  (`agents/assign/<date>-<who>.md`, created when needed): disjoint unit lists (two agents on one unit tangle), their own
  worktree and branch (`gemini/...`, `codex/...`), the merge rules, what to report. The owner hands
  it over; update the file (a dated UPDATE section on top) when plans change.
- **Merging their branches**: read the diff, then `git branch -f agent/<name> origin/<branch>` and
  `python tools/agents/merge.py <name>`. Check what the gates cannot: fake-match labels, logic kept,
  EA's form preferred over a fake (portability is not a reason to change EA-style C: a `// port:`
  note instead), no pasted asm (the asm gate refuses it). If two branches solved the same function,
  keep the one closer to how EA wrote it.
- **Heavy jobs** (long permuter or sweeper runs) go to the owner's PC runner:
  `docs/infrastructure.md` "Heavy jobs". Read the `pc-results/` branch, apply a hit by hand.
- After merging, clean up with the branch-cleanup workflow (`docs/infrastructure.md`).

## Gates and invariants

- `main.dol: OK` after every merge; only exact 100% counts; no function loses its exact match.
- CI on main is checked after every push (GitHub Actions "Build"); a red run is fixed before the next
  round. It was red from 2026-09-29 00:24 to 14:10 UTC unseen (a file-case rename left a stale object
  in CI's build cache; fixed by tools/build/prune_stale.py in the workflow). After renaming, splitting
  or folding a unit, run `python tools/build/prune_stale.py` locally too.
- Parity counts never rise: `typeaudit.py --count`, `symaudit.py --count`, whole-file `lint.py`.
- A unit is DONE when every function is exact, its data is in C and it is linked (Matching).
- Naming lanes hand in batch files; the orchestrator replays them on main with `tools/match/name.py`
  (merging their branches conflicts on callers in other files). The replay (2026-09-29, after rounds
  of silent losses): `tools/agents/check_batches.py` on EVERY lane's files together before anything
  is replayed (malformed rows, one name given twice, a name main already has); then per lane
  `tools/agents/replay_lane.sh`, which stops on a refused row, a failed rename, a conflict or a build
  that is not OK, and ends with `tools/agents/lanediff.py`: every lane line main does not have.
  A lane is merged only when lanediff's lines are each carried over or explained in the round's log.
- Lint covers headers too (long lines; `tools/match/wraphdr.py` rewraps comments). A failure or rule
  slip a lane reports is either fixed at its cause (tool, prompt, rule) before the next round or put
  to the owner; it is never reported as "minor" and passed over.

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
