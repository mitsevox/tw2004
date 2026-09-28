// earnings.h (our name): the prize table (gEarningsTable, 0x22F0 bytes, loaded from the 'ERN '
// stream by Earnings.c) and the Earnings.c functions the game modes call. Multipliers are
// percentages (100 = x1). Prizes that depend on a golfer are indexed by earnings rating
// (GM_Earnings_RateGolfer, 0..25).

#ifndef GAME_EARNINGS_H
#define GAME_EARNINGS_H

#include "game_types.h"
#include "platform.h"

#define NUM_EARNINGS_RATINGS 26  // GM_Earnings_CapRating caps a rating at 25

// Beating a CPU golfer of one rating in stroke play (GM_Earnings_GetStrokeWinnings; TW06: GM_Earnings_GetStrokeWinnings
// (by position)).
typedef struct StrokePrize {
    s32  nBase;                 // 0x0  the prize for a win
    s32  nPerStroke;            // 0x4  and for each stroke of the margin, up to 5
} StrokePrize;

// A skin's value at one rating (GM_Earnings_GetSkinsHoleValue; TW06: GM_Earnings_GetSkinsHoleValue (by position)).
typedef struct SkinsValue {
    s32  aValue[4];             // 0x00  holes 1..6, 7..12, 13..17, 18
    s32  n10;                   // 0x10
} SkinsValue;

// A ladder event's prize (GameMode4.c).
typedef struct MatchPrize {
    s32  nBase;                 // 0x0  for the win
    s32  nPerHole;              // 0x4  and for each hole of the margin, up to 5
    s32  nBio;                  // 0x8  the EA Sports Bio accomplishment the win posts (aBio), -1 none
} MatchPrize;

// An EA Sports Bio accomplishment the game posts (0x48 bytes): a value and its text ("Won The Long
// Drive Challenge Trophy Ball"), handed to EASBio_SetAccomplishment.
typedef struct BioAccomplishment {
    s32  nValue;                // 0x00
    char szName[0x44];          // 0x04
} BioAccomplishment;

#define NUM_BIO_ACCOMPLISHMENTS 49  // 0xA54..0x181C: GM_Earnings_CheckShotGoals's shot goals start at 0x1860

// A shot goal (0x34 bytes, GM_Earnings_CheckShotGoals): what a shot must show when it stops to earn an award, or
// a money prize (award 0x27). Laid out as PuttGoal, with more ball tests.
typedef struct ShotGoal {
    s32  nId;                   // 0x00  goals with the same nonzero id compete: the biggest nValue is kept
    u32  uModes;                // 0x04  the game modes it counts in, a bit per mode
    u8   unk08;                 // 0x08
    u8   uPars;                 // 0x09  the hole's pars it counts on: bit 0 par 3, 1 par 4, 2 par 5
    u8   uShotKinds;            // 0x0A  a bit per Player.nShotKind
    u8   uLies;                 // 0x0B  a bit per Earnings_GetSurfaceClassBit class of the ground the shot left
    f32  f0C;                   // 0x0C  the most fn_800D04AC may return
    u8   uBallLies;             // 0x10  a bit per Earnings_GetLieBit class of the ball's lie
    f32  f14;                   // 0x14  the least fn_800D0550 (the shot's length) may return
    f32  f18;                   // 0x18  the most fn_800D0478 (the distance from the pin) may return, 0 any
    u32  uClubs;                // 0x1C  a bit per Player.nClub
    u16  uFlags;                // 0x20  more tests, a bit each (GM_Earnings_CheckShotGoals)
    u8   uMults;                // 0x22  the multipliers a prize takes (as PuttGoal.uMults)
    u8   unk23;                 // 0x23
    s32  nValue;                // 0x24  the prize (award 0x27), else what ranks goals with the same id
    u8   unk28;                 // 0x28
    s8   nAward;                // 0x29  the award it gives (GM_Earnings_AwardThisTrophyBallToUser), 0x27 a money prize
    s8   n2A;                   // 0x2A  kept with a money prize (gShotPrizeMsgs)
    u8   unk2B;                 // 0x2B
    u8   bEnabled;              // 0x2C
    u8   unk2D[3];              // 0x2D
    s32  nBio;                  // 0x30  the EA Sports Bio accomplishment it posts (aBio), -1 none
} ShotGoal;

