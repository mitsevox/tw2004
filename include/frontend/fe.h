// fe.h (our name): the front end, the game's menus: the menu screens (FE_Manager.c), the
// Create-A-Player database (FE_CrAPDB.c), the golfers animated on menu screens (FEgolferanim.c),
// the PGA Tour messages (FE_PGATourMessages.c) and the logo editor (FE_LogoDesign.c).

#ifndef FRONTEND_FE_H
#define FRONTEND_FE_H

#include "game_types.h"
#include "platform.h"
#include "endian.h"
#include "game/save.h"

// ---- the menu screens (FE_Manager.c) ---------------------------------------------------------

// A movie waiting to be played (0x108 bytes).
typedef struct FEMovie {
    s32 nKind;                  // 0x000  FE_MOVIE_CREDITS or FE_MOVIE_BIO
    u8  unk4[0x104 - 0x4];
    s32 nBio;                   // 0x104  FE_MOVIE_BIO: which one, from 0 ("bio01"); nothing in
                                //        this build writes it (GM_vQueueMovie): always bio01
} FEMovie;
LAYOUT_ASSERT(FEMovie, 0x108);

#define FE_MOVIE_CREDITS 1      // "credits"
#define FE_MOVIE_BIO     3      // "bios/bio<nn>"
#define FE_NUM_MOVIES    6      // the length of the movie queue

// The front end's state (gFEState, 0x660 bytes). Only what the cleaned code reads.
typedef struct FEState {
    u8  aLoaded[5];             // 0x000  per player slot: a profile is loaded (its name is shown)
    u8  aCPU[5];                // 0x005  per player slot: a CPU player (FE_vExitUI gives it
                                //        CONTROLLER_CPU and no profile)
    s8  aBackup[5];             // 0x00A  per player slot: its row in p658 (-1: none)
    u8  bFirstTime;             // 0x00F  the menus' first start (TW07 firstTime): the intro movie
                                //        plays and the front end loads without the loading
                                //        screen; set by FE_vOpenONCE and front-end message 150
    u8  b10;                    // 0x010  a flag of the menu scripts (front-end messages 333 /
                                //        334 read and set it); set by FE_vOpenONCE
    u8  bDemoStarting;          // 0x011  the menus start the demo (GM_vStartDemo); cleared as a
                                //        game starts (FE_vExitUI)
    u8  unk12[2];
    s32 nMode;                  // 0x014  the game mode the menus start in (FE_vExitUI): the
                                //        session's, or 4, 23, 27 or 28
    u8  bTourCardWithheld;      // 0x018  while clear a profile stored without a TOUR card gets
                                //        level 1 (GM_vStoreProfileInSlot); set by FE_vOpenONCE,
                                //        cleared by the "THEKITCHENSINK" cheat code
    u8  unk19[3];
    s32 nMCRewardMoney;         // 0x01C  the memory card's reward money (GM_vSetMCRewardMoney); a
                                //        new profile gets it on top of its 25,000
    s32 nMovieNext;             // 0x020  } the movie queue: the next to play, and where the next
    s32 nMovieFree;             // 0x024  } one is added (equal when it is empty)
    FEMovie aMovies[FE_NUM_MOVIES];     // 0x028
    SaveProfile* p658;          // 0x658  backup copies of the profiles; moved to ARAM and freed
                                //        by FE_MoveBackupsToARAM
    u8  unk65C[0x660 - 0x65C];
} FEState;
LAYOUT_ASSERT(FEState, 0x660);

extern FEState gFEState;

// GM_vIsGolferUnlocked and GM_vIsGolferUnlockedByDefault set it to 0.2 for a locked golfer, else 0
// (also for one that is not available).
extern f32 gFELockedGolferShade;        // uiProcessPolygon.c's (= 0.25f); nothing in this build reads it

// The UI's controller input and fade state (gUIState, 0x4C bytes; our name). Only what the
// cleaned code reads.
typedef struct UIState {
    u8  bFadeToBlack;           // 0x00  the fade to black runs (set when the round or the menus
                                //       are left; uiProcessInterface.c draws it and clears it)
    u8  abPluggedIn[4];         // 0x01  per controller: plugged in (UI_ReadControllers); read by a
                                //       menu message (GM_vIsControllerPluggedIn: 1 for index 9)
    u8  unk5[3];
    u32 auLastButtons[4];       // 0x08  per controller: the buttons held last frame, cleared when
                                //       they have been held past 8 frames (so they count as
                                //       pressed again)
    u32 anHeldFrames[4];        // 0x18  per controller: frames the same buttons have been held,
                                //       restarted past 8; cleared by UI_vInitModule
    u8  abPluggedInLast[4];     // 0x28  per controller: abPluggedIn, copied at the end of each
                                //       UI_ReadControllers; nothing reads it
    u8  abAssigned[4];          // 0x2C  per controller: given to a player (GM_vSetPlayerController;
                                //       GM_vIsControllerAssigned reads it)
    u8  abInputEnabled[4];      // 0x30  per controller: its buttons reach the UI
                                //       (UI_SetControllerEnabled; UI_vInitModule enables all four)
    s32 nFramesNoPad;           // 0x34  frames with no controller plugged in (UI_ReadControllers)
    s32 nNumPluggedIn;          // 0x38  controllers plugged in, counted every frame
                                //       (UI_ReadControllers; GM_vGetNumControllersPluggedIn)
    s32 nPictureTable;          // 0x3C  the UI file table holding the picture entries (kind 2;
                                //       UI_ResolveFileEntries)
    u8  bButtonsBlocked;        // 0x40  no button reaches the UI: a two-player mode (7 or 26) in
                                //       the menus has too few controllers (UI_ReadControllers)
    u8  unk41[0x44 - 0x41];
    f32 fFade;                  // 0x44  the fade to black before a movie, 0 to 1
    u8  b48;                    // 0x48  cleared by UI_vInitModule; nothing else uses it
    u8  bHideMenuGolfer;        // 0x49  set with bButtonsBlocked: the menu golfer is not drawn
                                //       (FEgolferanim.c FE_IsGolferRenderAllowed)
    u8  unk4A[0x4C - 0x4A];
} UIState;
LAYOUT_ASSERT(UIState, 0x4C);

