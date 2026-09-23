// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void Vec3Copy();

void fn_80064F54(u8* p0, s32 p1, s32 p2);
void fn_80064F54(u8* p0, s32 p1, s32 p2) {
    Vec3Copy((p0 + 0x20), p2);
}
