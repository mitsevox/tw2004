// FE_Manager.c (EA's name, from its StaticMem_Alloc file names): the front end's manager. TW07
// spreads the same functions over FE_Manager.c, FE_Movies.c, FE_UserMgr.c and FE_CrAPUtils.c; here
// they come in this order:
//  - start-up (FE_vOpenONCE) and the golfers' bios from the stream (FE_CharBios_*);
//  - FE_Movies.c's part: movie paths, the movie queue the menus fill and FE_movieFade plays after a
//    fade to black, the intro movie, a trophy ball's replay;
//  - the module's start and shut-down (FE_vInitModule / FE_vCloseModule, FE_InitManager /
//    FE_CloseManager, which copies the created golfers into the golfer table);
//  - the profile backups the menus keep (FE_BackupProfile...), then FE_UserMgr.c's getters
//    (FE_spGetGolfer, FE_GetCurrentProfile, FE_GetCurrUserID) and FE_bIsLicensedGolfer;
//  - FE_CrAPUtils.c's part: Create-A-Player's daily sale, the item lock check
//    (FE_CrAP_IsItemLocked; FE_DateToInt / FE_IntToDate sit among them), the equipment tiers, the
//    random created golfer and its default choices;
//  - FE_vExitUI, which sets the players up when the menus start a game, and the profile backups'
//    moves out to ARAM and back while a game runs.

#include "engine.h"
#include "ustream.h"
#include "game.h"
#include "charstate.h"
#include "core/easb.h"
#include "frontend/fe.h"
#include "core/goaram.h"
#include "llvideo.h"

// Outside this file.
void fn_80037FB4(u8 a, f32* pColor);    // a full-screen colour (GoPostFx.c)
void fn_80010284(void);
void FE_StreamInterruptState(void);
void FE_StreamSetNextState(int a);
void FE_StreamWaitForState(int a);
void FE_vInitFECharModule(void);
void FE_vExecuteClearGolferCache(void);
void UI_FreeTxf2BankPixels(void);
void UI_RestoreAfterMovie(void);
void Gaud_StartFEMusic(int a);
void Gaud_ExitFE(void);
void GameMode4_ExitFE(void);
void FE_CrAP_SetTriggerAnims(u8 b);
void FE_CrAP_UnequipSlot(s16 nSlot);            // FE_CrAPDB.c
u8   PasswordManager_IsPasswordEntered(int a);
u8   PasswordManager_IsSponsorshipPasswordEntered(int n);
s32  fn_801258E8(void);                 // EASportsBio.c
void UI_InitLoadingBarTilePos(void);

// This file, in address order.
void FE_vOpenONCE(void);
void FE_CharBios_FreeStreamMemory(void);
void FE_Manager_FreeStreamMemory(void);
void FE_CharBios_RegisterStreamClients(void);
void FE_Manager_RegisterStreamClients(void);
void FE_CharBios_UnRegisterStreamClients(void);
void FE_CharBios_LoadBIOfromStream(UStreamObject* pObject);
void FE_MakeMoviePathWithSubDir(char* pName, char* pDir, char* pPath);
void FE_MakeBioMoviePath(char* pName, char* pPath);
void FE_movieFade(void);
void FE_PreMovieSetup(void);
void FE_PostMovieSetup(void);
void FE_PlayPGATourMovie(void);
void FE_PlayRTEMovie(void);
void FE_PlayLadderMovie(void);
void FE_PlayIntroMovies(void);
void FE_PlayTrophyBallHighlight(Replay* pReplay);
void FE_vInitModule(void);
void FE_vCloseModule(void);
void FE_InitManager(void);
void FE_CloseManager(void);
void FE_CrAP_UpdateSaleInfo(int a, int b);
u8   FE_CrAP_IsAssetUndesirable(s16 nPart, CrAPAsset* pAsset);
u8   FE_CrAP_IsCrazyHairColor(CrAPAsset* pAsset);
u8   FE_CrAP_IsCrazyHat(CrAPAsset* pAsset);
u8   FE_CrAP_IsCrazyFaceHairColor(CrAPAsset* pAsset);
void FE_CrAP_EquipDefaults(void);
void FE_MoveBackupsToARAM(void);
void FE_RestoreBackupsFromARAM(void);
u8   FE_IsHiddenAttribute(int nAttr);

// This file's globals (fe.h), each section in reverse address order as the compiler lays it out.
FEState gFEState;               // the front end's state: player slots, backup rows, the movie queue
FEProfile* gpFEProfile;         // the profile the menus work on (FE_InitManager .. FE_CloseManager)
u32 gFEBackupAramAddr;          // the profile backups' ARAM address while they are there (0: not)
u32 gFEBackupSize;              // the profile backups' size in bytes (FE_BACKUP_SIZE)
FEBio* gpFEBios;                // the golfers' bios, copied from the 'BIO ' stream object

// A new profile's unlocks (SaveProfile_InitNew, PasswordManager_SetDefaults): golfers 0..29 are
// gStartUnlockedGolfers and gStartLockedGolfers (the ones the single-golfer cheat codes unlock);
// every course is unlocked but gStartLockedCourses. GM_GetGameProgress counts the locked ones since
// unlocked.
s32 gStartLockedCourses[6] = {3, 9, 12, 17, 18, 4};
s32 gStartUnlockedGolfers[16] = {0, 18, 21, 3, 5, 7, 11, 23, 10, 13, 14, 28, 22, 24, 4, 12};
s32 gStartLockedGolfers[14] = {9, 16, 6, 26, 29, 2, 8, 15, 17, 19, 20, 25, 27, 1};

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x80283AC0), before the 0.0f and 0.05f FE_movieFade uses first; its body is unknown.
static f32 FE_Manager_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Sets the front end's state up once, at start-up (gomainloop fn_8006C720): no player slot loaded
// or CPU, no backup rows (aBackup -1), no profile backups (p658), no movie queued, the menus' start
// mode (nMode) -1; b0F, b10 and b18 set, b11 and n1C cleared. Then the points fe_movies.c draws at
// (UI_InitLoadingBarTilePos) and gUILoadingScreen's picture.
void FE_vOpenONCE(void) {
    int i;
    for (i = 0; i < 5; i++) {
        gFEState.aLoaded[i] = 0;
        gFEState.aCPU[i] = 0;
        gFEState.aBackup[i] = -1;
    }
    gFEState.bFirstTime = 1;
    gFEState.b10 = 1;
    gFEState.nMode = -1;
    gFEState.bDemoStarting = 0;
    gFEState.bTourCardWithheld = 1;
    gFEState.nMCRewardMoney = 0;
    gFEState.nMovieNext = 0;
    gFEState.nMovieFree = 0;
    gFEState.p658 = NULL;
    UI_InitLoadingBarTilePos();
    gUILoadingScreen.p30 = NULL;
}

// Frees the golfers' bios (gpFEBios, the copy FE_CharBios_LoadBIOfromStream made), if there are
// any.
void FE_CharBios_FreeStreamMemory(void) {
    if (gpFEBios != NULL) {
        StaticMem_Free(gpFEBios);
        gpFEBios = NULL;
    }
}

// Empty in this build. Called only by FE_CloseManager, right after FE_CharBios_FreeStreamMemory.
void FE_Manager_FreeStreamMemory(void) {
}

// Has the stream loader hand 'BIO ' objects (the golfers' bios) to FE_CharBios_LoadBIOfromStream.
// Called by the front end's stream-client registration (streammanagerhole.c fn_80014668).
void FE_CharBios_RegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback(TAG('B', 'I', 'O', ' '), FE_CharBios_LoadBIOfromStream);
}

