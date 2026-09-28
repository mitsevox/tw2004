// UMemPool.c (EA's name, from the assert in UMemPool_Create; also in EA's 2002 source tree).
// Despite the name, most of this unit is 4x4 matrix code: it reads as EA's UMatFlt.c and UMath.c
// linked just before UMemPool.c (TW06 and TW07 keep them as neighbouring legacy/lib files, in that
// order; TW07's UMatFlt.c has mat44flt_EulerAngles, mat44flt_ExtractEulerAngles, mat44flt_gaussj
// and mat44flt_Invert in the order they have here), with no assert or data block to split on.
// In address order:
// - matrices (row vectors, translation in row 3): copies, Euler angles to and from a matrix,
//   transposes, inverses and the projection matrices the render context builds;
// - 4-float vector helpers (Vec_Copy, Vec_Swap, paired-single negate, scale, multiply and
//   add-scaled), atan2f, fabsf, fabs and logf;
// - the log2 lookup table (1024 entries) and its init and close (possibly UMath.c's module
//   functions: TW07's UMath.c has four near-empty MF_v* init/close functions);
// - the pools of fixed-size nodes carved from one allocation (the file streamer keeps its object
//   nodes in one): UMemPool_Create, DeleteMemPool, AllocPoolMem, ReturnPoolMem.

#include "engine.h"

f32* gLog2Table;   // log2(1 + i / 1024) for i = 0..1023: the log2 of a float's mantissa in [1, 2)

int  mat44flt_gaussj(f32 (*pA)[4], f32 (*pB)[4]);
void Vec_Swap(f32* pA, f32* pB);
void Mtx_Identity(f32 (*pDst)[4]);
void fn_8000AE0C(f32* pSrc, f32* pDst);
void fn_8000AE9C(void);
void fn_800BADB4(f32 (*pMtx)[4], f32* pIn, f32* pOut);     // a vector through a matrix

// Copies a 4x4 matrix, one row at a time.
void Mtx_Copy(f32 (*pSrc)[4], f32 (*pDst)[4]) {
    Vec_Copy(pSrc[0], pDst[0]);
    Vec_Copy(pSrc[1], pDst[1]);
    Vec_Copy(pSrc[2], pDst[2]);
    Vec_Copy(pSrc[3], pDst[3]);
}

// Copies rows 0-2 of a 4x4 matrix (the rotation, each row with its fourth float); row 3, the
// translation, is left as it is.
void Mtx_CopyRotation(f32 (*pSrc)[4], f32 (*pDst)[4]) {
    Vec_Copy(pSrc[0], pDst[0]);
    Vec_Copy(pSrc[1], pDst[1]);
    Vec_Copy(pSrc[2], pDst[2]);
}

