// GameMode8.c (our name): speed golf, the code of game modes 6, 7 and 8 (GM_IsSpeedGolfMode). EA's
// own words: TW06's events SpeedgolfReady / SpeedgolfGo (0x44 / 0x45 here) and TW07's UI calls
// GM_vGetSpeedGolfHoleScore, GM_vGetSpeedGolfWinner and GM_vTimerOut. After each shot the golfer
// runs to the ball as the ball-placement cursor (PlaceBall_UpdateMomentums), faster with a button:
// speed golf's golfer states replace the ball flight (12, and 24), and add a countdown before each
// hole (25) and the end of a player's hole (26). Mode 8 (SpeedGolf_Init) is solo against the clock:
// a hole scores its seconds (Player.n290) plus 3 a stroke, and a full round under the course's
// limits wins a prize (SpeedGolf_GetWinner). Mode 6 (GameMode6.c) is two players at match play: the
// first to hole out wins the hole (SpeedGolfMatch_*). Mode 7 (GameMode7.c) is two players who start
// with 3000 points each and trade points on 42 events (SpeedGolfPoints_*,
// SpeedGolf_TradeEventPoints): one who holes out drains the other's points and plays the hole again
// from the tee until the other finishes, and one whose points run out loses.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "frontend/fe.h"
#include "unsorted/cull.h"

// fake match: the (s8) on GOLFERSTATE_GetCurrentState and the (u8) on GOLFERSTATE_Set's player (see game.h).

// A course's limits for mode 8's prize (SpeedGolf_GetWinner): a round's seconds plus 3 a stroke
// under nLimit5000 wins 5000, under nLimit2500 2500, under nLimit1000 1000.
typedef struct SGCourse {
    s32 nLimit1000;             // 0x0
    s32 nLimit2500;             // 0x4
    s32 nLimit5000;             // 0x8
} SGCourse;

// One of mode 7's events: the flag it sets on the player and the points it moves from the other.
typedef struct SGEvent {
    u64 uFlags;                 // 0x0  or'd into the player's uC48
    s32 nPoints;                // 0x8  negative for a penalty
    u8  unkC[4];
} SGEvent;
// Mode 7's 42 events (SpeedGolf_TradeEventPoints): event n sets bit n of the player's uC48. 0 the
// hole's first shot to fly; 1 the longer first drive (par 4 or 5); 2 holed out while the other still
// plays; 3 first on the green; 4 out of bounds; 5 a lateral hazard; 6 sand; 7 nearer the pin on the
// green; 8 to 11 par, birdie, eagle, albatross; 12 a hole in one on a par 3, 13 on a par 4 or 5, 14
// a second one on the hole; 17 to 19 a long holed putt or shot and 20 a long shot stopping by the
// pin (never given: SpeedGolf_GroundDistance is always 0); 21 and 22 on the green in and under
// regulation; 23 par or better after sand or a hazard; 24 and 25 the replay's drive beats the
// other's or one's own; 26 and 27 the replay's ball on the green nearer the pin than the other's or
// one's own; 28 the replay reaches the green; 29 the replay holed while the other plays; 30 to 32
// out of bounds, hazard and sand on the replay; 33 three holes won in a row, 34 more; 35 a win after
// three or more losses; 37 fewer strokes than the other on the hole; 38 the replay's first shot; 39
// back to the tee (button 0x25); 40 lost (points ran out) and 41 won that way. 15, 16 and 36 are
// not given in this build.
SGEvent gSpeedGolfEvents[42] = {
    {(u64)1 << 0, 50}, {(u64)1 << 1, 100}, {(u64)1 << 2, 150},
    {(u64)1 << 3, 150}, {(u64)1 << 4, -50}, {(u64)1 << 5, -75},
    {(u64)1 << 6, -25}, {(u64)1 << 7, 50}, {(u64)1 << 8, 100},
    {(u64)1 << 9, 200}, {(u64)1 << 10, 500}, {(u64)1 << 11, 2000},
    {(u64)1 << 12, 2000}, {(u64)1 << 13, 15000}, {(u64)1 << 14, 6000},
    {(u64)1 << 15, 200}, {(u64)1 << 16, 500}, {(u64)1 << 17, 250},
    {(u64)1 << 18, 1000}, {(u64)1 << 19, 5000}, {(u64)1 << 20, 500},
    {(u64)1 << 21, 150}, {(u64)1 << 22, 250}, {(u64)1 << 23, 250},
    {(u64)1 << 24, 250}, {(u64)1 << 25, 250}, {(u64)1 << 26, 250},
    {(u64)1 << 27, 100}, {(u64)1 << 28, 200}, {(u64)1 << 29, 250},
    {(u64)1 << 30, -100}, {(u64)1 << 31, -150}, {(u64)1 << 32, -50},
    {(u64)1 << 33, 100}, {(u64)1 << 34, 250}, {(u64)1 << 35, 150},
    {(u64)1 << 36, 500}, {(u64)1 << 37, 100}, {(u64)1 << 38, 50},
    {(u64)1 << 39, -50}, {(u64)1 << 40, 0}, {(u64)1 << 41, 0},
};
// Per event, its commentary line in playlist 4 (0xFFFF none; SpeedGolf_PlayEventComment plays those
// of events 0 to 36 only).
u16 gSpeedGolfEventComments[44] = {
    0x0000, 0x0001, 0x0010, 0x0009, 0x000A, 0x0018, 0x0003, 0x0002, 0x001E, 0x0004, 0x001D,
    0x0019, 0x000E, 0x0011, 0x000E, 0x000C, 0x000F, 0x001A, 0x000B, 0x0013, 0x001B, 0x0005,
    0x0012, 0x000C, 0x0006, 0x001C, 0x000D, 0x0014, 0x0015, 0x0020, 0x0017, 0x0018, 0x0003,
    0x0021, 0x0007, 0x0008, 0x001F, 0xFFFF, 0x0016, 0x0024, 0x0022, 0x0023, 0x0025, 0x0026,
};
// Per course (gpGame->nCurCourse), mode 8's prize limits (SpeedGolf_GetPrizeScores).
SGCourse gSpeedGolfPrizeScores[21] = {
    {750, 675, 600}, {800, 725, 650}, {725, 650, 575}, {800, 725, 650}, {750, 675, 600},
    {800, 725, 650}, {725, 650, 575}, {750, 675, 600}, {750, 675, 600}, {750, 675, 600},
    {750, 675, 600}, {750, 675, 600}, {750, 675, 600}, {750, 675, 600}, {750, 675, 600},
    {750, 675, 600}, {750, 675, 600}, {750, 675, 600}, {750, 675, 600}, {750, 675, 600},
    {750, 675, 600},
};
s32 gSpeedGolfUnused;           // zeroed by modes 6 and 7's setup, read nowhere
s32 gSpeedGolfEventLogCount;    // the next gSpeedGolfEventLog entry (0..99; the UI reads those below)
s32 gSpeedGolfHoleWinEvents;    // how many times event 37 was given; read nowhere
u8  gSpeedGolfSecondHoleTip;    // the round's second hole: run tip 3 (nC3C bit 23)
// The round's first hole: run tips 1 and 2 (nC3C bits 21 and 22); set by modes 7 and 8's round
// setup (GameMode7.c), passed on to gSpeedGolfSecondHoleTip when the hole ends.
u8  gSpeedGolfFirstHoleTips;
// Set every unpaused frame (SpeedGolf_UpdatePlayers); the first mode 7 player to go back to the
// shot state (1) in a frame clears it, and the other waits for the next frame (nC3C bit 20).
u8  gSpeedGolfCanSwitchToShot;
u8    Gaud_GetCommentStatus(void);
void  SpeedGolf_Vec4Sub(f32* pA, f32* pB, f32* pOut);
// The run's pace (fCB4): a button press adds gSpeedGolfPaceBoost; it falls by gSpeedGolfPaceDecay
// a frame, or gSpeedGolfPaceIdleDecay once the button has not been pressed for
// gSpeedGolfPaceIdleTime seconds; it stays within gSpeedGolfPaceMin..gSpeedGolfPaceMax.
f32 gSpeedGolfPaceBoost = 0.13f;
f32 gSpeedGolfPaceDecay = 0.012f;
f32 gSpeedGolfPaceMin = 0.65f;
f32 gSpeedGolfPaceMax = 1.85f;
f32 gSpeedGolfPaceIdleDecay = 0.065f;
f32 gSpeedGolfPaceIdleTime = 0.25f;

// The last 100 events given (SpeedGolf_TradeEventPoints), for the UI's event list.
SGLog gSpeedGolfEventLog[100];
// fake match: a one-entry array, so the compiler loads it where SpeedGolf_RunUpdate compares with it
// instead of folding in its own 1.0f (the original has this constant first in the file's .sdata2)
const f32 lbl_80284708[1] = {1.0f};

void  SpeedGolf_ShowEvent(s32 nSlot, s32 nEvent, s32 nPoints);
void  SpeedGolf_ResetRunDelay(int nPlayer);

u8    SpeedGolf_HoleFinished(int nPlayer, u8 bCheck);
u8    SpeedGolf_GameFinished(u8 bCheck);
void  SpeedGolf_EndHole(void);
s32   SpeedGolf_GetHoleTimeScore(int nPlayer, int nHole);
s32   SpeedGolf_GetTotalTimeScore(int nPlayer);
void  SpeedGolf_CountdownInit(int nPlayer);
void  SpeedGolf_CountdownUpdate(int nPlayer);
void  SpeedGolf_CountdownExit(int nPlayer);
void  SpeedGolf_RunInit(int nPlayer);
void  SpeedGolfPoints_ScoreHoledShotLength(int nPlayer, int nOther);
f32   SpeedGolf_GroundDistance(f32* pA, f32* pB);
void  SpeedGolf_RunUpdate(int nPlayer);
void  SpeedGolf_RunExit(int nPlayer);
void  SpeedGolf_UpdatePlayers(void);
void  SpeedGolf_HoleOverInit(int nPlayer);
void  SpeedGolf_HoleOverUpdate(int nPlayer);
void  SpeedGolf_HoleOverExit(int nPlayer);
void  SpeedGolf_ShowPoints(s32 nSlot, s32 nPoints, s32 bReset);
void  SpeedGolf_SendMessage19(s32 nSlot, s32 n);
void  SpeedGolf_ShowReady(void);
void  SpeedGolf_ShowStopBallPrompt(s32 nSlot, s32 bShow);
void  SpeedGolf_ShowRunTip(s32 nSlot, s32 nTip);
void  SpeedGolf_ShowBallDirection(s32 nSlot, s32 nDir);
void  SpeedGolf_ShowGo(void);
void  SpeedGolf_ShowPointsGain(s32 nSlot, s32 nPoints);
void  SpeedGolf_StartComment(s32 nLine, s32 a);

// Game mode 8, solo speed golf, starts (GM_SetModeType): its callbacks go in, all shared with modes
// 6 and 7 (GameMode6.c, GameMode7.c) apart from this setup, the hole's end (SpeedGolf_HoleFinished,
// SpeedGolf_EndHole) and the game's end (SpeedGolf_GameFinished). No stroke limit, gimmes or
// mulligans, the mode flags b271..b288 and n290 / n294 (no post-shot reactions) 0; gpGame n10, nC
// and n4 1 (n4 1: holes won, not mode 7's points, in SpeedGolf_EndHole and SpeedGolf_EndGame), nDC
// 0; split screen from lbl_8028227C; one player.
void SpeedGolf_Init(void) {
    gpGame->pfnInit = SpeedGolf_Init;
    gpGame->pfnShutdown = SpeedGolf_Shutdown;
    gpGame->pfnStartGamePreData = SpeedGolf_StartGamePreData;
    gpGame->pfnSetupNextGolfer = SpeedGolf_SetupNextGolfer;
    gpGame->pfnGetHonors = SpeedGolf_GetHonors;
    gpGame->pfnHoleFinished = SpeedGolf_HoleFinished;
    gpGame->pfnGameFinished = SpeedGolf_GameFinished;
    gpGame->pfnGoToPlayoff = SpeedGolf_GoToPlayoff;
    gpGame->pfnEndHole = SpeedGolf_EndHole;
    gpGame->pfnEndGame = SpeedGolf_EndGame;
    gpGame->pfnLoadHole = SpeedGolf_LoadHole;
    gpGame->pfn220 = SpeedGolf_Update;
    gpGame->pfn230 = SpeedGolf_RenderBallTarget;
    gpGame->pfnCheckControllerPulled = SpeedGolf_CheckControllerPulled;
    gpGame->pfnSetTimer = SpeedGolf_SetHoleTime;
    gpGame->b271 = 0;
    gpGame->bStrokeLimit = 0;
    gpGame->b273 = 0;
    gpGame->b277 = 0;
    gpGame->bGimmesAllowed = 0;
    gpGame->b279 = 0;
    gpGame->b27F = 0;
    gpGame->b280 = 0;
    gpGame->b281 = 0;
    gpGame->b283 = 0;
    gpGame->bAllowGameBreakers = 0;
    gpGame->b286 = 0;
    gpGame->b288 = 0;
    gpGame->n290 = 0;
    gpGame->n294 = 0;
    gpGame->nMulligans = 0;
    gpGame->n10 = 1;
    gpGame->nC = 1;
    gpGame->nDC = 0;
    gpGame->n4 = 1;
    gSession.nSplitScreen = lbl_8028227C;
    Session_SetNumPlayers(1);
}

