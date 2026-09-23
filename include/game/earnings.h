// earnings.h (our name): the prize table (lbl_80200538, 0x22F0 bytes, loaded from the 'ERN '
// stream by Earnings.c) and the Earnings.c functions the game modes call. Multipliers are
// percentages (100 = x1). Prizes that depend on a golfer are indexed by earnings rating
// (fn_800D3C7C, 0..25).

#ifndef GAME_EARNINGS_H
#define GAME_EARNINGS_H

#include "game_types.h"
#include "platform.h"

#define NUM_EARNINGS_RATINGS 26  // fn_800D3CF8 caps a rating at 25

// Beating a CPU golfer of one rating in stroke play (TW06 GM_Earnings_GetStrokeWinnings).
typedef struct StrokePrize {
    s32  nBase;                 // 0x0  the prize for a win
    s32  nPerStroke;            // 0x4  and for each stroke of the margin, up to 5
} StrokePrize;

// A skin's value at one rating (TW06 GM_Earnings_GetSkinsHoleValue).
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
// Drive Challenge Trophy Ball"), handed to fn_80125874.
typedef struct BioAccomplishment {
    s32  nValue;                // 0x00
    char szName[0x44];          // 0x04
} BioAccomplishment;

#define NUM_BIO_ACCOMPLISHMENTS 50  // the 'ERN ' data fills 50 (0xA54..0x1864); the rest is other data

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

#define NUM_COURSE_PRICES 24    // the 'ERN ' data has 24 rows (0x114..0x1D4): fn_800D3A20 reads 0..20, 21 and 23

typedef struct EarningsTable {
    u8   unk0[0x114];
    CoursePrice aCoursePrice[NUM_COURSE_PRICES];     // 0x114  per course (SaveProfile.aCourseUnlocked)
    StrokePrize aStrokePrize[NUM_EARNINGS_RATINGS];  // 0x1D4  per rating of the beaten golfer
    SkinsValue aSkins[NUM_EARNINGS_RATINGS];         // 0x2A4  per best rating in the game
    u8   unk4AC[0x5E4 - 0x4AC];
    MatchPrize aLadderPrize[25];    // 0x5E4  per ladder event
    MiniPrize aMini[20];            // 0x710
    u8   unk940[0x980 - 0x940];
    s32  aTeePct[3];            // 0x980  the tee multiplier, as [2 - nTeeSet] (tee set 3 pays as 1)
    s32  aPinSetPct[4];         // 0x98C  the multiplier for the hole's pin set (gpGame->nPinSet) 0..3
    s32  aTourPct[6];           // 0x99C  the TOUR card multiplier per level 1..6 (level 0 pays as 1)
    u8   unk9B4[0x9E4 - 0x9B4];
    s32  n9E4;                  // 0x9E4  paid with award 0xC, once every challenge has a medal (GameMode5)
    u8   unk9E8[0x9F0 - 0x9E8];
    s32  nLadderDone;           // 0x9F0  paid when the last ladder event is won
    u8   unk9F4[0xA24 - 0x9F4];
    s32  nA24;                  // 0xA24  paid with award 0x1C after a challenge (GameMode5)
    u8   unkA28[0xA54 - 0xA28];
    BioAccomplishment aBio[NUM_BIO_ACCOMPLISHMENTS];    // 0xA54
    u8   unk1864[0x22F0 - 0x1864];
} EarningsTable;
LAYOUT_ASSERT(EarningsTable, 0x22F0);

extern EarningsTable lbl_80200538;
extern s32 lbl_801FFD68[10];    // the courses fn_800D3A20 unlocked, for their messages
extern s32 lbl_80200448[10];    // a working table: fn_800D439C messages the entries of kind 2 or 4
extern s32 lbl_802004C0[10];    // with these ids
extern s32 lbl_802004E8[10];    // the putt record ids (2)
extern s32 lbl_80200470[10];    // the putt record results (fn_800D7B1C)
extern s32 lbl_80200498[10];    // a working table: fn_800D3DDC messages the entries of kind 2 or 4
extern s32 lbl_80200510[10];    // with these ids
extern s32 lbl_80282258;        // their count

// Earnings.c
int  fn_800D38F0(int nWinner, int nLoser, int nMargin, s32* pPrize);   // a ladder event's winnings
int  fn_800D3C1C(void);                // TW06 GM_GetHighestRatedGolfer: the best rating among the players
s32  fn_800D3D64(int nRating, int nHole);   // TW06 GM_Earnings_GetSkinsHoleValue
u8   fn_800D750C(int nPlayer, int nAward);  // give an award if the player does not have it yet
void fn_800D8D10(int nPlayer);          // clear the player's flags b30C..b30F
void fn_800D8D38(int nPlayer);          // clear the flags fn_800D9350 sets (b310..b312)
void fn_800D8D5C(int nPlayer);          // clear the player's money breakdown for the round
u8   fn_800D9998(int nPlayer, int nAward);

#endif
