// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8009EE28();

s32 fn_800A2630(u8* p0);
s32 fn_800A2668(u8* p0);
s32 fn_800A26A0(u8* p0);
s32 fn_800A2630(u8* p0) {
    s32 t0;
    t0 = fn_8009EE28(*(s32*)p0, *(s32*)(p0 + 0x4));
    return ((u32)__cntlzw((-18 - t0)) >> 5);
}

s32 fn_800A2668(u8* p0) {
    s32 t0;
    t0 = fn_8009EE28(*(s32*)p0, *(s32*)(p0 + 0x4));
    return ((u32)__cntlzw((-18 - t0)) >> 5);
}

s32 fn_800A26A0(u8* p0) {
    s32 t0;
    t0 = fn_8009EE28(*(s32*)p0, *(s32*)(p0 + 0x4));
    return ((u32)__cntlzw((-18 - t0)) >> 5);
}
