// GameModeDriverPGATour.c (TW06's GameModeDriverPGATour; TW07 GameModeDriver_PGATour.cpp): game
// mode 23, the PGA TOUR career. Ten seasons (2004-2013) of up to 31 tournaments; the tour data
// (gPgaData) comes from the 'PGAc' (tournaments), 'PGAt' (their formats), 'PGAp' (sponsorship
// offers) and 'PGAn' (names) stream objects. The player plays a tournament's rounds one by one
// against a simulated field (PGATourSimulation.c), with a cut after the second round and a playoff
// on a tie for the lead; the calendar (FE_Calendar.c) can skip ahead, simulating the tournaments
// in between. The season is kept in save profile 0 (TourSeason): the current tournament and round,
// each tournament's champion and the player's result ("Did Not Play", "Cut" or a place). The
// prize bracket grows with the tournaments won.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "game/modes/pgatour.h"
#include "game/modes/pgatoursim.h"

// PGA TOUR driver state; only this file uses it. The uninitialised ones are defined last address
// first: the compiler lays out a file's .bss and .sbss last definition first.
s32 gPgaSavedWeather = 4;      // the options' nWeather from before a tour round (Shutdown puts it back)
s32 gPgaSavedOptions18 = 1;     // the options' n18 from before a tour round (SetTournament keeps it;
                                //   nothing puts it back)

PgaData gPgaData;               // the tour data, from the 'PGA' stream objects
PgaTour_WinInfo gPgaWinInfo;    // the player's prize in the tournament just played: set when it
                                //   is paid (GameModeDriverPGATour_AwardMoney), shown on the
                                //   result screen. TW07: GetWinInfo
PgaStatCounts gPgaRoundStats;   // the current round's statistics (see pgatour.h)

s32 gPgaPlayoffHole;            // the playoff hole index: set to 16, each playoff moves it on
                                //   (17, 15, 16, 17, ...; GameModeDriverPGATour_GoToPlayoff)
u8  gbPgaTourRoundActive;       // 1 from a tour round's start until the mode shuts down
                                //   (read through GM_Currently_PgaTourMode)
s32 gPgaSavedWind;              // the options' nWind from before a tour round (Shutdown puts it back)

// Not in a C unit yet
void UI_GetMoneyString(s32 nMoney, char* pDst);               // money as text

void GameModeDriverPGATour_LoadPGAcFromStream(UStreamObject* pObject);
void GameModeDriverPGATour_LoadPGAtFromStream(UStreamObject* pObject);
void GameModeDriverPGATour_LoadPGApFromStream(UStreamObject* pObject);
void GameModeDriverPGATour_Locale_PgaTourMode_LoadPGAnFromStream(UStreamObject* pObject);
void GameModeDriverPGATour_Shutdown(void);
void GameModeDriverPGATour_StartGamePreData(void);
void GameModeDriverPGATour_EndGame(void);
u8   GameModeDriverPGATour_IsPuttForLead(int nPlayer);
u8   GameModeDriverPGATour_IsPuttForWin(s32 nPlayer);
s32  GameModeDriverPGATour_GetCurrentLead(int nPlayer);
s32  GameModeDriverPGATour_GetPotentialLead(int nPlayer);
s32  GameModeDriverPGATour_GetPotentialHoleResult(int nPlayer);
void GameModeDriverPGATour_EndTournament(int nPlayer);
s32  GameModeDriverPGATour_GetCurrentBracket(int nPlayer);
void GameModeDriverPGATour_SimCurrentTournament(int nPlayer, u8 bSimUser);
void GameModeDriverPGATour_PostHoleLoadInit(void);
void GameModeDriverPGATour_EndHole(void);
u8   GameModeDriverPGATour_GameFinished(u8 bCheck);
u8   GameModeDriverPGATour_GoToPlayoff(u8 bCheck);
s32  GameModeDriverPGATour_GetEventOnOrAfter(s32 i);
s32  GameModeDriverPGATour_GetNumEventsWon(void);

// Game mode 23's setup (pfnInit, from GM_SetModeType): stroke play's golfer order, honors and
// hole-finished rules with the tour's own hooks (round start and end, hole start and end, game over
// and playoff, the lead and putt-for-lead / putt-for-win answers); no mulligans, gpGame nC and n10
// 1, round 0 of 1 (nDC, nE0) and no split screen.
void GameModeDriverPGATour_Init(void) {
    gpGame->pfnInit = GameModeDriverPGATour_Init;
    gpGame->pfnShutdown = GameModeDriverPGATour_Shutdown;
    gpGame->pfnLoadHole = GameModeDriverPGATour_PostHoleLoadInit;
    gpGame->pfnSetupNextGolfer = GameModeStroke_SetupNextGolfer;
    gpGame->pfnGetHonors = GameModeStroke_GetHonors;
    gpGame->pfnHoleFinished = GameModeStroke_HoleFinished;
    gpGame->pfnGameFinished = GameModeDriverPGATour_GameFinished;
    gpGame->pfnGoToPlayoff = GameModeDriverPGATour_GoToPlayoff;
    gpGame->pfnStartGamePreData = GameModeDriverPGATour_StartGamePreData;
    gpGame->pfnEndHole = GameModeDriverPGATour_EndHole;
    gpGame->pfnEndGame = GameModeDriverPGATour_EndGame;
    gpGame->pfnIsPuttForLead = GameModeDriverPGATour_IsPuttForLead;
    // IsPuttForWin's player is an s32 (long): as an int its profile index compiles differently
    gpGame->pfnIsPuttForWin = (u8 (*)(int))GameModeDriverPGATour_IsPuttForWin;
    gpGame->pfnGetCurrentLead = GameModeDriverPGATour_GetCurrentLead;
    gpGame->pfnGetPotentialLead = GameModeDriverPGATour_GetPotentialLead;
    gpGame->pfnGetPotentialHoleResult = GameModeDriverPGATour_GetPotentialHoleResult;
    gpGame->b274 = 0;
    gpGame->nScoringType = 0;
    gpGame->nMulligans = 0;
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gpGame->nDC = 0;
    gpGame->nE0 = 1;
    gSession.nSplitScreen = 0;
}

// Empty in this build. GM_DeInitModule calls it as a round is torn down, beside the Earnings stream
// free (TW06 GM_Earnings_FreeStreamMemory).
void GameModeDriverPGATour_FreeStreamMemory(void) {
}

// Registers the loaders of the tour's four stream objects: 'PGAc' the tournaments, 'PGAt' their
// formats, 'PGAp' the 'PGAp' records and 'PGAn' the names. The hole stream manager (fn_80014864)
// calls it.
void GameModeDriverPGATour_RegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback('PGAc', GameModeDriverPGATour_LoadPGAcFromStream);
    Stream_RegisterLoadChunkCallback('PGAt', GameModeDriverPGATour_LoadPGAtFromStream);
    Stream_RegisterLoadChunkCallback('PGAp', GameModeDriverPGATour_LoadPGApFromStream);
    Stream_RegisterLoadChunkCallback('PGAn', GameModeDriverPGATour_Locale_PgaTourMode_LoadPGAnFromStream);
}

