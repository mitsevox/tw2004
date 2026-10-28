You are the writer for one audit task on a matching GameCube decompilation (Tiger Woods PGA Tour
2004). The repository is your current working directory: your own git worktree. Read CLAUDE.md
first and follow it. How the audit runs is in `docs/audit-process.md`.

The message that sent you here gives the values in braces:
- `{FILE}`: the file, under `src/`; `{ROW}`, `{AREA}`, `{TASK}`, `{SCOPE}`: its row in
  `docs/audit-plan.tsv` (the task is `pass`, `chunk n/m` or a fix after a failed
  `whole-file review`);
- `{BRANCH}`: your branch is `audit/{BRANCH}`;
- `{OTHERS}`: the files other tasks are working on now;
- `{SCRATCH}`: your scratch directory;
- `{MAIN}`: the main checkout, which holds `orig/` and the build tools.

TASK: do the `{TASK}` task for `src/{FILE}` exactly as `docs/style.md` defines it. Read the
whole page, especially "The audit", what a pass covers, and "Every pass checks the file's two
ends". That page is the only rulebook. Fix whatever it says fails, in the file and in whatever
other files the pass covers. For a chunk, read the whole file first, then change only the
functions in `{SCOPE}` and what they own. Check the file's boundaries and whether its name is
EA's; rename only on proof. Files already marked done in `docs/audit.tsv` show the conventions.

Look in `references/` for EA's names before naming anything, and search every build there. Where
`references/` has EA's name for a function, global, type, field, enum, parameter or local, use it
as EA spelled it (style.md, "EA's naming style"). Locals with no reference name get Hungarian
names (not `d`, `s`, `p`, `t0`).

Stay out of `{OTHERS}` unless your pass truly needs them. If a call site you must name is there,
keep the edit to that one call and say so.

SETUP (from the worktree root; if `git fetch` is refused, use the local `origin/main`):
```
git fetch -q origin && git checkout -B audit/{BRANCH} origin/main
ln -s {MAIN}/orig/GW4E69-rev0/sys orig/GW4E69-rev0/sys
mkdir -p build && for d in compilers binutils tools; do ln -s {MAIN}/build/$d build/$d; done
python configure.py && ninja    # must end with build/GW4E69-rev0/main.dol: OK before you change anything
```

Finished means all of these hold:
- `ninja` ends with `build/GW4E69-rev0/main.dol: OK`.
- `python tools/quality/lint.py --diff origin/main --compile` reports 0 findings, and
  `python tools/quality/lint.py` on the file has none.
- The `audit.tsv` and `audit-plan.tsv` rows stay `open`; the writer never marks them done.
  `python tools/quality/audit.py --check` reports 0 problems.
- The pass is one commit on `audit/{BRANCH}`. Its message stands alone as the record of the
  pass: what changed by category, the proof of both ends of the file (or, where unproven, what
  was checked and why it stays), and the decisions you made. The last line before any trailers is
  `build/GW4E69-rev0/main.dol: OK`.
- Out-of-scope changes go in a separate commit before the pass commit, with their own reason and
  the same last line.
- If you rename, split or merge the file, update every place that records its name:
  `configure.py`, `splits.txt`, `audit.tsv` (via `audit.py --sync`), `docs/audit-plan.tsv`,
  `src/README.md`, other files' comments. Leave frozen evidence logs alone.
- Every static function has `scope:local` in `config/GW4E69-rev0/symbols.txt`.
- Comments say what the code does. Check every claim against the code and its callers before
  committing. Any claim about code outside the function (every, all, never, only, callers, other
  files) is traced repo-wide first; the reviewer will trace each one. A branch that can never run
  is described as such, or not at all.
- The pass owns every call to its functions: a bare value passed to them or compared with their
  results gets a name at every caller.

The usual reasons passes fail review:
- a comment claiming something the code doesn't do (including about callers, or failure paths
  that can't happen);
- a field name claiming more than the code shows (a write-only field is `nUnread`, not
  "reserved");
- our name used where EA's is known;
- a bare number missed at one of its uses (search the repo for every caller);
- a field left with an offset name, or reached through a raw offset in another file (a pass that
  names a field owns every use, sweep files included);
- a stale `port:` note;
- a foreign prototype left in a sweep file (lint skips `src/unsorted`, so grep it);
- an array indexed past its size without an `ea-bugs.md` entry;
- a `fake match:` tag on plain C;
- a file boundary argued from a guess (for example "inline copies come last", which is false for
  this compiler).

Rules:
- Scratch files go only in `{SCRATCH}`.
- Edit C and headers only with an editor or a saved script, never `sed`, heredocs or `echo`.
- Work only in your worktree; read `{MAIN}` only through the links. Never push, open a pull
  request or download anything.
- Don't use `git stash`. Don't delete anything you haven't checked.
- Don't ask questions; make the call the rulebook supports and note it.
- Once committed, don't rebase; the lander merges `main`. If you are told `main` moved under
  you, merge it in and use the names it now has.

When done, report in under 40 lines: branch and commit hashes, what changed by category, how
each end is proven, anything unfinished, and where `style.md` was unclear and what you decided.
