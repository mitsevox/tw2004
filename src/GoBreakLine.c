// GoBreakLine.c (EA's name, from its asserts): the putt's break line, a line on the green from the
// ball that shows how the putt will break (BreakLine, breakline.h). Partly decompiled.

#include "golfer.h"
#include "camera.h"
#include "breakline.h"

BreakLine* lbl_80282228;
u8 lbl_8028222C;

void fn_800360A0(void* pMesh);         // Skin.c: frees a mesh object

void fn_800C808C(void) {
    lbl_80282228 = fn_80009B34(sizeof(BreakLine), 2, 16, "GoBreakLine.c", 93);
    lbl_80282228->fAB30 = 27.0f;
    lbl_80282228->fAB34 = 0.0f;
    lbl_80282228->fAB38 = 20.0f;
    lbl_80282228->fAB3C = 0.009f;
}

void fn_800C8108(void) {
    fn_80009E70(lbl_80282228);
    lbl_80282228 = NULL;
}

void fn_800C830C(void) {
    fn_800360A0(lbl_80282228->aMesh[0]);
    if (gSession.nSplitScreen) {
        fn_800360A0(lbl_80282228->aMesh[1]);
    }
    lbl_8028222C = 0;
}

// Steps view nView's line while its player, a human, stands over a putt within 75 of the hole
// and the ball is within an inch of the pin (EA's test; distances in yards).
void BreakLine_Update(int nView) {
    int nPlayer = fn_8001707C(nView);

    if (36.0f * Vec_Distance(lbl_80282228->vPin, gPlayers[nPlayer].vTarget) <= 1.0f &&
        gPlayers[nPlayer].nShotKind == 0 && gPlayers[nPlayer].swing.nState == 0 &&
        gPlayers[nPlayer].fDistance < 75.0f && !Player_IsCPU(nPlayer) && lbl_8028222C) {
        if (!lbl_80282228->abSkip[nView]) {
            BreakLine_Step(nView);
            return;
        }
        lbl_80282228->abSkip[nView] = 0;
    }
}

// View nView's point.
void fn_800C8C3C(int nView, f32* pOut) {
    Vec_Copy(lbl_80282228->aViewPoint[nView], pOut);
}

// a + b into out (three floats)
#ifdef __MWERKS__
asm void fn_800C9310(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_add f2, f2, f0
    ps_add f3, f3, f1
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_800C9310(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

// a - b into out (four floats)
#ifdef __MWERKS__
asm void fn_800C9334(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 0, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 0, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_800C9334(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
    pOut[3] = pA[3] - pB[3];
}
#endif

// a - b into out (three floats)
#ifdef __MWERKS__
asm void fn_800C9358(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_800C9358(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif
