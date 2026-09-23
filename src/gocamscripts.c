// gocamscripts.c (TW06's golf/cameras/gocamscripts.c; the CameraScript_ / CamScript_ functions): the
// camera scripts. A view's script moves the camera from shot to shot (the shots and sequences
// GoDynamicCam.c picks), keeps it above the ground and on the fairway, and aims it at the ball
// and the pin. The unit covers the file's certain core; its edges are still open.

#include "golfer.h"
#include "game.h"
#include "camera.h"

void fn_80045428(f32* pA, f32* pB, f32* pOut);  // pOut = pA - pB (paired singles)

// Raises pPos by fUp and moves it fSide sideways, square to the line from pTarget to it.
void fn_8004255C(f32* pPos, f32* pTarget, f32 fUp, f32 fSide) {
    f32 vDir[4];

    pPos[1] += fUp;
    fn_80045428(pPos, pTarget, vDir);
    if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
        fn_800BAF04(vDir, vDir);
    }
    pPos[0] += fSide * -vDir[2];
    pPos[2] += fSide * vDir[0];
}

// The script's shot, or else its next one (unless the next kind is 5), has bAC 0 or 13 while the
// ball makes no update this frame.
u8 fn_80043920(CamScript* pScript, int nPlayer) {
    if (pScript->pShot == NULL) {
        return 0;
    }
    if ((pScript->pShot->bAC == 0 || pScript->pShot->bAC == 13)
        && GameEffects_BallUpdatesThisFrame(nPlayer) < 1) {
        return 1;
    }
    if (pScript->pNextShot == NULL || pScript->nBC == 5) {
        return 0;
    }
    if ((pScript->pNextShot->bAC == 0 || pScript->pNextShot->bAC == 13)
        && GameEffects_BallUpdatesThisFrame(nPlayer) < 1) {
        return 1;
    }
    return 0;
}

// The pin, when pPos is near no AI target: pOut gets the nearest target, or the current pin
// position of the hole.
void fn_80044768(f32* pPos, f32* pOut) {
    CourseInfo* pCourse = fn_8000C594();

    if (pCourse != NULL && AI_NearestTarget(pPos, pOut) < 0) {
        int nPin = Game_CurrentPinSet();

        Vec3Copy(&pCourse->pin[nPin].x, pOut);
    }
}

// ---- sweep code (not yet cleaned up) ----

s32 fn_80044AA8(void* arg0);

s32 fn_80044AA8(void* arg0) {
    u32 temp_r0;

    if (arg0 == NULL) {
        return 0;
    }
    temp_r0 = (*(u32*)((u8*)(arg0) + 0x2C));
    if (temp_r0 == 0xAU) {
        return 0;
    }
    if (temp_r0 == 9U) {
        return 0;
    }
    if ((temp_r0 >= 1U) && (temp_r0 <= 0xCU)) {
        return 1;
    }
    if (temp_r0 == 0x12U) {
        return 1;
    }
    return 0;
}

// ---- end of sweep code ----

// The highest of the nCount heights that is not above fMax, or TER_NO_GROUND.
f32 fn_80044B0C(f32* pHeights, u32 nCount, f32 fMax) {
    f32 fBest = -10000000.0f;
    u8 bFound = 0;
    u32 i;

    if (nCount == 0) return TER_NO_GROUND;
    for (i = 0; i < nCount; i++) {
        if (pHeights[i] >= fBest && pHeights[i] <= fMax) {
            fBest = pHeights[i];
            bFound = 1;
        }
    }
    if (bFound) return fBest;
    return TER_NO_GROUND;
}

// The lowest of the nCount heights that is not below fMin, or TER_NO_GROUND.
f32 fn_80044B70(f32* pHeights, u32 nCount, f32 fMin) {
    f32 fBest = 10000000.0f;
    u8 bFound = 0;
    u32 i;

    if (nCount == 0) return TER_NO_GROUND;
    for (i = 0; i < nCount; i++) {
        if (pHeights[i] <= fBest && pHeights[i] >= fMin) {
            fBest = pHeights[i];
            bFound = 1;
        }
    }
    if (bFound) return fBest;
    return TER_NO_GROUND;
}

