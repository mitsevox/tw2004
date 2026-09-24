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
    u32  n44;                   // 0x44  winnings this month: the leader at a month's end gets that
                                //       month's award, then every golfer's is cleared (fn_801180C4)
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
    u16  n4E96;                 // 0x4E96  a run of the player's tournament wins (fn_801180C4);
                                //         reset to 0 when the player finishes elsewhere
    u16  n4E98;                 // 0x4E98  a run of tour rounds, counted on each 18th hole
                                //         (GameModeDriverPGATour_EndHole); reset to 0 when the run breaks
    u16  n4E9A;                 // 0x4E9A  wins of the tournaments whose Tournament.nC is set
                                //         (fn_801180C4)
} TourSeason;
LAYOUT_ASSERT(TourSeason, 0x4E9C);

// A saved custom round (0x70 bytes): 18 holes, each a hole number and the course it is from.
// A new profile has three, emptied by the profile setup at 0x80057C88.
typedef struct SavedRound {
    u8   n0;                    // 0x00  cleared by the setup; set by a menu message (fn_80083860);
                                //       read unsigned (FE_MessageTable.c fn_800807DC)
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
    u16  n6;                    // 0x6  the tournament's aPrize[bracket][1] (thousands of dollars:
                                //      fn_8010F440 reads it unsigned)
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

// A logo's shape: 64 x 64 (drawn into the texture "__LogoSquare") or 128 x 32 ("__LogoRect").
#define LOGO_SQUARE 0
#define LOGO_RECT   1

// A saved user logo (0x1022 bytes): the profile holds five and the logo editor (FE_LogoDesign.c,
// fn_8010FB70) edits the one its LogoEdit.n0 names.
typedef struct LogoRecord {
    u8   aPixels[0x1000];       // 0x0000  64 x 64 or 128 x 32 colour indexes
    char szName[0x20];          // 0x1000
    u8   b1020;                 // 0x1020
    u8   nShape;                // 0x1021  LOGO_SQUARE or LOGO_RECT
} LogoRecord;
LAYOUT_ASSERT(LogoRecord, 0x1022);

// A part's (or a set's) choice: its variant and that variant's option. For a set, the variant is
// an entry of its p7C run and the option one of that entry's p8C run.
typedef struct SkinChoice {
    s32  nVariant;              // 0x0  -1 none
    s32  nOption;               // 0x4  -1 none
} SkinChoice;
LAYOUT_ASSERT(SkinChoice, 8);

// A golfer's look as kept outside the skins: SaveProfile.choices from 0x5500 (the created golfer),
// or what Character.pChoices points at. fn_800CC1EC fills it from the body's skin or the skin from
// it; fn_800CC408 gives the other six skins theirs; char_tex_manager.c puts its logos on the model.
typedef struct SkinChoices {
    u8   unk0[0x113];
    s8   n113;                  // 0x113  set by fn_8008DD34; FEgolferanim.c passes it to the
                                //        character (fn_8008EA44)
    SkinChoice aParts[40];      // 0x114  the body's, per part (-1 -1 throughout: not set yet)
    SkinChoice aSets[116];      // 0x254  the body's, per set
    SkinChoice aSkinParts[6][10];   // 0x5F4  the six skins' of CharSkinSet
    SkinChoice aSkinSets[6][10];    // 0x7D4
    u8   a9B4[26];              // 0x9B4  the 26 sliders (fn_8010E4DC; a menu message reads slider n
                                //        signed); set to 50 each when FE_CrAP_InitCrAPInfo clears
                                //        the profile's 0x5500..0xB634 (fn_80058208)
    u8   unk9CE[2];
    LogoRecord aLogo[5];        // 0x9D0  the user logos ("_usrtextr0".."_usrtextr4")
    u8   n5A7A;                 // 0x5A7A  (the profile's 0xAF7A) set by a menu message, which passes
                                //         it to fn_80103B8C (s8); read back signed
    u8   unk5A7B;
} SkinChoices;
LAYOUT_ASSERT(SkinChoices, 0x5A7C);

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
    s32  n68;                   // 0x00068  cleared by the profile setup (fn_80057438)
    s32  n6C;                  // 0x0006C  money: every payout is added here too (fn_800D3548)
    u8   b70;                   // 0x00070  set when an award is won, a round is counted or a challenge
                                //          starts; cleared when a round is set up (GameRound.c)
    u8   unk71[3];
    s32  n74;                   // 0x00074  stroke-play rounds counted
    s32  n78;                   // 0x00078  their strokes
    s32  n7C;                   // 0x0007C  full rounds counted
    s32  n80;                  // 0x00080  holes whose putts are counted (fewer than 10; fn_800D9458)
    s32  n84;                   // 0x00084  their putts
    s32  n88;                  // 0x00088  drives counted (the tee shot of a par 4 or 5 off class-1
                                //          ground; fn_800D8FE4)
    s32  n8C;                   // 0x0008C  their distance together
    s32  n90;                   // 0x00090  } par 4 and 5 holes counted (fn_800D9458), and those where
    s32  n94;                   // 0x00094  } the player's b2E4 was set
    s32  n98;                   // 0x00098  } every hole counted, and those where the player's b2F6
    s32  n9C;                   // 0x0009C  } was set
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
    Award a1C0[16];            // 0x001C0  the won ones count for GM_GetBonusProgress. 0..11: Player
                                //          of the Month, per month (the tour's month money leader,
                                //          n44; FE_PGATourMessages.c fn_8010F3A4); 12..15: the
                                //          four trophies (both awarded by PGATourSimulation
                                //          fn_801180C4; GameMode22 fn_8012597C reads their days)
    Award a200[3];              // 0x00200  the player's career winnings first, in the top 5 and in the
                                //          top 25 of the tour (PGATourSimulation fn_801180C4)
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
    s32  n5168;                 // 0x05168  set to 3 with the medals by the profile setup
    s32  aMedal[29];           // 0x0516C  the best medal per challenge group (0 best, 3 none)
    u8   unk51E0[4];
    u16  aMedalDate[29];        // 0x051E4  the day each was earned (fn_800D2994)
    u8   unk521E[0x5220 - 0x521E];
    u8   aTipSeen[15];          // 0x05220  per swing tip test: its full tip was shown (fn_800D1DAC)
    u8   b522F;                 // 0x0522F  set by a menu command during a round (GameUICommands.c);
                                //          when set, fn_800D1DAC only shows short tips
    SavedRound aSavedRound[NUM_SAVED_ROUNDS];   // 0x05230
    GolferRecord createdGolfer; // 0x05380  the created golfer's record (fn_80077A80: golfers
                                //          from FIRST_CREATED_GOLFER on are read here)
    u8   unk54C0[0x54C2 - 0x54C0];
    // The created golfer kept in this slot (golfer FIRST_CREATED_GOLFER + the slot), copied into
    // the session's PlayerProfile by Golfer.c.
    u8   n54C2;                 // 0x054C2  -> PlayerProfile.unk2
    u8   unk54C3[5];
    u64  aGolferNames[6];       // 0x054C8  -> PlayerProfile.aNames
    s8   nGolferOutfit;         // 0x054F8  -> PlayerProfile.nOutfit; fn_801069AC stores a ball's
                                //          index there (fn_800484F4, -1: none)
    u8   nGolferBallType;       // 0x054F9  -> PlayerProfile.nBallType
    u8   unk54FA[0x5500 - 0x54FA];
    // The created golfer's look: the body's parts and sets (fn_80103D6C), its six other skins'
    // (fn_80103DE0), its sliders and its logos. char.c hands it to the character (fn_8001D4A4
    // passes fn_80077ACC() + 0x5500 as a SkinChoices*).
    SkinChoices choices;        // 0x05500
    s8   nDateMonth;            // 0x0AF7C  } a date, set and read by menu messages packed as
    s8   nDateDay;              // 0x0AF7D  } fn_80078604 packs it (FE_CrAPMessages.c
    s16  nDateYear;             // 0x0AF7E  } fn_80108178, fn_80108244)
    s32  aAF80[53];             // 0x0AF80  per slot: a Create-A-Player asset (FE_CrAPDB.c
                                //          fn_80103D14), -1 for none; an asset's n2E is its slot
    // Four bit arrays with a bit per Create-A-Player asset (0x80057F18's loop over them all, which
    // also clears aB344 and aB4BC; fn_8001E9CC tests a bit).
    u32  aAssetLocked[94];      // 0x0B054  the asset was locked (fn_80078008) when last checked
    u32  aB1CC[94];             // 0x0B1CC  set where fn_80105C0C gives 0; an asset of lock kind 0
                                //          stays locked until the bit its fn_80105610 names is set
    u32  aB344[94];             // 0x0B344
    u32  aB4BC[94];             // 0x0B4BC
    TourSeason tour;            // 0x0B634
    u8   a104D0[118];           // 0x104D0  per real-time event (fn_800F0FBC's ids); GameMode22 fn_80125AA4
                                //          counts the nonzero ones in a month
    u8   unk10546[0x10548 - 0x10546];
    u32  a10548[1];             // 0x10548  a bit array: FE_CrAPMessages.c's fn_80108E4C tests bit n; fn_80058304 tests one (bit 1 for FE_Manager)
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
void fn_80057ED0(SaveProfile* pProfile, const char* pName);     // PasswordManager.c: name it

// GameManager.c: the profile's completion score (fn_800D439C raises the TOUR card level with it)
f32  GM_GetGameProgress(SaveProfile* pProfile);

// Earnings.c: the awards
u8   fn_800D7770(int nPlayer, Award* pAward);   // mark an award won today; 1 if it was not won before

// fe_craputils.c (TW06's FE_CrAP_ utilities)
void FE_CrAP_InitCrAPInfo(SaveProfile* pProfile);
u8   fn_80058304(SaveProfile* pProfile, int nBit);  // bit nBit of pProfile->u10548
void fn_80058560(SaveProfile* pProfile, int nKind, char* pName);  // add pName to list nKind
void fn_80058624(SaveProfile* pProfile, int nKind, char* pName);  // take pName out of list nKind
u8   fn_800587A8(SaveProfile* pProfile, int nKind, char* pName);  // pName is in list nKind (0..2)
void fn_800588D4(s16 n);            // set lbl_80281DF0 (switched on, value n)
int  fn_800588E8(void);             // lbl_80281DF0's value (callers take it without extsh)

// 0x800588F4: marked hole i's kind-0 byte (a5004/a10578) or kind-1 value (a504C/a1057C); -1 for
// another kind.
int  fn_800588F4(SaveProfile* pProfile, int nKind, int i);
void fn_8005897C(SaveProfile* pProfile, int nKind, int i, int nValue);  // and set it

#endif