// Empty in this build. Called by the front end's stream-client registration (streammanagerhole.c
// fn_80014668), right after FE_CharBios_RegisterStreamClients.
void FE_Manager_RegisterStreamClients(void) {
}

// Stops the stream loader handing 'BIO ' objects to FE_CharBios_LoadBIOfromStream
// (streammanagerhole.c fn_800146C4).
void FE_CharBios_UnRegisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback(TAG('B', 'I', 'O', ' '));
}

// The 'BIO ' stream object's handler: copies its data (the golfers' bios, FEBio records) into a new
// block, gpFEBios, and frees the object.
void FE_CharBios_LoadBIOfromStream(UStreamObject* pObject) {
    gpFEBios = StaticMem_Alloc(pObject->uSize, 2, 32, "FE_Manager.c", 285);
    Mem_cpy(gpFEBios, pObject->pData, pObject->uSize);
    StaticMem_Free(pObject);
}

// A movie's skip test (LLVideo_PlayFile's pfnStop, whose arguments it ignores): any button on any
// controller.
u8 FE_IsMovieSkipPressed(Video* pVideo, int nArg) {
    int i;
    Input_vUpdate();
    for (i = 0; i < 4; i++) {
        if (Input_ReadControlPad(i)) {
            return 1;
        }
    }
    return 0;
}

// A movie's path on the disc: "data/movies/<name>.NGC".
void FE_MakeMoviePath(char* pName, char* pPath) {
    sprintf(pPath, "data/movies/%s.%s", pName, "NGC");
}

// A movie's path in a subfolder of the movies folder: "data/movies/<dir>/<name>.NGC".
void FE_MakeMoviePathWithSubDir(char* pName, char* pDir, char* pPath) {
    sprintf(pPath, "data/movies/%s/%s.%s", pDir, pName, "NGC");
}

// A cameo movie's path: "data/movies/cameos/<name>.NGC" (fe_movies.c).
void FE_MakeCameoMoviePath(char* pName, char* pPath) {
    FE_MakeMoviePathWithSubDir(pName, "cameos", pPath);
}

// A golfer's bio movie's path: "data/movies/bios/<name>.NGC" (FE_movieFade, for "bio<nn>").
void FE_MakeBioMoviePath(char* pName, char* pPath) {
    FE_MakeMoviePathWithSubDir(pName, "bios", pPath);
}

// ---- the movie queue: the menus queue a movie, the screen fades to black and it plays ----------

// Adds an entry to the movie queue (gFEState.aMovies, a ring of FE_NUM_MOVIES) and returns it for
// the caller to fill in (menu messages GM_vPlayCredits, GM_vQueueMovie). Nothing checks for a full
// ring.
FEMovie* FE_movieGetFreeEntry(void) {
    FEMovie* pMovie = &gFEState.aMovies[gFEState.nMovieFree++];
    if (gFEState.nMovieFree % FE_NUM_MOVIES == 0) {
        gFEState.nMovieFree = 0;
    }
    return pMovie;
}

// The queue is empty.
u8 FE_movieIsQueueEmpty(void) {
    return gFEState.nMovieFree == gFEState.nMovieNext;
}

// Once a frame: while a movie is queued, fade the screen to black; once it is black, play the
// movie and take it off the queue.
void FE_movieFade(void) {
    char szPath[256];
    char szName[32];
    f32 vColor[4];
    FEMovie* pMovie;
    if (!FE_movieIsQueueEmpty()) {
        vColor[0] = 0.0f;
        vColor[1] = 0.0f;
        vColor[2] = 0.0f;
        vColor[3] = gUIState.fFade;
        fn_80037FB4(1, vColor);
        gUIState.fFade += 0.05f;
        pMovie = &gFEState.aMovies[gFEState.nMovieNext];
        if (gUIState.fFade >= 1.0f) {
            Gaud_StopMusic();
            FE_PreMovieSetup();
            switch (pMovie->nKind) {
            case FE_MOVIE_CREDITS:
                FE_MakeMoviePath("credits", szPath);
                LLVideo_PlayFile(szPath, FE_IsMovieSkipPressed, 0, 0);
                break;
            case FE_MOVIE_BIO:
                sprintf(szName, "bio%02d", pMovie->nBio + 1);
                FE_MakeBioMoviePath(szName, szPath);
                LLVideo_PlayFile(szPath, FE_IsMovieSkipPressed, 0, 0);
                break;
            case 4:                     // fake match: the original never compares with 4; this empty
                break;                  // case only makes the dispatch test 3 before 1
            }
            FE_PostMovieSetup();
            Gaud_StartFEMusic(0);
            gUIState.fFade = 0.0f;
            gFEState.nMovieNext++;
            if (gFEState.nMovieNext % FE_NUM_MOVIES == 0) {
                gFEState.nMovieNext = 0;
            }
        }
    }
}

// Makes room before a movie plays (FE_movieFade): the menu golfers' streaming is stopped
// (FE_StreamSetNextState(1), interrupted, waited for), the golfer cache cleared, and the pixel data
// of the menus' texture banks freed (UI_FreeTxf2BankPixels).
void FE_PreMovieSetup(void) {
    FE_StreamSetNextState(1);
    FE_StreamInterruptState();
    FE_StreamWaitForState(1);
    FE_vClearGolferCache();
    FE_vExecuteClearGolferCache();
    UI_FreeTxf2BankPixels();
}

// After a movie (FE_movieFade): UI_RestoreAfterMovie (empty) and the menu golfers' textures set up again
// (FE_InitGolferTextures).
void FE_PostMovieSetup(void) {
    UI_RestoreAfterMovie();
    FE_InitGolferTextures();
}

// Empty (in TW07 too). Called when the menus' fade to black ends and game mode 23, the PGA TOUR
// season, starts (uiProcessInterface.c UI_ExitFade).
void FE_PlayPGATourMovie(void) {
}

// Empty in this build (TW07's plays a movie). Called when the menus' fade to black ends and game
// mode 24, the real-time events, starts (uiProcessInterface.c UI_ExitFade).
void FE_PlayRTEMovie(void) {
}

// Empty in this build. Called when the menus' fade to black ends and game mode 4, the ladder,
// starts (uiProcessInterface.c UI_ExitFade).
void FE_PlayLadderMovie(void) {
}

// Plays the intro movie at boot (GoEntry.c), unless bit 0x4000 of the session's flags is set; any
// button skips it (FE_IsMovieSkipPressed).
void FE_PlayIntroMovies(void) {
    char szPath[64];
    if (!(gSession.uFlags & 0x4000)) {
        FE_MakeMoviePath("intro", szPath);
        LLVideo_PlayFile(szPath, FE_IsMovieSkipPressed, 0, 0);
    }
}

// Sets a trophy ball's saved shot up to be replayed (the trophy room, menu message
// GM_vShowAwardReplay): copies it into gReplayData, then game mode 10 (the replay) with its golfer
// as player 0's and its course.
void FE_PlayTrophyBallHighlight(Replay* pReplay) {
    Mem_cpy(&gReplayData, pReplay, sizeof(Replay));
    GM_SetModeType(10);
    Session_SetGolfer(gReplayData.player.golfer.nIndex, 0);
    GM_SetCurrentCourse(gReplayData.nCourse);
}

// Starts the front end (GO_vInitFE): the 'txf ' texture-group stream client (fn_80010284), the
// menus' message table, the manager (FE_InitManager), the menu golfers' module, and the profile
// backups back from ARAM (FE_RestoreBackupsFromARAM).
void FE_vInitModule(void) {
    fn_80010284();
    FE_InitGameMessages();
    FE_InitManager();
    FE_vInitFECharModule();
    FE_RestoreBackupsFromARAM();
}

