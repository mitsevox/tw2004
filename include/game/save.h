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
    u16  nDate;                 // 0x2  the day it was won (CalDate_GetToday). TW06: dateWon
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
// is counted in gPgaRoundStats and added to the player's own
// (GameModeDriverPGATour_CommitUserRoundStatCounts).
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
    u32  nMonthWinnings;        // 0x44  the leader at a month's end gets that month's award, then
                                //       every golfer's is cleared
                                //       (GM_PgaTourSim_CheckEndOfTournamentAward)
    u16  nSeasonWins;           // 0x48  (0x80117E98; a new season clears 0x00-0x4A, 0x80117860)
    u8   nPlayerOfYearPoints;   // 0x4A  1 a win, 3 more where GameModeDriverPGATour_GetEventInfo's
                                //       nC is set. TW06: playerOfYearPoints
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
    s32  nWinnings;             // 0x18  its share of the purse (SplitWinnings)
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
    u16  nEventsStarted;        // 0x4E94  tournaments started (counted in a first round,
                                //         GameModeDriverPGATour_EndGame)
    u16  nWinStreak;            // 0x4E96  tournaments won in a row: +1 for a win, 0 when the
                                //         player plays one and does not win
                                //         (GM_PgaTourSim_CheckEndOfTournamentAward)
    u16  nParRoundStreak;       // 0x4E98  rounds in a row at or under the course's par, counted
                                //         on each 18th hole (GameModeDriverPGATour_EndHole); 0
                                //         after a round over par
    u16  nMajorWins;            // 0x4E9A  majors won (Tournament.bIsAMajor;
                                //         GM_PgaTourSim_CheckEndOfTournamentAward)
} TourSeason;
LAYOUT_ASSERT(TourSeason, 0x4E9C);

// A saved custom round (0x70 bytes): 18 holes, each a hole number and the course it is from.
// A new profile has three, emptied by the profile setup at 0x80057C88.
typedef struct SavedRound {
    u8   bInUse;                // 0x00  the custom round is in use (GM_vSetSavedRoundInUse,
                                //       GM_vIsCustomRoundUsed); cleared by the setup
    char szName[0x14];          // 0x01  the round's name, shown as its course (GameUICommands.c)
    s8   n15;                   // 0x15  set to 1 by the setup; menu messages set and read it
                                //       (read signed: GM_vGetCustomRoundN15)
    s8   nHoleNum[18];          // 0x16  -1 = none
    s32  nCourse[18];           // 0x28
} SavedRound;

#define NUM_SAVED_ROUNDS 3      // the setup's loop count

// A PGA TOUR tournament won, in a save profile (8 bytes): filled in when the player finishes first
// (GameModeDriverPGATour_EndTournament).
typedef struct TourWin {
    Award award;                // 0x0  won, and the day (GM_Earnings_GiveAwardToUser)
    u16  nScore;                // 0x4  the player's score (GM_PgaTourSim_GetTotalScoreFromEntrantID,
                                //      as SeasonEvent.nUserScore)
    u16  n6;                    // 0x6  the tournament's aPrize[bracket][1] (thousands of dollars:
                                //      PGATourWins_GetDetails reads it unsigned)
} TourWin;

// A sponsorship slot in a save profile (our name; the slots' terms are the tour table's
// PgaSponsorship, TW06 GM_PgaTour_SponsorshipSlot_t). Slot i is signed once the game progress
// reaches its level (FE_PGATourMessages.c PGASponsor_SignNext), with a sponsor picked at random; each worn
// Create-A-Player asset of that sponsor then pays the slot's bonus cash
// (FE_CrAP_CollectSponsorshipItems). lbl_80281DF0 is one more, outside the profiles: the sponsor
// a new profile starts with (user.c clears it; the profile setup at 0x80057D64 copies it into
// slot 0).
typedef struct SponsorSlot {
    u8   bSigned;               // 0x0  the slot is signed
    u8   unk1;
    s16  nSponsor;              // 0x2  the sponsor: a Create-A-Player asset's n2C (one of
                                //      lbl_80193CFC's 11)
} SponsorSlot;
LAYOUT_ASSERT(SponsorSlot, 4);

