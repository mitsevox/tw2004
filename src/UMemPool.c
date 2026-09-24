// UMemPool.c (EA's name, from its asserts; also in EA's 2002 source tree): pools of fixed-size
// nodes carved from one allocation (the file streamer keeps its object nodes in one), and before
// them a block of math helpers: 4x4 matrices, paired-single vector operations, atan2f, fabsf and
// the natural logarithm with its log2 lookup table.

#include "engine.h"

f32* lbl_80281BD8;                      // the log2 table: 1024 entries over the mantissa of [1, 2)

int  fn_8000A818(f32 (*pA)[4], f32 (*pB)[4]);
void fn_8000AD34(f32* pA, f32* pB);
void fn_8000ADC0(f32 (*pDst)[4]);
void fn_8000AE0C(f32* pSrc, f32* pDst);
void fn_8000AE9C(void);
void fn_800BADB4(f32 (*pMtx)[4], f32* pIn, f32* pOut);     // a vector through a matrix

// Copies a 4x4 matrix, one row at a time.
void fn_8000A0E8(f32 (*pSrc)[4], f32 (*pDst)[4]) {
    Vec_Copy(pSrc[0], pDst[0]);
    Vec_Copy(pSrc[1], pDst[1]);
    Vec_Copy(pSrc[2], pDst[2]);
    Vec_Copy(pSrc[3], pDst[3]);
}

// Copies the first three rows of a 4x4 matrix.
void fn_8000A144(f32 (*pSrc)[4], f32 (*pDst)[4]) {
    Vec_Copy(pSrc[0], pDst[0]);
    Vec_Copy(pSrc[1], pDst[1]);
    Vec_Copy(pSrc[2], pDst[2]);
}

// A rotation matrix from three angles (the 3x3 part and a zero fourth column; row 3 is left as
// it is). An angle of exactly 0 skips its sin and cos.
void fn_8000A194(f32 (*pMtx)[4], f32 fA, f32 fB, f32 fC) {
    f32 fSinA;
    f32 fCosA;
    f32 fSinB;
    f32 fCosB;
    f32 fSinC;
    f32 fCosC;
    f32 fSinCSinB;
    f32 fSinBCosC;

    if (fC == 0.0f) {
        if (fB == 0.0f) {
            pMtx[1][1] = 1.0f;
            pMtx[2][1] = 0.0f;
            pMtx[1][2] = 0.0f;
            pMtx[1][0] = 0.0f;
            pMtx[0][1] = 0.0f;
            if (fA != 0.0f) {
                fSinA = fn_800095F0(fA);
                fCosA = fn_80009638(fA);
                pMtx[2][2] = fCosA;
                pMtx[0][0] = fCosA;
                pMtx[2][0] = fSinA;
                pMtx[0][2] = -fSinA;
            } else {
                pMtx[2][2] = 1.0f;
                pMtx[0][0] = 1.0f;
                pMtx[2][0] = 0.0f;
                pMtx[0][2] = 0.0f;
            }
        } else {
            fSinB = fn_800095F0(fB);
            fCosB = fn_80009638(fB);
            if (fA == 0.0f) {
                pMtx[0][0] = 1.0f;
                pMtx[2][0] = 0.0f;
                pMtx[1][0] = 0.0f;
                pMtx[0][2] = 0.0f;
                pMtx[0][1] = 0.0f;
                pMtx[2][2] = fCosB;
                pMtx[1][1] = fCosB;
                pMtx[1][2] = fSinB;
                pMtx[2][1] = -fSinB;
            } else {
                fSinA = fn_800095F0(fA);
                fCosA = fn_80009638(fA);
                pMtx[0][0] = fCosA;
                pMtx[0][1] = 0.0f;
                pMtx[0][2] = -fSinA;
                pMtx[1][1] = fCosB;
                pMtx[2][1] = -fSinB;
                pMtx[1][0] = fSinB * fSinA;
                pMtx[1][2] = fSinB * fCosA;
                pMtx[2][0] = fCosB * fSinA;
                pMtx[2][2] = fCosB * fCosA;
            }
        }
    } else {
        fSinC = fn_800095F0(fC);
        fCosC = fn_80009638(fC);
        if (fB == 0.0f) {
            if (fA == 0.0f) {
                pMtx[2][2] = 1.0f;
                pMtx[2][1] = 0.0f;
                pMtx[2][0] = 0.0f;
                pMtx[1][2] = 0.0f;
                pMtx[0][2] = 0.0f;
                pMtx[0][0] = fCosC;
                pMtx[1][1] = fCosC;
                pMtx[0][1] = fSinC;
                pMtx[1][0] = -fSinC;
            } else {
                fSinA = fn_800095F0(fA);
                fCosA = fn_80009638(fA);
                pMtx[2][0] = fSinA;
                pMtx[2][1] = 0.0f;
                pMtx[2][2] = fCosA;
                pMtx[0][1] = fSinC;
                pMtx[1][1] = fCosC;
                pMtx[0][0] = fCosC * fCosA;
                pMtx[0][2] = -(fCosC * fSinA);
                pMtx[1][0] = -(fSinC * fCosA);
                pMtx[1][2] = fSinC * fSinA;
            }
        } else {
            fSinB = fn_800095F0(fB);
            fCosB = fn_80009638(fB);
            if (fA == 0.0f) {
                pMtx[2][0] = 0.0f;
                pMtx[2][1] = -fSinB;
                pMtx[2][2] = fCosB;
                pMtx[0][0] = fCosC;
                pMtx[1][0] = -fSinC;
                pMtx[0][1] = fSinC * fCosB;
                pMtx[0][2] = fSinC * fSinB;
                pMtx[1][1] = fCosC * fCosB;
                pMtx[1][2] = fCosC * fSinB;
            } else {
                fSinA = fn_800095F0(fA);
                fCosA = fn_80009638(fA);
                pMtx[2][1] = -fSinB;
                fSinCSinB = fSinC * fSinB;
                fSinBCosC = fSinB * fCosC;
                pMtx[0][0] = fCosC * fCosA + fSinCSinB * fSinA;
                pMtx[0][1] = fSinC * fCosB;
                pMtx[0][2] = fSinCSinB * fCosA - fCosC * fSinA;
                pMtx[1][0] = fSinBCosC * fSinA - fSinC * fCosA;
                pMtx[1][1] = fCosC * fCosB;
                pMtx[1][2] = fSinC * fSinA + fSinBCosC * fCosA;
                pMtx[2][0] = fCosB * fSinA;
                pMtx[2][2] = fCosB * fCosA;
            }
        }
    }
    pMtx[2][3] = 0.0f;
    pMtx[1][3] = 0.0f;
    pMtx[0][3] = 0.0f;
}

