// GameModeMatch.c (TW06's GameModeMatch): game mode 1, one-against-one match play, with a
// sudden-death playoff when the match is tied after the last hole. GameModeBattle.c reuses most of
// these callbacks.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/modes/challenge.h"
#include "game/save.h"

extern s32 lbl_80281658;                    // who has the honor in the playoff (5 = nobody yet)

int  GameModeMatch_GetTeeHonors(int nPlayer);
void GameModeMatch_EndGame(void);

// Two players; the CPU may concede.
void GameModeMatch_Init(void) {
    gpGame->pfnInit = GameModeMatch_Init;
    gpGame->pfnSetupNextGolfer = fn_800E9F14;
    gpGame->pfnGetHonors = GameModeMatch_GetHonors;
    gpGame->pfnHoleFinished = GameModeMatch_HoleFinished;
    gpGame->pfnGameFinished = GameModeMatch_GameFinished;
    gpGame->pfnGoToPlayoff = GameModeMatch_GoToPlayoff;
    gpGame->pfnEndHole = GameModeMatch_EndHole;
    gpGame->pfnEndGame = GameModeMatch_EndGame;
    gpGame->bAIConcedes = 1;
    gpGame->n4 = 1;
    gpGame->nMulligans = 0;
    gpGame->nC = 2;
    gpGame->n10 = 2;
    gpGame->nDC = 0;
    lbl_80281658 = 5;
    gSession.nSplitScreen = 0;
}

// The hole starts: the first golfer to play gets ready, the other waits.
void fn_800E9F14(void) {
    int i;
    lbl_80282278 = gpGame->pfnGetHonors(5);
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i == lbl_80282278) {
            GOLFERSTATE_Set(GS_PRE_SHOT, i);
        } else {
            GOLFERSTATE_Set(GS_WAIT, i);
        }
    }
}

// In the playoff the player who had the honor at its start;
// otherwise the winner of the last decided hole, else the first other player (5 = nobody).
int GameModeMatch_GetTeeHonors(int nPlayer) {
    int h;
    int i;
    if (gpGame->bD4 && nPlayer != lbl_80281658) {
        return lbl_80281658;
    }
    for (h = Game_CurHoleIndex() - 1; h >= 0; h--) {
        if (gpGame->bHoleSelected[h]) {
            for (i = 0; i < gNumPlayersSetUp; i++) {
                if (i != nPlayer && gPlayers[(u32)i].nModePoints[h] != 0) {
                    return i;
                }
            }
        }
    }
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer) {
            return i;
        }
    }
    return 5;
}

// On the tee the honor; otherwise the player farthest from the pin
// (off the green first).
s32 GameModeMatch_GetHonors(int nPlayer) {
    int i;
    CourseInfo* pCourse;
    int nPinSet;
    f32 fBest;
    int nBest;
    f32 dx;
    f32 dz;
    f32 d;
    nBest = GameModeMatch_GetTeeHonors(nPlayer);
    if (nBest != 5 && Player_OnTee(nBest)) {
        return nBest;
    }
    nBest = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && !Player_IsHoled(i)) {
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
        if (i != nPlayer && !Player_IsHoled(i) && PLAYER(i)->ball.nLie != LIE_GREEN_e) {
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
            if (i != nPlayer && !Player_IsHoled(i)) {
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

// Both holed; or one holed and the other can no longer win the
// hole (nor halve it when the holed player is dormie). Unless only checking, the loser's score gets
// a stroke for the putt they did not take.
u8 GameModeMatch_HoleFinished(int nPlayer, u8 bCheck) {
    int nLeft;
    int h;
    if (Player_IsHoled(0) && Player_IsHoled(1)) {
        return 1;
    }
    if (Player_IsHoled(0) && (!lbl_80282240 || nPlayer != 0)) {
        if (gPlayers[0].nStrokes[Game_CurHoleIndex()] <= gPlayers[1].nStrokes[Game_CurHoleIndex()]) {
            if (!bCheck) {
                gPlayers[1].nStrokes[Game_CurHoleIndex()]++;
            }
            return 1;
        }
    }
    if (Player_IsHoled(1) && (!lbl_80282240 || nPlayer != 1)) {
        if (gPlayers[1].nStrokes[Game_CurHoleIndex()] <= gPlayers[0].nStrokes[Game_CurHoleIndex()]) {
            if (!bCheck) {
                gPlayers[0].nStrokes[Game_CurHoleIndex()]++;
            }
            return 1;
        }
    }
    nLeft = 0;
    for (h = Game_CurHoleIndex(); h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            nLeft++;
        }
    }
    if (Player_IsHoled(0) && (!lbl_80282240 || nPlayer != 0) &&
        gPlayers[0].nHolesWon == gPlayers[1].nHolesWon + nLeft) {
        if (gPlayers[0].nStrokes[Game_CurHoleIndex()] <= gPlayers[1].nStrokes[Game_CurHoleIndex()] + 1) {
            if (!bCheck) {
                gPlayers[1].nStrokes[Game_CurHoleIndex()]++;
            }
            return 1;
        }
    }
    if (Player_IsHoled(1) && (!lbl_80282240 || nPlayer != 1) &&
        gPlayers[1].nHolesWon == gPlayers[0].nHolesWon + nLeft) {
        if (gPlayers[1].nStrokes[Game_CurHoleIndex()] <= gPlayers[0].nStrokes[Game_CurHoleIndex()] + 1) {
            if (!bCheck) {
                gPlayers[0].nStrokes[Game_CurHoleIndex()]++;
            }
            return 1;
        }
    }
    return 0;
}

#define PLAYER_AT(i) (&gPlayers[i])
// Clears every player's round (all 18 holes) for a playoff.
#define CLEAR_ROUNDS(P)                             \
    for (i = 0; i < gNumPlayersSetUp; i++) {        \
        for (h = 0; h < 18; h++) {                  \
            P(i)->nStrokes[h] = 0;                  \
            P(i)->nPutts[h] = 0;                    \
            P(i)->nModePoints[h] = 0;               \
            P(i)->n22C[h] = 0;                      \
            P(i)->n290[h] = 0;                      \
            P(i)->b2F6[h] = 0;                      \
            P(i)->b2E4[h] = 0;                      \
        }                                           \
        P(i)->n2D8 = 0;                             \
        P(i)->n2DC = 0;                             \
        P(i)->n2E0 = 0;                             \
        P(i)->n308 = 0;                             \
    }

u8 GameModeMatch_GameFinished(u8 bCheck) {
    int nLeft;
    int h;
    int i;
    if (gpGame->bD4) {
        if (gPlayers[0].nHolesWon != gPlayers[1].nHolesWon) {
            return 1;
        }
        if (!bCheck) {
            gpGame->nD8++;
            fn_800E2BA4();
            CLEAR_ROUNDS(PLAYER_AT);
            fn_800E45C0();
        }
    } else {
        nLeft = 0;
        for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
            if (gpGame->bHoleSelected[h]) {
                nLeft++;
            }
        }
        if (nLeft == 0) {
            return !GameModeMatch_GoToPlayoff(bCheck);
        }
        if (gPlayers[1].nHolesWon + nLeft < gPlayers[0].nHolesWon ||
            gPlayers[0].nHolesWon + nLeft < gPlayers[1].nHolesWon) {
            return 1;
        }
    }
    return 0;
}

// The honor for the playoff is worked out as if on the next hole.
u8 GameModeMatch_GoToPlayoff(u8 bCheck) {
    int h;
    int i;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return 0;
        }
    }
    if (gPlayers[0].nHolesWon == gPlayers[1].nHolesWon) {
        if (bCheck) {
            return 1;
        }
        gpGame->nCurHole++;
        lbl_80281658 = GameModeMatch_GetTeeHonors(5);
        gpGame->nCurHole--;
        gpGame->bD5 = 1;
        for (h = 0; h < 18; h++) {
            if (!gpGame->bHoleSelected[h]) {
                gpGame->bD5 = 0;
            }
        }
        fn_800E2BA4();
        CLEAR_ROUNDS(PLAYER);
        gpGame->bD4 = 1;
        gpGame->nD8++;
        fn_800E45C0();
        return 1;
    }
    return 0;
}

