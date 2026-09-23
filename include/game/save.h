// save.h (our name): the save profiles. gpSaveData points to one profile per player slot
// (PlayerNumber_t), 0x10600 bytes each: the unlocks, the awards, the saved replays and custom
// rounds, the challenge medals, the created golfer and the PGA TOUR season. Earnings.c keeps most
// of it. The profile is a memory-card record: a port reads and writes it field by field,
// big-endian, never by copying the struct.

#ifndef GAME_SAVE_H
#define GAME_SAVE_H

#include "golfer.h"

// An award in a save profile: whether it is won, and when. TW06: AwardInfoBase.
typedef struct Award {
    u8   bWon;                  // 0x0  TW06: bWon
    u8   unk1;
    u16  nDate;                 // 0x2  the day it was won (fn_800D2994). TW06: dateWon
} Award;

// A tournament of the season in a save profile (TW06: PGATourSeason_EventData, the same layout).
typedef struct SeasonEvent {
    char szChampName[0x10];     // 0x00  the tournament's champion. TW06: champName
    s32  nChampScore;           // 0x10  TW06: champScore
    u16  nEventPar;             // 0x14  TW06: eventPar
    u16  nUserBracket;          // 0x16  the player's bracket when it was played. TW06: userBracket
    s32  nUserScore;            // 0x18  TW06: userScore
    s32  nUserRank;             // 0x1C  the player's finishing place. TW06: userRank
    s32  nUserRankType;         // 0x20  0 did not play, 1 missed the cut, 2 placed. TW06: eUserRankType
} SeasonEvent;

// The PGA TOUR season in a save profile (TW06: PGATourSeason_t, which has 29 tournaments).
typedef struct TourSeason {
    s32  nSeason;               // 0x00  0 = 2004. TW06: season
    s32  nEvent;                // 0x04  the current tournament. TW06: eventID
    s32  nRound;                // 0x08  its round. TW06: round
    SeasonEvent aEvent[31];     // 0x0C
} TourSeason;

// A saved custom round (0x70 bytes): 18 holes, each a hole number and the course it is from.
// A new profile has three, emptied by the profile setup at 0x80057C88.
typedef struct SavedRound {
    u8   unk0[0x15];            // 0x00  byte 0 cleared by the setup
    u8   b15;                   // 0x15  set to 1 by the setup
    s8   nHoleNum[18];          // 0x16  -1 = none
    s32  nCourse[18];           // 0x28
} SavedRound;

#define NUM_SAVED_ROUNDS 3      // the setup's loop count

// One save profile (0x10600 bytes).
typedef struct SaveProfile {
    u8   bActive;               // 0x00000  1: the slot holds a profile; payouts are scaled and awards given only then
    char szName[0x1C - 0x1];    // 0x00001  the profile's name, compared with the record holders'
    u8   aGolferUnlocked[30];   // 0x0001C  per golfer (fn_80058278 sets, fn_8005832C tests)
    u8   aCourseUnlocked[23];   // 0x0003A  per course
    u8   aRewardUnlocked[0x70 - 0x51];  // 0x00051  per reward (fn_80058428 sets); the
                                //          "THEKITCHENSINK" code (0x80056568) sets the first 18
    u8   b70;                   // 0x00070  set when an award is won, a round is counted or a challenge
                                //          starts; cleared when a round is set up (GameRound.c)
    u8   unk71[3];
    s32  n74;                   // 0x00074  stroke-play rounds counted
    s32  n78;                   // 0x00078  their strokes
    s32  n7C;                   // 0x0007C  full rounds counted
    u8   unk80[0xA8 - 0x80];
    s32  nA8;                   // 0x000A8  the best stroke-play round (0: none yet)
    u8   unkAC[0xC8 - 0xAC];
    struct {
        u8 b;                   //          1: the tournament is won (fn_800F02A8)
        u8 unk1[7];
    } aC8[31];                  // 0x000C8  one per PGA TOUR tournament
    struct {
        u8 b;
        u8 unk1[3];
    } a1C0[16];                 // 0x001C0  flags GM_GetBonusProgress counts
    u8   unk200[0x20C - 0x200];
    Award aRTEAward[75];        // 0x0020C  per real-time event id. TW06: rteEventAwardInfo
    Award aLadderAward[25];     // 0x00338  per ladder event (GameMode4.c); fn_800584DC's earnings
                                //          rating counts the won ones
    Award aAward[39];           // 0x0039C
    u8   aReplay[5][0xF28];     // 0x00438  a Replay each, saved with awards 0, 6, 9, 3 and 13
    s32  nTourCardLevel;        // 0x05000  0..6: 1 once the lessons are done (GameMode11), raised by
                                //          fn_800D439C; the payout multiplier (fn_800D7220)
    u8   a5004[71];             // 0x05004  per marked hole 0..70 (fn_800E1CE8): fn_800588F4's kind 0
    u8   unk504B;
    s32  a504C[71];             // 0x0504C  the same, fn_800588F4's kind 1
    u8   unk5168[4];
    s32  aMedal[29];            // 0x0516C  the best medal per challenge group (0 best, 3 none)
    u8   unk51E0[4];
    u16  aMedalDate[29];        // 0x051E4  the day each was earned (fn_800D2994)
    u8   unk521E[0x5230 - 0x521E];
    SavedRound aSavedRound[NUM_SAVED_ROUNDS];   // 0x05230
    u8   unk5380[0x54C2 - 0x5380];
    // The created golfer kept in this slot (golfer FIRST_CREATED_GOLFER + the slot), copied into
    // the session's PlayerProfile by Golfer.c.
    u8   n54C2;                 // 0x054C2  -> PlayerProfile.unk2
    u8   unk54C3[5];
    char szGolferNames[6][8];   // 0x054C8  -> PlayerProfile.szNames
    u8   nGolferOutfit;         // 0x054F8  -> PlayerProfile.nOutfit
    u8   nGolferBallType;       // 0x054F9  -> PlayerProfile.nBallType
    u8   unk54FA[0xB634 - 0x54FA];
    TourSeason tour;            // 0x0B634
    u8   unkBA9C[0x104C8 - 0xBA9C];
    u16  n104C8;                // 0x104C8  counts the tournaments started
    u8   unk104CA[0x10578 - 0x104CA];
    u8   a10578[4];             // 0x10578  marked holes 71..74: fn_800588F4's kind 0
    s32  a1057C[4];             // 0x1057C  and kind 1
    u8   unk1058C[0x10600 - 0x1058C];
} SaveProfile;

extern SaveProfile* gpSaveData;
extern SaveProfile* lbl_80281DF4;       // unlocks that hold for every profile (the cheat codes set them)

// Earnings.c: the awards
s32  fn_800D7770(int nPlayer, Award* pAward);   // mark an award won today; 1 if it was not won before

// 0x800588F4: marked hole i's kind-0 byte (a5004/a10578) or kind-1 value (a504C/a1057C); -1 for
// another kind.
int  fn_800588F4(SaveProfile* pProfile, int nKind, int i);

#endif
