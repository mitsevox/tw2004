// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_8009637C(u8* p0);
s32 fn_8009637C(u8* p0) {
    return ((u32)((1 - ((s8)*(u8*)(p0 + 0x26))) | (((s8)*(u8*)(p0 + 0x26)) - 1)) >> 31);
}