// Unregisters the four 'PGA' loaders of RegisterStreamClients (hole stream manager, fn_800148A8).
void GameModeDriverPGATour_UnregisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('PGAc');
    Stream_UnregisterLoadChunkCallback('PGAt');
    Stream_UnregisterLoadChunkCallback('PGAp');
    Stream_UnregisterLoadChunkCallback('PGAn');
}

// The 'PGAc' loader: the season's 31 tournaments (gPgaData.aTournament).
void GameModeDriverPGATour_LoadPGAcFromStream(UStreamObject* pObject) {
    // port: the 'PGAc' object is copied straight into gPgaData.aTournament (Tournament[31]); it is
    //       big-endian on disc, so a little-endian port converts it field by field here
    //       (docs/format-byteorder.md)
    Stream_StreamLoadFixedSize(pObject, sizeof(gPgaData.aTournament), gPgaData.aTournament);
}

// The 'PGAt' loader: the 31 tournament formats (gPgaData.aTourEvent: rounds, courses, pins, tees).
void GameModeDriverPGATour_LoadPGAtFromStream(UStreamObject* pObject) {
    // port: the 'PGAt' object is copied straight into gPgaData.aTourEvent (TourEvent[31]); it is
    //       big-endian on disc, so a little-endian port converts it field by field here
    //       (docs/format-byteorder.md)
    Stream_StreamLoadFixedSize(pObject, sizeof(gPgaData.aTourEvent), gPgaData.aTourEvent);
}

// The 'PGAp' loader: 11 sponsorship offers (gPgaData.aSponsorship).
void GameModeDriverPGATour_LoadPGApFromStream(UStreamObject* pObject) {
    // port: the 'PGAp' object is copied straight into gPgaData.aSponsorship (PgaSponsorship[11]);
    //       it is big-endian on disc, so a little-endian port converts it field by field here
    //       (docs/format-byteorder.md)
    Stream_StreamLoadFixedSize(pObject, sizeof(gPgaData.aSponsorship), gPgaData.aSponsorship);
}

// The 'PGAn' loader: the tournament names block (Tournament.nName are offsets into it) is copied
// into a new 16-byte aligned block, gPgaData.pNames, and the stream object freed. An empty object
// leaves pNames as it was.
void GameModeDriverPGATour_Locale_PgaTourMode_LoadPGAnFromStream(UStreamObject* pObject) {
    void* pData;
    u32 nSize = fn_8000E81C(pObject, &pData);
    if (nSize) {
        gPgaData.pNames = fn_800951A0(nSize, 0x10, 1);
        Mem_cpy(gPgaData.pNames, pData, nSize);
        StaticMem_Free(pObject);
    }
}

// pfnShutdown, as the mode ends: gpGame's nC and n10 go back to 1, the options' nWeather and nWind
// that PrepareForTeeOff replaced come back, and the tour-round flag (GM_Currently_PgaTourMode) is
// cleared. options.n18, which SetTournament replaced (keeping the old value in gPgaSavedOptions18),
// is not put back (see the EA bug there).
void GameModeDriverPGATour_Shutdown(void) {
    gpGame->nC = 1;
    gpGame->n10 = 1;
    gSession.options.nWeather = gPgaSavedWeather;
    gSession.options.nWind = gPgaSavedWind;
    gbPgaTourRoundActive = 0;
}

// pfnStartGamePreData, as a round starts (GM_InitModule_PreDataStream): the round count
// (gpGame->nE0) comes from the current tournament's format.
void GameModeDriverPGATour_StartGamePreData(void) {
    s32 nTourEvent = gPgaData.aTournament[gpSaveData->tour.nEvent].nTourEvent - 1;
    gpGame->nE0 = gPgaData.aTourEvent[nTourEvent].nRounds;
}

// Sets up tournament format i (an index into gPgaData.aTourEvent) for the current round: all five
// players play its tee set, the course is the round's course (round gpGame->nDC), every hole uses
// the round's pin position (profile 0's tour.nRound), and the round's n8 replaces options.n18, the
// green speed (the old value kept in gPgaSavedOptions18, never read back), and is applied
// (fn_80055C40). The tee set is written back to the format unchanged; the original has that store.
void GameModeDriverPGATour_SetTournament(s32 i) {
    PlayerNumber_t nPlayer = PLR_1_e;
    TourEvent* pEvent = &gPgaData.aTourEvent[i];
    PlayerNumber_t k;
    int h;
    s32 nTee;
    nTee = pEvent->nTeeSet;
    for (k = 0; k < 5; k++) {
        gSession.nTeeSet[k] = nTee;
    }
    pEvent->nTeeSet = nTee;
    GM_SetCurrentCourse(gPgaData.aTourEvent[i].aRound[gpGame->nDC].nCourse);
    gSession.nPinSet = gPgaData.aTourEvent[i].aRound[gpSaveData[nPlayer].tour.nRound].nPinSet - 1;
    for (h = 0; h < 18; h++) {
        gpGame->nPinSet[h] = gPgaData.aTourEvent[i].aRound[gpSaveData[nPlayer].tour.nRound].nPinSet - 1;
    }
    // EA bug: the player's green speed option is kept here but nothing puts it back
    // (GameModeDriverPGATour_Shutdown restores only nWeather and nWind), so after a tour round the
    // option holds the tournament's green speed instead of the one the player chose
    gPgaSavedOptions18 = gSession.options.n18;
    gSession.options.n18 = (u8)gPgaData.aTourEvent[i].aRound[gpSaveData[nPlayer].tour.nRound].n8;
    fn_80055C40(gSession.options.n18);
}

// A round of the current tournament is about to start (front-end message GM_vStartEventCheckDisc).
// The options' nC and nWind are saved (Shutdown puts them back) and set to 4 and calm, the tour-round flag
// (GM_Currently_PgaTourMode) set, the playoff hole reset and the win info cleared. For a tournament
// with a format: one player, the round number and count, the format's course set up
// (SetTournament), the user-quit flag cleared, the round started for the field
// (GM_PgaTourSim_SimRound, flags 5: the player in it, scores not kept yet) at the bracket's
// strength, the round's statistics cleared and counted (a round, and a tournament on its first
// round), and all 18 holes selected.
void GameModeDriverPGATour_PrepareForTeeOff(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    s32 nEvent = gpSaveData[nPlayer].tour.nEvent;
    PgaStatCounts* pRec = &gPgaRoundStats;
    s32 nFormat;
    gPgaSavedWeather = gSession.options.nWeather;
    gPgaSavedWind = gSession.options.nWind;
    gSession.options.nWeather = 4;
    gSession.options.nWind = 0;
    gbPgaTourRoundActive = 1;
    gPgaPlayoffHole = 16;
    gPgaWinInfo.bPlaced = 0;
    if (gPgaData.aTournament[nEvent].nTourEvent) {
        gSession.nNumPlayers = 1;
        gpGame->nDC = gpSaveData[nPlayer].tour.nRound;
        nFormat = gPgaData.aTournament[nEvent].nTourEvent - 1;
        gpGame->nE0 = gPgaData.aTourEvent[nFormat].nRounds;
        GameModeDriverPGATour_SetTournament(gPgaData.aTournament[nEvent].nTourEvent - 1);
        GM_PgaTourSim_SetUserQuit(0, 0);
        GM_PgaTourSim_SimRound(0, &gpSaveData[nPlayer].tour.aEvent[gpSaveData[nPlayer].tour.nEvent],
                    gpSaveData[nPlayer].tour.nRound,
                    gPgaData.aTourEvent[nFormat].aFieldLowScore[GameModeDriverPGATour_GetCurrentBracket(0)],
                    5);
        Mem_set(pRec, 0, sizeof(*pRec));
        pRec->nRounds++;
        if (gpSaveData[nPlayer].tour.nRound == 0) {
            pRec->nEvents++;
        }
        GM_SelectHoleSet(1);
    }
}

