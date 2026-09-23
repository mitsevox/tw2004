// emotion.c (TW06's emotion.c, golf/ai/emotion.c): the golfers' emotions. After each shot a
// player's PlayerEmotion (golfer.h) records how the shot turned out, and that picks the reaction
// the golfer plays.

#include "game.h"
#include "physics.h"

int  fn_8006AA70(int nPlayer);
int  fn_8006AA84(int nPlayer);
void fn_8006ACE0(int nPlayer, int nResult);
void fn_8006AE50(int nPlayer);
void fn_8006B0B8(int nPlayer);
void fn_8006B250(int nPlayer);
void fn_8006B57C(int nPlayer);
void fn_8006B6D0(int nPlayer);
void fn_8006B6F0(int nPlayer);
void fn_8006B87C(int nPlayer);
u8   fn_8006BAD8(int nPlayer, s32* pOut);
void fn_8006BB5C(int nPlayer);
u8   fn_8006BDC8(int nPlayer, u8 bBefore);
void fn_8006BE80(f32* pA, f32* pB, f32* pOut);
u8   fn_8006BEA4(void);

int fn_8006AA70(int nPlayer) {
    return lbl_801D5F78[nPlayer].n0;
}

int fn_8006AA84(int nPlayer) {
    return lbl_801D5F78[nPlayer].n4;
}

int fn_8006AA9C(int nPlayer) {
    return lbl_801D5F78[nPlayer].n14;
}

// Records how a shot turned out. The mode is asked about the shot with the hole's stroke count
// one lower; if it says so and the ball is in the cup, the outcome becomes 2.
void fn_8006AAB4(int nPlayer, int nResult) {
    gPlayers[nPlayer].nStrokes[gpGame->nCurHole]--;
    if (gpGame->pfn1FC(nPlayer) && gPlayers[nPlayer].ball.nLie == LIE_INCUP_e) {
        nResult = 2;
    }
    gPlayers[nPlayer].nStrokes[gpGame->nCurHole]++;
    lbl_801D5F78[nPlayer].n8 = nResult;

    switch (nResult) {
    case 0:
    case 1:
    case 2:
        lbl_801D5F78[nPlayer].n0 = 0;
        break;
    case 3:
    case 4:
    case 6:
    case 7:
        lbl_801D5F78[nPlayer].n0 = 1;
        break;
    }

    switch (nResult) {
    case 5:
        lbl_801D5F78[nPlayer].n4 = 0;
        break;
    case 0:
        lbl_801D5F78[nPlayer].n4 = 1;
        break;
    case 1:
        lbl_801D5F78[nPlayer].n4 = 2;
        break;
    case 2:
        lbl_801D5F78[nPlayer].n4 = 3;
        break;
    }

    lbl_801D5F78[nPlayer].n14 = lbl_801D5F78[nPlayer].n8;
    lbl_801D5F78[nPlayer].nC = lbl_801D5F78[nPlayer].n0;
    lbl_801D5F78[nPlayer].n10 = lbl_801D5F78[nPlayer].n4;

    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)
            && gPlayers[nPlayer].ball.nLie == LIE_INCUP_e) {
        lbl_801D5F78[nPlayer].n8 = 2;
        lbl_801D5F78[nPlayer].n14 = 2;
        lbl_801D5F78[nPlayer].nC = 0;
        lbl_801D5F78[nPlayer].n10 = 3;
        lbl_801D5F78[nPlayer].n0 = 0;
        lbl_801D5F78[nPlayer].n4 = 3;
    }
}

void fn_8006ACE0(int nPlayer, int nResult) {
    lbl_801D5F78[nPlayer].n14 = nResult;
}

// Sets the reaction to play. Reaction 1 does not replace reaction 2; a change to anything but
// 4 or 5 updates the golfer's emotion at once.
void fn_8006ACF8(int nPlayer, int a) {
    int nOld = lbl_801D5F78[nPlayer].n18;

    if (a == 1) {
        if (lbl_801D5F78[nPlayer].n18 != 2) {
            lbl_801D5F78[nPlayer].n18 = a;
        }
    } else {
        lbl_801D5F78[nPlayer].n18 = a;
    }
    if (a != 4 && a != 5 && nOld != lbl_801D5F78[nPlayer].n18) {
        Emotion_UpdatePlayerEmotion(nPlayer);
    }
}

