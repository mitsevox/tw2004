# Infrastructure: where things run and how they connect

The machines, the git flow, CI, the public page and decomp.dev. Matching tools are in
[`workflow.md`](workflow.md).

## Where the work happens

| Where | What runs there | Game files |
|---|---|---|
| **Cloud session** (Claude Code on the web) | edits, builds, pushes branches, opens PRs | fetched by `tools/cloud/setup.sh` from the private build container (owner's `TW_BUILD_TOKEN`, read-only) |
| **Owner's PC** | local builds | `orig/` on the PC, never leaves it |
| **GitHub Actions** | the build on every push and PR, the public page | the private container `ghcr.io/mitsevox/tw2004-build:main` |

Setting up a cloud session: [`../tools/cloud/README.md`](../tools/cloud/README.md). Everything
platform-specific in the tools is in `tools/build/hosttools.py`, so they run unchanged on Windows
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

If CI reports "main.dol not found", the container image `:main` is empty: re-run the latest
container build in `mitsevox/tw2004-build`. Pushing workflow files from the GitHub CLI needs the
`workflow` scope (`gh auth refresh -h github.com -s workflow`).

## The public progress page (shared site)

https://mitsevox.github.io/games/tw2004/, on the owner's shared portfolio site
(`mitsevox/mitsevox.github.io`, report-v1). On every push to main:

1. the `build` job's `tools/dashboard/pages.py append` adds the run's numbers (sha, date, exact
   functions, matched and linked code and data, from report.json) to `history.json` on the
   data-only branch `pages-history`;
2. the `site-report` job runs `tools/dashboard/site_report.py` on report.json, `docs/audit.tsv`
   and that history, packages the result with the site's own `tools/package_report.py` (from a
   pinned site revision) and uploads the `site-report` artifact;
3. the site's own CI imports that artifact on its next refresh.

Only report.json numbers, file (unit) and function names and commit subjects are published: no
game data, nothing from `/orig`. The site trusts only runs whose `build.yml` git blob is listed in
its `config/tw2004-producer.json`: a change to `build.yml` needs that list updated, or the site
keeps the last report it accepted. The local dashboard (`python tools/dashboard/server.py`) shows
the same numbers.

The history: linked bytes are exact for every commit (summed from `configure.py` and `splits.txt`
at that commit, no build needed); matched bytes need a build report of that commit, otherwise the
history repeats the previous value. To rebuild past commits for real matched numbers:
`python tools/dashboard/backfill_history.py <oldest> --pages-history out.json` (main's first-parent
commits, about 30 s each), then merge `out.json` into `history.json` on `pages-history`. History
from before the repository's rewrite exists only in a local `build/dashboard_history.json`:
`pages.py merge <history.json> <that file>` adds it.

## decomp.dev

The project is listed at https://decomp.dev/mitsevox/tw2004 (README badges). decomp.dev reads the
`GW4E69_report` artifact of the latest CI run on main. Without its GitHub App
(https://github.com/apps/decomp-dev) installed on the repo it checks every 30 minutes; the "Force
refresh" button on https://decomp.dev/manage/mitsevox/tw2004 fetches right away. If it lags, first
check that the latest run's `GW4E69_report` has the new numbers.
