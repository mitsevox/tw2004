// GameModeMatch.c (TW06's GameModeMatch, TW07's GameMode_Match.cpp): game mode 1, one-against-one
// match play. A hole goes to the player who holes out in fewer strokes; the match ends once one
// player leads by more holes than are left, with a sudden-death playoff when it is tied after the
// last hole. GameMode4.c (the ladder's matches), GameModeBattle.c and GameModeDriverRTE.c reuse
// most of these callbacks. The file ends with two Play Now helpers on GameMode5.c's challenge list
// (PlayNow_GetCurrentGroup, PlayNow_GetGroupFirstChallenge).

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/modes/challenge.h"
#include "game/save.h"

s32 gMatchPlayoffHonors = 5;            // who has the honor in the playoff (5 = nobody yet)

int  GameModeMatch_GetTeeHonors(int nPlayer);
void GameModeMatch_EndGame(void);

// Mode 1's setup (GM_SetModeType): this file's callbacks, two players (nC and n10 2), CPU players
// may concede, no mulligans, no split screen, and nobody holds the playoff honor yet
// (gMatchPlayoffHonors 5). GameMode4 (the ladder's matches), GameModeBattle and GameModeDriverRTE
// reuse most of the callbacks.
void GameModeMatch_Init(void) {
    gpGame->pfnInit = GameModeMatch_Init;
    gpGame->pfnSetupNextGolfer = GameModeMatch_SetupNextGolfer;
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
    gMatchPlayoffHonors = 5;
    gSession.nSplitScreen = 0;
}

// Match play's next turn (pfnSetupNextGolfer, once every golfer waits): the player pfnGetHonors(5)
// picks becomes the player whose turn it is (lbl_80282278) and goes to the pre-shot state; the
// other waits. Modes 4 and 25 and the real-time events use it too.
void GameModeMatch_SetupNextGolfer(void) {
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

// Who has the honor on the tee, leaving nPlayer out (nPlayer 5 leaves nobody out; 5 = nobody). In
// the playoff: the player who had it when the playoff started (gMatchPlayoffHonors), unless that is
// nPlayer. Otherwise, of the players other than nPlayer, the one who won the latest hole any of
// them won (nModePoints), else the first of them.
int GameModeMatch_GetTeeHonors(int nPlayer) {
    int h;
    int i;
    if (gpGame->bInPlayoff && nPlayer != gMatchPlayoffHonors) {
        return gMatchPlayoffHonors;
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

// Who plays after nPlayer in match play (pfnGetHonors; 5 = nobody): the tee honor
// (GameModeMatch_GetTeeHonors) while that player is on the tee; otherwise, of the other players not
// holed out, the one farthest from the pin off the green, else the farthest on it. With nPlayer 5
// and nobody left to play, 5.
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
    pCourse = Ter_GetTGD();
    nPinSet = Game_CurrentPinSet();
    fBest = 0.0f;
    nBest = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && !Player_IsHoled(i) && PLAYER(i)->ball.nLie != LIE_GREEN_e) {
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
            if (i != nPlayer && !Player_IsHoled(i)) {
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

// Mode 1's hole-over test (pfnHoleFinished). Over when both players have holed out, or when one has
// and the other can no longer halve the hole or, with the holed player dormie (ahead by as many
// holes as are left, this one included), can no longer win it; unless bCheck only asks, the other's
// score then gets a stroke for the putt they did not take. While lbl_80282240 is set, nPlayer's own
// holed ball does not count.
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
            P(i)->nSkinsWon[h] = 0;                      \
            P(i)->n290[h] = 0;                      \
            P(i)->bGreenInReg[h] = 0;                      \
            P(i)->bFairwayHit[h] = 0;                      \
        }                                           \
        P(i)->n2D8 = 0;                             \
        P(i)->nLongestDrive = 0;                             \
        P(i)->nLongestPutt = 0;                             \
        P(i)->n308 = 0;                             \
    }

// Mode 1's game-over test (pfnGameFinished). In the playoff: over once the players have won
// different numbers of holes; otherwise, unless bCheck only asks, the next playoff hole is picked
// (GM_Pick_PlayOffHole), every player's round is cleared and the tie message queued. Outside it:
// after the last selected hole, over unless GameModeMatch_GoToPlayoff starts a playoff; before it,
// over once either player leads by more holes than are left.
u8 GameModeMatch_GameFinished(u8 bCheck) {
    int nLeft;
    int h;
    int i;
    if (gpGame->bInPlayoff) {
        if (gPlayers[0].nHolesWon != gPlayers[1].nHolesWon) {
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
            return !GameModeMatch_GoToPlayoff(bCheck);
        }
        if (gPlayers[1].nHolesWon + nLeft < gPlayers[0].nHolesWon ||
            gPlayers[0].nHolesWon + nLeft < gPlayers[1].nHolesWon) {
            return 1;
        }
    }
    return 0;
}

// Starts the sudden-death playoff after the last selected hole when both players have won as many
// holes; bCheck 1 only asks. The playoff's tee honor is worked out as if on the next hole
// (gMatchPlayoffHonors), the playoff notes whether the round was all 18 holes, GM_Pick_PlayOffHole
// picks the hole, every player's round is cleared and the tie message queued. Returns 1 when there
// is a playoff.
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
        gMatchPlayoffHonors = GameModeMatch_GetTeeHonors(5);
        gpGame->nCurHole--;
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

// Mode 1's end of hole (pfnEndHole): the player who holed out in fewer strokes than the other wins
// the hole (nModePoints 1 on it, one more in nHolesWon); a halved hole changes nothing.
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

// Mode 1's end of game (pfnEndGame), after a full round (GM_FullRoundOfGolf) and outside a Play Now
// challenge: the player with more holes won (player 1 when level) wins by the difference. A human
// winner with an active profile counts a game won for the EA Sports Bio and, when
// GM_Earnings_GetStrokeWinnings gives money, has the prize message (0x6B) queued, the money paid
// and booked (the prize added to money.n14, money.n10 set to money.n24 minus the prize).
void GameModeMatch_EndGame(void) {
    int nPrize;
    int nWinner;
    int nLoser;
    int nMargin;
    int nMoney;
    int nProfile;
    if (GM_FullRoundOfGolf()) {
        switch (PlayNow_IsChallengeRunning()) {
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
                    GUI_QueueMessage(0, 0x6B, nPrize, nProfile);
                    GM_Earnings_AwardMoney(nWinner, nMoney, 0);
                    gPlayers[nWinner].money.n14 += nPrize;
                    gPlayers[nWinner].money.n10 = gPlayers[nWinner].money.n24 - nPrize;
                }
            }
        }
    }
}

// The current Play Now challenge's group (gChallengeList[gCurChallenge].nGroup): the medal slot
// PlayNow_EndGame saves, and the intro and name the HUD shows. Play Now code (GameMode5.c's
// challenge list) though it sits in this file.
s32 PlayNow_GetCurrentGroup(void) {
    return gChallengeList[gCurChallenge].nGroup;
}

// The index in the challenge list (gChallengeList, gNumChallenges entries) of group n's first
// challenge; 0, not -1, when no challenge has that group (PlayNow_GetGroupName tests for -1: see
// the EA bug there).
int PlayNow_GetGroupFirstChallenge(int n) {
    int i;
    int nFound = 0;
    for (i = 0; i < gNumChallenges; i++) {
        if (n == gChallengeList[i].nGroup) {
            nFound = i;
            break;
        }
    }
    return nFound;
}
