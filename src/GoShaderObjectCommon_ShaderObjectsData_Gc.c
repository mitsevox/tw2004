// GoShaderObjectCommon_ShaderObjectsData_Gc.c (EA's name, from its asserts): builds the display
// lists that draw the shader objects (the grass and other course objects drawn through GX
// texture and colour stages). Partly decompiled.

#include "engine.h"
#include "gx.h"

int fn_80073878(TexBank* pBank, TexEntry* pTex);
u16 fn_80076D68(f32 fU, f32 fV, f32 fScale, f32 fInv);    // GoShaderObjectCommon_TexAnimManager_Gc.c

// Another texture in pTex's bank with the same u0 as pTex: its index, or -1.
int fn_80073878(TexBank* pBank, TexEntry* pTex) {
    int i;
    for (i = 0; i < pBank->n2; i++) {
        if (pBank->p8[i].u0 == pTex->u0 && pTex != &pBank->p8[i]) return i;
    }
    return -1;
}

// A command of the display list: three s16s make a texture matrix (fn_80076D68), loaded as
// texture matrix 0x39 (the first two: 0 for none, else 0.5 + 10000 / n; the third: a scale,
// made positive, and its inverse).
u16* fn_80074BE0(u16* p) {
    f32 fU;
    f32 fV;
    f32 fScale;
    f32 fInv;
    u16* pNext;

    fInv = fV = fU = 0.0f;
    if (p[0] != 0) {
        fU = 0.5f + 10000.0f / (s16)p[0];
    }
    if (p[1] != 0) {
        fV = 0.5f + 10000.0f / (s16)p[1];
    }
    fScale = (s16)p[2];
    pNext = p + 3;
    if (fScale != 0.0f) {
        fInv = 1.0f / fScale;
    }
    if (fScale < 0.0f) {
        fScale *= -1.0f;
    }
    GXLoadTexMtxIndx(fn_80076D68(fU, fV, fScale, fInv), 0x39, 1);
    return pNext;
}

// ---- sweep code (not yet cleaned up) ----

u8* fn_80074CEC(u8* p0);

u8* fn_80074CEC(u8* p0) {
    return (p0 + 0x2);
}

// ---- end of sweep code ----

// A command of the display list: the blend mode (0 none, 1 alpha blend, 2 add source times
// alpha, 3 subtract).
u16* fn_80074CF4(u16* p) {
    switch (*p++) {
    case 0:
        GXSetBlendMode(0, 4, 5, 0);
        break;
    case 1:
        GXSetBlendMode(1, 4, 5, 0);
        break;
    case 2:
        GXSetBlendMode(1, 4, 0, 0);
        break;
    case 3:
        GXSetBlendMode(3, 1, 1, 0);
        break;
    }
    return p;
}

// ---- sweep code (not yet cleaned up) ----

void fn_8007524C(void);
void fn_80075250(s32 p0);
void fn_8007525C(s32 p0);
void fn_80075268(s32 p0);
void fn_80075274(s32 p0);

void fn_8007524C(void) {
}

void fn_80075250(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_8007525C(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_80075268(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_80075274(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

// ---- end of sweep code ----
