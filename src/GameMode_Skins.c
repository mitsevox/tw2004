// GameMode_Skins.c (TW06's and TW07's name, GameMode_Skins.cpp there, class GameModeSkins): game
// mode 2, Skins. The lowest score on a hole, alone, wins its skin: the hole's money with everything
// carried over (nSkinsWon, totalled in nSkinsTotal); a tie carries the skin over to the next hole.
// When the last hole's skin is carried over, a sudden-death playoff follows on random holes until
// one is won. Humans are paid their skins at the end; a ladder event (LadderedMode.c) played as
// Skins is won by the most skins money. The file ends with three selected-hole helpers that only
// speed golf (GameMode8.c, which follows it) calls.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "game/earnings.h"

void  GameMode4_WinSkinsEvent(void);
void  GameMode4_WinEvent(void);
s32 gSkinsCarryOver;                    // the money carried over by tied holes
s32 gSkinsNumCarryOver;                 // the skins carried over by tied holes

void GameModeSkins_StartGamePreData(void);
void GameModeSkins_SetupNextGolfer(void);
s32  GameModeSkins_GetHonors(int nPlayer);
u8   GameModeSkins_HoleFinished(int nPlayer, u8 bCheck);
u8   GameModeSkins_GameFinished(u8 bCheck);
u8   GameModeSkins_GoToPlayoff(u8 bCheck);
void GameModeSkins_EndHole(void);
void GameModeSkins_EndGame(void);
s32  GameModeSkins_CurrentHoleNumberSkins(void);

// Mode 2's setup (GM_SetModeType): this file's callbacks, b274 cleared, CPU players may concede, no
// mulligans, no split screen, and nothing carried over.
void GameModeSkins_Init(void) {
    gpGame->pfnInit = GameModeSkins_Init;
    gpGame->pfnSetupNextGolfer = GameModeSkins_SetupNextGolfer;
    gpGame->pfnGetHonors = GameModeSkins_GetHonors;
    gpGame->pfnHoleFinished = GameModeSkins_HoleFinished;
    gpGame->pfnGameFinished = GameModeSkins_GameFinished;
    gpGame->pfnGoToPlayoff = GameModeSkins_GoToPlayoff;
    gpGame->pfnEndHole = GameModeSkins_EndHole;
    gpGame->pfnStartGamePreData = GameModeSkins_StartGamePreData;
    gpGame->pfnEndGame = GameModeSkins_EndGame;
    gpGame->b274 = 0;
    gpGame->bAIConcedes = 1;
    gpGame->n4 = 2;
    gpGame->nMulligans = 0;
    gpGame->nC = 4;
    gpGame->n10 = 2;
    gpGame->nDC = 0;
    gSkinsCarryOver = 0;
    gSkinsNumCarryOver = 0;
    gSession.nSplitScreen = 0;
}

// Mode 2's round start (pfnStartGamePreData): nothing is carried over (money or skins).
void GameModeSkins_StartGamePreData(void) {
    gSkinsCarryOver = 0;
    gSkinsNumCarryOver = 0;
}

// Mode 2's next turn (pfnSetupNextGolfer, once every golfer waits): the player pfnGetHonors(5)
// picks becomes the player whose turn it is (lbl_80282278) and goes to the pre-shot state (1);
// every other player waits (19).
void GameModeSkins_SetupNextGolfer(void) {
    int i;
    lbl_80282278 = gpGame->pfnGetHonors(5);
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i == lbl_80282278) {
            GOLFERSTATE_Set(1, i);
        } else {
            GOLFERSTATE_Set(19, i);
        }
    }
}

