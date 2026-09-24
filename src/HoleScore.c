// HoleScore.c (our name; TW06's golf/gamemode/analysisutilities.c, medium evidence: TW06 names
// Hole_ScoreAfterTapIn GameAnalysis_IsPuttFor): per-player round analysis for the situation
// scripts, the earnings and the game modes: distances to the pin, the lie the shot started from,
// and counts and streaks of holes by score against par.

#include "golfer.h"
#include "game.h"

f32  fn_800D04AC(int nPlayer);
f32  fn_800D04E0(int nPlayer);
int  fn_800D0514(int nPlayer);
f32  fn_800D05A4(f32* pPos);
u32  fn_800D0BAC(int nPlayer);
u8   fn_800D0D54(int nPlayer);
int  fn_800D0DC8(int nPlayer, int nToPar);
int  fn_800D0E74(int nPlayer);
int  fn_800D0F04(int nPlayer, int nToPar);
int  fn_800D0FBC(int nPlayer);
int  fn_800D10B0(int nPlayer);
int  fn_800D1170(int nPlayer, u8 bOnlyFlagged);
int  fn_800D1250(int nPlayer);
int  fn_800D1330(int nPlayer);
void fn_800D1674(f32* pA, f32* pB, f32* pOut);
void fn_800C8C3C(int nView, f32* pOut);   // GoBreakLine: a point kept per view

// For a human player: whether Player.ballBefore passes the fn_800D782C check or, when that ball
// is in the cup, the fn_800D7B1C putt check.
u8 fn_800CF77C(int nPlayer) {
    if (Player_IsCPU(nPlayer)) {
        return 0;
    }
    if (fn_800D782C(nPlayer, &gPlayers[nPlayer].ballBefore, 0, 1, 1) != 0) {
        return 1;
    }
    if (gPlayers[nPlayer].ballBefore.nLie == LIE_INCUP_e &&
        fn_800D7B1C(nPlayer, &gPlayers[nPlayer].ballBefore, 0, 1, 1) != 0) {
        return 1;
    }
    return 0;
}

// As fn_800CF77C, with the checks fn_800D7660 and fn_800D7684.
u8 fn_800CF848(int nPlayer) {
    if (Player_IsCPU(nPlayer)) {
        return 0;
    }
    if (fn_800D7660(nPlayer, &gPlayers[nPlayer].ballBefore, 1) != 0) {
        return 1;
    }
    if (gPlayers[nPlayer].ballBefore.nLie == LIE_INCUP_e &&
        fn_800D7684(nPlayer, &gPlayers[nPlayer].ballBefore, 1) != 0) {
        return 1;
    }
    return 0;
}

// The player's score against par for the round once the tap-in on this hole drops; 0 when
// fn_8008AB40 is set.
int fn_800CFFE4(int nPlayer) {
    int nPar = 0;
    int nStrokes = 0;
    int i;
    if (fn_8008AB40()) {
        return 0;
    }
    for (i = 0; i < Game_CurHoleIndex(); i++) {
        nPar += fn_800D2AD8(i);
        nStrokes += gPlayers[nPlayer].nStrokes[i];
    }
    return (nStrokes - nPar) + gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1 - fn_800D2B08();
}

// Whether holing the ball now would finish the hole: puts the ball in the cup with one more
// stroke, asks the mode (pfnHoleFinished, only asking), then puts both back.
u8 fn_800D024C(int nPlayer) {
    int nLie;
    int nStrokes;
    int bFinished;

    nLie = gPlayers[nPlayer].ball.nLie;
    gPlayers[nPlayer].ball.nLie = LIE_INCUP_e;
    nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()]++;
    lbl_80282240 = 1;
    bFinished = gpGame->pfnHoleFinished(nPlayer, 1) != 0;
    lbl_80282240 = 0;
    gPlayers[nPlayer].ball.nLie = nLie;
    gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] = nStrokes;
    return bFinished;
}

// The ball's distance from the pin: where it lies, where the shot started, and where it lay before
// the shot.
f32 fn_800D0478(int nPlayer) {
    return fn_800D05A4(gPlayers[nPlayer].ball.vPos);
}

f32 fn_800D04AC(int nPlayer) {
    return fn_800D05A4(gPlayers[nPlayer].ball.vStart);
}

f32 fn_800D04E0(int nPlayer) {
    return fn_800D05A4(gPlayers[nPlayer].ballBefore.vPos);
}

// The class (SurfaceType.nClass) of the surface under the lying ball before the shot, or -1.
int fn_800D0514(int nPlayer) {
    int nSurface = gPlayers[nPlayer].ballBefore.nSurface;
    if (nSurface >= 0) {
        return gSurfaceTypes[nSurface].nClass;
    }
    return -1;
}