// 1 while a PGA TOUR round is being played: from GameModeDriverPGATour_PrepareForTeeOff until the
// mode's Shutdown.
u8 GM_Currently_PgaTourMode(void) {
    return gbPgaTourRoundActive;
}

// A profile, and its current tournament. fake match: GameModeDriverPGATour_EndGame reaches the
// profile through these in two statements, where the original adds the profile's offset to
// gpSaveData last (indexed load/store); gpSaveData[nPlayer] written out adds it first.
static inline SaveProfile* Tour_Profile(PlayerNumber_t nPlayer) {
    return &gpSaveData[nPlayer];
}

static inline SeasonEvent* Tour_CurrentEvent(PlayerNumber_t nPlayer) {
    return &gpSaveData[nPlayer].tour.aEvent[gpSaveData[nPlayer].tour.nEvent];
}

// pfnEndGame, when a tour round ends, for profile 0: the first round counts a tournament started
// (tour.nEventsStarted); after the second round of a tournament of four or more rounds the cut is
// made (GM_PgaTourSim_CutBadGolfers) and a player who missed it is marked cut; the player's total
// score goes into the season record, and after the last round the tournament ends
// (GameModeDriverPGATour_EndTournament). The round number moves on later
// (GameModeDriverPGATour_CheckAdvanceTournament).
void GameModeDriverPGATour_EndGame(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    if (gpSaveData[nPlayer].tour.nRound == 0) {
        Tour_Profile(nPlayer)->tour.nEventsStarted++;
    }
    if (GameModeDriverPGATour_GetRounds(gpSaveData[nPlayer].tour.nEvent) >= 4 &&
        gpSaveData[nPlayer].tour.nRound == 1) {
        GM_PgaTourSim_CutBadGolfers(0);
        if (GM_PgaTourSim_GetWasCutFromEntrantID(0, 0)) {
            Tour_CurrentEvent(nPlayer)->nUserRankType = 1;
        }
    }
    gpSaveData[nPlayer].tour.aEvent[gpSaveData[nPlayer].tour.nEvent].nUserScore = GM_PgaTourSim_GetTotalScoreFromEntrantID(0, 0, 1);
    if (gpSaveData[nPlayer].tour.nRound + 1 >=
        GameModeDriverPGATour_GetRounds(gpSaveData[nPlayer].tour.nEvent)) {
        GameModeDriverPGATour_EndTournament(0);
    }
}

// Whether holing this putt puts the player in the lead:
// in a playoff, beating the best score on this hole; otherwise, not ahead now and ahead with it.
u8 GameModeDriverPGATour_IsPuttForLead(int nPlayer) {
    int bLead;
    if (gpGame->bInPlayoff) {
        return gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1 <
               GM_PgaTourSim_GetBestOpponentPlayoffHoleScore(nPlayer, Game_CurHoleIndex());
    }
    bLead = 0;
    if (GM_GetGolferRelativeCumulativeScore(nPlayer, 0)
        >= GM_PgaTourSim_GetBestOpponentRelativeScore(nPlayer, 1) &&
        GM_GetGolferRelativeCumulativeScore(nPlayer, 1) + 1
                < GM_PgaTourSim_GetBestOpponentRelativeScore(nPlayer, 1)) {
        bLead = 1;
    }
    return bLead;
}

// Whether holing this putt wins the tournament: in a playoff, a putt for the lead (IsPuttForLead);
// otherwise on the last hole of the last round, a putt that would put the player strictly ahead.
u8 GameModeDriverPGATour_IsPuttForWin(s32 nPlayer) {
    s32 nRounds;
    if (gpGame->bInPlayoff) {
        return GameModeDriverPGATour_IsPuttForLead(nPlayer);
    }
    nRounds = GameModeDriverPGATour_GetRounds(gpSaveData[nPlayer].tour.nEvent);
    return GM_GetNumHolesRemainingInRound() == 1 && gpSaveData[nPlayer].tour.nRound + 1 >= nRounds &&
           GM_GetGolferRelativeCumulativeScore(nPlayer, 1) + 1
                   < GM_PgaTourSim_GetBestOpponentRelativeScore(nPlayer, 1);
}

// Strokes ahead of the best other player, negative when behind (in a playoff, on this hole).
s32 GameModeDriverPGATour_GetCurrentLead(int nPlayer) {
    if (gpGame->bInPlayoff) {
        return GM_PgaTourSim_GetBestOpponentPlayoffHoleScore(nPlayer, Game_CurHoleIndex()) - gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()];
    }
    return GM_PgaTourSim_GetBestOpponentRelativeScore(nPlayer, 1)
            - GM_GetGolferRelativeCumulativeScore(nPlayer, 0);
}

// GetCurrentLead as it would be if the ball dropped with one more stroke: strokes ahead of the best
// other player, negative when behind (in a playoff, on this hole).
s32 GameModeDriverPGATour_GetPotentialLead(int nPlayer) {
    if (gpGame->bInPlayoff) {
        return GM_PgaTourSim_GetBestOpponentPlayoffHoleScore(nPlayer, Game_CurHoleIndex()) -
               (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] + 1);
    }
    return GM_PgaTourSim_GetBestOpponentRelativeScore(nPlayer, 1)
            - (GM_GetGolferRelativeCumulativeScore(nPlayer, 1) + 1);
}

// pfnGetPotentialHoleResult: how the hole would end for the player if the ball dropped now. The
// tour never says: always 3, unknown (TW06 GM_HoleResult_t: 0 loses, 1 ties, 2 wins, 3 unknown);
// HoleScore.c's default (fn_800D030C) works it out for the other modes.
s32 GameModeDriverPGATour_GetPotentialHoleResult(int nPlayer) {
    return 3;
}

// The player's result in the current tournament (TW06 PgaTour_WinInfo: placed, position, winnings):
// cleared as a round starts, set by AwardMoney, read by the result screen (GameUICommands).
PgaTour_WinInfo* GameModeDriverPGATour_GetWinInfo(void) {
    return &gPgaWinInfo;
}

// After a tournament the player won (called by EndTournament): the end-of-tournament movies go on
// GUI queue 5. The first tour win gets 31; a major (Tournament.bIsAMajor) gets 8 once three majors
// are won, else one of 27..30 at random; otherwise tournaments 9 and 8 have their own (9 and 10).
// Nothing when the win info says the player did not place.
void GameModeDriverPGATour_PlayEndOfGameMovies(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    Tournament* p = GameModeDriverPGATour_GetEventInfo(gpSaveData[nPlayer].tour.nEvent);
    int i;
    int nWins;
    if (gPgaWinInfo.bPlaced == 1) {
        if (GameModeDriverPGATour_GetNumEventsWon() == 0) {
            GUI_QueueMessage(5, 31, 0, 0);
        }
        if (p->bIsAMajor) {
            nWins = 0;
            for (i = 0; i <= gpSaveData[nPlayer].tour.nEvent; i++) {
                if (gPgaData.aTournament[i].bIsAMajor && gpSaveData[nPlayer].tour.aEvent[i].nUserRank == 1) {
                    nWins++;
                }
            }
            if (nWins >= 3) {
                GUI_QueueMessage(5, 8, 0, 0);
            } else {
                GUI_QueueMessage(5, (Misc_RandFunc(1) & 3) + 27, 0, 0);
            }
        } else if (gpSaveData[nPlayer].tour.nEvent == 9) {
            GUI_QueueMessage(5, 9, 0, 0);
        } else if (gpSaveData[nPlayer].tour.nEvent == 8) {
            GUI_QueueMessage(5, 10, 0, 0);
        }
    }
}

