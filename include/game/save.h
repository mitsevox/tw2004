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

// The tour golfers: 174 pros (the table the 'PGST' stream object fills) and the player.
#define PGA_NUM_PROS        174
#define PGA_USER_GOLFER     174     // the player's golfer id
#define PGA_NUM_GOLFERS     175
#define PGA_MAX_ENTRANTS    128     // a tournament's field

// One golfer's season counts, from which each tour statistic is worked out (0x58 bytes). The
// profile keeps one per tour golfer (PGATourSimulation.c simulates the pros'); the player's round
// is counted in lbl_80205ED8 and added to the player's own (GameModeDriverPGATour fn_800EED0C).
// TW06: GM_Pga_StatCounts, which has three more counts (water saves, water hits, long putts)
// between nNonGIRPars and nEagles.
typedef struct PgaStatCounts {
    u16  nEvents;               // 0x00  tournaments started (counted in a first round)
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
    u16  nBirdies;              // 0x26  birdies or better. TW06: nBirdies
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
    s32  n44;                   // 0x44
    u16  nSeasonWins;           // 0x48  (0x80117E98; a new season clears 0x00-0x4A, 0x80117860)
    u8   nPlayerOfYearPoints;   // 0x4A  1 a win, 3 more where fn_800EFA70's nC is set. TW06: playerOfYearPoints
    u8   unk4B;
    u16  nConsecutiveCuts;      // 0x4C  TW06: nConsecutiveCuts
    u8   unk4E[2];
    u32  nCareerWinnings;       // 0x50  TW06: careerWinnings
    u16  nCareerWins;           // 0x54  (0x80117E98; kept from season to season)
    u8   unk56[2];
} PgaStatCounts;
LAYOUT_ASSERT(PgaStatCounts, 0x58);

// An entrant of the current tournament in a save profile (0x1C bytes; GetEntrantMCPtr). TW06:
// PgaTourSim_Entrant_MC_t, laid out differently.
typedef struct PgaEntrantMC {
    s16  nGolfer;               // 0x00  golfer id (PGA_USER_GOLFER: the player)
    s16  nTargetScore;          // 0x02  the four-round total the simulation aims at (0x80119E28)
    s32  aRoundStrokes[4];      // 0x04  per round (0x80117CB8)
    s32  bWasCut;               // 0x14  set with the golfer's consecutive-cuts count cleared (0x80117C50)
    s32  n18;                   // 0x18
} PgaEntrantMC;
LAYOUT_ASSERT(PgaEntrantMC, 0x1C);

// The current tournament's field in a save profile (0xE04 bytes, cleared as one by 0x80117AF8).
typedef struct PgaField {
    s16  nEntrants;             // 0x000  (GM_PgaTourSim_GetNumEntrants)
    s16  nWinner;               // 0x002  the winning entrant, -1 while the tournament is on (0x80117E98)
    PgaEntrantMC aEntrant[PGA_MAX_ENTRANTS];    // 0x004
} PgaField;
LAYOUT_ASSERT(PgaField, 0xE04);

// The PGA TOUR in a save profile (0x4E9C bytes, cleared as one by 0x801176C0): the season (TW06:
// PGATourSeason_t, which has 29 tournaments), every tour golfer's season counts, and the current
// tournament's field.
typedef struct TourSeason {
    s32  nSeason;               // 0x0000  0 = 2004. TW06: season
    s32  nEvent;                // 0x0004  the current tournament. TW06: eventID
    s32  nRound;                // 0x0008  its round. TW06: round
    SeasonEvent aEvent[31];     // 0x000C
    PgaStatCounts aStats[PGA_NUM_GOLFERS];      // 0x0468  per golfer id
    PgaField field;             // 0x4090
    u16  n4E94;                 // 0x4E94  counts the tournaments started
    u8   unk4E96[2];
    u16  n4E98;                 // 0x4E98  a run of tour rounds, counted on each 18th hole
                                //         (GameModeDriverPGATour_EndHole); reset to 0 when the run breaks
    u8   unk4E9A[2];
} TourSeason;
LAYOUT_ASSERT(TourSeason, 0x4E9C);

