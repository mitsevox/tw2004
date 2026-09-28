// PGATourSimulation.c (TW06's pgatoursimulation.c; TW07's PGATourSimulation.c, whose
// GM_PgaTourSim_ functions pair with these in the same order): the PGA TOUR simulation behind game
// mode 23 (GameModeDriverPGATour.c). For each tournament it picks the field (100 to 127 of the 174
// tour pros, and the player) and gives every entrant a four-round target score; each round it
// simulates the CPU entrants' holes toward that target, counts their season statistics and keeps
// the round scores; it makes the cut after the second round, and at the end settles the playoff
// and the winner, pays out the purse and gives the player's awards. It keeps the field (an entrant
// table in the save profile and one in memory with the hole strokes and playoff state), the
// entrants' score order, and the season statistics of every tour golfer: the counts in the save
// profile (PgaStatCounts), each statistic worked out from them (driving distance, greens in
// regulation, scoring average, ...), each statistic's ranking, and the text fe_stats.c prints.

#include "engine.h"
#include "game.h"
#include "game/save.h"
#include "game/modes/pgatoursim.h"
#include "game/modes/pgatour.h"

// A zero-initialised global (gPgaScoreSortPlayer) stays in .sdata, as in the original.
#pragma explicit_zero_data on

PgaEntrantMC* GetEntrantMCPtr(int nPlayer, int nEntrant);
void GM_PgaTourSim_PassEntrant(int nPlayer, int nEntrant);
s32  CalculateCutRow(int nPlayer);
void GM_PgaTourSim_SelectEntrants(PgaEntrantMC* aEntrant, s16* pnEntrants, u8 bUser);
void GM_PgaTourSim_DetermineTargetScores(int nPlayer, int n);
void GM_PgaTourSim_SimEntrantScoresOnHole(int nPlayer, int nRound, int nEntrant, int nHole);
void GM_PgaTourSim_SimAdjustEntrantScores(int nPlayer, int nEntrant, int nRound);
s32  FindFirstCutEntrantIndex(int nPlayer);
s32  TournamentRankIncreasing(const void* pA, const void* pB);
s32  TournamentRankIncreasingForCutEntrants(const void* pA, const void* pB);
s32  StatRankDecreasing(const void* pA, const void* pB);
s32  TotalEntrantHoleScores(int nEntrant);
void GM_PgaTourSim_SimEntrantStatsOnHole(int nPlayer, int nRound, int nEntrant, int nHole);
void CalcScoreRankings(int nPlayer);
void CalcScoreRankingsForCutEntrants(int nPlayer);
void CalcRankingsForStat(int nPlayer, GM_Pga_StatTypes_t nStat);
void CalcSimplePlayerStats(int nPlayer, int nGolfer);
void CalcComplex1PlayerStats(int nGolfer);
void CalcComplex2PlayerStats(int nGolfer);
void CalcAllStatsIfDirty(int nPlayer);
void CalcScoreRankingsIfDirty(int nPlayer);
void CalcAllStats(int nPlayer);
void CalcAllAroundScore(int nGolfer, f32* pfValue);
void CalcTotalDriving(int nGolfer, f32* pfValue);
void CalcBallStriking(int nGolfer, f32* pfValue);
void SplitWinnings(int nPlayer, s32 nTotal, s32 nFirstRow, s32 nCount);
void GM_PgaTourSim_DistributeWinnings(int nPlayer, int nPurse, int nFirstPrize);
void GM_PgaTourSim_CheckEndOfTournamentAward(int nPlayer, u8 bUser, u8 bFirst);
void PlayPGAAwardVideo(int nMovie, int nNumRandom);
void PGATourSimulation_LoadPGSTFromStream(UStreamObject* pObject);

char* GameModeDriverPGATour_GetInitialChampName(s32 i);               // a tournament's first champion
s32  GameModeDriverPGATour_GetInitialChampScore(s32 i);                // and the champion's score
s32  GameModeDriverPGATour_GetCurrentBracket(int nPlayer);             // the player's bracket, 0..9

// The statistic and the player the statistic sort comparisons read, set around each ranking's qsort
// (nStat -1 outside one).
PgaStatSort gPgaStatSort = { -1, 0 };
// The player whose profile the score sort comparisons read, set around the score order's qsorts.
int gPgaScoreSortPlayer = 0;
u8 gbStatsDirty = 1;            // the statistics need working out again (CalcAllStatsIfDirty)
u8 gbScoresDirty = 1;           // the score order needs sorting again (CalcScoreRankingsIfDirty)

PgaPro gPgaPros[PGA_NUM_PROS];                          // the tour pros ('PGST' stream object)
PgaStatRanking gPgaStatRankings[GM_PGA_STAT_COUNT];     // each statistic's values and ranking
PgaEntrant gPgaEntrants[PGA_MAX_ENTRANTS];              // the entrants' holes, not saved
PgaScoreRanking gPgaScoreRanking;                       // the entrants in score order, and places

s32 gPgaUserPlayoffScore;       // the player's strokes on the playoff hole being played
u8  gbPgaUserQuit;              // the player quit the tour round (GM_PgaTourSim_DidUserQuit)

// An entrant's record in the player's save profile (PgaEntrantMC: its golfer, target score, round
// scores and cut), kept between sessions. TW07's also takes the caller's line number.
PgaEntrantMC* GetEntrantMCPtr(int nPlayer, int nEntrant) {
    return &gpSaveData[nPlayer].tour.field.aEntrant[nEntrant];
}

// An entrant's record of the round being played, in memory only (PgaEntrant: current hole, hole
// strokes, playoff).
PgaEntrant* GetEntrantNonMCPtr(int nEntrant) {
    return &gPgaEntrants[nEntrant];
}

// Registers the 'PGST' stream object's loader, which fills the tour pros' table (gPgaPros); the
// hole stream manager calls it with its other stream clients.
void PGATourSimulation_OpenONCE(void) {
    Stream_RegisterLoadChunkCallback('PGST', PGATourSimulation_LoadPGSTFromStream);
}

// Unregisters the 'PGST' loader of PGATourSimulation_OpenONCE.
void PGATourSimulation_CloseONCE(void) {
    Stream_UnregisterLoadChunkCallback('PGST');
}

// The 'PGST' stream object's loader: the tour pros' table (gPgaPros, 174 PgaPro records) read in
// one piece.
void PGATourSimulation_LoadPGSTFromStream(UStreamObject* pObject) {
    Stream_StreamLoadFixedSize(pObject, sizeof(gPgaPros), gPgaPros);
}

// A profile's PGA TOUR started from scratch (a new profile, PasswordManager and FE_MessageTable):
// the whole TourSeason cleared, each of the 31 tournaments' champion and winning score set to the
// tour data's first ones, and each pro's career winnings to its amount in gPgaPros.
void GM_PgaTourSim_ClearAllSeasons(TourSeason* pTour) {
    int i;

    Mem_set(pTour, 0, sizeof(*pTour));
    for (i = 0; i < 31; i++) {
        strcpy(pTour->aEvent[i].szChampName, GameModeDriverPGATour_GetInitialChampName(i));
        pTour->aEvent[i].nChampScore = GameModeDriverPGATour_GetInitialChampScore(i);
    }
    for (i = 0; i < PGA_NUM_PROS; i++) {
        pTour->aStats[i].nCareerWinnings = gPgaPros[i].nCareerWinnings;
    }
}

// A new season (GameModeDriverPGATour_AdvanceSeason): every tour golfer's season counts cleared,
// nEvents through nPlayerOfYearPoints (bytes 0x00-0x4A of PgaStatCounts); the consecutive cuts and
// the career winnings and wins go on.
void GM_PgaTourSim_ClearSeason(TourSeason* pTour) {
    int i;

    for (i = 0; i < PGA_NUM_GOLFERS; i++) {
        Mem_set(&pTour->aStats[i], 0, (u8*)&pTour->aStats[0].unk4B - (u8*)&pTour->aStats[0]);
    }
}

// A round of a tournament for the field. A first round also starts the tournament: the event's par
// and the player's bracket reset, the field emptied (GM_PgaTourSim_ResetTournament), picked
// (GM_PgaTourSim_SelectEntrants) and given its target scores (GM_PgaTourSim_DetermineTargetScores,
// n: the field's low score for the player's bracket, TourEvent.aFieldLowScore). The round's par is
// added to the event's, and every entrant not cut has its 18 holes simulated (the player's only
// with flag 2). uFlags: 1 the player is in the field; 2 the player's round is simulated too; 4 the
// player is about to play the round (GameModeDriverPGATour_PrepareForTeeOff): the holes stay
// uncommitted (no statistics, round scores or cut; that follows the player's round,
// GameModeDriverPGATour_CheckAdvanceTournament), the player's entrant starts on hole 0 and the
// others on random holes. Without 4 the round is finished here: statistics, every entrant on hole
// 18, round scores committed, hole scores reset, and after the second round the cut. TW07 has more
// arguments (field size, cut place).
void GM_PgaTourSim_SimRound(int nPlayer, SeasonEvent* pEvent, int nRound, int n, int uFlags) {
    s32 nEntrants;
    PgaEntrant* pEntrant;
    int nHole;
    int i;

    Mem_set(&gPgaScoreRanking, 0, sizeof(gPgaScoreRanking));
    gbScoresDirty = 1;
    Mem_set(gPgaEntrants, 0, sizeof(gPgaEntrants));
    if (nRound == 0) {
        pEvent->nEventPar = 0;
        pEvent->nUserBracket = GameModeDriverPGATour_GetCurrentBracket(nPlayer);
        GM_PgaTourSim_ResetTournament(nPlayer);
        GM_PgaTourSim_SelectEntrants(gpSaveData[nPlayer].tour.field.aEntrant,
                                     &gpSaveData[nPlayer].tour.field.nEntrants, uFlags & 1);
        GM_PgaTourSim_DetermineTargetScores(nPlayer, n);
    }
    pEvent->nEventPar += (u16)fn_800D2FB4(gSession.nTeeSet[0]);
    nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);
    for (i = 0; i < nEntrants; i++) {
        if (!GM_PgaTourSim_GetWasCutFromEntrantID(nPlayer, i) && (!GM_PgaTourSim_IsEntrantUser(nPlayer, i) || (uFlags & 2))) {
            for (nHole = 0; nHole < 18; nHole++) {
                GM_PgaTourSim_SimEntrantScoresOnHole(nPlayer, nRound, i, nHole);
            }
            GM_PgaTourSim_SimAdjustEntrantScores(nPlayer, i, nRound);
        }
    }
    if (!(uFlags & 4)) {
        GM_PgaTourSim_SimStats(nPlayer);
    }
    if (!(uFlags & 4)) {
        for (i = 0; i < nEntrants; i++) {
            GetEntrantNonMCPtr(i)->nCurrentHole = 18;
        }
        GM_PgaTourSim_CommitRoundScores(nPlayer);
        GM_PgaTourSim_ResetHoleScores(nPlayer);
    }
    if (nRound == 1 && !(uFlags & 4)) {
        GM_PgaTourSim_CutBadGolfers(nPlayer);
    }
    if (uFlags & 4) {
        pEntrant = GetEntrantNonMCPtr(0);
        pEntrant->nCurrentHole = 0;
        for (i = 1; i < nEntrants; i++) {
            pEntrant = GetEntrantNonMCPtr(i);
            pEntrant->nCurrentHole = Misc_RandFunc(0) % 18;
        }
    }
}

// A new tournament: the saved field emptied (no entrants) and no winner yet (nWinner -1).
// GM_PgaTourSim_SimRound calls it for a first round, GameModeDriverPGATour_CheckAdvanceTournament
// when the player quit a first round.
void GM_PgaTourSim_ResetTournament(int nPlayer) {
    Mem_set(&gpSaveData[nPlayer].tour.field, 0, sizeof(PgaField));
    gpSaveData[nPlayer].tour.field.nWinner = -1;
}

// The cut (after the second round: GM_PgaTourSim_SimRound, GameModeDriverPGATour_EndGame): the
// entrants in the score rows above CalculateCutRow's row pass (GM_PgaTourSim_PassEntrant), the rest
// are cut (GM_PgaTourSim_CutEntrant). TW07 passes the cut place; here it is 70.
void GM_PgaTourSim_CutBadGolfers(int nPlayer) {
    s32 i;
    s32 nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);
    s32 nCutRow = CalculateCutRow(nPlayer);

    for (i = 0; i < nCutRow; i++) {
        GM_PgaTourSim_PassEntrant(nPlayer, gPgaScoreRanking.aEntrant[i]);
    }
    for (i = nCutRow; i < nEntrants; i++) {
        GM_PgaTourSim_CutEntrant(nPlayer, gPgaScoreRanking.aEntrant[i]);
    }
}

// An entrant made the cut: its golfer's consecutive cuts go up by one.
void GM_PgaTourSim_PassEntrant(int nPlayer, int nEntrant) {
    PgaEntrantMC* pEntrantMC = GetEntrantMCPtr(nPlayer, nEntrant);
    PgaStatCounts* pStats = &gpSaveData[nPlayer].tour.aStats[pEntrantMC->nGolfer];

    pStats->nConsecutiveCuts++;
}

// An entrant missed the cut: marked cut and its golfer's consecutive cuts back to 0; the score
// order is sorted again. GameModeDriverPGATour_SkipToEvent also cuts the player from a tournament
// it abandons.
void GM_PgaTourSim_CutEntrant(int nPlayer, int nEntrant) {
    PgaEntrantMC* pEntrantMC = GetEntrantMCPtr(nPlayer, nEntrant);
    PgaStatCounts* pStats = &gpSaveData[nPlayer].tour.aStats[pEntrantMC->nGolfer];

    pEntrantMC->bWasCut = 1;
    pStats->nConsecutiveCuts = 0;
    gbScoresDirty = 1;
}

// The entrant's strokes on the holes before its current hole become its saved score for the current
// round (tour.nRound).
void CommitEntrantRoundScore(int nPlayer, int nEntrant) {
    PgaEntrantMC* pEntrantMC = GetEntrantMCPtr(nPlayer, nEntrant);
    PgaEntrant* pEntrant = GetEntrantNonMCPtr(nEntrant);
    s32 i;

    pEntrantMC->aRoundStrokes[gpSaveData[nPlayer].tour.nRound] = 0;
    // The (s16) is in the original (an extsh before each add).
    for (i = 0; i < pEntrant->nCurrentHole; i++) {
        pEntrantMC->aRoundStrokes[gpSaveData[nPlayer].tour.nRound] += (s16)pEntrant->aHoleStrokes[i];
    }
    gbScoresDirty = 1;
}

// Every entrant's holes saved as its score for the current round (CommitEntrantRoundScore).
void GM_PgaTourSim_CommitRoundScores(int nPlayer) {
    s32 nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);
    s32 i;

    for (i = 0; i < nEntrants; i++) {
        CommitEntrantRoundScore(nPlayer, i);
    }
}

// Whether the player quit the tour round being played (GM_PgaTourSim_SetUserQuit);
// GameModeDriverPGATour_CheckAdvanceTournament then throws the round away. TW07 takes the player;
// this build keeps one flag and takes nothing (FE_PGATourMessages still passes 0).
u8 GM_PgaTourSim_DidUserQuit(void) {
    return gbPgaUserQuit;
}

// Sets the player-quit flag (GM_PgaTourSim_DidUserQuit): 1 when the player quits the round
// (GM_vExitGame), 0 at tee off (GameModeDriverPGATour_PrepareForTeeOff). nPlayer is not read.
void GM_PgaTourSim_SetUserQuit(int nPlayer, u8 b) {
    gbPgaUserQuit = b;
}

// Every entrant back to the first tee with no strokes on any hole, for the next round.
void GM_PgaTourSim_ResetHoleScores(int nPlayer) {
    s32 nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);
    s32 i;
    PgaEntrant* pEntrant;
    int nHole;

    for (i = 0; i < nEntrants; i++) {
        pEntrant = GetEntrantNonMCPtr(i);
        pEntrant->nCurrentHole = 0;
        for (nHole = 0; nHole < 18; nHole++) {
            pEntrant->aHoleStrokes[nHole] = 0;
        }
    }
    gbScoresDirty = 1;
}

// The tournament is over: the winner is entrant 0 (the player's slot) if it is in the playoff
// (GM_PgaTourSim_InitPlayoff), else a random one of the playoff entrants. The winner's golfer gets
// a season win, a career win and a Player of the Year point (3 more for a major), the purse for the
// player's bracket is paid out (GM_PgaTourSim_DistributeWinnings), and the player's awards follow
// (GM_PgaTourSim_CheckEndOfTournamentAward: bUser, entrant 0 is the player; bFirst, entrant 0 was
// placed first before the winner was set, ties included).
void GM_PgaTourSim_SimTournamentWinner(int nPlayer) {
    s32 nWinner;
    s32 nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);
    u8 bUser;
    u8 bFirst;
    s32 nPlayoff;
    s32 nPick;
    s32 nRand;
    s32 i;
    PgaEntrantMC* pWinner;
    s32 nPurse;
    s32 nFirstPrize;

    CalcScoreRankingsIfDirty(nPlayer);
    bUser = GM_PgaTourSim_IsEntrantUser(nPlayer, 0);
    bFirst = gPgaScoreRanking.aRank[0] == 1;
    nPlayoff = GM_PgaTourSim_GetNumPlayoffEntrants(nPlayer);
    if (GM_PgaTourSim_EntrantIsInPlayoff(nPlayer, 0)) {
        nWinner = 0;
    } else {
        // fake match: the pick goes through its own local (orig computes it in r0, then copies it)
        nRand = Misc_RandFunc(0) % nPlayoff;
        nPick = nRand;
        for (i = 0; i < nEntrants; i++) {
            if (GM_PgaTourSim_EntrantIsInPlayoff(nPlayer, i)) {
                nWinner = i;
                if (nPick-- == 0) {
                    break;
                }
            }
        }
    }
    pWinner = GetEntrantMCPtr(nPlayer, nWinner);
    gpSaveData[nPlayer].tour.field.nWinner = nWinner;
    gbScoresDirty = 1;
    gpSaveData[nPlayer].tour.aStats[pWinner->nGolfer].nSeasonWins++;
    gpSaveData[nPlayer].tour.aStats[pWinner->nGolfer].nCareerWins++;
    gpSaveData[nPlayer].tour.aStats[pWinner->nGolfer].nPlayerOfYearPoints++;
    if (GameModeDriverPGATour_GetEventInfo(gpSaveData[nPlayer].tour.nEvent)->bIsAMajor != 0) {
        gpSaveData[nPlayer].tour.aStats[pWinner->nGolfer].nPlayerOfYearPoints += 3;
    }
    nPurse = GameModeDriverPGATour_ComputePurseForBracket(gpSaveData[nPlayer].tour.nEvent,
                                                          GameModeDriverPGATour_GetCurrentBracket(nPlayer));
    nFirstPrize = GameModeDriverPGATour_ComputeFirstPrizeForBracket(
        gpSaveData[nPlayer].tour.nEvent, GameModeDriverPGATour_GetCurrentBracket(nPlayer));
    GM_PgaTourSim_DistributeWinnings(nPlayer, nPurse, nFirstPrize);
    GM_PgaTourSim_CheckEndOfTournamentAward(nPlayer, bUser, bFirst);
}

// The player's awards after a tournament. bUser: the player played it (TW07 bUserEntered); bFirst:
// entrant 0 was placed first (TW07 bUserWon). A major placed first adds to the profile's
// nMajorWins; the win streak goes up with a win and back to 0 when the player plays and does not
// win. Leading every pro's career winnings, being in the top 5 or in the top 25 wins the a200
// career-money awards (all three, the last two, the last one), the highest one's movie playing the
// first time it is won. After the season's last tournament: a1C0[12] two or more wins in the first
// season, [13] leading the Player of the Year points, [14] leading the season's winnings, [15]
// leading the scoring average with 15 tournaments or more. When a month ends (the next tournament
// ends in another month, or the season is over), no pro ahead of the player's winnings for the
// month (n44) wins that month's award a1C0[month - 1], and every golfer's month winnings start
// again.
void GM_PgaTourSim_CheckEndOfTournamentAward(int nPlayer, u8 bUser, u8 bFirst) {
    int i;
    s32 nAhead;
    s32 nNext;
    TourSeason* pTour = &gpSaveData[nPlayer].tour;
    PgaStatCounts* pStats = &gpSaveData[nPlayer].tour.aStats[PGA_USER_GOLFER];

    // EA bug: the test does not check bUser. A tournament the player skips
    // (GameModeDriverPGATour_SkipToEvent) is simulated without the player, entrant 0 is then a
    // random pro, and a major that pro wins still adds to the player's nMajorWins (award 36).
    if (bFirst && GameModeDriverPGATour_GetEventInfo(gpSaveData[nPlayer].tour.nEvent)->bIsAMajor != 0) {
        pTour->nMajorWins++;
    }
    if (bUser && bFirst) {
        pTour->nWinStreak++;
    } else if (bUser && !bFirst) {
        pTour->nWinStreak = 0;
    }

    nAhead = 0;
    for (i = 0; i < PGA_NUM_PROS; i++) {
        if (gpSaveData[nPlayer].tour.aStats[i].nCareerWinnings > pStats->nCareerWinnings) {
            nAhead++;
        }
    }
    if (nAhead == 0) {
        if (GM_Earnings_GiveAwardToUser(nPlayer, &gpSaveData[nPlayer].a200[0])) {
            PlayPGAAwardVideo(1, 1);
        }
        GM_Earnings_GiveAwardToUser(nPlayer, &gpSaveData[nPlayer].a200[1]);
        GM_Earnings_GiveAwardToUser(nPlayer, &gpSaveData[nPlayer].a200[2]);
    } else if (nAhead <= 4) {
        if (GM_Earnings_GiveAwardToUser(nPlayer, &gpSaveData[nPlayer].a200[1])) {
            PlayPGAAwardVideo(3, 1);
        }
        GM_Earnings_GiveAwardToUser(nPlayer, &gpSaveData[nPlayer].a200[2]);
    } else if (nAhead <= 24) {
        if (GM_Earnings_GiveAwardToUser(nPlayer, &gpSaveData[nPlayer].a200[2])) {
            PlayPGAAwardVideo(4, 1);
        }
    }

    nNext = GameModeDriverPGATour_GetNextEvent();
    if (nNext == -1) {
        if (gpSaveData[nPlayer].tour.nSeason == 0
            && pStats->nSeasonWins > 1
            && GM_Earnings_GiveAwardToUser(nPlayer, &gpSaveData[nPlayer].a1C0[12])) {
            PlayPGAAwardVideo(12, 2);
        }
        if (GM_PgaTourSim_IsLeaderForStat(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_PLAYER_OF_YEAR_POINTS)
            && GM_Earnings_GiveAwardToUser(nPlayer, &gpSaveData[nPlayer].a1C0[13])) {
            PlayPGAAwardVideo(11, 1);
        }
        if (GM_PgaTourSim_IsLeaderForStat(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_SEASON_WINNINGS)
            && GM_Earnings_GiveAwardToUser(nPlayer, &gpSaveData[nPlayer].a1C0[14])) {
            if ((Misc_RandFunc(0) & 1) == 0) {
                PlayPGAAwardVideo(5, 1);
            } else {
                PlayPGAAwardVideo(2, 1);
            }
        }
        if (pStats->nEvents >= 15
            && GM_PgaTourSim_IsLeaderForStat(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_SCORING)
            && GM_Earnings_GiveAwardToUser(nPlayer, &gpSaveData[nPlayer].a1C0[15])) {
            PlayPGAAwardVideo(6, 2);
        }
    }

    if (nNext == -1
        || CalDate_GetMonth(GameModeDriverPGATour_GetEndDate(nNext))
               != CalDate_GetMonth(GameModeDriverPGATour_GetEndDate(gpSaveData[nPlayer].tour.nEvent))) {
        for (i = 0; i < PGA_NUM_PROS; i++) {
            if (pStats->n44 < gpSaveData[nPlayer].tour.aStats[i].n44) {
                break;
            }
        }
        if (i == PGA_NUM_PROS
            && GM_Earnings_GiveAwardToUser(nPlayer,
                                           &gpSaveData[nPlayer].a1C0[CalDate_GetMonth(
                                                   GameModeDriverPGATour_GetEndDate(
                                        gpSaveData[nPlayer].tour.nEvent)) - 1])) {
            PlayPGAAwardVideo(14, 6);
        }
        for (i = 0; i < PGA_NUM_GOLFERS; i++) {
            gpSaveData[nPlayer].tour.aStats[i].n44 = 0;
        }
    }
}

