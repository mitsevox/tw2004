// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800734D0();

s32 fn_800734A0(void);
s32 fn_800734A0(void) {
    s32 t0;
    t0 = fn_800734D0();
    return ((u32)((1 - t0) | (t0 - 1)) >> 31);
}