// Works out the golfer's emotion for his current state; a change of n0 or n4 raises event 42.
void Emotion_UpdatePlayerEmotion(int nPlayer) {
    s8 nState = GOLFERSTATE_GetCurrentState(nPlayer);   // fake match: the s8 (see game.h)
    int nWas0 = lbl_801D5F78[nPlayer].n0;
    int nWas4 = lbl_801D5F78[nPlayer].n4;

    switch (nState) {
    case GS_PRE_SHOT:
    case GS_SHOT_SETUP:
    case GS_ZOOM:
    case GS_ELEVATOR:
    case GS_GREEN:
    case GS_KNEE_CAM:
        fn_8006AE50(nPlayer);
        break;
    case GS_SWING:
    case GS_TAP_IN:
        fn_8006B0B8(nPlayer);
        break;
    case GS_SIMULATE:
        fn_8006B250(nPlayer);
        break;
    case GS_IN_THE_HOLE:
    case GS_SHOW_YARDAGE:
    case GS_REMOVE_BALL:
        break;
    case GS_NONE:
    case GS_GREEN_WATCH_ROLL:
    case GS_GREEN_REVERSE_PUTT:
    case GS_GREEN_MORPH:
    case GS_REPLAY_SWING:
    case GS_FADE_TO_TAP_IN:
    case GS_FADE_TO_REMOVE_BALL:
    case GS_WAIT:
    case GS_INITIAL_FLY_BY:
    case GS_MID_HOLE_FLY_BY:
    case GS_PLACE_BALL:
    default:
        lbl_801D5F78[nPlayer].nC = 4;
        lbl_801D5F78[nPlayer].n10 = 0;
        lbl_801D5F78[nPlayer].n14 = 5;
        lbl_801D5F78[nPlayer].n0 = 4;
        lbl_801D5F78[nPlayer].n4 = 0;
        lbl_801D5F78[nPlayer].n8 = 5;
        break;
    }
    if (nWas0 != lbl_801D5F78[nPlayer].n0 || nWas4 != lbl_801D5F78[nPlayer].n4) {
        EVENT_Trigger(nPlayer, 42, NULL, -1);
    }
}

// Before the shot: the emotion from the ball's lie.
void fn_8006AE50(int nPlayer) {
    switch (gPlayers[nPlayer].ball.nLie) {
    case LIE_TEE_e:
        lbl_801D5F78[nPlayer].n0 = 2;
        lbl_801D5F78[nPlayer].n4 = 1;
        break;
    case LIE_FAIRWAY_e:
    case LIE_FAIRWAY_TIGHT_e:
        lbl_801D5F78[nPlayer].n0 = 2;
        lbl_801D5F78[nPlayer].n4 = 1;
        break;
    case LIE_ROUGH_HIGH_e:
        lbl_801D5F78[nPlayer].n0 = 2;
        lbl_801D5F78[nPlayer].n4 = 2;
        break;
    case LIE_ROUGH_e:
        lbl_801D5F78[nPlayer].n0 = 2;
        lbl_801D5F78[nPlayer].n4 = 2;
        break;
    case LIE_THICK_ROUGH_e:
        lbl_801D5F78[nPlayer].n0 = 2;
        lbl_801D5F78[nPlayer].n4 = 2;
        break;
    case LIE_SAND_HIGH_e:
        lbl_801D5F78[nPlayer].n0 = 2;
        lbl_801D5F78[nPlayer].n4 = 2;
        break;
    case LIE_SAND_MEDIUM_e:
        lbl_801D5F78[nPlayer].n0 = 2;
        lbl_801D5F78[nPlayer].n4 = 2;
        break;
    case LIE_SAND_DEEP_e:
        lbl_801D5F78[nPlayer].n0 = 2;
        lbl_801D5F78[nPlayer].n4 = 2;
        break;
    case LIE_GREEN_e:
        if (gpGame->pfn1FC(nPlayer)) {
            lbl_801D5F78[nPlayer].n0 = 2;
            lbl_801D5F78[nPlayer].n4 = 3;
        } else if (gpGame->pfn1F8(nPlayer)) {
            lbl_801D5F78[nPlayer].n0 = 2;
            lbl_801D5F78[nPlayer].n4 = 2;
        } else {
            lbl_801D5F78[nPlayer].n0 = 2;
            lbl_801D5F78[nPlayer].n4 = 1;
        }
        break;
    case LIE_FRINGE_e:
        lbl_801D5F78[nPlayer].n0 = 2;
        lbl_801D5F78[nPlayer].n4 = 1;
        break;
    case LIE_CARTPATH_e:
        lbl_801D5F78[nPlayer].n0 = 2;
        lbl_801D5F78[nPlayer].n4 = 1;
        break;
    default:
        lbl_801D5F78[nPlayer].n0 = 2;
        lbl_801D5F78[nPlayer].n4 = 0;
        break;
    }
}

