// TerrainGround.c (our name): the ground under a point on the course's terrain grid: the
// supporting triangle and its height, and the surface type there. Split out of AnimStream's
// extent: its own .sdata2 block (0x80284460-0x80284478) and its callers (GoTerrainCollision,
// Golfer, emotion, GameEffects) show it is terrain code, not animation streaming.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "endian.h"