// The player who holed out in fewer strokes wins the hole.
void GameModeMatch_EndHole(void) {
    if (Player_IsHoled(0) &&
        gPlayers[0].nStrokes[Game_CurHoleIndex()] < gPlayers[1].nStrokes[Game_CurHoleIndex()]) {
        gPlayers[0].nModePoints[Game_CurHoleIndex()] = 1;
        gPlayers[0].nHolesWon++;
    }
    if (Player_IsHoled(1) &&
        gPlayers[1].nStrokes[Game_CurHoleIndex()] < gPlayers[0].nStrokes[Game_CurHoleIndex()]) {
        gPlayers[1].nModePoints[Game_CurHoleIndex()] = 1;
        gPlayers[1].nHolesWon++;
    }
}

// A human winner with a profile gets the prize.
void GameModeMatch_EndGame(void) {
    int nPrize;
    int nWinner;
    int nLoser;
    int nMargin;
    int nMoney;
    int nProfile;
    if (fn_800E1BBC()) {
        switch (fn_800EC550()) {
        case 0:
            break;
        default:
            return;
        }
        if (gPlayers[0].nHolesWon > gPlayers[1].nHolesWon) {
            nMargin = gPlayers[0].nHolesWon - gPlayers[1].nHolesWon;
            nWinner = 0;
            nLoser = 1;
        } else {
            nMargin = gPlayers[1].nHolesWon - gPlayers[0].nHolesWon;
            nWinner = 1;
            nLoser = 0;
        }
        nMoney = GM_Earnings_GetStrokeWinnings(nWinner, nLoser, nMargin, &nPrize);
        if (!Player_IsCPU(nWinner)) {
            nProfile = gPlayers[nWinner].nIndex;
            if (gpSaveData[nProfile].bActive) {
                EASBio_SetCurrentGameWon(1);
                if (nMoney) {
                    fn_800E4364(0, 0x6B, nPrize, nProfile);
                    fn_800D3548(nWinner, nMoney, 0);
                    gPlayers[nWinner].money.n14 += nPrize;
                    gPlayers[nWinner].money.n10 = gPlayers[nWinner].money.n24 - nPrize;
                }
            }
        }
    }
}

// The current challenge's group.
s32 fn_800EAC7C(void) {
    return lbl_80281664[lbl_802822F4].nGroup;
}

// The index of the first challenge of group n (0 if none).
int fn_800EAC94(int n) {
    int i;
    int nFound = 0;
    for (i = 0; i < lbl_80281668; i++) {
        if (n == lbl_80281664[i].nGroup) {
            nFound = i;
            break;
        }
    }
    return nFound;
}
