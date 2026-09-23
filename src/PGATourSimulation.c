// PGATourSimulation.c (TW06's pgatoursimulation.c): the PGA TOUR simulation behind game mode 23
// (GameModeDriverPGATour.c). It keeps the tour field (an entrant table in the save profile and
// one in memory) and the season statistics of every tour golfer: the counts in the save profile
// (PgaStatCounts), each statistic worked out from them (driving distance, greens in regulation,
// scoring average, ...), the rankings, and the text fe_stats.c prints. Most of it is still
// assembly; the sweep code below is the matched small functions.

#include "game/modes/pgatoursim.h"

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_80224070[];
u8* fn_80117628(s32 p0);
void UStream_UnregisterHandler();
void fn_8011766C(void);
extern u8 lbl_8024B9CC[];
void fn_8000E790();
void fn_80117694(s32 p0);
extern u8 lbl_80282500;
u8 fn_80117DE0(void);
void fn_80117DE8(s32 p0, s32 p1);
s32 fn_80118A4C(u8* p0, u8* p1);
extern u8 lbl_80226870[];
void fn_8011A85C(s32 arg0);
s32 fn_80118F10(s32 p0, s32 p1, s32 p2);
extern s32 gpSaveData;
extern u8 lbl_80194074[];
s32 fn_80119028(s32 p0);
s32 fn_80117600();
s32 fn_80119118(void);
u8 fn_8011908C(s32, s32);
s32 fn_8011913C(s32 arg0, s32 arg1, u8 arg2);
extern u8 lbl_80223C70[];
void fn_8011ABF8(s32 arg0);
s32 fn_801197A4(void);
s32 fn_801197CC(s32 p0, s32 p1);
extern u8 lbl_8028184D;
void fn_801198F8(s32 p0, s32 p1);
s32 fn_80119A04(s32 p0, s32 p1);
s32 fn_80119A2C(void);
s32 fn_8011A6F4(s32 p0, s32 p1);
void fn_8011B160(s32 p0);
extern u8 lbl_8028184C;
s32 fn_8011A890(s32 arg0);
s32 fn_8011AAC0(s32);
s32 fn_8011AE1C(s32, s32);
void fn_8011AEC8(s32 arg0);
s32 fn_8011AC40(s32, s32);
void fn_8011AF14(s32 arg0);
s32 fn_8011AF60(s32);
void fn_8011B00C(void);
void fn_8011B048(s32 arg0);
void fn_8011B094(s32 p0);
void fn_8011B100(void);
void fn_8011B13C(s32 p0);
void fn_8011C054(void);
void fn_8011C058(u8 v);
void fn_8011C060(u8 v);

u8* fn_80117628(s32 p0) {
    return (lbl_80224070 + (p0 * 80));
}

void fn_8011766C(void) {
    UStream_UnregisterHandler(1346851668);
}

void fn_80117694(s32 p0) {
    fn_8000E790(p0, 18096, lbl_8024B9CC);
}

u8 fn_80117DE0(void) {
    return lbl_80282500;
}

void fn_80117DE8(s32 p0, s32 p1) {
    lbl_80282500 = p1;
}

s32 fn_80118A4C(u8* p0, u8* p1) {
    return (*(s32*)p0 - *(s32*)p1);
}

s32 fn_80118F10(s32 p0, s32 p1, s32 p2) {
    fn_8011A85C(p0);
    return *(s32*)((lbl_80226870 + (p1 * 4900)) + (p2 << 2));
}

s32 fn_80119028(s32 p0) {
    return *(s32*)(lbl_80194074 + (p0 << 2));
}

s32 fn_80119118(void) {
    s32 t0;
    t0 = fn_80117600();
    return *(s16*)((u8*)t0);
}

s32 fn_8011913C(s32 arg0, s32 arg1, u8 arg2) {
    s32 var_r31;
    s32 var_r3;

    var_r31 = *(s32*)fn_80117628(arg1);
    if (fn_8011908C(arg0, arg1) != 0) {
        if (arg2 != 0) {
            var_r31 += 1;
        }
    } else if (arg2 == 0) {
        var_r31 -= 1;
    }
    var_r3 = 0x12;
    if (var_r31 <= 0x12) {
        var_r3 = var_r31;
    }
    return var_r3;
}

s32 fn_801197A4(void) {
    s32 t0;
    t0 = fn_80117600();
    return (*(s32*)(((u8*)t0) + 0x14) & 0xFF);
}

s32 fn_801197CC(s32 p0, s32 p1) {
    fn_8011ABF8(p0);
    return *(s32*)(lbl_80223C70 + (p1 << 2));
}

void fn_801198F8(s32 p0, s32 p1) {
    u8* t0;
    t0 = fn_80117628(0);
    *(s32*)((u8*)t0) = p1;
    lbl_8028184D = 1;
}

s32 fn_80119A04(s32 p0, s32 p1) {
    u8* t0;
    t0 = fn_80117628(p1);
    return *(s32*)((u8*)t0);
}

s32 fn_80119A2C(void) {
    s32 t0;
    t0 = fn_80117600();
    return *(s32*)(((u8*)t0) + 0x18);
}

s32 fn_8011A6F4(s32 p0, s32 p1) {
    u8* t0;
    t0 = fn_80117628(p1);
    return (*(s32*)(((u8*)t0) + 0x4C) & 0xFF);
}

void fn_8011A85C(s32 arg0) {
    if ((u8) lbl_8028184C != 0) {
        fn_8011B160(arg0);
        lbl_8028184C = 0;
    }
}

void fn_8011ABF8(s32 arg0) {
    if ((u8) lbl_8028184D != 0) {
        fn_8011A890(arg0);
        lbl_8028184D = 0;
        fn_8011AAC0(arg0);
    }
}

void fn_8011AEC8(s32 arg0) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_8011AE1C(arg0, var_r31);
        var_r31 += 1;
    } while (var_r31 < 0xAF);
}

void fn_8011AF14(s32 arg0) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_8011AC40(arg0, var_r31);
        var_r31 += 1;
    } while (var_r31 < 0x1C);
}

void fn_8011B00C(void) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_8011AF60(var_r31);
        var_r31 += 1;
    } while (var_r31 < 0xAF);
}

void fn_8011B048(s32 arg0) {
    s32 var_r31;

    var_r31 = 0x1C;
    do {
        fn_8011AC40(arg0, var_r31);
        var_r31 += 1;
    } while (var_r31 < 0x1E);
}

void fn_8011B100(void) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_8011B094(var_r31);
        var_r31 += 1;
    } while (var_r31 < 0xAF);
}

void fn_8011B13C(s32 p0) {
    fn_8011AC40(p0, 30);
}

void fn_8011B160(s32 p0) {
    fn_8011AEC8(p0);
    fn_8011AF14(p0);
    fn_8011B00C();
    fn_8011B048(p0);
    fn_8011B100();
    fn_8011B13C(p0);
}

// ---- end of sweep code ----

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

// ---- sweep code (not yet cleaned up) ----

void fn_8011C054(void) {
}

void fn_8011C058(u8 v) {
    lbl_8028184C = v;
}

void fn_8011C060(u8 v) {
    lbl_8028184D = v;
}

// ---- end of sweep code ----
