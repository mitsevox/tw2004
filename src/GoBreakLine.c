// GoBreakLine.c (EA's name, from its asserts): the putt's break line, a line on the green from the
// ball that shows how the putt will break (BreakLine, breakline.h). Partly decompiled.

#include "golfer.h"
#include "game.h"
#include "camera.h"
#include "breakline.h"

BreakLine* lbl_80282228;
u8 lbl_8028222C;

void fn_80036054(void* pMesh, int n, s32* pDesc);    // Skin.c: sets up a mesh object
void fn_800360A0(void* pMesh);         // Skin.c: frees a mesh object
void fn_800C9358(f32* pA, f32* pB, f32* pOut);

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

// Sets the line up for a hole: its settings, a mesh per view, the "brkline" texture and the pin.
void fn_800C8134(void) {
    CourseInfo* pCourse = fn_8000C594();
    int nPin = Game_CurrentPinSet();
    s32 desc[2];
    u64 uHash;

    lbl_80282228->fAAE0 = 0.02f;
    lbl_80282228->fAAE4 = 4.0f;
    lbl_80282228->nAAEC = 5;
    lbl_80282228->fAB0C = 0.01f;
    lbl_80282228->fAB10 = -800.0f;
    lbl_80282228->fAB14 = 0.0f;
    lbl_80282228->fAB18 = 100.0f;
    lbl_80282228->fAB1C = 0.024f;
    lbl_80282228->anColor[0] = 0x80;
    lbl_80282228->anColor[1] = 0x80;
    lbl_80282228->anColor[2] = 0x80;
    lbl_80282228->anColor[3] = 0x33;
    desc[0] = gSession.nSplitScreen ? 450 : 900;
    desc[1] = 1;
    fn_80036054(lbl_80282228->aMesh[0], 5, desc);
    if (gSession.nSplitScreen) {
        fn_80036054(lbl_80282228->aMesh[1], 5, desc);
    }
    uHash = fn_8000BEE4("brkline");
    fn_800102DC(uHash, &lbl_80282228->pBank, &lbl_80282228->pTex);
    Vec_Copy(&pCourse->pin[nPin].x, lbl_80282228->vPin);
    lbl_80282228->abSkip[0] = 1;
    lbl_80282228->anA928[0] = 0;
    if (gSession.nSplitScreen) {
        lbl_80282228->abSkip[1] = 1;
        lbl_80282228->anA928[1] = 0;
    }
    lbl_8028222C = 0;
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

// The caddie's putt read for view nView, in feet: how far past (+) or short of the hole the aim
// point lies along the line from the ball, and how far to the side of that line (the sign gives
// the side). 0, 0 in split screen or when the points coincide; -999 when there is no tip, 999
// when the caddie gave up.
void fn_800C9038(int nView, f32* pLong, f32* pSide) {
    int nPlayer = fn_8001707C(nView);
    int nPin = Game_CurrentPinSet();
    CourseInfo* pCourse = fn_8000C594();
    s8 nTip;
    f32 vTip[4];
    f32 vPin[4];
    f32 vHole[4];
    f32 vBall[4];
    f32 vAim[4];
    f32 vToHole[4];
    f32 vToAim[4];
    f32 vCross[4];
    f32 fHoleDist;
    f32 fAimDist;
    f32 fDot;
    f32 fAlong;
    f32 fSide;
    f32 fSign;

    if (gSession.nSplitScreen) {
        *pLong = 0.0f;
        *pSide = 0.0f;
        return;
    }
    nTip = Caddie_GetTip(nPlayer, vTip);
    if (nTip == 0) {
        *pLong = -999.0f;
        *pSide = -999.0f;
        return;
    }
    if (nTip == 2) {
        *pLong = 999.0f;
        *pSide = 999.0f;
        return;
    }
    // Flatten the three points onto the ground (y = 0).
    Vec_Copy(&pCourse->pin[nPin].x, vPin);
    Vec_Copy(vPin, vHole);
    vHole[1] = 0.0f;
    Vec_Copy(gPlayers[nPlayer].vBall, vBall);
    vBall[1] = 0.0f;
    Vec_Copy(vTip, vAim);
    vAim[1] = 0.0f;
    if (vAim[0] == vHole[0] && vAim[2] == vHole[2]) {
        *pLong = 0.0f;
        *pSide = 0.0f;
        return;
    }
    fHoleDist = Vec_Distance(vHole, vBall);
    if (fabs(fHoleDist) < 0.001f) {
        *pLong = 0.0f;
        *pSide = 0.0f;
        return;
    }
    if (vAim[0] == vBall[0] && vAim[2] == vBall[2]) {
        *pLong = 0.0f;
        *pSide = 0.0f;
        return;
    }
    fAimDist = Vec_Distance(vAim, vBall);
    if (fabs(fAimDist) < 0.001f) {
        *pLong = 0.0f;
        *pSide = 0.0f;
        return;
    }
    if (vBall[0] == vHole[0] && vBall[2] == vHole[2]) {
        *pLong = 0.0f;
        *pSide = 0.0f;
        return;
    }
    fn_800C9358(vHole, vBall, vToHole);
    fn_800BAF04(vToHole, vToHole);
    fn_800C9358(vAim, vBall, vToAim);
    fn_800BAF04(vToAim, vToAim);
    fDot = fn_8000C5FC(vToHole, vToAim);
    if (0.0f == fDot) {
        *pLong = 0.0f;
        *pSide = 0.0f;
        return;
    }
    fAlong = fDot * fAimDist;
    fSide = fAimDist * fn_800095F0(fn_80009614(fDot));
    vec4flt_CrossProduct(vToHole, vToAim, vCross);
    fAlong = fAlong - fHoleDist;
    if (vCross[1] < 0.0f) {
        fSign = 1.0f;
    } else {
        fSign = -1.0f;
    }
    fSide = fSide * fSign;
    *pLong = 3.0f * fAlong;
    *pSide = 3.0f * fSide;
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