// A logo's shape: 64 x 64 (drawn into the texture "__LogoSquare") or 128 x 32 ("__LogoRect").
#define LOGO_SQUARE 0
#define LOGO_RECT   1

// A saved user logo (0x1022 bytes): the profile holds five and the logo editor (FE_LogoDesign.c,
// FE_LogoDesign_GetCurrentLogo) edits the one its LogoEdit.nLogo names.
typedef struct LogoRecord {
    u8   aPixels[0x1000];       // 0x0000  64 x 64 or 128 x 32 colour indexes
    char szName[0x20];          // 0x1000
    u8   bSaved;                // 0x1020  kept by GM_vSaveLogo (clear: not made yet)
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
// or what Character.pChoices points at. SkinPart_ApplyBodyChoices fills it from the body's skin
// or the skin from it; SkinPart_ApplyClubChoices gives the other six skins theirs;
// char_tex_manager.c puts its logos on the model.
typedef struct SkinChoices {
    // The created golfer's three custom animation lists (fe_craputils.c FE_CrAP_AddCustomAnimation,
    // FE_CrAP_RemoveCustomAnimation, FE_CrAP_IsCustomAnimationSelected; skalib.c
    // AnimLib_ApplyCustomAnims plays them): lists 0 and 1 hold up to 8 animation names with a
    // count, list 2 one name (its count is 0 or 1).
    s8   nCustomAnims0;         // 0x000  names in aszCustomAnims0
    char aszCustomAnims0[8][0x10];  // 0x001  list 0: group 5 style 7's reactions
    s8   nCustomAnims1;         // 0x081  names in aszCustomAnims1
    char aszCustomAnims1[8][0x10];  // 0x082  list 1: group 5 style 1's reactions
    s8   nCustomAnims2;         // 0x102  1: szCustomAnim2 is set
    char szCustomAnim2[0x10];   // 0x103  list 2: group 0's clip
    s8   bLeftHanded;           // 0x113  the created golfer is left-handed
                                //        (FE_SetProfileLeftHanded; FEgolferanim.c hands it on with
                                //        FE_CRAPSetHandednessForScreen)
    SkinChoice aParts[40];      // 0x114  the body's, per part (-1 -1 throughout: not set yet)
    SkinChoice aSets[116];      // 0x254  the body's, per set
    SkinChoice aSkinParts[6][10];   // 0x5F4  the six skins' of CharSkinSet
    SkinChoice aSkinSets[6][10];    // 0x7D4
    u8   a9B4[26];              // 0x9B4  the 26 sliders (CharSlider_UpdateCharacterBasedOnSliderValues;
                                //        a menu message reads slider n signed); set to 50 each when
                                //        FE_CrAP_InitCrAPInfo clears
                                //        the profile's 0x5500..0xB634 (FE_CrAP_ResetSliders)
    u8   unk9CE[2];
    LogoRecord aLogo[5];        // 0x9D0  the user logos ("_usrtextr0".."_usrtextr4")
    u8   nGender;               // 0x5A7A  (the profile's 0xAF7A) the created golfer's gender
                                //         (GM_vSetCrAPGolferInfo, which makes it the database's
                                //         current one); read back signed
    u8   unk5A7B;
} SkinChoices;
LAYOUT_ASSERT(SkinChoices, 0x5A7C);

// A saved shot (gReplayData, 0x801D6030): the seed, player 0 as it was, and the conditions.
typedef struct Replay {
    u32    nSeed;               // 0x000
    u8     unk4[4];
    Player player;              // 0x008  player 0 before the shot
    s32    nCourse;             // 0xF00
    s16    nHole;               // 0xF04
    s8     nTeeSet;             // 0xF06
    s8     nPinSet;             // 0xF07  the session's pin set when the shot was saved
    f32    fF08;                // 0xF08
    f32    fF0C;                // 0xF0C
    u8     bF10;                // 0xF10  in-flight replays are on (GameMode.c)
    u8     unkF11;
    s16    nWeather;            // 0xF12  the weather option (option 2, changing, saved as 3 or 4)
    s16    nWeatherAmount;      // 0xF14  the weather amount x 100; forced (PlayNow_ForceWeather)
                                //        for options 1..3 (GameModeReplay.c)
    s16    nWindDir;            // 0xF16
    s16    nWindSpeed;          // 0xF18
    s16    nF1A;                // 0xF1A  -> Physics_SetGreenSpeedByType
    s16    nF1C;                // 0xF1C  -> Physics_SetFairwaySpeedByType
    s16    nF1E;                // 0xF1E  -> Physics_SetRoughLengthByType
    s16    nStrokes;            // 0xF20  strokes on the hole before the shot
} Replay;
LAYOUT_ASSERT(Replay, 0xF28);

// One save profile (0x10600 bytes).
typedef struct SaveProfile {
    u8   bActive;               // 0x00000  1: the slot holds a profile; payouts are scaled and
                                //          awards given only then
    char szName[0x1C - 0x1];    // 0x00001  the profile's name, compared with the record holders'
    u8   aGolferUnlocked[30];   // 0x0001C  per golfer (UserInfo_UnlockGolfer sets,
                                //          UserInfo_IsGolferAvailable tests)
    u8   aCourseUnlocked[23];   // 0x0003A  per course
    u8   aRewardUnlocked[0x64 - 0x51];  // 0x00051  per reward (UserInfo_UnlockReward sets); the
                                //          "THEKITCHENSINK" code (0x80056568) sets the first 18
    s32  nTotalCash;            // 0x00064  all the money ever won: every payout is added
                                //          (GM_Earnings_AwardMoney), nothing taken off; a course
                                //          unlocks when it reaches the course's price
                                //          (GM_Earnings_CheckUnlockCourses)
    s32  n68;                   // 0x00068  cleared by the profile setup (SaveProfile_InitNew)
    s32  nCurrentCash;          // 0x0006C  the money to spend: every payout and a sponsorship's
                                //          start cash are added, the menus set it (the pro shop)
    u8   bChanged;              // 0x00070  set when a statistic, an award or money changes or a
                                //          challenge starts; cleared when a round is set up
                                //          (GameRound.c)
    u8   unk71[3];
    // The profile's statistics (GM_RecordIndividualShotStats / HoleStats / RoundStats), shown by
    // the menus (FE_MessageTable.c); counted only with mulligans off.
    s32  nStrokeRounds;         // 0x00074  stroke-play rounds counted
    s32  nStrokeRoundStrokes;   // 0x00078  their strokes
    s32  nRounds;               // 0x0007C  full rounds counted
    s32  nPuttHoles;            // 0x00080  holes whose putts are counted (fewer than 10)
    s32  nPutts;                // 0x00084  their putts
    s32  nDrives;               // 0x00088  drives counted (the tee shot of a par 4 or 5 off class-1
                                //          ground)
    s32  nDriveDistance;        // 0x0008C  their yards together
    s32  nFairways;             // 0x00090  par 4 and 5 holes counted
    s32  nFairwaysHit;          // 0x00094  those where the player's bFairwayHit was set
    s32  nHoles;                // 0x00098  every hole counted
    s32  nGreensHit;            // 0x0009C  those where the player's bGreenInReg was set
    s32  nLongestDrive;         // 0x000A0  the longest of those drives, in yards
    s32  nLongestPutt;          // 0x000A4  the longest putt holed, in feet
    s32  nBestRound;            // 0x000A8  the best stroke-play round (0: none yet)
    s32  nHolesInOne;           // 0x000AC  holes by their score: a hole in one
    s32  nAlbatrosses;          // 0x000B0  3 under par
    s32  nEagles;               // 0x000B4  2 under
    s32  nBirdies;              // 0x000B8  1 under
    s32  nPars;                 // 0x000BC
    s32  nBogeys;               // 0x000C0  1 over
    s32  nDoubleBogeys;         // 0x000C4  2 or more over
    TourWin aC8[31];           // 0x000C8  one per PGA TOUR tournament
    Award aTourAward[16];       // 0x001C0  the won ones count for GM_GetBonusProgress. 0..11: Player
                                //          of the Month, per month (the tour's month money leader,
                                //          nMonthWinnings; FE_PGATourMessages.c
                                //          TrophyRoom_GetPlayerOfMonthStatus); 12..15: the four
                                //          trophies (both awarded by
                                //          GM_PgaTourSim_CheckEndOfTournamentAward;
                                //          TrophyRoom_GetTourTrophy reads their days)
    Award aMoneyListAward[3];   // 0x00200  the player's career winnings first, in the top 5 and in
                                //          the top 25 of the tour's money list
                                //          (GM_PgaTourSim_CheckEndOfTournamentAward; TW07
                                //          AwardInfoPGAMoneyList)
    Award aRTEAward[75];        // 0x0020C  per real-time event id. TW06: rteEventAwardInfo
    Award aLadderAward[25];     // 0x00338  per ladder event (LadderedMode.c);
                                //          UserInfo_GetNumLadderEventsWon's earnings rating counts
                                //          the won ones
    Award aAward[39];           // 0x0039C  the trophy balls (GM_Earnings_AwardTrophyBall): 0..22
                                //          count in GM_GetGameProgress, 23..38 (the PGA TOUR and
                                //          career awards) in GM_GetBonusProgress
    Replay aReplay[5];          // 0x00438  a Replay each, saved with awards 0, 6, 9, 3 and 13
    s32  nTourCardLevel;        // 0x05000  0..6: level 1 comes from the lessons (GameMode11), the rest
                                //          from GM_Earnings_PayRoundGoals; it scales payouts
                                //          (GM_Earnings_ComputeTOURCardModifiers)
    u8   a5004[71];             // 0x05004  per par-5 hole 0..70 (GM_ConvertCourseAndHoleToPar5EagleIndex):
                                //          1 once the profile has eagled it (Earnings.c);
                                //          UserInfo_GetPar5EagleStat's kind 0
    u8   unk504B;
    s32  a504C[71];             // 0x0504C  the same holes: the date of that eagle (packed by FE_DateToInt);
                                //          UserInfo_GetPar5EagleStat's kind 1
    s32  n5168;                 // 0x05168  set to 3 with the medals by the profile setup
    s32  aMedal[29];           // 0x0516C  the best medal per challenge group (0 best, 3 none)
    u8   unk51E0[4];
    u16  aMedalDate[29];        // 0x051E4  the day each was earned (CalDate_GetToday)
    u8   unk521E[0x5220 - 0x521E];
    u8   aTipSeen[15];          // 0x05220  per swing tip test: its full tip was shown (CTIP_ShowCaddieTip)
    u8   bCaddieTipsOff;        // 0x0522F  the full caddie tips are off (UI command 75,
                                //          GM_vDisableCaddieTips): CTIP_ShowCaddieTip only shows
                                //          short tips
    SavedRound aSavedRound[NUM_SAVED_ROUNDS];   // 0x05230
    GolferRecord createdGolfer; // 0x05380  the created golfer's record (FE_spGetGolfer: golfers
                                //          from FIRST_CREATED_GOLFER on are read here)
    u8   unk54C0[0x54C2 - 0x54C0];
    // The created golfer kept in this slot (golfer FIRST_CREATED_GOLFER + the slot), copied into
    // the session's PlayerProfile by Session_SetupProfiles (Code8002EE1C.c).
    s8   nGolferGlove;          // 0x054C2  -> PlayerProfile.nGlove (front-end messages 264, 265)
    u8   unk54C3[5];
    u64  aGolferNames[6];       // 0x054C8  -> PlayerProfile.aNames
    s8   nGolferOutfit;         // 0x054F8  -> PlayerProfile.nOutfit; FE_CrAP_TryBallSwappingAsset
                                //          stores a ball's index there
                                //       (DynObj_GetGolfBallLogoIndex, -1: none)
    u8   nGolferBallType;       // 0x054F9  -> PlayerProfile.nBallType
    u8   unk54FA[0x5500 - 0x54FA];
    // The created golfer's look: the body's parts and sets (FE_CrAP_SaveBodySkinChoices), its six
    // other skins' (FE_CrAP_SaveClubSkinChoices), its sliders and its logos. char.c hands it to the
    // character (Character_SetClubsAndClothes passes FE_GetCurrentProfile() + 0x5500 as a
    // SkinChoices*).
    SkinChoices choices;        // 0x05500
    s8   nDateMonth;            // 0x0AF7C  } a date, set and read by menu messages packed as
    s8   nDateDay;              // 0x0AF7D  } FE_DateToInt packs it (FE_CrAPMessages.c
    s16  nDateYear;             // 0x0AF7E  } GM_vSetCrAPGolferInfo, GM_vGetCrAPGolferInfo)
    s32  aAF80[53];             // 0x0AF80  per slot: a Create-A-Player asset (FE_CrAPDB.c
                                //          FE_CrAP_GetEquippedAsset), -1 for none; an asset's n2E is its slot
    // Four bit arrays with a bit per Create-A-Player asset (0x80057F18's loop over them all, which
    // also clears aAssetNew and aAssetMarkedNew; BitArray_TestBit tests a bit).
    u32  aAssetLocked[94];      // 0x0B054  the asset was locked (FE_CrAP_IsItemLocked) when last checked
    u32  aAssetOwned[94];       // 0x0B1CC  owned: set for the level-0 assets and when bought
                                //          (GM_vPurchaseCrAPItem), cleared when sold; an asset of
                                //          lock kind 0 stays locked until the bit its
                                //          FE_CrAP_GetPartGMLockValByAssetNum names is set
    u32  aAssetNew[94];         // 0x0B344  unlocked since it was last seen (GM_vCheckCrAPUnlocks)
    u32  aAssetMarkedNew[94];   // 0x0B4BC  a new one the menus marked (GM_vMarkNewCrAPItem);
                                //          GM_vClearMarkedNewCrAPItems clears both bits
    TourSeason tour;            // 0x0B634
    u8   a104D0[118];           // 0x104D0  per real-time event, by the ids of
                                //          GM_RealtimeMode_GetStartDate (0..117):
                                //          TrophyRoom_CountEventsInMonth counts the nonzero ones
                                //          in a month; no C code here writes it
    u8   unk10546[0x10548 - 0x10546];
    u32  aUserFlags[1];         // 0x10548  flag bits (UserInfo_SetUserFlag / UserInfo_GetUserFlag;
                                //          menu messages 503 / 504); bit 1: the Game Boy Advance
                                //          link's unlocks were given (GM_vGbaGrantUnlocks)
    SponsorSlot aSponsor[11];   // 0x1054C  the sponsorship slots; cleared by the profile setup;
                                //          FE_CrAP_IsItemLocked's lock kinds 10 (a sponsor signed) and 11
                                //          (so many slots signed) read them
    u8   a10578[4];             // 0x10578  par-5 holes 71..74, as a5004: eagled
                                //          (UserInfo_GetPar5EagleStat's kind 0)
    s32  a1057C[4];             // 0x1057C  and their eagle dates, as a504C (kind 1)
    u8   unk1058C[0x10600 - 0x1058C];
} SaveProfile;
LAYOUT_ASSERT(SaveProfile, 0x10600);

// The session's record tables as the save file keeps them: a copy of gSession from aCourseRecord
// to recC (0xF00..0x5B2C, the same layout). The replay recorder (Replay.c) keeps one too.
typedef struct SaveRecords {
    CourseRecord aCourseRecord[NUM_COURSE_RECORDS];    // 0x0000
    RecordEntry recA[8][5];     // 0x41A0
    RecordEntry recB[3][3][5];  // 0x44C0
    RecordEntry recC[5][2][5];  // 0x4844
} SaveRecords;
LAYOUT_ASSERT(SaveRecords, 0x4C2C);

extern SaveProfile* gpSaveData;
extern SaveProfile* lbl_80281DF4;       // unlocks that hold for every profile (the cheat codes set them)
extern SponsorSlot lbl_80281DF0;        // a new profile's first sponsor (see SponsorSlot)
extern u32 gPasswordEnteredBits[8];     // the cheat codes entered, one bit each (0..6;
                                        // PasswordManager_IsPasswordEntered);
                                        // FE_CrAP_IsItemLocked's lock kind 6 tests bits 1..5
extern u32 gSponsorPasswordBits[16];    // the sponsors' cheat codes entered, one bit each
                                        // (gSponsorPasswords;
                                        // PasswordManager_IsSponsorshipPasswordEntered)
extern s32 gStartLockedGolfers[14];     // the golfers GM_GetGameProgress counts as unlockable
extern s32 gStartLockedCourses[6];      // the courses GM_GetGameProgress counts as unlockable

// The password manager (0x80056480-0x80057F18; TW06's passwordmanager.cpp)
void PasswordManager_SetDefaults(void);
void SaveProfile_InitSlot(int nSlot);    // sets up save profile nSlot
void SaveProfile_SetName(SaveProfile* pProfile, const char* pName);     // PasswordManager.c: name it

// GameMode.c: the profile's completion score (GM_Earnings_PayRoundGoals raises the TOUR card level with it)
f32  GM_GetGameProgress(SaveProfile* pProfile);

// Earnings.c: the awards
// Mark an award won today; 1 if it was not won before.
u8   GM_Earnings_GiveAwardToUser(int nPlayer, Award* pAward);

// fe_craputils.c (TW06's FE_CrAP_ utilities)
extern char gszNoLogoName[];     // "NoLogoName": a user logo's name until one is given
void FE_CrAP_InitCrAPInfo(SaveProfile* pProfile);
void UserInfo_UnlockGolfer(int nProfile, int nGolfer);        // unlock a golfer for the profile
void UserInfo_SetUserFlag(SaveProfile* pProfile, int nBit, u8 bSet);    // set or clear bit nBit of aUserFlags
u8   UserInfo_GetUserFlag(SaveProfile* pProfile, int nBit);  // bit nBit of pProfile->aUserFlags
u8   UserInfo_IsGolferAvailable(int nProfile, int nGolfer);        // the golfer is unlocked for the profile
void UserInfo_UnlockCourse(int nProfile, int nCourse);        // unlock a course (aCourseUnlocked)
u8   UserInfo_IsCourseUnlocked(int nProfile, int nCourse);        // whether a course is unlocked
void UserInfo_UnlockReward(int nProfile, int nReward);        // unlock a reward
void UserInfo_UnlockCourseSlot21(int nProfile);     // set aCourseUnlocked[21] (no event)
u8   UserInfo_IsCourseSlot21Unlocked(int nProfile);
void UserInfo_UnlockCourseSlot22(int nProfile);     // and for aCourseUnlocked[22]
u8   UserInfo_IsCourseSlot22Unlocked(int nProfile);
int  UserInfo_GetNumLadderEventsWon(int nProfile);  // how many ladder events the profile has won
// Add pName to list nKind
void FE_CrAP_AddCustomAnimation(SaveProfile* pProfile, int nKind, char* pName);
// Take pName out of list nKind
void FE_CrAP_RemoveCustomAnimation(SaveProfile* pProfile, int nKind, char* pName);
// pName is in list nKind (0..2)
u8   FE_CrAP_IsCustomAnimationSelected(SaveProfile* pProfile, int nKind, char* pName);
void FE_SetStartingSponsor(s16 n);            // sign lbl_80281DF0 with sponsor n
int  FE_GetStartingSponsor(void);             // lbl_80281DF0's sponsor (callers take it without extsh)

// 0x800588F4: par-5 eagle record i: kind 0 whether that hole is eagled (a5004/a10578), kind 1 the
// eagle's date (a504C/a1057C); -1 for another kind.
int  UserInfo_GetPar5EagleStat(SaveProfile* pProfile, int nKind, int i);
void UserInfo_SetPar5EagleStat(SaveProfile* pProfile, int nKind, int i, int nValue);  // and set it

#endif
