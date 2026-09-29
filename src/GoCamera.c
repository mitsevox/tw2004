// GoCamera.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): a render
// context's lens (TW07's CA_SCamera, here CamLens): its projection (perspective or flat), near and
// far clip, field of view, and where it stands and what it looks at, as a camera-to-world matrix
// (m4) and its inverse (m44), optionally with the world scaled around a point. TW07's GoCamera.c
// has the first six named functions in this order (after its empty CA_vInitOnce and
// CA_vCloseOnce, not in this file); the setters after them are GameCube copies of GoCamera.h's
// inlines, and Camera_Subtract3 / Camera_Invert3 paired-single copies of LLMath's.

#include "camera.h"

void LLMath_CopyMat44(f32 (*pSrc)[4], f32 (*pDst)[4]);     // UMemPool.c: copy a 4x4 matrix
void LLMath_InvertNormalized(f32 (*pSrc)[4], f32 (*pDst)[4]);     // UMemPool.c: rotation+translation inverse
void LLMath_IdentifyMat(f32 (*pMtx)[4]);                     // identity
void CA_vSetDefaultScalingVectors(CamLens* pLens);
f32  Math_Tan(f32 x);                              // tan, as a float
void Camera_SetLensFarClip(u8* p, f32 v);             // GoTerrain.c: sets the lens's far clip, fAC
void LLMath_mat44fltMultiplyList(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows);
void CA_vSetCameraViewToWorldMatrix(CamLens* pLens, f32 (*pMtx)[4]);
void CA_vSetCameraNearFarZ(CamLens* pLens, f32 fA8, f32 fAC);
void CA_vSetCameraNearZ(CamLens* pLens, f32 fA8);
void Camera_Subtract3(f32* pA, f32* pB, f32* pOut);
void Camera_Invert3(f32* pA, f32* pOut);

// Works out the lens's fB0 from its field of view: tan(fov / 2) over tan(30 degrees), so 1 at the
// default 60 degrees.
void CA_vUpdateInternalFieldOfViewData(CamLens* pLens) {
    pLens->fB0 = Math_Tan(0.5f * pLens->fFov) / 0.57735026f;
}

// A new lens (0xBC bytes) with CA_vSetDefaultCamera's settings.
CamLens* CA_spCreateCamera(void) {
    CamLens* pLens = StaticMem_Alloc(sizeof(CamLens), 2, 16, "GoCamera.c", 152);

    CA_vSetDefaultCamera(pLens);
    return pLens;
}

void CA_vReleaseCamera(CamLens* pLens) {
    StaticMem_Free(pLens);
}

// fake match: stands in for a function the original linker stripped. The file's pool has 0.1,
// 4096, 60 degrees and 20 (CA_vSetDefaultCamera's settings) right after
// CA_vUpdateInternalFieldOfViewData's constants, before the 1.0 and 0.0
// Camera_SetCameraPositionAndTarget uses first; its body is unknown, this one only reproduces the order.
static void GoCamera_StrippedFn(CamLens* pLens) {
    CA_vSetCameraNearFarZ(pLens, 0.1f, 4096.0f);
    CA_vSetCameraFieldOfView(pLens, DEG(60.0f));
    CA_vSetCameraFlatSize(pLens, 20.0f, 20.0f);
}

// Stands the lens at pPos looking at pTarget, level (its x axis flat) unless it looks almost
// straight up or down, where it keeps the old x axis. A target closer than 0.1 keeps the old aim
// (TW07 passes that 0.1 as a threshold parameter).
void Camera_SetCameraPositionAndTarget(CamLens* pLens, f32* pPos, f32* pTarget) {
    f32 vDir[4];

    CA_vSetDefaultScalingVectors(pLens);
    pLens->m4[3][0] = pPos[0];
    pLens->m4[3][1] = pPos[1];
    pLens->m4[3][2] = pPos[2];
    pLens->m4[3][3] = 1.0f;
    vDir[3] = 0.0f;
    Camera_Subtract3(pTarget, pPos, vDir);
    if ((f32)Math_Sqrt(Vec3_LengthSqClamped(vDir)) > 0.1f) {
        LLMath_Normalize3(vDir, pLens->m4[2]);
        if (fabsf(pLens->m4[2][1]) < 0.99f) {
            pLens->m4[0][0] = pLens->m4[2][2];
            pLens->m4[0][1] = 0.0f;
            pLens->m4[0][2] = -pLens->m4[2][0];
            pLens->m4[0][3] = 0.0f;
            LLMath_Normalize3(pLens->m4[0], pLens->m4[0]);
        }
        vec4flt_CrossProduct(pLens->m4[2], pLens->m4[0], pLens->m4[1]);
    }
    LLMath_InvertNormalized(pLens->m4, pLens->m44);
}