extern UIState gUIState;

// The loading screen and its bar of eight tiles (gUILoadingScreen, 0x38 bytes; our name):
// uiProcessPolygon.c UI_InitLoadingBar sets it up and UI_DrawLoadingScreenAndProgressBar steps it;
// FE_Manager.c clears bRunning and pPicture. Only what the cleaned code reads.
typedef struct UILoadingScreen {
    s32 n0;                     // 0x00  set to 0 by UI_InitLoadingBar; nothing reads it
    f32 fNextTile;              // 0x04  when fElapsed passes it the next tile is shown and it moves
                                //       on by fTileSecs (it starts at fTileSecs)
    f32 fNextRedraw;            // 0x08  until a tile is due the screen is redrawn only when
                                //       fElapsed passes it, which moves it on 2 seconds
    f32 fTileSecs;              // 0x0C  seconds per tile: (4.83 * nNumPlayers + 3.1) / 8
    f32 fElapsed;               // 0x10  seconds since it was set up
    s32 nNumPlayers;            // 0x14  the number of players in game type 4, else 0
    u8  bRunning;               // 0x18  set up and running (UI_InitLoadingBar does nothing while set)
    u8  unk19[0x1C - 0x19];
    s32 nLastTile;              // 0x1C  the last tile shown (-1: none yet; up to 8)
    u64 uLastTime;              // 0x20  TI_sRead's clock when it was set up, then at the last step
    u64 uTime;                  // 0x28  TI_sRead's clock at this step
    struct LLPict* pPicture;    // 0x30  the loading picture, decoded from the 'load' object
                                //       (UI_DecodeLoadingPicture)
    u8  unk34[0x38 - 0x34];
} UILoadingScreen;
LAYOUT_ASSERT(UILoadingScreen, 0x38);

extern UILoadingScreen gUILoadingScreen;

// A corner of a quad uiProcessPolygon.c UIPoly_UnpackVertex turns into draw arrays (uiArc.c builds
// them too). Our name.
typedef struct FEVertex {
    f32 fU;                     // 0x00  texture coordinates
    f32 fV;                     // 0x04
    f32 fX;                     // 0x08  position
    f32 fY;                     // 0x0C
    f32 fZ;                     // 0x10
    u8  auColour[4];            // 0x14  red, green, blue, alpha
} FEVertex;
LAYOUT_ASSERT(FEVertex, 0x18);

// A textured quad of the front end that uiProcessPolygon.c UIPoly_ProcessMessage takes messages
// for. Our name; only what the cleaned code reads (its size is not known).
typedef struct FEQuad {
    s16 nEntry;                 // 0x00  } its UI file entry: entry nEntry of table nTable (-1:
    s16 nTable;                 // 0x02  } none, untextured); message 5 sets them
    s16 nColour;                // 0x04  its colour in the front end's colour table (-1: none)
    u8  unk6[0x8 - 0x6];
    s16 nFlags;                 // 0x08  bit 0: a texture keeps the corners' colours (UIPoly_Draw)
    s16 nA;                     // 0x0A  passed to UI_LoadEntryPicture / UI_ReleaseEntryPicture,
                                //       which ignore it
    FEVertex aVtx[4];           // 0x0C  the corners (UIPoly_Draw draws them)
} FEQuad;

// A message argument of UIPoly_ProcessMessage: a number or a float, by message. Our name.
typedef union FEMsgArg {
    s32 n;
    f32 f;
} FEMsgArg;

void FE_InitGolferTextures(void);  // FEgolferanim.c (FE_Manager.c, uiProcessInterface.c call it)

// uiProcessPolygon.c: the quads' message handler (the studio's handler 0, uiProcessInterface.c
// UI_OpenInterface).
void UIPoly_ProcessMessage(FEQuad* pQuad, int nMsg, u32 bSplit, FEMsgArg* pArgs);

// Four floats each, set by uiProcessPolygon.c UIPoly_Draw: UIPoly_TintVertex tints a vertex colour to
// gpUIPolyColourMul * (colour + gpUIPolyColourAdd).
extern f32* gpUIPolyColourMul;
extern f32* gpUIPolyColourAdd;

// A value handed back to the UI as a hint a few frames later (gUIDelayedHint, 0xC bytes; our
// name): front-end message 53 (GM_vMCfunction) and round command 84 (GM_vIG_MCfunction) set it,
// uiProcessInterface.c UI_DrawInterface counts the frames and sends it.
typedef struct UIDelayedHint {
    s32 nFrames;                // 0x0  -1: nothing waiting; the command sets 0, UI_DrawInterface
                                //      counts it up each frame; past 2 it sends nValue, back to -1
    s32 nValue;                 // 0x4  sent with hint 0x23 in the menus, 0x24 in a round
    u8  unk8[4];
} UIDelayedHint;
LAYOUT_ASSERT(UIDelayedHint, 0xC);

