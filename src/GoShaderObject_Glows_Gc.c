// GoShaderObject_Glows_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet
// decompiled; the sweep code below is the matched small functions.

#include "glows.h"
#include "gx.h"
#include "core/startup.h"
#include "unsorted/cull.h"

void fn_80097EC4(f32* pPos);
void fn_800124A8(void);                                 // LLFont.c: end the primitive
void fn_80012520(u32 ePrim, u32 eFormat, u16 nVerts);  // LLFont.c: GXBegin
void fn_80070168(void);                                 // calls a display list (see fn_80098408)

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

// Glow nGlow as nPoints four-pointed stars, each turned a further pi / nPoints: a strip per star
// through five points a quarter turn apart, alternately on the unit circle and at 1/32 of it.
void fn_800981D0(int nGlow, int nPoints) {
    GlowList* pList;
    int i;
    int j;
    f32 fRadius;
    f32 fAngle;
    f32 fBase;
    f32 v[4];                                   // the fourth is not used

    pList = fn_80097F44(nGlow, nPoints * 14);
    for (i = 0; i < nPoints; i++) {
        fBase = PI / (f32)nPoints * (f32)i + PI / 2.0f;
        fn_80012520(0x98, 4, 10);
        j = 0;
        do {
            fAngle = PI / 2.0f * (f32)j + fBase;
            if (j & 1) {
                fRadius = 0.03125f;
            } else {
                fRadius = 1.0f;
            }
            v[0] = fRadius * fn_80009638(fAngle);
            v[1] = fRadius * fn_800095F0(fAngle);
            v[2] = 0.0f;
            fn_80097EC4(v);
            j++;
        } while (j <= 4);
        fn_800124A8();
    }
    fn_80098004(pList);
}

// ---- sweep code (not yet cleaned up) ----

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

// Draws a queued glow at pPos (in view space) unless its n25 has bit 0x80: glow n25's display list
// through two matrices, the ring's inner edge scaled by f18 and its outer by f1C, both turned by
// f20, in the colours uColorA and uColorB; n24 draws it over everything.
void fn_80098408(GlowQueued* pGlow, f32* pPos) {
    f32 fSin;
    f32 fCos;
    f32 fOuter;
    f32 fInner;
    f32 x;
    f32 y;
    f32 z;
    f32 mInner[3][4];
    f32 mOuter[3][4];
    GXColor colour;

    if (!(pGlow->n25 & 0x80)) {
        if (0.0f != pGlow->f20) {
            fSin = fn_800095F0(pGlow->f20);
            fCos = fn_80009638(pGlow->f20);
        } else {
            fSin = 0.0f;
            fCos = 1.0f;
        }
        fInner = pGlow->f18;
        fOuter = pGlow->f1C;
        x = -pPos[0];
        y = pPos[1];
        z = -pPos[2];
        mInner[0][0] = -fInner * fCos;
        mInner[0][1] = fInner * fSin;
        mInner[0][2] = 0.0f;
        mInner[0][3] = x;
        mInner[1][0] = fInner * fSin;
        mInner[1][1] = fInner * fCos;
        mInner[1][2] = 0.0f;
        mInner[1][3] = y;
        mInner[2][0] = 0.0f;
        mInner[2][1] = 0.0f;
        mInner[2][2] = -1.0f;
        mInner[2][3] = z;
        mOuter[0][0] = -fOuter * fCos;
        mOuter[0][1] = fOuter * fSin;
        mOuter[0][2] = 0.0f;
        mOuter[0][3] = x;
        mOuter[1][0] = fOuter * fSin;
        mOuter[1][1] = fOuter * fCos;
        mOuter[1][2] = 0.0f;
        mOuter[1][3] = y;
        mOuter[2][0] = 0.0f;
        mOuter[2][1] = 0.0f;
        mOuter[2][2] = -1.0f;
        mOuter[2][3] = z;
        GXLoadPosMtxImm(mInner, 0);
        GXLoadPosMtxImm(mOuter, 3);
        colour.r = pGlow->uColorA;
        colour.g = pGlow->uColorA >> 8;
        colour.b = pGlow->uColorA >> 16;
        colour.a = pGlow->uColorA >> 24;
        GXSetTevColor(1, colour);
        colour.r = pGlow->uColorB;
        colour.g = pGlow->uColorB >> 8;
        colour.b = pGlow->uColorB >> 16;
        colour.a = pGlow->uColorB >> 24;
        GXSetTevColor(2, colour);
        if (pGlow->n24) {
            GXSetZMode(0, 7, 0);                // no depth test, no depth write
        } else {
            GXSetZMode(1, 3, 1);                // less or equal, with depth write
        }
        // port: fn_80070168 (sweep_80070168.c) is defined without parameters but hands r3 and r4
        // on to GXCallDisplayList: the list and its size
        ((void (*)(void*, u32))fn_80070168)(lbl_801D99D0.a[pGlow->n25].p4, lbl_801D99D0.a[pGlow->n25].n0);
    }
}

// Draws the queued glows whose n24 is bOnTop, each through pMtx, when it ends up in front (z > 0).
void fn_800985FC(GlowQueue* pQueue, f32 (*pMtx)[4], int bOnTop) {
    GlowQueued* pGlow;
    int i;
    Vec4 v;

    fn_8001614C();
    GXSetClipMode(0);
    GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
    GXSetNumTevStages(1);
    GXSetTevColorIn(0, 2, 4, 10, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 1, 2, 5, 7);
    GXSetTevAlphaOp(0, 0, 0, 1, 1, 0);
    pGlow = pQueue->a;
    for (i = 0; i < pQueue->nCount; i++) {
        if (!(pGlow->n24 ^ bOnTop)) {
            v.x = pGlow->vPos[0];
            v.y = pGlow->vPos[1];
            v.z = pGlow->vPos[2];
            v.w = 1.0f;
            fn_800BAD60(pMtx, &v, &v);
            if (v.z > 0.0f) {
                fn_80098408(pGlow, &v.x);
            }
        }
        pGlow++;
    }
}

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
    fn_800985FC(*(GlowQueue**)(p0 + 0x4), *(f32 (**)[4])(p0 + 0xC), ((u32)__cntlzw(*(s32*)(p0 + 0x8)) >> 5));
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
