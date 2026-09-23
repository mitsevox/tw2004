// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_80030214(u8* p0);
s32 fn_80030234(u8* p0);
s32 fn_80030214(u8* p0) {
    return (*(s32*)(p0 + 0x8) + ((u32)((*(s32*)p0 * *(s32*)(p0 + 0x4)) * 5) >> 2));
}

s32 fn_80030234(u8* p0) {
    return (*(s32*)(p0 + 0x8) + (*(s32*)p0 * *(s32*)(p0 + 0x4)));
}
