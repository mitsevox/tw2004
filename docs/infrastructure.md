# Infrastructure: where things run and how they connect

The machines, the git flow, CI and the public page. Matching tools are in
[`workflow.md`](workflow.md).

## Where the work happens

| Where | What runs there | Game files |
|---|---|---|
| **Cloud session** (Claude Code on the web) | edits, builds, pushes branches, opens PRs | fetched by `tools/cloud/setup.sh` from the private build container (owner's `TW_BUILD_TOKEN`, read-only) |
| **Owner's PC** | local builds | `orig/` on the PC, never leaves it |
| **GitHub Actions** | the build on every push and PR, publishing on main | the private image `ghcr.io/mitsevox/tw2004-build`, pinned by digest in `validate.yml` |

Setting up a cloud session: [`../tools/cloud/README.md`](../tools/cloud/README.md). Everything
platform-specific in the tools is in `tools/build/hosttools.py`, so they run unchanged on Windows
and Linux.

## Git flow

- **main is the only long-lived branch**, and every commit on it ends with `main.dol: OK`.
- **Work arrives on branches** and goes into main once it builds with `main.dol: OK` and CI is
  green. Merged branches are deleted by GitHub ("Automatically delete head branches").
- **Never in git:** `main.dol`, disc images, ELF/PDB/SELF, archives, art or other game data
  (`orig/*/*` is ignored), official SDK files (CLAUDE.md).

## CI: the build (`.github/workflows/validate.yml`)

Every push and pull request builds in the private image (repo `mitsevox/tw2004-build`, **private**;
tw2004 has Read access in the package's "Manage Actions access"). The image holds the original at
`/orig/GW4E69-rev0/sys/main.dol` and the toolchain at the versions `configure.py` pins (binutils,
compilers, decomp-toolkit); `validate.yml` names it by digest in `BUILD_IMAGE`, never by `:main`.
After the image changes, copy the digest from the image run's summary into `validate.yml`.

The jobs: `Plan affected targets` picks the targets a change touches (`config/ci-targets.json`);
`Tool tests` runs the kit's checks and the scaffold check; `Validate GW4E69-rev0` builds with
`main.dol: OK`, measures the target and runs the project check (`tools/quality/audit.py --check`,
the audit table against `src/` and the lint, with the image's compilers through `TW_COMPILERS`);
`Validate targets` is the one required check. Builds of main queue one after another; newer pushes
to any other branch cancel older runs. Pushing workflow files from the GitHub CLI needs the
`workflow` scope (`gh auth refresh -h github.com -s workflow`).

## The public progress page

https://mitsevox.github.io/games/tw2004/. On every push to main the target job's last step, the
site's publish action, converts the build's report.json and pushes the numbers to the site's data
repository; the page updates within minutes. Only report.json numbers, unit and function names and
the commit are published: no game data, nothing from `/orig`. `config/site.json` holds the game's
title and presentation. The local dashboard (`python tools/dashboard/server.py`) shows the same
numbers from a local build.
