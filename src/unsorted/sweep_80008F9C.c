// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_80008F9C(u8* p0, u8* p1);
void fn_80008F9C(u8* p0, u8* p1) {
    *(f32*)(p1 + 0xC) = *(f32*)(p0 + 0xC);
    *(f32*)p1 = -*(f32*)p0;
    *(f32*)(p1 + 0x4) = -*(f32*)(p0 + 0x4);
    *(f32*)(p1 + 0x8) = -*(f32*)(p0 + 0x8);
}
