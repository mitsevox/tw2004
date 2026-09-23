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

// ---- sweep code (not yet cleaned up) ----

s32 fn_80043920(void* arg0, s32 arg1);

s32 fn_80043920(void* arg0, s32 arg1) {
    u8 temp_r0;
    u8 temp_r0_2;
    void* temp_r3;
    void* temp_r3_2;

    temp_r3 = (*(void**)((u8*)(arg0) + 0xAC));
    if (temp_r3 == NULL) {
        return 0;
    }
    temp_r0 = (*(u8*)((u8*)(temp_r3) + 0xAC));
    if (((temp_r0 == 0) || (temp_r0 == 0xD)) && (GameEffects_BallUpdatesThisFrame(arg1) < 1)) {
        return 1;
    }
    temp_r3_2 = (*(void**)((u8*)(arg0) + 0xB0));
    if ((temp_r3_2 == NULL) || ((s32) (*(s32*)((u8*)(arg0) + 0xBC)) == 5)) {
        return 0;
    }
    temp_r0_2 = (*(u8*)((u8*)(temp_r3_2) + 0xAC));
    if (((temp_r0_2 == 0) || (temp_r0_2 == 0xD)) && (GameEffects_BallUpdatesThisFrame(arg1) < 1)) {
        return 1;
    }
    return 0;
}

// ---- end of sweep code ----

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
