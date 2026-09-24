// GoShaderObject_Rain_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the
// rain shader object. Partly decompiled.

#include "game_types.h"
#include "engine.h"
#include "rain.h"

void fn_800B4B5C(void);
void fn_800B4BB0(void);
void fn_800B4BB4(void);
void fn_800B4BB8(void);
void fn_800B4BD8(void);
void fn_800B4BFC(void);

void fn_800B4B5C(void) {
    lbl_802814B8->n0 = 0;
    lbl_802814B8->n4 = 0;
    lbl_802814B8->f8 = fn_80029B64(3075.0f);
    lbl_802814B8->pBank = NULL;
    lbl_802814B8->pTex = NULL;
}

void fn_800B4BB0(void) {
}

void fn_800B4BB4(void) {
}

void fn_800B4BB8(void) {
    lbl_802814B8->n0 = 0;
    lbl_802814B8->n4 = 1 - lbl_802814B8->n4;
}

void fn_800B4BD8(void) {
    lbl_802814B8->n0 = (lbl_802814B8->n0 + 1) % 2;
}

void fn_800B4BFC(void) {
}

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
