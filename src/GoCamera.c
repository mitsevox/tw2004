// GoCamera.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): not yet
// decompiled; the sweep code below is the matched small functions.

#include "camera.h"

void fn_80076A14(f32* pA, f32* pB, f32* pOut);
void fn_80076A38(f32* pA, f32* pOut);

// ---- sweep code (not yet cleaned up) ----

void fn_8007644C(CamLens* pLens) {
    fn_80009E70(pLens);
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

s32 fn_8000A0E8(s32, s32);
s32 fn_8000A798(s32, s32);
s32 fn_8000ADC0(s32);
s32 fn_8001728C(s32);
void fn_80076954(s32 arg0, s32 arg1);
void fn_800354B4(u8* p, f32 v);
void fn_800769C0(u8* p0, f32 x0, f32 x1);
void fn_80076A04(u8* p, f32 v);

void fn_80076948(CamLens* pLens, f32 fB4, f32 fB8) {
    pLens->fB4 = fB4;
    pLens->fB8 = fB8;
}

void fn_80076954(s32 arg0, s32 arg1) {
    if (arg1 == 0) {
        fn_8000ADC0(arg0 + 4);
        fn_8000ADC0(arg0 + 0x44);
    } else {
        fn_8000A0E8(arg1, arg0 + 4);
        fn_8000A798(arg1, arg0 + 0x44);
    }
    fn_8001728C(arg0);
}

void fn_800769C0(u8* p0, f32 x0, f32 x1) {
    fn_80076A04(p0, x0);
    fn_800354B4(p0, x1);
}

void fn_80076A04(u8* p, f32 v) {
    *(f32*)(p + 0xA8) = v;
}

void fn_80076A0C(CamLens* pLens, s32 nType) {
    pLens->nType = nType;
}

// ---- end of sweep code ----

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