// Shuts the front end down (gomainloop fn_8006CB2C): the manager (FE_CloseManager: the created
// golfers into the golfer table, the bios and the menus' profile freed), then the profile backups
// out to ARAM (FE_MoveBackupsToARAM).
void FE_vCloseModule(void) {
    FE_CloseManager();
    FE_MoveBackupsToARAM();
}

// Allocates the profile the menus work on (gpFEProfile), cleared: slot 0, the slot's own profile
// (not the working copy), n1 -1; clears every gUITxf2BankState entry and gUILoadingScreen (b18, its
// picture); keeps the hashes of the logo textures "__LogoSquare" and "__LogoRect".
void FE_InitManager(void) {
    int i;
    gpFEProfile = StaticMem_Alloc(sizeof(FEProfile), 2, 16, "FE_Manager.c", 1035);
    Mem_set(gpFEProfile, 0, sizeof(FEProfile));
    gpFEProfile->bCopy = 0;
    gpFEProfile->bEditingCopy = 0;
    gpFEProfile->nSlot = 0;
    gpFEProfile->n1 = -1;
    for (i = 0; i < FE_NUM_801D8890; i++) {
        gUITxf2BankState[i].b0 = 0;
        gUITxf2BankState[i].n4 = 0;
        gUITxf2BankState[i].b1 = 0;
    }
    gUILoadingScreen.b18 = 0;
    gUILoadingScreen.p30 = NULL;
    gpFEProfile->uSquareHash = fn_8000BEE4("__LogoSquare");
    gpFEProfile->uRectHash = fn_8000BEE4("__LogoRect");
    gpFEProfile->bAllGolfersPickable = 0;
    gpFEProfile->bGolferPicked = 0;
    gpFEProfile->nEASBioError = 0;
}

// Closes the manager (FE_vCloseModule). First every player on a created golfer gets its save slot's
// created golfer (gpSaveData[i].createdGolfer) copied over its golfer-table entry, keeping the
// table's hidden attributes (FE_IsHiddenAttribute: aggression, IQ, speed) in both attribute
// blocks; TW07 has this part as FE_TransferUserStatsToGolferStats. Then the bios
// (FE_CharBios_FreeStreamMemory) and the menus' profile (gpFEProfile) are freed.
void FE_CloseManager(void) {
    int i;
    int j;
    int nGolfer;
    s8 aAttr[NUM_ATTRS];
    s8 aAttrAlt[NUM_ATTRS];
    for (i = 0; i < gSession.nNumPlayers; i++) {
        nGolfer = gSession.nGolfer[i];
        if (nGolfer >= FIRST_CREATED_GOLFER) {
            gSession.nGolfer[i] = nGolfer;      // the original stores it back unchanged
            for (j = 0; j < NUM_ATTRS; j++) {
                aAttr[j] = gGolferTable[nGolfer].attr[j];
                aAttrAlt[j] = gGolferTable[nGolfer].attrAlt[j];
            }
            Mem_cpy(&gGolferTable[gSession.nGolfer[i]], &gpSaveData[i].createdGolfer, sizeof(GolferRecord));
            for (j = 0; j < NUM_ATTRS; j++) {
                if (FE_IsHiddenAttribute(j)) {
                    gGolferTable[gSession.nGolfer[i]].attr[j] = aAttr[j];
                    gGolferTable[gSession.nGolfer[i]].attrAlt[j] = aAttrAlt[j];
                }
            }
        }
    }
    FE_CharBios_FreeStreamMemory();
    FE_Manager_FreeStreamMemory();
    StaticMem_Free(gpFEProfile);
}

// Backs up every active save slot's profile (gpSaveData[0..3]) into the row of the same number of
// the backups (gFEState.p658; menu message GM_vBackupAllProfiles).
void FE_BackupAllProfiles(void) {
    int i;
    for (i = 0; i < 4; i++) {
        if (gpSaveData[i].bActive) {
            Mem_cpy(&gFEState.p658[i], &gpSaveData[i], sizeof(SaveProfile));
        }
    }
}

// Backs up save slot nSlot's profile into the backups (gFEState.p658). A slot with a backup row
// (aBackup) overwrites it. A slot without one takes row nSlot: when some row is free (not bActive)
// and it is not row nSlot, the two rows are swapped first (FE_SwapBackupRows), so what sat in row
// nSlot moves to the free row (no aBackup is changed for it); with no free row, row nSlot is
// overwritten.
void FE_BackupProfileClaimRow(int nSlot) {
    int nFree = -1;
    int i;
    if (gFEState.aBackup[nSlot] == -1) {
        for (i = 0; i < 4; i++) {
            if (!gFEState.p658[i].bActive) {
                nFree = i;
                break;
            }
        }
        if (nFree == -1) {
            Mem_cpy(&gFEState.p658[nSlot], &gpSaveData[nSlot], sizeof(SaveProfile));
            gFEState.aBackup[nSlot] = nSlot;
        } else if (nSlot == nFree) {
            Mem_cpy(&gFEState.p658[nSlot], &gpSaveData[nSlot], sizeof(SaveProfile));
            gFEState.aBackup[nSlot] = nSlot;
        } else {
            FE_SwapBackupRows(nFree, nSlot);
            Mem_cpy(&gFEState.p658[nSlot], &gpSaveData[nSlot], sizeof(SaveProfile));
            gFEState.aBackup[nSlot] = nSlot;
        }
    } else {
        Mem_cpy(&gFEState.p658[gFEState.aBackup[nSlot]], &gpSaveData[nSlot], sizeof(SaveProfile));
    }
}

// Backs up save slot nSlot's profile into its backup row (gFEState.aBackup[nSlot], menu message
// GM_vBackupProfile). The slot must have a row: -1 is not checked.
void FE_BackupProfile(int nSlot) {
    Mem_cpy(&gFEState.p658[gFEState.aBackup[nSlot]], &gpSaveData[nSlot], sizeof(SaveProfile));
}

// Swaps backup rows a and b (gFEState.p658) through a temporary block.
void FE_SwapBackupRows(int a, int b) {
    SaveProfile* pTemp = StaticMem_Alloc(sizeof(SaveProfile), 1, 32, "FE_Manager.c", 1194);
    Mem_cpy(pTemp, &gFEState.p658[b], sizeof(SaveProfile));
    Mem_cpy(&gFEState.p658[b], &gFEState.p658[a], sizeof(SaveProfile));
    Mem_cpy(&gFEState.p658[a], pTemp, sizeof(SaveProfile));
    StaticMem_Free(pTemp);
}

// Golfer nGolfer's record: its golfer-table entry, or for a created golfer (FIRST_CREATED_GOLFER
// and up) the created golfer of the profile the menus work on (FE_GetCurrentProfile).
GolferRecord* FE_spGetGolfer(int nGolfer) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    if (nGolfer < FIRST_CREATED_GOLFER) {
        return &gGolferTable[nGolfer];
    }
    return &pProfile->createdGolfer;
}

// ---- the profile being worked on ---------------------------------------------------------------

// The profile the front end is working on: its working copy while bCopy is set, else the slot's own
// save profile.
SaveProfile* FE_GetCurrentProfile(void) {
    if (gpFEProfile->bCopy) {
        return &gpFEProfile->profile;
    }
    return &gpSaveData[gpFEProfile->nSlot];
}

// The player slot whose profile the menus work on (gpFEProfile->nSlot, the slot
// FE_GetCurrentProfile reads when not on the working copy).
int FE_GetCurrUserID(void) {
    return gpFEProfile->nSlot;
}

