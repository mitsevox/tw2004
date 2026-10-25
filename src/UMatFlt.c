// UMatFlt.c: 4x4 float matrices for the cameras, the golfer's skeleton, the course's objects and the
// renderer: copies, transposes, rotations to and from Euler angles, inverses and the projections.
// A vector is a row times a matrix, so row 3 is the translation. Then the four-float vector helpers.

#include "engine.h"

static int  mat44flt_gaussj(f32 (*a)[4], f32 (*b)[4]);
static void Vec_Swap(f32* pA, f32* pB);
static void LLMath_Negate3(const f32* src, f32* dest);

// Copies a 4x4 matrix, one row at a time.
void LLMath_CopyMat44(f32 (*src)[4], f32 (*dest)[4]) {
    LLMath_CopyVec(src[0], dest[0]);
    LLMath_CopyVec(src[1], dest[1]);
    LLMath_CopyVec(src[2], dest[2]);
    LLMath_CopyVec(src[3], dest[3]);
}

// Copies rows 0-2 of a 4x4 matrix (the rotation, each row with its fourth float); row 3, the
// translation, is left as it is.
void LLMath_CopyMat34(f32 (*src)[4], f32 (*dest)[4]) {
    LLMath_CopyVec(src[0], dest[0]);
    LLMath_CopyVec(src[1], dest[1]);
    LLMath_CopyVec(src[2], dest[2]);
}

// A rotation matrix from the yaw y (about the y axis), the pitch p (about x) and the roll r (about
// z), in radians, into rows 0-2 of M (their fourth float set to 0; row 3 is left as it is). An
// angle of exactly 0 skips its sin and cos.
void mat44flt_EulerAngles(f32 y, f32 p, f32 r, f32 (*M)[4]) {
    f32 cosy;
    f32 siny;
    f32 cosp;
    f32 sinp;
    f32 cosr;
    f32 sinr;
    f32 sinr_sinp;
    f32 cosr_sinp;

    if (r == 0.0f) {
        if (p == 0.0f) {
            M[1][1] = 1.0f;
            M[2][1] = 0.0f;
            M[1][2] = 0.0f;
            M[1][0] = 0.0f;
            M[0][1] = 0.0f;
            if (y != 0.0f) {
                siny = sinf(y);
                cosy = cosf(y);
                M[2][2] = cosy;
                M[0][0] = cosy;
                M[2][0] = siny;
                M[0][2] = -siny;
            } else {
                M[2][2] = 1.0f;
                M[0][0] = 1.0f;
                M[2][0] = 0.0f;
                M[0][2] = 0.0f;
            }
        } else {
            sinp = sinf(p);
            cosp = cosf(p);
            if (y == 0.0f) {
                M[0][0] = 1.0f;
                M[2][0] = 0.0f;
                M[1][0] = 0.0f;
                M[0][2] = 0.0f;
                M[0][1] = 0.0f;
                M[2][2] = cosp;
                M[1][1] = cosp;
                M[1][2] = sinp;
                M[2][1] = -sinp;
            } else {
                siny = sinf(y);
                cosy = cosf(y);
                M[0][0] = cosy;
                M[0][1] = 0.0f;
                M[0][2] = -siny;
                M[1][1] = cosp;
                M[2][1] = -sinp;
                M[1][0] = sinp * siny;
                M[1][2] = sinp * cosy;
                M[2][0] = cosp * siny;
                M[2][2] = cosp * cosy;
            }
        }
    } else {
        sinr = sinf(r);
        cosr = cosf(r);
        if (p == 0.0f) {
            if (y == 0.0f) {
                M[2][2] = 1.0f;
                M[2][1] = 0.0f;
                M[2][0] = 0.0f;
                M[1][2] = 0.0f;
                M[0][2] = 0.0f;
                M[0][0] = cosr;
                M[1][1] = cosr;
                M[0][1] = sinr;
                M[1][0] = -sinr;
            } else {
                siny = sinf(y);
                cosy = cosf(y);
                M[2][0] = siny;
                M[2][1] = 0.0f;
                M[2][2] = cosy;
                M[0][1] = sinr;
                M[1][1] = cosr;
                M[0][0] = cosr * cosy;
                M[0][2] = -(cosr * siny);
                M[1][0] = -(sinr * cosy);
                M[1][2] = sinr * siny;
            }
        } else {
            sinp = sinf(p);
            cosp = cosf(p);
            if (y == 0.0f) {
                M[2][0] = 0.0f;
                M[2][1] = -sinp;
                M[2][2] = cosp;
                M[0][0] = cosr;
                M[1][0] = -sinr;
                M[0][1] = sinr * cosp;
                M[0][2] = sinr * sinp;
                M[1][1] = cosr * cosp;
                M[1][2] = cosr * sinp;
            } else {
                siny = sinf(y);
                cosy = cosf(y);
                M[2][1] = -sinp;
                sinr_sinp = sinr * sinp;
                cosr_sinp = sinp * cosr;
                M[0][0] = cosr * cosy + sinr_sinp * siny;
                M[0][1] = sinr * cosp;
                M[0][2] = sinr_sinp * cosy - cosr * siny;
                M[1][0] = cosr_sinp * siny - sinr * cosy;
                M[1][1] = cosr * cosp;
                M[1][2] = sinr * siny + cosr_sinp * cosy;
                M[2][0] = cosp * siny;
                M[2][2] = cosp * cosy;
            }
        }
    }
    M[2][3] = 0.0f;
    M[1][3] = 0.0f;
    M[0][3] = 0.0f;
}

