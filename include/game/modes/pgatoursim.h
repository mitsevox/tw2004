// pgatoursim.h (our name): the PGA TOUR simulation (PGATourSimulation.c, TW06's
// pgatoursimulation.c): the season statistics kept for every tour golfer.

#ifndef GAME_MODES_PGATOURSIM_H
#define GAME_MODES_PGATOURSIM_H

#include "game/save.h"     // the tour golfers, PgaStatCounts, PgaEntrantMC

// The tour statistics, in the order of every per-statistic table (TW06's GM_Pga_StatTypes_t,
// which has one more, the tour ranking). The first 28 are worked out from one golfer's counts;
// the last three add up rankings in the others.
typedef enum GM_Pga_StatTypes_t {
    GM_PGA_STAT_DRIVING = 0,
    GM_PGA_STAT_FAIRWAYS = 1,
    GM_PGA_STAT_GIR = 2,
    GM_PGA_STAT_PUTTSPERROUND = 3,
    GM_PGA_STAT_PUTTING = 4,
    GM_PGA_STAT_SAVES = 5,
    GM_PGA_STAT_SCRAMBLING = 6,
    GM_PGA_STAT_BOUNCEBACK = 7,
    GM_PGA_STAT_HOLESPEREAGLE = 8,
    GM_PGA_STAT_BIRDIESPERROUND = 9,
    GM_PGA_STAT_PAR3BIRDS = 10,
    GM_PGA_STAT_PAR4BIRDS = 11,
    GM_PGA_STAT_PAR5BIRDS = 12,
    GM_PGA_STAT_BIRDIECONVERSION = 13,
    GM_PGA_STAT_SCORING = 14,
    GM_PGA_STAT_PARBREAKERS = 15,
    GM_PGA_STAT_PAR3PERFORMANCE = 16,
    GM_PGA_STAT_PAR4PERFORMANCE = 17,
    GM_PGA_STAT_PAR5PERFORMANCE = 18,
    GM_PGA_STAT_LONGESTDRIVE = 19,
    GM_PGA_STAT_LONGESTPUTT = 20,
    GM_PGA_STAT_TOTAL_EAGLES = 21,
    GM_PGA_STAT_TOTAL_BIRDIES = 22,
    GM_PGA_STAT_CONSECUTIVE_CUTS = 23,
    GM_PGA_STAT_SEASON_WINNINGS = 24,
    GM_PGA_STAT_CAREER_WINNINGS = 25,
    GM_PGA_STAT_ROUNDS = 26,
    GM_PGA_STAT_PLAYER_OF_YEAR_POINTS = 27,
    GM_PGA_STAT_SIMPLE_COUNT = 28,
    GM_PGA_STAT_ALLAROUND = 28,
    GM_PGA_STAT_TOTALDRIVING = 29,
    GM_PGA_STAT_COMPLEX1_COUNT = 30,
    GM_PGA_STAT_BALLSTRIKING = 30,
    GM_PGA_STAT_COUNT = 31
} GM_Pga_StatTypes_t;

// A tour pro (0x68 bytes): the 'PGST' stream object's data.
typedef struct PgaPro {
    char szName[0x18];          // 0x00  (GM_PgaTourSim_GetNameFromGolferID)
    // The pro's season form, which the simulated holes follow (fn_8011A074):
    f32  fDriveAvg;             // 0x18  driving distance (a drive: this plus 30 x a normal random number)
    f32  fFairwayPct;           // 0x1C  fairways hit, percent
    f32  fGIRPct;               // 0x20  greens in regulation, percent
    f32  fPuttAvg;              // 0x24  putts per round (per hole: this / 18)
    u8   unk28[4];
    f32  fSandSavePct;          // 0x2C  sand saves, percent
    u8   unk30[0x50 - 0x30];
    f32  f50;                  // 0x50  sorts the entrants, lowest first (fn_80118A5C)
    u8   unk54[4];
    f32  fPar3Avg;              // 0x58  scoring average on par 3s (fn_80119B54)
    f32  fPar4Avg;              // 0x5C  on par 4s
    f32  fPar5Avg;              // 0x60  on par 5s
    s32  nCareerWinnings;       // 0x64  before the game's first season (0x801176C0)
} PgaPro;
LAYOUT_ASSERT(PgaPro, 0x68);
extern PgaPro lbl_8024B9CC[PGA_NUM_PROS];

// An entrant of the tournament being played, in memory (0x50 bytes; GetEntrantNonMCPtr).
// TW06: PgaTourSim_Entrant_NonMC_t, the same layout.
typedef struct PgaEntrant {
    s32  nCurrentHole;          // 0x00  TW06: currentHole
    s32  aHoleStrokes[18];      // 0x04  TW06: holeScore
    s32  bInPlayoff;            // 0x4C  TW06: inPlayoff
} PgaEntrant;
LAYOUT_ASSERT(PgaEntrant, 0x50);
extern PgaEntrant lbl_80224070[PGA_MAX_ENTRANTS];

// The entrants in score order (0x400 bytes), rebuilt when the scores change.
#define PGA_SCORE_CUT       0x7FFFFFFF          // a cut entrant's score in the sorts: the worst
#define PGA_SCORE_WINNER    (-0x7FFFFFFF - 1)   // the winner's: the best
typedef struct PgaScoreRanking {
    s32  aEntrant[PGA_MAX_ENTRANTS];    // 0x000  the entrant in each row
    s32  aRank[PGA_MAX_ENTRANTS];       // 0x200  each entrant's place
} PgaScoreRanking;
LAYOUT_ASSERT(PgaScoreRanking, 0x400);
extern PgaScoreRanking lbl_80223C70;

