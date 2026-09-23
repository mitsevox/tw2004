// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80280DD8;

s32 fn_800106C4(s32 p0);
s32 fn_800106C4(s32 p0) {
    return *(s32*)(((u8*)*(s32*)((u8*)lbl_80280DD8)) + (p0 * 12));
}
