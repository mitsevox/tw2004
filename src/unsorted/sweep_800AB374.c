// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80282080;

s32 fn_800AB374(void);
s32 fn_800AB374(void) {
    return (((u32)__cntlzw(lbl_80282080) >> 5) & 0xFF);
}