// The size of the tournament's field in the player's profile (100 to 127 once picked,
// GM_PgaTourSim_SelectEntrants).
s32 GM_PgaTourSim_GetNumEntrants(int nPlayer) {
    return gpSaveData[nPlayer].tour.field.nEntrants;
}

// The number of entrants in first place: the leading score rows placed 1 (cut entrants skipped). It
// reads the score order as it is, without sorting it again.
s32 GM_PgaTourSim_GetNumFirstPlaceEntrants(int nPlayer) {
    s32 nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);
    s32 i;
    s32 nEntrant;
    s32 nCount = 0;

    for (i = 0; i < nEntrants; i++) {
        nEntrant = gPgaScoreRanking.aEntrant[i];
        if (!GM_PgaTourSim_GetWasCutFromEntrantID(nPlayer, nEntrant)) {
            if (gPgaScoreRanking.aRank[nEntrant] != 1) {
                break;
            }
            nCount++;
        }
    }
    return nCount;
}

// A statistic's value as text, with the statistic's number of decimal places.
void GM_PgaTourSim_GetStatValString(GM_Pga_StatTypes_t nStat, f32 fValue, char* szOut) {
    char szFormat[8];

    switch (gPgaStatDecimals[nStat]) {
    case 0:
        sprintf(szOut, "%.0f", fValue);
        break;
    case 1:
        sprintf(szOut, "%.1f", fValue);
        break;
    case 2:
        sprintf(szOut, "%.2f", fValue);
        break;
    case 3:
        sprintf(szOut, "%.3f", fValue);
        break;
    default:
        sprintf(szFormat, "%%.%df", gPgaStatDecimals[nStat]);
        sprintf(szOut, szFormat, fValue);
        break;
    }
}

// Picks a tournament's field: 100 to 127 of the pros in random order (a shuffle of all of them),
// the first replaced by the player's golfer when bUser is set; the rest of the table empty (golfer
// -1). TW07 passes the field's size limits.
void GM_PgaTourSim_SelectEntrants(PgaEntrantMC* aEntrant, s16* pnEntrants, u8 bUser) {
    s32 aGolfer[PGA_NUM_PROS];
    int i;
    s32 j;
    s32 nSwap;

    for (i = 0; i < PGA_NUM_PROS; i++) {
        aGolfer[i] = i;
    }
    for (i = 0; i < PGA_NUM_PROS - 1; i++) {
        j = i + Misc_RandFunc(0) % (PGA_NUM_PROS - i);
        if (j != i) {
            nSwap = aGolfer[j];
            aGolfer[j] = aGolfer[i];
            aGolfer[i] = nSwap;
        }
    }
    *pnEntrants = Misc_RandFunc(0) % 28 + 100;
    if (bUser) {
        aGolfer[0] = PGA_USER_GOLFER;
    }
    for (i = 0; i < *pnEntrants; i++) {
        aEntrant[i].nGolfer = aGolfer[i];
    }
    for (i = *pnEntrants; i < PGA_MAX_ENTRANTS; i++) {
        aEntrant[i].nGolfer = -1;
    }
}

// A sort comparison for s32s, smallest first.
s32 IntCompareIncreasing(const void* pA, const void* pB) {
    return *(const s32*)pA - *(const s32*)pB;
}

// A sort comparison for entrant ids (GM_PgaTourSim_DetermineTargetScores): by their pro's
// historical score rank (PgaPro f50), lowest first; the player's golfer counts as 10000, last. It
// reads profile 0's field.
s32 HistoricalScoreRankCompareIncreasing(const void* pA, const void* pB) {
    s32 nEntrantB = *(const s32*)pB;
    s32 nGolfer;
    f32 fA;
    f32 fB;

    nGolfer = GM_PgaTourSim_GetGolferIDFromEntrantID(0, *(const s32*)pA);
    if (nGolfer == PGA_USER_GOLFER) {
        fA = 10000.0f;
    } else {
        fA = gPgaPros[nGolfer].f50;
    }
    nGolfer = GM_PgaTourSim_GetGolferIDFromEntrantID(0, nEntrantB);
    if (nGolfer == PGA_USER_GOLFER) {
        fB = 10000.0f;
    } else {
        fB = gPgaPros[nGolfer].f50;
    }
    if (fA < fB) {
        return -1;
    }
    return fA > fB;
}

// Gives each entrant the four-round total the simulation aims at. The entrants are ordered by their
// pro's historical rank (HistoricalScoreRankCompareIncreasing) and shuffled a little (each row may
// swap with one up to a few rows down; the player, sorted last, is never moved up). The targets are
// par for four rounds plus n (the field's low score for the player's bracket) plus 18 plus 8 x a
// normal random number, kept within par + n to par + n + 25, sorted, the best at most par + n + 3,
// and handed out in the entrants' order. The player's entrant also gets the best target.
void GM_PgaTourSim_DetermineTargetScores(int nPlayer, int n) {
    s32 aOrder[PGA_MAX_ENTRANTS];
    s32 aTarget[PGA_MAX_ENTRANTS];
    PgaEntrantMC* pEntrantMC;
    s32 nEntrants;
    s32 nRoundPar;
    s32 nPar;
    s32 nTarget;
    s32 nEntrant;
    s32 i;
    s32 j;

    nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);
    for (i = 0; i < nEntrants; i++) {
        aOrder[i] = i;
    }
    for (i = nEntrants; i < PGA_MAX_ENTRANTS; i++) {
        aOrder[i] = -1;
    }
    qsort(aOrder, nEntrants, sizeof(aOrder[0]), HistoricalScoreRankCompareIncreasing);
    for (i = 0; i < nEntrants - 1; i++) {
        j = i + (s32)fabsf(3.0f * Misc_RandFuncg(0));
        j = (j <= nEntrants - 1) ? j : nEntrants - 1;
        if (j != i && !GM_PgaTourSim_IsEntrantUser(nPlayer, aOrder[j])) {
            nEntrant = aOrder[j];
            aOrder[j] = aOrder[i];
            aOrder[i] = nEntrant;
        }
    }
    nRoundPar = fn_800D2FB4(gSession.nTeeSet[0]) * 4;
    nPar = nRoundPar + n;
    for (i = 0; i < nEntrants; i++) {
        nTarget = 8.0f * Misc_RandFuncg(0) + (18.0f + nPar);
        nTarget = (nTarget <= nPar) ? nPar : nTarget;
        aTarget[i] = (nTarget <= nPar + 25) ? nTarget : nPar + 25;
    }
    qsort(aTarget, nEntrants, sizeof(aTarget[0]), IntCompareIncreasing);
    if (aTarget[0] > nPar + 3) {
        aTarget[0] = nPar + 3;
    }
    for (i = 0; i < nEntrants; i++) {
        pEntrantMC = GetEntrantMCPtr(nPlayer, aOrder[i]);
        pEntrantMC->nTargetScore = aTarget[i];
    }
    if (GM_PgaTourSim_IsEntrantUser(nPlayer, 0)) {
        pEntrantMC = GetEntrantMCPtr(nPlayer, 0);
        pEntrantMC->nTargetScore = aTarget[0];
    }
}

// A tour golfer's name: a pro's, or for the player's golfer the profile's name.
char* GM_PgaTourSim_GetNameFromGolferID(int nPlayer, int nGolfer) {
    if (nGolfer == PGA_USER_GOLFER) {
        return gpSaveData[nPlayer].szName;
    }
    return gPgaPros[nGolfer].szName;
}

// The golfer's place in a statistic's ranking (1 = best; golfers whose values print the same share
// it), the statistics worked out again first if they changed.
s32 GM_PgaTourSim_GetStatRankFromGolferID(int nPlayer, int nGolfer, GM_Pga_StatTypes_t nStat) {
    CalcAllStatsIfDirty(nPlayer);
    return gPgaStatRankings[nStat].aRank[nGolfer];
}

// The golfer's value of a statistic, the statistics worked out again first if they changed.
f32 GM_PgaTourSim_GetStatValueFromGolferID(int nPlayer, int nGolfer, GM_Pga_StatTypes_t nStat) {
    CalcAllStatsIfDirty(nPlayer);
    return gPgaStatRankings[nStat].aValue[nGolfer].fValue;
}

// The golfer in row nRow of a statistic's ranking (row 0 leads), the statistics worked out again
// first if they changed.
s32 GM_PgaTourSim_GetGolferIDFromStatRow(int nPlayer, GM_Pga_StatTypes_t nStat, int nRow) {
    CalcAllStatsIfDirty(nPlayer);
    return gPgaStatRankings[nStat].aGolfer[nRow];
}

// Whether no other tour golfer beats the golfer's value of a statistic (ties allowed): higher is
// better where the statistic's ranking sorts highest first (gPgaStatCompares), lower otherwise. The
// statistics are worked out again first if they changed.
u8 GM_PgaTourSim_IsLeaderForStat(int nPlayer, int nGolfer, GM_Pga_StatTypes_t nStat) {
    f32 fValue;
    s32 i;

    CalcAllStatsIfDirty(nPlayer);
    fValue = gPgaStatRankings[nStat].aValue[nGolfer].fValue;
    for (i = 0; i < PGA_NUM_GOLFERS; i++) {
        if (gPgaStatCompares[nStat] == StatRankDecreasing) {
            if (i != nGolfer && gPgaStatRankings[nStat].aValue[i].fValue > fValue) {
                return 0;
            }
        } else {
            if (i != nGolfer && gPgaStatRankings[nStat].aValue[i].fValue < fValue) {
                return 0;
            }
        }
    }
    return 1;
}

// The statistic's view (gPgaStatViews): the statistics screen shows each golfer's tournaments
// played for 0 and rounds played for 1.
s32 GM_PgaTourSim_GetStatView(GM_Pga_StatTypes_t nStat) {
    return gPgaStatViews[nStat];
}

// The golfer's tournaments started this season.
s32 GM_PgaTourSim_GetNEventsFromGolferID(int nPlayer, int nGolfer) {
    return gpSaveData[nPlayer].tour.aStats[nGolfer].nEvents;
}

// The golfer's rounds played this season.
s32 GM_PgaTourSim_GetNRoundsFromGolferID(int nPlayer, int nGolfer) {
    return gpSaveData[nPlayer].tour.aStats[nGolfer].nRounds;
}

// Entrant 0 is the player's slot; it is the player when it holds the player's golfer.
u8 GM_PgaTourSim_IsEntrantUser(int nPlayer, int nEntrant) {
    PgaEntrantMC* pEntrant = GetEntrantMCPtr(nPlayer, nEntrant);
    int bUser = 0;

    if (nEntrant == 0 && pEntrant->nGolfer == PGA_USER_GOLFER) {
        bUser = 1;
    }
    return bUser;
}

// The entrant's place in the tournament (1 = first; entrants with the same score share it), the
// score order sorted again first if the scores changed.
s32 GM_PgaTourSim_GetScoreRankFromEntrantID(int nPlayer, int nEntrant) {
    CalcScoreRankingsIfDirty(nPlayer);
    return gPgaScoreRanking.aRank[nEntrant];
}

