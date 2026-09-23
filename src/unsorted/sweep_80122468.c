// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80194758[];
extern s32 lbl_80281970;

void fn_80122468(s32 p0);
void fn_80122468(s32 p0) {
    *(s32*)(((u8*)lbl_80281970) + 0x44) = *(s32*)(lbl_80194758 + ((-p0) << 2));
}
