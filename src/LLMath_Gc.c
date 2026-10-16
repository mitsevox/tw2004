// LLMath_Gc.c: hand-written paired-single vector and matrix math used all over the game: vectors
// through a 4x4 matrix, normalising, lengths and distances. A vector is four floats (x, y, z, w); a
// routine whose name ends in 3 reads only x, y and z and leaves the w of its output as it was.

#include "engine.h"
#include "unsorted/cull.h"

static const f32 gNormalizeMinLengthSq = 1.0f / 1073741824.0f;   // 2^-30
static const f32 gRsqrtHalf = 0.5f;
static const f32 gRsqrtThree = 3.0f;
static const f32 gLLMathZero = 0.0f;

// ---- matrices ------------------------------------------------------------------------------
// A vector is a row times the matrix: dest[j] = A[0] * M[0][j] + A[1] * M[1][j] + A[2] * M[2][j]
// + A[3] * M[3][j], so row 3 is the translation, applied in full when A's w is 1. A's floats and
// all of M are read before dest is written, and the List forms read each row of Alist before
// writing that row of Blist, so the output may be the source.

// The four-float vector A times M, into dest.
asm void LLMath_mat44fltMultiply(register f32 (*M)[4], register Vec4* A, register Vec4* dest) {
    nofralloc
    psq_l     f0, 0(A), 0, 0
    psq_l     f1, 0(M), 0, 0
    psq_l     f3, 8(M), 0, 0
    psq_l     f4, 16(M), 0, 0
    ps_muls0  f2, f1, f0
    psq_l     f5, 24(M), 0, 0
    ps_muls0  f3, f3, f0
    psq_l     f6, 32(M), 0, 0
    ps_madds1 f2, f4, f0, f2
    psq_l     f1, 8(A), 0, 0
    ps_madds1 f3, f5, f0, f3
    psq_l     f0, 40(M), 0, 0
    ps_madds0 f2, f6, f1, f2
    psq_l     f4, 48(M), 0, 0
    ps_madds0 f3, f0, f1, f3
    psq_l     f0, 56(M), 0, 0
    ps_madds1 f2, f4, f1, f2
    ps_madds1 f3, f0, f1, f3
    psq_st    f2, 0(dest), 0, 0
    psq_st    f3, 8(dest), 0, 0
    blr
}

// dest's x, y and z = A's x, y and z times the top-left 3x3 of M: a direction through the matrix
// without its translation. M's fourth column and row are not read.
asm void LLMath_mat44fltMultiply33(register f32 (*M)[4], register f32* A, register f32* dest) {
    nofralloc
    psq_l     f0, 0(A), 0, 0
    psq_l     f1, 0(M), 0, 0
    psq_l     f3, 8(M), 1, 0
    ps_muls0  f2, f1, f0
    psq_l     f1, 16(M), 0, 0
    ps_muls0  f3, f3, f0
    psq_l     f4, 24(M), 1, 0
    ps_madds1 f2, f1, f0, f2
    psq_l     f1, 8(A), 1, 0
    ps_madds1 f3, f4, f0, f3
    psq_l     f0, 32(M), 0, 0
    ps_madds0 f2, f0, f1, f2
    psq_l     f0, 40(M), 1, 0
    ps_madds0 f3, f0, f1, f3
    psq_st    f2, 0(dest), 0, 0
    psq_st    f3, 8(dest), 1, 0
    blr
}

// LLMath_mat44fltMultiply on each of the nb rows of Alist into the same row of Blist; with nb 4 this
// is the matrix product Blist = Alist x M. M is read once, first, so Blist may also be M. nb is at
// least 1: 0 would run the loop 2^32 times.
asm void LLMath_mat44fltMultiplyList(register f32 (*M)[4], register f32 (*Alist)[4],
                                     register f32 (*Blist)[4], register int nb) {
    nofralloc
    psq_l     f4, 0(M), 0, 0
    psq_l     f5, 8(M), 0, 0
    psq_l     f6, 16(M), 0, 0
    psq_l     f7, 24(M), 0, 0
    psq_l     f8, 32(M), 0, 0
    psq_l     f9, 40(M), 0, 0
    psq_l     f10, 48(M), 0, 0
    psq_l     f11, 56(M), 0, 0
    mtctr     nb
loop:
    psq_l     f0, 0(Alist), 0, 0
    psq_l     f1, 8(Alist), 0, 0
    ps_muls0  f2, f4, f0
    ps_muls0  f3, f5, f0
    ps_madds1 f2, f6, f0, f2
    ps_madds1 f3, f7, f0, f3
    ps_madds0 f2, f8, f1, f2
    ps_madds0 f3, f9, f1, f3
    ps_madds1 f2, f10, f1, f2
    ps_madds1 f3, f11, f1, f3
    psq_st    f2, 0(Blist), 0, 0
    psq_st    f3, 8(Blist), 0, 0
    addi      Blist, Blist, 16
    addi      Alist, Alist, 16
    bdnz      loop
    blr
}