// The last round is over: the winner is settled (GM_PgaTourSim_SimTournamentWinner). If the player
// finished first, the movies are queued (PlayEndOfGameMovies) and, the first time this tournament
// is won, the win is recorded in the profile (aC8: the date, the score and the tournament's first
// prize aPrize[bracket][1]). The player's leaderboard winnings, if any, are paid
// (GM_Earnings_AwardMoney) and added to gPlayers[].money.n4.
void GameModeDriverPGATour_EndTournament(int nPlayer) {
    s32 nBracket = GameModeDriverPGATour_GetCurrentBracket(nPlayer);
    Tournament* p = GameModeDriverPGATour_GetEventInfo(gpSaveData[nPlayer].tour.nEvent);
    s32 nMoney;
    GM_PgaTourSim_SimTournamentWinner(nPlayer);
    if (GM_PgaTourSim_GetScoreRankFromEntrantID(nPlayer, 0) == 1) {
        GameModeDriverPGATour_PlayEndOfGameMovies();
        if (GM_Earnings_GiveAwardToUser(nPlayer,
                                        &gpSaveData[nPlayer].aC8[gpSaveData[nPlayer].tour.nEvent].award)) {
            gpSaveData[nPlayer].aC8[gpSaveData[nPlayer].tour.nEvent].nScore = GM_PgaTourSim_GetTotalScoreFromEntrantID(nPlayer, 0, 1);
            gpSaveData[nPlayer].aC8[gpSaveData[nPlayer].tour.nEvent].n6 = p->aPrize[nBracket][1];
        }
    }
    nMoney = GM_PgaTourSim_GetLeaderboardWinningsFromEntrantID(nPlayer, 0);
    if (nMoney) {
        GM_Earnings_AwardMoney(0, nMoney, NULL);
        gPlayers[nPlayer].money.n4 += nMoney;
    }
}

// The tournament is over for the player: its champion and winning score are kept with the
// player's result (cut, a place, or did not play), and the season moves on to the next tournament.
void GameModeDriverPGATour_AdvanceEvent(int nPlayer) {
    SeasonEvent* p = &gpSaveData[nPlayer].tour.aEvent[gpSaveData[nPlayer].tour.nEvent];
    s32 nLeader = GM_PgaTourSim_GetEntrantIDFromScoreRow(nPlayer, 0);
    s32 nGolfer = GM_PgaTourSim_GetGolferIDFromEntrantID(nPlayer, nLeader);
    strcpy(p->szChampName, GM_PgaTourSim_GetNameFromGolferID(nPlayer, nGolfer));
    p->nChampScore = GM_PgaTourSim_GetRelativeScoreFromEntrantID(nPlayer, nLeader, 1);
    if (GM_PgaTourSim_IsEntrantUser(nPlayer, 0)) {
        if (GM_PgaTourSim_GetWasCutFromEntrantID(nPlayer, 0)) {
            p->nUserRank = 0;
            p->nUserScore = 0;
            p->nUserRankType = 1;
        } else {
            p->nUserRank = GM_PgaTourSim_GetScoreRankFromEntrantID(nPlayer, 0);
            p->nUserScore = GM_PgaTourSim_GetRelativeScoreFromEntrantID(nPlayer, 0, 1);
            p->nUserRankType = 2;
        }
    } else {
        p->nUserRank = 0;
        p->nUserScore = 0;
        p->nUserRankType = 0;
    }
    gpSaveData[nPlayer].tour.nRound = 0;
    gpSaveData[nPlayer].tour.nEvent =
        GameModeDriverPGATour_GetEventOnOrAfter(gpSaveData[nPlayer].tour.nEvent + 1);
}

// The round's statistics go into the player's own season counts (golfer PGA_USER_GOLFER): most
// are added, the longest drive and putt keep the higher value.
void GameModeDriverPGATour_CommitUserRoundStatCounts(s32 nPlayer) {
    PgaStatCounts* pRound = &gPgaRoundStats;
    PgaStatCounts* pTotal = &gpSaveData[nPlayer].tour.aStats[PGA_USER_GOLFER];
    pTotal->nEvents += pRound->nEvents;
    pTotal->nRounds += pRound->nRounds;
    pTotal->nLongestDrive = pTotal->nLongestDrive <= pRound->nLongestDrive
                  ? pRound->nLongestDrive : pTotal->nLongestDrive;
    pTotal->nDrives += pRound->nDrives;
    pTotal->nDriveDistance += pRound->nDriveDistance;
    pTotal->nLongestPutt = pTotal->nLongestPutt <= pRound->nLongestPutt
                  ? pRound->nLongestPutt : pTotal->nLongestPutt;
    pTotal->nFairwaysHit += pRound->nFairwaysHit;
    pTotal->nFairways += pRound->nFairways;
    pTotal->nGreensHit += pRound->nGreensHit;
    pTotal->nHoles += pRound->nHoles;
    pTotal->nPutts += pRound->nPutts;
    pTotal->nGIRPutts += pRound->nGIRPutts;
    pTotal->nBunkerSaves += pRound->nBunkerSaves;
    pTotal->nBunkers += pRound->nBunkers;
    pTotal->nNonGIRPars += pRound->nNonGIRPars;
    pTotal->nBirdiesAfterBogey += pRound->nBirdiesAfterBogey;
    pTotal->nBogeys += pRound->nBogeys;
    pTotal->nEagles += pRound->nEagles;
    pTotal->nBirdies += pRound->nBirdies;
    pTotal->nPar3Birdies += pRound->nPar3Birdies;
    pTotal->nPar3Holes += pRound->nPar3Holes;
    pTotal->nPar4Birdies += pRound->nPar4Birdies;
    pTotal->nPar4Holes += pRound->nPar4Holes;
    pTotal->nPar5Birdies += pRound->nPar5Birdies;
    pTotal->nPar5Holes += pRound->nPar5Holes;
    pTotal->nGIRBirdies += pRound->nGIRBirdies;
    pTotal->nStrokes += pRound->nStrokes;
    pTotal->nPar3Strokes += pRound->nPar3Strokes;
    pTotal->nPar4Strokes += pRound->nPar4Strokes;
    pTotal->nPar5Strokes += pRound->nPar5Strokes;
    pTotal->nSeasonWinnings += pRound->nSeasonWinnings;
    pTotal->nMonthWinnings += pRound->nMonthWinnings;
    pTotal->nSeasonWins += pRound->nSeasonWins;
    pTotal->nPlayerOfYearPoints += pRound->nPlayerOfYearPoints;
    pTotal->nCareerWinnings += pRound->nCareerWinnings;
    pTotal->nCareerWins += pRound->nCareerWins;
}

