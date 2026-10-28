# Tiger Woods PGA Tour 2004

A matching decompilation of **Tiger Woods PGA Tour 2004** (GameCube, EA Redwood Shores, 2003): C source that compiles back to a byte-identical copy of the retail `main.dol`, written the way EA wrote it.

This repository contains no game assets, no game code binaries and no official SDK files. You need your own copy of the game to build it.

The match is complete: all 7,647 functions are byte-exact and every unit links from C. Work now goes into making the code read well: naming what is still unnamed, giving structs real fields, and keeping comments to what explains the game.

## Targets

| Id | Region | Revision | Status | Original SHA-1 |
| --- | --- | --- | --- | --- |
| `GW4E69-rev0` | USA | 0 | active | `bbbc55485e51973931dee169e7bf87bc7379223f` |

`config/targets.json` is the registry; each target's inputs are in `config/<id>/`. `main.dol` is identical on Disc 1 and Disc 2, so either disc works.

## Building

Needs Python 3 and [ninja](https://github.com/ninja-build/ninja). On Linux and macOS, [wibo](https://github.com/decompals/wibo) is downloaded automatically to run the compiler.

1. Put your disc image (ISO/GCM, RVZ, WIA, WBFS, CISO, NFS, GCZ or TGC) in `orig/GW4E69-rev0/`, or the extracted `main.dol` at `orig/GW4E69-rev0/sys/main.dol`.
2. `python configure.py` then `ninja`. The build ends with `build/GW4E69-rev0/main.dol: OK` when the output matches.
3. `ninja build/GW4E69-rev0/report.json` measures the target.

To check every active target: `for id in $(python3 -c 'from tools.targets import build_targets; print(*build_targets())'); do python3 configure.py --version "$id" && ninja; done`.

Pinned tool versions are in `configure.py`; the build structure is [dtk-template](https://github.com/encounter/dtk-template). A cloud session sets itself up with `tools/cloud/setup.sh` ([tools/cloud/README.md](tools/cloud/README.md)).

## CI and the progress site

`.github/workflows/validate.yml` builds every affected target in the private build image from [mitsevox/tw2004-build](https://github.com/mitsevox/tw2004-build), runs the audit check, and publishes each measurement to the [progress site](https://mitsevox.github.io/games/tw2004/) on `main`. Pull requests publish nothing. Details: [docs/infrastructure.md](docs/infrastructure.md).

## Contributing

The project owner is **[mitsevox](https://github.com/mitsevox)**, the sole merge authority. See [CONTRIBUTING.md](CONTRIBUTING.md) for the tiers, the task loop and the review checklist; agents start with [AGENTS.md](AGENTS.md), whose Project section holds this repository's own rules and points to the audit process.

## Layout

| Directory | Purpose |
| --- | --- |
| `src/`, `include/` | The game's C source and headers; [src/README.md](src/README.md) maps the files to the game's subsystems |
| `extern/` | The Nintendo SDK, C library and Metrowerks runtime, built from public GameCube decompilations |
| `config/<id>/` | One target's toolkit inputs (`symbols.txt` holds every name), `build.sha1` and `evidence.tsv` |
| `orig/<id>/` | Where the original goes locally; ignored by Git |
| `build/` | Generated; ignored |
| `tools/`, `tests/` | The kit tools, dtk-template's tools and the project's own: naming, lint, matching, and `tools/mods/` Gecko codes built from the decomp (untested) |
| `docs/`, `references/` | Durable project knowledge ([docs index](docs/README.md)) and symbol dumps from related EA builds (TW07, TW06, Madden 2003 and others) that EA's names are taken from |

## Credits and licenses

Imported source is credited in [CREDITS.md](CREDITS.md) and in each `extern/` folder; this project's own code is under [LICENSE](LICENSE) (CC0 1.0).