extern UIDelayedHint gUIDelayedHint;

// The state of one 'txf2' texture bank (gUITxf2BankState, one per bank of lbl_801A26DC; our name).
typedef struct UITxf2BankState {
    u8  b0;                     // 0x0  set (and b1 cleared) at the menus' shutdown for the banks
                                //      gUITxf2BankMarkOnExit marks; cleared by FE_InitManager;
                                //      nothing reads it
    u8  b1;                     // 0x1  only cleared (with b0)
    u8  unk2[2];
    s32 nFreeBeforeMovie;       // 0x4  > 0: the bank's pixels are freed before a movie
                                //      (UI_FreeTxf2BankPixels); only ever cleared in this build
} UITxf2BankState;
LAYOUT_ASSERT(UITxf2BankState, 0x8);

#define UI_NUM_TXF2_BANKS 200
extern UITxf2BankState gUITxf2BankState[UI_NUM_TXF2_BANKS];
// One word per gUITxf2BankState entry: nonzero sets that entry's b0 (and clears its b1) when the front
// end is shut down in game type 3 (uiProcessInterface.c UI_CloseInterface).
extern u32 gUITxf2BankMarkOnExit[UI_NUM_TXF2_BANKS];

// The profile being worked on in the menus (gpFEProfile points to it; 0x11708 bytes, allocated
// and cleared by FE_InitManager).
typedef struct FEProfile {
    u8  bAwardReplay;           // 0x00000  the replay played is a trophy ball's
                                //          (GM_vShowAwardReplay); with game mode 10 the menus then
                                //          start in mode 27
    s8  n1;                     // 0x00001  -1 when it is set up
    s8  nSlot;                  // 0x00002  the player slot whose profile it is
    s8  nCustomRoundSlot;       // 0x00003  the save slot whose custom round the menus edit
    s8  nCustomRound;           // 0x00004  that custom round (0..2; GM_vGetHolePar reads its holes)
    s8  n5;                     // 0x00005  only set and read by menu messages (GM_vSetFEProfileN5)
    u8  unk6[0x10 - 0x6];
    SaveProfile profile;        // 0x00010  a working copy
    u8  unk10610[0x1061C - 0x10610];
    s32 n1061C;                 // 0x1061C  the award whose replay is shown (GM_vShowAwardReplay)
    s8  n10620;                 // 0x10620  read and cleared by menu messages
    s8  a10621[15][2];          // 0x10621  pairs a menu message reads (GM_vGetAttributeLevelUp)
    u8  bCopy;                  // 0x1063F  the working copy is the profile, not the slot's own
    u8  bEditingCopy;           // 0x10640  the logo editor works on logoCopy (GM_vCRAPCreatingLogo)
    u8  unk10641[3];
    // The day's sale items (FE_CrAP_UpdateSaleInfo, seeded from today's date), per gender and sale category
    // (-1 clothes, -2 accessories, -3 clubs and balls): a part picked at random, then up to five of
    // its assets as list entry and choice (-1: none; GM_vGetCrAPSaleItems, GM_vIsCrAPItemOnSale).
    s32 nDateSeed;              // 0x10644  the seed (today's date, packed)
    s16 aSalePart[2][3];        // 0x10648  the part on sale
    s16 aSaleEntry[2][3][5];    // 0x10654  its items' list entries (EA's subcategories)
    s16 aSaleChoice[2][3][5];   // 0x10690  and choices
    u8  unk106CC[0x106D0 - 0x106CC];
    u64 uSquareHash;            // 0x106D0  the hash of "__LogoSquare" (the square logo's texture)
    u64 uRectHash;              // 0x106D8  the hash of "__LogoRect"
    LogoRecord logoCopy;        // 0x106E0  the logo being made, copied into the profile's logo
                                //          FE_LogoDesign_GetCurrentLogoNumber when kept (GM_vSaveLogo)
    u8  bAllGolfersPickable;    // 0x11702  every golfer counts as unlocked (GM_vSetAllGolfersPickable)
    u8  bGolferPicked;          // 0x11703  Play Now: player 0 keeps the golfer picked
                                //          (GM_vSetPlayerGolfer), not the created one
    s32 nEASBioError;           // 0x11704  the last EA Sports Bio error (GM_vEASBioGetLastError)
} FEProfile;
LAYOUT_ASSERT(FEProfile, 0x11708);

extern FEProfile* gpFEProfile;

// One golfer's bio in the 'BIO ' stream object, as the menus show it (FE_MessageTable.c
// GM_vFindGolferBio, GM_vGetBioTexts, GM_vGetBioLines).
typedef struct FEBio {
    s32  nId;                   // 0x000  the golfer it describes (GM_vFindGolferBio searches on it)
    char sz4[0x20];             // 0x004  } texts the menus show
    char sz24[0x10];            // 0x024  }
    s32  a34[6];                // 0x034  numbers the menus show
    char sz4C[0x1C];            // 0x04C
    s32  n68;                   // 0x068
    s32  nCourse;               // 0x06C  shown as the course's name (lbl_80191990); -1: "N/A"
    char sz70[0x28];            // 0x070
    char sz98[0x160];           // 0x098  up to five lines, split at '\n'
} FEBio;
LAYOUT_ASSERT(FEBio, 0x1F8);

extern s32 gStartUnlockedGolfers[16];  // the golfers unlocked from the start (GM_vIsGolferUnlockedByDefault)

#define FE_NUM_BIOS 29
extern FEBio* gpFEBios;           // a copy of the 'BIO ' stream object's data (FE_CharBios_LoadBIOfromStream)

