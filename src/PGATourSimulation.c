// PGATourSimulation.c (TW06's pgatoursimulation.c): the PGA TOUR simulation behind game mode 23
// (GameModeDriverPGATour.c). It keeps the tour field (an entrant table in the save profile and
// one in memory, with each entrant's hole strokes and playoff state), the entrants' score
// ranking, and the season statistics of every tour golfer: the counts in the save profile
// (PgaStatCounts), each statistic worked out from them (driving distance, greens in regulation,
// scoring average, ...), each statistic's ranking, and the text fe_stats.c prints.

#include "engine.h"
#include "game.h"
#include "game/save.h"
#include "game/modes/pgatoursim.h"
#include "game/modes/pgatour.h"

PgaEntrantMC* GetEntrantMCPtr(int nPlayer, int nEntrant);
void fn_80117BF4(int nPlayer, int nEntrant);
s32  fn_80119A50(int nPlayer);
void fn_801187F0(PgaEntrantMC* aEntrant, s16* pnEntrants, u8 bUser);
void fn_80118B0C(int nPlayer, int n);
void fn_80119B54(int nPlayer, int nRound, int nEntrant, int nHole);
void fn_80119E28(int nPlayer, int nEntrant, int nRound);
s32  fn_80119AE0(int nPlayer);
s32  fn_8011BDF8(const void* pA, const void* pB);
s32  fn_8011BF74(const void* pA, const void* pB);
s32  fn_8011BCFC(const void* pA, const void* pB);
s32  TotalEntrantHoleScores(int nEntrant);
void fn_8011A074(int nPlayer, int nRound, int nEntrant, int nHole);
void fn_8011A890(int nPlayer);
void fn_8011AAC0(int nPlayer);
void fn_8011AC40(int nPlayer, GM_Pga_StatTypes_t nStat);
void fn_8011AE1C(int nPlayer, int nGolfer);
void fn_8011AF60(int nGolfer);
void fn_8011B094(int nGolfer);
void CalcAllStatsIfDirty(int nPlayer);
void CalcScoreRankingsIfDirty(int nPlayer);
void CalcAllStats(int nPlayer);
void CalcAllAroundScore(int nGolfer, f32* pfValue);
void CalcTotalDriving(int nGolfer, f32* pfValue);
void CalcBallStriking(int nGolfer, f32* pfValue);
void fn_8011B978(int nPlayer, s32 nTotal, s32 nFirstRow, s32 nCount);
void fn_8011BAD4(int nPlayer, int nTotal, int n);
void fn_801180C4(int nPlayer, u8 bUser, u8 bFirst);
void fn_8011C054(int nA, int nB);
void fn_80117694(UStreamObject* pObject);

char* GameModeDriverPGATour_GetInitialChampName(s32 i);               // a tournament's first champion
s32  GameModeDriverPGATour_GetInitialChampScore(s32 i);                // and the champion's score
s32  fn_800EF0E0(s32 nPlayer);          // GameModeDriverPGATour.c: the player's bracket

PgaEntrantMC* GetEntrantMCPtr(int nPlayer, int nEntrant) {
    return &gpSaveData[nPlayer].tour.field.aEntrant[nEntrant];
}

PgaEntrant* GetEntrantNonMCPtr(int nEntrant) {
    return &lbl_80224070[nEntrant];
}

void fn_8011763C(void) {
    UStream_RegisterHandler('PGST', fn_80117694);
}

void fn_8011766C(void) {
    UStream_UnregisterHandler('PGST');
}

// The 'PGST' stream object: the tour pros.
void fn_80117694(UStreamObject* pObject) {
    fn_8000E790(pObject, sizeof(lbl_8024B9CC), lbl_8024B9CC);
}

// A new PGA TOUR in the profile: everything cleared, the tournaments' champions from the tour data,
// and each pro's career winnings to date.
void fn_801176C0(TourSeason* pTour) {
    int i;

    fn_80005AE8(pTour, 0, sizeof(*pTour));
    for (i = 0; i < 31; i++) {
        strcpy(pTour->aEvent[i].szChampName, GameModeDriverPGATour_GetInitialChampName(i));
        pTour->aEvent[i].nChampScore = GameModeDriverPGATour_GetInitialChampScore(i);
    }
    for (i = 0; i < PGA_NUM_PROS; i++) {
        pTour->aStats[i].nCareerWinnings = lbl_8024B9CC[i].nCareerWinnings;
    }
}

// A new season: every golfer's season counts cleared (up to nPlayerOfYearPoints; the consecutive
// cuts and the career totals go on).
void fn_80117860(TourSeason* pTour) {
    int i;

    for (i = 0; i < PGA_NUM_GOLFERS; i++) {
        fn_80005AE8(&pTour->aStats[i], 0, (u8*)&pTour->aStats[0].unk4B - (u8*)&pTour->aStats[0]);
    }
}

// A round of a tournament for the field (TW06: GM_PgaTourSim_SimRound). The first round also
// picks the field. uFlags: 1 the player is in the field, 2 the player's round is simulated too,
// 4 the round only starts: no scores or statistics are kept and the other entrants are put on
// random holes. 97.9%: with nPlayer an int it is exact, but GameModeDriverPGATour fn_800EF130 (linked)
// needs this prototype's s32 to match; only saved registers differ here.
void fn_801178C8(s32 nPlayer, SeasonEvent* pEvent, int nRound, int n, int uFlags) {
    s32 nEntrants;
    int i;
    PgaEntrant* pEntrant;
    s32 nHole;

    fn_80005AE8(&lbl_80223C70, 0, sizeof(lbl_80223C70));
    gbScoresDirty = 1;
    fn_80005AE8(lbl_80224070, 0, sizeof(lbl_80224070));
    if (nRound == 0) {
        pEvent->nEventPar = 0;
        pEvent->nUserBracket = fn_800EF0E0(nPlayer);
        fn_80117AF8(nPlayer);
        fn_801187F0(gpSaveData[nPlayer].tour.field.aEntrant, &gpSaveData[nPlayer].tour.field.nEntrants,
                    uFlags & 1);
        fn_80118B0C(nPlayer, n);
    }
    pEvent->nEventPar += (u16)fn_800D2FB4(gSession.nTeeSet[0]);
    nEntrants = fn_80118664(nPlayer);
    for (i = 0; i < nEntrants; i++) {
        if (!fn_801197A4(nPlayer, i) && (!fn_8011908C(nPlayer, i) || (uFlags & 2))) {
            for (nHole = 0; nHole < 18; nHole++) {
                fn_80119B54(nPlayer, nRound, i, nHole);
            }
            fn_80119E28(nPlayer, i, nRound);
        }
    }
    if (!(uFlags & 4)) {
        fn_8011A538(nPlayer);
    }
    if (!(uFlags & 4)) {
        for (i = 0; i < nEntrants; i++) {
            GetEntrantNonMCPtr(i)->nCurrentHole = 18;
        }
        fn_80117D80(nPlayer);
        fn_80117DF0(nPlayer);
    }
    if (nRound == 1 && !(uFlags & 4)) {
        fn_80117B58(nPlayer);
    }
    if (uFlags & 4) {
        pEntrant = GetEntrantNonMCPtr(0);
        pEntrant->nCurrentHole = 0;
        for (i = 1; i < nEntrants; i++) {
            pEntrant = GetEntrantNonMCPtr(i);
            pEntrant->nCurrentHole = Rand_Next(0) % 18;
        }
    }
}

// A new tournament: the field emptied, no winner yet.
void fn_80117AF8(int nPlayer) {
    fn_80005AE8(&gpSaveData[nPlayer].tour.field, 0, sizeof(PgaField));
    gpSaveData[nPlayer].tour.field.nWinner = -1;
}

