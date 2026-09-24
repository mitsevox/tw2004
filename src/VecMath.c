// VecMath.c (our name; TW06 names these functions vec4flt_*): hand-written paired-single vector
// and matrix helpers used all over the game: vectors through a 4x4 matrix, normalising, lengths
// and distances.

#include "game_types.h"
#include "engine.h"
#include "unsorted/cull.h"

void fn_800BADB4(f32 (*pMtx)[4], f32* pIn, f32* pOut);
void fn_800BADF8(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows);
void fn_800BAE5C(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows);

// ---- matrices ------------------------------------------------------------------------------
// A row vector times a 4x4 matrix: out[j] = in[0] * m[0][j] + in[1] * m[1][j] + ...

#ifdef __MWERKS__
// A four-float vector through the whole matrix.
asm void fn_800BAD60(register float mtx[4][4], register Vec4* src, register Vec4* dst) {
    nofralloc
    psq_l     f0, 0(src), 0, 0
    psq_l     f1, 0(mtx), 0, 0
    psq_l     f3, 8(mtx), 0, 0
    psq_l     f4, 16(mtx), 0, 0
    ps_muls0  f2, f1, f0
    psq_l     f5, 24(mtx), 0, 0
    ps_muls0  f3, f3, f0
    psq_l     f6, 32(mtx), 0, 0
    ps_madds1 f2, f4, f0, f2
    psq_l     f1, 8(src), 0, 0
    ps_madds1 f3, f5, f0, f3
    psq_l     f0, 40(mtx), 0, 0
    ps_madds0 f2, f6, f1, f2
    psq_l     f4, 48(mtx), 0, 0
    ps_madds0 f3, f0, f1, f3
    psq_l     f0, 56(mtx), 0, 0
    ps_madds1 f2, f4, f1, f2
    ps_madds1 f3, f0, f1, f3
    psq_st    f2, 0(dst), 0, 0
    psq_st    f3, 8(dst), 0, 0
    blr
}

// A three-float vector through the matrix's top-left 3x3 (a rotation, no translation).
asm void fn_800BADB4(register f32 (*pMtx)[4], register f32* pIn, register f32* pOut) {
    nofralloc
    psq_l     f0, 0(pIn), 0, 0
    psq_l     f1, 0(pMtx), 0, 0
    psq_l     f3, 8(pMtx), 1, 0
    ps_muls0  f2, f1, f0
    psq_l     f1, 16(pMtx), 0, 0
    ps_muls0  f3, f3, f0
    psq_l     f4, 24(pMtx), 1, 0
    ps_madds1 f2, f1, f0, f2
    psq_l     f1, 8(pIn), 1, 0
    ps_madds1 f3, f4, f0, f3
    psq_l     f0, 32(pMtx), 0, 0
    ps_madds0 f2, f0, f1, f2
    psq_l     f0, 40(pMtx), 1, 0
    ps_madds0 f3, f0, f1, f3
    psq_st    f2, 0(pOut), 0, 0
    psq_st    f3, 8(pOut), 1, 0
    blr
}

// fn_800BAD60 for nRows four-float vectors in a row.
asm void fn_800BADF8(register f32 (*pMtx)[4], register f32 (*pSrc)[4], register f32 (*pDst)[4],
                     register int nRows) {
    nofralloc
    psq_l     f4, 0(pMtx), 0, 0
    psq_l     f5, 8(pMtx), 0, 0
    psq_l     f6, 16(pMtx), 0, 0
    psq_l     f7, 24(pMtx), 0, 0
    psq_l     f8, 32(pMtx), 0, 0
    psq_l     f9, 40(pMtx), 0, 0
    psq_l     f10, 48(pMtx), 0, 0
    psq_l     f11, 56(pMtx), 0, 0
    mtctr     nRows
loop:
    psq_l     f0, 0(pSrc), 0, 0
    psq_l     f1, 8(pSrc), 0, 0
    ps_muls0  f2, f4, f0
    ps_muls0  f3, f5, f0
    ps_madds1 f2, f6, f0, f2
    ps_madds1 f3, f7, f0, f3
    ps_madds0 f2, f8, f1, f2
    ps_madds0 f3, f9, f1, f3
    ps_madds1 f2, f10, f1, f2
    ps_madds1 f3, f11, f1, f3
    psq_st    f2, 0(pDst), 0, 0
    psq_st    f3, 8(pDst), 0, 0
    addi      pDst, pDst, 16
    addi      pSrc, pSrc, 16
    bdnz      loop
    blr
}

