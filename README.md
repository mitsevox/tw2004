Tiger Woods PGA Tour 2004
=========================

[![Code progress](https://decomp.dev/mitsevox/tw2004.svg?mode=shield&measure=code&label=Code)](https://decomp.dev/mitsevox/tw2004)
[![Functions](https://decomp.dev/mitsevox/tw2004.svg?mode=shield&measure=functions&label=Functions)](https://decomp.dev/mitsevox/tw2004)

A matching decompilation of Tiger Woods PGA Tour 2004 for the GameCube: C source that compiles
back to a byte-identical copy of the retail `main.dol`. The aim is EA's code as EA wrote it. A PC
port and mods will start from that code later.

This repository contains no game assets, no game code binaries and no official SDK files. You need
your own copy of the game to build it.

Supported version: `GW4E69`, Rev 0 (USA). `main.dol` is identical on Disc 1 and Disc 2
(SHA-1 `bbbc55485e51973931dee169e7bf87bc7379223f`), so either disc works.

Status
======

**The match is complete.** All 7,647 functions are byte-exact and all 460 units link from C;
the build reproduces the retail `main.dol` exactly.

The project is now in its **readability** phase: making every file understandable to a person
reading it cold, with real names, correct comments and readable locals. As of 2026-09-29, of the 6,641
functions in EA's code:

- 5,548 are named (EA's own name where a related build confirms it, otherwise read from the code);
- 4,912 (74.0%) have been reviewed in the pass;
- 154 of 279 source files are fully through it.

Live numbers: the [progress page](https://mitsevox.github.io/tw2004/) and
[decomp.dev](https://decomp.dev/mitsevox/tw2004).

What's here
===========

- `src/`, `include/`: the game's C source and headers. Start with
  [`src/README.md`](src/README.md), the map of the code by subsystem (boot and main loop,
  rendering, cameras, ball physics, golfers, the round and game modes, front end, audio, saves).
- `extern/`: the Nintendo SDK, C library, Metrowerks runtime and debugger, built from public
  GameCube decompilations (credits in each folder's `CREDITS.md` / `README.md`).
- `config/GW4E69/`: decomp-toolkit configuration (symbols, splits) and
  `name_sources.tsv`, the evidence behind every name.
- `docs/`: see [`docs/README.md`](docs/README.md) for the index and how far to trust each file.
  Highlights: [`docs/style.md`](docs/style.md) (how the C must read),
  [`docs/compiler/decomp-notes.md`](docs/compiler/decomp-notes.md) (the CodeWarrior rulebook),
  [`docs/game/formats/`](docs/game/formats/README.md) (the game's data file formats),
  [`docs/game/gameplay.md`](docs/game/gameplay.md) (gameplay logic read from the code; not yet audited).
- `tools/`: build, matching and naming scripts; `mods/gecko/` has Gecko codes (an
  always-pool-cue gimme and a no-lucky-shots code, untested). A confirmed widescreen culling fix is
  in [`docs/game/tw2004-notes.md`](docs/game/tw2004-notes.md), "Frustum setup and widescreen codes".
- `docs/evidence/notes/`: kept findings: EA bugs found, misfiled units, TW07/TW06 name pairing.

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

The build must end with `build/GW4E69/main.dol: OK`. Every command beyond that is in
[`docs/workflow.md`](docs/workflow.md).

Credits
=======

Built on [decomp-toolkit](https://github.com/encounter/decomp-toolkit) and
[objdiff](https://github.com/encounter/objdiff). The SDK and runtime code in `extern/` comes from
other GameCube decompilations (Metroid Prime, Final Fantasy Crystal Chronicles, The Wind Waker,
Twilight Princess, Pikmin 2, Sonic Heroes, Gauntlet: Dark Legacy); see each folder's credits.
