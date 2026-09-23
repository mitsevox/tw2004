// pgatour.h (our name): the PGA TOUR season of game mode 23 (GameModeDriverPGATour.c): the
// tournaments and their formats, loaded from the 'PGA' stream objects. The player's season is in
// the save profile (save.h, TourSeason).

#ifndef GAME_MODES_PGATOUR_H
#define GAME_MODES_PGATOUR_H

#include "game/save.h"

// One tournament of the season (0x64 bytes). TW06: GM_PgaTour_EventSlot_t, which has the name
// index first and the champion's name and score together further on.
typedef struct Tournament {
    s32  nName;                 // 0x00  offset into the names block. TW06: nameIdx
    s32  nTourEvent;            // 0x04  1-based entry in aTourEvent (0 = one round)
    u8   unk8[4];
    s32  nC;                    // 0x0C  nonzero for some tournaments: winning three of them gets its own
                                //       message (fn_800EE8C4)
    s32  n10;                   // 0x10
    char szChampName[0x10];     // 0x14  the champion before the season is played. TW06: champName
    s32  nChampScore;           // 0x24  TW06: champScore
    s16  aPrize[10][2];         // 0x28  per bracket (fn_800EF0E0): first prize and purse, in thousands
    u16  aStartDate[10];        // 0x50  per season (GameModeDriverPGATour_GetCurrentSeason). TW06: startDate
} Tournament;

// One round of a tournament's format (0xC bytes).
typedef struct TourRound {
    s32  nCourse;               // 0x0
    s32  nPinSet;               // 0x4  1-based: the pin position every hole uses (Session.nPinSet)
    s32  n8;                    // 0x8  -> GameOptions.n18 (fn_800EE0A0)
} TourRound;

// A tournament's format (0x54 bytes). TW06: Tournament_events_t, which starts with nRounds too.
typedef struct TourEvent {
    s32  nRounds;               // 0x00
    TourRound aRound[4];        // 0x04
    s32  nTeeSet;               // 0x34  every player's tee set (Session.nTeeSet)
    u8   unk38[8];
    s16  a40[10];               // 0x40  per bracket (fn_800EF0E0), passed to fn_801178C8
} TourEvent;

// A 'PGAp' record.
typedef struct PgaTriple {
    s32  n0;                    // 0x0
    s32  n4;                    // 0x4
    s32  n8;                    // 0x8
} PgaTriple;

// The tour's data, loaded from the 'PGA' stream objects. TW06: PGA_Master (GameModeDriverPGATour::m_PgaData).
typedef struct PgaData {
    Tournament aTournament[31]; // 0x0000  'PGAc'
    TourEvent  aTourEvent[31];  // 0x0C1C  'PGAt'
    u8         unk1648[0x6FC8 - 0x1648];
    PgaTriple  aTriple[11];     // 0x6FC8  'PGAp'
    char*      pNames;          // 0x704C  'PGAn'. TW06: pStrTable
    u8         unk7050[4];
} PgaData;

// The current round's statistics: cleared as each round of a tournament starts (fn_800EE2C8),
// added to the player's season counts in the profile as it ends (fn_800EED0C).
extern PgaStatCounts lbl_80205ED8;

typedef struct Pga80205F30 {
    u8   b0;                    // 0x0
    u8   unk1[3];
    s32  n4;                    // 0x4
    s32  n8;                    // 0x8
} Pga80205F30;

// GameModeDriverPGATour.c, as the career calendar (GameModeDriver.c) uses it
u8   GameModeDriverPGATour_GetEventByDate(u16 nDate, s32* pId, s32* pRound);
s32  GameModeDriverPGATour_GetSelectedEvent(s32* pRound);
s32  GameModeDriverPGATour_GetFinalEventOfSeason(void);
void fn_800EF9D0(s32 nEvent);
Tournament* fn_800EFA70(s32 i);
s32  GameModeDriverPGATour_GetRounds(s32 i);
u16  fn_800EFD38(s32 i);                // the tournament's start date
u16  GameModeDriverPGATour_GetEndDate(s32 i);
char* GameModeDriverPGATour_GetName(s32 i);
s32  fn_800EFE3C(s32 i);
int  fn_800F009C(void);                 // the leader's score in the current tournament
int  fn_800F018C(void);                 // the player's own score in it

// GameModeDriverPGATour.c, as the calendar's event details (EventInfo.c) use it
s32  GameModeDriverPGATour_GetCourses(Tournament* p, s32* pCourses);
void GameModeDriverPGATour_GetWinnerEarningsString(s32 i, char* pDst);
void GameModeDriverPGATour_GetCurrentEventLeader(char* pDst);
void GameModeDriverPGATour_GetPurseString(s32 i, char* pDst);
void GameModeDriverPGATour_GetUserFinishString(s32 i, char* pDst);
void GameModeDriverPGATour_GetChamp(s32 i, char* pDst);
s32  GameModeDriverPGATour_GetChampScore(s32 i);
s32  fn_800F0428(s32 nPlayer);

#endif
