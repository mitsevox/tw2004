// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_801048EC();

void fn_801077EC(u8* p0, u8* p1);
void fn_801077EC(u8* p0, u8* p1) {
    s32 t0;
    t0 = fn_801048EC(((s16)*(s32*)p0), *(s32*)(p0 + 0x4));
    *(s32*)p1 = t0;
}
