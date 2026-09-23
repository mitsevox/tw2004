// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800F0EA0();

s32 fn_800F1008(void);
s32 fn_800F1008(void) {
    s32 t0;
    t0 = fn_800F0EA0();
    return *(s32*)(((u8*)t0) + 0x14);
}