// Whether golfer nGolfer is one of the 16 licensed golfers (ids 0, 1, 3, 4, 5, 7, 10, 11, 13, 14,
// 18, 21, 22, 23, 24 and 28). Session_SetupProfiles gives them ball type 0 and the other golfers a
// ball picked from their spin.
u8 FE_bIsLicensedGolfer(int n) {
    if (n == 0 || n == 1 || n == 3 || n == 4 || n == 5 || n == 7 || n == 10 || n == 11 || n == 13 ||
        n == 14 || n == 18 || n == 21 || n == 22 || n == 23 || n == 24 || n == 28) {
        return 1;
    }
    return 0;
}

// Picks today's sale items (FE_CrAP_UpdateSaleInfo) for the three sale categories (-1 clothes, -2
// accessories, -3 clubs and balls) and both genders; run once the Create-A-Player assets are loaded
// (FE_CrAP_PostAssetsLoad).
void FE_SetupSaleInfo(void) {
    FE_CrAP_UpdateSaleInfo(-1, 0);
    FE_CrAP_UpdateSaleInfo(-1, 1);
    FE_CrAP_UpdateSaleInfo(-2, 0);
    FE_CrAP_UpdateSaleInfo(-2, 1);
    FE_CrAP_UpdateSaleInfo(-3, 0);
    FE_CrAP_UpdateSaleInfo(-3, 1);
}

// A sale category (-1, -2 or -3) as the index 0, 1 or 2 of FEProfile's sale arrays; anything else
// 0.
int FE_GetSaleIDFromSaleCategory(int n) {
    switch (n) {
    case -1:
        return 0;
    case -2:
        return 1;
    case -3:
        return 2;
    default:
        return 0;
    }
}

// Picks today's sale items for gender b and sale category a (-1 clothes: parts 0, 1, 2, 7; -2
// accessories: 8, 19, 20; -3 clubs and balls: 12; anything else: nothing), seeded by today's date
// (FE_DateToInt; 3081979 if that is 0): one of the category's parts at random, then up to five
// different assets of that part for gender b (or either, gender 2) that are unlocked for the
// current profile (FE_CrAP_IsItemLocked) and have a level above 0. They go in gpFEProfile's
// aSalePart, aSaleEntry and aSaleChoice (-1: none). The random stream is then seeded from the clock
// again (gSession.nSeed).
void FE_CrAP_UpdateSaleInfo(int a, int b) {
    int aFound[3000];
    int aKinds[88];         // fake match: 4 are used; 88 gives the original's stack frame
    s32 nMonth;
    s32 nDay;
    s32 nYear;
    s32 nHour;
    s32 nMinute;
    s32 nSecond;
    s32 nMsec;
    s32 nPart;
    s32 nChoice;
    s16 nKind;
    int j;
    SaveProfile* pProfile;
    int nCount;
    int nCategory;
    int nSeed;
    int i;
    int nFound;
    int nKinds;
    s8 nB;                  // b as FE_CrAP_SetCurrentGender and FE_CrAP_GetAssetGender take it
    pProfile = FE_GetCurrentProfile();
    nFound = 0;
    nCount = FE_CrAP_GetNumEntriesInCrAPDB();
    nKind = 0;
    nPart = 0;
    nChoice = 0;
    nCategory = FE_GetSaleIDFromSaleCategory(a);
    switch (a) {
    case -1:
        aKinds[0] = 0;
        aKinds[1] = 1;
        aKinds[2] = 2;
        aKinds[3] = 7;
        nKinds = 4;
        break;
    case -2:
        aKinds[0] = 8;
        aKinds[1] = 19;
        aKinds[2] = 20;
        nKinds = 3;
        break;
    case -3:
        aKinds[0] = 12;
        nKinds = 1;
        break;
    default:
        return;
    }
    fn_8011E020(&nMonth, &nDay, &nYear, &nHour, &nMinute, &nSecond, &nMsec);
    nSeed = FE_DateToInt(nMonth, nDay, nYear);
    for (j = 0; j < 5; j++) {
        gpFEProfile->aSaleEntry[b][nCategory][j] = -1;
        gpFEProfile->aSaleChoice[b][nCategory][j] = -1;
    }
    gpFEProfile->nDateSeed = nSeed;
    if (gpFEProfile->nDateSeed == 0) {
        gpFEProfile->nDateSeed = 3081979;          // 8/3/1979, packed as FE_DateToInt does
    }
    Misc_SetSeedFunc(0, gpFEProfile->nDateSeed);
    gpFEProfile->aSalePart[b][nCategory] = aKinds[Misc_RandFunc(0) % nKinds];
    nB = b;
    for (i = 0; i < nCount; i++) {
        FE_CrAP_SetCurrentGender(FE_CrAP_GetAssetGender(i));
        nKind = FE_CrAP_GetCategoryFromAssetID(i);
        if (nKind == gpFEProfile->aSalePart[b][nCategory] &&
            (FE_CrAP_GetAssetGender(i) == nB || FE_CrAP_GetAssetGender(i) == 2) &&
            !FE_CrAP_IsItemLocked(i, pProfile) && FE_CrAP_GetLevelFromAssetID(i) > 0) {
            aFound[nFound] = i;
            nFound++;
        }
    }
    FE_CrAP_SetCurrentGender(nB);
    for (j = 0; j < 5; j++) {
        if (j >= nFound) break;
    retry:
        FE_CrAP_GetCategorySubcategoryAndEntryNumFromAssetID(aFound[Misc_RandFunc(0) % nFound], &nKind,
                &nPart, &nChoice);
        gpFEProfile->aSaleEntry[b][nCategory][j] = nPart;
        gpFEProfile->aSaleChoice[b][nCategory][j] = nChoice;
        for (i = 0; i < j; i++) {
            if (gpFEProfile->aSaleEntry[b][nCategory][j] == gpFEProfile->aSaleEntry[b][nCategory][i] &&
                gpFEProfile->aSaleChoice[b][nCategory][j] == gpFEProfile->aSaleChoice[b][nCategory][i]) {
                goto retry;                         // fake match: a do-while scores 98.3
            }
        }
    }
    gSession.nSeed = Misc_CreateRandomSeed();
    Misc_SetSeedFunc(0, gSession.nSeed);
}

