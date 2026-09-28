// GameModeFourBall.c (TW06's class GameModeFourBall; TW07's GameMode_FourBall.cpp): game mode 20,
// two-against-two match play where each team counts its better ball on every hole. Team 0 is players
// 0 and 1, team 1 players 2 and 3. The mode's callbacks and the team tests they share (TeamDone,
// TeamConceded, TeamBestPossibleScore, TeamMatchWins: TW06's and TW07's names). A hole goes to the
// team that holes out beyond the other's reach, or whose opponents both concede; its point is
// booked on the team's first player (0 or 2). The match ends once a team leads by more holes than
// are left; level after the last hole, a sudden-death playoff follows. A human team that beats an
// all-CPU team over a full round is paid the team stroke prize (GameModeFourBall_EndGame).
// GameModeBestBall.c (mode 19) is its stroke-play twin.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "game/save.h"

u8   GameModeFourBall_TeamDone(int nTeam);
u8   GameModeFourBall_TeamConceded(int nTeam);
int  GameModeFourBall_TeamBestPossibleScore(int nTeam);
int  GameModeFourBall_TeamMatchWins(int nTeam);
void GameModeFourBall_SetupNextGolfer(void);
s32  GameModeFourBall_GetHonors(int nPlayer);
int  GameModeFourBall_GetPlayerTeam(int nPlayer);
u8   GameModeFourBall_HoleFinished(int nPlayer, u8 bCheck);
u8   GameModeFourBall_GameFinished(u8 bCheck);
u8   GameModeFourBall_GoToPlayoff(u8 bCheck);
void GameModeFourBall_EndHole(void);
void GameModeFourBall_EndGame(void);

// Game mode 20's setup (GM_SetModeType): this file's callbacks; CPU players may concede
// (bAIConcedes), n4 1, no mulligans, nC and n10 4 as in the other team modes, nDC 0 and split
// screen off. Unlike GameModeBestBall_Init it does not reset the current hole.
void GameModeFourBall_Init(void) {
    gpGame->pfnInit = GameModeFourBall_Init;
    gpGame->pfnSetupNextGolfer = GameModeFourBall_SetupNextGolfer;
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

// Whether team nTeam (0: players 0 and 1; 1: players 2 and 3) is done on the current hole: one
// partner has holed out in no more strokes than the other has so far plus one, so the other can no
// longer beat it. The same body as GameModeBestBall_TeamDone.
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

// Whether team nTeam has conceded the current hole: both partners' balls are in the cup
// (Player_IsHoled) with the golfer state GS_CONCEDED (Player_IsHoledNotState23 false).
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

// The best score team nTeam can still make on the current hole, at most 9: the lower of each
// partner's strokes so far plus one (holing the next shot) and a holed partner's strokes.
// GameModeFourBall_HoleFinished and GameModeFourBall_EndHole compare the two teams' with it.
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

// Holes won by team nTeam: nHolesWon of its first player (0 or 2), where GameModeFourBall_EndHole
// books them. The current hole index is read and not used (TW07's has a currHole local too).
int GameModeFourBall_TeamMatchWins(int nTeam) {
    int nHole = Game_CurHoleIndex();
    int a;
    a = 2;
    if (nTeam == 0) {
        a = 0;
    }
    return gPlayers[a].nHolesWon;
}

// The next shot (pfnSetupNextGolfer): the golfer the mode's honors pick (pfnGetHonors(5), nobody
// left out) becomes the player whose turn it is (lbl_80282278) and gets ready (GS_PRE_SHOT); the
// others wait (GS_WAIT). GameModeBestBall_SetupNextGolfer without its split-screen path.
void GameModeFourBall_SetupNextGolfer(void) {
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

// Who plays next after nPlayer (pfnGetHonors; 5 = nobody). On the tee: the team that won the last
// decided hole (nModePoints on player 0 or 2; a halved hole keeps the order) goes first, and within
// that team the player with fewer strokes on the last selected hole (a tie keeps their order; the
// other pair is not reordered); the first in that order who is not nPlayer, is on the tee and whose
// team is not done (GameModeFourBall_TeamDone). Otherwise the player farthest from the pin among
// those off the green, then among all, leaving out nPlayer, holed-out players and done teams; 5
// when that is nPlayer.
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
    pCourse = Ter_GetTGD();
    nPinSet = Game_CurrentPinSet();
    fBest = 0.0f;
    nBest = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && !Player_IsHoled(i) &&
            !GameModeFourBall_TeamDone(GameModeFourBall_GetPlayerTeam(i)) &&
            PLAYER(i)->ball.nLie != LIE_GREEN_e) {
            dx = PLAYER(i)->ball.vPos[0] - pCourse->pin[nPinSet].x;
            dz = PLAYER(i)->ball.vPos[2] - pCourse->pin[nPinSet].z;
            d = Math_Sqrt(dx * dx + dz * dz);
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
                d = Math_Sqrt(dx * dx + dz * dz);
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

// The team of player nPlayer: 0 for players 0 and 1, 1 for 2 and 3.
int GameModeFourBall_GetPlayerTeam(int nPlayer) {
    return nPlayer / 2;
}

// Whether the hole is over (pfnHoleFinished; bCheck is not read): both teams done or either
// conceded; or one team done and the other can no longer beat its best possible score, or can at
// best tie it while the done team is dormie (ahead by as many holes as are left, this one
// included). While lbl_80282240 is set, a done team's own player (nPlayer) does not end it these
// last two ways.
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
            P(i)->bGreenInReg[h] = 0;                      \
            P(i)->bFairwayHit[h] = 0;                      \
        }                                           \
        P(i)->n2D8 = 0;                             \
        P(i)->nLongestDrive = 0;                             \
        P(i)->nLongestPutt = 0;                             \
        P(i)->n308 = 0;                             \
    }

