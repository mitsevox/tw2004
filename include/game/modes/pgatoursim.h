// pgatoursim.h (our name): the PGA TOUR simulation (PGATourSimulation.c, TW06's
// pgatoursimulation.c): the season statistics kept for every tour golfer.

#ifndef GAME_MODES_PGATOURSIM_H
#define GAME_MODES_PGATOURSIM_H

#include "platform.h"

// The tour golfers: 174 pros (the table the 'PGST' stream object fills) and the player.
#define PGA_NUM_PROS        174
#define PGA_USER_GOLFER     174     // the player's golfer id
#define PGA_NUM_GOLFERS     175
#define PGA_MAX_ENTRANTS    128     // the in-memory entrant table and the score ranking

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

// One golfer's season counts, from which each tour statistic is worked out (0x58 bytes; the
// save profile holds one per tour golfer, 0x58 apart). TW06: GM_Pga_StatCounts, which has
// three more counts (water saves, water hits, long putts) between nNonGIRPars and nEagles.
typedef struct PgaStatCounts {
    u8   unk0[2];
    u16  nRounds;               // 0x02  TW06: nRounds
    u16  nLongestDrive;         // 0x04  TW06: longestDrive
    u16  nDrives;               // 0x06  TW06: nDrives
    u32  nDriveDistance;        // 0x08  all drives together. TW06: totalDriveDistance
    u16  nLongestPutt;          // 0x0C  TW06: longestPutt
    u16  nFairwaysHit;          // 0x0E  TW06: nFairwaysHit
    u16  nFairways;             // 0x10  TW06: nFairwaysPossible
    u16  nGreensHit;            // 0x12  greens in regulation. TW06: nGreensHit
    u16  nHoles;                // 0x14  TW06: nHoles
    u16  nPutts;                // 0x16  TW06: nPutts
    u16  nGIRPutts;             // 0x18  putts on greens hit in regulation. TW06: nGIRPutts
    u16  nBunkerSaves;          // 0x1A  TW06: nBunkerSaves
    u16  nBunkers;              // 0x1C  TW06: nBunkers
    u16  nNonGIRPars;           // 0x1E  pars on greens missed in regulation. TW06: nNonGIRPars
    u16  nBirdiesAfterBogey;    // 0x20  TW06: nBirdiesAfterBogey
    u16  nBogeys;               // 0x22  bogeys or worse. TW06: nBogeysOrWorse
    u16  nEagles;               // 0x24  TW06: nEagles
    u16  nBirdies;              // 0x26  TW06: nBirdies
    u16  nPar3Birdies;          // 0x28  TW06: nPar3Birdies
    u16  nPar3Holes;            // 0x2A  TW06: nPar3Holes
    u16  nPar4Birdies;          // 0x2C  TW06: nPar4Birdies
    u16  nPar4Holes;            // 0x2E  TW06: nPar4Holes
    u16  nPar5Birdies;          // 0x30  TW06: nPar5Birdies
    u16  nPar5Holes;            // 0x32  TW06: nPar5Holes
    u16  nGIRBirdies;           // 0x34  birdies on greens hit in regulation. TW06: nGIRBirdies
    u16  nStrokes;              // 0x36  TW06: nStrokes
    u16  nPar3Strokes;          // 0x38  TW06: nPar3Strokes
    u16  nPar4Strokes;          // 0x3A  TW06: nPar4Strokes
    u16  nPar5Strokes;          // 0x3C  TW06: nPar5Strokes
    u8   unk3E[2];
    u32  nSeasonWinnings;       // 0x40  TW06: seasonWinnings
    u8   unk44[0x4A - 0x44];
    u8   nPlayerOfYearPoints;   // 0x4A  TW06: playerOfYearPoints
    u8   unk4B;
    u16  nConsecutiveCuts;      // 0x4C  TW06: nConsecutiveCuts
    u8   unk4E[2];
    u32  nCareerWinnings;       // 0x50  TW06: careerWinnings
    u8   unk54[0x58 - 0x54];
} PgaStatCounts;
LAYOUT_ASSERT(PgaStatCounts, 0x58);

