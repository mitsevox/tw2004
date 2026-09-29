// GameModeBestBall.c (TW06's class GameModeBestBall, gamemode_bestball.cpp; TW07's
// GameMode_BestBall.cpp): game mode 19, two-against-two stroke play where each team counts its
// better ball on every hole. Team 0 is players 0 and 1, team 1 players 2 and 3. The mode's callbacks
// in TW06's method order (Init, TeamDone, GetPartner, SetupNextGolfer, GetHonors, ...), then the
// team scores the scorecard and the round total use (GM_BestBallMode_GetTeamHoleScore,
// GM_BestBallMode_GetTeamRelativeScore). At the end of each hole the ball that does not count gets 9
// strokes (GameModeBestBall_EndHole); a human team that beats an all-CPU team over a full round is
// paid from the CPU golfers' stroke prizes (GameModeBestBall_EndGame).

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "game/earnings.h"

u8   GameModeBestBall_TeamDone(int nTeam);
int  GameModeBestBall_GetPartner(int nPlayer);
void GameModeBestBall_SetupNextGolfer(void);
s32  GameModeBestBall_GetHonors(int nPlayer);
int  GameModeBestBall_GetPlayerTeam(int nPlayer);
u8   GameModeBestBall_HoleFinished(int nPlayer, u8 bCheck);
u8   GameModeBestBall_GameFinished(u8 bCheck);
u8   GameModeBestBall_GoToPlayoff(u8 bCheck);
void GameModeBestBall_EndHole(void);
void GameModeBestBall_EndGame(void);

// Game mode 19's setup (GM_SetModeType): its callbacks; nScoringType 0, one mulligan per player per
// nine (nMulligans 2), nC and n10 4 as in the other team modes, nDC 0, the current hole back to 0
// (GM_SetCurrentHole) and split screen off.
void GameModeBestBall_Init(void) {
    gpGame->pfnInit = GameModeBestBall_Init;
    gpGame->pfnSetupNextGolfer = GameModeBestBall_SetupNextGolfer;
    gpGame->pfnGetHonors = GameModeBestBall_GetHonors;
    gpGame->pfnHoleFinished = GameModeBestBall_HoleFinished;
    gpGame->pfnGameFinished = GameModeBestBall_GameFinished;
    gpGame->pfnGoToPlayoff = GameModeBestBall_GoToPlayoff;
    gpGame->pfnEndHole = GameModeBestBall_EndHole;
    gpGame->pfnEndGame = GameModeBestBall_EndGame;
    gpGame->nScoringType = 0;
    gpGame->nMulligans = 2;
    gpGame->nC = 4;
    gpGame->n10 = 4;
    gpGame->nDC = 0;
    GM_SetCurrentHole(0);
    gSession.nSplitScreen = 0;
}