// One golfer's value of a statistic, and as text (0x14 bytes).
typedef struct PgaStatValue {
    f32  fValue;                // 0x00
    char szValue[0x10];         // 0x04  GM_PgaTourSim_GetStatValString's text
} PgaStatValue;

// One statistic's ranking of every tour golfer (0x1324 bytes; one per statistic).
typedef struct PgaStatRanking {
    s32  aGolfer[PGA_NUM_GOLFERS];          // 0x000  the golfer in each row
    s32  aRank[PGA_NUM_GOLFERS];            // 0x2BC  each golfer's place
    PgaStatValue aValue[PGA_NUM_GOLFERS];   // 0x578  each golfer's value
} PgaStatRanking;
LAYOUT_ASSERT(PgaStatRanking, 0x1324);
extern PgaStatRanking lbl_80226870[GM_PGA_STAT_COUNT];

// What the statistic sort comparisons read (they get only two golfer ids): the statistic being
// ranked and the player whose profile holds the counts. nStat is -1 outside a sort.
typedef struct PgaStatSort {
    s32  nStat;                 // 0x0
    s32  nPlayer;               // 0x4
} PgaStatSort;
extern PgaStatSort lbl_80281840;
extern s32 lbl_80281848;        // the same for the score sorts: the player
// Per statistic: its sort comparison, fn_8011BCFC (higher is better) or fn_8011BBD8 (lower).
extern s32 (*lbl_80193FF8[GM_PGA_STAT_COUNT])(const void* pA, const void* pB);
extern s32 lbl_80194074[GM_PGA_STAT_COUNT];     // per statistic: GM_PgaTourSim_GetStatView
extern s32 lbl_801940F0[32];    // per statistic: the decimal places GM_PgaTourSim_GetStatValString prints
extern u8  gbStatsDirty;        // the statistics need working out again
extern u8  gbScoresDirty;       // the score ranking needs sorting again
extern u8  lbl_80282500;
extern s32 lbl_80282504;        // the player's playoff strokes (fn_8011989C)

// PGATourSimulation.c functions other files call. The TW06 name follows where the code matches it.
void  fn_8011763C(void);                        // TW06: PGATourSimulation_OpenONCE
void  fn_8011766C(void);                        // TW06: PGATourSimulation_CloseONCE
void  fn_80117860(TourSeason* pTour);
void  fn_801178C8(s32 nPlayer, SeasonEvent* pEvent, int nRound, int n, int uFlags);   // TW06: GM_PgaTourSim_SimRound
void  fn_80117AF8(int nPlayer);
void  fn_80117B58(int nPlayer);
void  fn_80117C50(int nPlayer, int nEntrant);
void  fn_80117D80(int nPlayer);
u8    fn_80117DE0(void);
void  fn_80117DE8(int nPlayer, u8 b);
void  fn_80117DF0(int nPlayer);
void  fn_80117E98(int nPlayer);
s32   fn_80118684(int nPlayer);
char* fn_80118E30(int nPlayer, int nGolfer);    // TW06: GM_PgaTourSim_GetNameFromGolferID
u8    fn_8011908C(int nPlayer, int nEntrant);   // TW06: GM_PgaTourSim_IsEntrantUser
s32   fn_801190D8(int nPlayer, int nEntrant);   // TW06: GM_PgaTourSim_GetScoreRankFromEntrantID
s32   fn_80119118(int nPlayer, int nEntrant);   // TW06: GM_PgaTourSim_GetGolferIDFromEntrantID
s32   fn_801191D0(int nPlayer, int nEntrant, u8 b);
s32   fn_80119588(int nPlayer, u8 b);
s32   fn_80119638(int nPlayer, int nEntrant, int nRound);
u8    fn_801197A4(int nPlayer, int nEntrant);   // TW06: GM_PgaTourSim_GetWasCutFromEntrantID
s32   fn_801197CC(int nPlayer, int nRow);       // TW06: GM_PgaTourSim_GetEntrantIDFromScoreRow
void  fn_8011989C(int nPlayer, int nStrokes);   // TW06: GM_PgaTourSim_SetUserEntrantHoleStrokes
void  fn_801198F8(int nPlayer, int nHole);
void  fn_80119934(int nPlayer);
s32   fn_80119A04(int nPlayer, int nEntrant);   // TW06: GM_PgaTourSim_GetCurrentHoleFromEntrantID
s32   fn_80119A2C(int nPlayer, int nEntrant);
void  fn_8011A538(int nPlayer);
void  fn_8011A5F8(int nPlayer);
s32   fn_8011A684(int nPlayer);                 // TW06: GM_PgaTourSim_GetNumPlayoffEntrants
u8    fn_8011A6F4(int nPlayer, int nEntrant);   // TW06: GM_PgaTourSim_EntrantIsInPlayoff
void  fn_8011A720(int nPlayer, int nHole);
s32   fn_8011A7C8(int nPlayer, int nHole);
void  fn_8011C058(u8 bDirty);                   // TW06: GM_PgaTourSim_SetStatsDirty
void  fn_8011C060(u8 bDirty);                   // TW06: GM_PgaTourSim_SetScoresDirty

#endif