// Who plays after nPlayer in Skins (pfnGetHonors; 5 = nobody, and nPlayer 5 leaves nobody out).
// Only a player who can still win the hole (fewer strokes than the best holed score) is picked. On
// the tee: a player who won a skin (nSkinsWon), latest hole first, then anyone on the tee;
// otherwise the player farthest from the pin, off the green first.
s32 GameModeSkins_GetHonors(int nPlayer) {
    int i;
    int h;
    CourseInfo* pCourse;
    int nPinSet;
    f32 fBest;
    int nBest;
    s32 nLow;
    f32 dx;
    f32 dz;
    f32 d;
    nLow = 999;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (Player_IsHoled(i) && PLAYER(i)->nStrokes[Game_CurHoleIndex()] < nLow) {
            nLow = PLAYER(i)->nStrokes[Game_CurHoleIndex()];
        }
    }
    for (h = Game_CurHoleIndex() - 1; h >= 0; h--) {
        if (gpGame->bHoleSelected[h]) {
            for (i = 0; i < gNumPlayersSetUp; i++) {
                if (i != nPlayer && Player_OnTee(i) && gPlayers[(u32)i].nSkinsWon[h] != 0 &&
                    PLAYER(i)->nStrokes[Game_CurHoleIndex()] < nLow) {
                    return i;
                }
            }
        }
    }
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && Player_OnTee(i) && PLAYER(i)->nStrokes[Game_CurHoleIndex()] < nLow) {
            return i;
        }
    }
    nBest = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (i != nPlayer && !Player_IsHoled(i) && PLAYER(i)->nStrokes[Game_CurHoleIndex()] < nLow) {
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
        if (i != nPlayer && !Player_IsHoled(i) && PLAYER(i)->ball.nLie != LIE_GREEN_e &&
            PLAYER(i)->nStrokes[Game_CurHoleIndex()] < nLow) {
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
            if (i != nPlayer && !Player_IsHoled(i) && PLAYER(i)->nStrokes[Game_CurHoleIndex()] < nLow) {
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

// Mode 2's hole-over test (pfnHoleFinished): not before someone has holed out; then the hole is
// over once nobody still playing can tie the best holed score or, when two holed players share it,
// beat it. nPlayer and bCheck are not read.
u8 GameModeSkins_HoleFinished(int nPlayer, u8 bCheck) {
    int i;
    int nBest = 5;
    int nSecond = 5;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (Player_IsHoled(i)) {
            if (nBest == 5 || nBest != 5 && PLAYER(i)->nStrokes[Game_CurHoleIndex()] <
                                            gPlayers[nBest].nStrokes[Game_CurHoleIndex()]) {
                nBest = i;
            }
            if (nBest != i && (nSecond == 5 || nSecond != 5 && PLAYER(i)->nStrokes[Game_CurHoleIndex()] <
                                                   gPlayers[nSecond].nStrokes[Game_CurHoleIndex()])) {
                nSecond = i;
            }
        }
    }
    if (nBest == 5) {
        return 0;
    }
    if (nSecond == 5 || nSecond != 5 && gPlayers[nSecond].nStrokes[Game_CurHoleIndex()] >
                                        gPlayers[nBest].nStrokes[Game_CurHoleIndex()]) {
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (!Player_IsHoled(i) && PLAYER(i)->nStrokes[Game_CurHoleIndex()] + 1 <=
                                      gPlayers[nBest].nStrokes[Game_CurHoleIndex()]) {
                return 0;
            }
        }
    } else {
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (!Player_IsHoled(i) && PLAYER(i)->nStrokes[Game_CurHoleIndex()] + 1 <
                                      gPlayers[nBest].nStrokes[Game_CurHoleIndex()]) {
                return 0;
            }
        }
    }
    return 1;
}

// Mode 2's game-over test (pfnGameFinished). In the playoff the game is over once no skin is
// carried over; otherwise, unless bCheck only asks, a new random hole (not the current one) becomes
// the only one selected, every player's scores are cleared and the tie message is queued. Outside
// the playoff the game is over after the last selected hole unless GameModeSkins_GoToPlayoff starts
// a playoff.
u8 GameModeSkins_GameFinished(u8 bCheck) {
    int nLeft;
    int h;
    int i;
    int nHole;
    if (gpGame->bInPlayoff) {
        if (gSkinsNumCarryOver == 0) {
            return 1;
        }
        if (bCheck) {
            return 0;
        }
        gpGame->nPlayoffHoles++;
        for (h = 0; h < 18; h++) {
            gpGame->bHoleSelected[h] = 0;
        }
        nHole = Game_CurHoleIndex();
        while (nHole == Game_CurHoleIndex()) {
            GM_SetCurrentHole(Misc_RandFunc(0) % 18);
        }
        gpGame->bHoleSelected[Game_CurHoleIndex()] = 1;
        // Every player's scores are cleared for the new playoff hole.
        for (i = 0; i < gNumPlayersSetUp; i++) {
            for (h = 0; h < 18; h++) {
                // fake match: one chained assignment (stored right to left, so nStrokes first, as in
                // GameModeSkins_GoToPlayoff) for the original register order
                gPlayers[i].bFairwayHit[h] = gPlayers[i].bGreenInReg[h] = gPlayers[i].n290[h]
                        = gPlayers[i].nSkinsWon[h] =
                    gPlayers[i].nModePoints[h] = gPlayers[i].nPutts[h] = gPlayers[i].nStrokes[h] = 0;
            }
            gPlayers[i].n2D8 = 0;
            gPlayers[i].nLongestDrive = 0;
            gPlayers[i].nLongestPutt = 0;
            gPlayers[i].n308 = 0;
        }
        GUI_GolfersTiedUIMessage();
    } else {
        nLeft = 0;
        for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
            if (gpGame->bHoleSelected[h]) {
                nLeft++;
            }
        }
        if (nLeft == 0) {
            return !GameModeSkins_GoToPlayoff(bCheck);
        }
    }
    return 0;
}