// fn_800BADB4 for nRows vectors in a row (three floats used of each four).
asm void fn_800BAE5C(register f32 (*pMtx)[4], register f32 (*pSrc)[4], register f32 (*pDst)[4],
                     register int nRows) {
    nofralloc
    psq_l     f4, 0(pMtx), 0, 0
    psq_l     f5, 8(pMtx), 1, 0
    psq_l     f6, 16(pMtx), 0, 0
    psq_l     f7, 24(pMtx), 1, 0
    psq_l     f8, 32(pMtx), 0, 0
    psq_l     f9, 40(pMtx), 1, 0
    mtctr     nRows
loop:
    psq_l     f0, 0(pSrc), 0, 0
    psq_l     f1, 8(pSrc), 1, 0
    ps_muls0  f2, f4, f0
    ps_muls0  f3, f5, f0
    ps_madds1 f2, f6, f0, f2
    ps_madds1 f3, f7, f0, f3
    ps_madds0 f2, f8, f1, f2
    ps_madds0 f3, f9, f1, f3
    psq_st    f2, 0(pDst), 0, 0
    psq_st    f3, 8(pDst), 1, 0
    addi      pDst, pDst, 16
    addi      pSrc, pSrc, 16
    bdnz      loop
    blr
}
#else
// port: untested, the plain-C versions for compilers without paired singles. The paired-single
// code rounds each multiply-add the same way, so results can differ in the last bits.
void fn_800BAD60(float mtx[4][4], Vec4* src, Vec4* dst) {
    f32* pIn = (f32*)src;
    f32 out[4];
    int j;
    for (j = 0; j < 4; j++) {
        out[j] = pIn[0] * mtx[0][j] + pIn[1] * mtx[1][j] + pIn[2] * mtx[2][j] + pIn[3] * mtx[3][j];
    }
    for (j = 0; j < 4; j++) {
        ((f32*)dst)[j] = out[j];
    }
}

void fn_800BADB4(f32 (*pMtx)[4], f32* pIn, f32* pOut) {
    f32 out[3];
    int j;
    for (j = 0; j < 3; j++) {
        out[j] = pIn[0] * pMtx[0][j] + pIn[1] * pMtx[1][j] + pIn[2] * pMtx[2][j];
    }
    for (j = 0; j < 3; j++) {
        pOut[j] = out[j];
    }
}

void fn_800BADF8(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows) {
    int i;
    for (i = 0; i < nRows; i++) {
        fn_800BAD60(pMtx, (Vec4*)pSrc[i], (Vec4*)pDst[i]);
    }
}

void fn_800BAE5C(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows) {
    int i;
    for (i = 0; i < nRows; i++) {
        fn_800BADB4(pMtx, pSrc[i], pDst[i]);
    }
}
#endif

// ---- vectors -------------------------------------------------------------------------------
// The square roots are one Newton step on the hardware estimate:
// r = frsqrte(s); r = 0.5 * r * (3 - s * r * r).

#ifdef __MWERKS__
static const f32 kVecEpsilon = 1.0f / 1073741824.0f;   // no scaling when length^2 <= 2^-30
static const f32 kHalf = 0.5f;
static const f32 kThree = 3.0f;
static const f32 kZero = 0.0f;

// Normalise a four-float vector from pSrc into pDst; a near-zero one is copied.
asm void Vec_Normalize(register f32* pSrc, register f32* pDst) {
    nofralloc
    psq_l    f3, 0(pSrc), 0, 0
    psq_l    f4, 8(pSrc), 0, 0
    ps_mul   f5, f3, f3
    ps_madd  f5, f4, f4, f5
    ps_sum0  f5, f5, f5, f5
    lfs      f0, kVecEpsilon
    fcmpo    cr0, f5, f0
    ble      store
    frsqrte  f6, f5
    lfs      f2, kHalf
    lfs      f0, kThree
    frsp     f6, f6
    fmuls    f1, f6, f6
    fmuls    f2, f2, f6
    fnmsubs  f0, f5, f1, f0
    fmuls    f6, f2, f0
    ps_muls0 f3, f3, f6
    ps_muls0 f4, f4, f6
store:
    psq_st   f3, 0(pDst), 0, 0
    psq_st   f4, 8(pDst), 0, 0
    blr
}

