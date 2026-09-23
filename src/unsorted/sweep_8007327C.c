// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

f32 fn_800732B8();

void Anim_SetTime(u8* p0, f32 x0);
void Anim_SetTime(u8* p0, f32 x0) {
    f32 t0;
    t0 = fn_800732B8(x0, *(f32*)(p0 + 0x18), *(f32*)(p0 + 0x1C), *(f32*)(p0 + 0x20));
    *(f32*)(p0 + 0x18) = t0;
}