// Over the ball: the emotion from the distance to the target (ignoring height). A GameBreaker lie
// is always 2.
void fn_8006B0B8(int nPlayer) {
    f32 vToTarget[3];
    f32 fDist;

    if ((u8)fn_800DB86C(nPlayer)) {     // fake match: the (u8), the caller tests only the low byte
        lbl_801D5F78[nPlayer].n0 = 2;
        lbl_801D5F78[nPlayer].n4 = 2;
        return;
    }
    fn_8006BE80(gPlayers[nPlayer].vBall, gPlayers[nPlayer].vTarget, vToTarget);
    vToTarget[1] = 0.0f;
    fDist = fn_80009680(fn_80009744(vToTarget));
    lbl_801D5F78[nPlayer].n0 = 2;
    switch (gPlayers[nPlayer].nShotKind) {
    case 0:
        if (fDist > 20.0f) {
            lbl_801D5F78[nPlayer].n4 = 2;
        } else if (fDist > 10.0f) {
            lbl_801D5F78[nPlayer].n4 = 1;
        } else {
            lbl_801D5F78[nPlayer].n4 = 0;
        }
        break;
    case 1:
        if (fDist > 250.0f) {
            lbl_801D5F78[nPlayer].n4 = 2;
        } else if (fDist > 100.0f) {
            lbl_801D5F78[nPlayer].n4 = 1;
        } else {
            lbl_801D5F78[nPlayer].n4 = 0;
        }
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        lbl_801D5F78[nPlayer].n4 = 0;
        break;
    case 7:
        lbl_801D5F78[nPlayer].n4 = 2;
        break;
    default:
        lbl_801D5F78[nPlayer].n4 = 0;
        break;
    }
}

void fn_8006B250(int nPlayer) {
    switch (lbl_801D5F78[nPlayer].n18) {
    case 0:
        fn_8006B57C(nPlayer);
        break;
    case 1:
        fn_8006B6D0(nPlayer);
        break;
    case 2:
        fn_8006B6F0(nPlayer);
        break;
    case 3:
        fn_8006B87C(nPlayer);
        break;
    }
}

