// PGATourSimulation.c (TW06's pgatoursimulation.c): the PGA TOUR simulation behind game mode 23
// (GameModeDriverPGATour.c). It keeps the tour field (an entrant table in the save profile and
// one in memory, with each entrant's hole strokes and playoff state), the entrants' score
// ranking, and the season statistics of every tour golfer: the counts in the save profile
// (PgaStatCounts), each statistic worked out from them (driving distance, greens in regulation,
// scoring average, ...), each statistic's ranking, and the text fe_stats.c prints.

#include "engine.h"
#include "game/save.h"
#include "game/modes/pgatoursim.h"

PgaEntrantMC* GetEntrantMCPtr(int nPlayer, int nEntrant);
s32  fn_80118664(int nPlayer);
void fn_8011A890(int nPlayer);
void fn_8011AAC0(int nPlayer);
void fn_8011AC40(int nPlayer, GM_Pga_StatTypes_t nStat);
void fn_8011AE1C(int nPlayer, int nGolfer);
void fn_8011AF60(int nGolfer);
void fn_8011B094(int nGolfer);
void CalcAllStatsIfDirty(int nPlayer);
void CalcScoreRankingsIfDirty(int nPlayer);
void CalcAllStats(int nPlayer);
void fn_80117694(UStreamObject* pObject);

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

u8 fn_80117DE0(void) {
    return lbl_80282500;
}

void fn_80117DE8(int nPlayer, u8 b) {
    lbl_80282500 = b;
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

// A sort comparison for s32s, smallest first.
s32 IntCompareIncreasing(const void* pA, const void* pB) {
    return *(const s32*)pA - *(const s32*)pB;
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

s32 GM_PgaTourSim_GetStatView(GM_Pga_StatTypes_t nStat) {
    return lbl_80194074[nStat];
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

u8 fn_801197A4(int nPlayer, int nEntrant) {
    return GetEntrantMCPtr(nPlayer, nEntrant)->bWasCut;
}

s32 fn_801197CC(int nPlayer, int nRow) {
    CalcScoreRankingsIfDirty(nPlayer);
    return lbl_80223C70.aEntrant[nRow];
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

s32 fn_80119A04(int nPlayer, int nEntrant) {
    return GetEntrantNonMCPtr(nEntrant)->nCurrentHole;
}

s32 fn_80119A2C(int nPlayer, int nEntrant) {
    return GetEntrantMCPtr(nPlayer, nEntrant)->n18;
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

void CalcAllStatsIfDirty(int nPlayer) {
    if (gbStatsDirty) {
        CalcAllStats(nPlayer);
        gbStatsDirty = 0;
    }
}

void CalcScoreRankingsIfDirty(int nPlayer) {
    if (gbScoresDirty) {
        fn_8011A890(nPlayer);
        gbScoresDirty = 0;
        fn_8011AAC0(nPlayer);
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

// Called from fn_801180C4; empty in this build.
void fn_8011C054(void) {
}

void fn_8011C058(u8 bDirty) {
    gbStatsDirty = bDirty;
}

void fn_8011C060(u8 bDirty) {
    gbScoresDirty = bDirty;
}
