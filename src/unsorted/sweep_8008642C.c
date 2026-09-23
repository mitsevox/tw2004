// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

f32 AI_MaxDistance();
f32 GM_GetGolferDistanceToPin();

void fn_8008642C(u8* p0, u8* p1);
void fn_80086460(u8* p0, u8* p1);
void fn_8008642C(u8* p0, u8* p1) {
    f32 t0;
    t0 = GM_GetGolferDistanceToPin(*(s32*)p0);
    *(f32*)p1 = t0;
}

void fn_80086460(u8* p0, u8* p1) {
    f32 t0;
    t0 = AI_MaxDistance(*(s32*)p0, *(s32*)(p0 + 0x4), *(s32*)(p0 + 0x8));
    *(f32*)p1 = t0;
}