// A saved custom round (0x70 bytes): 18 holes, each a hole number and the course it is from.
// A new profile has three, emptied by the profile setup at 0x80057C88.
typedef struct SavedRound {
    s8   n0;                    // 0x00  cleared by the setup; set by a menu message (fn_80083860)
    char szName[0x14];          // 0x01  the round's name, shown as its course (GameUICommands.c)
    s8   n15;                   // 0x15  set to 1 by the setup; menu messages set and read it
                                //       (read signed: fn_80080C2C)
    s8   nHoleNum[18];          // 0x16  -1 = none
    s32  nCourse[18];           // 0x28
} SavedRound;

#define NUM_SAVED_ROUNDS 3      // the setup's loop count

// A PGA TOUR tournament won, in a save profile (8 bytes): filled in when the player finishes first
// (GameModeDriverPGATour fn_800EEA3C).
typedef struct TourWin {
    Award award;                // 0x0  won, and the day (fn_800D7770)
    u16  nScore;                // 0x4  the player's score (fn_801191D0, as SeasonEvent.nUserScore)
    s16  n6;                    // 0x6  the tournament's aPrize[bracket][1]
} TourWin;

// One entry of SaveProfile.a1054C: a switch and a value (our name). lbl_80281DF0 is one more,
// outside the profiles: user.c clears it and the code at 0x80057D64 copies it into a new profile's
// first entry.
typedef struct SaveLockEntry {
    u8   b;                     // 0x0
    u8   unk1;
    s16  n;                     // 0x2
} SaveLockEntry;
LAYOUT_ASSERT(SaveLockEntry, 4);