// The profile backups (FEState.p658) can be moved out to ARAM (FE_MoveBackupsToARAM) and back
// (FE_RestoreBackupsFromARAM).
#define FE_BACKUP_SIZE 0x41820  // the four slots' backups (4 x 0x10600) and 0x20 more
extern u32 gFEBackupSize;       // their size
extern u32 gFEBackupAramAddr;   // their ARAM address while they are there (0: not there)

// ---- the golfers animated on menu screens (FEgolferanim.c) -----------------------------------

// A state of the golfer loader (gFEStreamStates): run by FE_StreamUpdateState.
typedef struct FEGolferState {
    void (*pfnEnter)(void);     // 0x00
    void (*pfnUpdate)(void);    // 0x04  every frame
    void (*pfnExit)(void);      // 0x08
    void (*pfnAbort)(void);     // 0x0C  FE_StreamInterruptState
    s32  nNext;                 // 0x10  the state that follows it
} FEGolferState;
LAYOUT_ASSERT(FEGolferState, 0x14);

#define FE_NUM_GOLFER_STATES 5  // state 0 is empty

// The golfer loader's state machine (gFEStreamStateMgr).
typedef struct FEGolferMachine {
    s32 nNext;                  // 0x0  the state after this one
    s32 nState;                 // 0x4  the running state (0: stopped)
    u8  bDone;                  // 0x8  the state is finished: go to nNext
    u8  bEnter;                 // 0x9  the state's pfnEnter is still to run
    u8  bAbort;                 // 0xA  set by FE_StreamInterruptState
    u8  bPaused;                // 0xB
} FEGolferMachine;
LAYOUT_ASSERT(FEGolferMachine, 0xC);

// ---- the Create-A-Player database (FE_CrAPDB.c) ----------------------------------------------

// A created golfer is put together from CRAP_NUM_PARTS parts (headwear, shirts, shoes...), and
// each part offers a list of assets to choose from.
#define CRAP_NUM_PARTS 24

// A Create-A-Player asset (0x118 bytes): a hat, a shirt, a colour... The 'CR_A' stream object is
// the array of them all (FE_CrAP_LoadAssetsFromStream). Only what the cleaned code reads.
typedef struct CrAPAsset {
    s32  n0;                    // 0x000  part 18's assets pass it to FE_SetLastCrAPAsset (sApplySlider)
    char szName[0x28 - 0x4];    // 0x004  "White", "Bright Red", "... backwards" ...
    s16  nPart;                 // 0x028  the part it is a choice for
    s16  nCategory;             // 0x02A  its category: where the category's name ("Hats",
                                //        "Visors") starts in the 'CR_S' strings (FE_CrAP_GetStringFromTable)
    s16  n2C;                   // 0x02C  its sponsor: an index into the brand names lbl_801935C8
                                //        (FE_CrAP_GetPartSponsor; TW06 sponsorID)
    s16  n2E;                   // 0x02E  its slot: the SaveProfile.aAF80 entry that holds it when worn
                                //        (0..52; -1: none; FE_CrAP_EquipAsset; TW06 itemSlot)
    s32  n30;                   // 0x030  its price (FE_CrAP_GetPartRetailPrice)
    s32  n34;                   // 0x034  its sale price (FE_CrAP_GetPartSalePrice)
    s32  nLevel;                // 0x038  its level (FE_CrAP_GetPartLevel): level 0 assets are owned from
                                //        the start
    s8   nAttrA;                // 0x03C  } the two attributes it raises (-1: none) and the tier
    s8   nTierA;                // 0x03D  } it raises each to
    s8   nAttrB;                // 0x03E  }
    s8   nTierB;                // 0x03F  }
    s8   nGender;               // 0x040  the gender it is for (2: either); offered when it is the
                                //        database's nGender or 2 (FE_CrAP_GetAssetGender,
                                //        FE_IsValidCurrentGender)
    s8   nLockKind;             // 0x041  } how it is unlocked and the number that goes with it
    s16  nLock;                 // 0x042  } (FE_CrAP_IsItemLocked)
    s16  n44;                   // 0x044  } its three colour ids (FE_CrAP_GetPartColor1..3;
    s16  n46;                   // 0x046  } TW06 color1..color3)
    s16  n48;                   // 0x048  }
    s8   a4A[6];                // 0x04A  indexed by FE_CrAP_GetPartValue's last argument
    s8   aColorKind[6];         // 0x050  per colour: 0..2 take the skin option's colour of that
                                //        kind (FE_CrAP_GetPartColorRGBA); -1 and others use aColor
    u8   unk56[2];
    u8   aColor[6][4];          // 0x058  its colours, RGBA; assets of a category whose first
                                //        colour differs are different choices
                                //        (FE_CrAP_GetNumberUniqueGeometries)
    u64  aPart[4];              // 0x070  } the ids of four skin parts it sets (SkinPart_FindPart finds
    u64  aVariant[4];           // 0x090  } them) and the id of each one's variant (FE_CrAP_ApplyAssetParts)
    u64  aSet[4];               // 0x0B0  the ids of four skin sets; taking the asset off puts
                                //        them back to "Defaults" (FE_CrAP_RemoveAssetSets)
    u64  aSetVariant[4];        // 0x0D0  } putting it on gives each set the variant and option
    u64  aSetOption[4];         // 0x0F0  } with these ids (FE_CrAP_ApplyAssetSets)
    s16  nUnlockText;           // 0x110  the offset in 'CR_S' of its unlock text (-1: none;
                                //        FE_CrAP_GetUnlockMessageFrom)
    s16  n112;                  // 0x112  } the offsets in 'CR_S' of the animation the menu golfer plays to
    s16  n114;                  // 0x114  } show it off and of its camera shot (FE_CrAP_TurnOnAsset,
                                //        } sApplySlider)
    s16  n116;                  // 0x116  (swapped by CrAPAssetsByteSwap)
} CrAPAsset;
LAYOUT_ASSERT(CrAPAsset, 0x118);

