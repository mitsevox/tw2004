#include "dolphin/mtx.h"
#include "dolphin/os.h"
#include <math.h>

static f32 Unit01[] = {0.0f, 1.0f};

extern f32 sinf(f32);

void C_MTXIdentity(Mtx m) {
#line 189
  ASSERTMSG(m, "MtxIdentity():  NULL Mtx 'm' ");
  m[0][0] = 1.0f;
  m[0][1] = 0.0f;
  m[0][2] = 0.0f;
  m[0][3] = 0.0f;

  m[1][0] = 0.0f;
  m[1][1] = 1.0f;
  m[1][2] = 0.0f;
  m[1][3] = 0.0f;

  m[2][0] = 0.0f;
  m[2][1] = 0.0f;
  m[2][2] = 1.0f;
  m[2][3] = 0.0f;
}

#ifdef GEKKO
void PSMTXIdentity(register Mtx m) {
  register f32 zero_c = 0.0f;
  register f32 one_c = 1.0f;
  register f32 c_01;
  register f32 c_10;
  // clang-format off
  asm {
    psq_st zero_c, 8(m), 0, 0
    ps_merge01 c_01, zero_c, one_c
    psq_st zero_c, 24(m), 0, 0
    ps_merge10 c_10, one_c, zero_c
    psq_st zero_c, 32(m), 0, 0
    psq_st c_01, 16(m), 0, 0
    psq_st c_10, 0(m), 0, 0
    psq_st c_10, 40(m), 0, 0
  }
  // clang-format on
}
#endif

#ifdef GEKKO
#endif

#ifdef GEKKO
asm void PSMTXConcat(const register Mtx mA, const register Mtx mB, register Mtx mAB) {
  // clang-format off
  nofralloc

#define FP0 fp0
#define FP1 fp1
#define FP2 fp2
#define FP3 fp3
#define FP4 fp4
#define FP5 fp5
#define FP6 fp6
#define FP7 fp7
#define FP8 fp8
#define FP9 fp9
#define FP10 fp10
#define FP11 fp11
#define FP12 fp12
#define FP13 fp13
#define FP14 fp14
#define FP15 fp15
#define FP31 fp31
    stwu    r1, -64(r1);
    psq_l   FP0, 0(mA), 0, 0;
    stfd    fp14, 8(r1);
    psq_l   FP6, 0(mB), 0, 0;
    addis   r6, 0, Unit01@ha;
    psq_l   FP7, 8(mB), 0, 0;
    stfd    fp15, 16(r1)
    addi    r6, r6, Unit01@l;
    stfd    fp31, 40(r1)
    psq_l   FP8, 16(mB), 0, 0
    ps_muls0 FP12, FP6, FP0
    psq_l   FP2, 16(mA), 0, 0
    ps_muls0 FP13, FP7, FP0
    psq_l   FP31, 0(r6), 0, 0
    ps_muls0 FP14, FP6, FP2
    psq_l   FP9, 24(mB), 0, 0
    ps_muls0 FP15, FP7, FP2
    psq_l   FP1, 8(mA), 0, 0
    ps_madds1 FP12, FP8, FP0, FP12
    psq_l   FP3, 24(mA), 0, 0
    ps_madds1 FP14, FP8, FP2, FP14
    psq_l   FP10, 32(mB), 0, 0
    ps_madds1 FP13, FP9, FP0, FP13
    psq_l   FP11, 40(mB), 0, 0
    ps_madds1 FP15, FP9, FP2, FP15
    psq_l   FP4, 32(mA), 0, 0
    psq_l   FP5, 40(mA), 0, 0
    ps_madds0 FP12, FP10, FP1, FP12
    ps_madds0 FP13, FP11, FP1, FP13
    ps_madds0 FP14, FP10, FP3, FP14
    ps_madds0 FP15, FP11, FP3, FP15
    psq_st  FP12, 0(mAB), 0, 0

    ps_muls0 FP2, FP6, FP4
    ps_madds1 FP13, FP31, FP1, FP13
    ps_muls0 FP0, FP7, FP4
    psq_st  FP14, 16(mAB), 0, 0
    ps_madds1 FP15, FP31, FP3, FP15

    psq_st  FP13, 8(mAB), 0, 0

    ps_madds1 FP2, FP8, FP4, FP2
    ps_madds1 FP0, FP9, FP4, FP0
    ps_madds0 FP2, FP10, FP5, FP2
    lfd    fp14, 8(r1)
    psq_st  FP15, 24(mAB), 0, 0
    ps_madds0 FP0, FP11, FP5, FP0
    psq_st  FP2, 32(mAB), 0, 0
    ps_madds1 FP0, FP31, FP5, FP0
    lfd    fp15, 16(r1)
    psq_st  FP0, 40(mAB), 0, 0

    lfd    fp31, 40(r1)
    addi   r1, r1, 64

    blr
  // clang-format on

#undef FP0
#undef FP1
#undef FP2
#undef FP3
#undef FP4
#undef FP5
#undef FP6
#undef FP7
#undef FP8
#undef FP9
#undef FP10
#undef FP11
#undef FP12
#undef FP13
#undef FP14
#undef FP15
#undef FP31
}
#endif