// One save profile (0x10600 bytes).
typedef struct SaveProfile {
    u8   bActive;               // 0x00000  1: the slot holds a profile; payouts are scaled and awards given only then
    char szName[0x1C - 0x1];    // 0x00001  the profile's name, compared with the record holders'
    u8   aGolferUnlocked[30];   // 0x0001C  per golfer (fn_80058278 sets, fn_8005832C tests)
    u8   aCourseUnlocked[23];   // 0x0003A  per course
    u8   aRewardUnlocked[0x64 - 0x51];  // 0x00051  per reward (fn_80058428 sets); the
                                //          "THEKITCHENSINK" code (0x80056568) sets the first 18
    s32  n64;                   // 0x00064  money: every payout is added (fn_800D3548); a course unlocks
                                //          when it reaches the course's price (fn_800D3A20)
    u8   unk68[4];
    s32  n6C;                   // 0x0006C  money: every payout is added here too (fn_800D3548)
    u8   b70;                   // 0x00070  set when an award is won, a round is counted or a challenge
                                //          starts; cleared when a round is set up (GameRound.c)
    u8   unk71[3];
    s32  n74;                   // 0x00074  stroke-play rounds counted
    s32  n78;                   // 0x00078  their strokes
    s32  n7C;                   // 0x0007C  full rounds counted
    u8   unk80[8];
    s32  n88;                   // 0x00088  drives counted (the tee shot of a par 4 or 5 off class-1
                                //          ground; fn_800D8FE4)
    s32  n8C;                   // 0x0008C  their distance together
    u8   unk90[0xA0 - 0x90];
    s32  nA0;                   // 0x000A0  the longest of those drives
    s32  nA4;                   // 0x000A4  the longest putt, in feet (fn_800D8FE4)
    s32  nA8;                   // 0x000A8  the best stroke-play round (0: none yet)
    s32  nAC;                   // 0x000AC  } read by menu messages (FE_MessageTable.c)
    s32  nB0;                   // 0x000B0  }
    s32  nB4;                   // 0x000B4  }
    s32  nB8;                   // 0x000B8  }
    s32  nBC;                   // 0x000BC  }
    s32  nC0;                   // 0x000C0  }
    s32  nC4;                   // 0x000C4  }
    TourWin aC8[31];           // 0x000C8  one per PGA TOUR tournament
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
    s32  nTourCardLevel;        // 0x05000  0..6: level 1 comes from the lessons (GameMode11), the rest
                                //          from fn_800D439C; it scales payouts (fn_800D7220)
    u8   a5004[71];             // 0x05004  per marked hole 0..70 (fn_800E1CE8): fn_800588F4's kind 0
    u8   unk504B;
    s32  a504C[71];             // 0x0504C  the same, fn_800588F4's kind 1
    u8   unk5168[4];
    s32  aMedal[29];            // 0x0516C  the best medal per challenge group (0 best, 3 none)
    u8   unk51E0[4];
    u16  aMedalDate[29];        // 0x051E4  the day each was earned (fn_800D2994)
    u8   unk521E[0x522F - 0x521E];
    u8   b522F;                 // 0x0522F  set by a menu command during a round (GameUICommands.c)
    SavedRound aSavedRound[NUM_SAVED_ROUNDS];   // 0x05230
    GolferRecord createdGolfer; // 0x05380  the created golfer's record (fn_80077A80: golfers
                                //          from FIRST_CREATED_GOLFER on are read here)
    u8   unk54C0[0x54C2 - 0x54C0];
    // The created golfer kept in this slot (golfer FIRST_CREATED_GOLFER + the slot), copied into
    // the session's PlayerProfile by Golfer.c.
    u8   n54C2;                 // 0x054C2  -> PlayerProfile.unk2
    u8   unk54C3[5];
    char szGolferNames[6][8];   // 0x054C8  -> PlayerProfile.szNames
    u8   nGolferOutfit;         // 0x054F8  -> PlayerProfile.nOutfit
    u8   nGolferBallType;       // 0x054F9  -> PlayerProfile.nBallType
    u8   unk54FA[0x5613 - 0x54FA];
    s8   n5613;                 // 0x05613  set by fn_8008DD34; FEgolferanim.c passes it to the
                                //          character (fn_8008EA44)
    u8   unk5614[0xB054 - 0x5614];
    // Four bit arrays with a bit per Create-A-Player asset (0x80057F18's loop over them all, which
    // also clears aB344 and aB4BC; fn_8001E9CC tests a bit).
    u32  aAssetLocked[94];      // 0x0B054  the asset was locked (fn_80078008) when last checked
    u32  aB1CC[94];             // 0x0B1CC  set where fn_80105C0C gives 0; an asset of lock kind 0
                                //          stays locked until the bit its fn_80105610 names is set
    u32  aB344[94];             // 0x0B344
    u32  aB4BC[94];             // 0x0B4BC
    TourSeason tour;            // 0x0B634
    u8   unk104D0[0x1054C - 0x104D0];
    SaveLockEntry a1054C[11];   // 0x1054C  cleared by the profile setup; fn_80078008's lock kinds
                                //          10 and 11 read them
    u8   a10578[4];             // 0x10578  marked holes 71..74: fn_800588F4's kind 0
    s32  a1057C[4];             // 0x1057C  and kind 1
    u8   unk1058C[0x10600 - 0x1058C];
} SaveProfile;
LAYOUT_ASSERT(SaveProfile, 0x10600);

extern SaveProfile* gpSaveData;
extern SaveProfile* lbl_80281DF4;       // unlocks that hold for every profile (the cheat codes set them)
extern SaveLockEntry lbl_80281DF0;
extern u32 lbl_801D5948[8];             // a bit array the code at 0x80056480 keeps; fn_80078008's lock
                                        // kind 6 tests bits 1..5 of it
extern u32 lbl_801D5908[16];            // a bit array the cheat codes of lbl_80188024 set (fn_800564AC)
extern s32 lbl_80189528[14];            // the golfers GM_GetGameProgress counts as unlockable
extern s32 lbl_801894D0[6];             // the courses GM_GetGameProgress counts as unlockable

// The password manager (0x80056480-0x80057F18; TW06's passwordmanager.cpp)
void fn_80056B8C(void);
void fn_80057364(int nSlot);    // sets up save profile nSlot

// GameManager.c: the profile's completion score (fn_800D439C raises the TOUR card level with it)
f32  GM_GetGameProgress(SaveProfile* pProfile);

// Earnings.c: the awards
u8   fn_800D7770(int nPlayer, Award* pAward);   // mark an award won today; 1 if it was not won before

// 0x800588F4: marked hole i's kind-0 byte (a5004/a10578) or kind-1 value (a504C/a1057C); -1 for
// another kind.
int  fn_800588F4(SaveProfile* pProfile, int nKind, int i);

#endif