// The pfnShutdown of modes 6, 7 and 8 (the mode ends): golfer state 12 gets the normal ball-flight
// functions back (STATEFUNC_Simulate*), and speed golf's own states 24 to 26 are emptied.
void SpeedGolf_Shutdown(void) {
    sGolferStateEngineTable[12].pfnEnter = STATEFUNC_SimulateInit;
    sGolferStateEngineTable[12].pfnUpdate = STATEFUNC_SimulateUpdate;
    sGolferStateEngineTable[12].pfnExit = STATEFUNC_SimulateExit;
    sGolferStateEngineTable[24].pfnEnter = NULL;
    sGolferStateEngineTable[24].pfnUpdate = NULL;
    sGolferStateEngineTable[24].pfnExit = NULL;
    sGolferStateEngineTable[25].pfnEnter = NULL;
    sGolferStateEngineTable[25].pfnUpdate = NULL;
    sGolferStateEngineTable[25].pfnExit = NULL;
    sGolferStateEngineTable[26].pfnEnter = NULL;
    sGolferStateEngineTable[26].pfnUpdate = NULL;
    sGolferStateEngineTable[26].pfnExit = NULL;
}

// Speed golf's golfer states go in as a round starts (mode 6's pfnStartGamePreData; modes 7 and 8
// through GameMode7.c's SpeedGolf_StartGamePreData): states 12 and 24 the shot and the run to the
// ball (SpeedGolf_Run*), 25 the countdown before a hole (SpeedGolf_Countdown*), 26 the end of the
// player's hole (SpeedGolf_HoleOver*). SpeedGolf_Shutdown puts the normal ones back.
void SpeedGolf_SetGolferStates(void) {
    sGolferStateEngineTable[12].pfnEnter = SpeedGolf_RunInit;
    sGolferStateEngineTable[12].pfnUpdate = SpeedGolf_RunUpdate;
    sGolferStateEngineTable[12].pfnExit = SpeedGolf_RunExit;
    sGolferStateEngineTable[24].pfnEnter = SpeedGolf_RunInit;
    sGolferStateEngineTable[24].pfnUpdate = SpeedGolf_RunUpdate;
    sGolferStateEngineTable[24].pfnExit = SpeedGolf_RunExit;
    sGolferStateEngineTable[25].pfnEnter = SpeedGolf_CountdownInit;
    sGolferStateEngineTable[25].pfnUpdate = SpeedGolf_CountdownUpdate;
    sGolferStateEngineTable[25].pfnExit = SpeedGolf_CountdownExit;
    sGolferStateEngineTable[26].pfnEnter = SpeedGolf_HoleOverInit;
    sGolferStateEngineTable[26].pfnUpdate = SpeedGolf_HoleOverUpdate;
    sGolferStateEngineTable[26].pfnExit = SpeedGolf_HoleOverExit;
}

// The pfnSetupNextGolfer of modes 6, 7 and 8 (GM_SetupGolfer_IfAllWaiting, once every golfer
// waits): each golfer in no state (0) or past state 12 whose post-shot UI is not animating goes to
// the pre-shot state (1), or waits (19) once holed; everyone's run flag (nC3C bit 0) is cleared.
void SpeedGolf_SetupNextGolfer(void) {
    int i;
    s8 nState;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        nState = GOLFERSTATE_GetCurrentState(i);
        if (nState < 1 || (nState > 12 && !GUI_IsPostShotUIAnimating(i))) {
            if (!Player_IsHoled(i)) {
                GOLFERSTATE_Set(1, i);
            } else {
                GOLFERSTATE_Set(19, i);
            }
        }
        PLAYER(i)->nC3C &= ~1;
    }
}

// The pfnGetHonors of modes 6, 7 and 8: 5, nobody plays after nPlayer (the golfers all play at
// once).
s32 SpeedGolf_GetHonors(int nPlayer) {
    return 5;
}

// Mode 6's pfnHoleFinished: the hole is over as soon as either player holes out (nPlayer and bCheck
// are not read).
u8 SpeedGolfMatch_HoleFinished(int nPlayer, u8 bCheck) {
    if (Player_IsHoled(0) || Player_IsHoled(1)) {
        return 1;
    }
    return 0;
}

// Mode 6's pfnEndHole: when one player holed out and the other did not, the one who did wins the
// hole (nModePoints 1 for the hole, nHolesWon + 1); both holed is a half.
void SpeedGolfMatch_EndHole(void) {
    if (!Player_IsHoled(0) || !Player_IsHoled(1)) {
        if (Player_IsHoled(0)) {
            gPlayers[0].nModePoints[Game_CurHoleIndex()] = 1;
            gPlayers[0].nHolesWon++;
            return;
        }
        if (Player_IsHoled(1)) {
            // EA bug: player 1's win is marked in player 0's nModePoints (asm 800F9CDC stores
            // through gPlayers, not gPlayers + 1)
            gPlayers[0].nModePoints[Game_CurHoleIndex()] = 1;
            gPlayers[1].nHolesWon++;
        }
    }
}

// Mode 6's pfnGameFinished (bCheck 1 only asks). In a playoff (gpGame->bInPlayoff): over when the
// holes won differ, else (bCheck 0) the next playoff hole is picked. Otherwise over when a player
// leads by more than the selected holes left; with none left, over unless the players are level
// (SpeedGolfMatch_GoToPlayoff, which starts the playoff when bCheck is 0).
u8 SpeedGolfMatch_GameFinished(u8 bCheck) {
    int nLeft;
    int h;
    if (gpGame->bInPlayoff) {
        if (gPlayers[0].nHolesWon != gPlayers[1].nHolesWon) {
            return 1;
        }
        if (!bCheck) {
            GM_Pick_PlayOffHole();
        }
    } else {
        nLeft = 0;
        for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
            if (gpGame->bHoleSelected[h]) {
                nLeft++;
            }
        }
        if (nLeft == 0) {
            return !SpeedGolfMatch_GoToPlayoff(bCheck);
        }
        if (gPlayers[1].nHolesWon + nLeft < gPlayers[0].nHolesWon ||
            gPlayers[0].nHolesWon + nLeft < gPlayers[1].nHolesWon) {
            return 1;
        }
    }
    return 0;
}

// Mode 6's pfnEndGame, for a full round only: the player with more holes won is the winner (player
// 1 when level), GM_Earnings_GetStrokeWinnings works out the money for the margin, and a human
// winner with an active profile gets it (GM_Earnings_AwardMoney, money.n1C), with UI message 0x6B
// for the prize when there is one.
void SpeedGolfMatch_EndGame(void) {
    int nPrize;
    int nWinner;
    int nLoser;
    int nMargin;
    int nMoney;
    int nProfile;
    if (GM_FullRoundOfGolf()) {
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
                if (nPrize) {
                    GUI_QueueMessage(0, 0x6B, nPrize, nProfile);
                }
                GM_Earnings_AwardMoney(nWinner, nMoney, NULL);
                gPlayers[nWinner].money.n1C += nMoney;
            }
        }
    }
}

// Mode 6's pfnGoToPlayoff (also from SpeedGolfMatch_GameFinished): 1 when no selected hole is left
// and the holes won are level, else 0. Unless bCheck only asks, the playoff starts:
// bPlayoffFullRound set when all 18 holes are selected, a playoff hole picked
// (GM_Pick_PlayOffHole), every player's strokes and nModePoints cleared, bInPlayoff (in a playoff)
// set and the tied message shown.
u8 SpeedGolfMatch_GoToPlayoff(u8 bCheck) {
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
        gpGame->bPlayoffFullRound = 1;
        for (h = 0; h < 18; h++) {
            if (!gpGame->bHoleSelected[h]) {
                gpGame->bPlayoffFullRound = 0;
            }
        }
        GM_Pick_PlayOffHole();
        for (i = 0; i < gNumPlayersSetUp; i++) {
            for (h = 0; h < 18; h++) {
                PLAYER(i)->nStrokes[h] = 0;
                PLAYER(i)->nModePoints[h] = 0;
            }
        }
        gpGame->bInPlayoff = 1;
        GUI_GolfersTiedUIMessage();
        return 1;
    }
    return 0;
}

// Mode 7's pfnHoleFinished, also asked by the swing and run code: 1 once both players have finished
// the hole (nC3C bit 3). nPlayer and bCheck are not read.
u8 SpeedGolfPoints_HoleFinished(int nPlayer, u8 bCheck) {
    if ((gPlayers[0].nC3C & 8) && (gPlayers[1].nC3C & 8)) {
        return 1;
    }
    return 0;
}

// Mode 7's pfnGameFinished: the game is over once either player is out (nC3C bit 13, which
// SpeedGolf_HoleOverUpdate sets after that player's or the other's points ran out) or no selected
// hole is left after the current one. bCheck is not read.
u8 SpeedGolfPoints_GameFinished(u8 bCheck) {
    int h;
    if ((gPlayers[0].nC3C & 0x2000) || (gPlayers[1].nC3C & 0x2000)) {
        return 1;
    }
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return 0;
        }
    }
    return 1;
}

// Mode 8's pfnHoleFinished: once player 0 holes out, the hole's time in seconds
// (GM_GetElapsedHoleTime) goes into n290 if none is stored yet (even when bCheck only asks); the
// hole is over when holed or when the time ran out (nC3C bit 26, SpeedGolf_TimerOut). nPlayer and
// bCheck are not read.
u8 SpeedGolf_HoleFinished(int nPlayer, u8 bCheck) {
    if (gPlayers[0].n290[Game_CurHoleIndex()] == 0 && Player_IsHoled(0)) {
        gPlayers[0].n290[Game_CurHoleIndex()] = GM_GetElapsedHoleTime();
    }
    if (Player_IsHoled(0) || (gPlayers[0].nC3C & 0x04000000)) {
        return 1;
    }
    return 0;
}

// Mode 8's pfnGameFinished: over when no selected hole is left after the current one. bCheck is not
// read.
u8 SpeedGolf_GameFinished(u8 bCheck) {
    int h;
    for (h = Game_CurHoleIndex() + 1; h < 18; h++) {
        if (gpGame->bHoleSelected[h]) {
            return 0;
        }
    }
    return 1;
}

// The pfnGoToPlayoff of modes 7 and 8: never a playoff.
u8 SpeedGolf_GoToPlayoff(u8 bCheck) {
    return 0;
}

// Mode 8's pfnEndHole. With gpGame->n4 0 each player's time score for the hole
// (SpeedGolf_GetHoleTimeScore) is worked out and thrown away; with n4 1 (what mode 8 sets) each of
// players 0 and 1 who holed out wins the hole (nModePoints 1, nHolesWon + 1).
void SpeedGolf_EndHole(void) {
    if (gpGame->n4 == 0) {
        SpeedGolf_GetHoleTimeScore(0, Game_CurHoleIndex());
        if (gNumPlayersSetUp > 1) {
            SpeedGolf_GetHoleTimeScore(1, Game_CurHoleIndex());
        }
    } else if (gpGame->n4 == 1) {
        if (Player_IsHoled(0)) {
            gPlayers[0].nModePoints[Game_CurHoleIndex()] = 1;
            gPlayers[0].nHolesWon++;
        }
        if (Player_IsHoled(1)) {
            gPlayers[1].nModePoints[Game_CurHoleIndex()] = 1;
            gPlayers[1].nHolesWon++;
        }
    }
}

// Mode 7's pfnEndHole: both players' points after the hole (nC44) are kept in nC6C for it; the
// scorecard (SpeedGolf_GetHoleScore) and SpeedGolf_HoleOverInit compare them hole by hole.
void SpeedGolfPoints_EndHole(void) {
    gPlayers[0].nC6C[Game_CurHoleIndex()] = gPlayers[0].nC44;
    gPlayers[1].nC6C[Game_CurHoleIndex()] = gPlayers[1].nC44;
}

// The pfnEndGame of modes 7 and 8. Only with gpGame->n4 0 (mode 7): each player's time total
// (SpeedGolf_GetTotalTimeScore) is worked out and thrown away, and the EASB bio records a game won:
// always with two players, alone only with points (nC44) above 0. Mode 8 (n4 1) does nothing here;
// its prize is paid by SpeedGolf_GetWinner.
void SpeedGolf_EndGame(void) {
    if (gpGame->n4 == 0) {
        SpeedGolf_GetTotalTimeScore(0);
        if (gNumPlayersSetUp > 1) {
            SpeedGolf_GetTotalTimeScore(1);
        }
        if (gNumPlayersSetUp == 1) {
            if (gPlayers[0].nC44 > 0) {
                EASBio_SetCurrentGameWon(1);
            }
        } else {
            EASBio_SetCurrentGameWon(1);
        }
    }
}

// A hole's time score: its seconds (n290) plus 30 a stroke. Only SpeedGolf_GetTotalTimeScore's sum
// is ever used (by the UI); the scorecard, SG_Score and the prize count 3 a stroke instead.
s32 SpeedGolf_GetHoleTimeScore(int nPlayer, int nHole) {
    return gPlayers[nPlayer].n290[nHole] + gPlayers[nPlayer].nStrokes[nHole] * 30;
}