// The same as Camera_SetCameraPositionAndTarget with the lens's x axis given (pSide, normalised
// here).
void Camera_SetCameraPositionAndTargetWithSideVector(CamLens* pLens, f32* pPos, f32* pTarget, f32* pSide) {
    f32 vDir[4];

    CA_vSetDefaultScalingVectors(pLens);
    pLens->m4[3][0] = pPos[0];
    pLens->m4[3][1] = pPos[1];
    pLens->m4[3][2] = pPos[2];
    pLens->m4[3][3] = 1.0f;
    vDir[3] = 0.0f;
    Camera_Subtract3(pTarget, pPos, vDir);
    if ((f32)Math_Sqrt(Vec3_LengthSqClamped(vDir)) > 0.1f) {
        LLMath_Normalize3(vDir, pLens->m4[2]);
        pLens->m4[0][0] = pSide[0];
        pLens->m4[0][1] = pSide[1];
        pLens->m4[0][2] = pSide[2];
        pLens->m4[0][3] = 0.0f;
        LLMath_Normalize3(pLens->m4[0], pLens->m4[0]);
        vec4flt_CrossProduct(pLens->m4[2], pLens->m4[0], pLens->m4[1]);
    }
    LLMath_InvertNormalized(pLens->m4, pLens->m44);
}

// Aims the lens like Camera_SetCameraPositionAndTarget, then scales the world by pScale around
// pCenter: m44 gets the scale, m4 its inverse (1 / pScale, kept in m84[0]).
void Camera_SetCameraPositionAndTargetWithOffsetAndScale(CamLens* pLens, f32* pPos, f32* pTarget, f32* pCenter, f32* pScale) {
    f32 vDir[4];
    f32 mB[4][4];
    f32 mA[4][4];
    f32 mTmp[4][4];

    LLMath_CopyVec(pScale, pLens->m84[1]);
    Camera_Invert3(pLens->m84[1], pLens->m84[0]);
    pLens->m4[3][0] = pPos[0];
    pLens->m4[3][1] = pPos[1];
    pLens->m4[3][2] = pPos[2];
    pLens->m4[3][3] = 1.0f;
    vDir[3] = 0.0f;
    Camera_Subtract3(pTarget, pPos, vDir);
    if ((f32)Math_Sqrt(Vec3_LengthSqClamped(vDir)) > 0.1f) {
        LLMath_Normalize3(vDir, pLens->m4[2]);
        if (fabsf(pLens->m4[2][1]) < 0.99f) {
            pLens->m4[0][0] = pLens->m4[2][2];
            pLens->m4[0][1] = 0.0f;
            pLens->m4[0][2] = -pLens->m4[2][0];
            pLens->m4[0][3] = 0.0f;
            LLMath_Normalize3(pLens->m4[0], pLens->m4[0]);
        }
        vec4flt_CrossProduct(pLens->m4[2], pLens->m4[0], pLens->m4[1]);
    }
    LLMath_InvertNormalized(pLens->m4, pLens->m44);

    // world to camera: move pCenter to the origin, scale, move it back
    LLMath_IdentifyMat(mTmp);
    mTmp[3][0] = pCenter[0];
    mTmp[3][1] = pCenter[1];
    mTmp[3][2] = pCenter[2];
    LLMath_mat44fltMultiplyList(mTmp, pLens->m44, mB, 4);
    LLMath_IdentifyMat(mTmp);
    mTmp[0][0] = pScale[0];
    mTmp[1][1] = pScale[1];
    mTmp[2][2] = pScale[2];
    LLMath_mat44fltMultiplyList(mTmp, mB, mA, 4);
    LLMath_IdentifyMat(mTmp);
    mTmp[3][0] = -pCenter[0];
    mTmp[3][1] = -pCenter[1];
    mTmp[3][2] = -pCenter[2];
    LLMath_mat44fltMultiplyList(mTmp, mA, pLens->m44, 4);

    // camera to world: the same with the inverse scale
    LLMath_IdentifyMat(mTmp);
    mTmp[3][0] = pCenter[0];
    mTmp[3][1] = pCenter[1];
    mTmp[3][2] = pCenter[2];
    LLMath_IdentifyMat(mB);
    mB[0][0] = pLens->m84[0][0];
    mB[1][1] = pLens->m84[0][1];
    mB[2][2] = pLens->m84[0][2];
    LLMath_mat44fltMultiplyList(mB, mTmp, mA, 4);
    LLMath_IdentifyMat(mTmp);
    mTmp[3][0] = -pCenter[0];
    mTmp[3][1] = -pCenter[1];
    mTmp[3][2] = -pCenter[2];
    LLMath_mat44fltMultiplyList(mTmp, mA, mB, 4);
    LLMath_mat44fltMultiplyList(pLens->m4, mB, mA, 4);
    LLMath_CopyMat44(mA, pLens->m4);
    LLMath_mat44fltMultiplyList(pLens->m4, pLens->m44, mB, 4);   // the result is never used
}

