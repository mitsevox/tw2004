// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gPlayers[];
s32 fn_8008AB40();
s32 fn_800E27C0();

void fn_800865E0(s32 p0, u8* p1);
void fn_80086610(s32 p0, u8* p1);
void fn_800865E0(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_8008AB40();
    *(s32*)p1 = t0;
}

void fn_80086610(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_800E27C0();
    *(s32*)p1 = t0;
}
