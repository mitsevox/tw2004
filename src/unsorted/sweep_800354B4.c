// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800354B4(u8* p, f32 v);
s32 fn_800354BC(u8* p);
void fn_800354B4(u8* p, f32 v) {
    *(f32*)(p + 0xAC) = v;
}

s32 fn_800354BC(u8* p) {
    return *(s32*)(p + 0x14);
}