// The yaw, pitch and roll (radians) of the rotation in rows 0-2 of M, as mat44flt_EulerAngles takes
// them: yaw atan2(M[2][0], M[2][2]), pitch -asin(M[2][1]), roll atan2(M[0][1], M[1][1]). Where
// M[2][2] or M[1][1] is 0, that angle is a quarter turn, signed by M[2][0] or M[0][1] times
// cos(pitch). Where M[2][2] and M[2][0] are both 0 (the pitch a quarter turn, signed by -M[2][1]),
// the roll is 0 and the yaw comes from row 0.
void mat44flt_ExtractEulerAngles(f32 (*M)[4], f32* pYaw, f32* pPitch, f32* pRoll) {
    f32 y;
    f32 p;
    f32 r;

    if (M[2][2] != 0.0f) {
        y = atan2f(M[2][0], M[2][2]);
        p = -asinf(M[2][1]);
        if (M[1][1] != 0.0f) {
            r = atan2f(M[0][1], M[1][1]);
        } else if (M[0][1] != 0.0f) {
            if (M[0][1] * cosf(p) > 0.0f) {
                r = PI / 2.0f;
            } else {
                r = -PI / 2.0f;
            }
        }
    } else if (M[2][0] == 0.0f) {
        if (M[2][1] > 0.0f) {
            p = -PI / 2.0f;
        } else {
            p = PI / 2.0f;
        }
        r = 0.0f;
        if (M[0][0] == 0.0f) {
            if (M[0][2] > 0.0f) {
                y = -PI / 2.0f;
            } else {
                y = PI / 2.0f;
            }
        } else {
            y = -atan2f(M[0][0], M[0][0]);
        }
    } else {
        p = -asinf(M[2][1]);
        if (M[2][0] * cosf(p) > 0.0f) {
            y = PI / 2.0f;
        } else {
            y = -PI / 2.0f;
        }
        if (M[1][1] != 0.0f) {
            r = atan2f(M[0][1], M[1][1]);
        } else if (M[0][1] * cosf(p) > 0.0f) {
            r = PI / 2.0f;
        } else {
            r = -PI / 2.0f;
        }
    }
    *pYaw = y;
    *pRoll = r;
    *pPitch = p;
}

// Transposes the 3x3 part of a 4x4 matrix (src and dst may be the same matrix).
void LLMath_Transpose33(f32 (*src)[4], f32 (*dst)[4]) {
    f32 f;

    dst[0][0] = src[0][0];
    dst[1][1] = src[1][1];
    dst[2][2] = src[2][2];
    f = src[1][0];
    dst[1][0] = src[0][1];
    dst[0][1] = f;
    f = src[2][0];
    dst[2][0] = src[0][2];
    dst[0][2] = f;
    f = src[1][2];
    dst[1][2] = src[2][1];
    dst[2][1] = f;
}

