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

// ---- sweep code (not yet cleaned up) ----

void fn_800B58B4(s32 p0);
void fn_800B58C0(f32 farg0, f32 farg1, f32 farg2);
void fn_800B58D4(f32 farg0, f32 farg1);
void fn_800B58E4(s32 p0, s32 p1, s32 p2, s32 p3);

void fn_800B58B4(s32 p0) {
    *(volatile u8*)0xCC008000 = p0;
}

void fn_800B58C0(f32 farg0, f32 farg1, f32 farg2) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
    *(f32* )0xCC008000 = farg2;
}

void fn_800B58D4(f32 farg0, f32 farg1) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
}

void fn_800B58E4(s32 p0, s32 p1, s32 p2, s32 p3) {
    *(volatile u8*)0xCC008000 = p0;
    *(volatile u8*)0xCC008000 = p1;
    *(volatile u8*)0xCC008000 = p2;
    *(volatile u8*)0xCC008000 = p3;
}

// ---- end of sweep code ----