// The match is over (pfnGameFinished). In the playoff: once the teams have won different numbers of
// holes; otherwise, unless bCheck only asks, the next playoff hole is counted and picked
// (GM_Pick_PlayOffHole), every player's round is cleared and the tie message queued. Outside it:
// after the last selected hole, over unless GameModeFourBall_GoToPlayoff starts a playoff; before
// it, over once a team leads by more holes than are left.
u8 GameModeFourBall_GameFinished(u8 bCheck) {
    int nLeft;
    int h;
    int i;
    if (gpGame->bInPlayoff) {
        if (GameModeFourBall_TeamMatchWins(0) != GameModeFourBall_TeamMatchWins(1)) {
            return 1;
        }
        if (!bCheck) {
            gpGame->nPlayoffHoles++;
            GM_Pick_PlayOffHole();
            CLEAR_ROUNDS(PLAYER_AT);
            GUI_GolfersTiedUIMessage();
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

// Starts the sudden-death playoff after the last selected hole when both teams have won as many
// holes; bCheck 1 only asks. The playoff notes whether the round was all 18 holes
// (bPlayoffFullRound), GM_Pick_PlayOffHole picks the hole, every player's round is cleared,
// bInPlayoff is set, the playoff hole counted and the tie message queued. Returns 1 when there is
// (or, asked, would be) a playoff.
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
        gpGame->bPlayoffFullRound = 1;
        for (h = 0; h < 18; h++) {
            if (!gpGame->bHoleSelected[h]) {
                gpGame->bPlayoffFullRound = 0;
            }
        }
        GM_Pick_PlayOffHole();
        CLEAR_ROUNDS(PLAYER);
        gpGame->bInPlayoff = 1;
        gpGame->nPlayoffHoles++;
        GUI_GolfersTiedUIMessage();
        return 1;
    }
    return 0;
}

// The hole is over (pfnEndHole): when a team conceded the other wins it (team 0's concession
// checked first); else a done team whose best possible score the other cannot match wins it. The
// winner gets nModePoints 1 on the hole and one more nHolesWon, both on its first player (0 or 2);
// a halved hole changes nothing.
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

// The match is over (pfnEndGame): after a full round (GM_FullRoundOfGolf) outside a Play Now
// challenge, the team with more holes won (team 1 when level) wins by the difference. When the
// winners are all human, the team winnings (GM_Earnings_GetStrokeWinningsTeam: nonzero only against
// an all-CPU team and without mulligans) go to each of them with an active profile: the prize
// message (0x6B, with the average base prize) queued, the money paid (GM_Earnings_AwardMoney) and
// added to money.n14. Unlike GameModeMatch_EndGame no EA SPORTS Bio win is counted.
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
    if (GM_FullRoundOfGolf()) {
        switch (PlayNow_IsChallengeRunning()) {
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
        nMoney = GM_Earnings_GetStrokeWinningsTeam(nWinner, nLoser, nMargin, &nPrize);
        if (Team_IsAllHuman(nWinner)) {
            nFirst = 2;
            if (nWinner == 0) {
                nFirst = 0;
            }
            for (k = 0, i = nFirst; k < 2; k++, i++) {
                p = PLAYER(i);
                if (gpSaveData[p->nIndex].bActive && nMoney) {
                    GUI_QueueMessage(0, 0x6B, nPrize, p->nIndex);
                    GM_Earnings_AwardMoney(i, nMoney, 0);
                    p->money.n14 += nMoney;
                }
            }
        }
    }
}