#ifdef GEKKO
#if (defined(__MWERKS__) && defined(_DEBUG))
#pragma global_optimizer on
#pragma optimization_level 1
#endif

#if (defined(__MWERKS__) && defined(_DEBUG))
#pragma optimization_level 0
#pragma global_optimizer reset
#endif

#endif

#ifdef GEKKO
#endif

#ifdef GEKKO
#endif

#ifdef GEKKO
asm u32 PSMTXInvXpose(const register Mtx src, register Mtx invX) {
  // clang-format off
  nofralloc

  psq_l       fp0, 0(src), 1, 0
  psq_l       fp1, 4(src), 0, 0
  psq_l       fp2, 16(src), 1, 0
  ps_merge10  fp6, fp1, fp0
  psq_l       fp3, 20(src), 0, 0
  psq_l       fp4, 32(src), 1, 0
  ps_merge10  fp7, fp3, fp2
  psq_l       fp5, 36(src), 0, 0
  ps_mul      fp11, fp3, fp6
  ps_merge10  fp8, fp5, fp4
  ps_mul      fp13, fp5, fp7
  ps_msub     fp11, fp1, fp7, fp11
  ps_mul      fp12, fp1, fp8
  ps_msub     fp13, fp3, fp8, fp13
  ps_msub     fp12, fp5, fp6, fp12
  ps_mul      fp10, fp3, fp4
  ps_mul      fp9,  fp0, fp5
  ps_mul      fp8,  fp1, fp2
  ps_msub     fp10, fp2, fp5, fp10
  ps_msub     fp9,  fp1, fp4, fp9
  ps_msub     fp8,  fp0, fp3, fp8
  ps_mul      fp7, fp0, fp13
  ps_sub      fp1, fp1, fp1
  ps_madd     fp7, fp2, fp12, fp7
  ps_madd     fp7, fp4, fp11, fp7
  ps_cmpo0    cr0, fp7, fp1
  bne         _regular
  addi        r3, 0, 0
  blr

_regular:
  fres        fp0, fp7
  psq_st      fp1,  12(invX), 1, 0
  ps_add      fp6, fp0, fp0
  ps_mul      fp5, fp0, fp0
  psq_st      fp1,  28(invX), 1, 0
  ps_nmsub    fp0, fp7, fp5, fp6
  psq_st      fp1,  44(invX), 1, 0
  ps_muls0    fp13, fp13, fp0
  ps_muls0    fp12, fp12, fp0
  ps_muls0    fp11, fp11, fp0
  psq_st      fp13,  0(invX), 0, 0
  psq_st      fp12,  16(invX), 0, 0
  ps_muls0    fp10, fp10, fp0
  ps_muls0    fp9,  fp9,  fp0
  psq_st      fp11,  32(invX), 0, 0
  psq_st      fp10,  8(invX), 1, 0
  ps_muls0    fp8,  fp8,  fp0
  addi        r3, 0, 1
  psq_st      fp9,   24(invX), 1, 0
  psq_st      fp8,   40(invX), 1, 0
  blr
  // clang-format on
}
#endif

#ifdef GEKKO
#endif

#ifdef GEKKO

#endif

#ifdef GEKKO

#endif

#ifdef GEKKO
void PSMTXTrans(register Mtx m, register f32 xT, register f32 yT, register f32 zT) {
  register f32 c0 = 0.0F;
  register f32 c1 = 1.0F;
  // clang-format off
  asm
  {
    stfs        xT,     12(m)
    stfs        yT,     28(m)
    psq_st      c0,      4(m), 0, 0
    psq_st      c0,     32(m), 0, 0
    stfs        c0,     16(m)
    stfs        c1,     20(m)
    stfs        c0,     24(m)
    stfs        c1,     40(m)
    stfs        zT,     44(m)
    stfs        c1,      0(m)
  }
  // clang-format on
}
#endif

#ifdef GEKKO
#endif

#ifdef GEKKO
void PSMTXScale(register Mtx m, register f32 xS, register f32 yS, register f32 zS) {
  register f32 c0 = 0.0F;
  // clang-format off
  asm
  {
    stfs        xS,      0(m)
    psq_st      c0,      4(m), 0, 0
    psq_st      c0,     12(m), 0, 0
    stfs        yS,     20(m)
    psq_st      c0,     24(m), 0, 0
    psq_st      c0,     32(m), 0, 0
    stfs        zS,     40(m)
    stfs        c0,     44(m)
  }
  // clang-format on
}
#endif

#ifdef GEKKO
#endif

#ifdef GEKKO
#endif

#ifdef GEKKO
#endif

