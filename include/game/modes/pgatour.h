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
    s32  bIsAMajor;             // 0x0C  a major: its winner gets 3 more Player of the Year points,
                                //       and winning three gets its own movie
                                //       (GameModeDriverPGATour_PlayEndOfGameMovies). TW06: isAMajor
    s32  nTextureID;            // 0x10  its icon in the calendar. TW06: textureID
    char szChampName[0x10];     // 0x14  the champion before the season is played. TW06: champName
    s32  nChampScore;           // 0x24  TW06: champScore
    s16  aPrize[10][2];         // 0x28  per bracket (GameModeDriverPGATour_GetCurrentBracket): the
                                //       purse [0] and the first prize [1], in thousands
    u16  aStartDate[10];        // 0x50  per season (GameModeDriverPGATour_GetCurrentSeason). TW06: startDate
} Tournament;

// One round of a tournament's format (0xC bytes).
typedef struct TourRound {
    s32  nCourse;               // 0x0
    s32  nPinSet;               // 0x4  1-based: the pin position every hole uses (Session.nPinSet)
    s32  n8;                    // 0x8  -> GameOptions.n18 (GameModeDriverPGATour_SetTournament)
} TourRound;

// A tournament's format (0x54 bytes). TW06: Tournament_events_t, which starts with nRounds too.
typedef struct TourEvent {
    s32  nRounds;               // 0x00
    TourRound aRound[4];        // 0x04
    s32  nTeeSet;               // 0x34  every player's tee set (Session.nTeeSet)
    u8   unk38[8];
    s16  aFieldLowScore[10];    // 0x40  per bracket (GameModeDriverPGATour_GetCurrentBracket): the
                                //       simulated field's lowest four-round score, over par
                                //       (GM_PgaTourSim_SimRound's targets: fn_80118B0C)
} TourEvent;

// A sponsorship offer (a 'PGAp' record). TW06: GM_PgaTour_SponsorshipSlot_t.
typedef struct PgaSponsorship {
    s32  nProgress;             // 0x0  offered once the profile's game progress reaches it
                                //      (GM_GetGameProgress). TW06: gameCompletion
    s32  nStartCash;            // 0x4  paid when it is signed. TW06: startCashBonus
    s32  nBonusCash;            // 0x8  paid for each of the sponsor's items worn
} PgaSponsorship;

// The tour's data, loaded from the 'PGA' stream objects. TW06: PGA_Master (GameModeDriverPGATour::m_PgaData).
typedef struct PgaData {
    Tournament aTournament[31]; // 0x0000  'PGAc'
    TourEvent  aTourEvent[31];  // 0x0C1C  'PGAt'
    u8         unk1648[0x6FC8 - 0x1648];
    PgaSponsorship aSponsorship[11];    // 0x6FC8  'PGAp'. TW06: sponsorships
    char*      pNames;          // 0x704C  'PGAn'. TW06: pStrTable
    u8         unk7050[4];
} PgaData;

// The current round's statistics: cleared as each round of a tournament starts (GameModeDriverPGATour_PrepareForTeeOff),
// added to the player's season counts in the profile as it ends (GameModeDriverPGATour_CommitUserRoundStatCounts).
extern PgaStatCounts gPgaRoundStats;

// The player's prize in the tournament just played (gPgaWinInfo). EA's name (TW06, TW07).
typedef struct PgaTour_WinInfo {
    u8   bPlaced;               // 0x0  the player was paid (GameModeDriverPGATour_AwardMoney)
    u8   unk1[3];
    s32  nPosition;             // 0x4  the player's place
    s32  nWinnings;             // 0x8  the money won
} PgaTour_WinInfo;

// GameModeDriverPGATour.c, as the career calendar (GameModeDriver.c) uses it
u8   GameModeDriverPGATour_GetEventByDate(u16 nDate, s32* pId, s32* pRound);
s32  GameModeDriverPGATour_GetSelectedEvent(s32* pRound);
s32  GameModeDriverPGATour_GetFinalEventOfSeason(void);
void GameModeDriverPGATour_SkipToEvent(s32 nEvent);
Tournament* GameModeDriverPGATour_GetEventInfo(s32 i);
Tournament* GM_PgaTourMode_GetEventInfoByDate(u16 nDate);
s32  GameModeDriverPGATour_GetRounds(s32 i);
u16  GameModeDriverPGATour_GetStartDate(s32 i);                // the tournament's start date
u16  GameModeDriverPGATour_GetEndDate(s32 i);
char* GameModeDriverPGATour_GetName(s32 i);
s32  GameModeDriverPGATour_GetTextureID(s32 i);
int  GameModeDriverPGATour_GetCurrentLeaderScore(void);                 // the leader's score in the current tournament
int  GameModeDriverPGATour_GetUserScore(s32 nEvent);           // the player's own score in it (nEvent is not used)

// GameModeDriverPGATour.c, as the tour simulation (PGATourSimulation.c) uses it
void GameModeDriverPGATour_AwardMoney(int nPlayer, s32 nCash);   // notes the player's prize
s32  GameModeDriverPGATour_GetNextEvent(void);  // -1 when the season is over
s32  GameModeDriverPGATour_ComputePurseForBracket(s32 i, s32 k);
s32  GameModeDriverPGATour_ComputeFirstPrizeForBracket(s32 i, s32 k);

// GameModeDriverPGATour.c, as the calendar's event details (EventInfo.c) use it
s32  GameModeDriverPGATour_GetCourses(Tournament* p, s32* pCourses);
void GameModeDriverPGATour_GetPurseString(s32 i, char* pDst);
void GameModeDriverPGATour_GetCurrentEventLeader(char* pDst);
void GameModeDriverPGATour_GetWinnerEarningsString(s32 i, char* pDst);
void GameModeDriverPGATour_GetUserFinishString(s32 i, char* pDst);
void GameModeDriverPGATour_GetChamp(s32 i, char* pDst);
s32  GameModeDriverPGATour_GetChampScore(s32 i);
s32  GameModeDriverPGATour_GetUsersCurrentEventID(s32 nPlayer);

// GameModeDriverPGATour.c, as FE_CrAPDB.c uses it
s32  GameModeDriverPGATour_GetSponsorshipBonusCash(s32 i);                // aSponsorship[i].nBonusCash

// GameModeDriverPGATour.c, as the PGA TOUR menus (FE_PGATourMessages.c) use it
void GameModeDriverPGATour_CheckAdvanceTournament(s32 nPlayer);
s32  GM_PgaTourMode_GetNEvents(void);                 // the number of tournaments (31)
s32  GameModeDriverPGATour_AdvanceSeason(void);                 // the next season: 0 after the tenth
s32  GameModeDriverPGATour_GetCurrentSeasonYear(void);                 // the current season's year
s32  GameModeDriverPGATour_GetCurrentEventID(void);
s32  GameModeDriverPGATour_GetSponsorshipProgress(s32 i);                // aSponsorship[i].nProgress
s32  GameModeDriverPGATour_GetSponsorshipStartCash(s32 i);                // aSponsorship[i].nStartCash

#endif