// The shot's length along the ground (the height left out).
f32 fn_800D0550(int nPlayer) {
    f32 vDiff[3];
    fn_800D1674(gPlayers[nPlayer].ball.vPos, gPlayers[nPlayer].ball.vStart, vDiff);
    vDiff[1] = 0.0f;
    return fn_80009680(fn_80009744(vDiff));
}

// A point's distance from the current pin along the ground; 0 with no hole loaded.
f32 fn_800D05A4(f32* pPos) {
    f32 vDiff[3];
    CourseInfo* pCourse = fn_8000C594();
    if (pCourse == NULL) return 0.0f;
    fn_800D1674(pPos, &pCourse->pin[Game_CurrentPinSet()].x, vDiff);
    vDiff[1] = 0.0f;
    return fn_80009680(fn_80009744(vDiff));
}

// The round's holes the player finished under par, counting back from the current hole (with
// bCurrent) or the one before; with bOnlyFlagged, only those whose gpGame->b16C entry is 1.
int fn_800D0620(int nPlayer, u8 bCurrent, u8 bOnlyFlagged) {
    int nCount = 0;
    int i;
    if (bCurrent) {
        i = Game_CurHoleIndex();
    } else {
        i = Game_CurHoleIndex() - 1;
    }
    for (; i >= 0; i--) {
        if (gpGame->bHoleSelected[i] && gPlayers[nPlayer].nStrokes[i] > 0 &&
            gPlayers[nPlayer].nStrokes[i] <= fn_800D2AD8(i) - 1 &&
            (gpGame->b16C[nPlayer][i] == 1 || !bOnlyFlagged)) {
            nCount++;
        }
    }
    return nCount;
}

// As fn_800D0620, two under par or better.
int fn_800D06FC(int nPlayer, u8 bCurrent, u8 bOnlyFlagged) {
    int nCount = 0;
    int i;
    if (bCurrent) {
        i = Game_CurHoleIndex();
    } else {
        i = Game_CurHoleIndex() - 1;
    }
    for (; i >= 0; i--) {
        if (gpGame->bHoleSelected[i] && gPlayers[nPlayer].nStrokes[i] > 0 &&
            gPlayers[nPlayer].nStrokes[i] <= fn_800D2AD8(i) - 2 &&
            (gpGame->b16C[nPlayer][i] == 1 || !bOnlyFlagged)) {
            nCount++;
        }
    }
    return nCount;
}

// The player's current run of holes under par: counting back from the current hole (with
// bCurrent) or the one before, until a hole that is not.
int fn_800D07D8(int nPlayer, u8 bCurrent) {
    int i;
    int nRun = 0;
    if (bCurrent) {
        i = Game_CurHoleIndex();
    } else {
        i = Game_CurHoleIndex() - 1;
    }
    for (; i >= 0; i--) {
        if (gpGame->bHoleSelected[i]) {
            if (gPlayers[nPlayer].nStrokes[i] <= 0 || gPlayers[nPlayer].nStrokes[i] > fn_800D2AD8(i) - 1) {
                break;
            }
            nRun++;
        }
    }
    return nRun;
}

// As fn_800D07D8, two under par or better.
int fn_800D089C(int nPlayer, u8 bCurrent) {
    int i;
    int nRun = 0;
    if (bCurrent) {
        i = Game_CurHoleIndex();
    } else {
        i = Game_CurHoleIndex() - 1;
    }
    for (; i >= 0; i--) {
        if (gpGame->bHoleSelected[i]) {
            if (gPlayers[nPlayer].nStrokes[i] <= 0 || gPlayers[nPlayer].nStrokes[i] > fn_800D2AD8(i) - 2) {
                break;
            }
            nRun++;
        }
    }
    return nRun;
}

// The angle (radians) at the start of the shot, along the ground, from the direction of the pin
// to that of the player's view's point (fn_800C8C3C); negative on one side.
f32 fn_800D0960(int nPlayer) {
    f32 vView[4];
    f32 vToPin[4];
    f32 vToView[4];
    f32 fCos;
    f32 fAngle;

    fn_800C8C3C(gPlayers[nPlayer].nView[0], vView);
    fn_800D1674(&gPlayers[nPlayer].ball.pCourse->pin[Game_CurrentPinSet()].x, gPlayers[nPlayer].ball.vStart,
                vToPin);
    vToPin[1] = 0.0f;
    fn_800D1674(vView, gPlayers[nPlayer].ball.vStart, vToView);
    vToView[1] = 0.0f;
    if ((f32)fn_80009680(fn_80009744(vToPin)) > 0.0f) {
        fn_800BAF04(vToPin, vToPin);
    }
    if ((f32)fn_80009680(fn_80009744(vToView)) > 0.0f) {
        fn_800BAF04(vToView, vToView);
    }
    fCos = fn_8000C5FC(vToView, vToPin);
    if (fCos < -1.0f) {
        fCos = -1.0f;
    } else if (fCos > 1.0f) {
        fCos = 1.0f;
    }
    fAngle = fn_80009614(fCos);
    if (vToView[2] * vToPin[0] - vToView[0] * vToPin[2] < 0.0f) {
        fAngle *= -1.0f;
    }
    return fAngle;
}

