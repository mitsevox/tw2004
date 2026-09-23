Tiger Woods PGA Tour 2004
=========================

[![Code progress](https://decomp.dev/mitsevox/tw2004.svg?mode=shield&measure=code&label=Code)](https://decomp.dev/mitsevox/tw2004)
[![Functions](https://decomp.dev/mitsevox/tw2004.svg?mode=shield&measure=functions&label=Functions)](https://decomp.dev/mitsevox/tw2004)

A work-in-progress decompilation and asset-format research project for
Tiger Woods PGA Tour 2004 (GameCube).

This repository does **not** contain any game assets or assembly whatsoever.
An existing copy of the game is required.

Supported versions:

- `GW4E69`: Rev 0 (USA), Disc 1 and Disc 2

`main.dol` is byte-identical on both discs
(SHA-1 `bbbc55485e51973931dee169e7bf87bc7379223f`), so there is one code target.
The discs differ only in which course data they carry.

Status
======

Last updated 2026-09-23.

- Builds byte-identical to the retail `main.dol`.
- Matched code: **18.3%** (284,176 of 1,550,720 bytes), **2,374 of 7,647 functions** (31.0%).
  Counting partial matches: 21.9%.
- Game code in C so far: the swing and its state machine (`Swing.c`), golfer attributes and the
  CPU's shot planning (`Golfer.c`), ball physics (`Ball.c`, 63 of 68 functions exact), the
  animation library (`skalib.c`), the asset stream decompressor (`UStream.c`), and about 1,140
  small functions (accessors, setters and call wrappers) in `unsorted/sweep_*` files, pending their
  real file names.
- SDK and runtime: 764 of 841 configured functions matched.
- Named: about 1,300 functions. About 6,340 are still `fn_ADDRESS`.
- Compiler: CodeWarrior GC/2.5, `-O4,p`. See `docs/compiler.md`.
- Asset formats: the CTRL container (`.hog`/`.gcb`/`.ngc`, with an extractor), course textures
  (exported to PNG) and animation libraries decoded; the rest inventoried. See `docs/formats`.
- Gameplay: nine gameplay hypotheses tested against the code (`docs/hypotheses.md`), with the
  mechanics written up in plain English in `docs/gameplay.md` (swing error, CPU putting, the
  cup pull, lucky shots, lies, wind, bounces).
- Gecko codes (`tools/codes/`): a widescreen pop-in fix, an always-pool-cue gimme (confirmed in
  game), and a no-lucky-shots code (untested).

Roadmap
=======

The project runs on two tracks.

Track A: Code decompilation (`main.dol`)
----------------------------------------

1. **Core systems and engine loop** - Metrowerks runtime, Dolphin SDK, GX rendering
   pipeline, ARAM audio streamer, filesystem / DVD manager.
2. **Gameplay logic and physics simulation** - analog swing mechanics, trajectory
   physics, collision, putting / caddy solver, golfer stat math, UI state machines.

Work proceeds bottom-up. The ladder is a priority order, not a dependency chain:
anything not yet decompiled is linked from the original binary.

| Level | Layer | Contents |
|------:|-------|----------|
| 5 | Game systems and AI | Caddy tips, commentary triggers, scorecard state |
| 4 | Game mechanics and event handlers | Swing input scaling, ball / cup collision, lie penalties |
| 3 | Core physics simulation | Ball flight integration, green normal queries, wind / drag |
| 2 | Math and geometry primitives | Vector, matrix, spline interpolation |
| 1 | EA engine services | Asset loaders (`.hog` / `.gcb`), renderer, animation, audio, UI, saves |
| 0 | Runtime and Dolphin SDK | Memory allocators, CodeWarrior runtime, OS / GX |

Before climbing: **discovery** - compiler identification, SDK auto-matching, splits,
and a strings / constants survey. The binary ships without symbols, so the contents
of levels 2-5 are hypotheses until the code is located. Level 1 loaders are the
bridge to Track B: each loader decompiled is a file format documented.

Track B: Asset reverse engineering (disc data)
----------------------------------------------

3. **Shared global assets** - character models, animation skeletons, clothing
   textures, audio containers, sound banks, fonts.
4. **Course file formats (Disc 1 and Disc 2)** - terrain meshes, green elevation
   grids, spline paths, skyboxes.

Format notes live in [`docs/formats`](docs/formats/README.md). Compiler findings live in [`docs/compiler.md`](docs/compiler.md).

Knowledge base: [`docs/tw2004-notes.md`](docs/tw2004-notes.md) (what we know about this game) and
[`docs/decomp-notes.md`](docs/decomp-notes.md) (general CodeWarrior / GameCube pitfalls and good practice).
What happened when, and why: [`docs/journal.md`](docs/journal.md).
Gameplay predictions to check against the source as it emerges: [`docs/hypotheses.md`](docs/hypotheses.md).
What the shot code actually does (AI targeting and error, human swing forgiveness, the RNG):
[`docs/gameplay.md`](docs/gameplay.md).

Project structure
=================

- `configure.py` - Project configuration and generator script.
- `config/GW4E69` - decomp-toolkit configuration (`config.yml`, `symbols.txt`, `splits.txt`).
- `config/GW4E69/build.sha1` - SHA-1 hashes for each built artifact, for final verification.
- `orig/GW4E69` - Disc 1 image. Ignored by `.gitignore`.
- `orig/GW4E69_disc2` - Disc 2 image (Track B only). Ignored by `.gitignore`.
- `src/`, `include/` - Decompiled C/C++ source and headers (Track A).
- `docs/formats` - File format documentation (Track B).
- `docs/*.md` - decomp-toolkit reference docs inherited from the project template.
- `tools/` - Build scripts shared between decomp-toolkit projects.
- `tools/research/` - Our own analysis scripts (not part of the build).
- `build/`, `extracted/` - Generated output. Ignored by `.gitignore`.

Dependencies
============

Windows
--------

On Windows, it's **highly recommended** to use native tooling. WSL or msys2 are **not** required.

- Install [Python](https://www.python.org/downloads/) and add it to `%PATH%`.
- Download [ninja](https://github.com/ninja-build/ninja/releases) and add it to `%PATH%`.
  - Quick install via pip: `pip install ninja`

macOS
------

- Install [ninja](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages):

  ```sh
  brew install ninja
  ```

[wibo](https://github.com/decompals/wibo), a minimal 32-bit Windows binary wrapper, will be automatically downloaded and used.

Linux
------

- Install [ninja](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages).

[wibo](https://github.com/decompals/wibo), a minimal 32-bit Windows binary wrapper, will be automatically downloaded and used.

Building
========

- Clone the repository:

  ```sh
  git clone https://github.com/mitsevox/tw2004.git
  ```

- Copy your Disc 1 image to `orig/GW4E69`.
  - Supported formats: ISO (GCM), RVZ, WIA, WBFS, CISO, NFS, GCZ, TGC
  - Either disc works for building; Disc 2 is only needed for asset research.

- Configure:

  ```sh
  python configure.py
  ```

- Build:

  ```sh
  ninja
  ```

Diffing
=======

Once the initial build succeeds, an `objdiff.json` should exist in the project root.

Download the latest release from [encounter/objdiff](https://github.com/encounter/objdiff). Under project settings, set `Project directory`. The configuration should be loaded automatically.

![](assets/objdiff.png)

References
==========

- [decomp-toolkit](https://github.com/encounter/decomp-toolkit) / [dtk-template](https://github.com/encounter/dtk-template) (this project's scaffold)
- [Discord: GC/Wii Decompilation](https://discord.gg/hKx3FJJgrV)
- [objdiff](https://github.com/encounter/objdiff), [decomp.me](https://decomp.me), [decomp.dev](https://decomp.dev)
