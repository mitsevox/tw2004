// GameModeStableford.c (TW06's GameModeStableford): game mode 18, modified Stableford. Each hole
// scores points by strokes against par from lbl_802816D0 (albatross 8, eagle 5, birdie 2, par 0,
// bogey -1, double bogey or worse -3; the points go in nModePoints). A player who can no longer
// beat double bogey is done with the hole.

#include "golfer.h"
#include "game.h"
#include "game/save.h"
#include "game/earnings.h"

// The points for 3 under par .. 2 or more over. TW06: GameModeStableford::stablefordPointTable.
extern s8  lbl_802816D0[6];

u8   GameModeStableford_PlayerDoneHole(int nPlayer);
void GameModeStableford_SetupNextGolfer(void);
s32  GameModeStableford_GetHonors(int nPlayer);
u8   GameModeStableford_HoleFinished(int nPlayer, u8 bCheck);
u8   GameModeStableford_GameFinished(u8 bCheck);
u8   fn_800FE8A0(u8 bCheck);
void GameModeStableford_EndHole(void);
void GameModeStableford_EndGame(void);

// One mulligan per player per round; the CPU may concede.
void GameModeStableford_Init(void) {
    gpGame->pfnInit = GameModeStableford_Init;
    gpGame->pfnSetupNextGolfer = GameModeStableford_SetupNextGolfer;
    gpGame->pfnGetHonors = GameModeStableford_GetHonors;
    gpGame->pfnHoleFinished = GameModeStableford_HoleFinished;
    gpGame->pfnGameFinished = GameModeStableford_GameFinished;
    gpGame->pfnGoToPlayoff = fn_800FE8A0;
    gpGame->pfnEndHole = GameModeStableford_EndHole;
    gpGame->pfnEndGame = GameModeStableford_EndGame;
    gpGame->bAIConcedes = 1;
    gpGame->n4 = 3;
    gpGame->nMulligans = 2;
    gpGame->nC = 4;
    gpGame->n10 = 1;
    gpGame->nDC = 0;
    fn_800E1480(0);
    gSession.nSplitScreen = 0;
}

// Holed, cut, or already at par + 1 strokes: the best
// left is double bogey, which scores no better than giving up.
u8 GameModeStableford_PlayerDoneHole(int nPlayer) {
    if (!Player_IsHoled(nPlayer) && !gPlayers[nPlayer].bPlayerCut &&
        gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] < fn_800D2B08() + 1) {
        return 0;
    }
    return 1;
}

// In split screen everyone plays at once; otherwise the
// player with the honor gets ready and the others wait.
void GameModeStableford_SetupNextGolfer(void) {
    int i;
    if (gSession.nSplitScreen == 1) {
        for (i = 0; i < gNumPlayersSetUp; i++) {
            GOLFERSTATE_Set(GS_PRE_SHOT, i);
        }
    } else {
        lbl_80282278 = gpGame->pfnGetHonors(5);
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (i == lbl_80282278) {
                GOLFERSTATE_Set(GS_PRE_SHOT, i);
            } else {
                GOLFERSTATE_Set(GS_WAIT, i);
            }
        }
    }
}

// Who plays next after nPlayer (5 = nobody). On the tee the
// honor goes by the scores on the holes played so far (the latest hole first, ties by the hole
// before); otherwise to the player farthest from the pin, off the green first.
s32 GameModeStableford_GetHonors(int nPlayer) {
    s32 aOrder[4] = {0, 1, 2, 3};
    s32 aSorted[4];
    int n;
    int h;
    int i;
    int k;
    s32 nLow;
    s32 nScore;
    s32 nHigh;
    CourseInfo* pCourse;
    int nPinSet;
    f32 fBest;
    int nBest;
    f32 dx;
    f32 dz;
    f32 d;
    for (h = 0; h < Game_CurHoleIndex(); h++) {
        if (gpGame->bHoleSelected[h]) {
            nLow = gPlayers[0].nStrokes[h];
            nHigh = nLow;
            for (i = 0; i < gNumPlayersSetUp; i++) {
                if (gPlayers[(u32)i].nStrokes[h] < nLow) {
                    nLow = gPlayers[(u32)i].nStrokes[h];
                }
                if (gPlayers[(u32)i].nStrokes[h] > nHigh) {
                    nHigh = gPlayers[(u32)i].nStrokes[h];
                }
            }
            n = 0;
            for (nScore = nLow; nScore <= nHigh; nScore++) {
                for (k = 0; k < gNumPlayersSetUp; k++) {
                    AddIfScore(aSorted, &n, aOrder[k], h, nScore);
                }
            }
            for (i = 0; i < gNumPlayersSetUp; i++) {
                aOrder[i] = aSorted[i];
            }
        }
    }
    for (h = 0; h < gNumPlayersSetUp; h++) {
        if (nPlayer != aOrder[h] && Player_OnTee(aOrder[h]) &&
            !GameModeStableford_PlayerDoneHole(aOrder[h])) {
            return aOrder[h];
        }
    }
    nBest = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && !GameModeStableford_PlayerDoneHole(i)) {
            nBest = i;
            break;
        }
    }
    if (nPlayer == 5 && nBest == 5) {
        return 5;
    }
    pCourse = fn_8000C594();
    nPinSet = Game_CurrentPinSet();
    fBest = 0.0f;
    nBest = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && PLAYER(i)->ball.nLie != LIE_GREEN_e && !GameModeStableford_PlayerDoneHole(i)) {
            dx = PLAYER(i)->ball.vPos[0] - pCourse->pin[nPinSet].x;
            dz = PLAYER(i)->ball.vPos[2] - pCourse->pin[nPinSet].z;
            d = fn_80009680(dx * dx + dz * dz);
            if (d > fBest) {
                fBest = d;
                nBest = i;
            }
        }
    }
    if (nBest == 5) {
        fBest = 0.0f;
        nBest = 5;
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (i != nPlayer && !GameModeStableford_PlayerDoneHole(i)) {
                dx = PLAYER(i)->ball.vPos[0] - pCourse->pin[nPinSet].x;
                dz = PLAYER(i)->ball.vPos[2] - pCourse->pin[nPinSet].z;
                d = fn_80009680(dx * dx + dz * dz);
                if (d > fBest) {
                    fBest = d;
                    nBest = i;
                }
            }
        }
    }
    if (nBest == nPlayer) {
        return 5;
    }
    return nBest;
}