// A rotation matrix from yaw (about y), pitch (about x) and roll (about z), in radians, into rows
// 0-2 of pMtx (their fourth float set to 0; row 3 is left as it is). An angle of exactly 0 skips
// its sin and cos.
void mat44flt_EulerAngles(f32 (*pMtx)[4], f32 fYaw, f32 fPitch, f32 fRoll) {
    f32 fSinYaw;
    f32 fCosYaw;
    f32 fSinPitch;
    f32 fCosPitch;
    f32 fSinRoll;
    f32 fCosRoll;
    f32 fSinRollSinPitch;
    f32 fCosRollSinPitch;

    if (fRoll == 0.0f) {
        if (fPitch == 0.0f) {
            pMtx[1][1] = 1.0f;
            pMtx[2][1] = 0.0f;
            pMtx[1][2] = 0.0f;
            pMtx[1][0] = 0.0f;
            pMtx[0][1] = 0.0f;
            if (fYaw != 0.0f) {
                fSinYaw = Math_Sin(fYaw);
                fCosYaw = Math_Cos(fYaw);
                pMtx[2][2] = fCosYaw;
                pMtx[0][0] = fCosYaw;
                pMtx[2][0] = fSinYaw;
                pMtx[0][2] = -fSinYaw;
            } else {
                pMtx[2][2] = 1.0f;
                pMtx[0][0] = 1.0f;
                pMtx[2][0] = 0.0f;
                pMtx[0][2] = 0.0f;
            }
        } else {
            fSinPitch = Math_Sin(fPitch);
            fCosPitch = Math_Cos(fPitch);
            if (fYaw == 0.0f) {
                pMtx[0][0] = 1.0f;
                pMtx[2][0] = 0.0f;
                pMtx[1][0] = 0.0f;
                pMtx[0][2] = 0.0f;
                pMtx[0][1] = 0.0f;
                pMtx[2][2] = fCosPitch;
                pMtx[1][1] = fCosPitch;
                pMtx[1][2] = fSinPitch;
                pMtx[2][1] = -fSinPitch;
            } else {
                fSinYaw = Math_Sin(fYaw);
                fCosYaw = Math_Cos(fYaw);
                pMtx[0][0] = fCosYaw;
                pMtx[0][1] = 0.0f;
                pMtx[0][2] = -fSinYaw;
                pMtx[1][1] = fCosPitch;
                pMtx[2][1] = -fSinPitch;
                pMtx[1][0] = fSinPitch * fSinYaw;
                pMtx[1][2] = fSinPitch * fCosYaw;
                pMtx[2][0] = fCosPitch * fSinYaw;
                pMtx[2][2] = fCosPitch * fCosYaw;
            }
        }
    } else {
        fSinRoll = Math_Sin(fRoll);
        fCosRoll = Math_Cos(fRoll);
        if (fPitch == 0.0f) {
            if (fYaw == 0.0f) {
                pMtx[2][2] = 1.0f;
                pMtx[2][1] = 0.0f;
                pMtx[2][0] = 0.0f;
                pMtx[1][2] = 0.0f;
                pMtx[0][2] = 0.0f;
                pMtx[0][0] = fCosRoll;
                pMtx[1][1] = fCosRoll;
                pMtx[0][1] = fSinRoll;
                pMtx[1][0] = -fSinRoll;
            } else {
                fSinYaw = Math_Sin(fYaw);
                fCosYaw = Math_Cos(fYaw);
                pMtx[2][0] = fSinYaw;
                pMtx[2][1] = 0.0f;
                pMtx[2][2] = fCosYaw;
                pMtx[0][1] = fSinRoll;
                pMtx[1][1] = fCosRoll;
                pMtx[0][0] = fCosRoll * fCosYaw;
                pMtx[0][2] = -(fCosRoll * fSinYaw);
                pMtx[1][0] = -(fSinRoll * fCosYaw);
                pMtx[1][2] = fSinRoll * fSinYaw;
            }
        } else {
            fSinPitch = Math_Sin(fPitch);
            fCosPitch = Math_Cos(fPitch);
            if (fYaw == 0.0f) {
                pMtx[2][0] = 0.0f;
                pMtx[2][1] = -fSinPitch;
                pMtx[2][2] = fCosPitch;
                pMtx[0][0] = fCosRoll;
                pMtx[1][0] = -fSinRoll;
                pMtx[0][1] = fSinRoll * fCosPitch;
                pMtx[0][2] = fSinRoll * fSinPitch;
                pMtx[1][1] = fCosRoll * fCosPitch;
                pMtx[1][2] = fCosRoll * fSinPitch;
            } else {
                fSinYaw = Math_Sin(fYaw);
                fCosYaw = Math_Cos(fYaw);
                pMtx[2][1] = -fSinPitch;
                fSinRollSinPitch = fSinRoll * fSinPitch;
                fCosRollSinPitch = fSinPitch * fCosRoll;
                pMtx[0][0] = fCosRoll * fCosYaw + fSinRollSinPitch * fSinYaw;
                pMtx[0][1] = fSinRoll * fCosPitch;
                pMtx[0][2] = fSinRollSinPitch * fCosYaw - fCosRoll * fSinYaw;
                pMtx[1][0] = fCosRollSinPitch * fSinYaw - fSinRoll * fCosYaw;
                pMtx[1][1] = fCosRoll * fCosPitch;
                pMtx[1][2] = fSinRoll * fSinYaw + fCosRollSinPitch * fCosYaw;
                pMtx[2][0] = fCosPitch * fSinYaw;
                pMtx[2][2] = fCosPitch * fCosYaw;
            }
        }
    }
    pMtx[2][3] = 0.0f;
    pMtx[1][3] = 0.0f;
    pMtx[0][3] = 0.0f;
}