// The Create-A-Player database (0x18 bytes, allocated by FE_CrAP_InitModule).
typedef struct CrAPDB {
    s32  nAssets;               // 0x00  how many assets pAssets holds
    s8   nGender;               // 0x04  the gender of the golfer being created, which picks the assets
                                //       offered (FE_IsValidCurrentGender); FE_CrAP_SetCurrentGender sets it
    u8   unk5[3];
    CrAPAsset* pAssets;         // 0x08  the 'CR_A' object's data: every asset
    char* pStrings;             // 0x0C  } the 'CR_S' object's data (the names the assets use)
    u32  uStringsSize;          // 0x10  } and its size
    u8   b14;                   // 0x14  1 when allocated; FE_CrAP_SetTriggerAnims sets it
    u8   unk15[3];
} CrAPDB;
LAYOUT_ASSERT(CrAPDB, 0x18);

// A 0x2C-byte record of the Create-A-Player database's table lbl_80282470 (64 of them,
// FE_CrAP_InitModule); FE_CrAP_GetSponsorshipItemInfo copies one out.
typedef struct CrAPRecord {
    s16  nSponsor;              // 0x00  the sponsor (an index into lbl_801935C8)
    u8   unk2[2];
    s32  nBonusCash;            // 0x04  the sponsorship's cash bonus
                                //       (GameModeDriverPGATour_GetSponsorshipBonusCash)
    char sz8[0x2C - 0x8];       // 0x08  the worn asset's name (FE_CrAP_CollectSponsorshipItems)
} CrAPRecord;
LAYOUT_ASSERT(CrAPRecord, 0x2C);

extern CrAPDB* lbl_80282460;
extern UStreamObject* lbl_80282464;  // the 'CR_A' object (the assets), kept until freed
extern UStreamObject* lbl_80282468;  // the 'CR_S' object (their names)
extern s32 lbl_8028246C;             // how many records lbl_80282470 holds (FE_CrAP_CollectSponsorshipItems)
// 64 records (FE_CrAP_GetSponsorshipItemInfo); freed by FE_CrAP_CloseModule
extern CrAPRecord* lbl_80282470;
extern s32* lbl_80282474;            // per part: the index of its first asset
// Per part, 24 entries: the categories FE_CrAP_GetSubCategoryIDForCategoryAndSubcategoryIndex found
extern s32* lbl_80282478;
// 0x600 entries, rows 24 apart (FE_CrAP_ResetLastCategoryTables clears 64 from each row's start);
// FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex stores a part's choice count
extern s32* lbl_8028247C;
extern s32* lbl_80282480;            // 24 entries (FE_CrAP_ResetLastCategoryTables sets them to -1)
extern SwapField lbl_80193228[20];   // an asset's byte-swap layout (CrAPAssetsByteSwap)
extern char lbl_801935C8[16][32];    // the 16 sponsors' brand names ("adidas", "Callaway Golf"...;
                                     // FE_CrAP_GetSponsorName)
extern char lbl_801937C8[11][32];    // the skin sets a logo can go on ("ushirtlogof",
                                     // "uhatlogof", "uarmtattool"...; sTurnOnLogo)
extern s32 lbl_802816E8;             // } an asset to put on and one to take off when
extern s32 lbl_802816EC;             // } FE_CrAP_RestoreLastRemovedAsset runs (-1: none)
// Per part: the name of its "All ..." entry that lists every category ("All Headwear"), or ""
extern char lbl_801932C8[CRAP_NUM_PARTS][32];

// The asset nAsset takes its attributes from (itself, or for lock kind 28 the asset its nLock
// names)
int  sGetLinkedAssetID(int nAsset);
CrAPAsset* sGetLinkedAsset(CrAPAsset* pAsset);                 // the same, by asset
u8   FE_CrAP_GetTriggerAnims(void);                            // the database's b14
// Set the database's nGender (which assets are offered)
void FE_CrAP_SetCurrentGender(s8 n);
s8   FE_CrAP_GetAssetGender(int nAsset);                       // an asset's nGender
s8   FE_CrAP_GetCurrentGender(void);                           // the database's nGender
// The profile's aAF80[nSlot], an asset (-1 past slot 52)
int  FE_CrAP_GetEquippedAsset(s16 nSlot);
int  FE_CrAP_GetNumberOfSubcategoryIndicesForCategory(s16 nPart);
void FE_CrAP_RestoreLastRemovedAsset(void);
// Copy the name of a part's entry n (for 0 its "All ..." entry when it has one); 0 if there is none
u8   FE_CrAP_GetSubCategoryNameForCategoryAndSubcategoryIndex(s16 nPart, int n, char* pDst);
// How many different choices fit a part's entry b
int  FE_CrAP_GetNumberUniqueGeometries(s16 nPart, int b);
// Where an asset is listed: its part, the entry of its category, its place there.
void FE_CrAP_GetCategorySubcategoryAndEntryNumFromAssetID(int nAsset, s16* pnPart, s32* pnEntry,
                                                          s32* pnPlace);