// The cut: the entrants above the cut row make it, the rest are out. TW06: GM_PgaTourSim_CutBadGolfers.
void fn_80117B58(int nPlayer) {
    s32 i;
    s32 nEntrants = fn_80118664(nPlayer);
    s32 nCutRow = fn_80119A50(nPlayer);

    for (i = 0; i < nCutRow; i++) {
        fn_80117BF4(nPlayer, lbl_80223C70.aEntrant[i]);
    }
    for (i = nCutRow; i < nEntrants; i++) {
        fn_80117C50(nPlayer, lbl_80223C70.aEntrant[i]);
    }
}

// An entrant made the cut. TW06: GM_PgaTourSim_PassEntrant.
void fn_80117BF4(int nPlayer, int nEntrant) {
    PgaEntrantMC* pEntrantMC = GetEntrantMCPtr(nPlayer, nEntrant);
    PgaStatCounts* pStats = &gpSaveData[nPlayer].tour.aStats[pEntrantMC->nGolfer];

    pStats->nConsecutiveCuts++;
}

// An entrant missed the cut. TW06: GM_PgaTourSim_CutEntrant.
void fn_80117C50(int nPlayer, int nEntrant) {
    PgaEntrantMC* pEntrantMC = GetEntrantMCPtr(nPlayer, nEntrant);
    PgaStatCounts* pStats = &gpSaveData[nPlayer].tour.aStats[pEntrantMC->nGolfer];

    pEntrantMC->bWasCut = 1;
    pStats->nConsecutiveCuts = 0;
    gbScoresDirty = 1;
}