// n is 149 on course 7's hole 2.
u8 fn_80044E2C(int n) {
    if (n == 149 && Game_GetCourse() == 7 && fn_80015464() == 2) {
        return 1;
    }
    return 0;
}

// The shot is of kind 6..10.
u8 fn_80044E74(CamShot* pShot) {
    if (pShot == NULL) return 0;
    if (pShot->bAD >= 6 && pShot->bAD <= 10) {
        return 1;
    }
    return 0;
}

// How much of the way from the shot's start to the pin the ball has covered, over the ground
// (0 at the start, 1 at the pin); 0 without a hole loaded.
f32 fn_80044F58(int nPlayer) {
    CourseInfo* pCourse = fn_8000C594();
    f32* pPin;
    f32 vStart[4];
    f32 vBall[4];
    f32 fStart;
    f32 fBall;

    if (pCourse == NULL) return 0.0f;
    pPin = &pCourse->pin[Game_CurrentPinSet()].x;
    fn_80045428(pPin, gPlayers[nPlayer].ball.vStart, vStart);
    fn_80045428(pPin, gPlayers[nPlayer].ball.vPos, vBall);
    vStart[1] = 0.0f;
    vBall[1] = 0.0f;
    fStart = fn_80009680(fn_80009744(vStart));
    fBall = fn_80009680(fn_80009744(vBall));
    if (fStart > 0.0f) {
        return 1.0f - fBall / fStart;
    }
    return 1.0f;
}

u8 fn_800453C8(int nPlayer, CamShot* pShot) {
    if (gSession.nGameType == 3) {
        return 0;
    }
    return fn_8001EDF4(gPlayers[nPlayer].pChar) != 0;
}

// a - b into out (three floats)
#ifdef __MWERKS__
asm void fn_80045428(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_80045428(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// a + b into out (three floats)
#ifdef __MWERKS__
asm void fn_8004544C(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_8004544C(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

void fn_80045470(CamLens* pLens, f32 fFov) {
    pLens->fFov = fFov;
    fn_800763BC(pLens);
}

// The quarter-speed slow motion (GameEffects.b11) on or off, with its sound events (0x35 on,
// 0x36 off); every second of its frames moves the ball.
void fn_80045494(u8 bOn, int nPlayer) {
    lbl_80202898.n2C = 2;
    if (bOn) {
        if (!lbl_80202898.b11) {
            EVENT_Trigger(nPlayer, 0x35, gPlayers[nPlayer].vBall, -1);
            lbl_80202898.b11 = bOn;
            lbl_80202898.n28 = 0;
        }
    } else if (lbl_80202898.b11) {
        EVENT_Trigger(nPlayer, 0x36, gPlayers[nPlayer].vBall, -1);
        lbl_80202898.b11 = bOn;
    }
}

// The half-speed slow motion (GameEffects.b10) on or off, with its sound events (0x37 on, 0x38 off).
void fn_80045558(u8 bOn, int nPlayer) {
    if (bOn) {
        if (!lbl_80202898.b10) {
            EVENT_Trigger(nPlayer, 0x37, gPlayers[nPlayer].vBall, -1);
            lbl_80202898.b10 = bOn;
        }
    } else if (lbl_80202898.b10) {
        EVENT_Trigger(nPlayer, 0x38, gPlayers[nPlayer].vBall, -1);
        lbl_80202898.b10 = bOn;
    }
}

// ---- sweep code (not yet cleaned up) ----

u8 fn_8004561C(void);

u8 fn_8004560C(void) {
    return lbl_80202898.bGameBreaker;
}

u8 fn_8004561C(void) {
    return lbl_80202898.b10;
}

u8 fn_8004562C(CamShot* pShot) {
    u8 nKind = pShot->bAC;

    if (nKind == 0 || (u8)(nKind - 13) <= 2U || nKind == 23) {
        return 1;
    }
    return 0;
}

// ---- end of sweep code ----