// Whether Create-A-Player asset nAsset is still locked for profile pProfile: never with bit 0x4000
// of the session's flags or cheat bit 0 (PasswordManager_IsPasswordEntered(0)). The asset gives a
// lock kind (FE_CrAP_GetPartGMLockIDByAssetNum) and a value n (FE_CrAP_GetPartGMLockValByAssetNum).
// Unlocked by kind:
//   0 bit n of aAssetOwned (bought)       2 bit 1 of a10548 (UserInfo_GetUserFlag)
//   6 cheat bit n + 1 (codes "A".."E")    7 award aC8[n] won; 8 n of those 31 won
//   9 PGA TOUR season n reached           10 sponsor n's code entered
//                                            (PasswordManager_IsSponsorshipPasswordEntered) or signed
//   11 n sponsors signed                  12 an EA Sports Bio of level n or more
//   14 ladder award n; 15 n of the 25     16 game progress n (GM_GetGameProgress)
//   17 real-time event award n; 18 n of the 75
//   19 the best medal (0) in challenge group n; 20 n challenge groups counted (EA bug there)
//   21 award n; 22 n of the first 23      23 award 23 + n (a bonus trophy ball)
//   24 n bonus trophy balls, but the count never runs (EA bug there)
//   25 a1C0[12 + n]; 26 n of a1C0[12..15] 27 TOUR card level n
// Kinds 3 and 13 are always locked; -1, 4, 28 and the rest never.
u8 FE_CrAP_IsItemLocked(s32 nAsset, SaveProfile* pProfile) {
    int aBits[5] = {1, 2, 3, 4, 5};
    int nCount = 0;
    s8 nKind;
    s16 n;
    u8 bLocked;
    int i;
    if (gSession.uFlags & 0x4000) {
        return 0;
    }
    nKind = FE_CrAP_GetPartGMLockIDByAssetNum(nAsset);
    n = FE_CrAP_GetPartGMLockValByAssetNum(nAsset);
    if (PasswordManager_IsPasswordEntered(0)) {
        return 0;
    }
    switch (nKind) {
    case 0:
        bLocked = BitArray_TestBit(pProfile->aAssetOwned, n) == 0;
        break;
    case 2:
        bLocked = !UserInfo_GetUserFlag(pProfile, 1);
        break;
    case 3:
        bLocked = 1;
        break;
    case 4:
        bLocked = 0;
        break;
    case 6:
        bLocked = BitArray_TestBit(gPasswordEnteredBits, aBits[n]) == 0;
        break;
    case 7:
        bLocked = !pProfile->aC8[n].award.bWon;
        break;
    case 8:
        bLocked = 1;
        for (i = 0; i < 31; i++) {
            if (pProfile->aC8[i].award.bWon) {
                nCount++;
            }
        }
        if (nCount >= n) {
            bLocked = 0;
        }
        break;
    case 9:
        bLocked = pProfile->tour.nSeason < n;
        break;
    case 10:
        bLocked = 1;
        if (PasswordManager_IsSponsorshipPasswordEntered(n)) {
            bLocked = 0;
        }
        for (i = 0; i < 11; i++) {
            if (pProfile->aSponsor[i].bSigned && pProfile->aSponsor[i].nSponsor == n) {
                bLocked = 0;
            }
        }
        break;
    case 11:
        bLocked = 1;
        for (i = 0; i < 11; i++) {
            if (pProfile->aSponsor[i].bSigned) {
                nCount++;
            }
        }
        if (nCount >= n) {
            bLocked = 0;
        }
        break;
    case 12:
        if (EASBio_IsBioLoaded() && n <= fn_801258E8()) {
            bLocked = 0;
        } else {
            bLocked = 1;
        }
        break;
    case 13:
        bLocked = 1;
        break;
    case 14:
        bLocked = !pProfile->aLadderAward[n].bWon;
        break;
    case 15:
        bLocked = 1;
        for (i = 0; i < 25; i++) {
            if (pProfile->aLadderAward[i].bWon) {
                nCount++;
            }
        }
        if (nCount >= n) {
            bLocked = 0;
        }
        break;
    case 16:
        bLocked = n > GM_GetGameProgress(pProfile);
        break;
    case 17:
        bLocked = !pProfile->aRTEAward[n].bWon;
        break;
    case 18:
        bLocked = 1;
        for (i = 0; i < 75; i++) {
            if (pProfile->aRTEAward[i].bWon) {
                nCount++;
            }
        }
        if (nCount >= n) {
            bLocked = 0;
        }
        break;
    case 19:
        bLocked = pProfile->aMedal[n] != 0;
        break;
    case 20:
        bLocked = 1;
        if (pProfile->nTourCardLevel >= 1) {
            nCount = 1;
        }
        // EA bug: counts the groups whose aMedal is not 0, the best medal. aMedal is 3 for no medal
        // (SaveProfile_InitNew sets all 29 to 3), so a new profile already counts 29 and unlocks
        // every item of this kind with n up to 29, and each best medal won lowers the count. The
        // menus' count of groups with a medal (FE_MessageTable.c message 175) tests aMedal != 3.
        for (i = 0; i < 29; i++) {
            if (pProfile->aMedal[i]) {
                nCount++;
            }
        }
        if (nCount >= n) {
            bLocked = 0;
        }
        break;
    case 21:
        bLocked = !pProfile->aAward[n].bWon;
        break;
    case 22:
        bLocked = 1;
        for (i = 0; i < 23; i++) {
            if (pProfile->aAward[i].bWon) {
                nCount++;
            }
        }
        if (nCount >= n) {
            bLocked = 0;
        }
        break;
    case 23:
        n += 23;
        bLocked = !pProfile->aAward[n].bWon;
        break;
    case 24:
        bLocked = 1;
        // EA bug: i starts at 23 and runs while i < 16, so nothing is counted and only n <= 0
        // unlocks; kind 23 reads bonus trophy ball n as aAward[23 + n] (awards 23..38).
        for (i = 23; i < 16; i++) {
            if (pProfile->aAward[i].bWon) {
                nCount++;
            }
        }
        if (nCount >= n) {
            bLocked = 0;
        }
        break;
    case 25:
        bLocked = !pProfile->a1C0[n + 12].bWon;
        break;
    case 26:
        bLocked = 1;
        for (i = 12; i < 16; i++) {
            if (pProfile->a1C0[i].bWon) {
                nCount++;
            }
        }
        if (nCount >= n) {
            bLocked = 0;
        }
        break;
    case 27:
        bLocked = pProfile->nTourCardLevel < n;
        break;
    case -1:
        bLocked = 0;
        break;
    case 28:
        bLocked = 0;
        break;
    default:
        bLocked = 0;
        break;
    }
    return bLocked;
}

// Packs a date into one number: day * 1000000 + month * 10000 + year (FE_IntToDate unpacks it).
int FE_DateToInt(int nMonth, int nDay, int nYear) {
    int nDate = nYear;
    nDate += nMonth * 10000;
    nDate += nDay * 1000000;
    return nDate;
}

// Unpacks a date FE_DateToInt packed into its month, day and year.
void FE_IntToDate(int nDate, int* pMonth, int* pDay, int* pYear) {
    *pDay = nDate / 1000000;
    nDate -= *pDay * 1000000;
    *pMonth = nDate / 10000;
    nDate -= *pMonth * 10000;
    *pYear = nDate;
}

// Notes which Create-A-Player assets are locked for pProfile, one bit each in aAssetLocked
// (FE_CrAP_IsItemLocked, each tested with its own gender made current); nothing before the asset
// database is loaded (menu message GM_vCheckCrAPUnlocks).
void FE_CrAP_SetupLockedAssets(SaveProfile* pProfile) {
    int i;
    int nCount;
    s8 nSaved;
    if (FE_CrAP_IsCrAPDBLoaded()) {
        nSaved = FE_CrAP_GetCurrentGender();
        nCount = FE_CrAP_GetNumEntriesInCrAPDB();
        for (i = 0; i < nCount; i++) {
            FE_CrAP_SetCurrentGender(FE_CrAP_GetAssetGender(i));
            if (FE_CrAP_IsItemLocked(i, pProfile)) {
                BitArray_SetBit(pProfile->aAssetLocked, i);
            } else {
                BitArray_ClearBit(pProfile->aAssetLocked, i);
            }
        }
        FE_CrAP_SetCurrentGender(nSaved);
    }
}

// ---- the created golfer's parts -----------------------------------------------------------------

