# Infrastructure: where things run and how they connect

The machines, the git flow, CI, the public page and decomp.dev. Matching tools are in
[`workflow.md`](workflow.md); running agent lanes is in [`../agents/README.md`](../agents/README.md).

## Where the work happens

| Where | What runs there | Game files |
|---|---|---|
| **Cloud session** (Claude Code on the web; the orchestrator) | reviews and merges every branch into main, runs lanes, fixes tools, writes the docs; sends heavy jobs to the PC | fetched by `tools/cloud/setup.sh` from the private build container (owner's `TW_BUILD_TOKEN`, read-only) |
| **Owner's PC** (LUCASPC, Windows, 20 cores) | a GitHub self-hosted runner for heavy jobs (`pc-job.yml`); online only while the owner runs it | `C:/dev/tw2004/orig`, never leaves the PC |
| **Owner's Mac** | outside agents (Gemini, ChatGPT/Codex) in their own worktrees, each on its own branch | local copy |
| **GitHub Actions** | the build on every push, the public page, branch cleanup | the private container `ghcr.io/mitsevox/tw2004-build:main` |

Setting up a cloud session: [`../tools/cloud/README.md`](../tools/cloud/README.md). Everything
platform-specific in the tools is in `tools/match/hosttools.py`, so they run unchanged on Windows
and Linux.

## Git flow

- **main is the only long-lived branch**, and every commit on it ends with `main.dol: OK`. The
  orchestrator commits to main directly (the owner's rule: when the checks pass).
- **Work from other hands arrives on branches** (`agent/<lane>` from cloud lanes, `gemini/...`,
  `codex/...` from the Mac, `pc-results/...` from the PC) and goes into main only through
  `python tools/agents/merge.py <name>` (branch `agent/<name>`; for another branch first
  `git branch -f agent/<name> origin/<branch>`). The gates: DOL OK, no exact function lost, lint,
  type/symbol audits, constants, the asm gate, the audit baseline (see `agents/README.md`).
- **Branches are cleaned up by a workflow**, since the session cannot delete branches itself:

  ```
  gh workflow run branch-cleanup.yml                                   # dry run: the job summary lists them
  gh workflow run branch-cleanup.yml -f dry_run=false [-f pattern='agent/*'] [-f pc_results_days=7]
  ```

  It deletes only branches whose tip is already in main (nothing is lost); main, pages-history
  and unmerged work are never touched; `pc-results/*` go only with `pc_results_days`, after
  their results were read.
- **Never in git:** `main.dol`, disc images, ELF/PDB/SELF, archives, art or other game data
  (`orig/*/*` is ignored), official SDK files (CLAUDE.md). Commit messages: no Co-Authored-By, no AI
  footer.

## CI: the build (`.github/workflows/build.yml`)

Every push and pull request builds in the private container, which holds only
`orig/GW4E69/sys/main.dol` (repo `mitsevox/tw2004-build`, **private**; tw2004 has Read access in
the package's "Manage Actions access"). The build downloads the compilers pinned in `configure.py`
(the container's are older) and uploads `GW4E69_report` (objdiff's report.json) and `GW4E69_maps`.
Builds of main queue one after another; newer pushes to any other branch cancel older runs.

Pitfalls met: when the build repo is created from its template, the template's first container
build can finish after the commit that adds `main.dol` and overwrite `:main` with an empty image
("main.dol not found": re-run the latest container build). Pushing workflow files from the owner's
GitHub CLI needs the `workflow` scope (`gh auth refresh -h github.com -s workflow`).

## The public progress page (GitHub Pages)

https://mitsevox.github.io/tw2004/ (Settings > Pages > Source: GitHub Actions). On every push to
main, `build.yml` after the report step:

1. `tools/dashboard/pages.py append` adds the run's numbers (sha, date, exact functions, matched
   and linked code and data, from report.json) to `history.json` on the data-only branch
   `pages-history`;
2. `tools/dashboard/server.py --export site` writes `index.html` (reads `progress.json`),
   `progress.json` and `history.json`;
3. the `pages` job deploys `site/` with `actions/deploy-pages`.

Only report.json numbers, file (unit) names and commit subjects are published: no game data,
nothing from `/orig`. To change the page, edit `server.py` (`PAGE`; the same page serves locally
with `python tools/dashboard/server.py`) and push to main.

The history: linked bytes are exact for every commit (summed from `configure.py` and `splits.txt`
at that commit, no build needed); matched bytes need a build report of that commit, otherwise the
page repeats the previous value (and says so). To rebuild past commits for real matched numbers:
`python tools/dashboard/backfill_history.py <oldest> --pages-history out.json` (main's first-parent
commits, about 30 s each), then merge `out.json` into `history.json` on `pages-history`. History
from before the repository's rewrite on 2026-09-24 exists only in the PC's
`build/dashboard_history.json`: `pages.py merge <history.json> <that file>` adds it.

## decomp.dev

The project is listed at https://decomp.dev/mitsevox/tw2004 (README badges). decomp.dev reads the
`GW4E69_report` artifact of the latest CI run on main. Without its GitHub App
(https://github.com/apps/decomp-dev) installed on the repo it checks every 30 minutes; the "Force
refresh" button on https://decomp.dev/manage/mitsevox/tw2004 fetches right away. If it lags, first
check that the latest run's `GW4E69_report` has the new numbers.

## Heavy jobs on the owner's PC (`.github/workflows/pc-job.yml`)

The PC is a self-hosted runner (LUCASPC, labels `self-hosted, Windows, X64, tw-pc`). Manual
dispatch only (never on push); one job at a time (concurrency group `pc-runner`), queued while the
PC is off; fork pull requests need approval to run workflows.

```
gh workflow run pc-job.yml -f tool=permute -f unit=hwsBurn -f fn=fn_8011172C -f minutes=360 -f jobs=18
gh workflow run pc-job.yml -f tool=leversweep -f unit=hwsBurn -f fn=fn_8011172C -f minutes=6
gh workflow run pc-job.yml -f tool=leversweep                  # --from-report: every near-miss
gh workflow run pc-job.yml ... -f ref=my-branch                # build from a pushed branch
gh workflow run pc-job.yml -f tool=declsearch -f targets=Unit:fn,Unit:fn -f minutes=240
gh workflow run pc-job.yml -f tool=libflags -f targets=UISEvent,UIStudio,UISApi,UISScreen -f minutes=240
```

`declsearch` (tools/match/declsearch.py) searches declaration orders over many functions at once,
each job with an equal time slice; results go to `pc-results/declsearch-<run id>` (per function
`score.txt` and the declaration diff). It beat the permuter on register-only near misses (first
run: ska_shared fn_8001FCF4 exact, unit linked). **Speed, measured 2026-09-26:** about 46 trials a
minute per worker on the PC (11,000 per worker in 4 hours on 18 workers), against about 550 a
minute per worker in the cloud: every trial writes a fresh temp `.c`/`.o` and runs the compiler and
objdump, and Windows (Defender scanning each new file is the likely cost) makes that slow.
Excluding the runner's work folder and `%TEMP%` from Defender's real-time scan should help; to be
measured.

`libflags` (tools/match/libflags.py) searches ONE compiler flag set for a group of units (a library:
real teams set flags per library), scoring all of them together with objdiff; results
`pc-results/libflags-<run id>` (best.txt, results.tsv). First use: the UIS units, EA Tiburon's IStudio
library (TW2005's paths put it in Code/Tiburon/IStudio/, not the Tiger team's Code/Golf/).

It checks out `ref` (default main; the workspace keeps `build/` between jobs), copies
`C:/dev/tw2004/orig`, builds and stops unless `main.dol: OK`, then runs the tool with `-j jobs`
(default 18) for `minutes` (default 360; the job times out an hour later). Results land on a new
branch `pc-results/<fn>-<run id>`: the permuter's best 5 outputs (`diff.txt`, `score.txt`; never
`source.c`) and its last progress line, or the sweeper's files from that run, plus the log and
`run.txt` (the inputs). Read them with `git fetch origin 'pc-results/*'`; apply a hit by hand.
Do not dispatch with many jobs while the owner uses the PC's cores (`-f jobs=2`).

Windows details the workflow handles: the runner service's PATH has Git's `cmd\` but not `bin\`
(a first `cmd` step adds it; PowerShell scripts are blocked by the PC's execution policy), and
dtk colours its `OK` (stripped before the check).

Cloud session access to GitHub: push and pull, run and read workflows (the GitHub tools), but not
delete branches or change settings; that is why branch cleanup is a workflow.