// The sum of SpeedGolf_GetHoleTimeScore (seconds plus 30 a stroke) over all 18 holes, selected or
// not; a UI command asks it, and SpeedGolf_EndGame works it out and throws it away.
s32 SpeedGolf_GetTotalTimeScore(int nPlayer) {
    int h;
    s32 n = 0;
    for (h = 0; h < 18; h++) {
        n += SpeedGolf_GetHoleTimeScore(nPlayer, h);
    }
    return n;
}

// The wait before the run to the ball (nC38, 59 frames from SpeedGolf_ResetRunDelay): counts it
// down a frame, stopping at -1. Returns 1 while waiting, 0 on the frame it runs out (the run
// starts), -1 every frame after (running).
s32 SpeedGolf_TickRunDelay(int nPlayer) {
    if (gPlayers[nPlayer].nC38 != -1) {
        gPlayers[nPlayer].nC38--;
    }
    if (gPlayers[nPlayer].nC38 <= 0) {
        return gPlayers[nPlayer].nC38;
    }
    return 1;
}

// Starts the wait before the run to the ball: nC38 59 frames (SpeedGolf_TickRunDelay counts it
// down).
void SpeedGolf_ResetRunDelay(int nPlayer) {
    gPlayers[nPlayer].nC38 = 59;
}

// A hole starts (through GameMode7.c's SpeedGolf_LoadHole, the pfnLoadHole of modes 6, 7 and 8):
// every player's speed golf flags (nC3C) and event flags (uC48) are cleared, the golfer is turned
// to the target (animation 1, emotion updated) and goes to state 25, the countdown.
void SpeedGolf_StartHole(void) {
    int i;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        PLAYER(i)->nC3C = 0;
        PLAYER(i)->uC48 = 0;
        Character_AlignShotWithTarget(i, 1, 1);
        fn_80095744(PLAYER(i)->pChar, 1);
        Emotion_UpdatePlayerEmotion(i);
        GOLFERSTATE_Set(25, i);
    }
}

// Golfer state 25 (the countdown before a hole's first shot), enter. The player's HUD slot nC58 is
// 2 for player 0 and 3 for player 1; the arrow, tip and stop-ball prompt are cleared; the run tips
// are picked (the round's first hole: nC3C bits 21 and 22, the second: bit 23). The golfer is moved
// to the ball with the shot planned, both of the player's views follow the player, the golfer is
// aimed and shown (animation 5) and camera mode 12 goes on. A 74-frame countdown starts (nC54, nC3C
// bit 1) with "ready" shown (SpeedGolf_ShowReady), the clock reset (message 17, 3), the player's
// panel and HUD messages sent, the run and deferred-switch flags (bits 0 and 20) off and the points
// shown; the event log starts again (gSpeedGolfEventLogCount 0).
void SpeedGolf_CountdownInit(int nPlayer) {
    int i;
    if (nPlayer == 0) {
        gPlayers[nPlayer].nC58 = 2;
    } else {
        gPlayers[nPlayer].nC58 = 3;
    }
    SpeedGolf_ShowBallDirection(gPlayers[nPlayer].nC58, 0);
    SpeedGolf_ShowRunTip(gPlayers[nPlayer].nC58, 0);
    SpeedGolf_ShowStopBallPrompt(gPlayers[nPlayer].nC58, 0);
    gPlayers[nPlayer].nC3C &= ~0x02000000;
    if (gSpeedGolfFirstHoleTips) {
        gPlayers[nPlayer].nC3C |= 0x600000;
    } else if (gSpeedGolfSecondHoleTip) {
        gPlayers[nPlayer].nC3C |= 0x800000;
    }
    GM_MovePlayerToBall(nPlayer);
    Shot_Plan(nPlayer, 1);
    for (i = 0; i < 2; i++) {
        ViewController_SetActivePlayerNumber(gPlayers[nPlayer].nView[i], nPlayer);
    }
    Character_PrepareForRendering(nPlayer);
    Character_AlignShotWithTarget(nPlayer, 1, 1);
    fn_800957D8(gPlayers[nPlayer].pChar);
    fn_80095744(gPlayers[nPlayer].pChar, 5);
    Emotion_UpdatePlayerEmotion(nPlayer);
    fn_80062F1C(ViewController_GetCameraControl(gPlayers[nPlayer].nView[0]));
    i = gPlayers[nPlayer].nView[0];
    CameraController_SetCameraMode(ViewController_GetCameraControl(i), 12, nPlayer, i);
    gPlayers[nPlayer].nC54 = 74;
    gPlayers[nPlayer].nC3C |= 2;
    SpeedGolf_ShowReady();
    GameModeSkillZoneTimed_SetHudClock(3);
    if (gPlayers[nPlayer].nC58 == 2) {
        GUI_ShowTogglePlayer1UI(1);
    } else {
        GUI_ShowTogglePlayer2UI(1);
    }
    SpeedGolf_SendMessage19(gPlayers[nPlayer].nC58, 1);
    fn_80062CB0(gPlayers[nPlayer].nC58, 1);
    fn_80062C80(gPlayers[nPlayer].nC58, 0);
    gPlayers[nPlayer].nC3C &= ~0x100001;
    SpeedGolf_ShowPoints(gPlayers[nPlayer].nC58, gPlayers[nPlayer].nC44, 1);
    gSpeedGolfEventLogCount = 0;
}

// Golfer state 25, update: the countdown (nC54) runs; player 0 triggers event 0x44 (TW06's
// SpeedgolfReady) once (nC3C bit 12) and plays comment 0x27 with 59 frames left. At 0 player 0
// shows "go" (SpeedGolf_ShowGo), starts the clock (message 17, 2), plays comment 0x26 and triggers
// event 0x45 (SpeedgolfGo); every player then gets 44 frames with bit 2 in place of bit 1
// (SpeedGolfPoints_ClearStartFlags ends them in mode 7) and goes to the shot setup (state 2) with
// the swing HUD on.
void SpeedGolf_CountdownUpdate(int nPlayer) {
    gPlayers[nPlayer].nC54--;
    if (gPlayers[nPlayer].nC54 == 59 && nPlayer == 0) {
        SpeedGolf_StartComment(0x27, 2);
    }
    if (gPlayers[nPlayer].nC54 <= 0) {
        if (gPlayers[nPlayer].nC3C & 2) {
            if (nPlayer == 0) {
                SpeedGolf_ShowGo();
                GameModeSkillZoneTimed_SetHudClock(2);
                SpeedGolf_StartComment(0x26, 2);
                EVENT_Trigger(nPlayer, 0x45, 0, 0);
            }
            gPlayers[nPlayer].nC54 = 44;
            gPlayers[nPlayer].nC3C ^= 6;
            GOLFERSTATE_Set(2, nPlayer);
            fn_80062C80(gPlayers[nPlayer].nC58, 1);
        }
    } else if (nPlayer == 0) {
        if (!(gPlayers[nPlayer].nC3C & 0x1000)) {
            gPlayers[nPlayer].nC3C |= 0x1000;
            EVENT_Trigger(nPlayer, 0x44, 0, 0);
        }
    }
}

// Golfer state 25 (the countdown), exit: empty in this build.
void SpeedGolf_CountdownExit(int nPlayer) {
}

// Mode 7's pfn228 (every frame of the swing state): counts nC54 down, and once it runs out with
// nC3C bit 2 set (the frames after the countdown's "go"), clears bits 1 and 2.
void SpeedGolfPoints_ClearStartFlags(int nPlayer) {
    gPlayers[nPlayer].nC54--;
    if (gPlayers[nPlayer].nC54 <= 0 && (gPlayers[nPlayer].nC3C & 4)) {
        gPlayers[nPlayer].nC3C &= ~6;
    }
}

// Golfer states 12 and 24 (the shot and the run), enter: the ball is kept in ballBefore (with no
// player), the ball's reaction event and emotion flags are cleared (fn_8006ACF8, fn_8006BAA8), the
// wait before the run starts (SpeedGolf_ResetRunDelay) and nC40 keeps the ball's lie (not read
// anywhere).
void SpeedGolf_RunInit(int nPlayer) {
    Player* p = &gPlayers[nPlayer];
    Mem_cpy(&p->ballBefore, &p->ball, sizeof(Ball));
    p->ballBefore.nPlayer = -1;
    fn_8006ACF8(nPlayer, 0);
    fn_8006BAA8(nPlayer);
    SpeedGolf_ResetRunDelay(nPlayer);
    gPlayers[nPlayer].nC40 = gPlayers[nPlayer].ball.nLie;
}

// The commentary line for event nEvent (gSpeedGolfEventComments, playlist 4): only events 0 to 36
// have one, and 0xFFFF is none.
void SpeedGolf_PlayEventComment(int nEvent) {
    // Read before the range check, but in bounds: the table has 44 entries and no caller passes an
    // event above 41 (0x29).
    u16 nSound = gSpeedGolfEventComments[nEvent];
    if (nEvent >= 37 || nSound == 0xFFFF) {
        return;
    }
    SpeedGolf_StartComment(nSound, 1);
}

// Mode 7's scoring: player nPlayer gets event nEvent (gSpeedGolfEvents): its flag is or'd into
// uC48, its points (negative for a penalty) are added to the player's (nC44) and taken from the
// other's, each shown on the HUD, with the event's popup (SpeedGolf_ShowEvent), an entry in
// gSpeedGolfEventLog and its commentary line. Ignored while the player is out or going out (nC3C
// bits 13, 14), except events 40 and 41. A player whose points reach 0 loses: 0 for them, 6000 for
// the other, both flagged (the loser nC3C bits 14 and 15, the winner 14 and 16) and sent to state
// 26. Event 37 is also counted (gSpeedGolfHoleWinEvents) and event 39 turns the swing HUD on.
void SpeedGolf_TradeEventPoints(int nPlayer, int nEvent) {
    int nOther;
    s32 nPoints;
    // gSpeedGolfEvents has 42 rows, so the bound 0x2A lets one past the end through; no caller passes
    // more than 0x29.
    if ((!(gPlayers[nPlayer].nC3C & 0x6000) || nEvent == 0x28 || nEvent == 0x29) && nEvent <= 0x2A) {
        if (nEvent == 0x25) {
            gSpeedGolfHoleWinEvents++;
        }
        if (nEvent == 0x27) {
            fn_80062C80(gPlayers[nPlayer].nC58, 1);
        }
        nOther = nPlayer ? 0 : 1;
        nPoints = gSpeedGolfEvents[nEvent].nPoints;
        gPlayers[nPlayer].uC48 |= gSpeedGolfEvents[nEvent].uFlags;
        gPlayers[nPlayer].nC44 += nPoints;
        if (gPlayers[nPlayer].nC44 <= 0 && !(gPlayers[nPlayer].nC3C & 0x2000) &&
            !(gPlayers[nPlayer].nC3C & 0x8000)) {
            gPlayers[nPlayer].nC44 = 0;
            gPlayers[nOther].nC44 = 6000;
            gPlayers[nPlayer].nC3C |= 0xC000;
            gPlayers[nOther].nC3C |= 0x10000 | 0x4000;
            if ((s8)GOLFERSTATE_GetCurrentState(nOther) != 26) {
                GOLFERSTATE_Set(26, (u8)nOther);
            }
            if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) != 26) {
                GOLFERSTATE_Set(26, (u8)nPlayer);
            }
        } else {
            SpeedGolf_ShowPoints(gPlayers[nPlayer].nC58, gPlayers[nPlayer].nC44, 0);
            SpeedGolf_ShowEvent(gPlayers[nPlayer].nC58, nEvent, nPoints);
            gSpeedGolfEventLog[gSpeedGolfEventLogCount].nEvent = nEvent;
            gSpeedGolfEventLog[gSpeedGolfEventLogCount].nPlayer = nPlayer;
            if (++gSpeedGolfEventLogCount >= 100) {
                gSpeedGolfEventLogCount = 0;
            }
            SpeedGolf_PlayEventComment(nEvent);
            if (nPoints != 0) {
                gPlayers[nOther].nC44 -= nPoints;
                SpeedGolf_ShowPoints(gPlayers[nOther].nC58, gPlayers[nOther].nC44, 0);
                if (gPlayers[nOther].nC44 <= 0) {
                    if (!(gPlayers[nOther].nC3C & 0x6000) && !(gPlayers[nOther].nC3C & 0x8000)) {
                        gPlayers[nOther].nC44 = 0;
                        gPlayers[nPlayer].nC44 = 6000;
                        gPlayers[nOther].nC3C |= 0xC000;
                        gPlayers[nPlayer].nC3C |= 0x10000 | 0x4000;
                        if ((s8)GOLFERSTATE_GetCurrentState(nOther) != 26) {
                            GOLFERSTATE_Set(26, (u8)nOther);
                        }
                        if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) != 26) {
                            GOLFERSTATE_Set(26, (u8)nPlayer);
                        }
                    }
                }
            }
        }
    }
}

