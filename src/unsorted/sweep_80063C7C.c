// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_80063C7C(u8* p0);
s32 fn_80063C7C(u8* p0) {
    return (((u32)__cntlzw((4 - *(s32*)(p0 + 0x144))) >> 5) & 0xFF);
}
