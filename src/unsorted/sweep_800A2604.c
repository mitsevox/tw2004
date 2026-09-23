// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281FF0;

s32 fn_800A2604(s32 p0);
s32 fn_800A2604(s32 p0) {
    return *(u8*)(((u8*)lbl_80281FF0) + (p0 * 76));
}
