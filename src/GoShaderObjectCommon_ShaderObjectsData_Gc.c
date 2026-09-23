// GoShaderObjectCommon_ShaderObjectsData_Gc.c (EA's name, from its asserts): builds the display
// lists that draw the shader objects (the grass and other course objects drawn through GX
// texture and colour stages). Partly decompiled.

#include "engine.h"

int fn_80073878(TexBank* pBank, TexEntry* pTex);

// Another texture in pTex's bank with the same u0 as pTex: its index, or -1.
int fn_80073878(TexBank* pBank, TexEntry* pTex) {
    int i;
    for (i = 0; i < pBank->n2; i++) {
        if (pBank->p8[i].u0 == pTex->u0 && pTex != &pBank->p8[i]) return i;
    }
    return -1;
}

// ---- sweep code (not yet cleaned up) ----

u8* fn_80074CEC(u8* p0);

u8* fn_80074CEC(u8* p0) {
    return (p0 + 0x2);
}

// ---- end of sweep code ----

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
