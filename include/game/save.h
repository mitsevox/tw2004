// save.h (our name): the save profiles. gpSaveData points to one profile per player slot
// (PlayerNumber_t), 0x10600 bytes each: the awards, the saved replays, the challenge medals and the
// PGA TOUR season. Earnings.c keeps most of it. The profile is a memory-card record: a port reads
// and writes it field by field, big-endian, never by copying the struct.

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

// One save profile (0x10600 bytes).
typedef struct SaveProfile {
    u8   b0;                    // 0x00000  payouts are scaled (and awards given, at 1) only when set
    u8   unk1[0x70 - 0x1];
    u8   b70;                   // 0x00070  set when an award is won, a round is counted or a challenge starts
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
    u8   unk1C0[0x20C - 0x1C0];
    Award aRTEAward[75];        // 0x0020C  per real-time event id. TW06: rteEventAwardInfo
    u8   unk338[0x39C - 0x338];
    Award aAward[39];           // 0x0039C
    u8   aReplay[5][0xF28];     // 0x00438  a Replay each, saved with awards 0, 6, 9, 3 and 13
    s32  n5000;                 // 0x05000  the TOUR card level, 0..6 (Earnings); GameMode5 needs 1 or more
    u8   unk5004[0x516C - 0x5004];
    s32  aMedal[29];            // 0x0516C  the best medal per challenge group (0 best, 3 none)
    u8   unk51E0[4];
    u16  aMedalDate[29];        // 0x051E4  the day each was earned (fn_800D2994)
    u8   unk521E[0xB634 - 0x521E];
    TourSeason tour;            // 0x0B634
    u8   unkBA9C[0x104C8 - 0xBA9C];
    u16  n104C8;                // 0x104C8  counts the tournaments started
    u8   unk104CA[0x10600 - 0x104CA];
} SaveProfile;

extern SaveProfile* gpSaveData;

// Earnings.c: the awards
s32  fn_800D7770(int nPlayer, Award* pAward);   // mark an award won today; 1 if it was not won before

#endif
