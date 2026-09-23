// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gNumPlayersSetUp;

void fn_80086378(s32 p0, u8* p1);
void fn_80086378(s32 p0, u8* p1) {
    *(s32*)p1 = gNumPlayersSetUp;
}