// Transposes a 4x4 matrix (src and dst may be the same matrix).
void LLMath_Transpose44(f32 (*src)[4], f32 (*dst)[4]) {
    f32 f;

    dst[0][0] = src[0][0];
    dst[1][1] = src[1][1];
    dst[2][2] = src[2][2];
    dst[3][3] = src[3][3];
    f = src[1][0];
    dst[1][0] = src[0][1];
    dst[0][1] = f;
    f = src[2][0];
    dst[2][0] = src[0][2];
    dst[0][2] = f;
    f = src[3][0];
    dst[3][0] = src[0][3];
    dst[0][3] = f;
    f = src[1][2];
    dst[1][2] = src[2][1];
    dst[2][1] = f;
    f = src[1][3];
    dst[1][3] = src[3][1];
    dst[3][1] = f;
    f = src[2][3];
    dst[2][3] = src[3][2];
    dst[3][2] = f;
}

// Inverts a rotation-plus-translation matrix (rotation in rows 0-2, translation in row 3, no
// scale): the rotation is transposed and the new translation is minus the old one turned by it.
// A and invA may be the same matrix.
void LLMath_InvertNormalized(f32 (*A)[4], f32 (*invA)[4]) {
    f32 aTurned[4];
    f32 translation[4];

    LLMath_CopyVec(A[3], translation);
    invA[0][3] = 0.0f;
    invA[1][3] = 0.0f;
    invA[2][3] = 0.0f;
    invA[3][3] = 1.0f;
    LLMath_Transpose33(A, invA);
    LLMath_mat44fltMultiply33(invA, translation, aTurned);
    LLMath_Negate3(aTurned, invA[3]);
}

// Gauss-Jordan elimination with full pivoting: a is replaced by its inverse, and b goes through the
// same row steps (so an identity b comes out as the inverse too). Returns 0, or -1 / -2 when a is
// singular, left part-way through.
static int mat44flt_gaussj(f32 (*a)[4], f32 (*b)[4]) {
    int indxc[4];
    int indxr[4];
    int ipiv[4];
    int i;
    int icol;
    int irow;
    int j;
    int k;
    int l;
    int ll;
    f32 big;
    f32 dum;
    f32 pivinv;
    f32 temp;

    for (j = 0; j < 4; j++) {
        ipiv[j] = 0;
    }
    for (i = 0; i < 4; i++) {
        big = 0.0f;
        for (j = 0; j < 4; j++) {
            if (ipiv[j] != 1) {
                for (k = 0; k < 4; k++) {
                    if (ipiv[k] == 0) {
                        if (fabsf(a[j][k]) >= big) {
                            big = fabsf(a[j][k]);
                            irow = j;
                            icol = k;
                        }
                    } else if (ipiv[k] > 1) {
                        return -1;
                    }
                }
            }
        }
        ipiv[icol]++;
        if (irow != icol) {
            Vec_Swap(a[irow], a[icol]);
            Vec_Swap(b[irow], b[icol]);
        }
        indxr[i] = irow;
        indxc[i] = icol;
        if (a[icol][icol] == 0.0f) {
            return -2;
        }
        pivinv = 1.0f / a[icol][icol];
        a[icol][icol] = 1.0f;
        LLMath_Scale(pivinv, a[icol], a[icol]);
        LLMath_Scale(pivinv, b[icol], b[icol]);
        for (ll = 0; ll < 4; ll++) {
            if (ll != icol) {
                dum = -a[ll][icol];
                a[ll][icol] = 0.0f;
                LLMath_AddScale(a[ll], a[icol], dum, a[ll]);
                LLMath_AddScale(b[ll], b[icol], dum, b[ll]);
            }
        }
    }
    // undo the column swaps, last first
    for (l = 3; l >= 0; l--) {
        if (indxr[l] != indxc[l]) {
            for (k = 0; k < 4; k++) {
                temp = a[k][indxr[l]];
                a[k][indxr[l]] = a[k][indxc[l]];
                a[k][indxc[l]] = temp;
            }
        }
    }
    return 0;
}

// The inverse of a 4x4 matrix, into invA (Gauss-Jordan through mat44flt_gaussj). A singular matrix
// is not reported: invA is left part-way through.
void mat44flt_Invert(f32 (*A)[4], f32 (*invA)[4]) {
    f32 b[4][4];

    LLMath_CopyMat44(A, invA);
    LLMath_IdentifyMat(b);
    mat44flt_gaussj(invA, b);
}

// The identity with x scaled by 2 / fWidth and y by 2 / fHeight, so a view fWidth by fHeight
// centred on 0 spans -1..1 (z is left alone). The render context uses it, with
// Mtx_PerspectiveDepthOverNear, for a lens whose nType is not 0.
void Mtx_OrthoScale(f32 (*pDst)[4], f32 fWidth, f32 fHeight) {
    LLMath_IdentifyMat(pDst);
    pDst[0][0] = 2.0f / fWidth;
    pDst[1][1] = 2.0f / fHeight;
}

