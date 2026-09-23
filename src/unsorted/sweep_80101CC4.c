// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_802823FC;

s32 fn_80101CC4(void);
s32 fn_80101CC4(void) {
    return (((u32)__cntlzw((12 - lbl_802823FC)) >> 5) & 0xFF);
}