u8   FE_IsValidCurrentGender(s8 n);                            // an asset of this gender is offered
// The asset in the first slot of aAF80 whose asset is of the part (-1: none)
int  FE_CrAP_GetFirstEquippedIndexForCategory(s16 nPart);
u8   FE_CrAP_GetColorNameFromID(int nOffset, char* pDst);      // copy a 'CR_S' name ("" for "NONE")
// Count a part's assets offered for the current gender: locked, owned, new and all
void FE_CrAP_GetCategoryInfo(s16 nPart, s32* pLocked, s32* pOwned, s32* pNew, s32* pAll);
u8   FE_CrAP_IsAssetRemovable(int nAsset);
s16  FE_CrAP_GetCategoryFromAssetID(int nAsset);               // the part an asset is a choice for
int  FE_CrAP_GetLevelFromAssetID(int nAsset);                  // an asset's nLevel
// The asset may be picked: not locked when last checked, and its aAssetOwned bit is set
u8   FE_CrAP_IsAssetAvailableForUser(int nAsset);
// How many choices a part's entry b has
int  FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(s16 nPart, int b);
// A part's choice i: the asset's index
int  FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(s16 nPart, int b, int i);
// A part's choice i: the asset
CrAPAsset* FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(s16 nPart, int b, int i);
CrAPAsset* FE_CrAP_GetAssetFromAssetIndex(int nAsset);         // an asset by index
int  FE_CrAP_GetPartAttributeUpgrade1ByAssetID(int nAsset);    // } the two attributes it raises
int  FE_CrAP_GetPartAttributeUpgrade2ByAssetID(int nAsset);    // } (-1: none)
int  FE_CrAP_GetPartAttributeModifier1ByAssetID(int nAsset);   // } and the tier it raises each to
int  FE_CrAP_GetPartAttributeModifier2ByAssetID(int nAsset);   // }
s8   FE_CrAP_GetPartGMLockIDByAssetNum(int nAsset);            // } an asset's lock kind and number
s16  FE_CrAP_GetPartGMLockValByAssetNum(int nAsset);           // } (-1: no such asset)
s32  FE_CrAP_GetNumEntriesInCrAPDB(void);                      // how many assets there are
s32  FE_CrAP_GetPartLevelFromAssetIndex(int nAsset);           // an asset's nLevel (-1: no such asset)
u8   FE_CrAP_IsCrAPDBLoaded(void);                             // the Create-A-Player database is allocated
char* FE_CrAP_GetStringFromTable(int nCategory);               // a category's name
// The category of a part's entry n (-1: its "All ..." entry; 0x40: none)
int  FE_CrAP_GetSubCategoryIDForCategoryAndSubcategoryIndex(s16 nPart, int n);
u8   FE_CrAP_IsAssetEquipped(CrAPAsset* pAsset);
void FE_CrAP_TurnOffPart(s16 nPart, int b, int i);
// A part's choice i, by the part, its entry b and i.
char* FE_CrAP_GetPartName(s16 nPart, int b, int i);
s16  FE_CrAP_GetPartColor1(s16 nPart, int b, int i);
s16  FE_CrAP_GetPartColor2(s16 nPart, int b, int i);
s16  FE_CrAP_GetPartColor3(s16 nPart, int b, int i);
s16  FE_CrAP_GetPartSponsor(s16 nPart, int b, int i);
s32  FE_CrAP_GetPartRetailPrice(s16 nPart, int b, int i);
s32  FE_CrAP_GetPartSalePrice(s16 nPart, int b, int i);
s32  FE_CrAP_GetPartLevel(s16 nPart, int b, int i);
int  FE_CrAP_GetPartAttributeUpgrade1(s16 nPart, int b, int i);
int  FE_CrAP_GetPartAttributeModifier1(s16 nPart, int b, int i);
int  FE_CrAP_GetPartAttributeUpgrade2(s16 nPart, int b, int i);
int  FE_CrAP_GetPartAttributeModifier2(s16 nPart, int b, int i);
s8   FE_CrAP_GetPartGMLockID(s16 nPart, int b, int i);
s16  FE_CrAP_GetPartGMLockVal(s16 nPart, int b, int i);
int  FE_CrAP_GetPartValue(s16 nPart, int b, int i, int n);
// pColor: 4 bytes
void FE_CrAP_GetPartColorRGBA(s16 nPart, int b, int i, int n, u8* pColor);
void FE_CrAP_GetPartVariantName(s16 nPart, int b, int i, char* pName);
u8   FE_CrAP_IsItemEquipped(s16 nPart, int b, int i);
u8   FE_CrAP_GetUnlockMessageFrom(s16 nPart, int b, int i, char* pDst);
// Copy the name of an asset's category
void FE_CrAP_GetSubcategoryNameFromAssetID(int nAsset, char* pDst);
void FE_CrAP_GetAssetNameFromAssetID(int nAsset, char* pDst);  // copy an asset's name
// The asset's place in the list of the part's offered assets that fit its entry n
void FE_CrAP_GetEntryNumFromAssetIDCategorySubcategory(int nAsset, s16 nPart, int n, s32* pnPlace);
// The asset in the first slot of aAF80 of the part that fits its entry n (-1: none)
int  FE_CrAP_GetFirstEquippedIndexForCategoryAndSubcategory(s16 nPart, int n);