// A perspective projection for row vectors (the transpose of GX's layout): x scaled by fScale times
// fScaleX, y by fScale times fScaleY, w = -z, and depth from fNear to fFar mapped to -1..0 as GX
// expects. The render context's projection for a lens whose nType is 0.
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
// only when fNear is 1).
void Mtx_PerspectiveDepthOverNear(f32 (*pDst)[4], f32 fScaleX, f32 fScaleY, f32 fNear, f32 fFar) {
    LLMath_IdentifyMat(pDst);
    pDst[0][0] = fScaleX;
    pDst[1][1] = fScaleY;
    pDst[2][2] = -1.0f / (fFar - fNear);
    pDst[3][2] = -(1.0f + fNear / (fFar - fNear));
    pDst[2][3] = -1.0f;
    pDst[3][3] = 0.0f;
}

// Copies a 4-float vector.
void LLMath_CopyVec(const f32* src, f32* dest) {
    dest[0] = src[0];
    dest[1] = src[1];
    dest[2] = src[2];
    dest[3] = src[3];
}

// Swaps two 4-float vectors.
static void Vec_Swap(f32* pA, f32* pB) {
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
void LLMath_IdentifyMat(f32 (*dest)[4]) {
    dest[0][0] = 1.0f;
    dest[0][1] = 0.0f;
    dest[0][2] = 0.0f;
    dest[0][3] = 0.0f;
    dest[1][0] = 0.0f;
    dest[1][1] = 1.0f;
    dest[1][2] = 0.0f;
    dest[1][3] = 0.0f;
    dest[2][0] = 0.0f;
    dest[2][1] = 0.0f;
    dest[2][2] = 1.0f;
    dest[2][3] = 0.0f;
    dest[3][0] = 0.0f;
    dest[3][1] = 0.0f;
    dest[3][2] = 0.0f;
    dest[3][3] = 1.0f;
}

// Negates the first three floats of src into dest; dest's fourth float is left as it is.
static asm void LLMath_Negate3(register const f32* src, register f32* dest) {
    nofralloc
    psq_l  f0, 0(src), 0, 0
    psq_l  f1, 8(src), 1, 0
    ps_neg f0, f0
    ps_neg f1, f1
    psq_st f0, 0(dest), 0, 0
    psq_st f1, 8(dest), 1, 0
    blr
}

// Scales all four floats of A by Amul into dest (A and dest may be the same).
asm void LLMath_Scale(register f32 Amul, register const f32* A, register f32* dest) {
    nofralloc
    fmr      f2, Amul
    psq_l    f0, 0(A), 0, 0
    psq_l    f1, 8(A), 0, 0
    ps_muls0 f0, f0, f2
    ps_muls0 f1, f1, f2
    psq_st   f0, 0(dest), 0, 0
    psq_st   f1, 8(dest), 0, 0
    blr
}

// Multiplies two 4-float vectors component by component into dest.
asm void LLMath_MultiplyVec(register const f32* A, register const f32* B, register f32* dest) {
    nofralloc
    psq_l  f0, 0(A), 0, 0
    psq_l  f1, 8(A), 0, 0
    psq_l  f2, 0(B), 0, 0
    psq_l  f3, 8(B), 0, 0
    ps_mul f0, f0, f2
    ps_mul f1, f1, f3
    psq_st f0, 0(dest), 0, 0
    psq_st f1, 8(dest), 0, 0
    blr
}

// A + B * Bmul into dest (four floats; dest may be A).
asm void LLMath_AddScale(register const f32* A, register const f32* B, register f32 Bmul,
                         register f32* dest) {
    nofralloc
    fmr       f4, Bmul
    psq_l     f0, 0(A), 0, 0
    psq_l     f1, 8(A), 0, 0
    psq_l     f2, 0(B), 0, 0
    psq_l     f3, 8(B), 0, 0
    ps_madds0 f2, f2, f4, f0
    ps_madds0 f3, f3, f4, f1
    psq_st    f2, 0(dest), 0, 0
    psq_st    f3, 8(dest), 0, 0
    blr
}

// The absolute value of a double.
double fabs(double x) {
    return __fabs(x);
}
