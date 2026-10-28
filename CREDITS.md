# Credits

Imported and reused source keeps its origin, pinned revision, license and local modifications in the credits file of its own folder. Copyright and license notices in imported files remain intact.

## Tooling

The build structure comes from [dtk-template](https://github.com/encounter/dtk-template) (`configure.py`, `tools/project.py`, `tools/ninja_syntax.py`, `tools/download_tool.py`, `tools/transform_dep.py`, `tools/decompctx.py`), with this project's configuration in `configure.py`. The build uses [decomp-toolkit](https://github.com/encounter/decomp-toolkit) and [objdiff](https://github.com/encounter/objdiff) at the versions `configure.py` pins.

## Library source

The SDK, C library and Metrowerks runtime code in `extern/` comes from other GameCube decompilations. Each folder names its source repository and revision:

| Folder | Credits |
| --- | --- |
| `extern/sdk/` | [extern/sdk/README.md](extern/sdk/README.md) (Metroid Prime) |
| `extern/trk/` | [extern/trk/README.md](extern/trk/README.md) (Pikmin 2) |
| `extern/ffcc/` | [extern/ffcc/CREDITS.md](extern/ffcc/CREDITS.md) (Final Fantasy Crystal Chronicles) |
| `extern/gauntlet/` | [extern/gauntlet/CREDITS.md](extern/gauntlet/CREDITS.md) (Gauntlet: Dark Legacy) |
| `extern/pikmin2/` | [extern/pikmin2/CREDITS.md](extern/pikmin2/CREDITS.md) (Pikmin 2) |
| `extern/sonicheroes/` | [extern/sonicheroes/CREDITS.md](extern/sonicheroes/CREDITS.md) (Sonic Heroes) |
| `extern/tp/` | [extern/tp/CREDITS.md](extern/tp/CREDITS.md) (Twilight Princess) |
| `extern/tww/` | [extern/tww/CREDITS.md](extern/tww/CREDITS.md) (The Wind Waker) |

## Game source

Everything under `src/` and `include/` is this project's reconstruction, released under `LICENSE` (CC0 1.0).
