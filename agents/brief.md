# Agent brief (every lane reads this first)

You are one of several agents working in parallel on the Tiger Woods PGA Tour 2004 (GameCube)
matching decomp. An orchestrator reviews your work, merges it into `main` and pushes it. Code
quality matters as much as the match: experienced decomp people will review it, and every agent must
write as if one author wrote everything. The hard rules in `../CLAUDE.md` apply to you; this file
adds how lanes work. Then read your role file in `agents/roles/`.

**The goal is EA's code, exactly as EA wrote it (owner, 2026-09-26).** A PC port comes later and
starts from that code; the decomp does not bend the C to suit a port. In order:
1. **EA's own form first, always.** EA's types, casts, pointers kept in `int`/`long`/`u32`, 32-bit
   assumptions, type puns, magic sizes: if that is how EA wrote it, that is the right C. Where it
   would bite a 64-bit port, add a `// port: <what>` note next to it; never rewrite it.
2. **Only when EA's form is out of reach (the endgame of a function):** a labelled
   `// fake match: <why>` that leaves the logic exactly unchanged (same behaviour for every input).
3. **Never** change what the function does to satisfy the compiler.
The C must still mean what the original did: every prototype matches its definition; values go
through arguments and returns, never by luck in r3; real struct fields where EA had fields; one
shared definition of shared data.

**Checkpoints:** your prompt gives a stop time. Stop there (check `date`), commit, and report, even
if things are going well.

## Your environment

- Your checkout is a git worktree (in your prompt), on branch `agent/<lane>`. Run every command from
  it; tools in `tools/match/` work on the checkout they are run from. Never touch the main checkout
  or another lane's worktree.
- Your scratch folder (in your prompt) holds m2c output, trial scripts and notes. Nothing from
  scratch goes into the repo. Write your per-function notes there as you go (tried / result).
- Start: `date`; `git merge main`; `python tools/match/rename.py config/GW4E69/name_sources.tsv
  --refs-only` (functions get renamed on main by the audit; this rewrites old names you use; again
  after every `git merge main`); `python configure.py && ninja` (`main.dol: OK`);
  `ninja build/GW4E69/report.json`.
- Merge main again right before your final commit and report, then rebuild and lint.
- Never `git stash`, never check out another branch, never push, merge into main or rebase.
- Permuter: only `python tools/match/permute.py <Unit> <fn> --minutes 15 -j 2`, one run at a time,
  for a function >= 95% after manual tries. Its "score 0" can change behaviour (it ignores branch
  targets) or rounding (a fused multiply-add on the other product): read the diff's meaning.
- Don't write "wait until the process is gone" loops that search process command lines for a name
  (the search finds itself). Before you stop, make sure nothing you started is still running.
- The compiler baseline is GC/2.5; GC/2.0 is not ruled out (docs/compiler.md). If a near-miss
  around u8 masking, byte indexes or bit-fields is stubborn, try the unit with
  `mw_version="GC/2.0"` in its configure.py Object (never commit that); a win is a major finding.

## Hard rules for lanes

- **Commit in your branch only**, plain messages like the history
  (`GameMode8.c: fn_800FA9E0, fn_800FAAB8 exact (43/66)`), never a Co-Authored-By line or AI footer.
- **Never delete any file** (no `rm`, not even scratch temp files, and never anything outside your
  worktree and scratch: `rm -f /dev/null` broke a cloud container on 2026-09-24). `git checkout --
  <file>` to undo your own edit is fine.
- **A helper you add for a match has no name evidence:** name it after its caller,
  `fn_<caller address>_Read` (or `_Calc`, `_Get`: mechanical, never a guess at its job), with a
  `// fake match:` comment. A name that says what it does needs the audit (docs/style.md).
- **Edit C and headers only with the editor tools.** For one edit repeated in many places, save a
  Python script with the Write tool, run it, read the whole `git diff`, and name it in your report.
  No sed / heredocs / `python -c` / `python -` on anything, scratch scripts included.