// pProfile's created golfer's equipment tiers, from the equipment in the 53 slots of the profile
// being worked on (FE_CrAP_GetEquippedAsset reads that profile, not pProfile; every caller passes
// it): each slot can raise up to two attributes' tiers.
void FE_CrAP_UpdateUserAttributeMods(SaveProfile* pProfile) {
    s32 i;
    int nAsset;
    s16 nSlot;
    int nAttrA;
    int nAttrB;
    int nTierA;
    int nTierB;
    if (FE_CrAP_IsCrAPDBLoaded()) {
        for (i = 0; i < NUM_ATTRS; i++) {
            pProfile->createdGolfer.tier[i] = 0;
        }
        for (nSlot = 0; nSlot < 53; nSlot++) {
            nAsset = FE_CrAP_GetEquippedAsset(nSlot);
            if (nAsset >= 0) {
                nAttrA = FE_CrAP_GetPartAttributeUpgrade1ByAssetID(nAsset);
                nAttrB = FE_CrAP_GetPartAttributeUpgrade2ByAssetID(nAsset);
                nTierA = FE_CrAP_GetPartAttributeModifier1ByAssetID(nAsset);
                nTierB = FE_CrAP_GetPartAttributeModifier2ByAssetID(nAsset);
                if (nAttrA >= 0 && pProfile->createdGolfer.tier[nAttrA] < nTierA) {
                    pProfile->createdGolfer.tier[nAttrA] = nTierA;
                }
                if (nAttrB >= 0 && pProfile->createdGolfer.tier[nAttrB] < nTierB) {
                    pProfile->createdGolfer.tier[nAttrB] = nTierB;
                }
            }
        }
    }
}

// Only part 10 (the face) has undesirable choices: scars, tattoos, acne, old age and make-up.
u8 FE_CrAP_IsAssetUndesirable(s16 nPart, CrAPAsset* pAsset) {
    switch (nPart) {
    case 10:
        if (stricmp(pAsset->szName, "Cheek Scar & Tat") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Facial Tattoo") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Acne") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Weathered") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Old") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Gold Makeup") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Beauty 4") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Punk") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Punk Makeup") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Alt Punk") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Alt Punk Makeup") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Pink Makeup") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Old Makeup") == 0) {
            return 1;
        }
        if (stricmp(pAsset->szName, "Old") == 0) {      // EA's list tests "Old" twice
            return 1;
        }
        break;
    }
    return 0;
}

// Puts a random choice on part nPart: a desirable one (not FE_CrAP_IsAssetUndesirable) nChance
// percent of the time, else an undesirable one; only choices FE_CrAP_IsAssetAvailableForUser
// allows, and nothing changes when none fits.
void FE_CrAP_RandomizeCategoryWithUndesirableTest(s16 nPart, int nChance) {
    int aChoices[250];
    u32 bDesirable = (int)(Misc_RandFunc(0) % 100) < nChance;
    int nFound = 0;
    int nCount = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(nPart, 0);
    int i;
    int nAsset;
    CrAPAsset* pAsset;
    for (i = 0; i < nCount; i++) {
        nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, 0, i);
        pAsset = FE_CrAP_GetAssetFromAssetIndex(nAsset);
        if (bDesirable && !FE_CrAP_IsAssetUndesirable(nPart, pAsset)
            && FE_CrAP_IsAssetAvailableForUser(nAsset)) {
            aChoices[nFound++] = i;
        } else if (!bDesirable && FE_CrAP_IsAssetUndesirable(nPart, pAsset)
                   && FE_CrAP_IsAssetAvailableForUser(nAsset)) {
            aChoices[nFound++] = i;
        }
    }
    if (nFound) {
        FE_CrAP_TurnOnPart(nPart, 0, aChoices[Misc_RandFunc(0) % nFound]);
    }
}

// The asset is named one of eight bright colours (White, Bright Red, Orange, Pink, Yellow, Green,
// Purple, Blue).
u8 FE_CrAP_IsCrazyHairColor(CrAPAsset* pAsset) {
    if (pAsset == NULL) {
        return 0;
    }
    if (stricmp(pAsset->szName, "White") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Bright Red") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Orange") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Pink") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Yellow") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Green") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Purple") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Blue") == 0) {
        return 1;
    }
    return 0;
}

// Anything worn on the head but a plain hat (one worn backwards counts) or a visor.
u8 FE_CrAP_IsCrazyHat(CrAPAsset* pAsset) {
    if (pAsset == NULL) {
        return 0;
    }
    if (stricmp(FE_CrAP_GetStringFromTable(pAsset->nCategory), "Hats") == 0 &&
        strstr(pAsset->szName, "backwards") == NULL) {
        return 0;
    }
    if (stricmp(FE_CrAP_GetStringFromTable(pAsset->nCategory), "Visors") == 0) {
        return 0;
    }
    return 1;
}

// The same test as FE_CrAP_IsCrazyHairColor.
u8 FE_CrAP_IsCrazyFaceHairColor(CrAPAsset* pAsset) {
    if (pAsset == NULL) {
        return 0;
    }
    if (stricmp(pAsset->szName, "White") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Bright Red") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Orange") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Pink") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Yellow") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Green") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Purple") == 0) {
        return 1;
    }
    if (stricmp(pAsset->szName, "Blue") == 0) {
        return 1;
    }
    return 0;
}