// The score the hole will finish on once the tap-in drops: strokes so far plus one, minus par.
int Hole_ScoreAfterTapIn(int nPlayer) {
    return gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1 - fn_800D2B08();
}

// Whether nobody took anything (mode points or n22C) on the last hole played before this one; 0
// on the round's first hole.
u8 fn_800D0AF4(void) {
    int i;
    int j;
    if (fn_800E1734()) {
        return 0;
    }
    for (i = Game_CurHoleIndex() - 1; i >= 0; i--) {
        if (gpGame->bHoleSelected[i]) {
            for (j = 0; j < gNumPlayersSetUp; j++) {
                if (gPlayers[j].nModePoints[i] != 0 || gPlayers[j].n22C[i] != 0) {
                    return 0;
                }
            }
            return 1;
        }
    }
    return 0;
}

// The class of the surface the shot started from, 0 for none.
u32 fn_800D0BAC(int nPlayer) {
    if (gPlayers[nPlayer].ball.nStartSurface < 0 ||
        gPlayers[nPlayer].ball.nStartSurface >= NUM_SURFACE_TYPES) {
        return 0;
    }
    return gSurfaceTypes[gPlayers[nPlayer].ball.nStartSurface].nClass;
}

// The ground under Player.vBall is not green (class 3) and the hole's strokes so far are three
// under par or better (bUnder: more than three); without bAnyLie, the ball must also be on the
// green or in the cup and two under par (bUnder: more).
u8 fn_800D0BF8(int nPlayer, u8 bUnder, u8 bAnyLie) {
    SurfaceType* pSurface;
    int nPar;
    int nStrokes;
    int nLie;

    pSurface = Ter_GetSupportingWorldMaterial(gPlayers[nPlayer].ball.pCourse, gPlayers[nPlayer].vBall);
    if (pSurface != NULL) {
        nPar = fn_800D2B08();
        if (bAnyLie) {
            nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
            if (bUnder) {
                if (pSurface->nClass != 3 && nStrokes < nPar - 3) {
                    return 1;
                }
            } else if (pSurface->nClass != 3 && nStrokes <= nPar - 3) {
                return 1;
            }
        } else {
            nLie = gPlayers[nPlayer].ball.nLie;
            nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
            if (bUnder) {
                if (pSurface->nClass != 3 && (nLie == LIE_GREEN_e || nLie == LIE_INCUP_e) &&
                    nStrokes < nPar - 2) {
                    return 1;
                }
            } else if (pSurface->nClass != 3 && (nLie == LIE_GREEN_e || nLie == LIE_INCUP_e) &&
                       nStrokes <= nPar - 2) {
                return 1;
            }
        }
    }
    return 0;
}

// The shot started from class 1 ground (fairway) and the ball now lies 1, 9 or 12 (12: in the cup).
u8 fn_800D0D54(int nPlayer) {
    if (fn_800D0BAC(nPlayer) != 1) return 0;
    if (gPlayers[nPlayer].ball.nLie == 1 || gPlayers[nPlayer].ball.nLie == 9 ||
        gPlayers[nPlayer].ball.nLie == 12) {
        return 1;
    }
    return 0;
}

// The round's holes the player finished nToPar or better; below -3 a hole in one always counts.
int fn_800D0DC8(int nPlayer, int nToPar) {
    int nCount = 0;
    int i;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            if (gPlayers[nPlayer].nStrokes[i] - fn_800D2AD8(i) <= nToPar ||
                (gPlayers[nPlayer].nStrokes[i] == 1 && nToPar < -3)) {
                nCount++;
            }
        }
    }
    return nCount;
}

// The round's holes the player finished over par.
int fn_800D0E74(int nPlayer) {
    int nCount = 0;
    int i;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            if (gPlayers[nPlayer].nStrokes[i] - fn_800D2AD8(i) >= 1) {
                nCount++;
            }
        }
    }
    return nCount;
}

// The longest run of the round's holes finished nToPar or better (as fn_800D0DC8 counts them).
int fn_800D0F04(int nPlayer, int nToPar) {
    int nRun;
    int nBest;
    int i;
    nBest = 0;
    nRun = 0;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            if (gPlayers[nPlayer].nStrokes[i] - fn_800D2AD8(i) <= nToPar ||
                (gPlayers[nPlayer].nStrokes[i] == 1 && nToPar < -3)) {
                nRun++;
            } else {
                if (nRun > nBest) {
                    nBest = nRun;
                }
                nRun = 0;
            }
        }
    }
    if (nRun > nBest) {
        nBest = nRun;
    }
    return nBest;
}

