# Infrastructure: where things run and how they connect

The machines, the git flow, CI, the public page and decomp.dev. Matching tools are in
[`workflow.md`](workflow.md).

## Where the work happens

| Where | What runs there | Game files |
|---|---|---|
| **Cloud session** (Claude Code on the web) | edits, builds, pushes branches, opens PRs | fetched by `tools/cloud/setup.sh` from the private build container (owner's `TW_BUILD_TOKEN`, read-only) |
| **Owner's PC** | local builds | `C:/dev/tw2004/orig`, never leaves the PC |
| **GitHub Actions** | the build on every push and PR, the public page | the private container `ghcr.io/mitsevox/tw2004-build:main` |

Setting up a cloud session: [`../tools/cloud/README.md`](../tools/cloud/README.md). Everything
platform-specific in the tools is in `tools/match/hosttools.py`, so they run unchanged on Windows
and Linux.

## Git flow

- **main is the only long-lived branch**, and every commit on it ends with `main.dol: OK`.
- **Work arrives on branches** and goes into main once it builds with `main.dol: OK` and CI is
  green. Merged branches are deleted by GitHub ("Automatically delete head branches").
- **Never in git:** `main.dol`, disc images, ELF/PDB/SELF, archives, art or other game data
  (`orig/*/*` is ignored), official SDK files (CLAUDE.md).

## CI: the build (`.github/workflows/build.yml`)

Every push and pull request builds in the private container, which holds only
`orig/GW4E69/sys/main.dol` (repo `mitsevox/tw2004-build`, **private**; tw2004 has Read access in
the package's "Manage Actions access"). The build downloads the compilers pinned in `configure.py`
(the container's are older), removes stale objects from the cached build folder
(`tools/build/prune_stale.py`: wibo's case-blind lookup otherwise writes a renamed unit into its
old-case object), and uploads `GW4E69_report` (objdiff's report.json) and `GW4E69_maps`.
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
