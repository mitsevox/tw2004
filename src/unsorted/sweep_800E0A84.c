// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* gpGame;

void fn_800E0A84(u8 v);
s32 fn_800E0A90(void);
void fn_800E0A84(u8 v) {
    *(u8*)(gpGame + 0x135) = v;
}

s32 fn_800E0A90(void) {
    return 0;
}