// Normalise a three-float vector from pSrc into pDst; a near-zero one is copied.
asm void fn_800BAF04(register f32* pSrc, register f32* pDst) {
    nofralloc
    psq_l    f3, 0(pSrc), 0, 0
    psq_l    f4, 8(pSrc), 1, 0
    ps_mul   f5, f3, f3
    ps_madd  f0, f4, f4, f5
    ps_sum0  f5, f0, f5, f5
    lfs      f0, kVecEpsilon
    fcmpo    cr0, f5, f0
    ble      store
    frsqrte  f6, f5
    lfs      f2, kHalf
    lfs      f0, kThree
    frsp     f6, f6
    fmuls    f1, f6, f6
    fmuls    f2, f2, f6
    fnmsubs  f0, f5, f1, f0
    fmuls    f6, f2, f0
    ps_muls0 f3, f3, f6
    ps_muls0 f4, f4, f6
store:
    psq_st   f3, 0(pDst), 0, 0
    psq_st   f4, 8(pDst), 1, 0
    blr
}

// Vec_Normalize that also returns the length (0 for a near-zero vector).
asm f32 fn_800BAF58(register f32* pSrc, register f32* pDst) {
    nofralloc
    psq_l    f3, 0(pSrc), 0, 0
    psq_l    f4, 8(pSrc), 0, 0
    ps_mul   f5, f3, f3
    ps_madd  f5, f4, f4, f5
    ps_sum0  f5, f5, f5, f5
    lfs      f0, kVecEpsilon
    fcmpo    cr0, f5, f0
    ble      zero
    frsqrte  f6, f5
    lfs      f2, kHalf
    lfs      f0, kThree
    frsp     f6, f6
    fmuls    f1, f6, f6
    fmuls    f2, f2, f6
    fnmsubs  f0, f5, f1, f0
    fmuls    f6, f2, f0
    ps_muls0 f3, f3, f6
    ps_muls0 f4, f4, f6
    psq_st   f3, 0(pDst), 0, 0
    psq_st   f4, 8(pDst), 0, 0
    fmuls    f1, f6, f5
    blr
zero:
    psq_st   f3, 0(pDst), 0, 0
    psq_st   f4, 8(pDst), 0, 0
    lfs      f1, kZero
    blr
}

// fn_800BAF04 that also returns the length (0 for a near-zero vector).
asm f32 fn_800BAFC0(register f32* pSrc, register f32* pDst) {
    nofralloc
    psq_l    f3, 0(pSrc), 0, 0
    psq_l    f4, 8(pSrc), 1, 0
    ps_mul   f5, f3, f3
    ps_madd  f0, f4, f4, f5
    ps_sum0  f5, f0, f5, f5
    lfs      f0, kVecEpsilon
    fcmpo    cr0, f5, f0
    ble      zero
    frsqrte  f6, f5
    lfs      f2, kHalf
    lfs      f0, kThree
    frsp     f6, f6
    fmuls    f1, f6, f6
    fmuls    f2, f2, f6
    fnmsubs  f0, f5, f1, f0
    fmuls    f6, f2, f0
    ps_muls0 f3, f3, f6
    ps_muls0 f4, f4, f6
    psq_st   f3, 0(pDst), 0, 0
    psq_st   f4, 8(pDst), 1, 0
    fmuls    f1, f6, f5
    blr
zero:
    psq_st   f3, 0(pDst), 0, 0
    psq_st   f4, 8(pDst), 1, 0
    lfs      f1, kZero
    blr
}

