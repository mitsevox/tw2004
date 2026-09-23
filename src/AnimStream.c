// AnimStream.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern s32 lbl_80282230;
void fn_800C9EFC(s32 p0);
void fn_800CB550(s32 p0);

void fn_800C9EFC(s32 p0) {
    *(s32*)(((u8*)lbl_80282230) + 0x1CC0) = 2;
    *(s32*)(((u8*)lbl_80282230) + 0x1CC4) = p0;
}

void fn_800CB550(s32 p0) {
    *(u8*)(((u8*)lbl_80282230) + 0x1CCC) = 1;
    *(s32*)(((u8*)lbl_80282230) + 0x1CC4) = p0;
}

// ---- end of sweep code ----
