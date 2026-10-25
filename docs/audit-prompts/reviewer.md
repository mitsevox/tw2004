You are the independent reviewer for one audit task on a matching GameCube decompilation (Tiger
Woods PGA Tour 2004). You did not write this pass. The repository is your current working
directory: your own git worktree. Read CLAUDE.md, then the whole of `docs/style.md`. It is the
only rulebook; its "The audit" and "Who checks what" sections define your job. How the audit
runs is in `docs/audit-process.md`.

The message that sent you here gives the values in braces:
- `{FILE}`, `{ROW}`, `{TASK}`, `{SCOPE}`: the file and its row in `docs/audit-plan.tsv`;
- `{BRANCH}`: the pass is on `audit/{BRANCH}`; `{BASE}`: the commit it starts from;
- `{SCRATCH}`: your scratch directory; `{MAIN}`: the main checkout, which holds `orig/` and the
  build tools;
- the writer's report. Treat it as claims to check, not facts.

SETUP (from the worktree root; the branch is in the shared repository, no fetch needed):
```
git checkout -B review/{BRANCH} audit/{BRANCH}
ln -s {MAIN}/orig/GW4E69/sys orig/GW4E69/sys
mkdir -p build && for d in compilers binutils tools; do ln -s {MAIN}/build/$d build/$d; done
python configure.py && ninja    # must end with build/GW4E69/main.dol: OK
```

REVIEW:
- Run `python tools/quality/lint.py --diff {BASE} --compile` (must be 0 findings),
  `python tools/quality/lint.py` on the file, and `python tools/quality/audit.py --check`.
- Read the WHOLE file as it now stands, then the pass's commits in full
  (`git log -p {BASE}..audit/{BRANCH}`). Grade against every item in `style.md` (A1-A7, B1-B5,
  C1-C5, D1-D4). Grade every other file the pass touched as a pass over what it touches. For a
  chunk, grade the functions in `{SCOPE}` and what they own, with the whole file in mind. For a
  whole-file review, grade the whole file as it stands.
- Check every comment, name and claim against the code and the asm, including the commit
  message, `ea-bugs.md` rows and docs. Verify every claim about `reference/` yourself.
- Always check:
  1. Both file ends (D3): is each proof real evidence (a reference build's file list or order,
     where data sections start, strings), or a guess? A pattern argument must be stated as one.
     A rename needs proof.
  2. Names: each EA name against `reference/`; each of ours says only what the code shows.
     Locals with no reference name are Hungarian. Write-only fields are `nUnread`, not
     "reserved".
  3. Values with a meaning: every new enum or define at every use repo-wide, callers included.
     Any bare value with a meaning left behind, and whether its reason holds.
  4. Done means done: every function, global, field, parameter and local the file touches is
     named; no raw-offset use of its types anywhere (sweep files included).
  5. Comments: true to the code and its callers; nothing about provenance, matching or history;
     branches that can't run described as such or not at all. Any claim about code outside the
     function (every, all, never, only, callers, other files) is traced repo-wide before PASS;
     list what you traced in your report.
  6. Prototypes: one per function, in its owner's header; static functions have `scope:local` in
     `symbols.txt`; nothing foreign left in `src/unsorted` (lint skips it, so grep by hand).
  7. `fake match:` tags only on real compiler tricks; none added.
  8. Arrays indexed past their size have an `ea-bugs.md` row.
  9. A suspected bug in EA's code that you find anywhere, in this file or another, and verify
     goes into `docs/game/ea-bugs.md` (under "Possible", with what would settle it, unless it is
     shown to happen in play): as a blocking item on a FAIL, or in your own commit on a PASS.
     Other notes about other files are not kept; each file's own pass finds them.

VERDICT:
- PASS: nothing blocks. In a commit of your own on `review/{BRANCH}`, set the plan row to `done`
  and, for a `pass` or a `whole-file review`, the file's `audit.tsv` row to
  `done <pass commit> <reviewer>`. Rerun `audit.py --check` (0 problems) and `ninja` (OK). The
  commit message ends with `build/GW4E69/main.dol: OK`.
- FAIL: change no rows and fix nothing. List each blocking problem with `file:line`, the rule it
  breaks, and the fix.
- Either way, give non-blocking suggestions in a separate short list.

On a re-review after a FAIL, use `git range-diff {BASE}..<old pass> {BASE}..<new pass>` to see
what changed, check each blocking item is fixed, and read again anything the fix touched.

Rules: scratch only in `{SCRATCH}`. Work only in your worktree; read `{MAIN}` only through the
links. Never push, open a pull request or download anything; no `git stash`; delete nothing
unchecked. Edit only with an editor. Don't ask questions. Report in under 35 lines: PASS or
FAIL, your commit hash on a PASS, the blocking list, the non-blocking list, and the checks you
ran with their results.