// The shot's outcome from where the ball ended (bBefore: from the look-ahead ball, and only the
// kept copy nC, n10, n14 is set): out of bounds or a mode's special case 4, a GameBreaker 2,
// anything else 5.
void fn_8006B2C4(int nPlayer, u8 bBefore) {
    Ball* pBall;

    if (Game_GetMode() == 11) {
        lbl_801D5F78[nPlayer].n4 = 0;
        lbl_801D5F78[nPlayer].n10 = 0;
        lbl_801D5F78[nPlayer].n0 = 4;
        lbl_801D5F78[nPlayer].nC = 4;
        lbl_801D5F78[nPlayer].n8 = 5;
        lbl_801D5F78[nPlayer].n14 = 5;
        return;
    }
    if (bBefore) {
        pBall = &gPlayers[nPlayer].ballBefore;
    } else {
        pBall = &gPlayers[nPlayer].ball;
    }
    if (fn_800E2B40(nPlayer, pBall)) {
        if (bBefore) {
            lbl_801D5F78[nPlayer].n10 = 3;
            lbl_801D5F78[nPlayer].nC = 1;
            lbl_801D5F78[nPlayer].n14 = 4;
            return;
        }
        lbl_801D5F78[nPlayer].n4 = 3;
        lbl_801D5F78[nPlayer].n10 = 3;
        lbl_801D5F78[nPlayer].n0 = 1;
        lbl_801D5F78[nPlayer].nC = 1;
        lbl_801D5F78[nPlayer].n8 = 4;
        lbl_801D5F78[nPlayer].n14 = 4;
        return;
    }
    if (fn_8006BDC8(nPlayer, bBefore)) {
        if (bBefore) {
            lbl_801D5F78[nPlayer].n10 = 3;
            lbl_801D5F78[nPlayer].nC = 1;
            lbl_801D5F78[nPlayer].n14 = 4;
            return;
        }
        lbl_801D5F78[nPlayer].n4 = 3;
        lbl_801D5F78[nPlayer].n10 = 3;
        lbl_801D5F78[nPlayer].n0 = 1;
        lbl_801D5F78[nPlayer].nC = 1;
        lbl_801D5F78[nPlayer].n8 = 4;
        lbl_801D5F78[nPlayer].n14 = 4;
        return;
    }
    if (fn_8004560C() && (!fn_8006BEA4() || fn_800DC818(pBall, nPlayer, bBefore))) {
        if (bBefore) {
            lbl_801D5F78[nPlayer].n10 = 3;
            lbl_801D5F78[nPlayer].nC = 0;
            lbl_801D5F78[nPlayer].n14 = 2;
            return;
        }
        lbl_801D5F78[nPlayer].n4 = 3;
        lbl_801D5F78[nPlayer].n10 = 3;
        lbl_801D5F78[nPlayer].n0 = 0;
        lbl_801D5F78[nPlayer].nC = 0;
        lbl_801D5F78[nPlayer].n8 = 2;
        lbl_801D5F78[nPlayer].n14 = 2;
        return;
    }
    if (bBefore) {
        lbl_801D5F78[nPlayer].n10 = 0;
        lbl_801D5F78[nPlayer].nC = 4;
        lbl_801D5F78[nPlayer].n14 = 5;
        return;
    }
    lbl_801D5F78[nPlayer].n4 = 0;
    lbl_801D5F78[nPlayer].n10 = 0;
    lbl_801D5F78[nPlayer].n0 = 4;
    lbl_801D5F78[nPlayer].nC = 4;
    lbl_801D5F78[nPlayer].n8 = 5;
    lbl_801D5F78[nPlayer].n14 = 5;
}

// Reaction 0: from how the swing's power compares with the power the target asked for.
void fn_8006B57C(int nPlayer) {
    f32 fWanted = AI_PowerForTarget(nPlayer);
    f32 fMiss;

    if (fWanted > 0.95f && fn_8005B64C(nPlayer) > 1.0f && fn_8005B64C(nPlayer) > 1.0f) {
        lbl_801D5F78[nPlayer].n0 = 0;
        lbl_801D5F78[nPlayer].n4 = 2;
        return;
    }
    fMiss = fabsf(fWanted - fn_8005B64C(nPlayer));
    if (fMiss < 0.2f) {
        lbl_801D5F78[nPlayer].n0 = 0;
        lbl_801D5F78[nPlayer].n4 = 1;
    } else if (fMiss < 0.3f) {
        lbl_801D5F78[nPlayer].n0 = 4;
        lbl_801D5F78[nPlayer].n4 = 0;
    } else if (fMiss < 0.4f) {
        lbl_801D5F78[nPlayer].n0 = 1;
        lbl_801D5F78[nPlayer].n4 = 1;
    } else {
        lbl_801D5F78[nPlayer].n0 = 1;
        lbl_801D5F78[nPlayer].n4 = 2;
    }
}

// Reaction 1.
void fn_8006B6D0(int nPlayer) {
    lbl_801D5F78[nPlayer].n0 = 1;
    lbl_801D5F78[nPlayer].n4 = 1;
}