// A shot has come to rest (SpeedGolf_RunUpdate, ball state 1, or 5 out of bounds). First a return
// to the shot state that mode 7 deferred (nC3C bit 20) happens once gSpeedGolfCanSwitchToShot
// allows. The stroke is counted. Out of bounds (GM_CheckForBallOOB) the ball is dropped from a
// lateral hazard (surface class 7) or replaced, the run flag, arrow, tip and prompt are cleared and
// the golfer goes back to state 1 (in mode 7 now if gSpeedGolfCanSwitchToShot allows, else deferred
// through bit 20), with mode 7's penalties: event 4 out of bounds, 5 in a hazard (nC3C bit 11), 30
// and 31 on the replay. In mode 7 a tee shot (vBall at the tee spot vA44) records the drive's
// length in fC50 (nC3C bit 5): the longer first drive on a par 4 or 5 gets event 1; the replay's
// drive gets event 24 when longer than the other's best (taking their drive flags) or 25 when
// longer than one's own. Returns 0 when the golfer goes back to state 1 (or waits to), else 1.
u8 SpeedGolf_OnBallAtRest(int nPlayer) {
    int nOther;
    f32 fDist;
    SurfaceType* pSurf;
    int nStrokes;               // read and never used
    int nOtherStrokes;          // read and never used
    f32 fX;
    f32 fZ;
    f32 dx;
    f32 dz;
    if (gSpeedGolfCanSwitchToShot && (gPlayers[nPlayer].nC3C & 0x100000)) {
        gSpeedGolfCanSwitchToShot = 0;
        gPlayers[nPlayer].nC3C &= ~0x100000;
        GOLFERSTATE_Switch(1, nPlayer);
        return 0;
    }
    if (Game_GetMode() != 7) {
        GM_PlayerAddStroke(nPlayer);
        if (GM_CheckForBallOOB(nPlayer)) {
            pSurf = Ter_GetSupportingWorldMaterial(gPlayers[nPlayer].ball.pCourse,
                                                   gPlayers[nPlayer].ball.vPos);
            if (pSurf != NULL && pSurf->nClass == 7) {
                GM_ReplaceLateralHazardBall(nPlayer);
                gPlayers[nPlayer].nC3C &= ~1;
            } else {
                gPlayers[nPlayer].nC3C &= ~1;
                GM_ReplaceOOBBall(nPlayer);
            }
            SpeedGolf_ShowBallDirection(gPlayers[nPlayer].nC58, 0);
            SpeedGolf_ShowRunTip(gPlayers[nPlayer].nC58, 0);
            SpeedGolf_ShowStopBallPrompt(gPlayers[nPlayer].nC58, 0);
            GOLFERSTATE_Switch(1, nPlayer);
            return 0;
        }
        return 1;
    }
    nOther = nPlayer ? 0 : 1;
    GM_PlayerAddStroke(nPlayer);
    nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    nOtherStrokes = gPlayers[nOther].nStrokes[Game_CurHoleIndex()];
    if (GM_CheckForBallOOB(nPlayer)) {
        pSurf = Ter_GetSupportingWorldMaterial(gPlayers[nPlayer].ball.pCourse, gPlayers[nPlayer].ball.vPos);
        if (pSurf != NULL && pSurf->nClass == 7) {
            if (gPlayers[nPlayer].nC3C & 0x10) {
                SpeedGolf_TradeEventPoints(nPlayer, 0x1F);
            } else {
                SpeedGolf_TradeEventPoints(nPlayer, 5);
            }
            GM_ReplaceLateralHazardBall(nPlayer);
            gPlayers[nPlayer].nC3C &= ~1;
            gPlayers[nPlayer].nC3C |= 0x800;
        } else {
            if (gPlayers[nPlayer].nC3C & 0x10) {
                SpeedGolf_TradeEventPoints(nPlayer, 0x1E);
            } else {
                SpeedGolf_TradeEventPoints(nPlayer, 4);
            }
            gPlayers[nPlayer].nC3C &= ~1;
            GM_ReplaceOOBBall(nPlayer);
        }
        SpeedGolf_ShowBallDirection(gPlayers[nPlayer].nC58, 0);
        SpeedGolf_ShowRunTip(gPlayers[nPlayer].nC58, 0);
        SpeedGolf_ShowStopBallPrompt(gPlayers[nPlayer].nC58, 0);
        if (gSpeedGolfCanSwitchToShot) {
            GOLFERSTATE_Switch(1, nPlayer);
            gSpeedGolfCanSwitchToShot = 0;
        } else {
            gPlayers[nPlayer].nC3C |= 0x100000;
        }
        return 0;
    }
    fX = gPlayers[nPlayer].vBall[0];
    fZ = gPlayers[nPlayer].vBall[2];
    if (fX == gPlayers[nPlayer].vA44[0] && fZ == gPlayers[nPlayer].vA44[2]) {
        dx = gPlayers[nPlayer].ball.vPos[0] - fX;
        dz = gPlayers[nPlayer].ball.vPos[2] - fZ;
        fDist = Math_Sqrt(dx * dx + dz * dz);
        if (!(gPlayers[nPlayer].nC3C & 0x20)) {
            gPlayers[nPlayer].fC50 = fDist;
            gPlayers[nPlayer].nC3C |= 0x20;
            if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == 1) {
                if ((gPlayers[nOther].nC3C & 0x20) && Course_GetCurHolePar() > 3) {
                    if (gPlayers[nPlayer].fC50 > gPlayers[nOther].fC50) {
                        SpeedGolf_TradeEventPoints(nPlayer, 1);
                    } else if (gPlayers[nPlayer].fC50 < gPlayers[nOther].fC50) {
                        SpeedGolf_TradeEventPoints(nOther, 1);
                    }
                }
            }
        } else if (gPlayers[nPlayer].nC3C & 8) {
            if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == gPlayers[nPlayer].nC60 + 1 &&
                Course_GetCurHolePar() > 3) {
                if (gPlayers[nOther].uC48 & 0x03000002) {
                    if (fDist > gPlayers[nOther].fC50) {
                        gPlayers[nOther].uC48 &= ~(u64)0x03000002;
                        SpeedGolf_TradeEventPoints(nPlayer, 0x18);
                        gPlayers[nPlayer].fC50 = fDist;
                    }
                } else if (fDist > gPlayers[nPlayer].fC50) {
                    SpeedGolf_TradeEventPoints(nPlayer, 0x19);
                    gPlayers[nPlayer].fC50 = fDist;
                }
            }
        }
    }
    return 1;
}

// Mode 7: the events of holing out in nStrokes (on a replay, its own strokes). Event 23 at or under
// par after sand or a hazard drop on the hole (nC3C bits 10 and 11, then cleared); a hole in one is
// event 12 on a par 3 and 13 otherwise, plus 14 when the player already had one on this hole (the
// first sets nC3C bit 9; the flags are cleared as each hole starts); otherwise par, birdie, eagle
// or albatross are events 8 to 11.
void SpeedGolfPoints_ScoreHoleResult(int nPlayer, int nStrokes) {
    int nPar;
    int nUnder;
    nPar = Course_GetCurHolePar();
    nUnder = nPar - nStrokes;
    if (nUnder >= 0 && (gPlayers[nPlayer].nC3C & 0xC00)) {
        SpeedGolf_TradeEventPoints(nPlayer, 0x17);
    }
    gPlayers[nPlayer].nC3C &= ~0xC00;
    if (nStrokes == 1) {
        if (nPar == 3) {
            SpeedGolf_TradeEventPoints(nPlayer, 0xC);
        } else {
            SpeedGolf_TradeEventPoints(nPlayer, 0xD);
        }
        if (gPlayers[nPlayer].nC3C & 0x200) {
            SpeedGolf_TradeEventPoints(nPlayer, 0xE);
        } else {
            gPlayers[nPlayer].nC3C |= 0x200;
        }
    } else if (nUnder >= 0) {
        switch (nUnder) {
        case 0:
            SpeedGolf_TradeEventPoints(nPlayer, 8);
            break;
        case 1:
            SpeedGolf_TradeEventPoints(nPlayer, 9);
            break;
        case 2:
            SpeedGolf_TradeEventPoints(nPlayer, 0xA);
            break;
        case 3:
            SpeedGolf_TradeEventPoints(nPlayer, 0xB);
            break;
        }
    }
}

// Mode 7: the holing shot's length (from vPreShot to the ball): a putt of 20/3 or more is event 17;
// any other shot of 10 or more is event 18, or 19 from 60. The unit is not proven (in yards, 20/3
// would be 20 feet). SpeedGolf_GroundDistance always returns 0, so none of these fire. nOther is
// not read.
void SpeedGolfPoints_ScoreHoledShotLength(int nPlayer, int nOther) {
    f32 fDist = SpeedGolf_GroundDistance(gPlayers[nPlayer].vPreShot, gPlayers[nPlayer].ball.vPos);
    if (gPlayers[nPlayer].nClub == CLUB_PUTTER_e) {
        if (fDist >= 20.0f / 3.0f) {
            SpeedGolf_TradeEventPoints(nPlayer, 0x11);
        }
    } else if (fDist >= 10.0f) {
        if (fDist >= 60.0f) {
            SpeedGolf_TradeEventPoints(nPlayer, 0x13);
        } else {
            SpeedGolf_TradeEventPoints(nPlayer, 0x12);
        }
    }
}

// Meant as the distance from pA to pB on the ground (x and z); it is always 0 (see below).
f32 SpeedGolf_GroundDistance(f32* pA, f32* pB) {
    f32 v[4];
    SpeedGolf_Vec4Sub(pB, pB, v);     // EA bug: pB less itself, so the distance is always 0
    return Math_Sqrt(v[0] * v[0] + v[2] * v[2]);
}

// A human's sticks for the run: the pad's stick bytes 3, 2, 0 and 1 (Input_sGetStickInfo), beyond a
// 96..160 dead zone and scaled to about -1..1, go into fA84, fA80, fA7C and fA8C (the last two
// negated); 0 inside the dead zone. PlaceBall_UpdateMomentums moves the runner (the placement
// point) by fA80 and fA84 and turns its heading by fA7C.
void SpeedGolf_ReadSticks(int nPlayer) {
    u8* pPad = Input_sGetStickInfo(gPlayers[nPlayer].nController);
    if (pPad) {
        if (pPad[3] < 96.0f) {
            gPlayers[nPlayer].fA84 = (96.0f - pPad[3]) / 96.0f;
        } else if (pPad[3] > 160.0f) {
            gPlayers[nPlayer].fA84 = (160.0f - pPad[3]) / 96.0f;
        } else {
            gPlayers[nPlayer].fA84 = 0.0f;
        }
        if (pPad[2] < 96.0f) {
            gPlayers[nPlayer].fA80 = (96.0f - pPad[2]) / 96.0f;
        } else if (pPad[2] > 160.0f) {
            gPlayers[nPlayer].fA80 = (160.0f - pPad[2]) / 96.0f;
        } else {
            gPlayers[nPlayer].fA80 = 0.0f;
        }
        if (pPad[0] < 96.0f) {
            gPlayers[nPlayer].fA7C = -((96.0f - pPad[0]) / 96.0f);
        } else if (pPad[0] > 160.0f) {
            gPlayers[nPlayer].fA7C = -((160.0f - pPad[0]) / 96.0f);
        } else {
            gPlayers[nPlayer].fA7C = 0.0f;
        }
        if (pPad[1] < 96.0f) {
            gPlayers[nPlayer].fA8C = -(96.0f - pPad[1]) / 96.0f;
        } else if (pPad[1] > 160.0f) {
            gPlayers[nPlayer].fA8C = -(160.0f - pPad[1]) / 96.0f;
        } else {
            gPlayers[nPlayer].fA8C = 0.0f;
        }
    }
}