// The yaw, pitch and roll (radians) of a rotation matrix, the reverse of mat44flt_EulerAngles: yaw
// is atan2(m[2][0], m[2][2]), pitch is -asin(m[2][1]) and roll is atan2(m[0][1], m[1][1]), with a
// quarter turn chosen by sign where an atan2 would divide by zero.
void mat44flt_ExtractEulerAngles(f32 (*pMtx)[4], f32* pYaw, f32* pPitch, f32* pRoll) {
    f32 fYaw;
    f32 fPitch;
    f32 fRoll;

    if (pMtx[2][2] != 0.0f) {
        fYaw = atan2f(pMtx[2][0], pMtx[2][2]);
        fPitch = -Math_Asin(pMtx[2][1]);
        if (pMtx[1][1] != 0.0f) {
            fRoll = atan2f(pMtx[0][1], pMtx[1][1]);
        } else if (pMtx[0][1] != 0.0f) {
            if (pMtx[0][1] * Math_Cos(fPitch) > 0.0f) {
                fRoll = PI / 2.0f;
            } else {
                fRoll = -PI / 2.0f;
            }
        }
    } else if (pMtx[2][0] == 0.0f) {
        if (pMtx[2][1] > 0.0f) {
            fPitch = -PI / 2.0f;
        } else {
            fPitch = PI / 2.0f;
        }
        fRoll = 0.0f;
        if (pMtx[0][0] == 0.0f) {
            if (pMtx[0][2] > 0.0f) {
                fYaw = -PI / 2.0f;
            } else {
                fYaw = PI / 2.0f;
            }
        } else {
            // both arguments are m[0][0], as in the original
            fYaw = -atan2f(pMtx[0][0], pMtx[0][0]);
        }
    } else {
        fPitch = -Math_Asin(pMtx[2][1]);
        if (pMtx[2][0] * Math_Cos(fPitch) > 0.0f) {
            fYaw = PI / 2.0f;
        } else {
            fYaw = -PI / 2.0f;
        }
        if (pMtx[1][1] != 0.0f) {
            fRoll = atan2f(pMtx[0][1], pMtx[1][1]);
        } else if (pMtx[0][1] * Math_Cos(fPitch) > 0.0f) {
            fRoll = PI / 2.0f;
        } else {
            fRoll = -PI / 2.0f;
        }
    }
    *pYaw = fYaw;
    // EA bug: fRoll is never set when m[2][2] != 0 and m[0][1] and m[1][1] are both 0
    *pRoll = fRoll;
    *pPitch = fPitch;
}

// Transposes the 3x3 part of a 4x4 matrix (pSrc and pDst may be the same matrix).
void Mtx_Transpose3x3(f32 (*pSrc)[4], f32 (*pDst)[4]) {
    f32 f;

    pDst[0][0] = pSrc[0][0];
    pDst[1][1] = pSrc[1][1];
    pDst[2][2] = pSrc[2][2];
    f = pSrc[1][0];
    pDst[1][0] = pSrc[0][1];
    pDst[0][1] = f;
    f = pSrc[2][0];
    pDst[2][0] = pSrc[0][2];
    pDst[0][2] = f;
    f = pSrc[1][2];
    pDst[1][2] = pSrc[2][1];
    pDst[2][1] = f;
}

