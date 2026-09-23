// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800A2248();

s32 fn_800A1758(u8* p0);
s32 fn_800A1758(u8* p0) {
    s32 t0;
    t0 = fn_800A2248(*(s32*)p0, *(s32*)(p0 + 0x4));
    return ((u32)__cntlzw(t0) >> 5);
}