// The round's holes with b2E4 set (by position in the score block, TW06's fairways[]).
int fn_800D0FBC(int nPlayer) {
    int nCount = 0;
    int i;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i] && gPlayers[nPlayer].b2E4[i]) {
            nCount++;
        }
    }
    return nCount;
}

// The longest run of the round's holes with b2E4 set; a par 3 does not break it.
int fn_800D10B0(int nPlayer) {
    int nRun;
    int nBest;
    int i;
    nBest = 0;
    nRun = 0;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i] && gPlayers[nPlayer].b2E4[i]) {
            nRun++;
        } else if (fn_800D2AD8(i) != 3) {
            if (nRun > nBest) {
                nBest = nRun;
            }
            nRun = 0;
        }
    }
    if (nRun > nBest) {
        nBest = nRun;
    }
    return nBest;
}

// The round's holes with b2F6 set (TW06's gir[]); with bOnlyFlagged, only those whose
// gpGame->b16C entry is 1.
int fn_800D1170(int nPlayer, u8 bOnlyFlagged) {
    int nCount = 0;
    int i;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i] && gPlayers[nPlayer].b2F6[i] &&
            (gpGame->b16C[nPlayer][i] == 1 || !bOnlyFlagged)) {
            nCount++;
        }
    }
    return nCount;
}

// The longest run of the round's holes with b2F6 set.
int fn_800D1250(int nPlayer) {
    int nRun;
    int nBest;
    int i;
    nBest = 0;
    nRun = 0;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i] && gPlayers[nPlayer].b2F6[i]) {
            nRun++;
        } else {
            if (nRun > nBest) {
                nBest = nRun;
            }
            nRun = 0;
        }
    }
    if (nRun > nBest) {
        nBest = nRun;
    }
    return nBest;
}

// The player's putts over the round's holes.
int fn_800D1330(int nPlayer) {
    int nPutts = 0;
    int i;
    for (i = 0; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            nPutts += gPlayers[nPlayer].nPutts[i];
        }
    }
    return nPutts;
}

// Where the wind blows from the player's aim, by quarter: 2 within 45 degrees of the aim, 4 the
// next quarter round, 1 the opposite quarter, 3 the last; 0 when the wind is 6 or less.
int fn_800D13F4(int nPlayer) {
    f32 vWind[4];
    f32 fAim = gPlayers[nPlayer].fAim;
    f32 fAngle;

    if (Wind_Get(vWind) > 6.0f) {
        fAngle = fn_8000AD78(-vWind[0], vWind[2]) - fAim;
        while (fAngle < 0.0f) {
            fAngle += 2.0f * PI;
        }
        while (fAngle > 2.0f * PI) {
            fAngle -= 2.0f * PI;
        }
        if (fAngle >= 7.0f * PI / 4.0f || fAngle <= PI / 4.0f) {
            return 2;
        }
        if (fAngle >= PI / 4.0f && fAngle <= 3.0f * PI / 4.0f) {
            return 4;
        }
        if (fAngle >= 3.0f * PI / 4.0f && fAngle <= 5.0f * PI / 4.0f) {
            return 1;
        }
        if (fAngle >= 5.0f * PI / 4.0f && fAngle <= 7.0f * PI / 4.0f) {
            return 3;
        }
        return 0;
    }
    return 0;
}

// The slope of the ground under the ball across the player's aim, in whole degrees (+-90 when the
// ground's normal has no part along the aim); 0 with no ground or a normal not of length 1.
int fn_800D1530(int nPlayer) {
    f32 vNormal[4];
    f32 vTurned[4];
    f32 fLength;
    f32 fAim;
    f32 fSin;
    f32 fCos;
    f32 fDegrees;

    if (!Ter_GetSupportingGroundNormal(fn_8000C594(), gPlayers[nPlayer].ball.vPos, vNormal)) {
        return 0;
    }
    fLength = fn_80009744(vNormal);
    if (fLength > 1.01f || fLength < 0.99f) {
        return 0;
    }
    fAim = gPlayers[nPlayer].fAim;
    fSin = fn_800095F0(fAim);
    fCos = fn_80009638(fAim);
    Vec3Copy(vNormal, vTurned);
    fn_80055D70(&vTurned[2], &vTurned[0], fSin, fCos);
    if (vTurned[1] < 0.000001f && vTurned[1] > -0.000001f) {
        if (vTurned[0] < 0.0f) {
            fDegrees = -90.0f;
        } else {
            fDegrees = 90.0f;
        }
    } else {
        fDegrees = fn_8000AD78(vTurned[0], vTurned[1]) * (180.0f / PI);
    }
    return (int)fDegrees;
}

// a - b into out (three floats)
#ifdef __MWERKS__
asm void fn_800D1674(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_800D1674(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif
