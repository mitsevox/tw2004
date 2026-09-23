// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_800CEEF4(u8* p);
s32 fn_800CEEFC(u8* p);
s32 fn_800CEEF4(u8* p) {
    return *(s32*)(p + 0x8);
}

s32 fn_800CEEFC(u8* p) {
    return *(s32*)(p + 0xC);
}