// Whether team nTeam (0: players 0 and 1; 1: players 2 and 3) is done on the current hole: one
// partner has holed out in no more strokes than the other has so far plus one, so the other can no
// longer beat it.
u8 GameModeBestBall_TeamDone(int nTeam) {
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

// The partner of player nPlayer: 0 and 1 play together, 2 and 3; 5 for any other player.
int GameModeBestBall_GetPartner(int nPlayer) {
    switch (nPlayer) {
    case 0:
        return 1;
    case 1:
        return 0;
    case 2:
        return 3;
    case 3:
        return 2;
    default:
        return 5;
    }
}

// The next shot (pfnSetupNextGolfer): in split screen every golfer gets ready at once
// (GS_PRE_SHOT); otherwise the golfer the mode's honors picks (pfnGetHonors, kept in lbl_80282278)
// gets ready and the others wait (GS_WAIT).
void GameModeBestBall_SetupNextGolfer(void) {
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

// Who plays next after nPlayer (pfnGetHonors; 5 = nobody). On the tee: the team that won the last
// decided hole (a tie keeps the order) goes first, and within each team the player with the better
// score on it; the first in that order who is not nPlayer, is on the tee, not cut and whose team is
// not done (GameModeBestBall_TeamDone). Otherwise the player farthest from the pin among those off
// the green, then among those on it, leaving out nPlayer, holed-out and cut players and done teams;
// 5 when that is nPlayer.
s32 GameModeBestBall_GetHonors(int nPlayer) {
    s32 aOrder[4] = {0, 1, 2, 3};  // the tee order before anyone has a lower team score
    s32* pOrder;        // fake match: the within-team compares read aOrder through a pointer
    int a;
    int b;
    int w;
    int t;
    int i;
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
            a = gPlayers[1].nStrokes[h];
            if (gPlayers[0].nStrokes[h] <= a) {
                a = gPlayers[0].nStrokes[h];
            }
            b = gPlayers[3].nStrokes[h];
            if (gPlayers[2].nStrokes[h] <= b) {
                b = gPlayers[2].nStrokes[h];
            }
            if (a < b) {
                w = 0;
            } else if (b < a) {
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
            t = aOrder[0];
            if (gPlayers[t].nStrokes[h] > gPlayers[pOrder[1]].nStrokes[h]) {
                aOrder[0] = aOrder[1];
                aOrder[1] = t;
            }
            t = aOrder[2];
            if (gPlayers[t].nStrokes[h] > gPlayers[pOrder[3]].nStrokes[h]) {
                aOrder[2] = aOrder[3];
                aOrder[3] = t;
            }
        }
    }
    // fake match: the tee-order loop reuses the hole counter h; a counter of its own gets another
    // register (h, t or w all match)
    for (h = 0; h < gNumPlayersSetUp; h++) {
        if (nPlayer != aOrder[h] && Player_OnTee(aOrder[h]) && !gPlayers[aOrder[h]].bPlayerCut &&
            !GameModeBestBall_TeamDone(GameModeBestBall_GetPlayerTeam(aOrder[h]))) {
            return aOrder[h];
        }
    }
    pCourse = Ter_GetTGD();
    nPinSet = Game_CurrentPinSet();
    fBest = 0.0f;
    nBest = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && !Player_IsHoled(i) && !PLAYER(i)->bPlayerCut
            && !GameModeBestBall_TeamDone(GameModeBestBall_GetPlayerTeam(i)) &&
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
            if (i != nPlayer && !Player_IsHoled(i) && !PLAYER(i)->bPlayerCut &&
                !GameModeBestBall_TeamDone(GameModeBestBall_GetPlayerTeam(i))) {
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

// The team of player nPlayer: 0 for players 0 and 1, 1 for 2 and 3 (TW06 has this as
// GameModeFourBall::GetPlayerTeam; its best ball mode has none).
int GameModeBestBall_GetPlayerTeam(int nPlayer) {
    return nPlayer / 2;
}

// The hole is over (pfnHoleFinished) once both teams are done (GameModeBestBall_TeamDone). nPlayer
// and bCheck are not read.
u8 GameModeBestBall_HoleFinished(int nPlayer, u8 bCheck) {
    int bDone = 0;
    if (GameModeBestBall_TeamDone(0) && GameModeBestBall_TeamDone(1)) {
        bDone = 1;
    }
    return bDone;
}

// The game is over (pfnGameFinished) when no selected hole is left after the current one. bCheck is
// not read.
u8 GameModeBestBall_GameFinished(u8 bCheck) {
    int h;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return 0;
        }
    }
    return 1;
}

// The mode's pfnGoToPlayoff: never a playoff (TW06's GameModeBestBall keeps
// GameModeBase::GoToPlayoff). bCheck is not read.
u8 GameModeBestBall_GoToPlayoff(u8 bCheck) {
    return 0;
}

// The hole is over (pfnEndHole): on each team the ball that does not count gets 9 strokes on the
// hole: of two holed balls the worse (the first player's on a tie), else the one not holed (the
// first player's when neither is).
void GameModeBestBall_EndHole(void) {
    int nHole = Game_CurHoleIndex();
    if (Player_IsHoled(0)) {
        if (Player_IsHoled(1)) {
            if (gPlayers[0].nStrokes[nHole] < gPlayers[1].nStrokes[nHole]) {
                gPlayers[1].nStrokes[nHole] = 9;
            } else {
                gPlayers[0].nStrokes[nHole] = 9;
            }
        } else {
            gPlayers[1].nStrokes[nHole] = 9;
        }
    } else {
        gPlayers[0].nStrokes[nHole] = 9;
    }
    if (Player_IsHoled(2)) {
        if (Player_IsHoled(3)) {
            if (gPlayers[2].nStrokes[nHole] < gPlayers[3].nStrokes[nHole]) {
                gPlayers[3].nStrokes[nHole] = 9;
            } else {
                gPlayers[2].nStrokes[nHole] = 9;
            }
        } else {
            gPlayers[3].nStrokes[nHole] = 9;
        }
    } else {
        gPlayers[2].nStrokes[nHole] = 9;
    }
}

// The round is over (pfnEndGame): after a full round (GM_FullRoundOfGolf) outside a Play Now
// challenge, a human team (Team_IsAllHuman) that beats an all-CPU team (Team_IsAllCPU) on the
// round's total strokes wins money: half of the two CPU golfers' base prizes
// (gEarningsTable.aStrokePrize by GM_Earnings_RateGolfer) plus half of their per-stroke prizes
// times the margin (at most 5 strokes). Each of the team's players with an active profile is paid
// (GM_Earnings_AwardMoney), gets a won game in the EA SPORTS Bio and, when the base half is
// nonzero, message 0x76 with it. EA reuses the team loop's counter for the inner loop, so the team
// loop ends after a team is paid.
void GameModeBestBall_EndGame(void) {
    int i;
    int nFirst;
    int nSum;
    int nTheirs;
    int nMargin;
    int nRating1;
    int nRating2;
    s32 nOurs;
    int nOther2;
    int nMoney;
    int nOther;
    int nPlayer;
    int nProfile;
    int nOtherTeam;
    if (GM_FullRoundOfGolf()) {
        switch (PlayNow_IsChallengeRunning()) {
        case 0:
            break;
        default:
            return;
        }
        for (i = 0; i < 2; i++) {
            if (Team_IsAllHuman(i)) {
                if (i == 0) {
                    nFirst = 0;
                    nOther = 2;
                    nOtherTeam = 1;
                } else {
                    nFirst = 2;
                    nOther = 0;
                    nOtherTeam = 0;
                }
                if (Team_IsAllCPU(nOtherTeam)) {
                    nOurs = GM_GetPlayerRoundScore(nFirst);
                    nOurs += GM_GetPlayerRoundScore(nFirst + 1);
                    nTheirs = GM_GetPlayerRoundScore(nOther);
                    nOther2 = nOther + 1;
                    nTheirs += GM_GetPlayerRoundScore(nOther2);
                    if (nOurs < nTheirs) {
                        nMargin = nTheirs - nOurs;
                        if (nMargin > 5) {
                            nMargin = 5;
                        }
                        nRating1 = GM_Earnings_RateGolfer(nOther);
                        nRating2 = GM_Earnings_RateGolfer(nOther2);
                        nSum = gEarningsTable.aStrokePrize[nRating1].nBase;
                        nSum += gEarningsTable.aStrokePrize[nRating2].nBase;
                        nMoney = nSum + gEarningsTable.aStrokePrize[nRating1].nPerStroke * nMargin;
                        nMoney += gEarningsTable.aStrokePrize[nRating2].nPerStroke * nMargin;
                        // fake match: nOurs (dead here) holds the base prize; TW07 has its own
                        // baseearned local, but a separate local puts it in another register
                        nOurs = nSum / 2;
                        nMoney /= 2;
                        for (i = 0; i < 2; i++) {
                            nPlayer = nFirst + i;
                            nProfile = PLAYER(nPlayer)->nIndex;
                            if (gpSaveData[nProfile].bActive) {
                                EASBio_SetCurrentGameWon(1);
                                if (nOurs) {
                                    GUI_QueueMessage(0, 0x76, nOurs, nProfile);
                                }
                                GM_Earnings_AwardMoney(nPlayer, nMoney, 0);
                            }
                        }
                    }
                }
            }
        }
    }
}

// The strokes of nPlayer's team on hole nHole: the better of nPlayer's and his partner's.
int GM_BestBallMode_GetTeamHoleScore(int nPlayer, int nHole) {
    if (gPlayers[nPlayer].nStrokes[nHole] <= gPlayers[GameModeBestBall_GetPartner(nPlayer)].nStrokes[nHole]) {
        return gPlayers[nPlayer].nStrokes[nHole];
    }
    return gPlayers[GameModeBestBall_GetPartner(nPlayer)].nStrokes[nHole];
}

// nPlayer's team score against par over the selected holes before the current one; with bCurrent,
// the current hole too once nPlayer's ball is in the cup.
int GM_BestBallMode_GetTeamRelativeScore(int nPlayer, u8 bCurrent) {
    int nPar;
    int nScore;
    int h;
    int n;
    nScore = 0;
    nPar = 0;
    n = gpGame->nCurHole;
    if (bCurrent && gPlayers[nPlayer].ball.nLie == LIE_INCUP_e && n < 18) {
        n++;
    }
    for (h = 0; h < n; h++) {
        if (gpGame->bHoleSelected[h]) {
            nPar += GM_GetHoleIndexPar(h);
            nScore += GM_BestBallMode_GetTeamHoleScore(nPlayer, h);
        }
    }
    return nScore - nPar;
}