// A random look for the created golfer (menu message GM_vRandomizeCrAPLookInFaceShot, and
// FE_CrAP_RandomizeAll): random parts 10, 9 and 16; hair (part 3) with a 10% chance of corn rows,
// an afro or a mohawk; parts 4, 5 and 6 on a random choice 20%, 10% and 10% of the time, else their
// first; part 14 a crazy colour (FE_CrAP_IsCrazyHairColor) one time in five, and part 15 the same
// choice 90% of the time, else its own pick; a hat (part 0) 40% of the time, a crazy one
// (FE_CrAP_IsCrazyHat) one time in five, else part 0's first choice. Slots 5..8 and 11..14 are
// emptied, then parts 19 and 20 are put on 30% of the time each; 20% of the time part 19 gets a
// choice drawn from the count of its subcategory 3 but turned on in subcategory 0; and part 8 5% of
// the time (else slot 13 emptied).
void FE_CrAP_RandomizeFace(SaveProfile* pProfile) {
    char szDebug[256];
    u8 bPicking;
    u8 bChance;
    int nAsset;
    int nCount;
    int nPick;
    int nPrev;
    CrAPAsset* pAsset;
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 10, 0);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 9, 0);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 16, 0);

    bChance = Misc_RandFunc(0) % 100 < 10;
    bPicking = 1;
    nCount = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(3, 0);
    while (bPicking) {
        nPick = Misc_RandFunc(0) % nCount;
        pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(3, 0, nPick);
        if (bChance && pAsset &&
            (stricmp(pAsset->szName, "Corn Rows") == 0 || stricmp(pAsset->szName, "Afro") == 0 ||
             stricmp(pAsset->szName, "Mohawk") == 0)) {
            FE_CrAP_TurnOnPart(3, 0, nPick);
            bPicking = 0;
        }
        if (!bChance &&
            (pAsset == NULL || (stricmp(pAsset->szName, "Corn Rows") != 0 &&
                                stricmp(pAsset->szName, "Afro") != 0 &&
                                stricmp(pAsset->szName, "Mohawk") != 0))) {
            FE_CrAP_TurnOnPart(3, 0, nPick);
            bPicking = 0;
        }
    }

    bChance = Misc_RandFunc(0) % 100 < 20;
    if (bChance) {
        nCount = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(4, 0);
        nPick = Misc_RandFunc(0) % (nCount - 1);
        nPick++;
        FE_CrAP_TurnOnPart(4, 0, nPick);
    } else {
        FE_CrAP_TurnOnPart(4, 0, 0);
    }
    bChance = Misc_RandFunc(0) % 100 < 10;
    if (bChance) {
        nCount = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(5, 0);
        nPick = Misc_RandFunc(0) % (nCount - 1);
        nPick++;
        FE_CrAP_TurnOnPart(5, 0, nPick);
    } else {
        FE_CrAP_TurnOnPart(5, 0, 0);
    }
    bChance = Misc_RandFunc(0) % 100 < 10;
    if (bChance) {
        nCount = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(6, 0);
        if (nCount != -1) {
            if (nCount > 1) {
                nPick = Misc_RandFunc(0) % (nCount - 1);
            } else {
                nPick = Misc_RandFunc(0) % (nCount - 1);   // EA bug: divides by zero for one choice
                nPick++;
            }
            FE_CrAP_TurnOnPart(6, 0, nPick);
        }
    } else {
        FE_CrAP_TurnOnPart(6, 0, 0);
    }

    bChance = Misc_RandFunc(0) % 100 < 80;
    bPicking = 1;
    nCount = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(14, 0);
    while (bPicking) {
        nPick = Misc_RandFunc(0) % nCount;
        pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(14, 0, nPick);
        if (bChance && !FE_CrAP_IsCrazyHairColor(pAsset)) {
            FE_CrAP_TurnOnPart(14, 0, nPick);
            bPicking = 0;
        }
        if (!bChance && FE_CrAP_IsCrazyHairColor(pAsset)) {
            FE_CrAP_TurnOnPart(14, 0, nPick);
            bPicking = 0;
        }
        nPrev = nPick;
    }
    bChance = Misc_RandFunc(0) % 100 < 90;
    if (bChance) {
        FE_CrAP_TurnOnPart(15, 0, nPrev);
    } else {
        bChance = Misc_RandFunc(0) % 100 < 80;
        bPicking = 1;
        nCount = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(15, 0);
        while (bPicking) {
            nPick = Misc_RandFunc(0) % nCount;
            pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(15, 0, nPick);
            if (bChance && !FE_CrAP_IsCrazyFaceHairColor(pAsset)) {
                FE_CrAP_TurnOnPart(15, 0, nPick);
                bPicking = 0;
            }
            if (!bChance && FE_CrAP_IsCrazyFaceHairColor(pAsset)) {
                FE_CrAP_TurnOnPart(15, 0, nPick);
                bPicking = 0;
            }
        }
    }

    bChance = Misc_RandFunc(0) % 100 < 40;
    if (bChance) {
        bChance = Misc_RandFunc(0) % 100 < 80;
        bPicking = 1;
        nCount = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(0, 0);
        while (bPicking) {
            nPick = Misc_RandFunc(0) % nCount;
            pAsset = FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(0, 0, nPick);
            if (bChance && !FE_CrAP_IsCrazyHat(pAsset)) {
                FE_CrAP_TurnOnPart(0, 0, nPick);
                bPicking = 0;
            }
            if (!bChance && FE_CrAP_IsCrazyHat(pAsset)) {
                FE_CrAP_TurnOnPart(0, 0, nPick);
                bPicking = 0;
            }
        }
        nAsset = FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(0, 0, nPick);
        sprintf(szDebug, "I hate everone: %d", nAsset);     // a leftover debug line; never shown
    } else {
        FE_CrAP_TurnOnPart(0, 0, 0);
    }

    FE_CrAP_UnequipSlot(5);
    FE_CrAP_UnequipSlot(6);
    FE_CrAP_UnequipSlot(7);
    FE_CrAP_UnequipSlot(8);
    FE_CrAP_UnequipSlot(11);
    FE_CrAP_UnequipSlot(12);
    FE_CrAP_UnequipSlot(13);
    FE_CrAP_UnequipSlot(14);
    if (Misc_RandFunc(0) % 100 < 30) {
        FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 19, 0);
    }
    if (Misc_RandFunc(0) % 100 < 30) {
        FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 20, 0);
    }
    bChance = Misc_RandFunc(0) % 100 < 20;
    if (bChance) {
        nCount = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(19, 3);
        nPick = Misc_RandFunc(0) % nCount;
        FE_CrAP_GetAssetFromCategoryAndSubCategoryIndexAndEntryNum(19, 3, nPick);
        FE_CrAP_TurnOnPart(19, 0, nPick);
    }
    bChance = Misc_RandFunc(0) % 100 < 5;
    if (bChance) {
        nCount = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(8, 0);
        FE_CrAP_TurnOnPart(8, 0, Misc_RandFunc(0) % nCount);
    } else {
        FE_CrAP_UnequipSlot(13);
    }
}

// A whole random created golfer (menu message GM_vRandomizeCrAPGolferInIdleShot): every subcategory
// of part 12 (clubs and balls) but 4 at its first choice, FE_SetCrapClub(0), random parts 1, 2 and
// 7, then part 7 again in subcategory 1 or 2, and a random look (FE_CrAP_RandomizeFace).
void FE_CrAP_RandomizeAll(SaveProfile* pProfile) {
    FE_CrAP_TurnOnPart(0xC, 1, 0);
    FE_CrAP_TurnOnPart(0xC, 2, 0);
    FE_CrAP_TurnOnPart(0xC, 3, 0);
    FE_CrAP_TurnOnPart(0xC, 5, 0);
    FE_CrAP_TurnOnPart(0xC, 6, 0);
    FE_CrAP_TurnOnPart(0xC, 7, 0);
    FE_CrAP_TurnOnPart(0xC, 0, 0);
    FE_SetCrapClub(0);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 1, 0);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 2, 0);
    FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 7, 0);
    FE_CrAP_RandomizeCrAPCategoryAndSubcategoryItem(pProfile, 7, (Misc_RandFunc(0) & 1) + 1, 0);
    FE_CrAP_RandomizeFace(pProfile);
}

// Part nPart at a random subcategory (FE_CrAP_GetNumberOfSubcategoryIndicesForCategory; 0 when it
// has none), then a random choice in it (FE_CrAP_RandomizeCrAPCategoryAndSubcategoryItem with the
// same nChance), which is returned.
int FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(SaveProfile* pProfile, s16 nPart, int nChance) {
    int nCount = FE_CrAP_GetNumberOfSubcategoryIndicesForCategory(nPart);
    int nPick;
    if (nCount > 0) {
        nPick = Misc_RandFunc(0) % nCount;
    } else {
        nPick = 0;
    }
    return FE_CrAP_RandomizeCrAPCategoryAndSubcategoryItem(pProfile, nPart, nPick, nChance);
}

// Part nPart, subcategory b at a random choice, which is returned (-1: nothing was picked). With a
// chance of nChance percent the part is left alone, except with bit 0x4000 of the session's flags
// (which also skips the intro movie), where it gets its first choice. Outside that mode only
// choices FE_CrAP_IsAssetAvailableForUser allows are drawn (at most 250; none: the first choice).
int FE_CrAP_RandomizeCrAPCategoryAndSubcategoryItem(SaveProfile* pProfile, s16 nPart, int b, int nChance) {
    int aChoices[250];
    int nFound = 0;
    int nCount;
    int i;
    int nPick;
    FE_CrAP_GetNumEntriesInCrAPDB();
    if ((int)(Misc_RandFunc(0) % 100) + 1 <= nChance) {
        if (gSession.uFlags & 0x4000) {
            FE_CrAP_TurnOnPart(nPart, b, 0);
            return 0;
        }
        return -1;
    }
    if (gSession.uFlags & 0x4000) {
        nCount = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(nPart, b);
        if (nCount != 0) {
            nPick = Misc_RandFunc(0) % nCount;
            FE_CrAP_TurnOnPart(nPart, b, nPick);
            return nPick;
        }
    } else {
        nCount = FE_CrAP_GetNumberOfEntriesForCategoryAndSubcategoryIndex(nPart, b);
        for (i = 0; i < nCount; i++) {
            if (FE_CrAP_IsAssetAvailableForUser(
                    FE_CrAP_GetAssetIndexFromCategoryAndSubCategoryIndexAndEntryNum(nPart, b, i))) {
                aChoices[nFound] = i;
                nFound++;
            }
            if (nFound == 250) break;
        }
        if (nFound == 0) {
            FE_CrAP_TurnOnPart(nPart, b, 0);
            return 0;
        }
        nPick = Misc_RandFunc(0) % nFound;
        FE_CrAP_TurnOnPart(nPart, b, aChoices[nPick]);
        return aChoices[nPick];
    }
    return -1;
}