void FE_MakeMoviePath(char* pName, char* pPath);        // "data/movies/<name>.NGC"
void FE_MakeCameoMoviePath(char* pName, char* pPath);   // "data/movies/cameos/<name>.NGC"
// A movie's skip test for LLVideo.c's LLVideo_PlayFile (whose arguments it ignores): any button.
u8   FE_IsMovieSkipPressed(struct Video* pVideo, int nArg);

// uiProcessPolygon.c: the texture bank loaded from LoadData.c's 'txf2' copy
// (UI_LoadLoadingBarTexture), its slot and its first texture.
struct TexEntry* UI_GetTexBankFirstTexture(struct TexBank* pBank);   // a bank's first texture
extern int gUILoadingBarBankSlot;                // the bank's slot
extern struct TexBank*  gpUILoadingBarBank;
extern struct TexEntry* gpUILoadingBarTexture;
extern f32 gUILoadingBarTilePos[8][2];          // eight x, y points UI_InitLoadingBarTilePos sets,
                                                // UI_DrawLoadingBarTile reads
void FE_CrAP_TurnOnPart(s16 nPart, int b, int i);    // FE_CrAPDB.c
int  FE_CrAP_GetNumEquippedItemsWithSponsor(s16 n);  // FE_CrAPDB.c: the profile's assets whose n2C is n
s32  FE_CrAP_CollectSponsorshipItems(void);          // FE_CrAPDB.c: fill lbl_80282470; how many records
// FE_CrAPDB.c: copy record n out
void FE_CrAP_GetSponsorshipItemInfo(int n, s16* pN0, s32* pN4, char* pDst);
void FE_CrAP_GetSponsorName(s16 n, char* pDst);      // FE_CrAPDB.c: name n of lbl_801935C8
// FE_CrAPDB.c: send message nMsg with its values to the front end (the EA Sports Bio screens).
void FE_SendHintInt(int nMsg, s32 nA);
void FE_SendHintIntString(int nMsg, s32 nA, char* szB);
void FE_SendHintIntStringInt(int nMsg, s32 nA, char* szB, s32 nC);
int  FE_SendHintString(char* sz, int nMsg);
void FE_SendHint7Args(int nMsg, s32 nA, s32 nB, s32 nC, s32 nD, s32 nE, s32 nF, f32 fG);
void FE_SendHint10Args(int nMsg, s32 nA, s32 nB, s32 nC, s32 nD, s32 nE, s32 nF, s32 nG, s32 nH, s32 nI,
                 s32 nJ);
SaveProfile* FE_GetCurrentProfile(void);         // the profile being worked on
u8   FE_CrAP_IsItemLocked(s32 nAsset, SaveProfile* pProfile);  // the asset is locked (FE_Manager.c)
int  FE_DateToInt(int nMonth, int nDay, int nYear);  // nDay * 1000000 + nMonth * 10000 + nYear
int  FE_GetCurrUserID(void);                 // its player slot
u8   FE_movieIsQueueEmpty(void);
int  FE_GetSaleIDFromSaleCategory(int n);                // -1, -2, -3 to 0, 1, 2; anything else to 0
void FE_CrAP_SetupLockedAssets(SaveProfile* pProfile);    // note which assets are locked (aAssetLocked)
void FE_CrAP_UpdateUserAttributeMods(SaveProfile* pProfile);
void FE_CrAP_RandomizeCategoryWithUndesirableTest(s16 nPart, int nChance);
void FE_CrAP_RandomizeFace(SaveProfile* pProfile);
void FE_CrAP_RandomizeAll(SaveProfile* pProfile);
// A random b and choice of part nPart; returns the choice
// (FE_CrAP_RandomizeCrAPCategoryAndSubcategoryItem)
int  FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(SaveProfile* pProfile, s16 nPart, int nChance);
int  FE_CrAP_RandomizeCrAPCategoryAndSubcategoryItem(SaveProfile* pProfile, s16 nPart, int b, int nChance);
u8   FE_bIsLicensedGolfer(int n);                // 16 of golfers 0..28: ball type 0 (Session_SetupProfiles)
void FE_SetupSaleInfo(void);                 // pick the day's random assets (FE_CrAP_UpdateSaleInfo)
FEMovie* FE_movieGetFreeEntry(void);             // the next free place in the movie queue
void FE_BackupAllProfiles(void);
void FE_BackupProfileClaimRow(int nSlot);
void FE_BackupProfile(int nSlot);
void FE_SwapBackupRows(int a, int b);         // swap backup rows a and b (p658)
GolferRecord* FE_spGetGolfer(int nGolfer); // a golfer's record (created golfers: the profile's)
void FE_IntToDate(int nDate, int* pMonth, int* pDay, int* pYear);  // unpack FE_DateToInt's number
void FE_vExitUI(void);
void Gaud_StopMusic(void);                 // (0x800A75B4) FE_Manager.c calls it after queueing a movie

// ---- the menus' message table (FE_MessageTable.c) --------------------------------------------

void FE_InitGameMessages(void);                 // fill the table
void MC_SetCurrentFileType(int n);      // sets lbl_80281FFC
extern char* lbl_80191990[30];          // per course: a string the menus show (a replay's course
                                        // picks it)
extern s32 lbl_80281FFC;                // set by MC_SetCurrentFileType: the lbl_8018C7D8 set (memcard.h) the
                                        // menus' memory-card messages use

// ---- the golfers animated on menu screens (FEgolferanim.c) ------------------------------------

