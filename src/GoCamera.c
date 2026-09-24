// GoCamera.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): a render camera's
// lens (CamLens): its field of view, where it stands and what it looks at, as a camera-to-world
// matrix (m4) and its inverse (m44).

#include "camera.h"

void fn_8000A0E8(f32 (*pSrc)[4], f32 (*pDst)[4]);     // UMemPool.c: copy a 4x4 matrix
void fn_8000A798(f32 (*pSrc)[4], f32 (*pDst)[4]);     // UMemPool.c: inverts a rotation+translation
void fn_8000ADC0(f32 (*pMtx)[4]);                     // identity
void fn_8001728C(CamLens* pLens);
f32  fn_80014280(f32 x);                              // tan, as a float
void fn_800354B4(u8* p, f32 v);                       // GoTerrain.c: sets the lens's fAC
void fn_800BADF8(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows);
void CA_vSetMatrix(CamLens* pLens, f32 (*pMtx)[4]);
void fn_800769C0(CamLens* pLens, f32 fA8, f32 fAC);
void fn_80076A04(CamLens* pLens, f32 fA8);
void fn_80076A14(f32* pA, f32* pB, f32* pOut);
void fn_80076A38(f32* pA, f32* pOut);

// fB0 is 1 at the default 60-degree field of view: tan(fov / 2) over tan(30 degrees).
void CA_vUpdateInternalFieldOfViewData(CamLens* pLens) {
    pLens->fB0 = fn_80014280(0.5f * pLens->fFov) / 0.57735026f;
}

CamLens* CA_spCreateCamera(void) {
    CamLens* pLens = fn_80009B34(sizeof(CamLens), 2, 16, "GoCamera.c", 152);

    CA_vInitCamera(pLens);
    return pLens;
}

void CA_vDestroyCamera(CamLens* pLens) {
    fn_80009E70(pLens);
}

// fake match: stands in for a function the original linker stripped. The file's pool has 0.1,
// 4096, 60 degrees and 20 (CA_vInitCamera's settings) right after CA_vUpdateInternalFieldOfViewData's constants, before
// the 1.0 and 0.0 CA_vSetLookAt uses first; its body is unknown, this one only reproduces the order.
static void GoCamera_StrippedFn(CamLens* pLens) {
    fn_800769C0(pLens, 0.1f, 4096.0f);
    fn_80045470(pLens, DEG(60.0f));
    fn_80076948(pLens, 20.0f, 20.0f);
}

// Stands the lens at pPos looking at pTarget, level (its x axis flat) unless it looks almost
// straight up or down, where it keeps the old x axis. A target closer than 0.1 keeps the old aim.
void CA_vSetLookAt(CamLens* pLens, f32* pPos, f32* pTarget) {
    f32 vDir[4];

    fn_8001728C(pLens);
    pLens->m4[3][0] = pPos[0];
    pLens->m4[3][1] = pPos[1];
    pLens->m4[3][2] = pPos[2];
    pLens->m4[3][3] = 1.0f;
    vDir[3] = 0.0f;
    fn_80076A14(pTarget, pPos, vDir);
    if ((f32)fn_80009680(fn_80009744(vDir)) > 0.1f) {
        fn_800BAF04(vDir, pLens->m4[2]);
        if (fabsf(pLens->m4[2][1]) < 0.99f) {
            pLens->m4[0][0] = pLens->m4[2][2];
            pLens->m4[0][1] = 0.0f;
            pLens->m4[0][2] = -pLens->m4[2][0];
            pLens->m4[0][3] = 0.0f;
            fn_800BAF04(pLens->m4[0], pLens->m4[0]);
        }
        vec4flt_CrossProduct(pLens->m4[2], pLens->m4[0], pLens->m4[1]);
    }
    fn_8000A798(pLens->m4, pLens->m44);
}

// The same with the lens's x axis given (pSide, normalised here).
void CA_vSetLookAtSide(CamLens* pLens, f32* pPos, f32* pTarget, f32* pSide) {
    f32 vDir[4];

    fn_8001728C(pLens);
    pLens->m4[3][0] = pPos[0];
    pLens->m4[3][1] = pPos[1];
    pLens->m4[3][2] = pPos[2];
    pLens->m4[3][3] = 1.0f;
    vDir[3] = 0.0f;
    fn_80076A14(pTarget, pPos, vDir);
    if ((f32)fn_80009680(fn_80009744(vDir)) > 0.1f) {
        fn_800BAF04(vDir, pLens->m4[2]);
        pLens->m4[0][0] = pSide[0];
        pLens->m4[0][1] = pSide[1];
        pLens->m4[0][2] = pSide[2];
        pLens->m4[0][3] = 0.0f;
        fn_800BAF04(pLens->m4[0], pLens->m4[0]);
        vec4flt_CrossProduct(pLens->m4[2], pLens->m4[0], pLens->m4[1]);
    }
    fn_8000A798(pLens->m4, pLens->m44);
}

