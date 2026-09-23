// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_800C44CC(u8* p0);
s32 fn_800C44E0(u8* p0);
s32 fn_800C44CC(u8* p0) {
    return (((u32)__cntlzw((7 - *(s32*)(p0 + 0x260))) >> 5) & 0xFF);
}

s32 fn_800C44E0(u8* p0) {
    return (((u32)__cntlzw((3 - *(s32*)(p0 + 0x260))) >> 5) & 0xFF);
}
