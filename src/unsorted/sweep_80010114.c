// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void GXSetTexCoordGen2();

void fn_80010114(s32 p0, s32 p1, s32 p2, s32 p3);
void fn_80010114(s32 p0, s32 p1, s32 p2, s32 p3) {
    GXSetTexCoordGen2(p0, p1, p2, p3, 0, 125);
}