// Aims the lens like CA_vSetLookAt, then scales the world by pScale around pCenter: m44 gets the
// scale, m4 its inverse (1 / pScale, kept in m84[0]).
void fn_80076664_SetScaledLookAt(CamLens* pLens, f32* pPos, f32* pTarget, f32* pCenter, f32* pScale) {
    f32 vDir[4];
    f32 mB[4][4];
    f32 mA[4][4];
    f32 mTmp[4][4];

    Vec_Copy(pScale, pLens->m84[1]);
    fn_80076A38(pLens->m84[1], pLens->m84[0]);
    pLens->m4[3][0] = pPos[0];
    pLens->m4[3][1] = pPos[1];
    pLens->m4[3][2] = pPos[2];
    pLens->m4[3][3] = 1.0f;
    vDir[3] = 0.0f;
    fn_80076A14(pTarget, pPos, vDir);
    if ((f32)fn_80009680(fn_80009744(vDir)) > 0.1f) {
        fn_800BAF04(vDir, pLens->m4[2]);
        if (fabsf(pLens->m4[2][1]) < 0.99f) {
            pLens->m4[0][0] = pLens->m4[2][2];
            pLens->m4[0][1] = 0.0f;
            pLens->m4[0][2] = -pLens->m4[2][0];
            pLens->m4[0][3] = 0.0f;
            fn_800BAF04(pLens->m4[0], pLens->m4[0]);
        }
        vec4flt_CrossProduct(pLens->m4[2], pLens->m4[0], pLens->m4[1]);
    }
    fn_8000A798(pLens->m4, pLens->m44);

    // world to camera: move pCenter to the origin, scale, move it back
    fn_8000ADC0(mTmp);
    mTmp[3][0] = pCenter[0];
    mTmp[3][1] = pCenter[1];
    mTmp[3][2] = pCenter[2];
    fn_800BADF8(mTmp, pLens->m44, mB, 4);
    fn_8000ADC0(mTmp);
    mTmp[0][0] = pScale[0];
    mTmp[1][1] = pScale[1];
    mTmp[2][2] = pScale[2];
    fn_800BADF8(mTmp, mB, mA, 4);
    fn_8000ADC0(mTmp);
    mTmp[3][0] = -pCenter[0];
    mTmp[3][1] = -pCenter[1];
    mTmp[3][2] = -pCenter[2];
    fn_800BADF8(mTmp, mA, pLens->m44, 4);

    // camera to world: the same with the inverse scale
    fn_8000ADC0(mTmp);
    mTmp[3][0] = pCenter[0];
    mTmp[3][1] = pCenter[1];
    mTmp[3][2] = pCenter[2];
    fn_8000ADC0(mB);
    mB[0][0] = pLens->m84[0][0];
    mB[1][1] = pLens->m84[0][1];
    mB[2][2] = pLens->m84[0][2];
    fn_800BADF8(mB, mTmp, mA, 4);
    fn_8000ADC0(mTmp);
    mTmp[3][0] = -pCenter[0];
    mTmp[3][1] = -pCenter[1];
    mTmp[3][2] = -pCenter[2];
    fn_800BADF8(mTmp, mA, mB, 4);
    fn_800BADF8(pLens->m4, mB, mA, 4);
    fn_8000A0E8(mA, pLens->m4);
    fn_800BADF8(pLens->m4, pLens->m44, mB, 4);   // the result is never used
}

// A new lens's settings: a perspective camera, fA8 0.1, fAC 4096, a 60-degree field of view,
// the identity matrix, a 20 x 20 flat view.
void CA_vInitCamera(CamLens* pLens) {
    fn_80076A0C_SetType(pLens, 0);
    fn_800769C0(pLens, 0.1f, 4096.0f);
    fn_80045470(pLens, DEG(60.0f));
    CA_vSetMatrix(pLens, NULL);
    fn_80076948(pLens, 20.0f, 20.0f);
}

void fn_80076948(CamLens* pLens, f32 fB4, f32 fB8) {
    pLens->fB4 = fB4;
    pLens->fB8 = fB8;
}

// Sets the lens's camera-to-world matrix (pMtx, or the identity when NULL) and its inverse.
void CA_vSetMatrix(CamLens* pLens, f32 (*pMtx)[4]) {
    if (pMtx == NULL) {
        fn_8000ADC0(pLens->m4);
        fn_8000ADC0(pLens->m44);
    } else {
        fn_8000A0E8(pMtx, pLens->m4);
        fn_8000A798(pMtx, pLens->m44);
    }
    fn_8001728C(pLens);
}

void fn_800769C0(CamLens* pLens, f32 fA8, f32 fAC) {
    fn_80076A04(pLens, fA8);
    fn_800354B4((u8*)pLens, fAC);
}

void fn_80076A04(CamLens* pLens, f32 fA8) {
    pLens->fA8 = fA8;
}

void fn_80076A0C_SetType(CamLens* pLens, s32 nType) {
    pLens->nType = nType;
}

// a - b into out (three floats)
#ifdef __MWERKS__
asm void fn_80076A14(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_80076A14(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// 1 / a into out, each of the three floats (ps_res: the hardware's reciprocal estimate)
#ifdef __MWERKS__
asm void fn_80076A38(register f32* pA, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    ps_res f0, f0
    ps_res f1, f1
    psq_st f0, 0(pOut), 0, 0
    psq_st f1, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles; ps_res is an
// estimate good to about 1/4096, this is the exact reciprocal.
void fn_80076A38(f32* pA, f32* pOut) {
    pOut[0] = 1.0f / pA[0];
    pOut[1] = 1.0f / pA[1];
    pOut[2] = 1.0f / pA[2];
}
#endif
