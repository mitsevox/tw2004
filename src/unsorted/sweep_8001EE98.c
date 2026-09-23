// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_8001EE98(u8* p0, s32 p1);
f32 fn_8001EEA4(u8* p0, u8* p1);
void fn_8001EE98(u8* p0, s32 p1) {
    *(u8*)(((u8*)*(s32*)(p0 + 0x38)) + 0xEE) = p1;
}

f32 fn_8001EEA4(u8* p0, u8* p1) {
    return (*(f32*)(p0 + 0xC) * *(f32*)(p1 + 0xC) + (*(f32*)(p0 + 0x8) * *(f32*)(p1 + 0x8) + (*(f32*)p0 * *(f32*)p1 + (*(f32*)(p0 + 0x4) * *(f32*)(p1 + 0x4)))));
}