// Starts the Skins playoff after the last selected hole when nobody won that hole's skin (it was
// carried over); bCheck 1 only asks. The playoff notes whether the round was all 18 holes, plays
// one random hole (not the current one) at a time, clears every player's scores and queues the tie
// message. Returns 1 when there is a playoff.
u8 GameModeSkins_GoToPlayoff(u8 bCheck) {
    int h;
    int i;
    int nHole;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return 0;
        }
    }
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (PLAYER(i)->nSkinsWon[Game_CurHoleIndex()] != 0) {
            return 0;
        }
    }
    if (bCheck) {
        return 1;
    }
    gpGame->bPlayoffFullRound = 1;
    for (h = 0; h < 18; h++) {
        if (!gpGame->bHoleSelected[h]) {
            gpGame->bPlayoffFullRound = 0;
        }
    }
    h = 0;
    while (h < 18) {
        gpGame->bHoleSelected[h++] = 0;
    }
    nHole = Game_CurHoleIndex();
    while (nHole == Game_CurHoleIndex()) {
        GM_SetCurrentHole(Misc_RandFunc(0) % 18);
    }
    gpGame->bHoleSelected[Game_CurHoleIndex()] = 1;
    // Every player's scores are cleared for the playoff.
    for (i = 0; i < gNumPlayersSetUp; i++) {
        for (h = 0; h < 18; h++) {
            PLAYER(i)->nStrokes[h] = 0;
            PLAYER(i)->nPutts[h] = 0;
            PLAYER(i)->nModePoints[h] = 0;
            PLAYER(i)->nSkinsWon[h] = 0;
            PLAYER(i)->n290[h] = 0;
            PLAYER(i)->bGreenInReg[h] = 0;
            PLAYER(i)->bFairwayHit[h] = 0;
        }
        PLAYER(i)->n2D8 = 0;
        PLAYER(i)->nLongestDrive = 0;
        PLAYER(i)->nLongestPutt = 0;
        PLAYER(i)->n308 = 0;
    }
    gpGame->bInPlayoff = 1;
    gpGame->nPlayoffHoles++;
    GUI_GolfersTiedUIMessage();
    return 1;
}

// Mode 2's end of hole (pfnEndHole). The lowest holed score alone wins the skin: the hole's value
// with everything carried over (GameModeSkins_CurrentHoleValue) in its nSkinsWon and its
// nSkinsTotal total, the hole marked won and the skins at stake added to its nHolesWon; the
// carry-over starts again. A tie carries the skin over (outside the playoff the hole's value at the
// players' best earnings rating, and one more skin).
void GameModeSkins_EndHole(void) {
    int i;
    int nBest = 5;
    int nSecond = 5;
    s32 n;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (Player_IsHoled(i)) {
            if (nBest == 5 || nBest != 5 && PLAYER(i)->nStrokes[Game_CurHoleIndex()] <
                                            gPlayers[nBest].nStrokes[Game_CurHoleIndex()]) {
                nBest = i;
            }
            if (nBest != i && (nSecond == 5 || nSecond != 5 && PLAYER(i)->nStrokes[Game_CurHoleIndex()] <
                                                   gPlayers[nSecond].nStrokes[Game_CurHoleIndex()])) {
                nSecond = i;
            }
        }
    }
    if (nSecond != 5 && gPlayers[nBest].nStrokes[Game_CurHoleIndex()] ==
                        gPlayers[nSecond].nStrokes[Game_CurHoleIndex()]) {
        if (!gpGame->bInPlayoff) {
            gSkinsCarryOver += GM_Earnings_GetSkinsHoleValue(GM_GetHighestRatedGolfer(), Game_CurHoleIndex());
            gSkinsNumCarryOver++;
        }
    } else {
        n = GameModeSkins_CurrentHoleValue();
        gPlayers[nBest].nSkinsWon[Game_CurHoleIndex()] = n;
        gPlayers[nBest].nSkinsTotal += gPlayers[nBest].nSkinsWon[Game_CurHoleIndex()];
        gPlayers[nBest].nModePoints[Game_CurHoleIndex()] = 1;
        gPlayers[nBest].nHolesWon += GameModeSkins_CurrentHoleNumberSkins();
        gSkinsCarryOver = 0;
        gSkinsNumCarryOver = 0;
    }
}

