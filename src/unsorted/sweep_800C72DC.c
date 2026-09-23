// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_800C72DC(u8* p0);
s32 fn_800C72DC(u8* p0) {
    return ((u32)((-*(s32*)(p0 + 0x198)) | *(s32*)(p0 + 0x198)) >> 31);
}