// The three angles of a rotation matrix (the reverse of fn_8000A194): *pA = atan2(m[2][0],
// m[2][2]), *pB = -asin(m[2][1]), *pC = atan2(m[0][1], m[1][1]), with a quarter turn chosen by
// sign where an atan2 would divide by zero.
void fn_8000A4E0(f32 (*pMtx)[4], f32* pA, f32* pB, f32* pC) {
    f32 fA;
    f32 fB;
    f32 fC;

    if (pMtx[2][2] != 0.0f) {
        fA = atan2f(pMtx[2][0], pMtx[2][2]);
        fB = -fn_8000965C(pMtx[2][1]);
        if (pMtx[1][1] != 0.0f) {
            fC = atan2f(pMtx[0][1], pMtx[1][1]);
        } else if (pMtx[0][1] != 0.0f) {
            if (pMtx[0][1] * fn_80009638(fB) > 0.0f) {
                fC = PI / 2.0f;
            } else {
                fC = -PI / 2.0f;
            }
        }
    } else if (pMtx[2][0] == 0.0f) {
        if (pMtx[2][1] > 0.0f) {
            fB = -PI / 2.0f;
        } else {
            fB = PI / 2.0f;
        }
        fC = 0.0f;
        if (pMtx[0][0] == 0.0f) {
            if (pMtx[0][2] > 0.0f) {
                fA = -PI / 2.0f;
            } else {
                fA = PI / 2.0f;
            }
        } else {
            // both arguments are m[0][0], as in the original
            fA = -atan2f(pMtx[0][0], pMtx[0][0]);
        }
    } else {
        fB = -fn_8000965C(pMtx[2][1]);
        if (pMtx[2][0] * fn_80009638(fB) > 0.0f) {
            fA = PI / 2.0f;
        } else {
            fA = -PI / 2.0f;
        }
        if (pMtx[1][1] != 0.0f) {
            fC = atan2f(pMtx[0][1], pMtx[1][1]);
        } else if (pMtx[0][1] * fn_80009638(fB) > 0.0f) {
            fC = PI / 2.0f;
        } else {
            fC = -PI / 2.0f;
        }
    }
    *pA = fA;
    // EA bug: fC is never set when m[2][2] != 0 and m[0][1] and m[1][1] are both 0
    *pC = fC;
    *pB = fB;
}