// The squared distance between two three-float points.
asm f32 fn_800BB028(register f32* pA, register f32* pB) {
    nofralloc
    psq_l    f0, 0(pA), 0, 0
    psq_l    f1, 8(pA), 1, 0
    psq_l    f2, 0(pB), 0, 0
    psq_l    f3, 8(pB), 1, 0
    ps_sub   f0, f0, f2
    ps_sub   f1, f1, f3
    ps_mul   f0, f0, f0
    ps_madd  f1, f1, f1, f0
    ps_sum0  f1, f1, f0, f0
    blr
}

// The distance between two three-float points.
asm f32 Vec_Distance(register f32* pA, register f32* pB) {
    nofralloc
    psq_l    f0, 0(pA), 0, 0
    psq_l    f1, 8(pA), 1, 0
    psq_l    f2, 0(pB), 0, 0
    psq_l    f3, 8(pB), 1, 0
    ps_sub   f0, f0, f2
    ps_sub   f1, f1, f3
    ps_mul   f0, f0, f0
    ps_madd  f3, f1, f1, f0
    ps_sum0  f3, f3, f0, f0
    lfs      f1, kZero
    fcmpo    cr0, f3, f1
    blelr
    frsqrte  f4, f3
    lfs      f2, kHalf
    lfs      f0, kThree
    frsp     f4, f4
    fmuls    f1, f4, f4
    fmuls    f2, f2, f4
    fnmsubs  f0, f3, f1, f0
    fmuls    f0, f2, f0
    fmuls    f1, f3, f0
    blr
}
#else
// port: untested, the plain-C versions for compilers without paired singles. The originals use
// the hardware square-root estimate plus one refinement step, so results differ in the last bits.
static f32 Vec3_LengthSq(const f32* p) {
    return p[0] * p[0] + p[1] * p[1] + p[2] * p[2];
}

void Vec_Normalize(f32* pSrc, f32* pDst) {
    f32 s = pSrc[0] * pSrc[0] + pSrc[1] * pSrc[1] + pSrc[2] * pSrc[2] + pSrc[3] * pSrc[3];
    f32 k = 1.0f;
    int i;
    if (s > 1.0f / 1073741824.0f) {
        k = 1.0f / (f32)sqrt(s);
    }
    for (i = 0; i < 4; i++) {
        pDst[i] = pSrc[i] * k;
    }
}

void fn_800BAF04(f32* pSrc, f32* pDst) {
    f32 s = Vec3_LengthSq(pSrc);
    f32 k = 1.0f;
    int i;
    if (s > 1.0f / 1073741824.0f) {
        k = 1.0f / (f32)sqrt(s);
    }
    for (i = 0; i < 3; i++) {
        pDst[i] = pSrc[i] * k;
    }
}

f32 fn_800BAF58(f32* pSrc, f32* pDst) {
    f32 s = pSrc[0] * pSrc[0] + pSrc[1] * pSrc[1] + pSrc[2] * pSrc[2] + pSrc[3] * pSrc[3];
    f32 fLen;
    int i;
    if (!(s > 1.0f / 1073741824.0f)) {
        for (i = 0; i < 4; i++) {
            pDst[i] = pSrc[i];
        }
        return 0.0f;
    }
    fLen = (f32)sqrt(s);
    for (i = 0; i < 4; i++) {
        pDst[i] = pSrc[i] / fLen;
    }
    return fLen;
}

f32 fn_800BAFC0(f32* pSrc, f32* pDst) {
    f32 s = Vec3_LengthSq(pSrc);
    f32 fLen;
    int i;
    if (!(s > 1.0f / 1073741824.0f)) {
        for (i = 0; i < 3; i++) {
            pDst[i] = pSrc[i];
        }
        return 0.0f;
    }
    fLen = (f32)sqrt(s);
    for (i = 0; i < 3; i++) {
        pDst[i] = pSrc[i] / fLen;
    }
    return fLen;
}

f32 fn_800BB028(f32* pA, f32* pB) {
    f32 d[3];
    d[0] = pA[0] - pB[0];
    d[1] = pA[1] - pB[1];
    d[2] = pA[2] - pB[2];
    return Vec3_LengthSq(d);
}

f32 Vec_Distance(f32* pA, f32* pB) {
    f32 s = fn_800BB028(pA, pB);
    if (!(s > 0.0f)) return 0.0f;
    return (f32)sqrt(s);
}
#endif
