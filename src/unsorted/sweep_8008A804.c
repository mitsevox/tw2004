// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8003DCAC();
s32 GameModeBattle_NumRemovableClubsLeft();

void fn_8008A804(u8* p0, u8* p1);
void fn_8008A838(s32 p0, u8* p1);
void fn_8008A804(u8* p0, u8* p1) {
    s32 t0;
    t0 = GameModeBattle_NumRemovableClubsLeft(*(s32*)p0);
    *(s32*)p1 = t0;
}

void fn_8008A838(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_8003DCAC();
    *(s32*)p1 = (t0 & 0xFF);
}