// A human player's controls each frame of the run (SpeedGolf_RunUpdate), until both players have
// finished the hole (SpeedGolfPoints_HoleFinished): the sticks (SpeedGolf_ReadSticks); buttons 0x1A
// or 0x1B, and 0x1C or 0x1D, trigger events 0x16 to 0x19 (EVENT_Trigger). The run tips
// (SpeedGolf_ShowRunTip): with nC3C bit 21 (the first hole) tip 1 at once unless the stick is
// already moving; with bit 22 or 23, tip 2 or 3 as soon as the stick moves, shown for 239 frames
// (bit 24); otherwise tip 1 comes back after 179 frames without the stick and goes when it moves.
// The pace fCB4 rises by gSpeedGolfPaceBoost with button 0x24 (nCB8 back to 0), else falls a frame
// by gSpeedGolfPaceDecay, or gSpeedGolfPaceIdleDecay once nCB8 passes gSpeedGolfPaceIdleTime
// seconds, scaled by the frame time; nCB8 only counts up in mode 7 (SpeedGolf_UpdatePlayers), so in
// mode 8 the pace always falls at the slow rate.
void SpeedGolf_UpdateHumanRun(int nPlayer) {
    f32 fStep;
    if (!SpeedGolfPoints_HoleFinished(nPlayer, 1)) {
        SpeedGolf_ReadSticks(nPlayer);
        if (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0x1A, 1)) {
            EVENT_Trigger(nPlayer, 0x16, 0, -1);
        } else if (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0x1B, 1)) {
            EVENT_Trigger(nPlayer, 0x17, 0, -1);
        }
        if (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0x1C, 1)) {
            EVENT_Trigger(nPlayer, 0x18, 0, -1);
        } else if (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0x1D, 1)) {
            EVENT_Trigger(nPlayer, 0x19, 0, -1);
        }
        if (gPlayers[nPlayer].nC3C & 0x200000) {
            gPlayers[nPlayer].nC3C &= ~0x200000;
            if (gPlayers[nPlayer].fA80 != 0.0f || gPlayers[nPlayer].fA84 != 0.0f) {
                gPlayers[nPlayer].nC54 = 179;
            } else {
                SpeedGolf_ShowRunTip(gPlayers[nPlayer].nC58, 1);
            }
        } else if (gPlayers[nPlayer].nC3C & 0xC00000) {
            if (gPlayers[nPlayer].fA80 != 0.0f || gPlayers[nPlayer].fA84 != 0.0f) {
                if (gPlayers[nPlayer].nC3C & 0x400000) {
                    SpeedGolf_ShowRunTip(gPlayers[nPlayer].nC58, 2);
                    gPlayers[nPlayer].nC3C &= ~0x400000;
                } else {
                    SpeedGolf_ShowRunTip(gPlayers[nPlayer].nC58, 3);
                    gPlayers[nPlayer].nC3C &= ~0x800000;
                }
                gPlayers[nPlayer].nC3C |= 0x1000000;
                gPlayers[nPlayer].nC54 = 239;
            }
        } else if (gPlayers[nPlayer].nC3C & 0x1000000) {
            gPlayers[nPlayer].nC54--;
            if (gPlayers[nPlayer].nC54 == 0) {
                SpeedGolf_ShowRunTip(gPlayers[nPlayer].nC58, 0);
                gPlayers[nPlayer].nC3C &= ~0x1000000;
            }
        } else if (gPlayers[nPlayer].fA80 != 0.0f || gPlayers[nPlayer].fA84 != 0.0f) {
            gPlayers[nPlayer].nC54 = 179;
            SpeedGolf_ShowRunTip(gPlayers[nPlayer].nC58, 0);
        } else {
            gPlayers[nPlayer].nC54--;
            if (gPlayers[nPlayer].nC54 == 0) {
                SpeedGolf_ShowRunTip(gPlayers[nPlayer].nC58, 1);
            }
        }
        fStep = FRAME_RATE / 60.0f * (FRAME_RATE * gSession.fFrameTime);
        if (Input_ReadControlPad(gPlayers[nPlayer].nController) & Controller_GetButtonMask(0x24, 0)) {
            gPlayers[nPlayer].fCB4 += gSpeedGolfPaceBoost;
            gPlayers[nPlayer].nCB8 = 0;
        } else if (gPlayers[nPlayer].nCB8 > (s32)(FRAME_RATE * gSpeedGolfPaceIdleTime)) {
            gPlayers[nPlayer].fCB4 -= gSpeedGolfPaceIdleDecay * fStep;
        } else {
            gPlayers[nPlayer].fCB4 -= gSpeedGolfPaceDecay * fStep;
        }
    }
}

// A CPU player's run each frame (SpeedGolf_RunUpdate), steering from the heading fA88 towards the
// ball: fA80 from the angle off (angle / PI; tripled between 1 and 15 degrees off), fA84 the
// forward push, falling with the angle off (a quarter from 45 degrees), both eased off within 31.25
// (squared distance) of the ball and again past 2 degrees off, when fA7C turns the heading too (0
// otherwise). The pace fCB4 rises by gSpeedGolfPaceBoost while under a level between
// gSpeedGolfPaceMin and gSpeedGolfPaceMax set by the golfer's speed attribute (half way at 100).
void SpeedGolf_UpdateCpuRun(Player* p) {
    f32 v[4];
    f32 fAngle;
    f32 fDistSq;
    f32 fOff;
    f32 fScale;
    f32 fLow;
    SpeedGolf_Vec4Sub(p->ball.vPos, p->vPlacement, v);
    fDistSq = v[0] * v[0] + v[2] * v[2];
    LLMath_Normalize3(v, v);
    fAngle = p->fA88 - atan2f(v[2], v[0]) - PI / 2.0f;
    while (fAngle < -PI) {
        fAngle += TWOPI;
    }
    while (fAngle > PI) {
        fAngle -= TWOPI;
    }
    fOff = fabsf(fAngle);
    if (fOff > DEG(1.0f) && fOff < DEG(15.0f)) {
        fOff *= 3.0f;
        fAngle *= 3.0f;
    }
    fScale = (PI - fOff) / PI;
    p->fA84 = fScale;
    if (fOff >= PI / 4.0f) {
        p->fA84 *= 0.25f;
    }
    p->fA80 = (1.0f / PI) * fAngle;
    if (fDistSq < 31.25f) {
        p->fA84 *= fDistSq / 31.25f;
        p->fA80 *= fDistSq / 31.25f;
    }
    if (fOff > DEG(2.0f)) {
        p->fA84 *= fScale;
        p->fA80 *= fScale;
        p->fA7C = (1.0f / PI) * -fAngle;
    } else {
        p->fA7C = 0.0f;
    }
    fLow = gSpeedGolfPaceMin;
    if (p->fCB4 < (gSpeedGolfPaceMax - fLow) *
                      (0.5f * (0.01f * (s8)Golfer_GetAttribute(p, ATTR_SPEED, ATTR_TOTAL))) + fLow) {
        p->fCB4 += gSpeedGolfPaceBoost;
    }
}

