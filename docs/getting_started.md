Getting started
===============

Setting up
----------

1. Install the [dependencies](dtk/dependencies.md): Python 3 and ninja. On Linux and macOS,
   `configure.py` downloads [wibo](https://github.com/decompals/wibo) to run the Windows compiler.
2. Put your copy of the game (USA, `GW4E69`, Rev 0) in `orig/GW4E69/`: the disc image (ISO/GCM,
   RVZ, WIA, WBFS, CISO, NFS, GCZ or TGC), or the extracted `main.dol` at
   `orig/GW4E69/sys/main.dol`. Either disc works; `main.dol` is the same on both.
3. Build:

   ```sh
   python configure.py
   ninja
   ```

   The first run downloads the compilers and decomp-toolkit. The build ends with
   `build/GW4E69/main.dol: OK` when every byte matches the retail game.

Reading the code
----------------

Start with [`src/README.md`](../src/README.md), the map of the source: which files make up each
part of the game (the main loop, rendering, cameras, ball physics, golfers, game modes, the front
end, audio, saves) and where a frame, a shot and a menu start.

Then [`style.md`](style.md) for how the C is written, and [`workflow.md`](workflow.md) for the
commands used to work on it.

Configuration
-------------

`config/GW4E69/` holds decomp-toolkit's configuration: `symbols.txt` (every symbol's name, address
and size), `splits.txt` (which address ranges belong to which source file) and `config.yml`.
[`dtk/`](dtk/) has the toolkit's own reference for these files.