// Reaction 2: from the surface class under the ball.
void fn_8006B6F0(int nPlayer) {
    SurfaceType* pSurface = fn_800CC190(fn_8000C594(), gPlayers[nPlayer].ball.vPos);

    if (pSurface == NULL) {
        lbl_801D5F78[nPlayer].n0 = 4;
        lbl_801D5F78[nPlayer].n4 = 0;
    } else if (pSurface->nClass == 5) {
        lbl_801D5F78[nPlayer].n0 = 1;
        lbl_801D5F78[nPlayer].n4 = 0;
    } else if (pSurface->nClass == 6) {
        lbl_801D5F78[nPlayer].n0 = 1;
        lbl_801D5F78[nPlayer].n4 = 1;
    } else if (pSurface->nClass == 3) {
        lbl_801D5F78[nPlayer].n0 = 0;
        lbl_801D5F78[nPlayer].n4 = 1;
    } else if (pSurface->nClass == 11) {
        lbl_801D5F78[nPlayer].n0 = 1;
        lbl_801D5F78[nPlayer].n4 = 1;
    } else if (pSurface->nClass == 4) {
        lbl_801D5F78[nPlayer].n0 = 0;
        lbl_801D5F78[nPlayer].n4 = 0;
    } else if (pSurface->nClass == 2) {
        lbl_801D5F78[nPlayer].n0 = 0;
        lbl_801D5F78[nPlayer].n4 = 0;
    } else {
        lbl_801D5F78[nPlayer].n0 = 4;
        lbl_801D5F78[nPlayer].n4 = 0;
    }
}

// Reaction 3: from how straight the ball went, the cosine between the aim and where it ended
// (both flat): straighter moves n4 up, wider moves it down and may flip n0 between 0 and 1.
void fn_8006B87C(int nPlayer) {
    f32 vAim[4];
    f32 vShot[4];
    f32 fDot;
    int nLevel;

    fn_8006BE80(gPlayers[nPlayer].vTarget, gPlayers[nPlayer].vBall, vAim);
    fn_8006BE80(gPlayers[nPlayer].ball.vPos, gPlayers[nPlayer].vBall, vShot);
    vAim[1] = 0.0f;
    vShot[1] = 0.0f;
    if (0.0f != vAim[0] && 0.0f != vAim[2]) {
        fn_800BAF04(vAim, vAim);
    }
    if (0.0f != vShot[0] && 0.0f != vShot[2]) {
        fn_800BAF04(vShot, vShot);
    }
    fDot = fn_8000C5FC(vAim, vShot);
    nLevel = lbl_801D5F78[nPlayer].n4;
    if (lbl_801D5F78[nPlayer].n0 == 0) {
        if (fDot > 0.9f) {
            nLevel++;
        } else if (!(fDot > 0.8f)) {
            if (fDot > 0.7f) {
                nLevel--;
            } else {
                nLevel -= 2;
            }
        }
        if (nLevel > 2) {
            lbl_801D5F78[nPlayer].n4 = 2;
        } else if (nLevel < 0) {
            lbl_801D5F78[nPlayer].n0 = 1;
            lbl_801D5F78[nPlayer].n4 = -nLevel;
        } else {
            lbl_801D5F78[nPlayer].n4 = nLevel;
        }
    } else if (lbl_801D5F78[nPlayer].n0 == 1) {
        if (fDot > 0.9f) {
            nLevel++;
        } else if (!(fDot > 0.8f)) {
            if (fDot > 0.7f) {
                nLevel--;
            } else {
                nLevel -= 2;
            }
        }
        if (nLevel > 2) {
            lbl_801D5F78[nPlayer].n4 = 2;
        } else if (nLevel < 0) {
            lbl_801D5F78[nPlayer].n0 = 0;
            lbl_801D5F78[nPlayer].n4 = -nLevel;
        } else {
            lbl_801D5F78[nPlayer].n4 = nLevel;
        }
    } else if (fDot > 0.9f) {
        lbl_801D5F78[nPlayer].n0 = 0;
        lbl_801D5F78[nPlayer].n4 = 0;
    } else if (!(fDot > 0.8f)) {
        if (fDot > 0.7f) {
            lbl_801D5F78[nPlayer].n0 = 1;
            lbl_801D5F78[nPlayer].n4 = 0;
        } else {
            lbl_801D5F78[nPlayer].n0 = 1;
            lbl_801D5F78[nPlayer].n4 = 1;
        }
    }
}

void fn_8006BAA8(int nPlayer) {
    lbl_801D5F78[nPlayer].b1C = 0;
    lbl_801D5F78[nPlayer].b1D = 0;
    lbl_801D5F78[nPlayer].b1E = 0;
    lbl_801D5F78[nPlayer].b1F = 0;
    lbl_801D5F78[nPlayer].n20 = -1;
}