// Transposes a 4x4 matrix (pSrc and pDst may be the same matrix).
void Mtx_Transpose4x4(f32 (*pSrc)[4], f32 (*pDst)[4]) {
    f32 f;

    pDst[0][0] = pSrc[0][0];
    pDst[1][1] = pSrc[1][1];
    pDst[2][2] = pSrc[2][2];
    pDst[3][3] = pSrc[3][3];
    f = pSrc[1][0];
    pDst[1][0] = pSrc[0][1];
    pDst[0][1] = f;
    f = pSrc[2][0];
    pDst[2][0] = pSrc[0][2];
    pDst[0][2] = f;
    f = pSrc[3][0];
    pDst[3][0] = pSrc[0][3];
    pDst[0][3] = f;
    f = pSrc[1][2];
    pDst[1][2] = pSrc[2][1];
    pDst[2][1] = f;
    f = pSrc[1][3];
    pDst[1][3] = pSrc[3][1];
    pDst[3][1] = f;
    f = pSrc[2][3];
    pDst[2][3] = pSrc[3][2];
    pDst[3][2] = f;
}

// Inverts a rotation-plus-translation matrix (rotation in rows 0-2, translation in row 3, no
// scale): the rotation is transposed and the new translation is minus the old one turned by it.
// pSrc and pDst may be the same matrix.
void Mtx_InvertRigid(f32 (*pSrc)[4], f32 (*pDst)[4]) {
    f32 aTurned[4];
    f32 aPos[4];

    Vec_Copy(pSrc[3], aPos);
    pDst[0][3] = 0.0f;
    pDst[1][3] = 0.0f;
    pDst[2][3] = 0.0f;
    pDst[3][3] = 1.0f;
    Mtx_Transpose3x3(pSrc, pDst);
    fn_800BADB4(pDst, aPos, aTurned);
    fn_8000AE0C(aTurned, pDst[3]);
}

// Gauss-Jordan elimination with full pivoting: pA is replaced by its inverse, and pB goes
// through the same row steps (so an identity pB also comes out as the inverse). Returns 0, or
// -1 / -2 when pA is singular.
int mat44flt_gaussj(f32 (*pA)[4], f32 (*pB)[4]) {
    int anCol[4];
    int anRow[4];
    int anPivot[4];
    int i;
    int j;
    int k;
    int nCol;
    int nRow;
    f32 fBig;
    f32 fPivInv;
    f32 fDum;

    for (j = 0; j < 4; j++) {
        anPivot[j] = 0;
    }
    for (i = 0; i < 4; i++) {
        fBig = 0.0f;
        for (j = 0; j < 4; j++) {
            if (anPivot[j] != 1) {
                for (k = 0; k < 4; k++) {
                    if (anPivot[k] == 0) {
                        if (fabsf(pA[j][k]) >= fBig) {
                            fBig = fabsf(pA[j][k]);
                            nRow = j;
                            nCol = k;
                        }
                    } else if (anPivot[k] > 1) {
                        return -1;
                    }
                }
            }
        }
        anPivot[nCol]++;
        if (nRow != nCol) {
            Vec_Swap(pA[nRow], pA[nCol]);
            Vec_Swap(pB[nRow], pB[nCol]);
        }
        anRow[i] = nRow;
        anCol[i] = nCol;
        if (pA[nCol][nCol] == 0.0f) {
            return -2;
        }
        fPivInv = 1.0f / pA[nCol][nCol];
        pA[nCol][nCol] = 1.0f;
        Vec_Scale(fPivInv, pA[nCol], pA[nCol]);
        Vec_Scale(fPivInv, pB[nCol], pB[nCol]);
        for (j = 0; j < 4; j++) {
            if (j != nCol) {
                fDum = -pA[j][nCol];
                pA[j][nCol] = 0.0f;
                fn_8000AE6C(pA[j], pA[nCol], fDum, pA[j]);
                fn_8000AE6C(pB[j], pB[nCol], fDum, pB[j]);
            }
        }
    }
    // undo the column swaps, last first
    for (j = 3; j >= 0; j--) {
        if (anRow[j] != anCol[j]) {
            for (k = 0; k < 4; k++) {
                fDum = pA[k][anRow[j]];
                pA[k][anRow[j]] = pA[k][anCol[j]];
                pA[k][anCol[j]] = fDum;
            }
        }
    }
    return 0;
}