// After a tour round (PGA TOUR menus, PGATourMsg_CheckAdvanceTournament). If the player quit the
// round (the tour simulation's user-quit flag, GM_PgaTourSim_DidUserQuit), every entrant goes back
// to the first tee with no strokes, and on the first round the field is emptied: the round does not
// count. Otherwise the round is committed (the player's statistics, the CPU entrants' statistics
// and every entrant's round score), the hole scores reset and the round number moved on; a player
// who missed the cut has the rest of the tournament simulated (SimCurrentTournament), and after the
// last round the tournament ends (AdvanceEvent).
void GameModeDriverPGATour_CheckAdvanceTournament(s32 nPlayer) {
    if (GM_PgaTourSim_DidUserQuit()) {
        GM_PgaTourSim_ResetHoleScores(nPlayer);
        if (gpSaveData[nPlayer].tour.nRound == 0) {
            GM_PgaTourSim_ResetTournament(nPlayer);
        }
    } else {
        GameModeDriverPGATour_CommitUserRoundStatCounts(nPlayer);
        GM_PgaTourSim_SimStats(nPlayer);
        GM_PgaTourSim_CommitRoundScores(nPlayer);
        GM_PgaTourSim_ResetHoleScores(nPlayer);
        gpSaveData[nPlayer].tour.nRound++;
        if (GM_PgaTourSim_GetWasCutFromEntrantID(nPlayer, 0)) {
            GameModeDriverPGATour_SimCurrentTournament(nPlayer, 0);
        }
        if (gpSaveData[nPlayer].tour.nRound >=
            GameModeDriverPGATour_GetRounds(gpSaveData[nPlayer].tour.nEvent)) {
            GameModeDriverPGATour_AdvanceEvent(nPlayer);
        }
    }
}

// The tour simulation's payout for the player (SplitWinnings): the win info (GetWinInfo) says the
// player placed, at entrant 0's rank on the leaderboard, winning nCash. No money is paid here;
// EndTournament pays it.
void GameModeDriverPGATour_AwardMoney(int nPlayer, s32 nCash) {
    gPgaWinInfo.bPlaced = 1;
    gPgaWinInfo.nPosition = GM_PgaTourSim_GetScoreRankFromEntrantID(nPlayer, 0);
    gPgaWinInfo.nWinnings = nCash;
}

// The player's bracket, 0..9: tournaments won (GameModeDriverPGATour_GetNumEventsWon) x 10 / 31, at
// most 9. It picks the prize column (Tournament.aPrize) and the field's strength
// (TourEvent.aFieldLowScore). Profile 0's wins; nPlayer is not read.
s32 GameModeDriverPGATour_GetCurrentBracket(int nPlayer) {
    s32 n = GameModeDriverPGATour_GetNumEventsWon() * 10 / 31;
    return n > 9 ? 9 : n;
}

// The rounds of the current tournament not played yet are simulated: each round's course is set and
// the round simulated for the field (GM_PgaTourSim_SimRound, flags 3 when bSimUser is set: the
// player's rounds simulated too, else 0), then the playoff and the winner are settled.
// GameModeDriverPGATour_SkipToEvent uses it to skip ahead, CheckAdvanceTournament after the player
// misses the cut.
void GameModeDriverPGATour_SimCurrentTournament(int nPlayer, u8 bSimUser) {
    s32 nRounds = GameModeDriverPGATour_GetRounds(gpSaveData[nPlayer].tour.nEvent);
    Tournament* pEventInfo = GameModeDriverPGATour_GetEventInfo(gpSaveData[nPlayer].tour.nEvent);
    s32 uFlags;
    s32 nTourEvent;
    while (gpSaveData[nPlayer].tour.nRound < nRounds) {
        if (pEventInfo->nTourEvent) {
            GM_SetCurrentCourse(gPgaData.aTourEvent[pEventInfo->nTourEvent - 1]
                                    .aRound[gpSaveData[nPlayer].tour.nRound].nCourse);
            uFlags = 0;
            nTourEvent = gPgaData.aTournament[gpSaveData[nPlayer].tour.nEvent].nTourEvent - 1;
            if (bSimUser) {
                uFlags = 3;
            }
            GM_PgaTourSim_SimRound(nPlayer, &gpSaveData[nPlayer].tour.aEvent[gpSaveData[nPlayer].tour.nEvent],
                        gpSaveData[nPlayer].tour.nRound,
                        gPgaData.aTourEvent[nTourEvent]
                            .aFieldLowScore[GameModeDriverPGATour_GetCurrentBracket(nPlayer)],
                        uFlags);
        }
        gpSaveData[nPlayer].tour.nRound++;
    }
    GM_PgaTourSim_InitPlayoff(nPlayer);
    GM_PgaTourSim_SimTournamentWinner(nPlayer);
}

// pfnLoadHole, at the start of each hole (GM_InitForHole): the other entrants move on round the
// course (GM_PgaTourSim_AdvanceField).
void GameModeDriverPGATour_PostHoleLoadInit(void) {
    GM_PgaTourSim_AdvanceField(0);
}

// pfnEndHole. Outside a playoff, the hole just finished goes into the player's round statistics
// (lbl_80205ED8): strokes, putts (more than 10 count as 0), bunkers and saves, fairways and greens
// hit, the result against par, per par 3, 4 and 5, drives and the longest drive and putt. After the
// 18th hole a round at or under par extends the profile's run of such rounds
// (tour.nParRoundStreak), any other ends it. Then the player's hole on the leaderboard moves on
// (GM_PgaTourSim_AdvancePlayer). The u16 casts on the sums are in the original (a clrlwi before each add).
void GameModeDriverPGATour_EndHole(void) {
    PlayerNumber_t nPlayer;
    PgaStatCounts* pRound;
    Player* p;
    int nHole;
    int nPar;
    int nStrokes;
    int nPutts;
    u8 bUnder;
    int nPrev;
    s32 n;
    if (gpGame->bInPlayoff) {
        return;
    }
    pRound = &gPgaRoundStats;
    nPlayer = PLR_1_e;
    p = &gPlayers[0];
    nHole = Game_CurHoleIndex();
    nPar = Course_GetCurHolePar();
    nStrokes = gPlayers[0].nStrokes[nHole];
    nPutts = gPlayers[0].nPutts[nHole];
    bUnder = nStrokes < nPar;
    if (nPutts > 10) {
        nPutts = 0;
    }
    pRound->nHoles++;
    pRound->nStrokes += (u16)nStrokes;
    if (p->bBunkerThisHole) {
        pRound->nBunkers++;
        if (nStrokes <= nPar) {
            pRound->nBunkerSaves++;
        }
    }
    if (nPar >= 4) {
        pRound->nFairways++;
        if (p->bFairwayHit[nHole]) {
            pRound->nFairwaysHit++;
        }
    }
    if (p->bGreenInReg[nHole]) {
        pRound->nGreensHit++;
        pRound->nGIRPutts += (u16)nPutts;
        if (bUnder) {
            pRound->nGIRBirdies++;
        }
    } else if (nStrokes <= nPar) {
        pRound->nNonGIRPars++;
    }
    pRound->nPutts += (u16)nPutts;
    if (bUnder) {
        if (nStrokes < nPar - 1) {
            pRound->nEagles++;
        }
        pRound->nBirdies++;
    } else if (nStrokes > nPar) {
        pRound->nBogeys++;
    }
    switch (nPar) {
    case 3:
        pRound->nPar3Holes++;
        pRound->nPar3Strokes += (u16)nStrokes;
        if (bUnder) {
            pRound->nPar3Birdies++;
        }
        break;
    case 4:
        pRound->nPar4Holes++;
        pRound->nPar4Strokes += (u16)nStrokes;
        if (bUnder) {
            pRound->nPar4Birdies++;
        }
        break;
    case 5:
        pRound->nPar5Holes++;
        pRound->nPar5Strokes += (u16)nStrokes;
        if (bUnder) {
            pRound->nPar5Birdies++;
        }
        break;
    }
    if (nHole >= 1 && bUnder) {
        nPrev = nHole - 1;
        if (p->nStrokes[nPrev] > Course_GetHolePar(nPrev)) {
            pRound->nBirdiesAfterBogey++;
        }
    }
    if (fn_800D3080(nHole)) {
        pRound->nDrives++;
        pRound->nDriveDistance += p->nC24;
    }
    pRound->nLongestDrive =
        pRound->nLongestDrive <= (u16)p->nLongestDrive ? (u16)p->nLongestDrive : pRound->nLongestDrive;
    pRound->nLongestPutt =
        pRound->nLongestPutt <= (u16)p->nLongestPutt ? (u16)p->nLongestPutt : pRound->nLongestPutt;
    if (nHole == 17) {
        n = GM_PgaTourSim_GetRoundScoreFromEntrantID(0, 0, gpSaveData[nPlayer].tour.nRound);
        if (n <= fn_800D2FB4(gSession.nTeeSet[0])) {
            gpSaveData[nPlayer].tour.nParRoundStreak++;
        } else {
            gpSaveData[nPlayer].tour.nParRoundStreak = 0;
        }
    }
    GM_PgaTourSim_AdvancePlayer(0, nHole + 1);
}