u8 fn_8006BAD8(int nPlayer, s32* pOut) {
    PlayerEmotion* pEmotion = &lbl_801D5F78[nPlayer];
    u8 bResult = 0;
    int n;

    if (!pEmotion->b1C) {
        *pOut = -1;
    } else {
        if (pEmotion->n20 != -1) {
            n = 3 - pEmotion->n20;
        } else {
            n = 0;
        }
        if (pEmotion->b1D) {
            *pOut = n;
            bResult = 1;
        } else if (pEmotion->b1E) {
            *pOut = n;
            pEmotion->b1E = 0;
        } else {
            *pOut = -1;
        }
    }
    return bResult;
}

// Every frame of the shot: while the ball flies, fn_800A6FE0 runs once it comes within 40 of the
// pin and fn_800A707C once it is out again; while it rolls near the pin it rates the putt by its
// closest approach (n20, a band from aBands) and notes when it stops heading for the hole.
void fn_8006BB5C(int nPlayer) {
    PlayerEmotion* pEmotion = &lbl_801D5F78[nPlayer];
    Ball* pBall = &gPlayers[nPlayer].ball;
    CourseInfo* pCourse = fn_8000C594();
    f32 vToPin[4];
    f32 aBands[4] = {0.5f, 2.0f, 4.5f, 9.3f};
    f32 vVel[4];
    f32 fClosest;
    u8 bNoBreak;
    int nBand;

    if (pCourse == NULL || pEmotion->b1D) return;

    fn_8006BE80(&pCourse->pin[Game_CurrentPinSet()].x, pBall->vPos, vToPin);
    pEmotion->b1C = 0;
    fClosest = pBall->fClosest;
    switch (pBall->nState) {
    case PHYSICS_BALLSTATE_BallRollingAndSlipping_e:
    case PHYSICS_BALLSTATE_BallPureRolling_e:
        if (fClosest < 9.3f && fClosest > 3.5f / 36.0f && pBall->fSpeed > 0.0f) {
            bNoBreak = pEmotion->nC == 0 && pEmotion->n10 >= 1;
            pEmotion->b1C = 1;
            fn_8001EF34(vToPin, 1.0f / fClosest, vToPin);
            fn_8001EF34(pBall->vVel, 1.0f / ((12.0f * (3.0f * pBall->fSpeed)) / 60.0f), vVel);
            if (vToPin[0] * vVel[0] + vToPin[2] * vVel[2] < 0.866f && !bNoBreak) {
                pEmotion->b1D = 1;
                return;
            }
            nBand = 3;
            if (fClosest < aBands[0]) {
                nBand = 0;
            } else if (fClosest < aBands[1]) {
                nBand = 1;
            } else if (fClosest < aBands[2]) {
                nBand = 2;
            }
            if (nBand < 0) {
                nBand = 0;
            }
            if (nBand > 3) {
                nBand = 3;
            }
            if (nBand != pEmotion->n20) {
                pEmotion->b1E = 1;
            }
            pEmotion->n20 = nBand;
        }
        break;
    case PHYSICS_BALLSTATE_BallFlying_e:
        if (!pEmotion->b1F) {
            if (fClosest < 40.0f) {
                pEmotion->b1F = 1;
                fn_800A6FE0();
            }
        } else if (fClosest >= 40.0f) {
            fn_800A707C();
            pEmotion->b1F = 0;
        }
        break;
    }
}

// With bBefore, whether the shot about to be taken would count for the mode (fn_800E23B0 with one
// more stroke), unless the ball is already in the cup; without it, the player's bC2D.
u8 fn_8006BDC8(int nPlayer, u8 bBefore) {
    if (bBefore) {
        if (gPlayers[nPlayer].ballBefore.nLie != LIE_INCUP_e) {
            return fn_800E23B0(nPlayer, gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1) != 0;
        }
        return 0;
    }
    return gPlayers[nPlayer].bC2D != 0;
}

// a - b into out (three floats)
#ifdef __MWERKS__
asm void fn_8006BE80(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_8006BE80(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// A scripted GameBreaker is on screen.
u8 fn_8006BEA4(void) {
    return lbl_80202898.bGameBreaker && lbl_80202898.nGBType == 0;
}