// Golfer states 12 and 24 (the shot and the run), update. Mode 7 first gives event 0 to the first
// shot to fly on the hole and event 38 to a replay's first shot from the tee. The ball moves
// (GM_SimulateBallMovement). When it comes to rest (ball state 1, or 5 out of bounds)
// SpeedGolf_OnBallAtRest counts the stroke. Holed in mode 7: the shot's length and the hole's
// result are scored (SpeedGolfPoints_ScoreHoledShotLength, SpeedGolfPoints_ScoreHoleResult), event
// 29 for a replay holed while the other still plays, event 37 for fewer strokes than the other; the
// first time a player holes out (nC3C bits 3 and 8, nC60 the strokes) the ball goes back to the tee
// for a replay (event 2) unless the other has finished, when both go to state 26. Holed in modes 6
// and 8: the clock stops (message 17, 0), game message 18 and state 13, and the run tips move on
// (gSpeedGolfFirstHoleTips to gSpeedGolfSecondHoleTip). Not holed: in mode 7 event 37 for the other
// when they holed out in no more strokes; outside the course the ball is replaced
// (GM_ReplaceOOBBall); otherwise it rests (ballBefore), and in mode 7 a ball on the green scores
// first on the green (3), green in or under regulation (21, 22), nearer the pin (7, 26, 27) and the
// replay's green (28), and a ball in sand event 6 (32 on the replay; nC3C bit 10). While the ball
// flies the swing still updates (SW_vUpdateSwing). Then the run: when SpeedGolf_TickRunDelay's wait
// ends the runner (the placement point) starts at the shot's spot (nC3C bit 0, camera 9, swing HUD
// off, pace gSpeedGolfPaceMin, heading at the ball); every frame after, the controls
// (SpeedGolf_UpdateHumanRun or SpeedGolf_UpdateCpuRun), the pace kept within
// gSpeedGolfPaceMin..gSpeedGolfPaceMax and scaled by the speed attribute moves the runner
// (PlaceBall_UpdateMomentums), the arrow to the ball is updated (SpeedGolf_ShowBallDirection), and
// the ball's distance to the runner or the camera, whichever is less, is checked: within 5 of a
// ball at rest the golfer arrives (vBall, run flag off, swing HUD on, prompts off, state 1, in mode
// 7 when gSpeedGolfCanSwitchToShot allows); within 5 of a moving ball a human's button 0x23 stops
// it at the last droppable spot (the prompt shows until then).
void SpeedGolf_RunUpdate(int nPlayer) {
    Player* p = &gPlayers[nPlayer];
    int nOther = nPlayer ? 0 : 1;
    f32 vStart[4];
    f32 vDir[4];
    f32 v[4];
    int nStrokes;
    int nPar;
    int nDiff;
    int n;
    CourseInfo* pHole;
    f32 fDist;
    f32 fToPlace;
    f32 fAngle;
    f32 fLow;
    f32 fHigh;
    Ball* pBall;
    f32* pTarget;
    if (Game_GetMode() == 7) {

        if (!(gPlayers[nPlayer].uC48 & 1) && !(gPlayers[nOther].uC48 & 1)) {
            SpeedGolf_TradeEventPoints(nPlayer, 0);
        }
        if (p->vA44[0] == p->ball.vPos[0] && p->vA44[2] == p->ball.vPos[2] && (p->nC3C & 8)) {
            if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] == gPlayers[nPlayer].nC60 &&
                !(gPlayers[nPlayer].uC48 & 0x4000000000LL)) {
                SpeedGolf_TradeEventPoints(nPlayer, 0x26);
            }
        }
    }
    GM_SimulateBallMovement(nPlayer);
    if (gPlayers[nPlayer].ball.nState == 0) {
    } else if (gPlayers[nPlayer].ball.nState == 1 || gPlayers[nPlayer].ball.nState == 5) {
        if (!SpeedGolf_OnBallAtRest(nPlayer)) {
            if (Game_GetMode() == 7 && (s8)GOLFERSTATE_GetCurrentState(nOther) == 26) {
                GOLFERSTATE_Set(26, (u8)nPlayer);
            }
            return;
        }
        nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
        if (gPlayers[nPlayer].ball.nLie == LIE_INCUP_e) {
            if (Game_GetMode() == 7) {
                SpeedGolfPoints_ScoreHoledShotLength(nPlayer, nOther);
                nStrokes = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
                if (gPlayers[nPlayer].nC3C & 8) {
                    gPlayers[nPlayer].nC3C |= 0x10;
                    if (!(gPlayers[nOther].nC3C & 8)) {
                        SpeedGolf_TradeEventPoints(nPlayer, 0x1D);
                    }
                    SpeedGolfPoints_ScoreHoleResult(nPlayer, nStrokes - gPlayers[nPlayer].nC60);
                } else {
                    gPlayers[nPlayer].nC3C |= 8;
                    gPlayers[nPlayer].nC5C = 59;
                    gPlayers[nPlayer].nC3C |= 0x100;
                    gPlayers[nPlayer].nC60 = nStrokes;
                    SpeedGolfPoints_ScoreHoleResult(nPlayer, nStrokes);
                    if (gPlayers[nOther].nC3C & 8) {
                        if (nStrokes < gPlayers[nOther].nC60) {
                            if (!(gPlayers[nPlayer].uC48 & 0x2000000000LL)) {
                                SpeedGolf_TradeEventPoints(nPlayer, 0x25);
                            }
                        } else if (nStrokes > gPlayers[nOther].nC60) {
                            if (!(gPlayers[nOther].uC48 & 0x2000000000LL)) {
                                SpeedGolf_TradeEventPoints(nOther, 0x25);
                            }
                        }
                    } else if (nStrokes <= gPlayers[nOther].nStrokes[Game_CurHoleIndex()]) {
                        if (!(gPlayers[nPlayer].uC48 & 0x2000000000LL)) {
                            SpeedGolf_TradeEventPoints(nPlayer, 0x25);
                        }
                    }
                }
                if ((gPlayers[nOther].nC3C & 8) || (gPlayers[nPlayer].nC3C & 0x10)) {
                    GOLFERSTATE_Set(26, (u8)nPlayer);
                    if (gPlayers[nOther].ball.nState != 2 && gPlayers[nOther].ball.nState != 3 &&
                        gPlayers[nOther].ball.nState != 4) {
                        GOLFERSTATE_Set(26, (u8)nOther);
                    }
                    return;
                }
                if (!(gPlayers[nPlayer].uC48 & 4)) {
                    SpeedGolf_TradeEventPoints(nPlayer, 2);
                }
                pHole = Ter_GetTGD();
                Physics_InitBall(&p->ball, &pHole->tee[gSession.nTeeSet[nPlayer]].x, nPlayer);
                LLMath_CopyVec(&pHole->tee[gSession.nTeeSet[nPlayer]].x, p->vBall);
                LLMath_CopyVec(&pHole->tee[gSession.nTeeSet[nPlayer]].x, p->vA44);
                gPlayers[nPlayer].nC3C &= ~1;
                if (gSpeedGolfCanSwitchToShot) {
                    GOLFERSTATE_Switch(1, nPlayer);
                    gSpeedGolfCanSwitchToShot = 0;
                } else {
                    gPlayers[nPlayer].nC3C |= 0x100000;
                }
            } else {
                GameModeSkillZoneTimed_SetHudClock(0);
                PlayNow_SendMessage18(nPlayer);
                GOLFERSTATE_Switch(13, nPlayer);
                if (gSpeedGolfFirstHoleTips) {
                    gSpeedGolfFirstHoleTips = 0;
                    gSpeedGolfSecondHoleTip = 1;
                } else {
                    gSpeedGolfSecondHoleTip = 0;
                }
            }
        } else {
            if (!(gPlayers[nPlayer].nC3C & 8) && (gPlayers[nOther].nC3C & 8)) {
                if (nStrokes >= gPlayers[nOther].nC60 && !(gPlayers[nOther].uC48 & 0x2000000000LL)) {
                    SpeedGolf_TradeEventPoints(nOther, 0x25);
                }
            }
            pBall = &p->ball;
            if (!Ter_PointInOOBNetwork(pBall->vPos)) {
                gPlayers[nPlayer].ball.nState = 5;
            }
            if (gPlayers[nPlayer].ball.nState == 5) {
                GM_ReplaceOOBBall(nPlayer);
                if (gSpeedGolfCanSwitchToShot) {
                    GOLFERSTATE_Switch(1, nPlayer);
                    gSpeedGolfCanSwitchToShot = 0;
                } else {
                    gPlayers[nPlayer].nC3C |= 0x100000;
                }
            } else {
                gPlayers[nPlayer].ball.nState = 0;
                Mem_cpy(&p->ballBefore, pBall, sizeof(Ball));
                nPar = Course_GetCurHolePar();
                if (gPlayers[nPlayer].ball.nLie == LIE_GREEN_e) {
                    if (Game_GetMode() == 7) {
                        fDist = SpeedGolf_GroundDistance(p->vPreShot, pBall->vPos);
                        if (SpeedGolf_GroundDistance(pBall->vPos, gpGame->pPinPos) <= lbl_80284708[0] && fDist
                            >= 20.0f) {
                            SpeedGolf_TradeEventPoints(nPlayer, 0x14);
                        }
                        if (!(gPlayers[nPlayer].nC3C & 0x40)) {
                            gPlayers[nPlayer].nC3C |= 0x40;
                            gPlayers[nPlayer].nC64 = gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
                            if (!(gPlayers[nOther].nC3C & 0x40)) {
                                SpeedGolf_TradeEventPoints(nPlayer, 3);
                            }
                            SpeedGolf_Vec4Sub(p->ball.vPos, gpGame->pPinPos, v);
                            p->fC68 = Math_Sqrt(v[0] * v[0] + v[2] * v[2]);
                            nDiff = nPar - 2 - gPlayers[nPlayer].nC64;
                            if (nDiff == 0) {
                                SpeedGolf_TradeEventPoints(nPlayer, 0x15);
                            } else if (nDiff > 0) {
                                SpeedGolf_TradeEventPoints(nPlayer, 0x16);
                            }
                            if ((gPlayers[nOther].uC48 & 0x600000) && nDiff >= 0) {
                                if (gPlayers[nOther].fC68 > gPlayers[nPlayer].fC68) {
                                    SpeedGolf_TradeEventPoints(nPlayer, 7);
                                } else if (gPlayers[nOther].fC68 < gPlayers[nPlayer].fC68) {
                                    SpeedGolf_TradeEventPoints(nOther, 7);
                                }
                            }
                        } else if ((gPlayers[nPlayer].nC3C & 8) && !(gPlayers[nPlayer].nC3C & 0x80)) {
                            gPlayers[nPlayer].nC3C |= 0x80;
                            SpeedGolf_TradeEventPoints(nPlayer, 0x1C);
                            nDiff = nPar - 2 - (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] -
                                                gPlayers[nPlayer].nC60);
                            if (nDiff == 0) {
                                SpeedGolf_TradeEventPoints(nPlayer, 0x15);
                            } else if (nDiff > 0) {
                                SpeedGolf_TradeEventPoints(nPlayer, 0x16);
                            }
                            if ((gPlayers[nOther].uC48 & 0x600000) && nDiff >= 0) {
                                SpeedGolf_Vec4Sub(p->ball.vPos, gpGame->pPinPos, v);
                                fDist = Math_Sqrt(v[0] * v[0] + v[2] * v[2]);
                                if (gPlayers[nPlayer].uC48 & 0x0C000080) {
                                    if (fDist < gPlayers[nPlayer].fC68) {
                                        SpeedGolf_TradeEventPoints(nPlayer, 0x1B);
                                        gPlayers[nPlayer].fC68 = fDist;
                                    }
                                } else if (gPlayers[nOther].uC48 & 0x0C000080) {
                                    if (fDist < gPlayers[nOther].fC68) {
                                        SpeedGolf_TradeEventPoints(nPlayer, 0x1A);
                                        gPlayers[nPlayer].fC68 = fDist;
                                        gPlayers[nOther].uC48 &= ~(u64)0x0C000080;
                                    }
                                } else {
                                    SpeedGolf_TradeEventPoints(nPlayer, 7);
                                    gPlayers[nPlayer].fC68 = fDist;
                                }
                            }
                        }
                    }
                } else if (Game_GetMode() == 7 &&
                           (gPlayers[nPlayer].ball.nLie == 6 || gPlayers[nPlayer].ball.nLie == 7 ||
                            gPlayers[nPlayer].ball.nLie == 8)) {

                    if (gPlayers[nPlayer].nC3C & 0x10) {
                        SpeedGolf_TradeEventPoints(nPlayer, 0x20);
                    } else {
                        SpeedGolf_TradeEventPoints(nPlayer, 6);
                    }
                    gPlayers[nPlayer].nC3C |= 0x400;
                }
            }
        }
        if (Game_GetMode() == 7 && (s8)GOLFERSTATE_GetCurrentState(nOther) == 26) {
            if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) != 26) {
                GOLFERSTATE_Set(26, (u8)nPlayer);
            }
            return;
        }
    } else {
        SW_vUpdateSwing(nPlayer);
    }
    switch (SpeedGolf_TickRunDelay(nPlayer)) {
    case 1:
        break;
    case 0:
        LLMath_CopyVec(p->vBall, vStart);
        PlaceBall_Set(nPlayer, vStart);
        PlaceBall_SetupTarget(nPlayer);
        n = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(ViewController_GetCameraControl(n), 9, nPlayer, n);
        gPlayers[nPlayer].nC3C |= 1;
        fn_80062C80(gPlayers[nPlayer].nC58, 0);
        gPlayers[nPlayer].fCB4 = gSpeedGolfPaceMin;
        if (gPlayers[nPlayer].nC3C & 0x200000) {
            gPlayers[nPlayer].nC54 = 59;
        } else {
            gPlayers[nPlayer].nC54 = 179;
        }
        SpeedGolf_Vec4Sub(p->ball.vPos, p->vPlacement, vDir);
        LLMath_Normalize(vDir, vDir);
        gPlayers[nPlayer].fA88 = PI / 2.0f + atan2f(vDir[2], vDir[0]);
        break;
    case -1:
        if (Player_IsCPU(nPlayer)) {
            SpeedGolf_UpdateCpuRun(p);
        } else {
            SpeedGolf_UpdateHumanRun(nPlayer);
        }
        if (gPlayers[nPlayer].fCB4 < gSpeedGolfPaceMin) {
            gPlayers[nPlayer].fCB4 = gSpeedGolfPaceMin;
        }
        if (gPlayers[nPlayer].fCB4 > gSpeedGolfPaceMax) {
            gPlayers[nPlayer].fCB4 = gSpeedGolfPaceMax;
        }
        fDist = gPlayers[nPlayer].fCB4;
        fDist *= 0.01f * (s8)Golfer_GetAttribute(p, ATTR_SPEED, ATTR_TOTAL);
        PlaceBall_UpdateMomentums(nPlayer, fDist);
        pBall = &p->ball;
        pTarget = p->vPlacement;
        SpeedGolf_Vec4Sub(pBall->vPos, pTarget, vDir);
        LLMath_Normalize(vDir, vDir);
        fAngle = gPlayers[nPlayer].fA88 - atan2f(vDir[2], vDir[0]) - PI / 2.0f;
        fAngle *= 180.0f / PI;
        if (gPlayers[nPlayer].nC58 == 2) {
            fLow = 40.0f;
            fHigh = 330.0f;
        } else {
            fLow = 30.0f;
            fHigh = 340.0f;
        }
        while (fAngle < 0.0f) {
            fAngle += 360.0f;
        }
        while (fAngle > 360.0f) {
            fAngle -= 360.0f;
        }
        if (fAngle <= fLow || fAngle >= fHigh) {
            n = 0;
        } else if (fAngle > fLow && fAngle < 135.0f) {
            n = 1;
        } else if (fAngle < fHigh && fAngle > 225.0f) {
            n = 3;
        } else {
            n = 2;
        }
        SpeedGolf_ShowBallDirection(gPlayers[nPlayer].nC58, n);
        SpeedGolf_Vec4Sub(pBall->vPos, pTarget, vDir);
        fToPlace = Math_Sqrt(vDir[0] * vDir[0] + vDir[2] * vDir[2]);
        // the distance to the view's camera lens position, if that is nearer; the second square
        // root is written twice, as a MIN() macro would expand
        SpeedGolf_Vec4Sub(pBall->vPos,
                          Camera_GetLens(
                                  ViewController_GetIndexedViewController(
                                          gPlayers[nPlayer].nView[0])->pCamera)->m4[3],
                          vDir);
        fDist = (fToPlace <= (f32)Math_Sqrt(vDir[0] * vDir[0] + vDir[2] * vDir[2]))

                    ? fToPlace
                    : (f32)Math_Sqrt(vDir[0] * vDir[0] + vDir[2] * vDir[2]);
        if (gPlayers[nPlayer].ball.nState == 0) {
            if (fDist < 5.0f) {
                LLMath_CopyVec(pBall->vPos, p->vBall);
                gPlayers[nPlayer].nC3C &= ~1;
                fn_80062C80(gPlayers[nPlayer].nC58, 1);
                SpeedGolf_ShowBallDirection(gPlayers[nPlayer].nC58, 0);
                SpeedGolf_ShowRunTip(gPlayers[nPlayer].nC58, 0);
                SpeedGolf_ShowStopBallPrompt(gPlayers[nPlayer].nC58, 0);
                if (gSpeedGolfCanSwitchToShot || Game_GetMode() != 7) {
                    GOLFERSTATE_Switch(1, nPlayer);
                    gSpeedGolfCanSwitchToShot = 0;
                } else {
                    gPlayers[nPlayer].nC3C |= 0x100000;
                }
            }
        } else if (fDist < 5.0f) {
            if (!Player_IsCPU(nPlayer)) {
                if (Input_ReadControlPad(gPlayers[nPlayer].nController)
                    & Controller_GetButtonMask(0x23, 0)) {
                    if (gPlayers[nPlayer].ball.nState != 1) {
                        Physics_DropBall(pBall, lbl_801D5888[nPlayer]);
                        gPlayers[nPlayer].ball.nState = 1;
                    }
                } else {
                    SpeedGolf_ShowStopBallPrompt(gPlayers[nPlayer].nC58, 1);
                }
            }
        } else {
            SpeedGolf_ShowStopBallPrompt(gPlayers[nPlayer].nC58, 0);
        }
        break;
    }
}

// Golfer states 12 and 24 (the shot and the run), exit: the player's first view goes to camera mode
// 25.
void SpeedGolf_RunExit(int nPlayer) {
    int nView = gPlayers[nPlayer].nView[0];
    CameraController_SetCameraMode(ViewController_GetCameraControl(nView), 25, nPlayer, nView);
}

// 1 when the player's pad has buttons 0x1000, 0x400 and 0x800 (Start, X and Y on a GameCube pad)
// all down, each pressed this frame or held; 0 for the CPU and for any other controller.
u8 SpeedGolf_IsStartXYHeld(int nPlayer) {
    int bDown;
    int nCtrl = gPlayers[nPlayer].nController;  // fake match: only the first read goes through nCtrl
    if (nCtrl >= 8) {
        return 0;
    }
    bDown = (Input_ReadControlPad(nCtrl) & 0x1000 || Input_ReadControlPad(gPlayers[nPlayer].nController)
             & 0x10000000) &&
            (Input_ReadControlPad(gPlayers[nPlayer].nController) & 0x400 ||
             Input_ReadControlPad(gPlayers[nPlayer].nController) & 0x4000000) &&
            (Input_ReadControlPad(gPlayers[nPlayer].nController) & 0x800 ||
             Input_ReadControlPad(gPlayers[nPlayer].nController) & 0x8000000);
    return bDown;
}