#define NUM_SHOT_GOALS 15

// A putt goal (0x24 bytes, GM_Earnings_CheckPuttGoals): what a hole must show when the putt drops to earn an
// award, or a money prize (award 0x27).
typedef struct PuttGoal {
    s32  nId;                   // 0x00  goals with the same nonzero id compete: the biggest nValue is kept
    u32  uModes;                // 0x04  the game modes it counts in, a bit per mode
    u8   unk08;                 // 0x08
    u8   uPars;                 // 0x09  the hole's pars it counts on: bit 0 par 3, 1 par 4, 2 par 5
    u8   uShotKinds;            // 0x0A  a bit per Player.nShotKind
    u8   uLies;                 // 0x0B  a bit per Earnings_GetSurfaceClassBit class of the ground the shot left
    f32  f0C;                   // 0x0C  the most fn_800D04AC may return
    u32  uClubs;                // 0x10  a bit per Player.nClub
    u8   uFlags;                // 0x14  more tests, a bit each (GM_Earnings_CheckPuttGoals)
    s8   nMaxPutts;             // 0x15  the most putts on the hole, 0 any
    s8   nScore;                // 0x16  the score on the hole it takes (GM_Earnings_CheckPuttGoals), 0 any
    u8   uMults;                // 0x17  the multipliers a prize takes: bit 0 course, 1 tee, 2 pin set, 3 TOUR card
    s32  nValue;                // 0x18  the prize (award 0x27), else what ranks goals with the same id
    s8   nAward;                // 0x1C  the award it gives (GM_Earnings_AwardThisTrophyBallToUser), 0x27 a money prize
    s8   n1D;                   // 0x1D  kept with a money prize (gPuttPrizeMsgs)
    u8   unk1E;                 // 0x1E
    u8   bEnabled;              // 0x1F
    s32  nBio;                  // 0x20  the EA Sports Bio accomplishment it posts (aBio), -1 none
} PuttGoal;

#define NUM_PUTT_GOALS 18

// A hole goal (0x2C bytes, GM_Earnings_CheckHoleGoals): what the round must show when a hole is finished to earn
// an award, or a money prize (award 0x27). The counts are minimums, 0 for any.
typedef struct HoleGoal {
    s32  nId;                   // 0x00  goals with the same nonzero id compete: the biggest nValue is kept
    u32  uModes;                // 0x04  the game modes it counts in, a bit per mode
    s8   bEachHole;             // 0x08  checked after each hole (else only when the round is over)
    s8   aToPar[5];             // 0x09  holes finished at par, -1, -2, -3 and -5 or better (fn_800D0DC8)
    s8   aRun[5];               // 0x0E  the same as runs of holes in a row (fn_800D0F04)
    s8   n13;                   // 0x13  holes counted by fn_800D0FBC (at most the course's fn_800D3208)
    s8   n14;                   // 0x14  holes counted by fn_800D1170
    s8   n15;                   // 0x15  the run fn_800D10B0 finds
    s8   n16;                   // 0x16  the run fn_800D1250 finds
    s8   n17;                   // 0x17  putts over the round (fn_800D1330)
    s8   nMaxStrokes;           // 0x18  the most strokes over the round (GM_GetPlayerRoundStrokes)
    u8   b19;                   // 0x19  counts on every hole; clear, only at the end (the full round's 18th
                                //       hole, or after the round)
    u8   unk1A;                 // 0x1A
    s8   nKind;                 // 0x1B  a whole-round test: 1 GM_Earnings_CheckEagleEveryPar5, 2 GM_Earnings_CheckWinAllTournaments, 3 never,
                                //       4 GM_Earnings_CheckFirstTournamentWin, 5 no hole over par, 6 under the course's par
    u8   uMults;                // 0x1C  the multipliers a prize takes (as PuttGoal.uMults)
    u8   unk1D[3];              // 0x1D
    s32  nValue;                // 0x20  the prize (award 0x27), else what ranks goals with the same id
    s8   nAward;                // 0x24  the award it gives (GM_Earnings_AwardThisTrophyBallToUser), 0x27 a money prize
    s8   n25;                   // 0x25  kept with a money prize (gHolePrizeMsgs)
    u8   unk26;                 // 0x26
    u8   bEnabled;              // 0x27
    s32  nBio;                  // 0x28  the EA Sports Bio accomplishment it posts (aBio), -1 none
} HoleGoal;

