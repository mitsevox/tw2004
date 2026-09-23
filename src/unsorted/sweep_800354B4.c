// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800354B4(u8* p, f32 v);
void fn_800354B4(u8* p, f32 v) {
    *(f32*)(p + 0xAC) = v;
}

s32 fn_800354BC(s32 p) {
    return *(s32*)((u8*)p + 0x14);
}
