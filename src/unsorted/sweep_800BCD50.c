// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* gpGame;

u8 fn_800BCD50(void);
s32 fn_800BCD5C(void);
u8 fn_800BCD50(void) {
    return *(u8*)(gpGame + 0xD4);
}

s32 fn_800BCD5C(void) {
    return *(s32*)(gpGame + 0xDC);
}