// pfnGameFinished: whether play is over. In a playoff, the playoff standings take the hole just
// played and play ends unless another playoff hole follows (GoToPlayoff). Otherwise 0 while a
// selected hole is left; after the last round's last hole the playoff is set up and play ends
// unless there is a tie to play off; after an earlier round, 1. bCheck only goes on to GoToPlayoff,
// which does not read it.
u8 GameModeDriverPGATour_GameFinished(u8 bCheck) {
    s32 i;
    if (gpGame->bInPlayoff) {
        GM_PgaTourSim_UpdatePlayoffs(0, Game_CurHoleIndex());
        return GameModeDriverPGATour_GoToPlayoff(bCheck) == 0;
    }
    for (i = Game_CurHoleIndex() + 1; i < 18; i++) {
        if (gpGame->bHoleSelected[i]) {
            return 0;
        }
    }
    if (gpGame->nDC + 1 >= gpGame->nE0) {
        GM_PgaTourSim_InitPlayoff(0);
        return GameModeDriverPGATour_GoToPlayoff(bCheck) == 0;
    }
    return 1;
}

// Whether there is a playoff: more than one entrant in it and the player one of them. If so every
// player's strokes and mode points are cleared, the playoff flags (gpGame bInPlayoff,
// bPlayoffFullRound) set, the next playoff hole (the 18th, then the 16th, 17th, 18th, ... :
// lbl_80282340) made the only one selected, and the golfers-tied message shown. bCheck is not read.
u8 GameModeDriverPGATour_GoToPlayoff(u8 bCheck) {
    u8 bPlayoff = 0;
    s32 i;
    int h;
    if (GM_PgaTourSim_GetNumPlayoffEntrants(0) > 1 && GM_PgaTourSim_EntrantIsInPlayoff(0, 0)) {
        bPlayoff = 1;
    }
    if (bPlayoff) {
        gpGame->bPlayoffFullRound = 1;
        for (i = 0; i < gNumPlayersSetUp; i++) {
            for (h = 0; h < 18; h++) {
                PLAYER(i)->nStrokes[h] = 0;
                PLAYER(i)->nModePoints[h] = 0;
            }
        }
        gPgaPlayoffHole++;
        if (gPgaPlayoffHole > 17) {
            gPgaPlayoffHole = 15;
        }
        GM_SelectHoleSet(0);
        GM_SelectSingleHole(gPgaPlayoffHole);
        gpGame->bInPlayoff = 1;
        GUI_GolfersTiedUIMessage();
    }
    return bPlayoff;
}

// The number of tournaments in a season (31).
s32 GM_PgaTourMode_GetNEvents(void) {
    return 31;
}

// Which tournament is played on a date and which of its rounds: each tournament starts on its
// aStartDate for the season (seasons 2004..2013) and has one round a day. Returns 1 with the
// tournament (*pId) and round (*pRound, 0-based); 0 with *pId -1 and *pRound 0 when none is on.
u8 GameModeDriverPGATour_GetEventByDate(u16 nDate, s32* pId, s32* pRound) {
    s32 nMonth;
    s32 nDay;
    s32 i;
    s32 d;
    s32 nYear;
    s32 nSeason;
    u8 bFound;
    CalDate_GetMDY(&nDate, &nMonth, &nDay, &nYear);
    bFound = 0;
    nSeason = nYear - 2004;
    if (nSeason >= 0 && nSeason < 10) {
        for (i = 0; i < 31; i++) {
            d = nDate - gPgaData.aTournament[i].aStartDate[nSeason];
            if (d >= 0 && d < GameModeDriverPGATour_GetRounds(i)) {
                *pId = i;
                bFound = 1;
                *pRound = d;
                break;
            }
        }
    }
    if (!bFound) {
        *pId = -1;
        *pRound = 0;
    }
    return bFound;
}

// The tournament profile 0 is on (0..30, -1 once the season is over) and, in *pRound, its current
// round (0-based).
s32 GameModeDriverPGATour_GetSelectedEvent(s32* pRound) {
    PlayerNumber_t nPlayer = PLR_1_e;
    *pRound = gpSaveData[nPlayer].tour.nRound;
    return gpSaveData[nPlayer].tour.nEvent;
}

// The first tournament held in profile 0's season after its current one, or -1 when the season has
// none left.
s32 GameModeDriverPGATour_GetNextEvent(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    return GameModeDriverPGATour_GetEventOnOrAfter(gpSaveData[nPlayer].tour.nEvent + 1);
}

// The last tournament held this season (searched from tournament 1 on; 0 if none).
s32 GameModeDriverPGATour_GetFinalEventOfSeason(void) {
    s32 nLast = 0;
    s32 i = GameModeDriverPGATour_GetEventOnOrAfter(1);
    while (i != -1) {
        nLast = i;
        i = GameModeDriverPGATour_GetEventOnOrAfter(i + 1);
    }
    return nLast;
}

