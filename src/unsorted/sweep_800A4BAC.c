// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800A4A88();

s32 fn_800A4BAC(void);
s32 fn_800A4BAC(void) {
    s32 t0;
    t0 = fn_800A4A88();
    return ((u32)((-(t0 & 0xFF)) | (t0 & 0xFF)) >> 31);
}