// A tour pro (0x68 bytes): the 'PGST' stream object's data.
typedef struct PgaPro {
    char szName[0x50];          // 0x00  (GM_PgaTourSim_GetNameFromGolferID)
    f32  f50;                   // 0x50
    u8   unk54[0x68 - 0x54];
} PgaPro;
LAYOUT_ASSERT(PgaPro, 0x68);
extern PgaPro lbl_8024B9CC[PGA_NUM_PROS];

// An entrant of the tournament in the save profile (0x1C bytes; GetEntrantMCPtr). TW06:
// PgaTourSim_Entrant_MC_t, laid out differently.
typedef struct PgaEntrantMC {
    s16  nGolfer;               // 0x00  golfer id (PGA_USER_GOLFER: the player)
    u8   unk2[0x14 - 0x2];
    s32  bWasCut;               // 0x14  set with the golfer's consecutive-cuts count cleared (0x80117C50)
    s32  n18;                   // 0x18
} PgaEntrantMC;
LAYOUT_ASSERT(PgaEntrantMC, 0x1C);

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

extern s32 lbl_80194074[GM_PGA_STAT_COUNT];     // per statistic: GM_PgaTourSim_GetStatView
extern s32 lbl_801940F0[32];    // per statistic: the decimal places GM_PgaTourSim_GetStatValString prints
extern u8  gbStatsDirty;        // the statistics need working out again
extern u8  gbScoresDirty;       // the score ranking needs sorting again
extern u8  lbl_80282500;
extern s32 lbl_80282504;        // the player's playoff strokes (fn_8011989C)

// PGATourSimulation.c functions other files call. The TW06 name follows where the code matches it.
void  fn_8011763C(void);                        // TW06: PGATourSimulation_OpenONCE
void  fn_8011766C(void);                        // TW06: PGATourSimulation_CloseONCE
u8    fn_80117DE0(void);
void  fn_80117DE8(int nPlayer, u8 b);
char* fn_80118E30(int nPlayer, int nGolfer);    // TW06: GM_PgaTourSim_GetNameFromGolferID
u8    fn_8011908C(int nPlayer, int nEntrant);   // TW06: GM_PgaTourSim_IsEntrantUser
s32   fn_801190D8(int nPlayer, int nEntrant);   // TW06: GM_PgaTourSim_GetScoreRankFromEntrantID
s32   fn_80119118(int nPlayer, int nEntrant);   // TW06: GM_PgaTourSim_GetGolferIDFromEntrantID
u8    fn_801197A4(int nPlayer, int nEntrant);   // TW06: GM_PgaTourSim_GetWasCutFromEntrantID
s32   fn_801197CC(int nPlayer, int nRow);       // TW06: GM_PgaTourSim_GetEntrantIDFromScoreRow
void  fn_8011989C(int nPlayer, int nStrokes);   // TW06: GM_PgaTourSim_SetUserEntrantHoleStrokes
void  fn_801198F8(int nPlayer, int nHole);
s32   fn_80119A04(int nPlayer, int nEntrant);   // TW06: GM_PgaTourSim_GetCurrentHoleFromEntrantID
s32   fn_80119A2C(int nPlayer, int nEntrant);
s32   fn_8011A684(int nPlayer);                 // TW06: GM_PgaTourSim_GetNumPlayoffEntrants
u8    fn_8011A6F4(int nPlayer, int nEntrant);   // TW06: GM_PgaTourSim_EntrantIsInPlayoff
void  fn_8011C058(u8 bDirty);                   // TW06: GM_PgaTourSim_SetStatsDirty
void  fn_8011C060(u8 bDirty);                   // TW06: GM_PgaTourSim_SetScoresDirty

#endif