// The inverse of a 4x4 matrix, into pDst (Gauss-Jordan through mat44flt_gaussj). A singular matrix
// is not reported: pDst is left part-way through.
void mat44flt_Invert(f32 (*pSrc)[4], f32 (*pDst)[4]) {
    f32 aIdentity[4][4];

    Mtx_Copy(pSrc, pDst);
    Mtx_Identity(aIdentity);
    mat44flt_gaussj(pDst, aIdentity);
}

// The identity with x scaled by 2 / fWidth and y by 2 / fHeight, so a view fWidth by fHeight
// centred on 0 spans -1..1 (z is left alone). RC_vUpdateRenderCtxScreenMatricesAndInfo uses it for
// a lens whose type is not 0.
void Mtx_OrthoScale(f32 (*pDst)[4], f32 fWidth, f32 fHeight) {
    Mtx_Identity(pDst);
    pDst[0][0] = 2.0f / fWidth;
    pDst[1][1] = 2.0f / fHeight;
}

// A perspective projection for row vectors (the transpose of GX's layout): x scaled by fScale times
// fScaleX, y by fScale times fScaleY, w = -z, and depth from fNear to fFar mapped to -1..0 as GX
// expects.
void Mtx_Perspective(f32 (*pDst)[4], f32 fScale, f32 fScaleX, f32 fScaleY, f32 fNear, f32 fFar) {
    pDst[0][0] = fScale * fScaleX;
    pDst[1][0] = 0.0f;
    pDst[2][0] = 0.0f;
    pDst[3][0] = 0.0f;
    pDst[0][1] = 0.0f;
    pDst[1][1] = fScale * fScaleY;
    pDst[2][1] = 0.0f;
    pDst[3][1] = 0.0f;
    pDst[0][2] = 0.0f;
    pDst[1][2] = 0.0f;
    pDst[2][2] = -fNear * (1.0f / (fFar - fNear));
    pDst[3][2] = (1.0f / (fFar - fNear)) * -(fFar * fNear);
    pDst[0][3] = 0.0f;
    pDst[1][3] = 0.0f;
    pDst[2][3] = -1.0f;
    pDst[3][3] = 0.0f;
}

// Like Mtx_Perspective (row vectors, w = -z) with x scaled by fScaleX and y by fScaleY, but its
// depth is Mtx_Perspective's divided by fNear: fFar maps to 0 and fNear to -1 / fNear (the same
// only when fNear is 1). RC_vUpdateRenderCtxScreenMatricesAndInfo uses it, with Mtx_OrthoScale, for
// a lens whose type is not 0.
void Mtx_PerspectiveDepthOverNear(f32 (*pDst)[4], f32 fScaleX, f32 fScaleY, f32 fNear, f32 fFar) {
    Mtx_Identity(pDst);
    pDst[0][0] = fScaleX;
    pDst[1][1] = fScaleY;
    pDst[2][2] = -1.0f / (fFar - fNear);
    pDst[3][2] = -(1.0f + fNear / (fFar - fNear));
    pDst[2][3] = -1.0f;
    pDst[3][3] = 0.0f;
}

// Copies a 4-float vector.
void Vec_Copy(const f32* pSrc, f32* pDst) {
    pDst[0] = pSrc[0];
    pDst[1] = pSrc[1];
    pDst[2] = pSrc[2];
    pDst[3] = pSrc[3];
}