#define NUM_HOLE_GOALS 29

// A row of the mini-game table (0x1C bytes): what landing on a surface is worth in the target
// games (modes 12, 13, 15, 16, 17). The row with nId 999 holds the target games' prizes instead.
typedef struct MiniPrize {
    s32  nId;                   // 0x00  a surface id (0x85..0x90 are the target rings), or 999
    s32  n4;                    // 0x04  points in modes 12 and 13; mode 13's prize in row 999
    s32  n8;                    // 0x08  points in mode 16; its prize in row 999
    s32  nC;                    // 0x0C  points in mode 17; its prize in row 999
    s32  n10;                   // 0x10  points in mode 15
    s32  n14;                   // 0x14  seconds in mode 13, bonus-meter points in mode 12
    s32  n18;                   // 0x18  balls in modes 13 and 17, the shot multiplier in mode 12
} MiniPrize;

// What unlocks a course (8 bytes): the money it takes, and the EA Sports Bio accomplishment
// ("Unlocked A Course") posted with it.
typedef struct CoursePrice {
    s32  nPrice;                // 0x0  the profile's money (SaveProfile.n64) that unlocks it; 0 = not for sale
    s32  nBio;                  // 0x4  aBio index, -1 none
} CoursePrice;

#define NUM_COURSE_PRICES 24    // the 'ERN ' data has 24 rows (0x114..0x1D4): GM_Earnings_CheckUnlockCourses reads 0..20, 21 and 23

// The payout multipliers are one table of 29 (EarningsTable.aMult; GameUICommands.c's GM_vGetWrapupData
// indexes it across the groups). Where each group starts:
#define EARN_MULT_COURSE 0      // 16: the course multiplier (x1..x4) per course, in GM_Earnings_GetCourseModifier's order
#define EARN_MULT_TEE    16     // 3: the tee percentage, as [2 - nTeeSet] (tee set 3 pays as 1)
#define EARN_MULT_PINSET 19     // 4: the percentage for the hole's pin set (gpGame->nPinSet) 0..3
#define EARN_MULT_TOUR   23     // 6: the TOUR card percentage per level 1..6 (level 0 pays as 1)
#define EARN_NUM_MULTS   29

// A row of the first table (12 bytes): a range of values and what it maps to. A menu message
// (fn_8007D708) looks a value up: the first row with n0 - 1 <= value <= n4 gives n8.
typedef struct EarningsRange {
    s32  n0;                    // 0x0
    s32  n4;                    // 0x4
    s32  n8;                    // 0x8
} EarningsRange;

#define NUM_EARNINGS_RANGES 23

