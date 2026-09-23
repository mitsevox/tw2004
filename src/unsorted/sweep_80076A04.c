// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_80076A04(u8* p, f32 v);
void fn_80076A0C(u8* p, s32 v);
void fn_80076A04(u8* p, f32 v) {
    *(f32*)(p + 0xA8) = v;
}

void fn_80076A0C(u8* p, s32 v) {
    *(s32*)(p + 0x0) = v;
}