// Only while the menus work on their working copy (bCopy): with the Create-A-Player trigger
// animations off, a random part 9, then the first choice of parts 3 (hair), 14, 15, 16 and of every
// subcategory of part 12 (clubs and balls), choices 0..7 of part 13's subcategories 0 and 1 and the
// first of its subcategory 2. Called when the menu golfer is golfer 7 or 29 and its textures are
// set up (FEgolferanim.c FE_StreamFunc_TexturesInit).
void FE_CrAP_EquipDefaults(void) {
    SaveProfile* pProfile = FE_GetCurrentProfile();
    int i;
    if (gpFEProfile->bCopy) {
        FE_CrAP_SetTriggerAnims(0);
        FE_CrAP_RandomizeCrAPCategoryInOneSubcategory(pProfile, 9, 0);
        FE_CrAP_TurnOnPart(3, 0, 0);
        FE_CrAP_TurnOnPart(0xE, 0, 0);
        FE_CrAP_TurnOnPart(0xF, 0, 0);
        FE_CrAP_TurnOnPart(0x10, 0, 0);
        FE_CrAP_TurnOnPart(0xC, 0, 0);
        FE_CrAP_TurnOnPart(0xC, 1, 0);
        FE_CrAP_TurnOnPart(0xC, 2, 0);
        FE_CrAP_TurnOnPart(0xC, 3, 0);
        FE_CrAP_TurnOnPart(0xC, 5, 0);
        FE_CrAP_TurnOnPart(0xC, 6, 0);
        FE_CrAP_TurnOnPart(0xC, 7, 0);
        FE_CrAP_TurnOnPart(0xC, 4, 0);
        for (i = 0; i < 8; i++) {
            FE_CrAP_TurnOnPart(0xD, 0, i);
            FE_CrAP_TurnOnPart(0xD, 1, i);
        }
        FE_CrAP_TurnOnPart(0xD, 2, 0);
        FE_CrAP_SetTriggerAnims(1);
    }
}

// Leaves the menus for a game (menu message GM_vSetupPlayers). In game modes 5 and 11, when slot 0
// holds a profile its backup row becomes row 0 and player 0 plays the created golfer (unless
// gpFEProfile->bGolferPicked is set); else player 0 plays golfer 0. Each player slot is a CPU player (no
// profile, no bag), a loaded profile (a created golfer brings its own bag), a CPU controller (no
// profile) or a table golfer with its bag (its save slot still marked active); slots past the
// players have no profile. Then the menus' return mode (gFEState.nMode): the game mode, or 4 (a
// ladder event), 23 (PGA TOUR), 27 (mode 10 with gpFEProfile->bAwardReplay set) or 28 (lessons with a TOUR
// card). Last: b11 cleared, the fade to black started, the demo off, and the front end's audio and
// ladder closed (Gaud_ExitFE, GameMode4_ExitFE).
void FE_vExitUI(void) {
    int i;
    if (Game_GetMode() == 5 || Game_GetMode() == 11) {
        if (gpSaveData[0].bActive) {
            gFEState.aBackup[0] = 0;
            if (!gpFEProfile->bGolferPicked) {
                Session_SetGolfer(FIRST_CREATED_GOLFER, 0);
            }
        } else {
            Session_SetGolfer(0, 0);
        }
    }
    for (i = 0; i < 5; i++) {
        if (gFEState.aCPU[i]) {
            gSession.nController[i] = CONTROLLER_CPU;
            gSession.uBag[i] = 0;
            gpSaveData[i].bActive = 0;
        } else if (gFEState.aLoaded[i]) {
            gpSaveData[i].bActive = 1;
            if (gSession.nGolfer[i] >= FIRST_CREATED_GOLFER) {
                gSession.uBag[i] = gpSaveData[i].createdGolfer.uBagMask;
            }
        } else if (gSession.nController[i] == CONTROLLER_CPU) {
            gpSaveData[i].bActive = 0;
        } else if (!gFEState.aLoaded[i]) {
            gpSaveData[i].bActive = 1;
            gSession.uBag[i] = gGolferTable[gSession.nGolfer[i]].uBagMask;
        }
        if (i > gSession.nNumPlayers - 1) {
            gpSaveData[i].bActive = 0;
        }
    }
    gFEState.nMode = Game_GetMode();
    if (GameMode4_IsEventRunning()) {
        gFEState.nMode = 4;
    }
    if (GM_Currently_PgaTourMode()) {
        gFEState.nMode = 23;
    }
    if (gpFEProfile->bAwardReplay && Game_GetMode() == 10) {
        gFEState.nMode = 27;
    }
    if (gFEState.nMode == 11 && gpSaveData[gpFEProfile->nSlot].nTourCardLevel > 0) {
        gFEState.nMode = 28;
    }
    gFEState.bDemoStarting = 0;
    gUIState.bFadeToBlack = 1;
    gSession.bDemo = 0;
    Gaud_ExitFE();
    GameMode4_ExitFE();
}

// ---- the profile backups in ARAM ---------------------------------------------------------------

// Moves the profile backups (gFEState.p658) out to ARAM (allocated once, kept in gFEBackupAramAddr)
// and frees their main memory; nothing when they are not in main memory (FE_vCloseModule).
void FE_MoveBackupsToARAM(void) {
    if (gFEState.p658 != NULL) {
        gFEBackupSize = FE_BACKUP_SIZE;
        if (gFEBackupAramAddr == 0) {
            gFEBackupAramAddr = GoARAM_Alloc(FE_BACKUP_SIZE);
        }
        GoARAM_WaitTransfer(GoARAM_CopyToAram(gFEState.p658, gFEBackupAramAddr, gFEBackupSize));
        StaticMem_Free(gFEState.p658);
        gFEState.p658 = NULL;
    }
}

// Gives the profile backups (gFEState.p658) main memory again, cleared, and copies them back from
// ARAM if they were there, freeing the ARAM (FE_vInitModule). Nothing when they are already in main
// memory.
void FE_RestoreBackupsFromARAM(void) {
    if (gFEState.p658 == NULL) {
        gFEBackupSize = FE_BACKUP_SIZE;
        gFEState.p658 = StaticMem_Alloc(gFEBackupSize, 2, 32, "FE_Manager.c", 2778);
        memset(gFEState.p658, 0, gFEBackupSize);
        if (gFEBackupAramAddr != 0) {
            GoARAM_WaitTransfer(GoARAM_CopyFromAram(gFEState.p658, gFEBackupAramAddr, gFEBackupSize));
            GoARAM_Free(gFEBackupAramAddr);
            gFEBackupAramAddr = 0;
        }
    }
}

// Whether attribute nAttr is one of the hidden ones (ATTR_AGGRESSION, ATTR_IQ, ATTR_SPEED), which
// FE_CloseManager keeps from the golfer table when it copies a created golfer in.
u8 FE_IsHiddenAttribute(int nAttr) {
    int bHidden = 0;
    if (nAttr == ATTR_AGGRESSION || nAttr == ATTR_IQ || nAttr == ATTR_SPEED) {
        bHidden = 1;
    }
    return bHidden;
}
