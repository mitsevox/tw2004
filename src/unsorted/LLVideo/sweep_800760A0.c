// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_800760A0(u8* p);
u8 fn_800760A8(u8* p);
s32 fn_800760A0(u8* p) {
    return *(s32*)(p + 0x10A4);
}

u8 fn_800760A8(u8* p) {
    return *(u8*)(p + 0x1022);
}
