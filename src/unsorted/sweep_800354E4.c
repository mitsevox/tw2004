// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_800354E4(u8* p0, s32 p1);
s32 fn_800354F4(u8* p0);
s32 fn_800354E4(u8* p0, s32 p1) {
    return *(s32*)(((u8*)*(s32*)(p0 + 0x8)) + (p1 << 2));
}

s32 fn_800354F4(u8* p0) {
    return *(s16*)((u8*)*(s32*)p0);
}
