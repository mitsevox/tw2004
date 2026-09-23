// GoShaderObject_Rain_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): not
// yet decompiled; the sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_800B4BB0(void);
void fn_800B4BB4(void);
extern s32 lbl_802814B8;
void fn_800B4BB8(void);
void fn_800B4BD8(void);
void fn_800B4BFC(void);

void fn_800B4BB0(void) {
}

void fn_800B4BB4(void) {
}

void fn_800B4BB8(void) {
    *(s32*)((u8*)lbl_802814B8) = 0;
    *(s32*)(((u8*)lbl_802814B8) + 0x4) = (1 - *(s32*)(((u8*)lbl_802814B8) + 0x4));
}

void fn_800B4BD8(void) {
    *(s32*)((u8*)lbl_802814B8) = ((((*(s32*)((u8*)lbl_802814B8) + 1) & 0x1) ^ ((u32)(*(s32*)((u8*)lbl_802814B8) + 1) >> 31)) - ((u32)(*(s32*)((u8*)lbl_802814B8) + 1) >> 31));
}

void fn_800B4BFC(void) {
}

// ---- end of sweep code ----
