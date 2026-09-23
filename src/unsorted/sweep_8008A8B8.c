// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 GameModeBattle_GetWinner();

void fn_8008A8B8(s32 p0, u8* p1);
void fn_8008A8B8(s32 p0, u8* p1) {
    s32 t0;
    t0 = GameModeBattle_GetWinner();
    *(s32*)p1 = t0;
}
