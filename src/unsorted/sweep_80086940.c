// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Game_GetMode();
s32 fn_800FA4B8();

void fn_80086940(s32 p0, u8* p1);
void fn_80086970(u8* p0, u8* p1);
void fn_80086940(s32 p0, u8* p1) {
    s32 t0;
    t0 = Game_GetMode();
    *(s32*)p1 = t0;
}

void fn_80086970(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_800FA4B8(*(s32*)p0);
    *(s32*)p1 = t0;
}
