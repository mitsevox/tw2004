// Small functions found by the sweep (sweep.py). Original file: startUp.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

s32 fn_800B5D34(s32, s32, s32);
extern s32 lbl_802820F8;

void fn_800B0698(s32 arg0);
void fn_800B0698(s32 arg0) {
    fn_800B5D34(lbl_802820F8, (arg0 + 0x1F) & 0xFFFFFFE0, 0x20);
}