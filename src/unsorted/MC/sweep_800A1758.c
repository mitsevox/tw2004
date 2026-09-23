// Small functions found by the sweep (sweep.py). Original file: MC.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

s32 fn_800A2248();

s32 fn_800A1758(u8* p0);
s32 fn_800A1758(u8* p0) {
    s32 t0;
    t0 = fn_800A2248(*(s32*)p0, *(s32*)(p0 + 0x4));
    return ((u32)__cntlzw(t0) >> 5);
}
