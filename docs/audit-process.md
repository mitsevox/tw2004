How the audit runs
==================

[`style.md`](style.md) says what an audit pass checks and when a file is done. This page says
how the work is run: who does what, in which order, and how a pass gets to `main`. With this
page and the two prompts in [`audit-prompts/`](audit-prompts/), anyone (a person or an agent
session) can run the next task in [`audit-plan.tsv`](audit-plan.tsv).

Roles
-----

- **Writer:** does one task from the plan on a branch of its own and commits it. Leaves the
  `audit.tsv` and plan rows `open`.
- **Reviewer:** someone who did not write the pass. Grades it against `style.md`, then either
  passes it (and marks the rows done, in a commit of its own) or fails it with a list of what
  blocks. Fixes nothing itself.
- **Lander:** merges `main` into the reviewed branch, runs the checks, opens the pull request and
  merges it once CI is green.

The writer never grades its own work. A byte match proves the code; it says nothing about names
or comments, so every one of them is read again by someone else.

Running a task
--------------

1. **Pick** the next `open` row of `audit-plan.tsv`. Note its area, file, task and scope.
2. **Write.** The writer works in its own worktree on `audit/<name>`, from current `main`, with
   [`audit-prompts/writer.md`](audit-prompts/writer.md) as its brief. Its result is one pass
   commit (plus, before it, any out-of-scope commit), and a short report.
3. **Review.** The reviewer works in its own worktree on `review/<name>`, made from
   `audit/<name>`, with [`audit-prompts/reviewer.md`](audit-prompts/reviewer.md) as its brief,
   and is given the writer's report as claims to check.
4. **On FAIL:** the writer gets the reviewer's blocking list and amends the pass commit (or adds
   the out-of-scope commit the fix needs). The same reviewer then re-reviews with
   `git range-diff <base>..<old pass> <base>..<new pass>` plus a fresh read of anything the fix
   touched. Repeat until PASS. There is no round limit.
5. **On PASS:** the reviewer's commit sets the file's `audit.tsv` row to
   `done <pass commit> <reviewer>` and the plan row to `done`.
6. **Land** (below).

Most passes so far failed their first review, nearly always on a comment or name that claimed
more than the code shows, or a bare value missed at one of its uses. That is the review working,
not a reason to skip it.

Landing
-------

From the main checkout, on the branch that carries pull requests:

```
git fetch origin
git checkout -B <pr-branch> review/<name>
git merge origin/main          # if main moved; message ends with build/GW4E69-rev0/main.dol: OK
rm build/GW4E69-rev0/src/<OldName>.o   # for each unit the pass (or main) renamed or removed
python configure.py && ninja   # build/GW4E69-rev0/main.dol: OK
python tools/quality/audit.py --check
python tools/quality/lint.py --diff origin/main --compile
git push -f origin <pr-branch>
```

Then open the pull request (Before / After / How), and merge it with a merge commit once the
`Validate targets` check is green. Every commit on the branch, merges included, ends with
`build/GW4E69-rev0/main.dol: OK`.

Merge conflicts with `main` are resolved by keeping both sides' intent: the plan and `audit.tsv`
keep every row that is done on either side; renamed types and functions take main's new name at
the lines the pass brought in.

Folds and other code moves
--------------------------

A `fold` puts an `unsorted/` scrap into the file it belongs to. The commit message gives the proof
of the boundary (`style.md`, "Every pass checks the file's two ends"); when nothing proves it, the
scrap goes to the neighbour the evidence favours, the message says so, with the evidence for and
against, and `src/README.md` marks that boundary unverified. Nothing stays in `unsorted/`.

Code that a fold, move or split adds to a file that is **open** in `audit.tsv` moves as it is:
the file's own pass brings it to the bar. Code added to a file that is **done** must be brought
to the `style.md` bar in the same change and kept consistent with what that file's audit set up
(its names, types, enums, header ownership and comment style), and an independent reviewer grades
it against that bar before it lands.

Big files
---------

A file over about 1,000 lines is planned as `chunk` tasks and a `whole-file review`
(`style.md`, "The audit"):

- **Chunks** run one at a time, each a full write, review and land, in plan order. The writer and
  the reviewer read the whole file first and change or grade only the chunk's functions. The
  rows stay `open`; the reviewer of a chunk marks only its plan row done.
- **The whole-file review** is done by a reviewer who wrote none of the chunks. It reads the whole
  file as it stands on `main` (its header, both ends, names across functions, every comment).
  If anything blocks, a writer fixes it as a pass commit and the same reviewer re-reviews. Only
  this review marks the file done in `audit.tsv`.

Running several at once
-----------------------

- Run tasks in parallel only on **different files**. Chunks of one file always run in order.
- Land one at a time. Each lander merges the new `main` in first.
- Passes in the same area often rename shared types or fields. When one lands, the next writer in
  that area **replays** its pass onto the new `main` (merge, then use the names main now has)
  before its review, so the reviewer grades the code as it will land.
- Tell each writer which files the others are working on; it keeps out of them except for a
  single call site it must name, and says so.

Where things go
---------------

- Each writer and reviewer has its own scratch directory outside the repo. Nothing from it is
  committed.
- A suspected EA bug that a writer or reviewer verifies, in any file, goes into
  [`game/ea-bugs.md`](game/ea-bugs.md) in that pass. Other notes about other files are not kept:
  each file's own pass finds them.
- Branches `audit/*` and `review/*` stay local; only the pull-request branch is pushed.
