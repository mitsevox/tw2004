// GameModeFourBall.c (TW06's GameModeFourBall): game mode 20, two-against-two match play where each
// team counts its better ball on every hole. Team 0 is players 0 and 1, team 1 players 2 and 3.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "game/save.h"

u8   GameModeFourBall_TeamDone(int nTeam);
u8   GameModeFourBall_TeamConceded(int nTeam);
int  GameModeFourBall_TeamBestPossibleScore(int nTeam);
int  GameModeFourBall_TeamMatchWins(int nTeam);
void fn_800E90FC(void);
s32  GameModeFourBall_GetHonors(int nPlayer);
int  GameModeFourBall_GetPlayerTeam(int nPlayer);
u8   GameModeFourBall_HoleFinished(int nPlayer, u8 bCheck);
u8   GameModeFourBall_GameFinished(u8 bCheck);
u8   GameModeFourBall_GoToPlayoff(u8 bCheck);
void GameModeFourBall_EndHole(void);
void GameModeFourBall_EndGame(void);

// Four players; the CPU may concede.
void GameModeFourBall_Init(void) {
    gpGame->pfnInit = GameModeFourBall_Init;
    gpGame->pfnSetupNextGolfer = fn_800E90FC;
    gpGame->pfnGetHonors = GameModeFourBall_GetHonors;
    gpGame->pfnHoleFinished = GameModeFourBall_HoleFinished;
    gpGame->pfnGameFinished = GameModeFourBall_GameFinished;
    gpGame->pfnGoToPlayoff = GameModeFourBall_GoToPlayoff;
    gpGame->pfnEndHole = GameModeFourBall_EndHole;
    gpGame->pfnEndGame = GameModeFourBall_EndGame;
    gpGame->bAIConcedes = 1;
    gpGame->n4 = 1;
    gpGame->nMulligans = 0;
    gpGame->nC = 4;
    gpGame->n10 = 4;
    gpGame->nDC = 0;
    gSession.nSplitScreen = 0;
}

// A partner has holed out and the other can no longer beat that
// score.
u8 GameModeFourBall_TeamDone(int nTeam) {
    int nHole = Game_CurHoleIndex();
    int a;
    int bDone;
    int b;
    a = 2;
    if (nTeam == 0) {
        a = 0;
    }
    b = 3;
    if (nTeam == 0) {
        b = 1;
    }
    bDone = 0;
    if ((Player_IsHoled(a) && gPlayers[a].nStrokes[nHole] <= gPlayers[b].nStrokes[nHole] + 1) ||
        (Player_IsHoled(b) && gPlayers[b].nStrokes[nHole] <= gPlayers[a].nStrokes[nHole] + 1)) {
        bDone = 1;
    }
    return bDone;
}

// Both partners picked up.
u8 GameModeFourBall_TeamConceded(int nTeam) {
    int nHole = Game_CurHoleIndex();
    int bConceded;
    int b;
    int a;
    a = 2;
    if (nTeam == 0) {
        a = 0;
    }
    b = 3;
    if (nTeam == 0) {
        b = 1;
    }
    bConceded = 0;
    if (Player_IsHoled(a) && !Player_IsHoledNotState23(a) &&
        Player_IsHoled(b) && !Player_IsHoledNotState23(b)) {
        bConceded = 1;
    }
    return bConceded;
}

// fake match: GameModeFourBall_TeamBestPossibleScore gets the team's players through these two
// helpers, and the const on the return types is what schedules its first loads like the original
// (found by the permuter); the other team functions above only match with the same code written out.
// The team's first player (0 or 2).
static inline const int FourBall_TeamFirst(int nTeam) {
    int a = 2;
    if (nTeam == 0) {
        a = 0;
    }
    return a;
}

// The team's second player (1 or 3).
static inline const int FourBall_TeamSecond(int nTeam) {
    int b = 3;
    if (nTeam == 0) {
        b = 1;
    }
    return b;
}

// The team's best score on this hole if a partner
// holes the next shot (at most 9), or its score once holed.
int GameModeFourBall_TeamBestPossibleScore(int nTeam) {
    int nHole = Game_CurHoleIndex();
    int a;
    int n;
    int b;
    int nBest;
    a = FourBall_TeamFirst(nTeam);
    b = FourBall_TeamSecond(nTeam);
    if (gPlayers[a].nStrokes[nHole] + 1 < 9) {
        n = gPlayers[a].nStrokes[nHole] + 1;
    } else {
        n = 9;
    }
    nBest = n <= gPlayers[b].nStrokes[nHole] + 1 ? n : gPlayers[b].nStrokes[nHole] + 1;
    if (Player_IsHoled(a)) {
        nBest = nBest <= gPlayers[a].nStrokes[nHole] ? nBest : gPlayers[a].nStrokes[nHole];
    }
    if (Player_IsHoled(b)) {
        nBest = nBest <= gPlayers[b].nStrokes[nHole] ? nBest : gPlayers[b].nStrokes[nHole];
    }
    return nBest;
}

