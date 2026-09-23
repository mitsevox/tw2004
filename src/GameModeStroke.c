// GameModeStroke.c (TW06's GameModeStroke, matched by method order): game mode 0, stroke play for
// up to four players. The lowest score on the last hole played has the honor, the farthest from the
// pin plays next, and a human who beats CPU golfers wins the best one's prize. Modes 9, 12, 13..17
// and 23 reuse some of these callbacks.

#include "golfer.h"
#include "game.h"
#include "game/save.h"
#include "game/earnings.h"

void GameModeStroke_EndGame(void);

// Up to four players, one mulligan each.
void GameModeStroke_Init(void) {
    gpGame->pfnInit = GameModeStroke_Init;
    gpGame->pfnSetupNextGolfer = GameModeStroke_SetupNextGolfer;
    gpGame->pfnGetHonors = GameModeStroke_GetHonors;
    gpGame->pfnHoleFinished = GameModeStroke_HoleFinished;
    gpGame->pfnGameFinished = GameModeStroke_GameFinished;
    gpGame->pfnGoToPlayoff = fn_800FFDB0;
    gpGame->pfnEndGame = GameModeStroke_EndGame;
    gpGame->n4 = 0;
    gpGame->nMulligans = 2;
    gpGame->nC = 4;
    gpGame->n10 = 1;
    gpGame->nDC = 0;
    fn_800E1480(0);
    gSession.nSplitScreen = 0;
}

// The hole starts: in split screen everyone
// plays at once; otherwise the first golfer gets ready and the others wait.
void GameModeStroke_SetupNextGolfer(void) {
    int i;
    if (gSession.nSplitScreen == 1) {
        for (i = 0; i < gNumPlayersSetUp; i++) {
            GOLFERSTATE_Set(GS_PRE_SHOT, i);
        }
        return;
    }
    lbl_80282278 = gpGame->pfnGetHonors(5);
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i == lbl_80282278) {
            GOLFERSTATE_Set(GS_PRE_SHOT, i);
        } else {
            GOLFERSTATE_Set(GS_WAIT, i);
        }
    }
}

// Who plays next after nPlayer (5 = nobody): on the tee, the order
// of the scores on each hole played so far (ties keep the order of the hole before); otherwise the
// player farthest from the pin (off the green first). Players who were cut do not play.
s32 GameModeStroke_GetHonors(int nPlayer) {
    s32 aOrder[4] = {0, 1, 2, 3};  // the tee order before any hole is played
    s32 aSorted[4];
    int h;
    int i;
    int nLow;
    int nHigh;
    int n;
    int k;
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
                n = gPlayers[(u32)i].nStrokes[h];
                if (n < nLow) {
                    nLow = n;
                }
                if (n > nHigh) {
                    nHigh = n;
                }
            }
            k = 0;
            for (n = nLow; n <= nHigh; n++) {
                for (i = 0; i < gNumPlayersSetUp; i++) {
                    AddIfScore(aSorted, &k, aOrder[i], h, n);
                }
            }
            for (i = 0; i < gNumPlayersSetUp; i++) {
                aOrder[i] = aSorted[i];
            }
        }
    }
    // fake match: h doubles as the player counter here; a separate counter gets a different register.
    for (h = 0; h < gNumPlayersSetUp; h++) {
        if (nPlayer != aOrder[h] && Player_OnTee(aOrder[h]) && !gPlayers[aOrder[h]].bPlayerCut) {
            return aOrder[h];
        }
    }
    nBest = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && !Player_IsHoled(i) && !PLAYER(i)->bPlayerCut) {
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
        if (i != nPlayer && !Player_IsHoled(i) && !PLAYER(i)->bPlayerCut &&
            PLAYER(i)->ball.nLie != LIE_GREEN_e) {
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
            if (i != nPlayer && !Player_IsHoled(i) && !PLAYER(i)->bPlayerCut) {
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

// Everyone still playing has holed out.
u8 GameModeStroke_HoleFinished(int nPlayer, u8 bCheck) {
    int i;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (!Player_IsHoled(i) && !PLAYER(i)->bPlayerCut) {
            return 0;
        }
    }
    return 1;
}

// No selected hole is left.
u8 GameModeStroke_GameFinished(u8 bCheck) {
    int h;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return 0;
        }
    }
    return 1;
}

// GoToPlayoff: stroke play has none.
u8 fn_800FFDB0(u8 bCheck) {
    return 0;
}

// Each human with a profile who beat CPU golfers wins the prize for
// the best earnings rating among them: its base prize plus its per-stroke prize for up to 5 strokes
// of margin.
void GameModeStroke_EndGame(void) {
    int i;
    int j;
    int nScore;
    int nOther;
    int nRating;
    int nBest;
    int nMargin;
    int nMoney;
    int nBase;
    int nProfile;
    u8 bFirst;
    bFirst = 1;
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
                nScore = fn_800E1788(i);
                for (j = 0; j < gNumPlayersSetUp; j++) {
                    if (i != j && Player_IsCPU(j)) {
                        nOther = fn_800E1788(j);
                        if (nScore < nOther) {
                            nRating = GM_Earnings_RateGolfer(j);
                            if (nRating > nBest) {
                                nBest = nRating;
                                nMargin = nOther - nScore;
                            }
                        }
                    }
                }
                if (nBest > -1) {
                    if (nMargin > 5) {
                        nMargin = 5;
                    }
                    nBase = lbl_80200538.aStrokePrize[nBest].nBase;
                    nMoney = nBase + lbl_80200538.aStrokePrize[nBest].nPerStroke * nMargin;
                    nProfile = PLAYER(i)->nIndex;
                    if (gpSaveData[nProfile].bActive) {
                        if (bFirst) {
                            EASBio_IncrementGamesWon(1);
                            bFirst = 0;
                        }
                        if (nBase) {
                            fn_800E4364(0, 0x6A, nBase, nProfile);
                        }
                        fn_800D3548(i, nMoney, NULL);
                    }
                }
            }
        }
    }
}