// Swaps two 4-float vectors.
void Vec_Swap(f32* pA, f32* pB) {
    f32 f;

    f = pA[0];
    pA[0] = pB[0];
    pB[0] = f;
    f = pA[1];
    pA[1] = pB[1];
    pB[1] = f;
    f = pA[2];
    pA[2] = pB[2];
    pB[2] = f;
    f = pA[3];
    pA[3] = pB[3];
    pB[3] = f;
}

// The float atan2: the angle of the point (x, y) in radians, -pi..pi, through the double atan2().
f32 atan2f(f32 y, f32 x) {
    return atan2(y, x);
}

// The float fabs: |x|, through the double fabs().
f32 fabsf(f32 x) {
    return fabs(x);
}

// Sets a 4x4 matrix to the identity.
void Mtx_Identity(f32 (*pDst)[4]) {
    pDst[0][0] = 1.0f;
    pDst[0][1] = 0.0f;
    pDst[0][2] = 0.0f;
    pDst[0][3] = 0.0f;
    pDst[1][0] = 0.0f;
    pDst[1][1] = 1.0f;
    pDst[1][2] = 0.0f;
    pDst[1][3] = 0.0f;
    pDst[2][0] = 0.0f;
    pDst[2][1] = 0.0f;
    pDst[2][2] = 1.0f;
    pDst[2][3] = 0.0f;
    pDst[3][0] = 0.0f;
    pDst[3][1] = 0.0f;
    pDst[3][2] = 0.0f;
    pDst[3][3] = 1.0f;
}