// The entrant's golfer id (PGA_USER_GOLFER: the player).
s32 GM_PgaTourSim_GetGolferIDFromEntrantID(int nPlayer, int nEntrant) {
    return GetEntrantMCPtr(nPlayer, nEntrant)->nGolfer;
}

// How many of the entrant's holes this round count in its score (b is TW07's includeCurrHole): its
// current hole number, one more for the player with b (the hole being played), one fewer for
// another entrant without b; at most 18.
s32 GetLastHoleToScore(int nPlayer, int nEntrant, u8 b) {
    s32 nHole;
    s32 nRet;

    nHole = GetEntrantNonMCPtr(nEntrant)->nCurrentHole;
    if (GM_PgaTourSim_IsEntrantUser(nPlayer, nEntrant)) {
        if (b) {
            nHole++;
        }
    } else if (!b) {
        nHole--;
    }
    nRet = 18;
    if (nHole <= 18) {
        nRet = nHole;
    }
    return nRet;
}

// The entrant's strokes in the tournament so far: every round played plus the counted holes of the
// current one (GetLastHoleToScore; b is TW07's includeCurrHole). In game type 3 before the first
// round, all four rounds' saved scores.
s32 GM_PgaTourSim_GetTotalScoreFromEntrantID(int nPlayer, int nEntrant, u8 b) {
    PgaEntrant* pEntrant;
    s32 nStrokes;
    s32 nHoles;
    s32 i;

    GetEntrantMCPtr(nPlayer, nEntrant);
    pEntrant = GetEntrantNonMCPtr(nEntrant);
    nStrokes = 0;
    if (gSession.nGameType == 3 && gpSaveData[nPlayer].tour.nRound == 0) {
        for (i = 0; i < 4; i++) {
            nStrokes += GM_PgaTourSim_GetRoundScoreFromEntrantID(nPlayer, nEntrant, i);
        }
    } else {
        for (i = 0; i < gpSaveData[nPlayer].tour.nRound; i++) {
            nStrokes += GM_PgaTourSim_GetRoundScoreFromEntrantID(nPlayer, nEntrant, i);
        }
        if (gpSaveData[nPlayer].tour.nRound < 4) {
            nHoles = GetLastHoleToScore(nPlayer, nEntrant, b);
            for (i = 0; i < nHoles; i++) {
                nStrokes += pEntrant->aHoleStrokes[i];
            }
        }
    }
    return nStrokes;
}

// The entrant's score to par so far: GM_PgaTourSim_GetTotalScoreFromEntrantID's strokes less the
// par of each round played (the course of that round from the tournament's course list; in game
// type 3 before the first round, all four) and of the counted holes of the current round (b as
// there). A built round (course 22) and courses 24 to 29 take each hole's course and hole number
// from the round's hole list. The tournament is the current one, or the season's last once the
// season is over (nEvent -1).
int GM_PgaTourSim_GetRelativeScoreFromEntrantID(int nPlayer, int nEntrant, u8 b) {
    s32 aCourses[4];
    s32 nEvent;
    s32 nScore;
    s32 nHoles;
    s32 nCourse;
    s32 nPar;
    s32 i;

    nEvent = gpSaveData[nPlayer].tour.nEvent;
    if (nEvent == -1) {
        nEvent = GameModeDriverPGATour_GetFinalEventOfSeason();
    }
    GameModeDriverPGATour_GetCourses(GameModeDriverPGATour_GetEventInfo(nEvent), aCourses);
    GetEntrantNonMCPtr(nEntrant);
    nScore = GM_PgaTourSim_GetTotalScoreFromEntrantID(nPlayer, nEntrant, b);
    if (gSession.nGameType == 3 && gpSaveData[nPlayer].tour.nRound == 0) {
        for (i = 0; i < 4; i++) {
            nScore -= fn_800D2F00(aCourses[i], 0);
        }
    } else {
        for (i = 0; i < gpSaveData[nPlayer].tour.nRound; i++) {
            nScore -= fn_800D2F00(aCourses[i], 0);
        }
        if (gpSaveData[nPlayer].tour.nRound < 4) {
            nHoles = GetLastHoleToScore(nPlayer, nEntrant, b);
            for (i = 0; i < nHoles; i++) {
                if (aCourses[gpSaveData[nPlayer].tour.nRound] == 22) {
                    nCourse = fn_800D3118(22, i);
                    nPar = fn_800D2ABC(nCourse, fn_800D315C(22, i) - 1);
                } else if (aCourses[gpSaveData[nPlayer].tour.nRound] >= 24
                           && aCourses[gpSaveData[nPlayer].tour.nRound] < 30) {
                    nCourse = fn_800D3118(aCourses[gpSaveData[nPlayer].tour.nRound], i);
                    nPar = fn_800D2ABC(nCourse,
                                       fn_800D315C(aCourses[gpSaveData[nPlayer].tour.nRound], i) - 1);
                } else {
                    nPar = fn_800D2ABC(aCourses[gpSaveData[nPlayer].tour.nRound], i);
                }
                nScore -= nPar;
            }
        }
    }
    return nScore;
}

// The best score to par among the entrants still in the tournament other than the player
// (0x7FFFFFFF if none; b as for GM_PgaTourSim_GetRelativeScoreFromEntrantID), for
// GameModeDriverPGATour's lead and putt-for-the-lead checks.
s32 GM_PgaTourSim_GetBestOpponentRelativeScore(int nPlayer, u8 b) {
    s32 nBest = 0x7FFFFFFF;
    s32 nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);
    s32 i;

    for (i = 0; i < nEntrants; i++) {
        if (!GM_PgaTourSim_IsEntrantUser(nPlayer, i) && !GM_PgaTourSim_GetWasCutFromEntrantID(nPlayer, i)) {
            nBest = (nBest <= GM_PgaTourSim_GetRelativeScoreFromEntrantID(nPlayer, i, b)) ? nBest : GM_PgaTourSim_GetRelativeScoreFromEntrantID(nPlayer, i, b);
        }
    }
    return nBest;
}

// The entrant's strokes in a round: for the round being played (outside game type 3) the holes
// so far, otherwise the round's saved total.
s32 GM_PgaTourSim_GetRoundScoreFromEntrantID(int nPlayer, int nEntrant, int nRound) {
    PgaEntrantMC* pEntrantMC = GetEntrantMCPtr(nPlayer, nEntrant);
    PgaEntrant* pEntrant = GetEntrantNonMCPtr(nEntrant);
    s32 nStrokes;
    s32 i;

    if (nRound == gpSaveData[nPlayer].tour.nRound && gSession.nGameType != 3) {
        nStrokes = 0;
        // The (s16) is in the original (an extsh before each add), as in CommitEntrantRoundScore.
        for (i = 0; i < pEntrant->nCurrentHole; i++) {
            nStrokes += (s16)pEntrant->aHoleStrokes[i];
        }
    } else {
        nStrokes = pEntrantMC->aRoundStrokes[nRound];
    }
    return nStrokes;
}

u8 GM_PgaTourSim_GetWasCutFromEntrantID(int nPlayer, int nEntrant) {
    return GetEntrantMCPtr(nPlayer, nEntrant)->bWasCut;
}

// The entrant in row nRow of the score order (row 0 leads), the order sorted again first if the
// scores changed.
s32 GM_PgaTourSim_GetEntrantIDFromScoreRow(int nPlayer, int nRow) {
    CalcScoreRankingsIfDirty(nPlayer);
    return gPgaScoreRanking.aEntrant[nRow];
}

// Whether another entrant (cut ones included) shares the entrant's place, for the leaderboards' tie
// mark.
u8 GM_PgaTourSim_IsEntrantTied(int nPlayer, int nEntrant) {
    s32 nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);
    s32 nRank = GM_PgaTourSim_GetScoreRankFromEntrantID(nPlayer, nEntrant);
    s32 i;

    for (i = 0; i < nEntrants; i++) {
        if (i != nEntrant && gPgaScoreRanking.aRank[i] == nRank) {
            return 1;
        }
    }
    return 0;
}

// The player's strokes so far on the hole being played (GameManager, as strokes are added): in a
// playoff they go to gPgaUserPlayoffScore, otherwise to entrant 0's current hole.
void GM_PgaTourSim_SetUserEntrantHoleStrokes(int nPlayer, int nStrokes) {
    PgaEntrant* pUser = GetEntrantNonMCPtr(0);

    if (pUser->bInPlayoff) {
        gPgaUserPlayoffScore = nStrokes;
    } else {
        pUser->aHoleStrokes[pUser->nCurrentHole] = nStrokes;
        gbScoresDirty = 1;
    }
}

// Moves the player's entrant to hole nHole (GameModeDriverPGATour_EndHole: the next one after a
// hole ends); the score order is sorted again.
void GM_PgaTourSim_AdvancePlayer(int nPlayer, int nHole) {
    GetEntrantNonMCPtr(0)->nCurrentHole = nHole;
    gbScoresDirty = 1;
}

// Before each of the player's holes (GameModeDriverPGATour_PostHoleLoadInit) every other entrant
// moves on 0 to 2 holes at random, kept at least one hole ahead of the player and at most 18 (the
// round finished).
void GM_PgaTourSim_AdvanceField(int nPlayer) {
    s32 nEntrants;
    PgaEntrant* pEntrant;
    s32 nMinHole;
    s32 i;

    nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);
    pEntrant = GetEntrantNonMCPtr(0);
    GetEntrantMCPtr(nPlayer, 0);
    nMinHole = pEntrant->nCurrentHole;
    nMinHole++;                 // fake match: one statement, "+ 1", swaps the saved registers
    for (i = 1; i < nEntrants; i++) {
        pEntrant = GetEntrantNonMCPtr(i);
        pEntrant->nCurrentHole += Misc_RandFunc(0) % 3;
        pEntrant->nCurrentHole = pEntrant->nCurrentHole <= nMinHole ? nMinHole : pEntrant->nCurrentHole;
        pEntrant->nCurrentHole = 18 < pEntrant->nCurrentHole ? 18 : pEntrant->nCurrentHole;
    }
    gbScoresDirty = 1;
}

// The entrant's current hole in the round being played: the holes it has finished, 0 to 18.
s32 GM_PgaTourSim_GetCurrentHoleFromEntrantID(int nPlayer, int nEntrant) {
    return GetEntrantNonMCPtr(nEntrant)->nCurrentHole;
}

// The entrant's prize money from the tournament (PgaEntrantMC n18, set when the purse is split,
// SplitWinnings).
s32 GM_PgaTourSim_GetLeaderboardWinningsFromEntrantID(int nPlayer, int nEntrant) {
    return GetEntrantMCPtr(nPlayer, nEntrant)->n18;
}

// The cut row: the first score row placed below 70th (the top 70 and ties play on); if there is
// none, the last row (so the last entrant is cut all the same). TW07 passes the cut place.
s32 CalculateCutRow(int nPlayer) {
    s32 nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);
    s32 nRow = nEntrants - 1;
    s32 i;

    CalcScoreRankingsIfDirty(nPlayer);
    for (i = 0; i < nEntrants; i++) {
        if (gPgaScoreRanking.aRank[gPgaScoreRanking.aEntrant[i]] > 70) {
            nRow = i;
            break;
        }
    }
    return nRow;
}

// The first score row holding an entrant who was cut, or -1 (the score order as it is).
s32 FindFirstCutEntrantIndex(int nPlayer) {
    s32 nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);
    s32 nRow = -1;
    s32 i;

    for (i = 0; i < nEntrants; i++) {
        if (GM_PgaTourSim_GetWasCutFromEntrantID(nPlayer, gPgaScoreRanking.aEntrant[i])) {
            nRow = i;
            break;
        }
    }
    return nRow;
}

// fake match: stands in for a function the original linker stripped. The file's pool has 0.0
// right after the int-to-float constant, before GM_PgaTourSim_SimEntrantScoresOnHole's 0.25; its body is unknown, this
// one only reproduces the order.
static f32 PGATourSimulation_StrippedFn(f32 x) {
    if (x > 0.0f) return x;
    return 0.0f;
}