// Every unpaused frame (SpeedGolf_Update, the pfn220 of modes 7 and 8). Mode 7, unless a player is
// out or going out (nC3C bits 13, 14): a player who has holed out (bit 8) takes 5 points a second
// (every 60 frames, nC5C) from one still playing, which can knock that one out (both to state 26);
// the pace's idle count nCB8 goes up; a human who has not finished and can afford event 39 may
// press button 0x25 (not while Start, X and Y are held, SpeedGolf_IsStartXYHeld) in state 24, or
// with the ball off the tee, not holed and in bounds outside states 2, 3, 4, 8 and 10, to pay event
// 39 and start the hole again from the tee (state 1). Mode 8: player 0 may do the same for free,
// with only the tee, cup and out-of-bounds test. Last, gSpeedGolfCanSwitchToShot is set.
void SpeedGolf_UpdatePlayers(void) {
    PlayerNumber_t i;
    PlayerNumber_t nOther;
    int nPlayer;
    int k;
    CourseInfo* pHole;
    if (gSession.nPaused == 0) {
        if (Game_GetMode() == 7) {
            i = PLR_1_e;
            nOther = PLR_2_e;
            if ((gPlayers[i].nC3C & 0x6000) || (gPlayers[nOther].nC3C & 0x6000)) {
                return;
            }
            for (k = 0; k < 2; k++) {
                nPlayer = i;    // fake match: the loop body indexes through an int copy
                if (gPlayers[nPlayer].nC3C & 0x100) {
                    if (!(gPlayers[nOther].nC3C & 8)) {
                        gPlayers[nPlayer].nC5C--;
                        if (gPlayers[nPlayer].nC5C == 0) {
                            gPlayers[nPlayer].nC44 += 5;
                            SpeedGolf_ShowPoints(gPlayers[nPlayer].nC58, gPlayers[nPlayer].nC44, 0);
                            SpeedGolf_ShowPointsGain(gPlayers[nPlayer].nC58, 5);
                            gPlayers[nOther].nC44 -= 5;
                            SpeedGolf_ShowPoints(gPlayers[nOther].nC58, gPlayers[nOther].nC44, 0);
                            gPlayers[nPlayer].nC5C = 59;
                            if (gPlayers[nOther].nC44 <= 0 && !(gPlayers[nOther].nC3C & 0x6000) &&
                                !(gPlayers[nOther].nC3C & 0x8000)) {
                                gPlayers[nOther].nC44 = 0;
                                gPlayers[nPlayer].nC44 = 6000;
                                gPlayers[nOther].nC3C |= 0xC000;
                                gPlayers[nPlayer].nC3C |= 0x10000 | 0x4000;
                                GOLFERSTATE_Set(26, (u8)nOther);
                                GOLFERSTATE_Set(26, (u8)nPlayer);
                                return;
                            }
                        }
                    }
                }
                gPlayers[nPlayer].nCB8++;
                if (!Player_IsCPU(nPlayer) && !SpeedGolfPoints_HoleFinished(nPlayer, 1) &&
                    gSpeedGolfEvents[0x27].nPoints + gPlayers[nPlayer].nC44 > 0) {
                    if ((Input_ReadControlPad(gPlayers[nPlayer].nController)
                         & Controller_GetButtonMask(0x25, 0)) &&
                        !SpeedGolf_IsStartXYHeld(nPlayer) &&
                        ((s8)GOLFERSTATE_GetCurrentState(nPlayer) == 24 ||
                         (gPlayers[nPlayer].ball.nLie != 0 && gPlayers[nPlayer].ball.nLie != LIE_INCUP_e &&
                          gPlayers[nPlayer].ball.nLie != 16 &&
                          (s8)GOLFERSTATE_GetCurrentState(nPlayer) != 2 &&
                          (s8)GOLFERSTATE_GetCurrentState(nPlayer) != 4 &&
                          (s8)GOLFERSTATE_GetCurrentState(nPlayer) != 3 &&
                          (s8)GOLFERSTATE_GetCurrentState(nPlayer) != 8 &&
                          (s8)GOLFERSTATE_GetCurrentState(nPlayer) != 10))) {
                        SpeedGolf_TradeEventPoints(nPlayer, 0x27);
                        pHole = Ter_GetTGD();
                        Physics_InitBall(&gPlayers[nPlayer].ball,
                                    &pHole->tee[gSession.nTeeSet[nPlayer]].x, nPlayer);
                        LLMath_CopyVec(&pHole->tee[gSession.nTeeSet[nPlayer]].x,
                                 gPlayers[nPlayer].vBall);
                        LLMath_CopyVec(&pHole->tee[gSession.nTeeSet[nPlayer]].x,
                                 gPlayers[nPlayer].vA44);
                        gPlayers[nPlayer].nC3C &= ~1;
                        SpeedGolf_ShowBallDirection(gPlayers[nPlayer].nC58, 0);
                        SpeedGolf_ShowRunTip(gPlayers[nPlayer].nC58, 0);
                        SpeedGolf_ShowStopBallPrompt(gPlayers[nPlayer].nC58, 0);
                        GOLFERSTATE_Switch(1, nPlayer);
                        gSpeedGolfCanSwitchToShot = 0;
                    }
                }
                nOther = PLR_1_e;
                i = PLR_2_e;
            }
        } else if (Game_GetMode() == 8) {
            i = PLR_1_e;
            if ((Input_ReadControlPad(gPlayers[i].nController) & Controller_GetButtonMask(0x25, 0))
                && !SpeedGolf_IsStartXYHeld(i) &&
                ((s8)GOLFERSTATE_GetCurrentState(i) == 24 ||
                 (gPlayers[i].ball.nLie != 0 && gPlayers[i].ball.nLie != LIE_INCUP_e &&
                  gPlayers[i].ball.nLie != 16))) {
                pHole = Ter_GetTGD();
                Physics_InitBall(&gPlayers[i].ball, &pHole->tee[gSession.nTeeSet[i]].x, i);
                LLMath_CopyVec(&pHole->tee[gSession.nTeeSet[i]].x, gPlayers[i].vBall);
                LLMath_CopyVec(&pHole->tee[gSession.nTeeSet[i]].x, gPlayers[i].vA44);
                gPlayers[i].nC3C &= ~1;
                SpeedGolf_ShowBallDirection(gPlayers[i].nC58, 0);
                SpeedGolf_ShowRunTip(gPlayers[i].nC58, 0);
                SpeedGolf_ShowStopBallPrompt(gPlayers[i].nC58, 0);
                GOLFERSTATE_Switch(1, i);
            }
        }
        gSpeedGolfCanSwitchToShot = 1;
    }
}

// Golfer state 26 (the hole is over for this player), enter: player 0 moves the run tips on
// (gSpeedGolfFirstHoleTips to gSpeedGolfSecondHoleTip); a wait of nC54 134 frames, the run flag,
// arrow, tip and prompt off, the points shown. In mode 7 (nC3C bit 25, a 239-frame wait), unless
// this is the round's first hole, the selected holes are each scored by who gained more points on
// it (1 won, 0 halved, 2 lost; 3000 before the first): a run of wins up to this hole is event 33
// (three) or 34 (more), one win after three or more losses event 35, each adding 119 frames to the
// wait.
void SpeedGolf_HoleOverInit(int nPlayer) {
    int aResult[18];
    int nHole;
    s32 nOther;
    int h;
    int nMine;
    int nTheirs;
    int nWon;
    int nLost;
    nHole = Game_CurHoleIndex();
    nOther = nPlayer ? 0 : 1;
    if (nPlayer == 0) {
        if (gSpeedGolfFirstHoleTips) {
            gSpeedGolfFirstHoleTips = 0;
            gSpeedGolfSecondHoleTip = 1;
        } else {
            gSpeedGolfSecondHoleTip = 0;
        }
    }
    gPlayers[nPlayer].nC54 = 134;
    gPlayers[nPlayer].nC3C &= ~1;
    SpeedGolf_ShowBallDirection(gPlayers[nPlayer].nC58, 0);
    SpeedGolf_ShowRunTip(gPlayers[nPlayer].nC58, 0);
    SpeedGolf_ShowStopBallPrompt(gPlayers[nPlayer].nC58, 0);
    SpeedGolf_ShowPoints(gPlayers[nPlayer].nC58, gPlayers[nPlayer].nC44, 1);
    if (Game_GetMode() == 7) {
        gPlayers[nPlayer].nC3C |= 0x2000000;
        gPlayers[nPlayer].nC54 = 239;
        if (nHole != fn_800F9328()) {
            for (h = fn_800F9328(); h != -1; h = fn_800F93D8(h)) {
                if (h == fn_800F9328()) {
                    nMine = gPlayers[nPlayer].nC6C[h] - 3000;
                    nTheirs = gPlayers[nOther].nC6C[h] - 3000;
                } else if (h == nHole) {
                    nMine = gPlayers[nPlayer].nC44 - gPlayers[nPlayer].nC6C[fn_800F9414(h)];
                    nTheirs = gPlayers[nOther].nC44 - gPlayers[nOther].nC6C[fn_800F9414(h)];
                } else {
                    nMine = gPlayers[nPlayer].nC6C[h] - gPlayers[nPlayer].nC6C[fn_800F9414(h)];
                    nTheirs = gPlayers[nOther].nC6C[h] - gPlayers[nOther].nC6C[fn_800F9414(h)];
                }
                if (nMine > nTheirs) {
                    aResult[h] = 1;
                } else if (nMine == nTheirs) {
                    aResult[h] = 0;
                } else {
                    aResult[h] = 2;
                }
            }
            nWon = 0;
            for (h = nHole; h != -1; h = fn_800F9414(h)) {
                if (aResult[h] != 1) break;
                nWon++;
            }
            if (nWon > 3) {
                gPlayers[nPlayer].nC3C |= 0x40000;
            } else if (nWon == 3) {
                gPlayers[nPlayer].nC3C |= 0x20000;
            } else if (nWon == 1) {
                if (nHole != 0) {
                    nLost = 0;
                    for (h = fn_800F9414(nHole); h != -1; h = fn_800F9414(h)) {
                        if (aResult[h] != 2) break;
                        nLost++;
                    }
                }
                // EA bug: on the first hole (nHole 0) nLost is never set
                if (nLost > 2) {
                    gPlayers[nPlayer].nC3C |= 0x80000;
                }
            }
            if (gPlayers[nPlayer].nC3C & 0x20000) {
                gPlayers[nPlayer].nC3C &= ~0x20000;
                SpeedGolf_TradeEventPoints(nPlayer, 0x21);
                gPlayers[nPlayer].nC54 += 119;
            } else if (gPlayers[nPlayer].nC3C & 0x40000) {
                gPlayers[nPlayer].nC3C &= ~0x40000;
                SpeedGolf_TradeEventPoints(nPlayer, 0x22);
                gPlayers[nPlayer].nC54 += 119;
            } else if (gPlayers[nPlayer].nC3C & 0x80000) {
                gPlayers[nPlayer].nC3C &= ~0x80000;
                SpeedGolf_TradeEventPoints(nPlayer, 0x23);
                gPlayers[nPlayer].nC54 += 119;
            }
        }
    }
}

// Golfer state 26, update: a player whose points ran out (nC3C bit 15) gets event 40, one who
// knocked the other out (bit 16) event 41, each adding 119 frames to the wait. When the wait (nC54)
// is over and no commentary line plays, a pending game over (bit 14) becomes bit 13 (out,
// SpeedGolfPoints_GameFinished), the player has finished the hole (bit 3) and the turn ends
// (GM_EndOfGolferTurn); if the other player is not in state 26 and their ball is at rest, both are
// marked finished and the turn is ended again (see the EA bug below).
void SpeedGolf_HoleOverUpdate(int nPlayer) {
    int nOther;
    if (gPlayers[nPlayer].nC3C & 0x8000) {
        SpeedGolf_TradeEventPoints(nPlayer, 0x28);
        gPlayers[nPlayer].nC3C &= ~0x8000;
        gPlayers[nPlayer].nC54 += 119;
    } else if (gPlayers[nPlayer].nC3C & 0x10000) {
        SpeedGolf_TradeEventPoints(nPlayer, 0x29);
        gPlayers[nPlayer].nC3C &= ~0x10000;
        gPlayers[nPlayer].nC54 += 119;
    }
    if (gPlayers[nPlayer].nC54-- <= 0 && !Gaud_GetCommentStatus()) {
        if (gPlayers[nPlayer].nC3C & 0x4000) {
            gPlayers[nPlayer].nC3C ^= 0x6000;
        }
        gPlayers[nPlayer].nC3C |= 8;
        GM_EndOfGolferTurn(nPlayer);
        nOther = nPlayer ? 0 : 1;
        if ((s8)GOLFERSTATE_GetCurrentState(nOther) != 26 && gPlayers[nOther].ball.nState == 0) {
            gPlayers[nOther].nC3C |= 8;
            gPlayers[nPlayer].nC3C |= 8;
            // EA bug: nPlayer's turn is ended a second time; the other player's is never ended here
            // (asm 800FD618 and 800FD664 both load r3 from r29, nPlayer)
            GM_EndOfGolferTurn(nPlayer);
        }
    }
}

// Golfer state 26 (the hole is over), exit: empty in this build.
void SpeedGolf_HoleOverExit(int nPlayer) {
}

// Whether the golfer is running to the ball (nC3C bit 0), in modes 7 and 8 only (0 in mode 6 and
// every other mode). The main loop asks it.
s32 SpeedGolf_IsRunning(int nPlayer) {
    if (Game_GetMode() == 7 || Game_GetMode() == 8) {
        return gPlayers[nPlayer].nC3C & 1;
    }
    return 0;
}

