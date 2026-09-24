// GoShaderObject_Glows_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet
// decompiled; the sweep code below is the matched small functions.

#include "glows.h"
#include "gx.h"
#include "core/startup.h"

void fn_80097EC4(f32* pPos);
void fn_800124A8(void);                                 // LLFont.c: end the primitive
void fn_80012520(u32 ePrim, u32 eFormat, u16 nVerts);  // LLFont.c: GXBegin

// Starts glow nGlow's display list, with room for nVerts vertices of 32 bytes and 1 KB more.
GlowList* fn_80097F44(int nGlow, int nVerts) {
    u32 uSize;
    void* pList;

    uSize = ((nVerts * 32 + 31) & ~31) + 0x400;
    pList = fn_80009B34(uSize, 1, 32, "GoShaderObject_Glows_Gc.c", 134);
    DCInvalidateRange(pList, uSize);
    GXBeginDisplayList(pList, uSize);
    GXResetWriteGatherPipe();
    GXClearVtxDesc();
    GXSetVtxDesc(0, 1);                         // matrix index, position, colour: direct
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(11, 1);
    GXInvalidateVtxCache();
    lbl_801D99D0.a[nGlow].p4 = pList;
    return &lbl_801D99D0.a[nGlow];
}

// Ends pList's display list and keeps a copy of just its size.
void fn_80098004(GlowList* pList) {
    void* pCopy;
    u32 uSize;

    uSize = GXEndDisplayList();
    pCopy = fn_80009B34(uSize, 2, 32, "GoShaderObject_Glows_Gc.c", 159);
    Mem_cpy(pCopy, pList->p4, uSize);
    DCFlushRange(pCopy, uSize);
    fn_80009E70(pList->p4);
    pList->p4 = pCopy;
    pList->n0 = uSize;
    lbl_801D99D0.nCount++;
}

// Glow nGlow as a ring of nSides sides around the unit circle: a strip with two vertices at each
// point (fn_80097EC4), starting and ending at (1, 0, 0).
void fn_8009809C(int nGlow, int nSides) {
    int nVerts;
    GlowList* pList;
    int i;
    f32 fAngle;
    f32 vRim[4];                                // the fourth is not used
    f32 vStart[4];

    nVerts = (nSides + 1) * 2;
    pList = fn_80097F44(nGlow, nVerts);
    fn_80012520(0x98, 4, nVerts);
    vStart[0] = 1.0f;
    vStart[1] = 0.0f;
    vStart[2] = 0.0f;
    fn_80097EC4(vStart);
    for (i = 1; i < nSides; i++) {
        fAngle = (2.0f * PI) * ((f32)i / (f32)nSides);
        vRim[0] = fn_80009638(fAngle);
        vRim[1] = fn_800095F0(fAngle);
        vRim[2] = 0.0f;
        fn_80097EC4(vRim);
    }
    fn_80097EC4(vStart);
    fn_800124A8();
    fn_80098004(pList);
}

// ---- sweep code (not yet cleaned up) ----

void fn_800981D0();
void fn_80098350(void);

void fn_80098350(void) {
    fn_8009809C(8, 3);
    fn_8009809C(9, 4);
    fn_8009809C(10, 5);
    fn_8009809C(11, 6);
    fn_8009809C(12, 8);
    fn_8009809C(0, 10);
    fn_8009809C(1, 16);
    fn_8009809C(2, 24);
    fn_8009809C(3, 32);
    fn_800981D0(4, 1);
    fn_800981D0(5, 2);
    fn_800981D0(6, 3);
    fn_800981D0(7, 4);
}

// ---- end of sweep code ----

void fn_80098740(void) {
    s32 i;
    lbl_801D99D0.nCount = 0;
    for (i = 0; i < NUM_GLOWS; i++) {
        lbl_801D99D0.a[i].p4 = NULL;
    }
    fn_80098350();
}

// Free the glows' data. The pointers are left as they were.
void fn_800987D4(void) {
    int i;
    for (i = 0; i < NUM_GLOWS; i++) {
        if (lbl_801D99D0.a[i].p4 != NULL) {
            fn_80009E70(lbl_801D99D0.a[i].p4);
            lbl_801D99D0.nCount--;
        }
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80098844(void);
void fn_80098848(void);
void fn_800985FC();
void fn_8009884C(u8* p0);
void fn_80098884(u8* p0, u8* p1);
void fn_800988A0(s32 p0, s32 p1, s32 p2, s32 p3);
void fn_800988B8(f32 farg0, f32 farg1, f32 farg2);
void fn_800988CC(s32 p0);
extern u8 lbl_801D9A40[];
void fn_80036054();
void fn_800360A0();
void fn_800988D8(void);
void fn_80098910(void);

void fn_80098844(void) {
}

void fn_80098848(void) {
}

void fn_8009884C(u8* p0) {
    fn_800985FC(*(s32*)(p0 + 0x4), *(s32*)(p0 + 0xC), ((u32)__cntlzw(*(s32*)(p0 + 0x8)) >> 5));
}

void fn_80098884(u8* p0, u8* p1) {
    *(s32*)(p0 + 0x4) = *(s32*)p1;
    *(s32*)(p0 + 0x8) = *(s32*)(p1 + 0x4);
    *(s32*)(p0 + 0xC) = *(s32*)(p1 + 0x8);
}

void fn_800988A0(s32 p0, s32 p1, s32 p2, s32 p3) {
    *(volatile u8*)0xCC008000 = p0;
    *(volatile u8*)0xCC008000 = p1;
    *(volatile u8*)0xCC008000 = p2;
    *(volatile u8*)0xCC008000 = p3;
}

void fn_800988B8(f32 farg0, f32 farg1, f32 farg2) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
    *(f32* )0xCC008000 = farg2;
}

void fn_800988CC(s32 p0) {
    *(volatile u8*)0xCC008000 = p0;
}

void fn_800988D8(void) {
    fn_80036054(lbl_801D9A40, 11, 0);
    lbl_80281F80 = NULL;
}

void fn_80098910(void) {
    fn_800360A0(lbl_801D9A40);
}

// ---- end of sweep code ----
