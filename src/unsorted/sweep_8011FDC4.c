// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281900;

void fn_8011FDC4(s32 p0);
void fn_8011FDC4(s32 p0) {
    *(s32*)(((u8*)*(s32*)(((u8*)lbl_80281900) + 0xD8)) + (*(s32*)(((u8*)lbl_80281900) + 0xE4) << 2)) = p0;
    *(s32*)(((u8*)lbl_80281900) + 0xE4) = (*(s32*)(((u8*)lbl_80281900) + 0xE4) + 1);
}