// Kept on the team's first player.
int GameModeFourBall_TeamMatchWins(int nTeam) {
    int nHole = Game_CurHoleIndex();
    int a;
    a = 2;
    if (nTeam == 0) {
        a = 0;
    }
    return gPlayers[a].nHolesWon;
}

// The hole starts: the first golfer to play gets ready, the others wait.
void fn_800E90FC(void) {
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

// On the tee the team that won the last decided hole, and (on
// team 0 only) the better score of the pair; otherwise the player farthest from the pin (off the
// green first) whose team is still playing.
s32 GameModeFourBall_GetHonors(int nPlayer) {
    s32 aOrder[4] = {0, 1, 2, 3};  // the tee order before anyone has won a hole
    s32* pOrder;        // fake match: the within-team compare reads aOrder through a pointer
    int w;
    int t;
    int i;
    int j;
    CourseInfo* pCourse;
    int nPinSet;
    f32 fBest;
    int nBest;
    int h;
    int nLead;
    f32 dx;
    f32 dz;
    f32 d;
    nLead = 0;
    pOrder = aOrder;
    for (h = 0; h < Game_CurHoleIndex(); h++) {
        if (gpGame->bHoleSelected[h]) {
            if (gPlayers[0].nModePoints[h] != 0) {
                w = 0;
            } else if (gPlayers[2].nModePoints[h] != 0) {
                w = 1;
            } else {
                w = nLead;
            }
            if (w != nLead) {
                nLead = w;
                t = aOrder[0];
                aOrder[0] = aOrder[2];
                aOrder[2] = t;
                t = aOrder[1];
                aOrder[1] = aOrder[3];
                aOrder[3] = t;
            }
            if (gPlayers[aOrder[1]].nStrokes[h] < gPlayers[pOrder[0]].nStrokes[h]) {
                t = aOrder[0];
                aOrder[0] = aOrder[1];
                aOrder[1] = t;
            }
        }
    }
    for (j = 0; j < gNumPlayersSetUp; j++) {
        if (nPlayer != aOrder[j] && Player_OnTee(aOrder[j]) &&
            !GameModeFourBall_TeamDone(GameModeFourBall_GetPlayerTeam(aOrder[j]))) {
            return aOrder[j];
        }
    }
    pCourse = fn_8000C594();
    nPinSet = Game_CurrentPinSet();
    fBest = 0.0f;
    nBest = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && !Player_IsHoled(i) &&
            !GameModeFourBall_TeamDone(GameModeFourBall_GetPlayerTeam(i)) &&
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
            if (i != nPlayer && !Player_IsHoled(i) &&
                !GameModeFourBall_TeamDone(GameModeFourBall_GetPlayerTeam(i))) {
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

int GameModeFourBall_GetPlayerTeam(int nPlayer) {
    return nPlayer / 2;
}

// Both teams done or one conceded; or one team done and the
// other can no longer beat it (or only tie, when dormie).
u8 GameModeFourBall_HoleFinished(int nPlayer, u8 bCheck) {
    int nLeft;
    int h;
    if (GameModeFourBall_TeamDone(0) && GameModeFourBall_TeamDone(1)) {
        return 1;
    }
    if (GameModeFourBall_TeamConceded(0) || GameModeFourBall_TeamConceded(1)) {
        return 1;
    }
    if (GameModeFourBall_TeamDone(0) && (!lbl_80282240 || (u32)nPlayer > 1)) {
        if (GameModeFourBall_TeamBestPossibleScore(0) < GameModeFourBall_TeamBestPossibleScore(1)) {
            return 1;
        }
    }
    if (GameModeFourBall_TeamDone(1) && (!lbl_80282240 || (u32)(nPlayer - 2) > 1)) {
        if (GameModeFourBall_TeamBestPossibleScore(1) < GameModeFourBall_TeamBestPossibleScore(0)) {
            return 1;
        }
    }
    nLeft = 0;
    for (h = Game_CurHoleIndex(); h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            nLeft++;
        }
    }
    if (GameModeFourBall_TeamDone(0) && (!lbl_80282240 || (u32)nPlayer > 1)) {
        if (nLeft + GameModeFourBall_TeamMatchWins(1) == GameModeFourBall_TeamMatchWins(0)) {
            if (GameModeFourBall_TeamBestPossibleScore(0) <= GameModeFourBall_TeamBestPossibleScore(1)) {
                return 1;
            }
        }
    }
    if (GameModeFourBall_TeamDone(1) && (!lbl_80282240 || (u32)(nPlayer - 2) > 1)) {
        if (nLeft + GameModeFourBall_TeamMatchWins(0) == GameModeFourBall_TeamMatchWins(1)) {
            if (GameModeFourBall_TeamBestPossibleScore(1) <= GameModeFourBall_TeamBestPossibleScore(0)) {
                return 1;
            }
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

// In a playoff: over once a team is ahead; otherwise (unless
// only checking) the next playoff hole starts. In the round: over when no holes are left and no
// playoff starts, or when a team leads by more than the holes left.
u8 GameModeFourBall_GameFinished(u8 bCheck) {
    int nLeft;
    int h;
    int i;
    if (gpGame->bD4) {
        if (GameModeFourBall_TeamMatchWins(0) != GameModeFourBall_TeamMatchWins(1)) {
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
            return !GameModeFourBall_GoToPlayoff(bCheck);
        }
        if (nLeft + GameModeFourBall_TeamMatchWins(0) < GameModeFourBall_TeamMatchWins(1) ||
            nLeft + GameModeFourBall_TeamMatchWins(1) < GameModeFourBall_TeamMatchWins(0)) {
            return 1;
        }
    }
    return 0;
}

// After the last hole with the match tied: a playoff starts
// (bD5 when the round played all 18 holes).
u8 GameModeFourBall_GoToPlayoff(u8 bCheck) {
    int h;
    int i;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return 0;
        }
    }
    if (GameModeFourBall_TeamMatchWins(0) == GameModeFourBall_TeamMatchWins(1)) {
        if (bCheck) {
            return 1;
        }
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

// The hole goes to the other team when a team conceded, or to a
// team that holed out and cannot be caught (the point goes on players 0 and 2).
void GameModeFourBall_EndHole(void) {
    int nWinner = -1;
    int nHole = Game_CurHoleIndex();
    if (GameModeFourBall_TeamConceded(0)) {
        nWinner = 1;
    } else if (GameModeFourBall_TeamConceded(1)) {
        nWinner = 0;
    } else if (GameModeFourBall_TeamDone(0) &&
               GameModeFourBall_TeamBestPossibleScore(0) < GameModeFourBall_TeamBestPossibleScore(1)) {
        nWinner = 0;
    } else if (GameModeFourBall_TeamDone(1) &&
               GameModeFourBall_TeamBestPossibleScore(1) < GameModeFourBall_TeamBestPossibleScore(0)) {
        nWinner = 1;
    }
    switch (nWinner) {
    case 0:
        gPlayers[0].nModePoints[nHole] = 1;
        gPlayers[0].nHolesWon++;
        return;
    case 1:
        gPlayers[2].nModePoints[nHole] = 1;
        gPlayers[2].nHolesWon++;
        return;
    }
}

// The winning team's human players with a profile get the prize
// money (by the margin).
void GameModeFourBall_EndGame(void) {
    int nPrize;
    Player* p;
    int nLoser;
    int nMargin;
    int nFirst;
    int nWinner;
    int i;
    int nMoney;
    int k;
    if (fn_800E1BBC()) {
        switch (fn_800EC550()) {
        case 0:
            break;
        default:
            return;
        }
        if (GameModeFourBall_TeamMatchWins(0) > GameModeFourBall_TeamMatchWins(1)) {
            nWinner = 0;
            nLoser = 1;
            nMargin = gPlayers[0].nHolesWon - gPlayers[2].nHolesWon;
        } else {
            nWinner = 1;
            nLoser = 0;
            nMargin = gPlayers[2].nHolesWon - gPlayers[0].nHolesWon;
        }
        nMoney = fn_800D37BC(nWinner, nLoser, nMargin, &nPrize);
        if (Team_IsAllHuman(nWinner)) {
            nFirst = 2;
            if (nWinner == 0) {
                nFirst = 0;
            }
            for (k = 0, i = nFirst; k < 2; k++, i++) {
                p = PLAYER(i);
                if (gpSaveData[p->nIndex].bActive && nMoney) {
                    fn_800E4364(0, 0x6B, nPrize, p->nIndex);
                    fn_800D3548(i, nMoney, 0);
                    p->money.n14 += nMoney;
                }
            }
        }
    }
}
