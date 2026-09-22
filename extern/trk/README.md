# MetroTRK source (reference build)

Metrowerks Target Resident Kernel for GameCube, as decompiled by the Pikmin 2 project
(https://github.com/projectPiki/pikmin2, `src/Dolphin/TRK_MINNOW_DOLPHIN`, released under CC0).
This game ships "MetroTRK for GAMECUBE v2.0"; Pikmin 2's is v2.6, so a few functions differ.
Compiled here with CodeWarrior GC/1.3.2 and the flags in `cflags_trk` (configure.py), which
reproduce this game's bytes for 97 of the ~150 TRK functions. Every unit is `NonMatching`: the DOL
is linked from the original code, the source only provides the comparison and the names.

`include/` holds just the headers these files need, copied from the same project.
