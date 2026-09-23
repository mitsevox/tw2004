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
void fn_800D1674(f32* pA, f32* pB, f32* pOut);

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

// The score the hole will finish on once the tap-in drops: strokes so far plus one, minus par.
int Hole_ScoreAfterTapIn(int nPlayer) {
    return gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1 - fn_800D2B08();
}

// The class of the surface the shot started from, 0 for none.
u32 fn_800D0BAC(int nPlayer) {
    if (gPlayers[nPlayer].ball.nStartSurface < 0 ||
        gPlayers[nPlayer].ball.nStartSurface >= NUM_SURFACE_TYPES) {
        return 0;
    }
    return gSurfaceTypes[gPlayers[nPlayer].ball.nStartSurface].nClass;
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