// The season skips ahead to tournament nEvent (the calendar's PGATour_Play, when the player picks a
// later day): a tournament profile 0 has started (round > 0) is abandoned, the player cut from it,
// and every tournament before nEvent has its remaining rounds simulated for the field and ends
// (champion and profile 0's result recorded, the season moving on).
void GameModeDriverPGATour_SkipToEvent(s32 nEvent) {
    PlayerNumber_t nPlayer = PLR_1_e;
    if (nEvent != gpSaveData[nPlayer].tour.nEvent && gpSaveData[nPlayer].tour.nRound > 0) {
        GM_PgaTourSim_CutEntrant(0, 0);
    }
    while (gpSaveData[nPlayer].tour.nEvent < nEvent) {
        GameModeDriverPGATour_SimCurrentTournament(0, 0);
        GameModeDriverPGATour_AdvanceEvent(0);
    }
}

// Tournament i's data (gPgaData.aTournament[i]), or NULL for -1 or i past the last (31).
Tournament* GameModeDriverPGATour_GetEventInfo(s32 i) {
    if (i != -1 && i < 31) {
        return &gPgaData.aTournament[i];
    }
    return 0;
}

// Tournament i's number of rounds (from its format; 1 without one).
s32 GameModeDriverPGATour_GetRounds(s32 i) {
    if (gPgaData.aTournament[i].nTourEvent) {
        return gPgaData.aTourEvent[gPgaData.aTournament[i].nTourEvent - 1].nRounds;
    }
    return 1;
}

// Profile 0's tour moves to the next season: it starts at the season's first tournament held and
// every golfer's season counts are cleared (GM_PgaTourSim_ClearSeason); returns 1. After the tenth
// season (2013) the tour is over: nSeason stays at 10, nEvent is set to 0 and it returns 0.
s32 GameModeDriverPGATour_AdvanceSeason(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    gpSaveData[nPlayer].tour.nSeason++;
    if (gpSaveData[nPlayer].tour.nSeason >= 10) {
        gpSaveData[nPlayer].tour.nSeason = 10;
        gpSaveData[nPlayer].tour.nEvent = 0;
        return 0;
    }
    gpSaveData[nPlayer].tour.nEvent = GameModeDriverPGATour_GetEventOnOrAfter(0);
    GM_PgaTourSim_ClearSeason(&gpSaveData[nPlayer].tour);
    return 1;
}

// Profile 0's season, 0 = 2004.
s32 GameModeDriverPGATour_GetCurrentSeason(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    return gpSaveData[nPlayer].tour.nSeason;
}

// The current season's year: 2004 for season 0.
s32 GameModeDriverPGATour_GetCurrentSeasonYear(void) {
    return GameModeDriverPGATour_GetCurrentSeason() + 2004;
}

// The first tournament from i on that is held in profile 0's season (it has a start date for that
// season), or -1 when none is left.
s32 GameModeDriverPGATour_GetEventOnOrAfter(s32 i) {
    PlayerNumber_t nPlayer = PLR_1_e;
    s32 nEvent;
    u8 bFound = 0;
    while (!bFound) {
        Tournament* p = GameModeDriverPGATour_GetEventInfo(i);
        if (p != NULL) {
            if (p->aStartDate[gpSaveData[nPlayer].tour.nSeason] != 0) {
                nEvent = i;
                bFound = 1;
            }
        } else {
            nEvent = -1;
            bFound = 1;
        }
        i++;
    }
    return nEvent;
}

// The tournament played on a date (any day of its rounds, in the date's own season), or NULL. The
// PGA TOUR entry of the calendar's gCalendarGetEventInfoByDate table.
Tournament* GM_PgaTourMode_GetEventInfoByDate(u16 nDate) {
    s32 nId;
    s32 nRound;
    if (GameModeDriverPGATour_GetEventByDate(nDate, &nId, &nRound)) {
        return GameModeDriverPGATour_GetEventInfo(nId);
    }
    return 0;
}

// Tournament i's total purse in bracket k, in dollars.
s32 GameModeDriverPGATour_ComputePurseForBracket(s32 i, s32 k) {
    Tournament* p = GameModeDriverPGATour_GetEventInfo(i);
    return p->aPrize[k][0] * 1000;
}

// Tournament i's first prize (the winner's share) in bracket k, in dollars.
s32 GameModeDriverPGATour_ComputeFirstPrizeForBracket(s32 i, s32 k) {
    Tournament* p = GameModeDriverPGATour_GetEventInfo(i);
    return p->aPrize[k][1] * 1000;
}

// Tournament i's first day in the current season, 0 when it is not held this season, or 0xFFFF for
// -1 or i past the last.
u16 GameModeDriverPGATour_GetStartDate(s32 i) {
    Tournament* p = GameModeDriverPGATour_GetEventInfo(i);
    if (p == NULL) {
        return 0xFFFF;
    }
    return p->aStartDate[GameModeDriverPGATour_GetCurrentSeason()];
}

// Tournament i's last day in the current season (its start date plus its rounds less one), or
// 0xFFFF for -1 or i past the last.
u16 GameModeDriverPGATour_GetEndDate(s32 i) {
    u16 nDate;
    Tournament* p = GameModeDriverPGATour_GetEventInfo(i);
    if (p == NULL) {
        return 0xFFFF;
    }
    nDate = p->aStartDate[GameModeDriverPGATour_GetCurrentSeason()];
    CalDate_AddDays(&nDate, GameModeDriverPGATour_GetRounds(i) - 1);
    return nDate;
}

// Tournament i's name, in the names block of the 'PGAn' object.
char* GameModeDriverPGATour_GetName(s32 i) {
    return gPgaData.pNames + gPgaData.aTournament[i].nName;
}

// Profile 0's current tournament: -1 once the season's last one is over.
s32 GameModeDriverPGATour_GetCurrentEventID(void) {
    PlayerNumber_t nPlayer = PLR_1_e;
    return gpSaveData[nPlayer].tour.nEvent;
}

// Tournament i's icon (Tournament.nTextureID): drawn in the calendar on its last day
// (PGATour_FillCell) and with a won tournament in the PGA TOUR menus. No range check: i must be a
// tournament.
s32 GameModeDriverPGATour_GetTextureID(s32 i) {
    return GameModeDriverPGATour_GetEventInfo(i)->nTextureID;
}

// Tournament i's champion before the tour is played, from the 'PGAc' data: a new tour in a profile
// starts each tournament's champion with it (GameModeDriverPGATour_GetChamp gives the latest).
char* GameModeDriverPGATour_GetInitialChampName(s32 i) {
    return gPgaData.aTournament[i].szChampName;
}

// The winning score of tournament i's champion before the tour is played, from the 'PGAc' data (see
// GameModeDriverPGATour_GetInitialChampName).
s32 GameModeDriverPGATour_GetInitialChampScore(s32 i) {
    return gPgaData.aTournament[i].nChampScore;
}

// The course of each round of tournament p into pCourses (up to 4); returns the number of rounds.
// The NULL test on its format never fails: p must have one (nTourEvent 0 would read the entry
// before aTourEvent).
s32 GameModeDriverPGATour_GetCourses(Tournament* p, s32* pCourses) {
    TourEvent* pEvent = &gPgaData.aTourEvent[p->nTourEvent - 1];
    s32 nRounds;
    s32 i;
    TourRound* pRound;
    if (pEvent != NULL) {
        nRounds = pEvent->nRounds;
        pRound = pEvent->aRound;
        for (i = 0; i < nRounds; i++) {
            pCourses[i] = pRound[i].nCourse;
        }
        return nRounds;
    }
    return 0;
}