// LLMath_mat44fltMultiply33 on each of the nb rows of Alist into the same row of Blist; with nb 3
// this is the product of the two top-left 3x3s, Blist's = Alist's x M's, leaving Blist's fourth
// column and row as they were. M is read once, first, so Blist may also be M. nb is at least 1: 0
// would run the loop 2^32 times.
asm void LLMath_mat44fltMultiplyList33(register f32 (*M)[4], register f32 (*Alist)[4],
                                       register f32 (*Blist)[4], register int nb) {
    nofralloc
    psq_l     f4, 0(M), 0, 0
    psq_l     f5, 8(M), 1, 0
    psq_l     f6, 16(M), 0, 0
    psq_l     f7, 24(M), 1, 0
    psq_l     f8, 32(M), 0, 0
    psq_l     f9, 40(M), 1, 0
    mtctr     nb
loop:
    psq_l     f0, 0(Alist), 0, 0
    psq_l     f1, 8(Alist), 1, 0
    ps_muls0  f2, f4, f0
    ps_muls0  f3, f5, f0
    ps_madds1 f2, f6, f0, f2
    ps_madds1 f3, f7, f0, f3
    ps_madds0 f2, f8, f1, f2
    ps_madds0 f3, f9, f1, f3
    psq_st    f2, 0(Blist), 0, 0
    psq_st    f3, 8(Blist), 1, 0
    addi      Blist, Blist, 16
    addi      Alist, Alist, 16
    bdnz      loop
    blr
}

// ---- vectors -------------------------------------------------------------------------------
// One over a length is the hardware's estimate refined by one Newton step,
// r = frsqrte(s), then r = 0.5 * r * (3 - s * r * r), for the squared length s; the length itself
// is s * r. Every source is read before anything is written, so A and dest may be the same.

// The four-float vector A scaled to length 1, into dest. One whose squared length is at most 2^-30
// (or not a number) is copied as it is.
asm void LLMath_Normalize(register f32* A, register f32* dest) {
    nofralloc
    psq_l    f3, 0(A), 0, 0
    psq_l    f4, 8(A), 0, 0
    ps_mul   f5, f3, f3
    ps_madd  f5, f4, f4, f5
    ps_sum0  f5, f5, f5, f5
    lfs      f0, gNormalizeMinLengthSq
    fcmpo    cr0, f5, f0
    ble      store
    frsqrte  f6, f5
    lfs      f2, gRsqrtHalf
    lfs      f0, gRsqrtThree
    frsp     f6, f6
    fmuls    f1, f6, f6
    fmuls    f2, f2, f6
    fnmsubs  f0, f5, f1, f0
    fmuls    f6, f2, f0
    ps_muls0 f3, f3, f6
    ps_muls0 f4, f4, f6
store:
    psq_st   f3, 0(dest), 0, 0
    psq_st   f4, 8(dest), 0, 0
    blr
}

// dest's x, y and z = A's scaled to length 1. One whose squared length is at most 2^-30 (or not a
// number) is copied as it is.
asm void LLMath_Normalize3(register f32* A, register f32* dest) {
    nofralloc
    psq_l    f3, 0(A), 0, 0
    psq_l    f4, 8(A), 1, 0
    ps_mul   f5, f3, f3
    ps_madd  f0, f4, f4, f5
    ps_sum0  f5, f0, f5, f5
    lfs      f0, gNormalizeMinLengthSq
    fcmpo    cr0, f5, f0
    ble      store
    frsqrte  f6, f5
    lfs      f2, gRsqrtHalf
    lfs      f0, gRsqrtThree
    frsp     f6, f6
    fmuls    f1, f6, f6
    fmuls    f2, f2, f6
    fnmsubs  f0, f5, f1, f0
    fmuls    f6, f2, f0
    ps_muls0 f3, f3, f6
    ps_muls0 f4, f4, f6
store:
    psq_st   f3, 0(dest), 0, 0
    psq_st   f4, 8(dest), 1, 0
    blr
}