// The hole is over when every player is done with it.
u8 GameModeStableford_HoleFinished(int nPlayer, u8 bCheck) {
    int i;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (!GameModeStableford_PlayerDoneHole(i)) {
            return 0;
        }
    }
    return 1;
}

// Over when no selected hole is left.
u8 GameModeStableford_GameFinished(u8 bCheck) {
    int h;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return 0;
        }
    }
    return 1;
}

// No playoff (the pfnGoToPlayoff slot).
u8 fn_800FE8A0(u8 bCheck) {
    return 0;
}

// A player who did not hole out is scored par + 2 (double
// bogey); every player gets the points for the hole.
void GameModeStableford_EndHole(void) {
    int nHole;
    s32 nPar;
    int i;
    int nDiff;
    nHole = Game_CurHoleIndex();
    nPar = fn_800D2B08();
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (!Player_IsHoled(i)) {
            gPlayers[(u32)i].nStrokes[nHole] = nPar + 2;
        }
        nDiff = gPlayers[(u32)i].nStrokes[nHole] - nPar;
        nDiff = (nDiff < -3) ? -3 : ((nDiff > 2) ? 2 : nDiff);
        gPlayers[(u32)i].nModePoints[nHole] = lbl_802816D0[nDiff + 3];
    }
}

// TW06's EndGame is empty. Each human with a profile whose round total (fn_800E1788: in this
// mode the Stableford points) is below a CPU player's wins money: the prize for the best earnings
// rating among those CPU players, its base plus its per-stroke prize for up to 5 of margin.
void GameModeStableford_EndGame(void) {
    int i;
    int j;
    int nBest;
    int nOurs;
    int nTheirs;
    int nRating;
    int nMoney;
    int nMargin;
    int nBase;
    int nProfile;
    u8 bFirst = 1;
    if (fn_800E1BBC()) {
        switch (fn_800EC550()) {
        case 0:
            break;
        default:
            return;
        }
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (!Player_IsCPU(i)) {
                nBest = -1;
                nOurs = fn_800E1788(i);
                for (j = 0; j < gNumPlayersSetUp; j++) {
                    if (i != j && Player_IsCPU(j)) {
                        nTheirs = fn_800E1788(j);
                        if (nOurs < nTheirs) {
                            nRating = GM_Earnings_RateGolfer(j);
                            if (nRating > nBest) {
                                nBest = nRating;
                                nMargin = nTheirs - nOurs;
                            }
                        }
                    }
                }
                if (nBest > -1) {
                    if (nMargin > 5) {
                        nMargin = 5;
                    }
                    nBase = lbl_80200538.aStrokePrize[nBest].nBase;
                    nProfile = PLAYER(i)->nIndex;
                    nMoney = nBase + lbl_80200538.aStrokePrize[nBest].nPerStroke * nMargin;
                    if (gpSaveData[nProfile].bActive) {
                        if (bFirst) {
                            EASBio_IncrementGamesWon(1);
                            bFirst = 0;
                        }
                        if (nBase) {
                            fn_800E4364(0, 0x75, nBase, nProfile);
                        }
                        fn_800D3548(i, nMoney, 0);
                    }
                }
            }
        }
    }
}