// Simulates an entrant's strokes on a hole: the pro's scoring average for the hole's par (the
// player's entrant uses the first pro's), scaled so the round's holes add up to a quarter of the
// entrant's target score, plus a normal random spread that grows with the par; 1 to 10 strokes.
void GM_PgaTourSim_SimEntrantScoresOnHole(int nPlayer, int nRound, int nEntrant, int nHole) {
    PgaEntrantMC* pEntrantMC = GetEntrantMCPtr(nPlayer, nEntrant);
    PgaEntrant* pEntrant = GetEntrantNonMCPtr(nEntrant);
    PgaPro* pPro;
    s32 nPar;
    f32 fScale;
    f32 fRound;                 // the pro's average round on this course
    f32 fPar3;
    f32 fPar4;
    f32 fPar5;
    f32 fStrokes;
    s32 nStrokes;

    if (GM_PgaTourSim_IsEntrantUser(nPlayer, nEntrant)) {
        pPro = &gPgaPros[0];
    } else {
        pPro = &gPgaPros[pEntrantMC->nGolfer];
    }
    nPar = Course_GetHolePar(nHole);
    fRound = pPro->fPar3Avg * fn_800D31A4(3) + pPro->fPar4Avg * fn_800D31A4(4)
           + pPro->fPar5Avg * fn_800D31A4(5);
    fScale = 0.25f * pEntrantMC->nTargetScore / fRound;
    fPar3 = fScale * pPro->fPar3Avg;
    fPar4 = fScale * pPro->fPar4Avg;
    fPar5 = fScale * pPro->fPar5Avg;
    switch (nPar) {
    case 3:
        fStrokes = 0.5f * Misc_RandFuncg(0) + fPar3;
        break;
    case 4:
        fStrokes = 0.6f * Misc_RandFuncg(0) + fPar4;
        break;
    case 5:
        fStrokes = 0.7f * Misc_RandFuncg(0) + fPar5;
        break;
    }
    // EA bug: fStrokes is never set on a hole whose par is not 3, 4 or 5.
    nStrokes = (s32)(0.5f + fStrokes);
    nStrokes = nStrokes > 1 ? nStrokes : 1;
    pEntrant->aHoleStrokes[nHole] = nStrokes > 10 ? 10 : nStrokes;
}

// The entrant's strokes on all 18 holes of the round (the running total
// GM_PgaTourSim_SimAdjustEntrantScores brings to its target).
s32 TotalEntrantHoleScores(int nEntrant) {
    PgaEntrant* pEntrant = GetEntrantNonMCPtr(nEntrant);
    s32 nTotal = 0;
    int i;

    for (i = 0; i < 18; i++) {
        nTotal += pEntrant->aHoleStrokes[i];
    }
    return nTotal;
}

// Brings an entrant's simulated round to its target: in the first three rounds a quarter of the
// entrant's target score plus a normal random number, kept within 3 strokes of it and rounded; in
// the last, what is left of the target after the strokes so far. Strokes come off random holes (a 2
// only very rarely becomes a 1) or go on them (a hole already at 6 or more only rarely, never past
// 10) until the round adds up. The statistics and score order are marked for working out again.
void GM_PgaTourSim_SimAdjustEntrantScores(int nPlayer, int nEntrant, int nRound) {
    PgaEntrantMC* pEntrantMC = GetEntrantMCPtr(nPlayer, nEntrant);
    PgaEntrant* pEntrant = GetEntrantNonMCPtr(nEntrant);
    s32 nTarget;
    f32 fTarget;
    f32 fScore;

    gbStatsDirty = 1;
    gbScoresDirty = 1;
    if (nRound < 3) {
        fTarget = 0.25f * pEntrantMC->nTargetScore;
        fScore = fTarget + Misc_RandFuncg(0);
        fScore = fScore <= fTarget - 3.0f ? fTarget - 3.0f : fScore;
        fScore = fScore <= 3.0f + fTarget ? fScore : 3.0f + fTarget;
        nTarget = (s32)(0.5f + fScore);
    } else {
        nTarget = pEntrantMC->nTargetScore - GM_PgaTourSim_GetTotalScoreFromEntrantID(nPlayer, nEntrant, 0);
    }
    while (TotalEntrantHoleScores(nEntrant) > nTarget) {
        s32 nHole = Misc_RandFunc(0) % 18;
        u8 bDone = 0;

        while (!bDone) {
            if (pEntrant->aHoleStrokes[nHole] > 1) {
                if (pEntrant->aHoleStrokes[nHole] == 2) {
                    if (Misc_RandFuncf(0) < 0.0005f) {
                        bDone = 1;
                        pEntrant->aHoleStrokes[nHole]--;
                    }
                } else {
                    pEntrant->aHoleStrokes[nHole]--;
                    bDone = 1;
                }
            }
            nHole++;
            if (nHole >= 18) {
                nHole = 0;
            }
        }
    }
    while (TotalEntrantHoleScores(nEntrant) < nTarget) {
        s32 nHole = Misc_RandFunc(0) % 18;
        u8 bDone = 0;

        while (!bDone) {
            if (pEntrant->aHoleStrokes[nHole] < 10) {
                if (pEntrant->aHoleStrokes[nHole] >= 6) {
                    if (Misc_RandFuncf(0) < 0.01f) {
                        bDone = 1;
                        pEntrant->aHoleStrokes[nHole]++;
                    }
                } else {
                    pEntrant->aHoleStrokes[nHole]++;
                    bDone = 1;
                }
            }
            nHole++;
            if (nHole >= 18) {
                nHole = 0;
            }
        }
    }
}

// One hole of a simulated round counted in the entrant's season statistics: the hole, round and
// tournament counts, the score against par, and a green in regulation, a drive, the putts, a
// fairway and a bunker save made up from the pro's season form (the player's entrant would use the
// first pro's; GM_PgaTourSim_SimStats skips it).
void GM_PgaTourSim_SimEntrantStatsOnHole(int nPlayer, int nRound, int nEntrant, int nHole) {
    PgaEntrantMC* pEntrantMC = GetEntrantMCPtr(nPlayer, nEntrant);
    PgaEntrant* pEntrant = GetEntrantNonMCPtr(nEntrant);
    PgaPro* pPro;
    PgaStatCounts* pStats = &gpSaveData[nPlayer].tour.aStats[pEntrantMC->nGolfer];
    s32 nPar;
    s32 nStrokes;
    u16 bGIR;
    s32 bHit;
    f32 fDrive;
    f32 fRandom;
    s32 nDrive;
    s32 nPutts;

    gbStatsDirty = 1;
    gbScoresDirty = 1;
    if (pEntrantMC->nGolfer != PGA_USER_GOLFER) {
        pPro = &gPgaPros[pEntrantMC->nGolfer];
    } else {
        pPro = &gPgaPros[0];
    }
    nPar = Course_GetHolePar(nHole);
    nStrokes = pEntrant->aHoleStrokes[nHole];

    pStats->nHoles++;
    if (nHole == 0) {
        pStats->nRounds++;
        if (nRound == 0) {
            pStats->nEvents++;
        }
    }
    pStats->nStrokes += (u16)nStrokes;
    if (nStrokes < nPar) {
        if (nStrokes < nPar - 1) {
            pStats->nEagles++;
        }
        pStats->nBirdies++;
    } else if (nStrokes > nPar) {
        pStats->nBogeys++;
    }
    if (nHole > 0 && nStrokes < nPar && pEntrant->aHoleStrokes[nHole - 1] > Course_GetHolePar(nHole - 1)) {
        pStats->nBirdiesAfterBogey++;
    }
    switch (nPar) {
    case 3:
        pStats->nPar3Holes++;
        pStats->nPar3Strokes += (u16)nStrokes;
        if (nStrokes < nPar) {
            pStats->nPar3Birdies++;
        }
        break;
    case 4:
        pStats->nPar4Holes++;
        pStats->nPar4Strokes += (u16)nStrokes;
        if (nStrokes < nPar) {
            pStats->nPar4Birdies++;
        }
        break;
    case 5:
        pStats->nPar5Holes++;
        pStats->nPar5Strokes += (u16)nStrokes;
        if (nStrokes < nPar) {
            pStats->nPar5Birdies++;
        }
        break;
    }

    // A green in regulation: fGIRPct percent of holes.
    bGIR = 0;
    if (Misc_RandFuncf(0) * 100.0f < pPro->fGIRPct) {
        bGIR = 1;
    }
    pStats->nGreensHit += bGIR;
    if (nStrokes < nPar && bGIR) {
        pStats->nGIRBirdies++;
    }
    if (nStrokes <= nPar && !bGIR) {
        pStats->nNonGIRPars++;
    }

    // The drive: the pro's average plus 30 x a normal random number, no longer than the hole;
    // one over 520 loses up to 50.
    fRandom = 30.0f * Misc_RandFuncg(0) + pPro->fDriveAvg;
    fDrive = (fRandom > 0.0f) ? fRandom : 0.0f;
    if (fDrive > fn_800D2C30(nHole, 0)) {
        fDrive = fn_800D2C30(nHole, 0);
    }
    if (fDrive > 520.0f) {
        fDrive -= 50.0f * Misc_RandFuncf(0);
    }
    nDrive = fDrive;
    pStats->nLongestDrive = ((u16)nDrive <= pStats->nLongestDrive) ? pStats->nLongestDrive : nDrive;
    if (fn_800D3080(nHole)) {
        pStats->nDrives++;
        pStats->nDriveDistance += (u16)nDrive;
    }

    // The putts: the pro's average per hole, never more than the strokes less one.
    nPutts = 0.5f + (0.3f * Misc_RandFuncg(0) + pPro->fPuttAvg / 18.0f);
    if (nPutts <= 0) {
        if (Misc_RandFuncf(0) < 0.05f) {
            nPutts = 0;
        } else {
            nPutts = 1;
        }
    }
    nPutts = (nPutts <= nStrokes - 1) ? nPutts : nStrokes - 1;
    if (bGIR) {
        pStats->nGIRPutts += (u16)nPutts;
    }
    pStats->nPutts += (u16)nPutts;

    // A fairway on a par 4 or 5: fFairwayPct percent of them.
    if (nPar >= 4) {
        bHit = 0;
        if (Misc_RandFuncf(0) * 100.0f < pPro->fFairwayPct) {
            bHit = 1;
        }
        pStats->nFairwaysHit += bHit;
        pStats->nFairways++;
    }

    // A bunker on one hole in ten not under par, saved fSandSavePct percent of the time.
    if (nStrokes >= nPar && Misc_RandFuncf(0) < 0.1f) {
        bHit = 0;
        if (Misc_RandFuncf(0) * 100.0f < pPro->fSandSavePct) {
            bHit = 1;
        }
        pStats->nBunkerSaves += bHit;
        pStats->nBunkers++;
    }
}

// The season statistics of the current round for every CPU entrant still playing (everyone in the
// first two rounds, then those who made the cut), hole by hole
// (GM_PgaTourSim_SimEntrantStatsOnHole).
void GM_PgaTourSim_SimStats(int nPlayer) {
    s32 nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);
    int i;
    s32 nHole;

    for (i = 0; i < nEntrants; i++) {
        if (!GM_PgaTourSim_IsEntrantUser(nPlayer, i)
            && (gpSaveData[nPlayer].tour.nRound <= 1 || !GM_PgaTourSim_GetWasCutFromEntrantID(nPlayer, i))) {
            for (nHole = 0; nHole < 18; nHole++) {
                GM_PgaTourSim_SimEntrantStatsOnHole(nPlayer, gpSaveData[nPlayer].tour.nRound, i, nHole);
            }
        }
    }
}

// Starts a playoff: every entrant placed first is in it, the others out, and the player's playoff
// strokes cleared.
void GM_PgaTourSim_InitPlayoff(int nPlayer) {
    s32 nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);
    s32 i;
    PgaEntrant* pEntrant;

    for (i = 0; i < nEntrants; i++) {
        pEntrant = GetEntrantNonMCPtr(i);
        pEntrant->bInPlayoff = GM_PgaTourSim_GetScoreRankFromEntrantID(nPlayer, i) == 1;
    }
    gPgaUserPlayoffScore = 0;
}