- **Float constants: EA's exact expression** (`1.0f/72.0f`, `59.94f/60.0f`), never a rounded
  decimal: objdiff masks constant values; `constcheck.py` does not.
- **Names and comments:** write only matching notes (`fake match:`, `EA bug:`, `port:`, register /
  data-order / section notes). Never rename a function, never change or remove an existing comment,
  never write a behaviour comment. Keep `fn_`/`lbl_` names and prefix+offset fields.
- **Do not edit `docs/`.** Report findings instead.
- **Shared files:** headers: add fields (offset order, `// 0xOFFSET` comment) and prototypes; change
  an existing one only with proof, and list it. `configure.py` / `splits.txt`: only through
  graduate.py / mkunit.py / fold.py, or by hand for a unit's data ranges (say so). No `extern` or
  `typedef` in a .c file; no private local view of shared data.
- **A call that passes more arguments than the callee takes:** if every caller does, give the
  definition an unused parameter with a comment; otherwise cast at the odd call site, marked
  `// port: EA passes an argument fn_X ignores`.
- Before every commit: `ninja` shows `main.dol: OK`; `python tools/match/lint.py --diff main` shows
  0 findings; `typeaudit.py --count` and `symaudit.py --count` are no higher than on main.

## Code quality

- `docs/style.md` is the standard. `src/GameMode14.c` and `src/GameModeStableford.c` are the models.
- Anything unnatural that exists for the match gets `// fake match: <why>`. Normal EA style needs no
  comment. Undefined behaviour is a bug even in an exact function; if the original really does it,
  keep it and mark it `// EA bug: <what>` (and `// port:` how a port should write it).
- **Fake matches are the fallback, not the first try (owner, 2026-09-25/26)**: allowed when EA's
  own form can't be found, labelled `// fake match: <why>`, logic exactly unchanged (same behaviour
  for every input), no reliance on registers or stack layout, no asm. An unused local, a copy
  through a temp, a declaration order, an identity inline, a cast that changes nothing: fine.
  Portability is NOT a reason to reject an EA-style form (see the goal above).
- **Compiler settings (owner, 2026-09-26):** a flag that makes a WHOLE file match (configure.py
  `extra_cflags`) is EA's build setting: use it. A `#pragma` wrapped around ONE function (e.g.
  `opt_dead_assignments off` ... `reset`) is a fake match: endgame only, labelled `// fake match:`,
  and it may stand in for a truer EA form. If one pragma fixes several functions of a file, try it
  file-wide. A 100% decomp can be cleaned up later; an unlinked unit cannot be ported at all.
  Permuter results rejected earlier as "fake" qualify under this rule if they keep the logic.
- If a function will not match after a real effort (~20 minutes: the decomp-notes "Try these first"
  list, `trial.py`, TW07's locals), leave it at its best score, clean and readable, note what you
  tried, and move on.

## Read first

0. **Before touching any function: `agents/tried/<fn>.md`** (the tried-ledger, one file per function
   not yet exact). Everything anyone tried on it is there. Never repeat a listed attempt unless you
   combine it with something new; add your own attempts to it before you stop (roles/matching.md).
1. `docs/workflow.md` (commands), `docs/style.md`, `docs/decomp-notes.md` ("Try these first" fully,
   then each section as needed; newest rules: "New from the first cloud lanes").
2. `include/golfer.h`, `include/game.h`, `include/engine.h`.
3. As needed: `docs/tw06-names.md`, `docs/reference-builds/tw07-ps3/` (EA's later source layout:
   parameter order, types, locals and their order often fix the last 1-5%).

## Reporting (short; the orchestrator re-runs every check)

At most 5 short lines unless your prompt asks for more: units linked; functions made exact (names);
lint + ninja OK or not; shared-file edits (one line); at most one verified finding in the form
"claim; evidence (unit fn: C before -> after, score before -> after); verified | observed". Report
facts only: if something failed or was skipped, say so.
