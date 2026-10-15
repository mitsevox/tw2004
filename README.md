Tiger Woods PGA Tour 2004
=========================

[![Code progress](https://decomp.dev/mitsevox/tw2004.svg?mode=shield&measure=code&label=Code)](https://decomp.dev/mitsevox/tw2004)
[![Functions](https://decomp.dev/mitsevox/tw2004.svg?mode=shield&measure=functions&label=Functions)](https://decomp.dev/mitsevox/tw2004)

A matching decompilation of Tiger Woods PGA Tour 2004 for the GameCube: C source that compiles
back to a byte-identical copy of the retail `main.dol`, written the way EA wrote it.

This repository contains no game assets, no game code binaries and no official SDK files. You need
your own copy of the game to build it.

Supported version: `GW4E69`, Rev 0 (USA). `main.dol` is identical on Disc 1 and Disc 2
(SHA-1 `bbbc55485e51973931dee169e7bf87bc7379223f`), so either disc works.

Status
======

The match is complete: all 7,647 functions are byte-exact and every unit links from C.

Work now goes into making the code read well: naming what is still unnamed, giving structs real
fields, and keeping comments to what explains the game. Progress:
[decomp.dev](https://decomp.dev/mitsevox/tw2004) and the [progress page](https://mitsevox.github.io/tw2004/).

Layout
======

- `src/`, `include/`: the game's C source and headers. [`src/README.md`](src/README.md) maps the
  files to the game's subsystems.
- `extern/`: the Nintendo SDK, C library and Metrowerks runtime, built from public GameCube
  decompilations (credits in each folder).
- `config/GW4E69/`: decomp-toolkit configuration (`symbols.txt` holds every name), and
  `name_sources.tsv`, a frozen log of how the earlier names were found.
- `reference/`: symbol dumps from related EA builds (TW07, TW06, Madden 2003 and others) that
  EA's names are taken from.
- `docs/`: the [docs index](docs/README.md).
- `tools/`: build helpers and the naming and lint tools.
- `mods/gecko/`: Gecko codes built from the decomp (untested).

Building
========

Needs Python 3 and [ninja](https://github.com/ninja-build/ninja). On Linux and macOS,
[wibo](https://github.com/decompals/wibo) is downloaded automatically to run the compiler.

1. Put your disc image (ISO/GCM, RVZ, WIA, WBFS, CISO, NFS, GCZ or TGC) in `orig/GW4E69/`, or the
   extracted `main.dol` at `orig/GW4E69/sys/main.dol`.
2. Run:

   ```sh
   python configure.py
   ninja
   ```

The build ends with `build/GW4E69/main.dol: OK` when the output matches.

Credits
=======

Built on [decomp-toolkit](https://github.com/encounter/decomp-toolkit) and
[objdiff](https://github.com/encounter/objdiff). The SDK and runtime code in `extern/` comes from
other GameCube decompilations (Metroid Prime, Final Fantasy Crystal Chronicles, The Wind Waker,
Twilight Princess, Pikmin 2, Sonic Heroes, Gauntlet: Dark Legacy); see each folder's credits.
