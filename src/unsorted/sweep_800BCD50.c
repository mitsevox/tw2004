// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "golfer.h"

u8 fn_800BCD50(void);
s32 fn_800BCD5C(void);
u8 fn_800BCD50(void) {
    return gpGame->bD4;
}

s32 fn_800BCD5C(void) {
    return gpGame->nDC;
}
