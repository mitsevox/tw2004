# MetroTRK source (reference build)

Metrowerks Target Resident Kernel for GameCube, as decompiled by the Pikmin 2 project
(https://github.com/projectPiki/pikmin2, `src/Dolphin/TRK_MINNOW_DOLPHIN`, released under CC0).
This game ships "MetroTRK for GAMECUBE v2.0"; Pikmin 2's is v2.6, so a few functions differ.
Compiled here with CodeWarrior GC/1.3.2 and the flags in `cflags_trk` (configure.py), which
reproduce this game's bytes for most of the ~150 TRK functions. Units whose functions all match are
linked (`Matching` in configure.py); the rest only provide the comparison and the names.

Changed for this game:
- `MetroTRK/nubinit.c`: the welcome string says v2.0, as this game's TRK does.
- `MetroTRK/mslsupp.c`: no `__close_console`. This game's comes from the MSL UART console code
  (0x8015C120, returns 0), which `__files` points at; a second definition here would clash with it.
- `include/Dolphin/db.h`: `__DBInterface` and `DBVerbose` are declared `extern` (they are the SDK
  db.c's globals); Pikmin 2's header defines them `static`, which gave gdev/main.c its own copies.

`include/` holds just the headers these files need, copied from the same project.

`MetroTRK/__exception.s` (the exception vector table) is from the Super Smash Bros. Melee
decompilation instead (https://github.com/doldecomp/melee, commit 430c286): Pikmin 2's newer table is
84 bytes longer, Melee's assembles to this game's exact bytes.
