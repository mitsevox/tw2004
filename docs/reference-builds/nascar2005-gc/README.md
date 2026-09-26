# NASCAR 2005: Chase for the Cup, GameCube (USA, GN4E69), EA Tiburon, 2004

Fetched on the owner's PC 2026-09-26 (PC job `refdwarf`, tools/ref/fetchdwarf.py; the archive and
ELF stay in C:/dev/refs). `NASCAR05.ELF` (18 MB) with DWARF 1.1 (CodeWarrior). `cus.txt`: its 230
compilation units (source path, code range).

Why we looked: the UIS units are EA Tiburon's IStudio library (TW2005's paths:
Code/Tiburon/IStudio/), and NASCAR 2005 is a Tiburon GameCube game with DWARF
(agents/findings/2026-09-26-tiburon-symbols.md).

What it has: **no IStudio / UIS**. Its UI is "Adapt" (F:/usr/local/Packages/Adapt/...), its engine
a Tiburon `Source/Common` + `Source/NGC` tree (anim, fileio, glib, Resource, MovieRcmp, particlelib,
Memory/TibNew.cpp ...). Same file names as ours: LLFont.c (a different font engine: FontLL*,
PrFont_t, not our FO_/UFontState code), skeleton.c, ChecksumCRC32.c, rcmp_mad_codec (C++ there),
the GBA SDK. So no direct help for the open functions; the unit list is kept as evidence of
Tiburon's library layout.