// The number of entrants still in the playoff.
s32 GM_PgaTourSim_GetNumPlayoffEntrants(int nPlayer) {
    s32 nEntrants;
    s32 i;
    s32 nCount = 0;

    nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);
    for (i = 0; i < nEntrants; i++) {
        if (GetEntrantNonMCPtr(i)->bInPlayoff) {
            nCount++;
        }
    }
    return nCount;
}

u8 GM_PgaTourSim_EntrantIsInPlayoff(int nPlayer, int nEntrant) {
    return GetEntrantNonMCPtr(nEntrant)->bInPlayoff;
}

// A playoff hole is over (nHole: its index in each entrant's aHoleStrokes). The player's strokes on
// it are lbl_80282504 (GM_PgaTourSim_SetUserEntrantHoleStrokes). The first opponent still in the
// playoff who took fewer knocks the player out and ends the check (the opponents after him are left
// as they are); an opponent who took more drops out; the same strokes play on.
void GM_PgaTourSim_UpdatePlayoffs(int nPlayer, int nHole) {
    s32 nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);
    s32 i;
    PgaEntrant* pEntrant;
    PgaEntrant* pUser;

    for (i = 1; i < nEntrants; i++) {
        pEntrant = GetEntrantNonMCPtr(i);
        if (pEntrant->bInPlayoff) {
            if (pEntrant->aHoleStrokes[nHole] < gPgaUserPlayoffScore) {
                pUser = GetEntrantNonMCPtr(0);
                pUser->bInPlayoff = 0;
                return;
            }
            if (pEntrant->aHoleStrokes[nHole] > gPgaUserPlayoffScore) {
                pEntrant->bInPlayoff = 0;
            }
        }
    }
}

// The fewest strokes on a playoff hole among the player's opponents still in the playoff (entrants
// 1 on), 999 when none is left. GameModeDriverPGATour.c compares it with the player's strokes for
// the lead and the putt to win.
s32 GM_PgaTourSim_GetBestOpponentPlayoffHoleScore(int nPlayer, int nHole) {
    s32 i;
    s32 nBest = 999;
    s32 nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);
    PgaEntrant* pEntrant;

    for (i = 1; i < nEntrants; i++) {
        pEntrant = GetEntrantNonMCPtr(i);
        if (pEntrant->bInPlayoff) {
            nBest = nBest <= pEntrant->aHoleStrokes[nHole] ? nBest : pEntrant->aHoleStrokes[nHole];
        }
    }
    return nBest;
}

// Every statistic and ranking worked out again (CalcAllStats) if the counts changed since the last
// time (gbStatsDirty). Every statistic getter calls it first.
void CalcAllStatsIfDirty(int nPlayer) {
    if (gbStatsDirty) {
        CalcAllStats(nPlayer);
        gbStatsDirty = 0;
    }
}

// The score order: every entrant sorted by score (TournamentRankIncreasing; lbl_80281848 tells it
// the player), then each given its place, entrants on the same score sharing it. A score counts the
// holes each entrant has finished; a cut entrant counts as the worst score, the winner as the best,
// so after a playoff the winner has first place alone.
void CalcScoreRankings(int nPlayer) {
    s32 i;
    s32 nEntrant;
    s32 nScore;
    s32 nRank;
    s32 nPrevScore;
    s32 nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);

    if (nEntrants == 0) return;
    for (i = 0; i < nEntrants; i++) {
        gPgaScoreRanking.aEntrant[i] = i;
    }
    for (i = nEntrants; i < PGA_MAX_ENTRANTS; i++) {
        gPgaScoreRanking.aEntrant[i] = -1;
    }
    gPgaScoreSortPlayer = nPlayer;
    qsort(gPgaScoreRanking.aEntrant, nEntrants, sizeof(gPgaScoreRanking.aEntrant[0]),
          TournamentRankIncreasing);
    gPgaScoreSortPlayer = 0;
    nPrevScore = 0;
    nRank = 0;
    for (i = 0; i < nEntrants; i++) {
        nEntrant = gPgaScoreRanking.aEntrant[i];
        if (GM_PgaTourSim_GetWasCutFromEntrantID(nPlayer, nEntrant)) {
            nScore = PGA_SCORE_CUT;
        } else if (nEntrant == gpSaveData[nPlayer].tour.field.nWinner) {
            nScore = PGA_SCORE_WINNER;
        } else {
            nScore = GM_PgaTourSim_GetRelativeScoreFromEntrantID(nPlayer, nEntrant, !GM_PgaTourSim_IsEntrantUser(nPlayer, nEntrant));
        }
        if (i == 0 || nPrevScore != nScore) {
            nRank = i + 1;
            nPrevScore = nScore;
        }
        gPgaScoreRanking.aRank[nEntrant] = nRank;
    }
}

// After CalcScoreRankings: the cut entrants, who all sorted last as the same worst score and so by
// name, are sorted again among themselves by their real score
// (TournamentRankIncreasingForCutEntrants), so the leader board lists them in score order. The
// places are then given again as there; the cut entrants still share one place.
void CalcScoreRankingsForCutEntrants(int nPlayer) {
    s32 nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);
    s32 nCutRow;
    s32 i;
    s32 nEntrant;
    s32 nScore;
    s32 nRank;
    s32 nPrevScore;

    if (nEntrants == 0) return;
    gPgaScoreSortPlayer = nPlayer;
    nCutRow = FindFirstCutEntrantIndex(nPlayer);
    if (nCutRow != -1) {
        qsort(&gPgaScoreRanking.aEntrant[nCutRow], nEntrants - nCutRow, sizeof(gPgaScoreRanking.aEntrant[0]),
              TournamentRankIncreasingForCutEntrants);
    }
    gPgaScoreSortPlayer = 0;
    nPrevScore = 0;
    nRank = 0;
    for (i = 0; i < nEntrants; i++) {
        nEntrant = gPgaScoreRanking.aEntrant[i];
        if (GM_PgaTourSim_GetWasCutFromEntrantID(nPlayer, nEntrant)) {
            nScore = PGA_SCORE_CUT;
        } else if (nEntrant == gpSaveData[nPlayer].tour.field.nWinner) {
            nScore = PGA_SCORE_WINNER;
        } else {
            nScore = GM_PgaTourSim_GetRelativeScoreFromEntrantID(nPlayer, nEntrant, !GM_PgaTourSim_IsEntrantUser(nPlayer, nEntrant));
        }
        if (i == 0 || nPrevScore != nScore) {
            nRank = i + 1;
            nPrevScore = nScore;
        }
        gPgaScoreRanking.aRank[nEntrant] = nRank;
    }
}

// The score order and places made again (CalcScoreRankings, then CalcScoreRankingsForCutEntrants)
// if a score changed since the last time (gbScoresDirty). Every score-rank getter calls it first.
void CalcScoreRankingsIfDirty(int nPlayer) {
    if (gbScoresDirty) {
        CalcScoreRankings(nPlayer);
        gbScoresDirty = 0;
        CalcScoreRankingsForCutEntrants(nPlayer);
    }
}

// Ranks every tour golfer in a statistic: sorts them with the statistic's comparison (lbl_80193FF8:
// StatRankIncreasing or StatRankDecreasing; lbl_80281840 tells it the statistic and the player),
// then gives each its place. Golfers whose values print the same share the place.
void CalcRankingsForStat(int nPlayer, GM_Pga_StatTypes_t nStat) {
    PgaStatRanking* pRanking = &gPgaStatRankings[nStat];
    char* szValue;
    s32 nRow;
    s32 nGolfer;
    s32 nRank;
    char szPrev[16];
    int i;

    for (i = 0; i < PGA_NUM_GOLFERS; i++) {
        pRanking->aGolfer[i] = i;
    }
    gPgaStatSort.nPlayer = nPlayer;
    gPgaStatSort.nStat = nStat;
    qsort(pRanking->aGolfer, PGA_NUM_GOLFERS, sizeof(pRanking->aGolfer[0]), gPgaStatCompares[nStat]);
    gPgaStatSort.nStat = -1;
    gPgaStatSort.nPlayer = 0;
    sprintf(szPrev, "");
    nRank = 0;
    for (nRow = 0; nRow < PGA_NUM_GOLFERS; nRow++) {
        nGolfer = pRanking->aGolfer[nRow];
        szValue = pRanking->aValue[nGolfer].szValue;
        if (strcmp(szPrev, szValue) != 0) {
            nRank = nRow + 1;
            strcpy(szPrev, szValue);
        }
        pRanking->aRank[nGolfer] = nRank;
    }
}

// Works out a golfer's 28 simple statistics from the golfer's season counts in the player's profile
// (the Calc functions of lbl_80193F88; their return is not used), each with its text
// (GM_PgaTourSim_GetStatValString).
void CalcSimplePlayerStats(int nPlayer, int nGolfer) {
    GM_Pga_StatTypes_t nStat;

    for (nStat = 0; nStat < GM_PGA_STAT_SIMPLE_COUNT; nStat++) {
        gPgaSimpleStatCalcs[nStat](&gpSaveData[nPlayer].tour.aStats[nGolfer],
                            &gPgaStatRankings[nStat].aValue[nGolfer].fValue);
        GM_PgaTourSim_GetStatValString(nStat, gPgaStatRankings[nStat].aValue[nGolfer].fValue,
                                       gPgaStatRankings[nStat].aValue[nGolfer].szValue);
    }
}

// The simple statistics of every tour golfer and the player (CalcSimplePlayerStats).
void CalcAllSimpleStats(int nPlayer) {
    s32 nGolfer;

    for (nGolfer = 0; nGolfer < PGA_NUM_GOLFERS; nGolfer++) {
        CalcSimplePlayerStats(nPlayer, nGolfer);
    }
}

// Every simple statistic's ranking (CalcRankingsForStat), which the combined statistics add up.
void CalcAllSimpleRankings(int nPlayer) {
    s32 nStat;

    for (nStat = 0; nStat < GM_PGA_STAT_SIMPLE_COUNT; nStat++) {
        CalcRankingsForStat(nPlayer, nStat);
    }
}

// Works out a golfer's two first combined statistics, all-around (CalcAllAroundScore) and total
// driving (CalcTotalDriving), from the simple statistics' rankings, each with its text.
void CalcComplex1PlayerStats(int nGolfer) {
    CalcAllAroundScore(nGolfer, &gPgaStatRankings[GM_PGA_STAT_ALLAROUND].aValue[nGolfer].fValue);
    CalcTotalDriving(nGolfer, &gPgaStatRankings[GM_PGA_STAT_TOTALDRIVING].aValue[nGolfer].fValue);
    GM_PgaTourSim_GetStatValString(GM_PGA_STAT_ALLAROUND,
                                   gPgaStatRankings[GM_PGA_STAT_ALLAROUND].aValue[nGolfer].fValue,
                                   gPgaStatRankings[GM_PGA_STAT_ALLAROUND].aValue[nGolfer].szValue);
    GM_PgaTourSim_GetStatValString(GM_PGA_STAT_TOTALDRIVING,
                                   gPgaStatRankings[GM_PGA_STAT_TOTALDRIVING].aValue[nGolfer].fValue,
                                   gPgaStatRankings[GM_PGA_STAT_TOTALDRIVING].aValue[nGolfer].szValue);
}

// Every tour golfer's all-around and total driving statistics (CalcComplex1PlayerStats); the simple
// rankings must be made first.
void CalcAllComplex1Stats(void) {
    s32 nGolfer;

    for (nGolfer = 0; nGolfer < PGA_NUM_GOLFERS; nGolfer++) {
        CalcComplex1PlayerStats(nGolfer);
    }
}

// The all-around and total driving rankings (CalcRankingsForStat).
void CalcAllComplex1Rankings(int nPlayer) {
    s32 nStat;

    for (nStat = GM_PGA_STAT_SIMPLE_COUNT; nStat < GM_PGA_STAT_COMPLEX1_COUNT; nStat++) {
        CalcRankingsForStat(nPlayer, nStat);
    }
}

