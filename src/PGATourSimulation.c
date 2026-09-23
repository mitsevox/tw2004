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

PgaEntrantMC* GetEntrantMCPtr(int nPlayer, int nEntrant);
s32  fn_80118664(int nPlayer);
s32  fn_801191D0(int nPlayer, int nEntrant, u8 b);
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
void fn_80117694(UStreamObject* pObject);

f32  fn_8000B318(int nStream);          // a normally distributed random number (mean 0, deviation 1)
int  fn_800D31A4(int nPar);             // the number of the 18 holes with that par

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
    fn_8015929C(pRanking->aGolfer, PGA_NUM_GOLFERS, sizeof(pRanking->aGolfer[0]), lbl_80193FF8[nStat]);
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

// Called from fn_801180C4; empty in this build.
void fn_8011C054(void) {
}

void fn_8011C058(u8 bDirty) {
    gbStatsDirty = bDirty;
}

void fn_8011C060(u8 bDirty) {
    gbScoresDirty = bDirty;
}