// A hole's score for the scorecard (the UI's command, TW07's GM_vGetSpeedGolfHoleScore). Mode 8:
// nC6C (seconds plus 3 a stroke), *pWon 0. Otherwise the points after the hole (nC6C), with *pWon 1
// when the player gained points on it (3000 before the first selected hole); the current hole
// counts from nC44 once both have finished it, and before that returns nC6C as it stands with *pWon
// 0. A hole not selected: 0.
s32 SpeedGolf_GetHoleScore(int nPlayer, int nHole, s32* pWon) {
    int nCur = Game_CurHoleIndex();
    int nGain;
    if (Game_GetMode() == 8) {
        *pWon = 0;
    } else if (nHole == Game_CurHoleIndex() && !SpeedGolfPoints_HoleFinished(nPlayer, 1)) {
        *pWon = 0;
        return gPlayers[nPlayer].nC6C[nHole];
    } else if (gpGame->bHoleSelected[nHole]) {
        if (nHole == fn_800F9328()) {
            if (nCur == nHole) {
                nGain = gPlayers[nPlayer].nC44 - 3000;
            } else {
                nGain = gPlayers[nPlayer].nC6C[nHole] - 3000;
            }
        } else if (nHole == nCur) {
            nGain = gPlayers[nPlayer].nC44 - gPlayers[nPlayer].nC6C[fn_800F9414(nHole)];
        } else {
            nGain = gPlayers[nPlayer].nC6C[nHole] - gPlayers[nPlayer].nC6C[fn_800F9414(nHole)];
        }
        if (nGain > 0) {
            *pWon = 1;
        } else {
            *pWon = 0;
        }
    } else {
        *pWon = 0;
        return 0;
    }
    return gPlayers[nPlayer].nC6C[nHole];
}

// The two players' names ("User n" without a loaded profile) and points, for the UI. Returns
// which player gained points on the current hole: 0, 1, or -1 for neither.
s32 SpeedGolfPoints_GetNamesAndPoints(char* szName1, s32* pPoints1, char* szName2, s32* pPoints2) {
    s32 bWon;
    char sz[32];
    int nHole = Game_CurHoleIndex();
    int nProfile;
    nProfile = gPlayers[0].nIndex;
    if (!lbl_801D7148.aLoaded[nProfile]) {
        sprintf(sz, "User %d", nProfile + 1);
        strcpy(szName1, sz);
    } else {
        strcpy(szName1, gpSaveData[nProfile].szName);
    }
    nProfile = gPlayers[1].nIndex;
    if (!lbl_801D7148.aLoaded[nProfile]) {
        sprintf(sz, "User %d", nProfile + 1);
        strcpy(szName2, sz);
    } else {
        strcpy(szName2, gpSaveData[nProfile].szName);
    }
    *pPoints1 = gPlayers[0].nC44;
    *pPoints2 = gPlayers[1].nC44;
    SpeedGolf_GetHoleScore(0, nHole, &bWon);
    if (bWon) {
        return 0;
    }
    SpeedGolf_GetHoleScore(1, nHole, &bWon);
    return bWon ? 1 : -1;
}

// The pfnSetTimer of modes 6, 7 and 8 (a UI command passes the hole's time in frames): the current
// hole's seconds (nFrames / 60) go into n290, and seconds plus 3 a stroke into nC6C.
void SpeedGolf_SetHoleTime(int nPlayer, int nFrames) {
    int n = nFrames / 60;
    gPlayers[nPlayer].n290[Game_CurHoleIndex()] = n;
    n += gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] * 3;
    gPlayers[nPlayer].nC6C[Game_CurHoleIndex()] = n;
}

// The hole's time ran out (the UI's timer-out command, TW07's GM_vTimerOut; skill zone mode has its
// own): when both golfers are between shots (states 1 to 4 or 10), player 0's hole ends: the ball
// in the cup, nC3C bit 26 (which SpeedGolf_HoleFinished ends the hole on), the clock stopped
// (message 17, 0), game message 18 and state 13.
void SpeedGolf_TimerOut(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    if (((s8)GOLFERSTATE_GetCurrentState(nPlayer) == 1 || (s8)GOLFERSTATE_GetCurrentState(nPlayer) == 2 ||
         (s8)GOLFERSTATE_GetCurrentState(nPlayer) == 3 || (s8)GOLFERSTATE_GetCurrentState(nPlayer) == 4 ||
         (s8)GOLFERSTATE_GetCurrentState(nPlayer) == 10) &&
        ((s8)GOLFERSTATE_GetCurrentState(PLR_2_e) == 1 || (s8)GOLFERSTATE_GetCurrentState(PLR_2_e) == 2 ||
         (s8)GOLFERSTATE_GetCurrentState(PLR_2_e) == 3 || (s8)GOLFERSTATE_GetCurrentState(PLR_2_e) == 4 ||
         (s8)GOLFERSTATE_GetCurrentState(PLR_2_e) == 10)) {
        gPlayers[nPlayer].ball.nLie = LIE_INCUP_e;
        gPlayers[nPlayer].nC3C |= 0x4000000;
        GameModeSkillZoneTimed_SetHudClock(0);
        PlayNow_SendMessage18(nPlayer);
        GOLFERSTATE_Switch(13, nPlayer);
    }
}

// The current course's three score limits for mode 8's prize (gSpeedGolfPrizeScores; 750, 675 and
// 600 on the first course): under the third wins 5000, under the second 2500, under the first 1000
// (SpeedGolf_GetWinner). A UI command shows them.
void SpeedGolf_GetPrizeScores(s32* pLimit1000, s32* pLimit2500, s32* pLimit5000) {
    *pLimit1000 = gSpeedGolfPrizeScores[gpGame->nCurCourse].nLimit1000;
    *pLimit2500 = gSpeedGolfPrizeScores[gpGame->nCurCourse].nLimit2500;
    *pLimit5000 = gSpeedGolfPrizeScores[gpGame->nCurCourse].nLimit5000;
}

// A hole on the solo scorecard: its seconds plus 3 per stroke.
static inline s32 SG_Score(s32 nSeconds, s32 nStrokes) {
    return nStrokes * 3 + nSeconds;
}

// The round's winner and prize (the UI's command, TW07's GM_vGetSpeedGolfWinner), paid each time it
// is asked (GM_Earnings_AwardMoney and money.n1C). Only for a full round (else -1 and *pMoney 0).
// Mode 8: player 0's total of time plus 3 a stroke over the selected holes before the current one
// and the current one, under the course's limits (SpeedGolf_GetPrizeScores), wins 5000, 2500 or
// 1000. Two players: whoever is ahead of 3000 points (by player 0's points) takes the margin, 4500
// from 3000 up. Returns the winner (-1 for none) and the money in *pMoney.
s32 SpeedGolf_GetWinner(s32* pMoney) {
    s32 nLimit1000;
    s32 nLimit2500;
    s32 nLimit5000;
    int nHole;
    int n;
    int nWinner;
    int h;
    nHole = Game_CurHoleIndex();
    if (!GM_FullRoundOfGolf()) {
        *pMoney = 0;
        return -1;
    }
    if (Game_GetMode() == 8) {
        n = 0;
        for (h = 0; h < nHole; h++) {
            if (gpGame->bHoleSelected[h]) {
                n += SG_Score(gPlayers[0].n290[h], gPlayers[0].nStrokes[h]);
            }
        }
        n += SG_Score(gPlayers[0].n290[Game_CurHoleIndex()], gPlayers[0].nStrokes[Game_CurHoleIndex()]);
        nWinner = 0;
        SpeedGolf_GetPrizeScores(&nLimit1000, &nLimit2500, &nLimit5000);
        if (n < nLimit5000) {
            n = 5000;
        } else if (n < nLimit2500) {
            n = 2500;
        } else if (n < nLimit1000) {
            n = 1000;
        } else {
            n = 0;
            nWinner = -1;
        }
        if (nWinner != -1) {
            GM_Earnings_AwardMoney(nWinner, n, NULL);
            gPlayers[nWinner].money.n1C += n;
        }
    } else {
        n = gPlayers[0].nC44 - 3000;
        if (n > 0) {
            nWinner = 0;
        } else if (n < 0) {
            n = -n;
            nWinner = 1;
        } else {
            n = 0;
            nWinner = -1;
        }
        if (nWinner != -1) {
            if (n >= 3000) {
                n = 4500;
            }
            GM_Earnings_AwardMoney(nWinner, n, NULL);
            gPlayers[nWinner].money.n1C += n;
        }
    }
    *pMoney = n;
    return nWinner;
}

// The solo total for the scorecard: nC6C (seconds plus 3 a stroke, SpeedGolf_SetHoleTime) of the
// selected holes before the current one, plus the current hole's score (SG_Score of its seconds and
// strokes); the current hole's seconds, strokes and score also go out through the pointers. nPlayer
// is not read: it is player 0.
s32 SpeedGolf_GetRoundScore(int nPlayer, s32* pSeconds, s32* pStrokes, s32* pScore) {
    int nHole;
    int n;
    int h;
    nHole = Game_CurHoleIndex();
    n = 0;
    for (h = 0; h < nHole; h++) {
        if (gpGame->bHoleSelected[h]) {
            n += gPlayers[0].nC6C[h];
        }
    }
    *pStrokes = gPlayers[0].nStrokes[Game_CurHoleIndex()];
    *pSeconds = gPlayers[0].n290[Game_CurHoleIndex()];
    *pScore = SG_Score(*pSeconds, *pStrokes);
    n += *pScore;
    return n;
}

// The pfn220 of modes 7 and 8 (every frame of a round): SpeedGolf_UpdatePlayers.
void SpeedGolf_Update(void) {
    SpeedGolf_UpdatePlayers();
}

// The pfn230 of modes 6, 7 and 8 (GM_RenderBallTarget): the placement target is never drawn for the
// mode's sake.
u8 SpeedGolf_RenderBallTarget(int nPlayer) {
    return 0;
}

// The pfnCheckControllerPulled of modes 6, 7 and 8, which GM_CheckControllerPulled asks (TW06's
// CheckControllerPulled): 1 during the countdown (nC3C bit 1) once under 71 frames are left, and
// otherwise unless player 0's view is in a colour fade (fn_80063C90).
u8 SpeedGolf_CheckControllerPulled(void) {
    if (gPlayers[0].nC3C & 2) {
        if (gPlayers[0].nC54 < 71) {
            return 1;
        }
    } else if (!fn_80063C90(ViewController_GetCameraControl(gPlayers[0].nView[0]))) {
        return 1;
    }
    return 0;
}

// Game message 21: a player's points on the HUD, with the player's HUD slot (Player.nC58), the
// points (nC44) and 1 as a hole starts or ends, 0 for a change.
void SpeedGolf_ShowPoints(s32 nSlot, s32 nPoints, s32 bReset) {
    GameMsg_Send3Ints(21, nSlot, nPoints, bReset);
}

// Game message 19 with a HUD slot (Player.nC58) and a byte: sent with 1 as the countdown starts
// (SpeedGolf_CountdownInit), after the player's panel is shown.
void SpeedGolf_SendMessage19(s32 nSlot, s32 n) {
    GameMsg_Send2Ints(19, nSlot, (n & 0xFF));
}

// Game message 16 with 1: the countdown's "ready" as a hole starts (SpeedGolf_CountdownInit;
// SpeedGolf_ShowGo sends 2 when it ends).
void SpeedGolf_ShowReady(void) {
    GameMsg_SendInt(16, 1);
}

// Game message 44: the prompt to stop the ball (button 0x23) for a HUD slot (Player.nC58): 1 shows
// it while the runner is within 5 of a moving ball, 0 hides it.
void SpeedGolf_ShowStopBallPrompt(s32 nSlot, s32 bShow) {
    GameMsg_Send2Ints(44, nSlot, bShow);
}

// Game message 41: the run's tip for a HUD slot (Player.nC58): 0 none, 1 after 179 frames without
// the stick, 2 and 3 the extra tips of the round's first and second hole
// (SpeedGolf_UpdateHumanRun).
void SpeedGolf_ShowRunTip(s32 nSlot, s32 nTip) {
    GameMsg_Send2Ints(41, nSlot, nTip);
}

// Game message 37: where the ball is from the runner's heading, for a HUD slot (Player.nC58): 0
// ahead, 1 and 3 to either side, 2 behind (SpeedGolf_RunUpdate); 0 also when the run is not on.
void SpeedGolf_ShowBallDirection(s32 nSlot, s32 nDir) {
    GameMsg_Send2Ints(37, nSlot, nDir);
}

// Game message 16 with 2: "go" as the countdown ends (SpeedGolf_CountdownUpdate, with event 0x45,
// TW06's SpeedgolfGo).
void SpeedGolf_ShowGo(void) {
    GameMsg_SendInt(16, 2);
}

// Game message 23: an event's popup for a HUD slot (Player.nC58), with the event (0..41) and its
// points (SpeedGolf_TradeEventPoints).
void SpeedGolf_ShowEvent(s32 nSlot, s32 nEvent, s32 nPoints) {
    GameMsg_Send3Ints(23, nSlot, nEvent, nPoints);
}

// Game message 22: points gained, for a HUD slot (Player.nC58): the 5 a second a holed player takes
// from the one still playing (SpeedGolf_UpdatePlayers).
void SpeedGolf_ShowPointsGain(s32 nSlot, s32 nPoints) {
    GameMsg_Send2Ints(22, nSlot, nPoints);
}

// Plays a line of commentary playlist 4 (speed golf's) through Gaud_StartComment: the line and
// Gaud_StartComment's third argument are passed on as given.
void SpeedGolf_StartComment(s32 nLine, s32 a) {
    Gaud_StartComment(4, nLine, a);
}

// Four floats of pA less pB into pOut.
#ifdef __MWERKS__
asm void SpeedGolf_Vec4Sub(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 0, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 0, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void SpeedGolf_Vec4Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
    pOut[3] = pA[3] - pB[3];
}
#endif