// Transposes the 3x3 part of a 4x4 matrix (pSrc and pDst may be the same matrix).
void fn_8000A6C8(f32 (*pSrc)[4], f32 (*pDst)[4]) {
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
void fn_8000A714(f32 (*pSrc)[4], f32 (*pDst)[4]) {
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

// Inverts a rotation-and-translation matrix: the rotation transposed, and the translation
// turned back through it and negated.
void fn_8000A798(f32 (*pSrc)[4], f32 (*pDst)[4]) {
    f32 aTurned[4];
    f32 aPos[4];

    Vec_Copy(pSrc[3], aPos);
    pDst[0][3] = 0.0f;
    pDst[1][3] = 0.0f;
    pDst[2][3] = 0.0f;
    pDst[3][3] = 1.0f;
    fn_8000A6C8(pSrc, pDst);
    fn_800BADB4(pDst, aPos, aTurned);
    fn_8000AE0C(aTurned, pDst[3]);
}

// Gauss-Jordan elimination with full pivoting: pA is replaced by its inverse, and pB goes
// through the same row steps (so an identity pB also comes out as the inverse). Returns 0, or
// -1 / -2 when pA is singular.
int fn_8000A818(f32 (*pA)[4], f32 (*pB)[4]) {
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
            fn_8000AD34(pA[nRow], pA[nCol]);
            fn_8000AD34(pB[nRow], pB[nCol]);
        }
        anRow[i] = nRow;
        anCol[i] = nCol;
        if (pA[nCol][nCol] == 0.0f) {
            return -2;
        }
        fPivInv = 1.0f / pA[nCol][nCol];
        pA[nCol][nCol] = 1.0f;
        fn_8000AE28(pA[nCol], fPivInv, pA[nCol]);
        fn_8000AE28(pB[nCol], fPivInv, pB[nCol]);
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

// Inverts a 4x4 matrix into pDst.
void fn_8000AB40(f32 (*pSrc)[4], f32 (*pDst)[4]) {
    f32 aIdentity[4][4];

    fn_8000A0E8(pSrc, pDst);
    fn_8000ADC0(aIdentity);
    fn_8000A818(pDst, aIdentity);
}

// A 2D projection: the identity, scaled so fWidth by fHeight spans 2 units (-1 to 1).
void fn_8000AB80(f32 (*pDst)[4], f32 fWidth, f32 fHeight) {
    fn_8000ADC0(pDst);
    pDst[0][0] = 2.0f / fWidth;
    pDst[1][1] = 2.0f / fHeight;
}

// A perspective projection in the GameCube's layout (the rows are the columns of GX's
// matrix): depth from fNear to fFar maps to -1..0, w = -z.
void fn_8000ABE8(f32 (*pDst)[4], f32 fScale, f32 fScaleX, f32 fScaleY, f32 fNear, f32 fFar) {
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

// A perspective projection with its x and y scales given, w = -z: depth fFar maps to 0 and
// fNear to -1 / fNear (-1 only when fNear is 1; fn_8000ABE8 maps it to -1).
void fn_8000AC5C(f32 (*pDst)[4], f32 fScaleX, f32 fScaleY, f32 fNear, f32 fFar) {
    fn_8000ADC0(pDst);
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
void fn_8000AD34(f32* pA, f32* pB) {
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

f32 atan2f(f32 y, f32 x) {
    return atan2(y, x);
}

f32 fabsf(f32 x) {
    return fabs(x);
}

// Sets a 4x4 matrix to the identity.
void fn_8000ADC0(f32 (*pDst)[4]) {
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
asm void fn_8000AE28(register f32* pIn, register f32 fScale, register f32* pOut) {
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
void fn_8000AE28(f32* pIn, f32 fScale, f32* pOut) {
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

    pEntry = lbl_80281BD8;
    i = 0;
    uMantissa = 0;
    do {
        // port: builds the float from its bits through a u32 pointer (see Rand_Float).
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
    lbl_80281BD8 = fn_800951A0(0x400 * sizeof(f32), 16, 1);
    fn_8000AE9C();
}

void fn_8000AF58(void) {
    fn_8009527C(lbl_80281BD8);
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
    pPool = fn_80009B34(uTotal, uFlags, uAlign, "UMemPool.c", 82);
    if (pPool != NULL) {
        pPool->nNodes = nNodes;
        pPool->nFree = nNodes;
        pPool->uNodeSize = uSize;
        // fake match: added as integers; `(u8*)pPool + uTotal` puts pPool first in the add
        pPool->pEnd = (u8*)(uTotal + (uptr)pPool);
        pNode = (u8*)pPool + uAlign;
        pPrev = NULL;
        while (nNodes-- > 0) {
            fn_80005AE8(pNode, 0xDD, uSize);
            ((UMemPoolNode*)pNode)->pNext = pPrev;
            pPrev = (UMemPoolNode*)pNode;
            pNode += uSize;
        }
        pPool->pFree = pPrev;
    }
    return pPool;
}

void UMemPool_Destroy(UMemPool* pPool) {
    fn_80009E70(pPool);
}

// Takes a node off the free list and fills it with 0xBB; NULL when the pool is empty.
void* UMemPool_Alloc(UMemPool* pPool) {
    UMemPoolNode* pNode;

    pNode = pPool->pFree;
    if (pNode != NULL) {
        pPool->pFree = pNode->pNext;
        pPool->nFree--;
        fn_80005AE8(pNode, 0xBB, pPool->uNodeSize);
    }
    return pNode;
}

// Fills a node with 0x99 and puts it back on the free list.
void UMemPool_Free(UMemPool* pPool, void* p) {
    UMemPoolNode* pNode;

    pNode = p;
    fn_80005AE8(pNode, 0x99, pPool->uNodeSize);
    pNode->pNext = pPool->pFree;
    pPool->pFree = pNode;
    pPool->nFree++;
}