// Tournament i's total purse as money text into pDst (no "$"): in the bracket profile 0 played it
// in for a tournament already past, else in the player's current bracket.
void GameModeDriverPGATour_GetPurseString(s32 i, char* pDst) {
    PlayerNumber_t nPlayer = PLR_1_e;
    SeasonEvent* p = &gpSaveData[nPlayer].tour.aEvent[i];
    s32 nBracket;
    if (i < GameModeDriverPGATour_GetCurrentEventID()) {
        nBracket = p->nUserBracket;
    } else {
        nBracket = GameModeDriverPGATour_GetCurrentBracket(nPlayer);
    }
    UI_GetMoneyString(GameModeDriverPGATour_ComputePurseForBracket(i, nBracket), pDst);
}

// The current tournament's leader into pDst: the golfer's name, or "Tied (%d players)" when several
// share first place.
void GameModeDriverPGATour_GetCurrentEventLeader(char* pDst) {
    s32 n = GM_PgaTourSim_GetNumFirstPlaceEntrants(0);
    if (n > 1) {
        sprintf(pDst, "Tied (%d players)", n);
    } else {
        s32 nLeader = GM_PgaTourSim_GetEntrantIDFromScoreRow(0, 0);
        s32 nGolfer = GM_PgaTourSim_GetGolferIDFromEntrantID(0, nLeader);
        strcpy(pDst, GM_PgaTourSim_GetNameFromGolferID(0, nGolfer));
    }
}

// The current tournament leader's score to par so far (the entrant in score row 0; the last
// argument is 1 unless the leader is the player).
int GameModeDriverPGATour_GetCurrentLeaderScore(void) {
    s32 nLeader = GM_PgaTourSim_GetEntrantIDFromScoreRow(0, 0);
    return GM_PgaTourSim_GetRelativeScoreFromEntrantID(0, nLeader, GM_PgaTourSim_IsEntrantUser(0, nLeader) == 0);
}

// Tournament i's first prize (the winner's share) as money text into pDst (no "$"): in the bracket
// profile 0 played it in for a tournament already past, else in the player's current bracket.
void GameModeDriverPGATour_GetWinnerEarningsString(s32 i, char* pDst) {
    PlayerNumber_t nPlayer = PLR_1_e;
    SeasonEvent* p = &gpSaveData[nPlayer].tour.aEvent[i];
    s32 nBracket;
    if (i < GameModeDriverPGATour_GetCurrentEventID()) {
        nBracket = p->nUserBracket;
    } else {
        nBracket = GameModeDriverPGATour_GetCurrentBracket(nPlayer);
    }
    UI_GetMoneyString(GameModeDriverPGATour_ComputeFirstPrizeForBracket(i, nBracket), pDst);
}

// The player's score to par so far in the current tournament (entrant 0). nEvent is not read
// (FE_CalendarPopups.c passes the tournament shown).
int GameModeDriverPGATour_GetUserScore(s32 nEvent) {
    return GM_PgaTourSim_GetRelativeScoreFromEntrantID(0, 0, GM_PgaTourSim_IsEntrantUser(0, 0) == 0);
}

// Profile 0's result in tournament i this season into pDst: "Did Not Play", "Cut", or the place as
// a number (pDst is left as it is for any other result kind).
void GameModeDriverPGATour_GetUserFinishString(s32 i, char* pDst) {
    switch (gpSaveData->tour.aEvent[i].nUserRankType) {
    case 0:
        strcpy(pDst, "Did Not Play");
        return;
    case 1:
        strcpy(pDst, "Cut");
        return;
    case 2:
        sprintf(pDst, "%d", gpSaveData->tour.aEvent[i].nUserRank);
        return;
    }
}

// Tournament i's latest champion into pDst, from profile 0's tour: set when the tournament ends,
// the tour data's champion (GameModeDriverPGATour_GetInitialChampName) until then.
void GameModeDriverPGATour_GetChamp(s32 i, char* pDst) {
    strcpy(pDst, gpSaveData->tour.aEvent[i].szChampName);
}

// The winning score of tournament i's latest champion (to par when the tournament was played on the
// tour), from profile 0's tour.
s32 GameModeDriverPGATour_GetChampScore(s32 i) {
    return gpSaveData->tour.aEvent[i].nChampScore;
}

// How many of the 31 tournaments profile 0 has won: its awards (aC8[].award), so each tournament
// counts once however often it was won. The player's prize bracket is worked out from it.
s32 GameModeDriverPGATour_GetNumEventsWon(void) {
    s32 n = 0;
    s32 i;
    for (i = 0; i < 31; i++) {
        if (gpSaveData->aC8[i].award.bWon == 1) {
            n++;
        }
    }
    return n;
}

// Sponsorship offer i's (0..10, the 'PGAp' data) game progress: the offer is made once the
// profile's game progress (GM_GetGameProgress) reaches it.
s32 GameModeDriverPGATour_GetSponsorshipProgress(s32 i) {
    return gPgaData.aSponsorship[i].nProgress;
}

// Sponsorship offer i's signing money, paid into the profile's money when the sponsorship is
// signed.
s32 GameModeDriverPGATour_GetSponsorshipStartCash(s32 i) {
    return gPgaData.aSponsorship[i].nStartCash;
}

// Sponsorship offer i's cash bonus: paid for each of the sponsor's items the player wears.
s32 GameModeDriverPGATour_GetSponsorshipBonusCash(s32 i) {
    return gPgaData.aSponsorship[i].nBonusCash;
}

// The tour's message after the player's hole into pDst; returns 1 when there is one, else 0. After
// the 18th hole of the second round: whether the player made the cut (the top 70). In a playoff
// (gpGame->bInPlayoff) that the player is in with at least one other: the opponent's best score on
// the playoff hole, which the player must beat.
s32 GameModeDriverPGATour_DisplayEndOfHoleMessage(char* pDst) {
    PlayerNumber_t nPlayer = PLR_1_e;
    if (gpSaveData[nPlayer].tour.nRound == 1 && GM_PgaTourSim_GetCurrentHoleFromEntrantID(0, 0) == 18) {
        if (GM_PgaTourSim_GetWasCutFromEntrantID(0, 0)) {
            strcpy(pDst, "TOURNAMENT CUT\n\nYou did not place in the top 70 after two\n"
                         "rounds. You have been cut from the tournament.");
            return 1;
        }
        strcpy(pDst, "TOURNAMENT CUT\n\nCongratulations! You placed in the top 70\n"
                     "after two rounds. You made the cut!");
        return 1;
    }
    if (gpGame->bInPlayoff && GM_PgaTourSim_EntrantIsInPlayoff(0, 0)
        && GM_PgaTourSim_GetNumPlayoffEntrants(0) > 1) {
        sprintf(pDst, "TOURNAMENT PLAYOFF\n\nYou're tied for first place. You must beat\n"
                      "your opponent's score of %d on the playoff\nhole to win.",
                GM_PgaTourSim_GetBestOpponentPlayoffHoleScore(0, gPgaPlayoffHole));
        return 1;
    }
    return 0;
}

// Profile nPlayer's current tournament: -1 once its season's last one is over (the calendar's
// PGATour_GetPopupType tests that).
s32 GameModeDriverPGATour_GetUsersCurrentEventID(s32 nPlayer) {
    return gpSaveData[nPlayer].tour.nEvent;
}