typedef struct EarningsTable {
    EarningsRange aRange[NUM_EARNINGS_RANGES];      // 0x000
    CoursePrice aCoursePrice[NUM_COURSE_PRICES];     // 0x114  per course (SaveProfile.aCourseUnlocked)
    StrokePrize aStrokePrize[NUM_EARNINGS_RATINGS];  // 0x1D4  per rating of the beaten golfer
    SkinsValue aSkins[NUM_EARNINGS_RATINGS];         // 0x2A4  per best rating in the game
    u8   unk4AC[0x5E4 - 0x4AC];
    MatchPrize aLadderPrize[25];    // 0x5E4  per ladder event
    MiniPrize aMini[20];            // 0x710
    s32  aMult[EARN_NUM_MULTS]; // 0x940  the payout multipliers, one table (EARN_MULT_...)
    s32  a9B4[12];              // 0x9B4  read by index by a menu message (fn_8007E174)
    s32  n9E4;                  // 0x9E4  paid with award 0xC, once every challenge has a medal (GameMode5)
    u8   unk9E8[0x9F0 - 0x9E8];
    s32  nLadderDone;           // 0x9F0  paid when the last ladder event is won
    u8   unk9F4[0xA24 - 0x9F4];
    s32  nA24;                  // 0xA24  paid with award 0x1C after a challenge (GameMode5)
    u8   unkA28[0xA54 - 0xA28];
    BioAccomplishment aBio[NUM_BIO_ACCOMPLISHMENTS];    // 0xA54
    u8   unk181C[0x1860 - 0x181C];
    ShotGoal aShotGoal[NUM_SHOT_GOALS];                 // 0x1860
    PuttGoal aPuttGoal[NUM_PUTT_GOALS];                 // 0x1B6C
    HoleGoal aHoleGoal[NUM_HOLE_GOALS];                 // 0x1DF4
} EarningsTable;
LAYOUT_ASSERT(EarningsTable, 0x22F0);

extern EarningsTable gEarningsTable;
extern s32 gUnlockedCourses[10];    // the courses GM_Earnings_CheckUnlockCourses unlocked, for their messages
extern s32 gRoundRecordResults[10];    // a working table: GM_Earnings_PayRoundGoals messages the entries of kind 2 or 4
extern s32 gRoundRecordKinds[10];    // with these ids
extern s32 gPuttRecordKinds[10];    // the putt record ids (2)
extern s32 gPuttRecordResults[10];    // the putt record results (HighScoreRecords_GetEndOfHoleRecord)
extern s32 gShotRecordResults[10];    // a working table: GM_Earnings_PayShotGoals messages the entries of kind 2 or 4
extern s32 gShotRecordKinds[10];    // with these ids
extern s32 gNumRecordHits;        // their count
extern s32 gShotPrizeBases[10];    // the money prizes GM_Earnings_CheckShotGoals finds, before the multipliers
extern s32 gPuttPrizeBases[10];    // the money prizes GM_Earnings_CheckPuttGoals finds, before the multipliers
extern s32 gHolePrizeBases[10];    // and those GM_Earnings_CheckHoleGoals finds

// Earnings.c
int  GM_Earnings_GetLadderWinnings(int nWinner, int nLoser, int nMargin, s32* pPrize);   // a ladder event's winnings
int  GM_GetHighestRatedGolfer(void);                 // the best rating among the players
s32  GM_Earnings_GetSkinsHoleValue(int nRating, int nHole);   // a skin's value
u8   GM_Earnings_AwardTrophyBall(int nPlayer, int nAward);  // give an award if the player does not have it yet
// Whether nValue and szName are already among the top five of a record (i the kind, k the table:
// course k's records, recB[k], recC[k]). MC.c tests the u8 result.
u8   HighScoreRecords_RecordExist(int i, int nValue, const char* szName, int k);
u8   HighScoreRecords_SkillZoneRecordExist(int i, int nValue, const char* szName, int k);
u8   HighScoreRecords_LongDriveRecordExist(int i, int nValue, const char* szName, int k);
// Offer nValue and szName to record kind nKind of the table that the game mode and
// gpGame->nCurHoleNum pick (MC.c sets both to reach recB and recC, and passes nPlayer 5).
// Earnings.c tests for 2 or 4, a new best.
int  HighScoreRecords_CheckRecord(int nKind, int nValue, int bSave, const char* szName, int nPlayer);
void GM_ClearShotBonusStats(int nPlayer);          // clear the player's flags b30C..bBunkerThisShot
void GM_ClearHoleBonusStats(int nPlayer);          // clear the flags GM_RecordBonusShotStats sets (bBunkerThisHole..b312)
void GM_ClearGameBonusStats(int nPlayer);          // clear the player's money breakdown for the round
u8   Earnings_IsTourAwardEarned(int nPlayer, int nAward);

#endif