// LLMath_Normalize that also returns A's four-float length; 0 for a vector it copies as it is.
asm f32 LLMath_NormalizeReturnLength(register f32* A, register f32* dest) {
    nofralloc
    psq_l    f3, 0(A), 0, 0
    psq_l    f4, 8(A), 0, 0
    ps_mul   f5, f3, f3
    ps_madd  f5, f4, f4, f5
    ps_sum0  f5, f5, f5, f5
    lfs      f0, gNormalizeMinLengthSq
    fcmpo    cr0, f5, f0
    ble      zero
    frsqrte  f6, f5
    lfs      f2, gRsqrtHalf
    lfs      f0, gRsqrtThree
    frsp     f6, f6
    fmuls    f1, f6, f6
    fmuls    f2, f2, f6
    fnmsubs  f0, f5, f1, f0
    fmuls    f6, f2, f0
    ps_muls0 f3, f3, f6
    ps_muls0 f4, f4, f6
    psq_st   f3, 0(dest), 0, 0
    psq_st   f4, 8(dest), 0, 0
    fmuls    f1, f6, f5
    blr
zero:
    psq_st   f3, 0(dest), 0, 0
    psq_st   f4, 8(dest), 0, 0
    lfs      f1, gLLMathZero
    blr
}

// LLMath_Normalize3 that also returns the length of A's x, y and z; 0 for a vector it copies as it
// is.
asm f32 LLMath_NormalizeReturnLength3(register f32* A, register f32* dest) {
    nofralloc
    psq_l    f3, 0(A), 0, 0
    psq_l    f4, 8(A), 1, 0
    ps_mul   f5, f3, f3
    ps_madd  f0, f4, f4, f5
    ps_sum0  f5, f0, f5, f5
    lfs      f0, gNormalizeMinLengthSq
    fcmpo    cr0, f5, f0
    ble      zero
    frsqrte  f6, f5
    lfs      f2, gRsqrtHalf
    lfs      f0, gRsqrtThree
    frsp     f6, f6
    fmuls    f1, f6, f6
    fmuls    f2, f2, f6
    fnmsubs  f0, f5, f1, f0
    fmuls    f6, f2, f0
    ps_muls0 f3, f3, f6
    ps_muls0 f4, f4, f6
    psq_st   f3, 0(dest), 0, 0
    psq_st   f4, 8(dest), 1, 0
    fmuls    f1, f6, f5
    blr
zero:
    psq_st   f3, 0(dest), 0, 0
    psq_st   f4, 8(dest), 1, 0
    lfs      f1, gLLMathZero
    blr
}

// The squared distance between the points A and B (x, y and z).
asm f32 LLMath_SquareDistanceBetween3(register f32* A, register f32* B) {
    nofralloc
    psq_l    f0, 0(A), 0, 0
    psq_l    f1, 8(A), 1, 0
    psq_l    f2, 0(B), 0, 0
    psq_l    f3, 8(B), 1, 0
    ps_sub   f0, f0, f2
    ps_sub   f1, f1, f3
    ps_mul   f0, f0, f0
    ps_madd  f1, f1, f1, f0
    ps_sum0  f1, f1, f0, f0
    blr
}

// The distance between the points A and B (x, y and z); 0 when the squared distance is 0 (or not a
// number).
asm f32 LLMath_DistanceBetween3(register f32* A, register f32* B) {
    nofralloc
    psq_l    f0, 0(A), 0, 0
    psq_l    f1, 8(A), 1, 0
    psq_l    f2, 0(B), 0, 0
    psq_l    f3, 8(B), 1, 0
    ps_sub   f0, f0, f2
    ps_sub   f1, f1, f3
    ps_mul   f0, f0, f0
    ps_madd  f3, f1, f1, f0
    ps_sum0  f3, f3, f0, f0
    lfs      f1, gLLMathZero
    fcmpo    cr0, f3, f1
    blelr
    frsqrte  f4, f3
    lfs      f2, gRsqrtHalf
    lfs      f0, gRsqrtThree
    frsp     f4, f4
    fmuls    f1, f4, f4
    fmuls    f2, f2, f4
    fnmsubs  f0, f3, f1, f0
    fmuls    f0, f2, f0
    fmuls    f1, f3, f0
    blr
}