// Negates a 3-float vector into pDst.
#ifdef __MWERKS__
asm void fn_8000AE0C(register f32* pSrc, register f32* pDst) {
    nofralloc
    psq_l  f0, 0(pSrc), 0, 0
    psq_l  f1, 8(pSrc), 1, 0
    ps_neg f0, f0
    ps_neg f1, f1
    psq_st f0, 0(pDst), 0, 0
    psq_st f1, 8(pDst), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_8000AE0C(f32* pSrc, f32* pDst) {
    pDst[0] = -pSrc[0];
    pDst[1] = -pSrc[1];
    pDst[2] = -pSrc[2];
}
#endif

// Scales a 4-float vector into pOut.
#ifdef __MWERKS__
asm void Vec_Scale(register f32 fScale, register f32* pIn, register f32* pOut) {
    nofralloc
    fmr      f2, fScale
    psq_l    f0, 0(pIn), 0, 0
    psq_l    f1, 8(pIn), 0, 0
    ps_muls0 f0, f0, f2
    ps_muls0 f1, f1, f2
    psq_st   f0, 0(pOut), 0, 0
    psq_st   f1, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void Vec_Scale(f32 fScale, f32* pIn, f32* pOut) {
    pOut[0] = pIn[0] * fScale;
    pOut[1] = pIn[1] * fScale;
    pOut[2] = pIn[2] * fScale;
    pOut[3] = pIn[3] * fScale;
}
#endif

// Multiplies two 4-float vectors component by component into pOut.
#ifdef __MWERKS__
asm void fn_8000AE48(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 0, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 0, 0
    ps_mul f0, f0, f2
    ps_mul f1, f1, f3
    psq_st f0, 0(pOut), 0, 0
    psq_st f1, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_8000AE48(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] * pB[0];
    pOut[1] = pA[1] * pB[1];
    pOut[2] = pA[2] * pB[2];
    pOut[3] = pA[3] * pB[3];
}
#endif

// pA + pB * fScale into pOut (four floats).
#ifdef __MWERKS__
asm void fn_8000AE6C(register f32* pA, register f32* pB, register f32 fScale, register f32* pOut) {
    nofralloc
    fmr       f4, fScale
    psq_l     f0, 0(pA), 0, 0
    psq_l     f1, 8(pA), 0, 0
    psq_l     f2, 0(pB), 0, 0
    psq_l     f3, 8(pB), 0, 0
    ps_madds0 f2, f2, f4, f0
    ps_madds0 f3, f3, f4, f1
    psq_st    f2, 0(pOut), 0, 0
    psq_st    f3, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_8000AE6C(f32* pA, f32* pB, f32 fScale, f32* pOut) {
    pOut[0] = pB[0] * fScale + pA[0];
    pOut[1] = pB[1] * fScale + pA[1];
    pOut[2] = pB[2] * fScale + pA[2];
    pOut[3] = pB[3] * fScale + pA[3];
}
#endif

// 0x8000AE94: the absolute value of a double (fabsf, which rounds its result to a float, is
// the float version).
double fabs(double x) {
    return __fabs(x);
}

// Fills the log2 table: entry i is log2(1 + i / 1024).
void fn_8000AE9C(void) {
    u32 uMantissa;
    f32* pEntry;
    u32 i;
    f32 f;

    pEntry = gLog2Table;
    i = 0;
    uMantissa = 0;
    do {
        // port: builds the float from its bits through a u32 pointer (see Misc_RandFuncf).
        *(u32*)&f = uMantissa | 0x3F800000;
        *pEntry = 1.442695f * logf(f);
        i++;
        uMantissa += 0x2000;
        pEntry++;
    } while (i < 0x400);
}

void fn_8000AF1C(void) {
}

void fn_8000AF20(void) {
    fn_8000AF1C();
    gLog2Table = fn_800951A0(0x400 * sizeof(f32), 16, 1);
    fn_8000AE9C();
}

void fn_8000AF58(void) {
    fn_8009527C(gLog2Table);
}

f32 logf(f32 x) {
    return log(x);
}

// Makes a pool of nNodes nodes of uNodeSize bytes each, every node aligned to uAlign (a power of
// two). The nodes are filled with 0xDD and chained into the free list, the last one first.
UMemPool* UMemPool_Create(int nNodes, u32 uNodeSize, u32 uFlags, u32 uAlign) {
    UMemPool* pPool;
    u32 uSize;
    u8* pNode;
    UMemPoolNode* pPrev;
    u32 uTotal;

    uSize = uAlign + uNodeSize;
    uSize = (uSize - 1) & ~(uAlign - 1);
    uTotal = uAlign + nNodes * uSize;
    pPool = StaticMem_Alloc(uTotal, uFlags, uAlign, "UMemPool.c", 82);
    if (pPool != NULL) {
        pPool->nNodes = nNodes;
        pPool->nFree = nNodes;
        pPool->uNodeSize = uSize;
        // fake match: added as integers; `(u8*)pPool + uTotal` puts pPool first in the add
        pPool->pEnd = (u8*)(uTotal + (uptr)pPool);
        pNode = (u8*)pPool + uAlign;
        pPrev = NULL;
        while (nNodes-- > 0) {
            Mem_set(pNode, 0xDD, uSize);
            ((UMemPoolNode*)pNode)->pNext = pPrev;
            pPrev = (UMemPoolNode*)pNode;
            pNode += uSize;
        }
        pPool->pFree = pPrev;
    }
    return pPool;
}

void DeleteMemPool(UMemPool* pPool) {
    StaticMem_Free(pPool);
}

// Takes a node off the free list and fills it with 0xBB; NULL when the pool is empty.
void* AllocPoolMem(UMemPool* pPool) {
    UMemPoolNode* pNode;

    pNode = pPool->pFree;
    if (pNode != NULL) {
        pPool->pFree = pNode->pNext;
        pPool->nFree--;
        Mem_set(pNode, 0xBB, pPool->uNodeSize);
    }
    return pNode;
}

// Fills a node with 0x99 and puts it back on the free list.
void ReturnPoolMem(UMemPool* pPool, void* p) {
    UMemPoolNode* pNode;

    pNode = p;
    Mem_set(pNode, 0x99, pPool->uNodeSize);
    pNode->pNext = pPool->pFree;
    pPool->pFree = pNode;
    pPool->nFree++;
}
