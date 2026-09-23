// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_8001EDF4(u8* p0);
s32 fn_8001EDF4(u8* p0) {
    return *(u8*)(((u8*)*(s32*)(p0 + 0x38)) + 0xEE);
}