// Works out a golfer's ball striking statistic (CalcBallStriking), which adds up the total driving
// ranking, with its text.
void CalcComplex2PlayerStats(int nGolfer) {
    CalcBallStriking(nGolfer, &gPgaStatRankings[GM_PGA_STAT_BALLSTRIKING].aValue[nGolfer].fValue);
    GM_PgaTourSim_GetStatValString(GM_PGA_STAT_BALLSTRIKING,
                                   gPgaStatRankings[GM_PGA_STAT_BALLSTRIKING].aValue[nGolfer].fValue,
                                   gPgaStatRankings[GM_PGA_STAT_BALLSTRIKING].aValue[nGolfer].szValue);
}

// Every tour golfer's ball striking statistic (CalcComplex2PlayerStats); the total driving ranking
// must be made first.
void CalcAllComplex2Stats(void) {
    s32 nGolfer;

    for (nGolfer = 0; nGolfer < PGA_NUM_GOLFERS; nGolfer++) {
        CalcComplex2PlayerStats(nGolfer);
    }
}

// The ball striking ranking, the only statistic of the second combined level.
void CalcAllComplex2Rankings(int nPlayer) {
    CalcRankingsForStat(nPlayer, GM_PGA_STAT_BALLSTRIKING);
}

// Works out and ranks every statistic of every tour golfer, level by level: the simple ones from
// the counts, then all-around and total driving from the simple rankings, then ball striking from
// the total driving ranking.
void CalcAllStats(int nPlayer) {
    CalcAllSimpleStats(nPlayer);
    CalcAllSimpleRankings(nPlayer);
    CalcAllComplex1Stats();
    CalcAllComplex1Rankings(nPlayer);
    CalcAllComplex2Stats();
    CalcAllComplex2Rankings(nPlayer);
}

// The simple tour statistics. Each takes a golfer's season counts and puts the statistic in
// *pfValue; lbl_80193F88 lists them in this order, one per statistic. A ratio with nothing to
// divide by is 0 and returns 0; every other value returns 1 (CalcSimplePlayerStats does not look).

u8 SafeDivide(u32 nCount, u32 nOutOf, f32* pfValue);
u8 SafeDividePct(u32 nCount, u32 nOutOf, f32* pfValue);

// Yards per drive: all drives' distance over the number of drives.
u8 CalcDrivingDistance(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDivide(pCounts->nDriveDistance, pCounts->nDrives, pfValue);
}

// *pfValue = nCount / nOutOf as a float, and returns 1; when nOutOf is 0, *pfValue = 0 and returns
// 0 (a golfer with nothing counted yet).
u8 SafeDivide(u32 nCount, u32 nOutOf, f32* pfValue) {
    if (nOutOf == 0) {
        *pfValue = 0.0f;
        return 0;
    }
    *pfValue = (f32)nCount / (f32)nOutOf;
    return 1;
}

// Driving accuracy: the percent of fairways hit (the par 4s and 5s).
u8 CalcAccuracy(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nFairwaysHit, pCounts->nFairways, pfValue);
}

// SafeDivide as a percentage: *pfValue = 100 * nCount / nOutOf, and returns 1; when nOutOf is 0,
// *pfValue = 0 and returns 0.
u8 SafeDividePct(u32 nCount, u32 nOutOf, f32* pfValue) {
    if (nOutOf == 0) {
        *pfValue = 0.0f;
        return 0;
    }
    *pfValue = 100.0f * ((f32)nCount / (f32)nOutOf);
    return 1;
}

// Greens in regulation: the percent of holes on which the green was hit in regulation.
u8 CalcGIR(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nGreensHit, pCounts->nHoles, pfValue);
}

// Putts per round.
u8 CalcPuttsPerRound(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDivide(pCounts->nPutts, pCounts->nRounds, pfValue);
}

// Putting average: putts per green hit in regulation.
u8 CalcPuttingAvg(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDivide(pCounts->nGIRPutts, pCounts->nGreensHit, pfValue);
}

// Sand saves: the percent of holes with a bunker in them that the golfer still finished in par or
// better.
u8 CalcSandSave(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nBunkerSaves, pCounts->nBunkers, pfValue);
}

// Scrambling: the percent of greens missed in regulation on which the golfer still made par or
// better.
u8 CalcScrambling(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nNonGIRPars, pCounts->nHoles - pCounts->nGreensHit, pfValue);
}

// Bounce back: the birdies or better made right after a bogey or worse, as a percent of the bogeys
// or worse.
u8 CalcBounceBack(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nBirdiesAfterBogey, pCounts->nBogeys, pfValue);
}

// Holes played per eagle (0 while the golfer has none).
u8 CalcHolesPerEagle(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDivide(pCounts->nHoles, pCounts->nEagles, pfValue);
}

// Birdie average: birdies or better per round.
u8 CalcBirdieAvg(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDivide(pCounts->nBirdies, pCounts->nRounds, pfValue);
}

// The percent of par 3s played that the golfer birdied.
u8 CalcPar3BirdieAvg(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nPar3Birdies, pCounts->nPar3Holes, pfValue);
}

// The percent of par 4s played that the golfer birdied.
u8 CalcPar4BirdieAvg(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nPar4Birdies, pCounts->nPar4Holes, pfValue);
}

// The percent of par 5s played that the golfer birdied.
u8 CalcPar5BirdieAvg(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nPar5Birdies, pCounts->nPar5Holes, pfValue);
}

// Birdie conversion: the percent of greens hit in regulation on which the golfer made birdie or
// better.
u8 CalcBirdieConversion(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nGIRBirdies, pCounts->nGreensHit, pfValue);
}

// Scoring average: strokes per round.
u8 CalcScoringAvg(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDivide(pCounts->nStrokes, pCounts->nRounds, pfValue);
}

// Par breakers: birdies plus eagles as a percent of the holes played.
// EA bug: nBirdies already counts every hole under par, eagles included (GameModeDriverPGATour.c
// and the simulated holes both count an eagle in nBirdies and nEagles), so each eagle counts twice.
u8 CalcParBreakers(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nBirdies + pCounts->nEagles, pCounts->nHoles, pfValue);
}

// Par 3 performance: strokes per par 3.
u8 CalcPar3ScoringAvg(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDivide(pCounts->nPar3Strokes, pCounts->nPar3Holes, pfValue);
}

// Par 4 performance: strokes per par 4.
u8 CalcPar4ScoringAvg(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDivide(pCounts->nPar4Strokes, pCounts->nPar4Holes, pfValue);
}

// Par 5 performance: strokes per par 5.
u8 CalcPar5ScoringAvg(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDivide(pCounts->nPar5Strokes, pCounts->nPar5Holes, pfValue);
}

u8 CalcLongestDrive(PgaStatCounts* pCounts, f32* pfValue) {
    *pfValue = pCounts->nLongestDrive;
    return 1;
}

u8 CalcLongestPutt(PgaStatCounts* pCounts, f32* pfValue) {
    *pfValue = pCounts->nLongestPutt;
    return 1;
}

u8 CalcTotalEagles(PgaStatCounts* pCounts, f32* pfValue) {
    *pfValue = pCounts->nEagles;
    return 1;
}

u8 CalcTotalBirdies(PgaStatCounts* pCounts, f32* pfValue) {
    *pfValue = pCounts->nBirdies;
    return 1;
}

u8 CalcConsecutiveCuts(PgaStatCounts* pCounts, f32* pfValue) {
    *pfValue = pCounts->nConsecutiveCuts;
    return 1;
}

u8 CalcSeasonWinnings(PgaStatCounts* pCounts, f32* pfValue) {
    *pfValue = pCounts->nSeasonWinnings;
    return 1;
}

u8 CalcCareerWinnings(PgaStatCounts* pCounts, f32* pfValue) {
    *pfValue = pCounts->nCareerWinnings;
    return 1;
}

u8 CalcRounds(PgaStatCounts* pCounts, f32* pfValue) {
    *pfValue = pCounts->nRounds;
    return 1;
}

u8 CalcPlayerOfYearPoints(PgaStatCounts* pCounts, f32* pfValue) {
    *pfValue = pCounts->nPlayerOfYearPoints;
    return 1;
}

// The combined statistics add up a golfer's places in others (lower is better), like the real
// tour's: all-around over eight statistics, total driving over distance and accuracy, ball
// striking over total driving and greens in regulation.

static inline f32 StatRank(GM_Pga_StatTypes_t nStat, int nGolfer) {
    return gPgaStatRankings[nStat].aRank[nGolfer];
}

// All-around: the sum of the golfer's places in driving distance, driving accuracy, greens in
// regulation, putting average, sand saves, holes per eagle, birdie average and scoring average
// (lower is better).
void CalcAllAroundScore(int nGolfer, f32* pfValue) {
    *pfValue = StatRank(GM_PGA_STAT_DRIVING, nGolfer) + StatRank(GM_PGA_STAT_FAIRWAYS, nGolfer)
             + StatRank(GM_PGA_STAT_GIR, nGolfer) + StatRank(GM_PGA_STAT_PUTTING, nGolfer)
             + StatRank(GM_PGA_STAT_SAVES, nGolfer) + StatRank(GM_PGA_STAT_HOLESPEREAGLE, nGolfer)
             + StatRank(GM_PGA_STAT_BIRDIESPERROUND, nGolfer) + StatRank(GM_PGA_STAT_SCORING, nGolfer);
}

// Total driving: the golfer's place in driving distance plus the place in driving accuracy (lower
// is better).
void CalcTotalDriving(int nGolfer, f32* pfValue) {
    *pfValue = StatRank(GM_PGA_STAT_DRIVING, nGolfer) + StatRank(GM_PGA_STAT_FAIRWAYS, nGolfer);
}

// Ball striking: the golfer's place in total driving plus the place in greens in regulation (lower
// is better).
void CalcBallStriking(int nGolfer, f32* pfValue) {
    *pfValue = StatRank(GM_PGA_STAT_TOTALDRIVING, nGolfer) + StatRank(GM_PGA_STAT_GIR, nGolfer);
}

// Pays one place's prize money: nTotal (the prizes of the score rows the place fills, added up) is
// split evenly among the nCount entrants from score row nFirstRow, the share cut to a whole number.
// Each share becomes the entrant's leader board winnings (PgaEntrantMC n18) and is added to the
// golfer's month, season and career winnings; the player's share is also paid into the profile
// (GameModeDriverPGATour_AwardMoney). nCount 0 pays nothing.
void SplitWinnings(int nPlayer, s32 nTotal, s32 nFirstRow, s32 nCount) {
    int i;
    s32 nShare;

    if (nCount == 0) {
        return;
    }
    nShare = (f32)nTotal / (f32)nCount;
    for (i = 0; i < nCount; i++) {
        int nEntrant = gPgaScoreRanking.aEntrant[nFirstRow + i];
        GetEntrantMCPtr(nPlayer, nEntrant)->n18 = nShare;
        gpSaveData[nPlayer].tour.aStats[GM_PgaTourSim_GetGolferIDFromEntrantID(nPlayer, nEntrant)].n44 += nShare;
        gpSaveData[nPlayer].tour.aStats[GM_PgaTourSim_GetGolferIDFromEntrantID(nPlayer, nEntrant)].nSeasonWinnings += nShare;
        gpSaveData[nPlayer].tour.aStats[GM_PgaTourSim_GetGolferIDFromEntrantID(nPlayer, nEntrant)].nCareerWinnings += nShare;
        if (GM_PgaTourSim_IsEntrantUser(nPlayer, nEntrant)) {
            GameModeDriverPGATour_AwardMoney(nPlayer, nShare);
        }
    }
}

