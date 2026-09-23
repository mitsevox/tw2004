// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_800B9930(u8* p0);
s32 fn_800B9930(u8* p0) {
    return (((u32)__cntlzw((2 - *(u8*)(p0 + 0x14))) >> 5) & 0xFF);
}