// The entrant's strokes on the holes played go down as the current round's score.
void fn_80117CB8(int nPlayer, int nEntrant) {
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

// Every entrant's round score. TW06: GM_PgaTourSim_CommitRoundScores.
void fn_80117D80(int nPlayer) {
    s32 nEntrants = fn_80118664(nPlayer);
    s32 i;

    for (i = 0; i < nEntrants; i++) {
        fn_80117CB8(nPlayer, i);
    }
}

u8 fn_80117DE0(void) {
    return lbl_80282500;
}

void fn_80117DE8(int nPlayer, u8 b) {
    lbl_80282500 = b;
}

// Every entrant back to the first tee with no strokes. TW06: GM_PgaTourSim_ResetHoleScores.
void fn_80117DF0(int nPlayer) {
    s32 nEntrants = fn_80118664(nPlayer);
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

// The tournament is over. The winner is the entrant in first place: the player if the player is
// in the playoff, else one of the playoff entrants at random. The winner's wins and Player of the
// Year points are counted, the prize money paid out, and the player's awards given (fn_801180C4).
void fn_80117E98(int nPlayer) {
    s32 nEntrants = fn_80118664(nPlayer);
    u8 bUser;
    u8 bFirst;
    s32 nPlayoff;
    s32 nPick;
    s32 nWinner;
    s32 i;
    PgaEntrantMC* pWinner;
    s32 nFirstPrize;
    s32 nPurse;

    CalcScoreRankingsIfDirty(nPlayer);
    bUser = fn_8011908C(nPlayer, 0);
    bFirst = lbl_80223C70.aRank[0] == 1;
    nPlayoff = fn_8011A684(nPlayer);
    if (fn_8011A6F4(nPlayer, 0)) {
        nWinner = 0;
    } else {
        nPick = Rand_Next(0) % nPlayoff;
        for (i = 0; i < nEntrants; i++) {
            if (fn_8011A6F4(nPlayer, i)) {
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
    if (fn_800EFA70(gpSaveData[nPlayer].tour.nEvent)->nC != 0) {
        gpSaveData[nPlayer].tour.aStats[pWinner->nGolfer].nPlayerOfYearPoints += 3;
    }
    nFirstPrize = GameModeDriverPGATour_ComputePurseForBracket(gpSaveData[nPlayer].tour.nEvent,
                                                                    fn_800EF0E0(nPlayer));
    nPurse = GameModeDriverPGATour_ComputeFirstPrizeForBracket(gpSaveData[nPlayer].tour.nEvent,
                                                          fn_800EF0E0(nPlayer));
    fn_8011BAD4(nPlayer, nFirstPrize, nPurse);
    fn_801180C4(nPlayer, bUser, bFirst);
}

// The player's awards after a tournament. bUser: the player played it; bFirst: the player's
// entrant finished first. Career winnings first on the tour, in the top 5 or in the top 25 win
// a200's awards. After the last tournament of the season: the four trophies (a1C0[12..15]: a
// first season with two wins or more, and leading the Player of the Year points, the season's
// winnings or, with 15 tournaments, the scoring average). When a month ends, leading every pro's
// winnings for the month wins that month's award, and the month's winnings start again.
void fn_801180C4(int nPlayer, u8 bUser, u8 bFirst) {
    SaveProfile* pProfile = &gpSaveData[nPlayer];
    s32 nAhead;
    s32 i;
    s32 nNext;

    if (bFirst && fn_800EFA70(gpSaveData[nPlayer].tour.nEvent)->nC != 0) {
        pProfile->tour.n4E9A++;
    }
    if (bUser && bFirst) {
        pProfile->tour.n4E96++;
    } else if (bUser && !bFirst) {
        pProfile->tour.n4E96 = 0;
    }

    nAhead = 0;
    for (i = 0; i < PGA_NUM_PROS; i++) {
        if (gpSaveData[nPlayer].tour.aStats[i].nCareerWinnings >
            gpSaveData[nPlayer].tour.aStats[PGA_USER_GOLFER].nCareerWinnings) {
            nAhead++;
        }
    }
    if (nAhead == 0) {
        if (fn_800D7770(nPlayer, &gpSaveData[nPlayer].a200[0])) {
            fn_8011C054(1, 1);
        }
        fn_800D7770(nPlayer, &gpSaveData[nPlayer].a200[1]);
        fn_800D7770(nPlayer, &gpSaveData[nPlayer].a200[2]);
    } else if (nAhead <= 4) {
        if (fn_800D7770(nPlayer, &gpSaveData[nPlayer].a200[1])) {
            fn_8011C054(3, 1);
        }
        fn_800D7770(nPlayer, &gpSaveData[nPlayer].a200[2]);
    } else if (nAhead <= 24) {
        if (fn_800D7770(nPlayer, &gpSaveData[nPlayer].a200[2])) {
            fn_8011C054(4, 1);
        }
    }

    nNext = GameModeDriverPGATour_GetNextEvent();
    if (nNext == -1) {
        if (gpSaveData[nPlayer].tour.nSeason == 0
            && gpSaveData[nPlayer].tour.aStats[PGA_USER_GOLFER].nSeasonWins > 1
            && fn_800D7770(nPlayer, &gpSaveData[nPlayer].a1C0[12])) {
            fn_8011C054(12, 2);
        }
        if (fn_80118F60(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_PLAYER_OF_YEAR_POINTS)
            && fn_800D7770(nPlayer, &gpSaveData[nPlayer].a1C0[13])) {
            fn_8011C054(11, 1);
        }
        if (fn_80118F60(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_SEASON_WINNINGS)
            && fn_800D7770(nPlayer, &gpSaveData[nPlayer].a1C0[14])) {
            if ((Rand_Next(0) & 1) == 0) {
                fn_8011C054(5, 1);
            } else {
                fn_8011C054(2, 1);
            }
        }
        if (pProfile->tour.aStats[PGA_USER_GOLFER].nEvents >= 15
            && fn_80118F60(nPlayer, PGA_USER_GOLFER, GM_PGA_STAT_SCORING)
            && fn_800D7770(nPlayer, &gpSaveData[nPlayer].a1C0[15])) {
            fn_8011C054(6, 2);
        }
    }

    if (nNext == -1
        || CalDate_GetMonth(GameModeDriverPGATour_GetEndDate(nNext))
               != CalDate_GetMonth(GameModeDriverPGATour_GetEndDate(gpSaveData[nPlayer].tour.nEvent))) {
        for (i = 0; i < PGA_NUM_PROS; i++) {
            if (pProfile->tour.aStats[PGA_USER_GOLFER].n44 < gpSaveData[nPlayer].tour.aStats[i].n44) {
                break;
            }
        }
        if (i == PGA_NUM_PROS
            && fn_800D7770(nPlayer, &gpSaveData[nPlayer].a1C0[CalDate_GetMonth(GameModeDriverPGATour_GetEndDate(
                                        gpSaveData[nPlayer].tour.nEvent)) - 1])) {
            fn_8011C054(14, 6);
        }
        for (i = 0; i < PGA_NUM_GOLFERS; i++) {
            gpSaveData[nPlayer].tour.aStats[i].n44 = 0;
        }
    }
}

// The size of the field. TW06: GM_PgaTourSim_GetNumEntrants.
s32 fn_80118664(int nPlayer) {
    return gpSaveData[nPlayer].tour.field.nEntrants;
}

// The number of entrants in first place: the leading rows of the score order that were not cut
// and are placed first. TW06: GM_PgaTourSim_GetNumFirstPlaceEntrants.
s32 fn_80118684(int nPlayer) {
    s32 nEntrants = fn_80118664(nPlayer);
    s32 i;
    s32 nEntrant;
    s32 nCount = 0;

    for (i = 0; i < nEntrants; i++) {
        nEntrant = lbl_80223C70.aEntrant[i];
        if (!fn_801197A4(nPlayer, nEntrant)) {
            if (lbl_80223C70.aRank[nEntrant] != 1) {
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

    switch (lbl_801940F0[nStat]) {
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
        sprintf(szFormat, "%%.%df", lbl_801940F0[nStat]);
        sprintf(szOut, szFormat, fValue);
        break;
    }
}

// Picks a tournament's field: 100 to 127 of the pros in random order (a shuffle of all of them),
// the first one replaced by the player's golfer when the player plays; the rest of the table empty.
void fn_801187F0(PgaEntrantMC* aEntrant, s16* pnEntrants, u8 bUser) {
    s32 aGolfer[PGA_NUM_PROS];
    int i;
    s32 j;
    s32 nSwap;

    for (i = 0; i < PGA_NUM_PROS; i++) {
        aGolfer[i] = i;
    }
    for (i = 0; i < PGA_NUM_PROS - 1; i++) {
        j = i + Rand_Next(0) % (PGA_NUM_PROS - i);
        if (j != i) {
            nSwap = aGolfer[j];
            aGolfer[j] = aGolfer[i];
            aGolfer[i] = nSwap;
        }
    }
    *pnEntrants = Rand_Next(0) % 28 + 100;
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

// A sort comparison for entrants: by their pro's f50, smallest first; the player's golfer last.
s32 fn_80118A5C(const void* pA, const void* pB) {
    s32 nEntrantB = *(const s32*)pB;
    s32 nGolfer;
    f32 fA;
    f32 fB;

    nGolfer = fn_80119118(0, *(const s32*)pA);
    if (nGolfer == PGA_USER_GOLFER) {
        fA = 10000.0f;
    } else {
        fA = lbl_8024B9CC[nGolfer].f50;
    }
    nGolfer = fn_80119118(0, nEntrantB);
    if (nGolfer == PGA_USER_GOLFER) {
        fB = 10000.0f;
    } else {
        fB = lbl_8024B9CC[nGolfer].f50;
    }
    if (fA < fB) {
        return -1;
    }
    return fA > fB;
}

// Gives each entrant the four-round total the simulation aims at. The entrants are ordered by
// their pro's form (fn_80118A5C) and shuffled a little (each may swap with one a few rows down;
// the player is never swapped in); the targets are random around the course's par for four
// rounds plus n + 18 (within par + n .. par + n + 25), sorted, the best one at most par + n + 3,
// and handed out in that order. The player gets the best target.
void fn_80118B0C(int nPlayer, int n) {
    s32 aOrder[PGA_MAX_ENTRANTS];
    s32 aTarget[PGA_MAX_ENTRANTS];
    PgaEntrantMC* pEntrantMC;
    s32 nEntrants;
    s32 nPar;
    s32 nTarget;
    s32 nEntrant;
    s32 i;
    s32 j;

    nEntrants = fn_80118664(nPlayer);
    for (i = 0; i < nEntrants; i++) {
        aOrder[i] = i;
    }
    for (i = nEntrants; i < PGA_MAX_ENTRANTS; i++) {
        aOrder[i] = -1;
    }
    qsort(aOrder, nEntrants, sizeof(aOrder[0]), fn_80118A5C);
    for (i = 0; i < nEntrants - 1; i++) {
        j = i + (s32)fabsf(3.0f * fn_8000B318(0));
        j = (j <= nEntrants - 1) ? j : nEntrants - 1;
        if (j != i && !fn_8011908C(nPlayer, aOrder[j])) {
            nEntrant = aOrder[j];
            aOrder[j] = aOrder[i];
            aOrder[i] = nEntrant;
        }
    }
    nPar = n + fn_800D2FB4(gSession.nTeeSet[0]) * 4;
    for (i = 0; i < nEntrants; i++) {
        nTarget = 8.0f * fn_8000B318(0) + (18.0f + nPar);
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
    if (fn_8011908C(nPlayer, 0)) {
        pEntrantMC = GetEntrantMCPtr(nPlayer, 0);
        pEntrantMC->nTargetScore = aTarget[0];
    }
}

// The golfer's name: a tour pro's, or the player's profile name.
char* fn_80118E30(int nPlayer, int nGolfer) {
    if (nGolfer == PGA_USER_GOLFER) {
        return gpSaveData[nPlayer].szName;
    }
    return lbl_8024B9CC[nGolfer].szName;
}

s32 GM_PgaTourSim_GetStatRankFromGolferID(int nPlayer, int nGolfer, GM_Pga_StatTypes_t nStat) {
    CalcAllStatsIfDirty(nPlayer);
    return lbl_80226870[nStat].aRank[nGolfer];
}

f32 GM_PgaTourSim_GetStatValueFromGolferID(int nPlayer, int nGolfer, GM_Pga_StatTypes_t nStat) {
    CalcAllStatsIfDirty(nPlayer);
    return lbl_80226870[nStat].aValue[nGolfer].fValue;
}

s32 GM_PgaTourSim_GetGolferIDFromStatRow(int nPlayer, GM_Pga_StatTypes_t nStat, int nRow) {
    CalcAllStatsIfDirty(nPlayer);
    return lbl_80226870[nStat].aGolfer[nRow];
}

// Whether no other tour golfer beats the golfer's value of a statistic (ties allowed).
u8 fn_80118F60(int nPlayer, int nGolfer, GM_Pga_StatTypes_t nStat) {
    f32 fValue;
    s32 i;

    CalcAllStatsIfDirty(nPlayer);
    fValue = lbl_80226870[nStat].aValue[nGolfer].fValue;
    for (i = 0; i < PGA_NUM_GOLFERS; i++) {
        if (lbl_80193FF8[nStat] == fn_8011BCFC) {
            if (i != nGolfer && lbl_80226870[nStat].aValue[i].fValue > fValue) {
                return 0;
            }
        } else {
            if (i != nGolfer && lbl_80226870[nStat].aValue[i].fValue < fValue) {
                return 0;
            }
        }
    }
    return 1;
}

s32 GM_PgaTourSim_GetStatView(GM_Pga_StatTypes_t nStat) {
    return lbl_80194074[nStat];
}

s32 fn_8011903C(int nPlayer, int nGolfer) {
    return gpSaveData[nPlayer].tour.aStats[nGolfer].nEvents;
}

s32 fn_80119064(int nPlayer, int nGolfer) {
    return gpSaveData[nPlayer].tour.aStats[nGolfer].nRounds;
}

// Entrant 0 is the player's slot; it is the player when it holds the player's golfer.
u8 fn_8011908C(int nPlayer, int nEntrant) {
    PgaEntrantMC* pEntrant = GetEntrantMCPtr(nPlayer, nEntrant);
    int bUser = 0;

    if (nEntrant == 0 && pEntrant->nGolfer == PGA_USER_GOLFER) {
        bUser = 1;
    }
    return bUser;
}

s32 fn_801190D8(int nPlayer, int nEntrant) {
    CalcScoreRankingsIfDirty(nPlayer);
    return lbl_80223C70.aRank[nEntrant];
}

s32 fn_80119118(int nPlayer, int nEntrant) {
    return GetEntrantMCPtr(nPlayer, nEntrant)->nGolfer;
}

// The entrant's current hole, one on for the player once finished with it, one back for the
// others while not, at most 18.
s32 fn_8011913C(int nPlayer, int nEntrant, u8 b) {
    s32 nHole;
    s32 nRet;

    nHole = GetEntrantNonMCPtr(nEntrant)->nCurrentHole;
    if (fn_8011908C(nPlayer, nEntrant)) {
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

// The entrant's strokes in the tournament so far: every round played plus the holes of the
// current one (in game type 3 before the first round, all four rounds; b as for fn_8011913C).
s32 fn_801191D0(int nPlayer, int nEntrant, u8 b) {
    PgaEntrant* pEntrant;
    s32 nStrokes;
    s32 nHoles;
    s32 i;

    GetEntrantMCPtr(nPlayer, nEntrant);
    pEntrant = GetEntrantNonMCPtr(nEntrant);
    nStrokes = 0;
    if (gSession.nGameType == 3 && gpSaveData[nPlayer].tour.nRound == 0) {
        for (i = 0; i < 4; i++) {
            nStrokes += fn_80119638(nPlayer, nEntrant, i);
        }
    } else {
        for (i = 0; i < gpSaveData[nPlayer].tour.nRound; i++) {
            nStrokes += fn_80119638(nPlayer, nEntrant, i);
        }
        if (gpSaveData[nPlayer].tour.nRound < 4) {
            nHoles = fn_8011913C(nPlayer, nEntrant, b);
            for (i = 0; i < nHoles; i++) {
                nStrokes += pEntrant->aHoleStrokes[i];
            }
        }
    }
    return nStrokes;
}

// The entrant's score to par so far (fn_801191D0's strokes less the par of the rounds and holes
// played). A built round (course 22) and courses 24..29 take each hole's course and number from
// fn_800D3118/fn_800D315C.
int fn_8011937C(int nPlayer, int nEntrant, u8 b) {
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
    GameModeDriverPGATour_GetCourses(fn_800EFA70(nEvent), aCourses);
    GetEntrantNonMCPtr(nEntrant);
    nScore = fn_801191D0(nPlayer, nEntrant, b);
    if (gSession.nGameType == 3 && gpSaveData[nPlayer].tour.nRound == 0) {
        for (i = 0; i < 4; i++) {
            nScore -= fn_800D2F00(aCourses[i], 0);
        }
    } else {
        for (i = 0; i < gpSaveData[nPlayer].tour.nRound; i++) {
            nScore -= fn_800D2F00(aCourses[i], 0);
        }
        if (gpSaveData[nPlayer].tour.nRound < 4) {
            nHoles = fn_8011913C(nPlayer, nEntrant, b);
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

// The best score among the entrants still in the field other than the player (b as for
// fn_8011937C).
s32 fn_80119588(int nPlayer, u8 b) {
    s32 nBest = 0x7FFFFFFF;
    s32 nEntrants = fn_80118664(nPlayer);
    s32 i;

    for (i = 0; i < nEntrants; i++) {
        if (!fn_8011908C(nPlayer, i) && !fn_801197A4(nPlayer, i)) {
            nBest = (nBest <= fn_8011937C(nPlayer, i, b)) ? nBest : fn_8011937C(nPlayer, i, b);
        }
    }
    return nBest;
}

// The entrant's strokes in a round: for the round being played (outside game type 3) the holes
// so far, otherwise the round's saved total.
s32 fn_80119638(int nPlayer, int nEntrant, int nRound) {
    PgaEntrantMC* pEntrantMC = GetEntrantMCPtr(nPlayer, nEntrant);
    PgaEntrant* pEntrant = GetEntrantNonMCPtr(nEntrant);
    s32 nStrokes;
    s32 i;

    if (nRound == gpSaveData[nPlayer].tour.nRound && gSession.nGameType != 3) {
        nStrokes = 0;
        // The (s16) is in the original (an extsh before each add), as in fn_80117CB8.
        for (i = 0; i < pEntrant->nCurrentHole; i++) {
            nStrokes += (s16)pEntrant->aHoleStrokes[i];
        }
    } else {
        nStrokes = pEntrantMC->aRoundStrokes[nRound];
    }
    return nStrokes;
}

u8 fn_801197A4(int nPlayer, int nEntrant) {
    return GetEntrantMCPtr(nPlayer, nEntrant)->bWasCut;
}

s32 fn_801197CC(int nPlayer, int nRow) {
    CalcScoreRankingsIfDirty(nPlayer);
    return lbl_80223C70.aEntrant[nRow];
}

// Whether another entrant holds the same place as the entrant (a tie).
u8 fn_80119808(int nPlayer, int nEntrant) {
    s32 nEntrants = fn_80118664(nPlayer);
    s32 nRank = fn_801190D8(nPlayer, nEntrant);
    s32 i;

    for (i = 0; i < nEntrants; i++) {
        if (i != nEntrant && lbl_80223C70.aRank[i] == nRank) {
            return 1;
        }
    }
    return 0;
}

// The player's strokes on the current hole, or in the playoff.
void fn_8011989C(int nPlayer, int nStrokes) {
    PgaEntrant* pUser = GetEntrantNonMCPtr(0);

    if (pUser->bInPlayoff) {
        lbl_80282504 = nStrokes;
    } else {
        pUser->aHoleStrokes[pUser->nCurrentHole] = nStrokes;
        gbScoresDirty = 1;
    }
}

void fn_801198F8(int nPlayer, int nHole) {
    GetEntrantNonMCPtr(0)->nCurrentHole = nHole;
    gbScoresDirty = 1;
}

// Moves the other entrants on 0 to 2 holes, keeping them at least one hole ahead of the player
// and at most on the 18th.
void fn_80119934(int nPlayer) {
    s32 nEntrants;
    PgaEntrant* pEntrant;
    s32 nMinHole;
    s32 i;

    nEntrants = fn_80118664(nPlayer);
    pEntrant = GetEntrantNonMCPtr(0);
    GetEntrantMCPtr(nPlayer, 0);
    nMinHole = pEntrant->nCurrentHole;
    nMinHole++;                 // fake match: one statement, "+ 1", swaps the saved registers
    for (i = 1; i < nEntrants; i++) {
        pEntrant = GetEntrantNonMCPtr(i);
        pEntrant->nCurrentHole += Rand_Next(0) % 3;
        pEntrant->nCurrentHole = pEntrant->nCurrentHole <= nMinHole ? nMinHole : pEntrant->nCurrentHole;
        pEntrant->nCurrentHole = 18 < pEntrant->nCurrentHole ? 18 : pEntrant->nCurrentHole;
    }
    gbScoresDirty = 1;
}

s32 fn_80119A04(int nPlayer, int nEntrant) {
    return GetEntrantNonMCPtr(nEntrant)->nCurrentHole;
}

s32 fn_80119A2C(int nPlayer, int nEntrant) {
    return GetEntrantMCPtr(nPlayer, nEntrant)->n18;
}

// The cut: the first score row placed below 70th (the top 70 and ties play on), else the last row.
s32 fn_80119A50(int nPlayer) {
    s32 nEntrants = fn_80118664(nPlayer);
    s32 nRow = nEntrants - 1;
    s32 i;

    CalcScoreRankingsIfDirty(nPlayer);
    for (i = 0; i < nEntrants; i++) {
        if (lbl_80223C70.aRank[lbl_80223C70.aEntrant[i]] > 70) {
            nRow = i;
            break;
        }
    }
    return nRow;
}

// The first score row holding an entrant who was cut, or -1.
s32 fn_80119AE0(int nPlayer) {
    s32 nEntrants = fn_80118664(nPlayer);
    s32 nRow = -1;
    s32 i;

    for (i = 0; i < nEntrants; i++) {
        if (fn_801197A4(nPlayer, lbl_80223C70.aEntrant[i])) {
            nRow = i;
            break;
        }
    }
    return nRow;
}

// Simulates an entrant's strokes on a hole: the pro's scoring average for the hole's par (the
// player's entrant uses the first pro's), scaled so the round's holes add up to a quarter of the
// entrant's target score, plus a normal random spread that grows with the par; 1 to 10 strokes.
void fn_80119B54(int nPlayer, int nRound, int nEntrant, int nHole) {
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

    if (fn_8011908C(nPlayer, nEntrant)) {
        pPro = &lbl_8024B9CC[0];
    } else {
        pPro = &lbl_8024B9CC[pEntrantMC->nGolfer];
    }
    nPar = fn_800D2AD8(nHole);
    fRound = pPro->fPar3Avg * fn_800D31A4(3) + pPro->fPar4Avg * fn_800D31A4(4)
           + pPro->fPar5Avg * fn_800D31A4(5);
    fScale = 0.25f * pEntrantMC->nTargetScore / fRound;
    fPar3 = fScale * pPro->fPar3Avg;
    fPar4 = fScale * pPro->fPar4Avg;
    fPar5 = fScale * pPro->fPar5Avg;
    switch (nPar) {
    case 3:
        fStrokes = 0.5f * fn_8000B318(0) + fPar3;
        break;
    case 4:
        fStrokes = 0.6f * fn_8000B318(0) + fPar4;
        break;
    case 5:
        fStrokes = 0.7f * fn_8000B318(0) + fPar5;
        break;
    }
    // EA bug: fStrokes is never set on a hole whose par is not 3, 4 or 5.
    nStrokes = (s32)(0.5f + fStrokes);
    nStrokes = nStrokes > 1 ? nStrokes : 1;
    pEntrant->aHoleStrokes[nHole] = nStrokes > 10 ? 10 : nStrokes;
}

s32 TotalEntrantHoleScores(int nEntrant) {
    PgaEntrant* pEntrant = GetEntrantNonMCPtr(nEntrant);
    s32 nTotal = 0;
    int i;

    for (i = 0; i < 18; i++) {
        nTotal += pEntrant->aHoleStrokes[i];
    }
    return nTotal;
}

// Brings an entrant's simulated round to its target: a quarter of the entrant's target score,
// give or take up to 3 strokes at random, in the first three rounds; in the last, what is left of
// the target. Strokes come off random holes (a par-like 2 only rarely) or go on them (a hole
// already at 6 or more only rarely) until the round adds up.
void fn_80119E28(int nPlayer, int nEntrant, int nRound) {
    PgaEntrantMC* pEntrantMC = GetEntrantMCPtr(nPlayer, nEntrant);
    PgaEntrant* pEntrant = GetEntrantNonMCPtr(nEntrant);
    s32 nTarget;
    f32 fTarget;
    f32 fScore;

    gbStatsDirty = 1;
    gbScoresDirty = 1;
    if (nRound < 3) {
        fTarget = 0.25f * pEntrantMC->nTargetScore;
        fScore = fTarget + fn_8000B318(0);
        fScore = fScore <= fTarget - 3.0f ? fTarget - 3.0f : fScore;
        fScore = fScore <= 3.0f + fTarget ? fScore : 3.0f + fTarget;
        nTarget = (s32)(0.5f + fScore);
    } else {
        nTarget = pEntrantMC->nTargetScore - fn_801191D0(nPlayer, nEntrant, 0);
    }
    while (TotalEntrantHoleScores(nEntrant) > nTarget) {
        s32 nHole = Rand_Next(0) % 18;
        u8 bDone = 0;

        while (!bDone) {
            if (pEntrant->aHoleStrokes[nHole] > 1) {
                if (pEntrant->aHoleStrokes[nHole] == 2) {
                    if (Rand_Float(0) < 0.0005f) {
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
        s32 nHole = Rand_Next(0) % 18;
        u8 bDone = 0;

        while (!bDone) {
            if (pEntrant->aHoleStrokes[nHole] < 10) {
                if (pEntrant->aHoleStrokes[nHole] >= 6) {
                    if (Rand_Float(0) < 0.01f) {
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

// One hole of a simulated round counted in the entrant's season statistics: the score against
// par, and a green in regulation, a drive, the putts, a fairway and a bunker save made up from
// the pro's season form (the player's entrant uses the first pro's).
void fn_8011A074(int nPlayer, int nRound, int nEntrant, int nHole) {
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
        pPro = &lbl_8024B9CC[pEntrantMC->nGolfer];
    } else {
        pPro = &lbl_8024B9CC[0];
    }
    nPar = fn_800D2AD8(nHole);
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
    if (nHole > 0 && nStrokes < nPar && pEntrant->aHoleStrokes[nHole - 1] > fn_800D2AD8(nHole - 1)) {
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
    if (Rand_Float(0) * 100.0f < pPro->fGIRPct) {
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
    fRandom = 30.0f * fn_8000B318(0) + pPro->fDriveAvg;
    fDrive = (fRandom > 0.0f) ? fRandom : 0.0f;
    if (fDrive > fn_800D2C30(nHole, 0)) {
        fDrive = fn_800D2C30(nHole, 0);
    }
    if (fDrive > 520.0f) {
        fDrive -= 50.0f * Rand_Float(0);
    }
    nDrive = fDrive;
    pStats->nLongestDrive = ((u16)nDrive <= pStats->nLongestDrive) ? pStats->nLongestDrive : nDrive;
    if (fn_800D3080(nHole)) {
        pStats->nDrives++;
        pStats->nDriveDistance += (u16)nDrive;
    }

    // The putts: the pro's average per hole, never more than the strokes less one.
    nPutts = 0.5f + (0.3f * fn_8000B318(0) + pPro->fPuttAvg / 18.0f);
    if (nPutts <= 0) {
        if (Rand_Float(0) < 0.05f) {
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
        if (Rand_Float(0) * 100.0f < pPro->fFairwayPct) {
            bHit = 1;
        }
        pStats->nFairwaysHit += bHit;
        pStats->nFairways++;
    }

    // A bunker on one hole in ten not under par, saved fSandSavePct percent of the time.
    if (nStrokes >= nPar && Rand_Float(0) < 0.1f) {
        bHit = 0;
        if (Rand_Float(0) * 100.0f < pPro->fSandSavePct) {
            bHit = 1;
        }
        pStats->nBunkerSaves += bHit;
        pStats->nBunkers++;
    }
}

// The season statistics of the round for every CPU entrant still playing (everyone in the first
// two rounds, then those who made the cut), hole by hole. TW06: GM_PgaTourSim_SimStats.
void fn_8011A538(int nPlayer) {
    s32 nEntrants = fn_80118664(nPlayer);
    int i;
    s32 nHole;

    for (i = 0; i < nEntrants; i++) {
        if (!fn_8011908C(nPlayer, i)
            && (gpSaveData[nPlayer].tour.nRound <= 1 || !fn_801197A4(nPlayer, i))) {
            for (nHole = 0; nHole < 18; nHole++) {
                fn_8011A074(nPlayer, gpSaveData[nPlayer].tour.nRound, i, nHole);
            }
        }
    }
}

// Starts a playoff between the entrants tied first. TW06: GM_PgaTourSim_InitPlayoff.
void fn_8011A5F8(int nPlayer) {
    s32 nEntrants = fn_80118664(nPlayer);
    s32 i;
    PgaEntrant* pEntrant;

    for (i = 0; i < nEntrants; i++) {
        pEntrant = GetEntrantNonMCPtr(i);
        pEntrant->bInPlayoff = fn_801190D8(nPlayer, i) == 1;
    }
    lbl_80282504 = 0;
}

s32 fn_8011A684(int nPlayer) {
    s32 nEntrants;
    s32 i;
    s32 nCount = 0;

    nEntrants = fn_80118664(nPlayer);
    for (i = 0; i < nEntrants; i++) {
        if (GetEntrantNonMCPtr(i)->bInPlayoff) {
            nCount++;
        }
    }
    return nCount;
}

u8 fn_8011A6F4(int nPlayer, int nEntrant) {
    return GetEntrantNonMCPtr(nEntrant)->bInPlayoff;
}

// A playoff hole played: an entrant who beat the player's strokes knocks the player out, and one
// who took more drops out. TW06: GM_PgaTourSim_UpdatePlayoffs.
void fn_8011A720(int nPlayer, int nHole) {
    s32 nEntrants = fn_80118664(nPlayer);
    s32 i;
    PgaEntrant* pEntrant;
    PgaEntrant* pUser;

    for (i = 1; i < nEntrants; i++) {
        pEntrant = GetEntrantNonMCPtr(i);
        if (pEntrant->bInPlayoff) {
            if (pEntrant->aHoleStrokes[nHole] < lbl_80282504) {
                pUser = GetEntrantNonMCPtr(0);
                pUser->bInPlayoff = 0;
                return;
            }
            if (pEntrant->aHoleStrokes[nHole] > lbl_80282504) {
                pEntrant->bInPlayoff = 0;
            }
        }
    }
}

// The fewest strokes on a playoff hole among the player's opponents still in it (999: none).
// TW06: GM_PgaTourSim_GetBestOpponentPlayoffHoleScore.
s32 fn_8011A7C8(int nPlayer, int nHole) {
    s32 i;
    s32 nBest = 999;
    s32 nEntrants = fn_80118664(nPlayer);
    PgaEntrant* pEntrant;

    for (i = 1; i < nEntrants; i++) {
        pEntrant = GetEntrantNonMCPtr(i);
        if (pEntrant->bInPlayoff) {
            nBest = nBest <= pEntrant->aHoleStrokes[nHole] ? nBest : pEntrant->aHoleStrokes[nHole];
        }
    }
    return nBest;
}

void CalcAllStatsIfDirty(int nPlayer) {
    if (gbStatsDirty) {
        CalcAllStats(nPlayer);
        gbStatsDirty = 0;
    }
}

// The score order: every entrant sorted by score (fn_8011BDF8), then each given a place, entrants
// with the same score sharing it. A cut entrant counts as the worst score, the winner as the best.
void fn_8011A890(int nPlayer) {
    s32 i;
    s32 nEntrant;
    s32 nScore;
    s32 nRank;
    s32 nPrevScore;
    s32 nEntrants = fn_80118664(nPlayer);

    if (nEntrants == 0) return;
    for (i = 0; i < nEntrants; i++) {
        lbl_80223C70.aEntrant[i] = i;
    }
    for (i = nEntrants; i < PGA_MAX_ENTRANTS; i++) {
        lbl_80223C70.aEntrant[i] = -1;
    }
    lbl_80281848 = nPlayer;
    qsort(lbl_80223C70.aEntrant, nEntrants, sizeof(lbl_80223C70.aEntrant[0]), fn_8011BDF8);
    lbl_80281848 = 0;
    nPrevScore = 0;
    nRank = 0;
    for (i = 0; i < nEntrants; i++) {
        nEntrant = lbl_80223C70.aEntrant[i];
        if (fn_801197A4(nPlayer, nEntrant)) {
            nScore = PGA_SCORE_CUT;
        } else if (nEntrant == gpSaveData[nPlayer].tour.field.nWinner) {
            nScore = PGA_SCORE_WINNER;
        } else {
            nScore = fn_8011937C(nPlayer, nEntrant, !fn_8011908C(nPlayer, nEntrant));
        }
        if (i == 0 || nPrevScore != nScore) {
            nRank = i + 1;
            nPrevScore = nScore;
        }
        lbl_80223C70.aRank[nEntrant] = nRank;
    }
}

// The same after the cut: the cut entrants, from the first cut row on, sorted among themselves
// (fn_8011BF74), then every place worked out again.
void fn_8011AAC0(int nPlayer) {
    s32 nEntrants = fn_80118664(nPlayer);
    s32 nCutRow;
    s32 i;
    s32 nEntrant;
    s32 nScore;
    s32 nRank;
    s32 nPrevScore;

    if (nEntrants == 0) return;
    lbl_80281848 = nPlayer;
    nCutRow = fn_80119AE0(nPlayer);
    if (nCutRow != -1) {
        qsort(&lbl_80223C70.aEntrant[nCutRow], nEntrants - nCutRow, sizeof(lbl_80223C70.aEntrant[0]),
              fn_8011BF74);
    }
    lbl_80281848 = 0;
    nPrevScore = 0;
    nRank = 0;
    for (i = 0; i < nEntrants; i++) {
        nEntrant = lbl_80223C70.aEntrant[i];
        if (fn_801197A4(nPlayer, nEntrant)) {
            nScore = PGA_SCORE_CUT;
        } else if (nEntrant == gpSaveData[nPlayer].tour.field.nWinner) {
            nScore = PGA_SCORE_WINNER;
        } else {
            nScore = fn_8011937C(nPlayer, nEntrant, !fn_8011908C(nPlayer, nEntrant));
        }
        if (i == 0 || nPrevScore != nScore) {
            nRank = i + 1;
            nPrevScore = nScore;
        }
        lbl_80223C70.aRank[nEntrant] = nRank;
    }
}

void CalcScoreRankingsIfDirty(int nPlayer) {
    if (gbScoresDirty) {
        fn_8011A890(nPlayer);
        gbScoresDirty = 0;
        fn_8011AAC0(nPlayer);
    }
}

// Ranks every tour golfer in a statistic: sorts them with the statistic's comparison, then gives
// each its place; golfers whose values print the same share the place.
void fn_8011AC40(int nPlayer, GM_Pga_StatTypes_t nStat) {
    PgaStatRanking* pRanking = &lbl_80226870[nStat];
    char* szValue;
    s32 nRow;
    s32 nGolfer;
    s32 nRank;
    char szPrev[16];
    int i;

    for (i = 0; i < PGA_NUM_GOLFERS; i++) {
        pRanking->aGolfer[i] = i;
    }
    lbl_80281840.nPlayer = nPlayer;
    lbl_80281840.nStat = nStat;
    qsort(pRanking->aGolfer, PGA_NUM_GOLFERS, sizeof(pRanking->aGolfer[0]), lbl_80193FF8[nStat]);
    lbl_80281840.nStat = -1;
    lbl_80281840.nPlayer = 0;
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

// Works out a golfer's simple statistics from the counts in the player's profile, with their text.
void fn_8011AE1C(int nPlayer, int nGolfer) {
    GM_Pga_StatTypes_t nStat;

    for (nStat = 0; nStat < GM_PGA_STAT_SIMPLE_COUNT; nStat++) {
        lbl_80193F88[nStat](&gpSaveData[nPlayer].tour.aStats[nGolfer],
                            &lbl_80226870[nStat].aValue[nGolfer].fValue);
        GM_PgaTourSim_GetStatValString(nStat, lbl_80226870[nStat].aValue[nGolfer].fValue,
                                       lbl_80226870[nStat].aValue[nGolfer].szValue);
    }
}

void CalcAllSimpleStats(int nPlayer) {
    s32 nGolfer;

    for (nGolfer = 0; nGolfer < PGA_NUM_GOLFERS; nGolfer++) {
        fn_8011AE1C(nPlayer, nGolfer);
    }
}

void CalcAllSimpleRankings(int nPlayer) {
    s32 nStat;

    for (nStat = 0; nStat < GM_PGA_STAT_SIMPLE_COUNT; nStat++) {
        fn_8011AC40(nPlayer, nStat);
    }
}

// Works out a golfer's all-around and total driving statistics, with their text.
void fn_8011AF60(int nGolfer) {
    CalcAllAroundScore(nGolfer, &lbl_80226870[GM_PGA_STAT_ALLAROUND].aValue[nGolfer].fValue);
    CalcTotalDriving(nGolfer, &lbl_80226870[GM_PGA_STAT_TOTALDRIVING].aValue[nGolfer].fValue);
    GM_PgaTourSim_GetStatValString(GM_PGA_STAT_ALLAROUND,
                                   lbl_80226870[GM_PGA_STAT_ALLAROUND].aValue[nGolfer].fValue,
                                   lbl_80226870[GM_PGA_STAT_ALLAROUND].aValue[nGolfer].szValue);
    GM_PgaTourSim_GetStatValString(GM_PGA_STAT_TOTALDRIVING,
                                   lbl_80226870[GM_PGA_STAT_TOTALDRIVING].aValue[nGolfer].fValue,
                                   lbl_80226870[GM_PGA_STAT_TOTALDRIVING].aValue[nGolfer].szValue);
}

void CalcAllComplex1Stats(void) {
    s32 nGolfer;

    for (nGolfer = 0; nGolfer < PGA_NUM_GOLFERS; nGolfer++) {
        fn_8011AF60(nGolfer);
    }
}

void CalcAllComplex1Rankings(int nPlayer) {
    s32 nStat;

    for (nStat = GM_PGA_STAT_SIMPLE_COUNT; nStat < GM_PGA_STAT_COMPLEX1_COUNT; nStat++) {
        fn_8011AC40(nPlayer, nStat);
    }
}

// Works out a golfer's ball striking statistic, with its text.
void fn_8011B094(int nGolfer) {
    CalcBallStriking(nGolfer, &lbl_80226870[GM_PGA_STAT_BALLSTRIKING].aValue[nGolfer].fValue);
    GM_PgaTourSim_GetStatValString(GM_PGA_STAT_BALLSTRIKING,
                                   lbl_80226870[GM_PGA_STAT_BALLSTRIKING].aValue[nGolfer].fValue,
                                   lbl_80226870[GM_PGA_STAT_BALLSTRIKING].aValue[nGolfer].szValue);
}

void CalcAllComplex2Stats(void) {
    s32 nGolfer;

    for (nGolfer = 0; nGolfer < PGA_NUM_GOLFERS; nGolfer++) {
        fn_8011B094(nGolfer);
    }
}

void CalcAllComplex2Rankings(int nPlayer) {
    fn_8011AC40(nPlayer, GM_PGA_STAT_BALLSTRIKING);
}

// Each golfer's values, then each statistic's ranking; the combined statistics come after the
// rankings they add up.
void CalcAllStats(int nPlayer) {
    CalcAllSimpleStats(nPlayer);
    CalcAllSimpleRankings(nPlayer);
    CalcAllComplex1Stats();
    CalcAllComplex1Rankings(nPlayer);
    CalcAllComplex2Stats();
    CalcAllComplex2Rankings(nPlayer);
}

// The tour statistics. Each takes a golfer's season counts and puts the statistic in *pfValue;
// the table at 0x80193F88 lists them in this order, one per statistic. A statistic with nothing
// to divide by is 0 and returns 0.

u8 SafeDivide(u32 nCount, u32 nOutOf, f32* pfValue);
u8 SafeDividePct(u32 nCount, u32 nOutOf, f32* pfValue);

u8 CalcDrivingDistance(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDivide(pCounts->nDriveDistance, pCounts->nDrives, pfValue);
}

u8 SafeDivide(u32 nCount, u32 nOutOf, f32* pfValue) {
    if (nOutOf == 0) {
        *pfValue = 0.0f;
        return 0;
    }
    *pfValue = (f32)nCount / (f32)nOutOf;
    return 1;
}

u8 CalcAccuracy(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nFairwaysHit, pCounts->nFairways, pfValue);
}

// The same as a percentage.
u8 SafeDividePct(u32 nCount, u32 nOutOf, f32* pfValue) {
    if (nOutOf == 0) {
        *pfValue = 0.0f;
        return 0;
    }
    *pfValue = 100.0f * ((f32)nCount / (f32)nOutOf);
    return 1;
}

u8 CalcGIR(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nGreensHit, pCounts->nHoles, pfValue);
}

u8 CalcPuttsPerRound(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDivide(pCounts->nPutts, pCounts->nRounds, pfValue);
}

u8 CalcPuttingAvg(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDivide(pCounts->nGIRPutts, pCounts->nGreensHit, pfValue);
}

u8 CalcSandSave(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nBunkerSaves, pCounts->nBunkers, pfValue);
}

// Pars saved on the greens missed in regulation.
u8 CalcScrambling(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nNonGIRPars, pCounts->nHoles - pCounts->nGreensHit, pfValue);
}

u8 CalcBounceBack(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nBirdiesAfterBogey, pCounts->nBogeys, pfValue);
}

u8 CalcHolesPerEagle(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDivide(pCounts->nHoles, pCounts->nEagles, pfValue);
}

u8 CalcBirdieAvg(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDivide(pCounts->nBirdies, pCounts->nRounds, pfValue);
}

u8 CalcPar3BirdieAvg(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nPar3Birdies, pCounts->nPar3Holes, pfValue);
}

u8 CalcPar4BirdieAvg(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nPar4Birdies, pCounts->nPar4Holes, pfValue);
}

u8 CalcPar5BirdieAvg(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nPar5Birdies, pCounts->nPar5Holes, pfValue);
}

u8 CalcBirdieConversion(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nGIRBirdies, pCounts->nGreensHit, pfValue);
}

u8 CalcScoringAvg(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDivide(pCounts->nStrokes, pCounts->nRounds, pfValue);
}

// Holes under par.
u8 CalcParBreakers(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDividePct(pCounts->nBirdies + pCounts->nEagles, pCounts->nHoles, pfValue);
}

u8 CalcPar3ScoringAvg(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDivide(pCounts->nPar3Strokes, pCounts->nPar3Holes, pfValue);
}

u8 CalcPar4ScoringAvg(PgaStatCounts* pCounts, f32* pfValue) {
    return SafeDivide(pCounts->nPar4Strokes, pCounts->nPar4Holes, pfValue);
}

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
    return lbl_80226870[nStat].aRank[nGolfer];
}

void CalcAllAroundScore(int nGolfer, f32* pfValue) {
    *pfValue = StatRank(GM_PGA_STAT_DRIVING, nGolfer) + StatRank(GM_PGA_STAT_FAIRWAYS, nGolfer)
             + StatRank(GM_PGA_STAT_GIR, nGolfer) + StatRank(GM_PGA_STAT_PUTTING, nGolfer)
             + StatRank(GM_PGA_STAT_SAVES, nGolfer) + StatRank(GM_PGA_STAT_HOLESPEREAGLE, nGolfer)
             + StatRank(GM_PGA_STAT_BIRDIESPERROUND, nGolfer) + StatRank(GM_PGA_STAT_SCORING, nGolfer);
}

void CalcTotalDriving(int nGolfer, f32* pfValue) {
    *pfValue = StatRank(GM_PGA_STAT_DRIVING, nGolfer) + StatRank(GM_PGA_STAT_FAIRWAYS, nGolfer);
}

void CalcBallStriking(int nGolfer, f32* pfValue) {
    *pfValue = StatRank(GM_PGA_STAT_TOTALDRIVING, nGolfer) + StatRank(GM_PGA_STAT_GIR, nGolfer);
}

// Entrants tied on a place share the prizes of the rows they fill: nCount entrants from score row
// nFirstRow each get nTotal / nCount, added to their golfer's winnings.
void fn_8011B978(int nPlayer, s32 nTotal, s32 nFirstRow, s32 nCount) {
    int i;
    s32 nShare;

    if (nCount == 0) {
        return;
    }
    nShare = (f32)nTotal / (f32)nCount;
    for (i = 0; i < nCount; i++) {
        int nEntrant = lbl_80223C70.aEntrant[nFirstRow + i];
        GetEntrantMCPtr(nPlayer, nEntrant)->n18 = nShare;
        gpSaveData[nPlayer].tour.aStats[fn_80119118(nPlayer, nEntrant)].n44 += nShare;
        gpSaveData[nPlayer].tour.aStats[fn_80119118(nPlayer, nEntrant)].nSeasonWinnings += nShare;
        gpSaveData[nPlayer].tour.aStats[fn_80119118(nPlayer, nEntrant)].nCareerWinnings += nShare;
        if (fn_8011908C(nPlayer, nEntrant)) {
            fn_800EF094(nPlayer, nShare);
        }
    }
}

// Pays the tournament's prizes (fn_800D3478 of nTotal and n per row) down the score ranking;
// only the first 70 rows are paid and the cut entrants get nothing.
void fn_8011BAD4(int nPlayer, int nTotal, int n) {
    s32 nRow;
    s32 nRank = -1;
    s32 nPool = 0;
    s32 nFirstRow = 0;
    s32 nTied = 0;
    s32 nEntrants = fn_80118664(nPlayer);
    s32 nEntrant;

    CalcScoreRankingsIfDirty(nPlayer);
    for (nRow = 0; nRow < nEntrants; nRow++) {
        nEntrant = lbl_80223C70.aEntrant[nRow];
        if (lbl_80223C70.aRank[nEntrant] != nRank && nRow < 70) {
            fn_8011B978(nPlayer, nPool, nFirstRow, nTied);
            nRank = lbl_80223C70.aRank[nEntrant];
            nFirstRow = nRow;
            nTied = 0;
            nPool = 0;
        }
        if (fn_801197A4(nPlayer, nEntrant)) {
            break;
        }
        if (nRow < 70) {
            nPool += fn_800D3478(nTotal, n, nRow);
        }
        nTied++;
    }
    fn_8011B978(nPlayer, nPool, nFirstRow, nTied);
}

// The statistic sort comparisons (lbl_80193FF8). Golfers whose values print differently go by
// value, a golfer with no value (0) last; the same printed value goes by name.

// Lower is better.
s32 fn_8011BBD8(const void* pA, const void* pB) {
    s32 nGolferA = *(const s32*)pA;
    s32 nGolferB = *(const s32*)pB;
    s32 nRet;
    int nPlayer = lbl_80281840.nPlayer;
    f32 fA = lbl_80226870[lbl_80281840.nStat].aValue[nGolferA].fValue;
    f32 fB = lbl_80226870[lbl_80281840.nStat].aValue[nGolferB].fValue;

    if (strcmp(lbl_80226870[lbl_80281840.nStat].aValue[nGolferA].szValue,
               lbl_80226870[lbl_80281840.nStat].aValue[nGolferB].szValue) != 0) {
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
        // EA bug: nRet is never set when the texts differ but the values are equal.
    } else {
        nRet = strcmp(fn_80118E30(nPlayer, nGolferA), fn_80118E30(nPlayer, nGolferB));
    }
    return nRet;
}

// Higher is better.
s32 fn_8011BCFC(const void* pA, const void* pB) {
    s32 nGolferA = *(const s32*)pA;
    s32 nGolferB = *(const s32*)pB;
    s32 nRet;
    int nPlayer = lbl_80281840.nPlayer;
    f32 fA = lbl_80226870[lbl_80281840.nStat].aValue[nGolferA].fValue;
    f32 fB = lbl_80226870[lbl_80281840.nStat].aValue[nGolferB].fValue;

    if (strcmp(lbl_80226870[lbl_80281840.nStat].aValue[nGolferA].szValue,
               lbl_80226870[lbl_80281840.nStat].aValue[nGolferB].szValue) != 0) {
        if (fA > fB) {
            nRet = -1;
        } else if (fA < fB) {
            nRet = 1;
        }
        // EA bug: nRet is never set when the texts differ but the values are equal.
    } else {
        nRet = strcmp(fn_80118E30(nPlayer, nGolferA), fn_80118E30(nPlayer, nGolferB));
    }
    return nRet;
}

// The score sort comparisons (lbl_80281848 is the player). Lower scores first, then by name.

// All entrants: a cut entrant sorts last, the winner first. 97.4%: only nPlayer and nEntrantA/nScoreB
// swap saved registers (declaration orders, int/s32, an inline score helper and the permuter tried).
s32 fn_8011BDF8(const void* pA, const void* pB) {
    s32 nScoreA;
    s32 nEntrantB = *(const s32*)pB;
    s32 nPlayer = lbl_80281848;
    s32 nEntrantA = *(const s32*)pA;
    PgaEntrantMC* pEntrantA = GetEntrantMCPtr(nPlayer, nEntrantA);
    PgaEntrantMC* pEntrantB = GetEntrantMCPtr(nPlayer, nEntrantB);
    s32 nScoreB;

    nScoreA = fn_8011937C(nPlayer, nEntrantA, !fn_8011908C(nPlayer, nEntrantA));
    if (fn_801197A4(nPlayer, nEntrantA)) {
        nScoreA = PGA_SCORE_CUT;
    } else if (nEntrantA == gpSaveData[nPlayer].tour.field.nWinner) {
        nScoreA = PGA_SCORE_WINNER;
    }
    nScoreB = fn_8011937C(nPlayer, nEntrantB, !fn_8011908C(nPlayer, nEntrantB));
    if (fn_801197A4(nPlayer, nEntrantB)) {
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
    return strcmp(fn_80118E30(nPlayer, pEntrantA->nGolfer), fn_80118E30(nPlayer, pEntrantB->nGolfer));
}

// The cut entrants among themselves.
s32 fn_8011BF74(const void* pA, const void* pB) {
    s32 nEntrantA = *(const s32*)pA;
    s32 nEntrantB = *(const s32*)pB;
    s32 nPlayer = lbl_80281848;
    PgaEntrantMC* pEntrantA = GetEntrantMCPtr(nPlayer, nEntrantA);
    PgaEntrantMC* pEntrantB = GetEntrantMCPtr(nPlayer, nEntrantB);
    s32 nScoreA = fn_8011937C(nPlayer, nEntrantA, !fn_8011908C(nPlayer, nEntrantA));
    s32 nScoreB = fn_8011937C(nPlayer, nEntrantB, !fn_8011908C(nPlayer, nEntrantB));

    if (nScoreA < nScoreB) {
        return -1;
    }
    if (nScoreA > nScoreB) {
        return 1;
    }
    return strcmp(fn_80118E30(nPlayer, pEntrantA->nGolfer), fn_80118E30(nPlayer, pEntrantB->nGolfer));
}

// Called from fn_801180C4 with two numbers as each award is won; empty in this build.
void fn_8011C054(int nA, int nB) {
}

void fn_8011C058(u8 bDirty) {
    gbStatsDirty = bDirty;
}

void fn_8011C060(u8 bDirty) {
    gbScoresDirty = bDirty;
}