// The tournament's prize money paid down the score order at its end
// (GM_PgaTourSim_SimTournamentWinner): each of the first 70 score rows is worth
// GM_Earnings_TournamentPayout(nPurse, nFirstPrize, row), and the entrants tied on a place split
// the rows they fill (SplitWinnings). The cut entrants, who sort last, get nothing.
void GM_PgaTourSim_DistributeWinnings(int nPlayer, int nPurse, int nFirstPrize) {
    s32 nRow;
    s32 nRank = -1;
    s32 nPool = 0;
    s32 nFirstRow = 0;
    s32 nTied = 0;
    s32 nEntrants = GM_PgaTourSim_GetNumEntrants(nPlayer);
    s32 nEntrant;

    CalcScoreRankingsIfDirty(nPlayer);
    for (nRow = 0; nRow < nEntrants; nRow++) {
        nEntrant = gPgaScoreRanking.aEntrant[nRow];
        if (gPgaScoreRanking.aRank[nEntrant] != nRank && nRow < 70) {
            SplitWinnings(nPlayer, nPool, nFirstRow, nTied);
            nRank = gPgaScoreRanking.aRank[nEntrant];
            nFirstRow = nRow;
            nTied = 0;
            nPool = 0;
        }
        if (GM_PgaTourSim_GetWasCutFromEntrantID(nPlayer, nEntrant)) {
            break;
        }
        if (nRow < 70) {
            nPool += GM_Earnings_TournamentPayout(nPurse, nFirstPrize, nRow);
        }
        // EA bug: after row 70 no new place starts, so an entrant below the 70th row who made the
        // cut is counted into the last paid place even with a worse score, and takes a share of
        // that place's prizes from the golfers who earned them. It happens when more than 70
        // make the cut (ties at 70th all do, CalculateCutRow; the field is 100 to 127).
        nTied++;
    }
    SplitWinnings(nPlayer, nPool, nFirstRow, nTied);
}

// The statistic sort comparisons (gPgaStatCompares). Golfers whose values print differently go by
// value, a golfer with no value (0) last; the same printed value goes by name.

// The statistic sort (qsort over golfer ids; lbl_80281840 gives the statistic and the player) for
// the statistics where lower is better: putts, putting and scoring averages, holes per eagle, the
// par 3, 4 and 5 averages and the combined rankings. Lower values first, but a golfer with no value
// (0) after every golfer with one; golfers whose values print the same go by name.
s32 StatRankIncreasing(const void* pA, const void* pB) {
    s32 nGolferA = *(const s32*)pA;
    s32 nGolferB = *(const s32*)pB;
    s32 nRet;
    int nPlayer = gPgaStatSort.nPlayer;
    f32 fA = gPgaStatRankings[gPgaStatSort.nStat].aValue[nGolferA].fValue;
    f32 fB = gPgaStatRankings[gPgaStatSort.nStat].aValue[nGolferB].fValue;

    if (strcmp(gPgaStatRankings[gPgaStatSort.nStat].aValue[nGolferA].szValue,
               gPgaStatRankings[gPgaStatSort.nStat].aValue[nGolferB].szValue) != 0) {
        if (fA < fB) {
            if (0.0f == fA) {
                nRet = 1;
            } else {
                nRet = -1;
            }
        } else if (fA > fB) {
            if (0.0f == fB) {
                nRet = -1;
            } else {
                nRet = 1;
            }
        }
        // EA bug: nRet is never set when the texts differ but the values are equal. It cannot
        // happen here: every value's text comes from GM_PgaTourSim_GetStatValString, so equal
        // values print the same.
    } else {
        nRet = strcmp(GM_PgaTourSim_GetNameFromGolferID(nPlayer, nGolferA),
                      GM_PgaTourSim_GetNameFromGolferID(nPlayer, nGolferB));
    }
    return nRet;
}

// The statistic sort (as StatRankIncreasing) for the statistics where higher is better: higher
// values first; golfers whose values print the same go by name. GM_PgaTourSim_IsLeaderForStat tells
// the two kinds apart by this function.
s32 StatRankDecreasing(const void* pA, const void* pB) {
    s32 nGolferA = *(const s32*)pA;
    s32 nGolferB = *(const s32*)pB;
    s32 nRet;
    int nPlayer = gPgaStatSort.nPlayer;
    f32 fA = gPgaStatRankings[gPgaStatSort.nStat].aValue[nGolferA].fValue;
    f32 fB = gPgaStatRankings[gPgaStatSort.nStat].aValue[nGolferB].fValue;

    if (strcmp(gPgaStatRankings[gPgaStatSort.nStat].aValue[nGolferA].szValue,
               gPgaStatRankings[gPgaStatSort.nStat].aValue[nGolferB].szValue) != 0) {
        if (fA > fB) {
            nRet = -1;
        } else if (fA < fB) {
            nRet = 1;
        }
        // EA bug: nRet is never set when the texts differ but the values are equal (it cannot
        // happen here, as in StatRankIncreasing).
    } else {
        nRet = strcmp(GM_PgaTourSim_GetNameFromGolferID(nPlayer, nGolferA),
                      GM_PgaTourSim_GetNameFromGolferID(nPlayer, nGolferB));
    }
    return nRet;
}

// The score sort comparisons (gPgaScoreSortPlayer is the player). Lower scores first, then by name.

// The score sort of CalcScoreRankings (qsort over entrant numbers; lbl_80281848 gives the player):
// lower score to par first, counting the holes each entrant has finished; a cut entrant sorts as
// the worst score, the winner as the best; the same score goes by the golfer's name.
s32 TournamentRankIncreasing(const void* pA, const void* pB) {
    PgaEntrantMC* pEntrantA;
    PgaEntrantMC* pEntrantB;
    int nEntrantA;
    int nEntrantB;
    s32 nScoreA;
    s32 nScoreB;
    int nPlayer;

    nEntrantA = *(const s32*)pA;
    nEntrantB = *(const s32*)pB;
    nPlayer = gPgaScoreSortPlayer;
    pEntrantA = GetEntrantMCPtr(nPlayer, nEntrantA);
    pEntrantB = GetEntrantMCPtr(nPlayer, nEntrantB);
    nScoreA = GM_PgaTourSim_GetRelativeScoreFromEntrantID(nPlayer, nEntrantA, !GM_PgaTourSim_IsEntrantUser(nPlayer, nEntrantA));
    if (GM_PgaTourSim_GetWasCutFromEntrantID(nPlayer, nEntrantA)) {
        nScoreA = PGA_SCORE_CUT;
    } else if (nEntrantA == gpSaveData[nPlayer].tour.field.nWinner) {
        nScoreA = PGA_SCORE_WINNER;
    }
    nScoreB = GM_PgaTourSim_GetRelativeScoreFromEntrantID(nPlayer, nEntrantB, !GM_PgaTourSim_IsEntrantUser(nPlayer, nEntrantB));
    if (GM_PgaTourSim_GetWasCutFromEntrantID(nPlayer, nEntrantB)) {
        nScoreB = PGA_SCORE_CUT;
    } else if (nEntrantB == gpSaveData[nPlayer].tour.field.nWinner) {
        nScoreB = PGA_SCORE_WINNER;
    }
    if (nScoreA < nScoreB) {
        return -1;
    }
    if (nScoreA > nScoreB) {
        return 1;
    }
    return strcmp(GM_PgaTourSim_GetNameFromGolferID(nPlayer, pEntrantA->nGolfer),
                  GM_PgaTourSim_GetNameFromGolferID(nPlayer, pEntrantB->nGolfer));
}

// The score sort of CalcScoreRankingsForCutEntrants: the cut entrants by their real score to par
// (no cut or winner override), then by the golfer's name.
s32 TournamentRankIncreasingForCutEntrants(const void* pA, const void* pB) {
    int nEntrantA = *(const s32*)pA;
    int nEntrantB = *(const s32*)pB;
    int nPlayer = gPgaScoreSortPlayer;
    PgaEntrantMC* pEntrantA = GetEntrantMCPtr(nPlayer, nEntrantA);
    PgaEntrantMC* pEntrantB = GetEntrantMCPtr(nPlayer, nEntrantB);
    s32 nScoreA = GM_PgaTourSim_GetRelativeScoreFromEntrantID(nPlayer, nEntrantA, !GM_PgaTourSim_IsEntrantUser(nPlayer, nEntrantA));
    s32 nScoreB = GM_PgaTourSim_GetRelativeScoreFromEntrantID(nPlayer, nEntrantB, !GM_PgaTourSim_IsEntrantUser(nPlayer, nEntrantB));

    if (nScoreA < nScoreB) {
        return -1;
    }
    if (nScoreA > nScoreB) {
        return 1;
    }
    return strcmp(GM_PgaTourSim_GetNameFromGolferID(nPlayer, pEntrantA->nGolfer),
                  GM_PgaTourSim_GetNameFromGolferID(nPlayer, pEntrantB->nGolfer));
}

// Empty in this build. GM_PgaTourSim_CheckEndOfTournamentAward calls it each time the player wins
// an award, with a movie number and a count (TW07's movieIndex and numRandom): TW07's name says it
// plays the award's video.
void PlayPGAAwardVideo(int nMovie, int nNumRandom) {
}

// Sets gbStatsDirty: 1 makes the next statistic getter work every statistic out again
// (CalcAllStatsIfDirty). FE message 742 sets it.
void GM_PgaTourSim_SetStatsDirty(u8 bDirty) {
    gbStatsDirty = bDirty;
}

// Sets gbScoresDirty: 1 makes the next score getter sort the field again
// (CalcScoreRankingsIfDirty). FE message 743 sets it.
void GM_PgaTourSim_SetScoresDirty(u8 bDirty) {
    gbScoresDirty = bDirty;
}

// Per simple statistic: the function that works it out from one golfer's season counts.
u8 (*gPgaSimpleStatCalcs[GM_PGA_STAT_SIMPLE_COUNT])(PgaStatCounts* pCounts, f32* pfValue) = {
    CalcDrivingDistance, CalcAccuracy, CalcGIR, CalcPuttsPerRound, CalcPuttingAvg, CalcSandSave,
    CalcScrambling, CalcBounceBack, CalcHolesPerEagle, CalcBirdieAvg, CalcPar3BirdieAvg,
    CalcPar4BirdieAvg, CalcPar5BirdieAvg, CalcBirdieConversion, CalcScoringAvg, CalcParBreakers,
    CalcPar3ScoringAvg, CalcPar4ScoringAvg, CalcPar5ScoringAvg, CalcLongestDrive, CalcLongestPutt,
    CalcTotalEagles, CalcTotalBirdies, CalcConsecutiveCuts, CalcSeasonWinnings, CalcCareerWinnings,
    CalcRounds, CalcPlayerOfYearPoints,
};
// Per statistic: its ranking's sort comparison, StatRankDecreasing where higher is better,
// StatRankIncreasing where lower is (GM_PgaTourSim_IsLeaderForStat compares the same way).
s32 (*gPgaStatCompares[GM_PGA_STAT_COUNT])(const void* pA, const void* pB) = {
    StatRankDecreasing, StatRankDecreasing, StatRankDecreasing, StatRankIncreasing, StatRankIncreasing,
    StatRankDecreasing, StatRankDecreasing, StatRankDecreasing, StatRankIncreasing, StatRankDecreasing,
    StatRankDecreasing, StatRankDecreasing, StatRankDecreasing, StatRankDecreasing, StatRankIncreasing,
    StatRankDecreasing, StatRankIncreasing, StatRankIncreasing, StatRankIncreasing, StatRankDecreasing,
    StatRankDecreasing, StatRankDecreasing, StatRankDecreasing, StatRankDecreasing, StatRankDecreasing,
    StatRankDecreasing, StatRankDecreasing, StatRankDecreasing, StatRankIncreasing, StatRankIncreasing,
    StatRankIncreasing,
};
// Per statistic: its view (GM_PgaTourSim_GetStatView). The statistics screen's played column shows
// the golfer's tournaments for 0 and rounds for 1.
s32 gPgaStatViews[GM_PGA_STAT_COUNT] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
};
// Per statistic: the decimal places GM_PgaTourSim_GetStatValString prints (one entry more than
// there are statistics).
s32 gPgaStatDecimals[32] = {
    1, 1, 1, 2, 3, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
