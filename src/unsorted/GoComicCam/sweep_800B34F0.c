// Small functions found by the sweep (sweep.py). Original file: GoComicCam.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

extern u8 lbl_8018FF78[];
extern s32 lbl_80282178;
s32 fn_80009B34();

void fn_800B34F0(void);
void fn_800B34F0(void) {
    s32 t0;
    t0 = fn_80009B34(516, 2, 0, lbl_8018FF78, 91);
    lbl_80282178 = t0;
}