void FE_setupStreaming(int nGolfer, int nOtherA, int nOtherB);   // show golfer nGolfer
void FE_StreamStopAllStreaming(void);
u8   FE_PauseFECharStreaming(u8 bPaused);   // pause the golfer loader (or not); the old setting
int  FE_StreamGetCurrentState(void);
void FE_vClearGolferCache(void);
void FE_ResetCrAPZoom(void);
void FE_SetCrapRotation(u8 bTarget, f32 fAngle);
void FE_SetCrAPCameraIdleState(int nState);
int  FE_HasGolferCharacter(void);
int  FE_IsGolferReady(void);
// 1: the animation was started
u8   FE_vTriggerCrAPAnimAndCamera(char* szAnim, char* szShot, u8 bBlend);
char* FE_GetCurrentAnimName(void);
void FE_SetCrapClub(int nClub);
void FE_SetTempCrapClub(int nClub);
void FE_QueueCrAPAnim(char* szAnim, char* szShot, s8 bFade, u8 bWaitForEnd);
void FE_SetAnimRepeatCount(int nCount);
void FE_RestartCrAPAnim(void);
void FE_SetCrapRenderState(int nState);
void FE_SetTempCrapRenderState(int nState);
u8   FE_SetDelayTextureSwap(u8 bDelay, f32 fTime);
void FE_QueueBallChange(char* szTex);
int  FE_GetCrapRenderState(void);
void FE_RestartClubIdleAnim(void);
void FE_SetNewTexturesFlag(u8 bNew);
u8   FE_GetClubStatesAllowed(void);
u8   FE_IsTextureSwapDue(void);
void FE_SetClubStatesAllowed(u8 bAllowed);
void FE_SetLastCrAPAsset(int nAsset);
int  FE_GetLastCrAPAsset(void);
void FE_SetLastCrAPCategory(int nPart);
int  FE_GetLastCrAPCategory(void);
void FE_ResetCrAPGolferFromPreview(void);
// Code800B9944.c: the ball the Create-A-Player menu golfer holds.
void FE_CrAPBall_RegisterStreamClients(void);     // its 'TEO ' models and 'BALF' logo bank
void FE_CrAPBall_UnRegisterStreamClients(void);
void FE_CrAPBall_Init(void);
void FE_CrAPBall_Free(void);
void FE_CrAPBall_MakeObjects(void);
void FE_CrAPBall_SetLogo(char* szBall);           // the logo on it (NULL: none)
void FE_CrAPBall_Render(u8 bTarget);              // the ball in the menu golfer's hand

// ---- the logo editor (FE_LogoDesign.c) -------------------------------------------------------

// A logo (LogoRecord, game/save.h) is 8-bit colour indexes into a 256-colour palette.

// The logo being edited (12 bytes, allocated by FE_LogoDesign_InitModule).
typedef struct LogoEdit {
    s32 nLogo;                  // 0x0  the user logo edited (0..4): SkinChoices.aLogo[nLogo]
                                //      (FE_LogoDesign_GetCurrentLogo)
    s32 nShape;                 // 0x4  LOGO_SQUARE or LOGO_RECT
    u8  bDirty;                 // 0x8  changed since it was last copied into its texture
} LogoEdit;
LAYOUT_ASSERT(LogoEdit, 0xC);

extern LogoEdit* gpLogoEdit;
extern s16* gpLogoClut;               // the palette: 256 colours, 1-bit alpha (the sign bit)
                                        // and 5-5-5 RGB; read signed (lha)
extern u8 gbLogoClutLoaded;                 // the palette has been copied from "__LogoSquare"

void FE_LogoDesign_SetCurrentLogoNumber(s32 n);           // pick the logo to edit (LogoEdit.nLogo)
s32  FE_LogoDesign_GetCurrentLogoNumber(void);            // which logo is edited
void FE_LogoDesign_SetCurrentLogoMode(s32 nShape);        // set its shape
// A palette colour, 0-255 each
void FE_LogoDesign_GetClutEntry(int nColor, u32* pR, u32* pG, u32* pB, u32* pA);
void FE_LogoDesign_RefreshLogo(void);                     // mark the logo changed
void FE_LogoDesign_SetLogoToPremadeTexture(char* pName);  // load the logo from a texture
void FE_LogoDesign_SetPixel(int nX, int nY, int nColor);  // set a pixel
LogoRecord* FE_LogoDesign_GetCurrentLogo(void);           // the logo being edited
s16* FE_LogoDesign_GetClut(void);                         // the palette
// A pixel's colour index, and its colour as FE_LogoDesign_GetClutEntry gives it
int  FE_LogoDesign_GetPixelColor(int nX, int nY, u32* pR, u32* pG, u32* pB, u32* pA);
// Copy pixels: bToTexture 0 from a texture into the logo, 1 from the logo into one.
void FE_LogoDesign_CopyLogoTexturePixels(u8* pDst, u8* pSrc, int bToTexture, int nWidth, int nHeight);
// The logo's pixels as a texture (in gLogoTexturePixels).
u8*  FE_LogoDesign_GetLogoAsTexture(u8* pLogo, int nWidth, int nHeight);

extern u8 gLogoTexturePixels[64 * 64];  // a logo's pixels laid out as a texture
                                        // (FE_LogoDesign_GetLogoAsTexture); 64 x 64 or 128 x 32

// ---- the UI's polygon element, loading screen and start-up movies (uiProcessPolygon.c) -------

extern u8 gbUIFirstMenuDraw;       // UI_ClearFirstMenuDraw clears it; the front end's shutdown in game type 3
                                // sets it (uiProcessInterface.c UI_CloseInterface)
void UI_FreeAllEntryPictures(void);
void UI_ClearFirstMenuDraw(void);
void UI_PlayStartUpMovies(void);

#endif