// Mode 2's end of game (pfnEndGame), outside a Play Now challenge unless it is a ladder event: each
// human player with an active profile is paid their skins money (nSkinsTotal), booked in money.n18,
// with a message (0x6C) when it is not 0; the first such winner counts a game won for the EA Sports
// Bio. In a ladder event, player 0 with more skins money than every other player wins it
// (GameMode4_WinSkinsEvent).
void GameModeSkins_EndGame(void) {
    int i;
    int nProfile;
    u8 bFirst = 1;
    s32 bWon;
    if (!PlayNow_IsChallengeRunning() || GameMode4_IsEventRunning()) {
        for (i = 0; i < gNumPlayersSetUp; i++) {
            if (!Player_IsCPU(i)) {
                nProfile = PLAYER(i)->nIndex;
                if (gpSaveData[nProfile].bActive) {
                    if (PLAYER(i)->nSkinsTotal != 0) {
                        if (bFirst) {
                            EASBio_IncrementGamesWon(1);
                            bFirst = 0;
                        }
                        GUI_QueueMessage(0, 0x6C, PLAYER(i)->nSkinsTotal, nProfile);
                    }
                    GM_Earnings_AwardMoney(i, PLAYER(i)->nSkinsTotal, 0);
                    PLAYER(i)->money.n18 += PLAYER(i)->nSkinsTotal;
                }
            }
        }
        if (GameMode4_IsEventRunning()) {
            bWon = 1;
            for (i = 1; i < gNumPlayersSetUp; i++) {
                if (gPlayers[0].nSkinsTotal <= gPlayers[i].nSkinsTotal) {
                    bWon = 0;
                }
            }
            if (bWon) {
                GameMode4_WinSkinsEvent();
            } else {
                // EA bug: a lost ladder event is scored as won as well: GameMode4_WinEvent marks its
                // award, unlocks its pro and reward (asm 800F923C; GameMode4 calls it only on a win)
                GameMode4_WinEvent();
            }
        }
    }
}

// The money the current skin is worth: what is carried over plus the hole's value at the players'
// best earnings rating (GM_Earnings_GetSkinsHoleValue, GM_GetHighestRatedGolfer). While the
// scorecard is up the next selected hole's value is used; in the playoff only the carry-over
// counts.
s32 GameModeSkins_CurrentHoleValue(void) {
    int h;
    if (gpGame->bInPlayoff) {
        return gSkinsCarryOver;
    }
    if (GUI_ScoreCardUp()) {
        for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
            if (gpGame->bHoleSelected[h]) {
                return gSkinsCarryOver + GM_Earnings_GetSkinsHoleValue(GM_GetHighestRatedGolfer(), h);
            }
        }
    }
    h = Game_CurHoleIndex();
    return gSkinsCarryOver + GM_Earnings_GetSkinsHoleValue(GM_GetHighestRatedGolfer(), h);
}

// Skins at stake on this hole: those carried over, plus one outside the playoff.
s32 GameModeSkins_CurrentHoleNumberSkins(void) {
    if (gpGame->bInPlayoff) {
        return gSkinsNumCarryOver;
    }
    return gSkinsNumCarryOver + 1;
}

// The round's first selected hole, or -1 when none is selected. Only speed golf (GameMode8.c, which
// follows these three functions) calls it.
s32 SpeedGolf_GetFirstSelectedHole(void) {
    int h;
    for (h = 0; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return h;
        }
    }
    return -1;
}

// The first selected hole after hole h, or -1 (GM_GetNextSelectedHole does the same from the
// current hole).
s32 SpeedGolf_GetNextSelectedHole(int h) {
    for (h = h + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return h;
        }
    }
    return -1;
}

// The last selected hole before hole h, or -1.
s32 SpeedGolf_GetPrevSelectedHole(int h) {
    for (h = h - 1; h >= 0; h--) {
        if (gpGame->bHoleSelected[h]) {
            return h;
        }
    }
    return -1;
}