// A new lens's settings: a perspective camera, near clip 0.1 and far clip 4096, a 60-degree field
// of view, the identity matrix, a 20 x 20 flat view.
void CA_vSetDefaultCamera(CamLens* pLens) {
    CA_vSetCameraProjectionMode(pLens, 0);
    CA_vSetCameraNearFarZ(pLens, 0.1f, 4096.0f);
    CA_vSetCameraFieldOfView(pLens, DEG(60.0f));
    CA_vSetCameraViewToWorldMatrix(pLens, NULL);
    CA_vSetCameraFlatSize(pLens, 20.0f, 20.0f);
}

void CA_vSetCameraFlatSize(CamLens* pLens, f32 fB4, f32 fB8) {
    pLens->fFlatWidth = fB4;
    pLens->fFlatHeight = fB8;
}

// Sets the lens's camera-to-world matrix (pMtx, or the identity when NULL) and its inverse.
void CA_vSetCameraViewToWorldMatrix(CamLens* pLens, f32 (*pMtx)[4]) {
    if (pMtx == NULL) {
        LLMath_IdentifyMat(pLens->m4);
        LLMath_IdentifyMat(pLens->m44);
    } else {
        LLMath_CopyMat44(pMtx, pLens->m4);
        LLMath_InvertNormalized(pMtx, pLens->m44);
    }
    CA_vSetDefaultScalingVectors(pLens);
}

void CA_vSetCameraNearFarZ(CamLens* pLens, f32 fA8, f32 fAC) {
    CA_vSetCameraNearZ(pLens, fA8);
    Camera_SetLensFarClip((u8*)pLens, fAC);
}

void CA_vSetCameraNearZ(CamLens* pLens, f32 fA8) {
    pLens->fA8 = fA8;
}

// 0: a perspective lens; anything else: flat (fFlatWidth x fFlatHeight).
void CA_vSetCameraProjectionMode(CamLens* pLens, s32 nType) {
    pLens->nType = nType;
}

// a - b into out (three floats)
#ifdef __MWERKS__
asm void Camera_Subtract3(register f32* pA, register f32* pB, register f32* pOut) {
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
void Camera_Subtract3(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// 1 / a into out, each of the three floats (ps_res: the hardware's reciprocal estimate)
#ifdef __MWERKS__
asm void Camera_Invert3(register f32* pA, register f32* pOut) {
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
// port: untested, the plain-C version for compilers without paired singles; ps_res is an estimate
//       good to about 1/4096, this is the exact reciprocal.
void Camera_Invert3(f32* pA, f32* pOut) {
    pOut[0] = 1.0f / pA[0];
    pOut[1] = 1.0f / pA[1];
    pOut[2] = 1.0f / pA[2];
}
#endif
