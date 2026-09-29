// streammanagerhole.c (our name, after TW06's golf/streaming/streammanagerhole.cpp; the 2003 game
// is C, and this one file holds what TW06/TW07 split into StreamManager, StreamManagerBase and one
// StreamManager<Part>.cpp per part): the stream manager. gStreamManagerLists holds seven file
// lists, one per part of the game: 0 in game (GlbData, GlbChar, the golfers' character and sac
// files), 1 front end (FEnd.gcb, FEChar.gcb), 2 loading screen (Load<n>.gcb), 3 FE character
// (one FEChars file), 4 globals (LoadOnce.gcb), 5 startup (LoadOnce.gcb, startup.gcb), 6 the hole
// (hole.hog). Per part: the stream handlers it registers and unregisters around its load, the
// files it adds, its open / stream-to-the-end / close, and its (empty) file-opened and
// file-closed callbacks. The file ends with Game_GetCurHoleNum (0x80015464); the renderer's state
// cache from 0x80015470 on is Code80015470.c.

#include "ustream.h"
#include "camera.h"
#include "game.h"
#include "golfer.h"
#include "character.h"
#include "frontend/fe.h"
#include "gx.h"
#include "llpict.h"
#include "unsorted/cull.h"

// The file names the parts add to their lists (the Fmt ones go through sprintf; the course
// directory and hole name build the hole's data/<course>/<hole>/hole.hog).
char gszStreamFrontendFile[] = "data/FEnd/FEnd.gcb";
char gszStreamFECharFile[] = "FEChar.gcb";
char gszStreamLoadOnceFile[] = "LoadOnce.gcb";
char gszStreamStartupFile[] = "startup.gcb";
char gszStreamLoadScreenFileFmt[] = "data/Load/Load%d.gcb";
char gszStreamCourseDirFmt[] = "data/%s/";
char gszStreamHoleFileName[] = "/hole.hog";
char gszStreamGlbDataFile[] = "GlbData.gcb";
char gszStreamGlbCharFile[] = "GlbChar.gcb";
char gszStreamCharFileFmt[] = "data/Chars/%02dchar.gcb";
char gszStreamMaleSacFile[] = "malesac.gcb";
char gszStreamFemaleSacFile[] = "femsac.gcb";
char gszStreamCharSacFileFmt[] = "data/CharSac/%02dchrsac.gcb";
char gszStreamFECharFileFmt[] = "data/FEChars/%02dcharfe.gcb";

// Defined here, last address first (CodeWarrior lays out .bss in reverse).
// gStreamManagerCharAdded: 30 flag bytes (of 0x38) that StreamManager_InitModule and
// StreamManagerIngame_SetupFileStream clear and nothing in this build reads (TW07's StreamManager
// has SetCharAdded / IsCharAdded). gStreamManagerLists: the seven file lists (0 in game .. 6 hole,
// see the top of the file) and the stream open on one of them.
u8          gStreamManagerCharAdded[0x38];
StreamLists gStreamManagerLists;

// Every list add, open, close and clear goes through this pointer (FEgolferanim.c and LoadData.c
// too).
StreamLists* gpStreamManagerLists = &gStreamManagerLists;

void StreamManager_InitModule();
void StreamManager_InitOnce(void);
void StreamManager_CloseOnce(void);
void Skalib_Register();
void Skalib_Unregister();
void fn_80010284();
void fn_800102B4();
void Character_RegisterClubStreamClientIG();
void Character_UnregisterClubStreamClient();
void Character_RegisterGolferStreamClientIG();
void Character_UnregisterGolferStreamClient();
void SkeletalObject_RegisterStreamClient();
void SkeletalObject_UnregisterStreamClient();
void MtaLib_Register();
void MtaLib_Unregister();
void DynamicCam_RegisterStreamClients();
void DynamicCam_UnRegisterStreamClients();
void DynObj_RegisterStreamClients();
void DynObj_UnRegisterStreamClients();
void SitDev_vRegisterStreamClients();
void SitDev_vUnregisterStreamClients();
void UI_RegisterStreamClients();
void UI_UnregisterStreamClients();
void MC_RegisterStreamClients();
void MC_UnRegisterStreamClients();
void fn_800A295C();
void fn_800A298C();
void StreamManagerIngame_RegisterStreamClients(void);
void StreamManagerIngame_UnregisterStreamClients(void);
void UI_InitLoadingBar(void);     // uiProcessPolygon.c: set up the loading screen
void UI_DrawLoadingScreenAndProgressBar(int nMode);    // uiProcessPolygon.c: update the loading screen
void UI_FreeLoadingPicture(void);     // uiProcessPolygon.c
void UI_LoadLoadingBarTexture(void);     // uiProcessPolygon.c
void StreamManagerHole_AddStreamFileName(const char* szName, void (*pfnOpened)(void*),
                                         void (*pfnClosed)(void*));
void StreamManagerIngame_StreamFiles(void);
void fn_8000B9E4();
void fn_8000BA14();
void Character_RegisterClubStreamClientFE();
void Character_RegisterGolferStreamClientFE();
void DynamicCam_RegisterStreamClientsFE();
void DynamicCam_UnRegisterStreamClientsFE();
void FE_CharBios_RegisterStreamClients();
void FE_Manager_RegisterStreamClients();
void FE_CharBios_UnRegisterStreamClients();
void FE_lite_vRegisterStreamClients();
void UI_vEATraxRegisterStreamClients();
void UI_vEATraxUnRegisterStreamClients();
void FE_CrAP_RegisterStreamClients();
void FE_CrAP_UnRegisterStreamClients();
void fn_80124A40();
void fn_80124A70();
void StreamManagerFrontend_RegisterStreamClients(void);
void StreamManagerFrontend_UnregisterStreamClients(void);
void startup_RegisterStreamClients();
void startup_UnregisterStreamClients();
void StreamManagerStartup_RegisterStreamClients(void);
void StreamManagerStartup_UnregisterStreamClients(void);
void StreamManagerStartup_StreamFiles(void);
void Golfer_RegisterStatsHandler();
void Golfer_UnregisterStatsHandler();
void Session_RegisterRecordsHandler();
void Session_UnregisterRecordsHandler();
void GM_CourseInfo_RegisterStreamClients();
void GM_CourseInfo_UnRegisterStreamClients();
void EarningsInfo_RegisterStreamClients();
void EarningsInfo_UnRegisterStreamClients();
void PlayNow_RegisterStreamClients();
void PlayNow_UnregisterStreamClients();
void GameModeDriverPGATour_RegisterStreamClients();
void GameModeDriverPGATour_UnregisterStreamClients();
void GameModeDriverRTE_RegisterStreamClients();
void GameModeDriverRTE_UnregisterStreamClients();
void GameMode4_RegisterStreamClients();
void GameMode4_UnregisterStreamClients();
void PGATourSimulation_OpenONCE();
void PGATourSimulation_CloseONCE();
void StreamManagerGlobals_RegisterStreamClients(void);
void StreamManagerGlobals_UnregisterStreamClients(void);
void StreamManagerGlobals_StreamFiles(void);
void Ter_RegisterStreamClients();
void Ter_UnRegisterStreamClients();
void StaticCam_RegisterStreamClients();
void StaticCam_UnRegisterStreamClients();
void fn_8011E468();
void fn_8011E4A4();
void StreamManagerHole_RegisterStreamClients(void);
void StreamManagerHole_UnregisterStreamClients(void);
void StreamManagerHole_InitModule(void);
void StreamManagerIngame_StreamSacFiles(void);
void StreamManagerFEChar_InitModule(void);
void StreamManagerIngame_BeginStreamCallbackIGChar(void* pArg);
void StreamManagerFrontend_BeginStreamCallback(void* pArg);
void StreamManagerLoadScreen_BeginStreamCallback(void* pArg);
void StreamManagerFEChar_BeginStreamCallback(void* pArg);
void StreamManagerGlobals_BeginStreamCallback(void* pArg);
void StreamManagerHole_BeginStreamCallback(void* pArg);
void StreamManagerIngame_EndStreamCallbackIGChar(void* pArg);
void StreamManagerFrontend_EndStreamCallback(void* pArg);
void StreamManagerLoadScreen_EndStreamCallback(void* pArg);
void StreamManagerFEChar_EndStreamCallback(void* pArg);
void StreamManagerFEChar_AddStreamFileName(const char* szName, void (*pfnOpened)(void*),
                                           void (*pfnClosed)(void*));
void StreamManagerLoadScreen_AddStreamFileName(const char* szName, void (*pfnOpened)(void*),
                                               void (*pfnClosed)(void*));
void StreamManagerIngame_AddStreamFileName(const char* szName, void (*pfnOpened)(void*),
                                           void (*pfnClosed)(void*));
u32  Skalib_CurSlot(void);          // skalib.c
int  Skalib_HasOverlays(int nSlot); // skalib.c
void StreamManagerGlobals_EndStreamCallback(void* pArg);
void StreamManagerHole_EndStreamCallback(void* pArg);
void UStream_Close();
s32 Stream_OpenStreamFiles();
void StreamManagerIngame_CloseStreamFiles(void);
void StreamManagerIngame_OpenStreamFiles(void);
void StreamManagerFrontend_CloseStreamFiles(void);
void StreamManagerFrontend_OpenStreamFiles(void);
void StreamManagerLoadScreen_CloseStreamFiles(void);
void StreamManagerLoadScreen_OpenStreamFiles(void);
void StreamManagerStartup_CloseStreamFiles(void);
void StreamManagerStartup_OpenStreamFiles(void);
void StreamManagerGlobals_OpenStreamFiles(void);
void StreamManagerHole_CloseStreamFiles(void);
void StreamManagerHole_OpenStreamFiles(void);
void StreamManagerHole_ClearStreamFileNames(void);
void StreamManagerIngame_ClearStreamFileNames(void);
void StreamManagerFEChar_ClearStreamFileNames(void);

void StreamManagerStartup_AddStreamFileName(const char* szName, void (*pfnOpened)(void*),
                                            void (*pfnClosed)(void*));
void StreamManagerGlobals_AddStreamFileName(const char* szName, void (*pfnOpened)(void*),
                                            void (*pfnClosed)(void*));
void StreamManagerFrontend_AddStreamFileName(const char* szName, void (*pfnOpened)(void*),
                                             void (*pfnClosed)(void*));

// Set up the stream lists once at boot: empty all seven, put the front end's files in the front-end
// list (1), LoadOnce.gcb in the globals list (4) and LoadOnce.gcb + startup.gcb in the startup list
// (5), clear gStreamManagerCharAdded, then run the FE-character and hole managers' (empty) init
// calls. Lists: 0 in game, 1 front end, 2 loading screen, 3 FE character, 4 globals, 5 startup, 6
// hole.
void StreamManager_InitModule(void) {
    int i;

    gpStreamManagerLists->aParams[0].nNumFiles = 0;
    gpStreamManagerLists->aParams[1].nNumFiles = 0;
    gpStreamManagerLists->aParams[2].nNumFiles = 0;
    gpStreamManagerLists->aParams[3].nNumFiles = 0;
    gpStreamManagerLists->aParams[4].nNumFiles = 0;
    gpStreamManagerLists->aParams[5].nNumFiles = 0;
    gpStreamManagerLists->aParams[6].nNumFiles = 0;
    StreamManagerFrontend_AddStreamFileName(gszStreamFrontendFile, StreamManagerFrontend_BeginStreamCallback,
                StreamManagerFrontend_EndStreamCallback);
    StreamManagerFrontend_AddStreamFileName(gszStreamFECharFile, StreamManagerFrontend_BeginStreamCallback,
                StreamManagerFrontend_EndStreamCallback);
    StreamManagerGlobals_AddStreamFileName(gszStreamLoadOnceFile, StreamManagerGlobals_BeginStreamCallback,
                StreamManagerGlobals_EndStreamCallback);
    StreamManagerStartup_AddStreamFileName(gszStreamLoadOnceFile, StreamManagerGlobals_BeginStreamCallback,
                StreamManagerGlobals_EndStreamCallback);
    StreamManagerStartup_AddStreamFileName(gszStreamStartupFile, StreamManagerGlobals_BeginStreamCallback,
                StreamManagerGlobals_EndStreamCallback);
    for (i = 0; i < 30; i++) {
        gStreamManagerCharAdded[i] = 0;
    }
    StreamManagerFEChar_InitModule();
    StreamManagerHole_InitModule();
}

// The stream manager's once-at-boot init (called from the game's init-once list): just
// StreamManager_InitModule.
void StreamManager_InitOnce(void) {
    StreamManager_InitModule();
}

// Add loading-screen file nFile (data/Load/Load<nFile>.gcb) to the loading-screen stream list (2).
// LoadData.c picks nFile for the course, empties list 2 first and streams it right after.
void StreamManager_AddLoadScreenFile(int nFile) {
    char szName[0x40];   // size unknown: the frame allows 0x40..0x48 bytes

    sprintf(szName, gszStreamLoadScreenFileFmt, nFile);
    StreamManagerLoadScreen_AddStreamFileName(szName, StreamManagerLoadScreen_BeginStreamCallback,
                StreamManagerLoadScreen_EndStreamCallback);
}

// The stream manager's shutdown, called from the game's close-once list. Empty in this build.
void StreamManager_CloseOnce(void) {
}

// Register every stream handler the in-game files need (cameras, UI, dynamic objects, clubs,
// animation libraries, golfers, skeletons, memory card, situations). GO_vInitIG calls it around
// StreamManagerIngame_StreamFiles.
void StreamManagerIngame_RegisterStreamClients(void) {
    fn_80010284();
    DynamicCam_RegisterStreamClients();
    UI_RegisterStreamClients();
    DynObj_RegisterStreamClients();
    Character_RegisterClubStreamClientIG();
    Skalib_Register();
    MtaLib_Register();
    Character_RegisterGolferStreamClientIG();
    SkeletalObject_RegisterStreamClient();
    MC_RegisterStreamClients();
    fn_800A295C();
    SitDev_vRegisterStreamClients();
}

// Unregister the handlers StreamManagerIngame_RegisterStreamClients registered.
void StreamManagerIngame_UnregisterStreamClients(void) {
    fn_800102B4();
    DynamicCam_UnRegisterStreamClients();
    UI_UnregisterStreamClients();
    DynObj_UnRegisterStreamClients();
    Character_UnregisterClubStreamClient();
    Skalib_Unregister();
    MtaLib_Unregister();
    Character_UnregisterGolferStreamClient();
    SkeletalObject_UnregisterStreamClient();
    MC_UnRegisterStreamClients();
    fn_800A298C();
    SitDev_vUnregisterStreamClients();
}

// Read the in-game stream list (0) to the end, drawing the loading screen and its progress bar
// while it streams.
void StreamManagerIngame_StreamFiles(void) {
    UI_InitLoadingBar();
    StreamManagerIngame_OpenStreamFiles();
    do {
        UI_DrawLoadingScreenAndProgressBar(0);
    } while ((u8)UStream_Update() != 0);   // fake match: this file tests the result as a byte
    StreamManagerIngame_CloseStreamFiles();
}

// Register every stream handler the front end's files need (FE golfer and club, animation
// libraries, UI, FE cameras, CrAP, character bios, FE manager, EA Trax). Called by GO_vInitFE.
void StreamManagerFrontend_RegisterStreamClients(void) {
    fn_80010284();
    Character_RegisterClubStreamClientFE();
    Skalib_Register();
    MtaLib_Register();
    UI_RegisterStreamClients();
    DynamicCam_RegisterStreamClientsFE();
    FE_lite_vRegisterStreamClients();
    FE_CrAPBall_RegisterStreamClients();
    Character_RegisterGolferStreamClientFE();
    MC_RegisterStreamClients();
    FE_CharBios_RegisterStreamClients();
    FE_Manager_RegisterStreamClients();
    FE_CrAP_RegisterStreamClients();
    fn_80124A40();
    fn_8000B9E4();
    UI_vEATraxRegisterStreamClients();
}

// Unregister the handlers StreamManagerFrontend_RegisterStreamClients registered.
void StreamManagerFrontend_UnregisterStreamClients(void) {
    fn_800102B4();
    Character_UnregisterClubStreamClient();
    Skalib_Unregister();
    MtaLib_Unregister();
    UI_UnregisterStreamClients();
    DynamicCam_UnRegisterStreamClientsFE();
    fn_8000BA14();
    FE_CrAPBall_UnRegisterStreamClients();
    Character_UnregisterGolferStreamClient();
    FE_CharBios_UnRegisterStreamClients();
    FE_CrAP_UnRegisterStreamClients();
    fn_80124A70();
    MC_UnRegisterStreamClients();
    UI_vEATraxUnRegisterStreamClients();
}

// Read the front end's stream list (1) to the end, with the loading screen unless the front end's
// bFirstTime is set (the first time, it streams with no screen).
void StreamManagerFrontend_StreamFiles(void) {
    if (gFEState.bFirstTime == 0) {
        UI_LoadLoadingBarTexture();
        UI_InitLoadingBar();
    }
    StreamManagerFrontend_OpenStreamFiles();
    do {
        if (gFEState.bFirstTime == 0) {
            UI_DrawLoadingScreenAndProgressBar(0);
        }
    } while (UStream_Update() != 0);
    if (gFEState.bFirstTime == 0) {
        UI_DrawLoadingScreenAndProgressBar(1);
    }
    StreamManagerFrontend_CloseStreamFiles();
    if (gFEState.bFirstTime == 0) {
        UI_FreeLoadingPicture();
    }
}

// Read the loading-screen stream list (2) to the end, with nothing drawn (it is the loading
// screen's own picture). Called by LoadData.c.
void StreamManagerLoadScreen_StreamFiles(void) {
    StreamManagerLoadScreen_OpenStreamFiles();
    do {

    } while ((u8)UStream_Update() != 0);   // fake match: this file tests the result as a byte
    StreamManagerLoadScreen_CloseStreamFiles();
}

// Register the stream handlers the startup files need: UI, memory card, startup and the globals'
// (StreamManagerGlobals_RegisterStreamClients).
void StreamManagerStartup_RegisterStreamClients(void) {
    fn_80010284();
    UI_RegisterStreamClients();
    MC_RegisterStreamClients();
    startup_RegisterStreamClients();
    StreamManagerGlobals_RegisterStreamClients();
}

// Unregister the handlers StreamManagerStartup_RegisterStreamClients registered.
void StreamManagerStartup_UnregisterStreamClients(void) {
    fn_800102B4();
    UI_UnregisterStreamClients();
    MC_UnRegisterStreamClients();
    startup_UnregisterStreamClients();
    StreamManagerGlobals_UnregisterStreamClients();
}

// Read the startup stream list (5: LoadOnce.gcb and startup.gcb) to the end, with no loading
// screen.
void StreamManagerStartup_StreamFiles(void) {
    StreamManagerStartup_OpenStreamFiles();
    do {

    } while ((u8)UStream_Update() != 0);   // fake match: this file tests the result as a byte
    StreamManagerStartup_CloseStreamFiles();
}

// Register the stream handlers of the global data (golfer stats, records, EA Trax, course info,
// Play Now, earnings, game modes, PGA Tour season). Called by
// StreamManagerStartup_RegisterStreamClients and by gomainloop around
// StreamManagerGlobals_StreamFiles.
void StreamManagerGlobals_RegisterStreamClients(void) {
    Golfer_RegisterStatsHandler();
    Session_RegisterRecordsHandler();
    UI_vEATraxRegisterStreamClients();
    GM_CourseInfo_RegisterStreamClients();
    PlayNow_RegisterStreamClients();
    EarningsInfo_RegisterStreamClients();
    GameMode4_RegisterStreamClients();
    PGATourSimulation_OpenONCE();
    GameModeDriverPGATour_RegisterStreamClients();
    GameModeDriverRTE_RegisterStreamClients();
}

// Unregister the handlers StreamManagerGlobals_RegisterStreamClients registered.
void StreamManagerGlobals_UnregisterStreamClients(void) {
    Golfer_UnregisterStatsHandler();
    Session_UnregisterRecordsHandler();
    UI_vEATraxUnRegisterStreamClients();
    GM_CourseInfo_UnRegisterStreamClients();
    PlayNow_UnregisterStreamClients();
    EarningsInfo_UnRegisterStreamClients();
    GameMode4_UnregisterStreamClients();
    PGATourSimulation_CloseONCE();
    GameModeDriverPGATour_UnregisterStreamClients();
    GameModeDriverRTE_UnregisterStreamClients();
}

// Read the globals stream list (4: LoadOnce.gcb) to the end, with no loading screen. It closes
// through StreamManagerFrontend_CloseStreamFiles (every close is the same).
void StreamManagerGlobals_StreamFiles(void) {
    StreamManagerGlobals_OpenStreamFiles();
    do {

    } while ((u8)UStream_Update() != 0);   // fake match: this file tests the result as a byte
    StreamManagerFrontend_CloseStreamFiles();
}

// Register the stream handlers a hole file needs (terrain, dynamic objects, static cameras).
void StreamManagerHole_RegisterStreamClients(void) {
    fn_80010284();
    Ter_RegisterStreamClients();
    DynObj_RegisterStreamClients();
    StaticCam_RegisterStreamClients();
    fn_8011E468();
}

// Unregister the handlers StreamManagerHole_RegisterStreamClients registered.
void StreamManagerHole_UnregisterStreamClients(void) {
    fn_8011E4A4();
    DynObj_UnRegisterStreamClients();
    Ter_UnRegisterStreamClients();
    fn_800102B4();
    StaticCam_UnRegisterStreamClients();
}

// Stream the current hole's file (data/<course>/<hole>/hole.hog, or the session's override) as
// list 6, updating the loading screen until it is all read.
void StreamManagerHole_StreamFiles(void) {
    char szPath[0x80];  // size unknown: the frame allows up to 0x84 bytes
    char* szCourse;
    char* szHole;

    UI_InitLoadingBar();
    StreamManagerHole_ClearStreamFileNames();
    if (gSession.n5B34 != 0) {
        StreamManagerHole_AddStreamFileName(gSession.p5B30, StreamManagerHole_BeginStreamCallback,
                    StreamManagerHole_EndStreamCallback);
    } else {
        szCourse = GM_GetCourseName();
        szHole = GameManager_GetHoleName(Game_GetCurHoleNum());
        sprintf(szPath, gszStreamCourseDirFmt, szCourse);
        strcat(szPath, szHole);
        strcat(szPath, gszStreamHoleFileName);
        StreamManagerHole_AddStreamFileName(szPath, StreamManagerHole_BeginStreamCallback,
                                            StreamManagerHole_EndStreamCallback);
    }
    StreamManagerHole_OpenStreamFiles();
    do {
        UI_DrawLoadingScreenAndProgressBar(0);
    } while (UStream_Update() != 0);
    UI_DrawLoadingScreenAndProgressBar(1);
    StreamManagerHole_CloseStreamFiles();
    UI_FreeLoadingPicture();
}

// Called by StreamManager_InitModule; empty in this build. Named for its place beside the hole
// manager's functions.
void StreamManagerHole_InitModule(void) {
}

// Refill the in-game stream list (0) with GlbData.gcb, GlbChar.gcb and every player's golfer's
// character file (data/Chars/<model + 1>char.gcb), clear gSacReloading and gStreamManagerCharAdded.
// Called by GO_vInitIG.
void StreamManagerIngame_SetupFileStream(void) {
    char szName[0x80];  // size unknown: the frame allows up to 0x84 bytes
    int nPlayer;
    int i;

    gSacReloading = 0;
    StreamManagerIngame_ClearStreamFileNames();
    StreamManagerIngame_AddStreamFileName(gszStreamGlbDataFile, StreamManagerGlobals_BeginStreamCallback,
                StreamManagerGlobals_EndStreamCallback);
    StreamManagerIngame_AddStreamFileName(gszStreamGlbCharFile, StreamManagerIngame_BeginStreamCallbackIGChar,
                StreamManagerIngame_EndStreamCallbackIGChar);
    for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
        sprintf(szName, gszStreamCharFileFmt, Character_GetGolferModelID(nPlayer) + 1);
        StreamManagerIngame_AddStreamFileName(szName, StreamManagerIngame_BeginStreamCallbackIGChar,
                    StreamManagerIngame_EndStreamCallbackIGChar);
    }
    for (i = 0; i < 30; i++) {
        gStreamManagerCharAdded[i] = 0;
    }
}

// Refill the in-game stream list (0) with the sac files: malesac.gcb if animation slot 0 has
// overlays, femsac.gcb if slot 1 has, and every player's golfer's CharSac file. Called by
// Character_LoadSacFiles.
void StreamManagerIngame_SetupSacFiles(void) {
    char szName[0x80];  // size unknown: the frame allows up to 0x88 bytes
    int nPlayer;

    StreamManagerIngame_ClearStreamFileNames();
    if (Skalib_HasOverlays(0) != 0) {
        StreamManagerIngame_AddStreamFileName(gszStreamMaleSacFile, StreamManagerGlobals_BeginStreamCallback,
                    StreamManagerGlobals_EndStreamCallback);
    }
    if (Skalib_HasOverlays(1) != 0) {
        StreamManagerIngame_AddStreamFileName(gszStreamFemaleSacFile,
                                              StreamManagerGlobals_BeginStreamCallback,
                    StreamManagerGlobals_EndStreamCallback);
    }
    for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
        sprintf(szName, gszStreamCharSacFileFmt, Character_GetGolferModelID(nPlayer) + 1);
        StreamManagerIngame_AddStreamFileName(szName, StreamManagerIngame_BeginStreamCallbackIGChar,
                    StreamManagerIngame_EndStreamCallbackIGChar);
    }
}

// Refill the in-game stream list (0) with the current animation slot's sac file (malesac.gcb for
// slot 0, else femsac.gcb) and the CharSac file of every player whose golfer has an overlay in that
// slot. Called by Character_ReloadSacFiles.
void StreamManagerIngame_SetupCurSlotSacFiles(void) {
    char szName[0x80];  // size unknown: the frame allows up to 0x8C bytes
    u32 nSlot;
    int nPlayer;
    LibSlot* pSlot;
    int nModel;
    int i;

    StreamManagerIngame_ClearStreamFileNames();
    nSlot = Skalib_CurSlot();
    if (nSlot == 0) {
        StreamManagerIngame_AddStreamFileName(gszStreamMaleSacFile, StreamManagerGlobals_BeginStreamCallback,
                    StreamManagerGlobals_EndStreamCallback);
    } else {
        StreamManagerIngame_AddStreamFileName(gszStreamFemaleSacFile,
                                              StreamManagerGlobals_BeginStreamCallback,
                    StreamManagerGlobals_EndStreamCallback);
    }
    pSlot = &gLibSlots[nSlot];
    for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
        nModel = Character_GetGolferModelID(nPlayer);
        for (i = 0; i < pSlot->nOverlays; i++) {
            if (pSlot->overlays[i].nGolferId == nModel) {
                break;
            }
        }
        if (i < pSlot->nOverlays) {
            sprintf(szName, gszStreamCharSacFileFmt, nModel + 1);
            StreamManagerIngame_AddStreamFileName(szName, StreamManagerIngame_BeginStreamCallbackIGChar,
                        StreamManagerIngame_EndStreamCallbackIGChar);
        }
    }
}

// Read the in-game stream list (0) to the end, drawing the loading screen, which the caller has
// already set up (StreamManagerIngame_StreamFiles also inits it). Called by Character_LoadSacFiles
// and Character_ReloadSacFiles.
void StreamManagerIngame_StreamSacFiles(void) {
    StreamManagerIngame_OpenStreamFiles();
    do {
        UI_DrawLoadingScreenAndProgressBar(0);
    } while ((u8)UStream_Update() != 0);   // fake match: this file tests the result as a byte
    StreamManagerIngame_CloseStreamFiles();
}

// Called by StreamManager_InitModule; empty in this build. Named for its place beside the FE
// character manager's functions.
void StreamManagerFEChar_InitModule(void) {
}

// Make the FE character stream list (3) hold only character nChar's front-end file
// (data/FEChars/<nChar + 1>charfe.gcb), and mark the CrAP golfer as not yet streamed
// (nStreamedId = -1). Called by FE_StreamFunc_SkinInit.
// port: FEgolferanim.c passes a second argument this ignores (nUnused).
void StreamManagerFEChar_SetupFileStream(s32 nChar, s32 nUnused) {
    char szName[0x100];  // size unknown: the frame allows up to 0x100 bytes

    StreamManagerFEChar_ClearStreamFileNames();
    sprintf(szName, gszStreamFECharFileFmt, nChar + 1);
    gpCrAPState->pB8->nStreamedId = -1;
    StreamManagerFEChar_AddStreamFileName(szName, StreamManagerFEChar_BeginStreamCallback,
                                          StreamManagerFEChar_EndStreamCallback);
}

// The stream opened callback of the in-game character files (GlbChar.gcb, data/Chars, data/CharSac)
// (UStream calls it when the file is opened). Empty in this build.
void StreamManagerIngame_BeginStreamCallbackIGChar(void* pArg) {
}

// The stream opened callback of the front end's files (FEnd.gcb, FEChar.gcb) (UStream calls it when
// the file is opened). Empty in this build.
void StreamManagerFrontend_BeginStreamCallback(void* pArg) {
}

// The stream opened callback of the loading screen's file (Load<n>.gcb) (UStream calls it when the
// file is opened). Empty in this build.
void StreamManagerLoadScreen_BeginStreamCallback(void* pArg) {
}

// The stream opened callback of the FE character file (data/FEChars) (UStream calls it when the
// file is opened). Empty in this build.
void StreamManagerFEChar_BeginStreamCallback(void* pArg) {
}

// The stream opened callback of the global files (GlbData.gcb, LoadOnce.gcb, startup.gcb,
// malesac/femsac) (UStream calls it when the file is opened). Empty in this build.
void StreamManagerGlobals_BeginStreamCallback(void* pArg) {
}

// The stream opened callback of the hole file (UStream calls it when the file is opened). Empty in
// this build.
void StreamManagerHole_BeginStreamCallback(void* pArg) {
}

// The stream closed callback of the in-game character files (GlbChar.gcb, data/Chars, data/CharSac)
// (UStream calls it when the file is closed). Empty in this build.
void StreamManagerIngame_EndStreamCallbackIGChar(void* pArg) {
}

// The stream closed callback of the front end's files (FEnd.gcb, FEChar.gcb) (UStream calls it when
// the file is closed). Empty in this build.
void StreamManagerFrontend_EndStreamCallback(void* pArg) {
}

// The stream closed callback of the loading screen's file (Load<n>.gcb) (UStream calls it when the
// file is closed). Empty in this build.
void StreamManagerLoadScreen_EndStreamCallback(void* pArg) {
}

// The stream closed callback of the FE character file (data/FEChars) (UStream calls it when the
// file is closed). Empty in this build.
void StreamManagerFEChar_EndStreamCallback(void* pArg) {
}

// The stream closed callback of the global files (GlbData.gcb, LoadOnce.gcb, startup.gcb,
// malesac/femsac) (UStream calls it when the file is closed). Empty in this build.
void StreamManagerGlobals_EndStreamCallback(void* pArg) {
}

// The stream closed callback of the hole file (UStream calls it when the file is closed). Empty in
// this build.
void StreamManagerHole_EndStreamCallback(void* pArg) {
}

// Add file szName to the startup stream list (5), with the calls UStream makes when it is opened
// and closed. No bounds check: the list holds 8 files.
void StreamManagerStartup_AddStreamFileName(const char* szName, void (*pfnOpened)(void*),
                                            void (*pfnClosed)(void*)) {
    strcpy(gpStreamManagerLists->aParams[5].aszName[gpStreamManagerLists->aParams[5].nNumFiles], szName);
    gpStreamManagerLists->aParams[5].apfnOpened[gpStreamManagerLists->aParams[5].nNumFiles] = pfnOpened;
    gpStreamManagerLists->aParams[5].apfnClosed[gpStreamManagerLists->aParams[5].nNumFiles] = pfnClosed;
    gpStreamManagerLists->aParams[5].nNumFiles++;
}

// Add file szName to the globals stream list (4), with the calls UStream makes when it is opened
// and closed. No bounds check: the list holds 8 files.
void StreamManagerGlobals_AddStreamFileName(const char* szName, void (*pfnOpened)(void*),
                                            void (*pfnClosed)(void*)) {
    strcpy(gpStreamManagerLists->aParams[4].aszName[gpStreamManagerLists->aParams[4].nNumFiles], szName);
    gpStreamManagerLists->aParams[4].apfnOpened[gpStreamManagerLists->aParams[4].nNumFiles] = pfnOpened;
    gpStreamManagerLists->aParams[4].apfnClosed[gpStreamManagerLists->aParams[4].nNumFiles] = pfnClosed;
    gpStreamManagerLists->aParams[4].nNumFiles++;
}

// Add file szName to the front end's stream list (1), with the calls UStream makes when it is
// opened and closed. No bounds check: the list holds 8 files.
void StreamManagerFrontend_AddStreamFileName(const char* szName, void (*pfnOpened)(void*),
                                             void (*pfnClosed)(void*)) {
    strcpy(gpStreamManagerLists->aParams[1].aszName[gpStreamManagerLists->aParams[1].nNumFiles], szName);
    gpStreamManagerLists->aParams[1].apfnOpened[gpStreamManagerLists->aParams[1].nNumFiles] = pfnOpened;
    gpStreamManagerLists->aParams[1].apfnClosed[gpStreamManagerLists->aParams[1].nNumFiles] = pfnClosed;
    gpStreamManagerLists->aParams[1].nNumFiles++;
}

// Add file szName to the loading-screen stream list (2), with the calls UStream makes when it is
// opened and closed. No bounds check: the list holds 8 files.
void StreamManagerLoadScreen_AddStreamFileName(const char* szName, void (*pfnOpened)(void*),
                                               void (*pfnClosed)(void*)) {
    strcpy(gpStreamManagerLists->aParams[2].aszName[gpStreamManagerLists->aParams[2].nNumFiles], szName);
    gpStreamManagerLists->aParams[2].apfnOpened[gpStreamManagerLists->aParams[2].nNumFiles] = pfnOpened;
    gpStreamManagerLists->aParams[2].apfnClosed[gpStreamManagerLists->aParams[2].nNumFiles] = pfnClosed;
    gpStreamManagerLists->aParams[2].nNumFiles++;
}

// Close the open stream (gpStreamManagerLists->nStream) after the in-game list (0) has streamed.
void StreamManagerIngame_CloseStreamFiles(void) {
    UStream_Close(gpStreamManagerLists->nStream);
}

// Start streaming the in-game list (0); the stream is kept in gpStreamManagerLists->nStream.
void StreamManagerIngame_OpenStreamFiles(void) {
    s32 t0;
    t0 = Stream_OpenStreamFiles(&gpStreamManagerLists->aParams[0]);
    gpStreamManagerLists->nStream = t0;
}

// Close the open stream (gpStreamManagerLists->nStream) after the front end's list (1) has
// streamed. StreamManagerGlobals_StreamFiles closes the globals list (4) with it too.
void StreamManagerFrontend_CloseStreamFiles(void) {
    UStream_Close(gpStreamManagerLists->nStream);
}

// Start streaming the front end's list (1); the stream is kept in gpStreamManagerLists->nStream.
void StreamManagerFrontend_OpenStreamFiles(void) {
    s32 t0;
    t0 = Stream_OpenStreamFiles(&gpStreamManagerLists->aParams[1]);
    gpStreamManagerLists->nStream = t0;
}

// Close the open stream (gpStreamManagerLists->nStream) after the loading-screen list (2) has
// streamed.
void StreamManagerLoadScreen_CloseStreamFiles(void) {
    UStream_Close(gpStreamManagerLists->nStream);
}

// Start streaming the loading-screen list (2); the stream is kept in gpStreamManagerLists->nStream.
void StreamManagerLoadScreen_OpenStreamFiles(void) {
    s32 t0;
    t0 = Stream_OpenStreamFiles(&gpStreamManagerLists->aParams[2]);
    gpStreamManagerLists->nStream = t0;
}

// Close the open stream (gpStreamManagerLists->nStream) after the startup list (5) has streamed.
void StreamManagerStartup_CloseStreamFiles(void) {
    UStream_Close(gpStreamManagerLists->nStream);
}

// Start streaming the startup list (5); the stream is kept in gpStreamManagerLists->nStream.
void StreamManagerStartup_OpenStreamFiles(void) {
    s32 t0;
    t0 = Stream_OpenStreamFiles(&gpStreamManagerLists->aParams[5]);
    gpStreamManagerLists->nStream = t0;
}

// Start streaming the globals list (4); the stream is kept in gpStreamManagerLists->nStream.
void StreamManagerGlobals_OpenStreamFiles(void) {
    s32 t0;
    t0 = Stream_OpenStreamFiles(&gpStreamManagerLists->aParams[4]);
    gpStreamManagerLists->nStream = t0;
}

// Close the open stream (gpStreamManagerLists->nStream) after the hole list (6) has streamed.
void StreamManagerHole_CloseStreamFiles(void) {
    UStream_Close(gpStreamManagerLists->nStream);
}

// Start streaming the hole list (6); the stream is kept in gpStreamManagerLists->nStream.
void StreamManagerHole_OpenStreamFiles(void) {
    s32 t0;
    t0 = Stream_OpenStreamFiles(&gpStreamManagerLists->aParams[6]);
    gpStreamManagerLists->nStream = t0;
}

// Add file szName to the hole stream list (6), with the calls UStream makes when it is opened and
// closed. No bounds check: the list holds 8 files.
void StreamManagerHole_AddStreamFileName(const char* szName, void (*pfnOpened)(void*),
                                         void (*pfnClosed)(void*)) {
    strcpy(gpStreamManagerLists->aParams[6].aszName[gpStreamManagerLists->aParams[6].nNumFiles], szName);
    gpStreamManagerLists->aParams[6].apfnOpened[gpStreamManagerLists->aParams[6].nNumFiles] = pfnOpened;
    gpStreamManagerLists->aParams[6].apfnClosed[gpStreamManagerLists->aParams[6].nNumFiles] = pfnClosed;
    gpStreamManagerLists->aParams[6].nNumFiles++;
}

void StreamManagerHole_ClearStreamFileNames(void) {
    gpStreamManagerLists->aParams[6].nNumFiles = 0;
}

// Add file szName to the in-game stream list (0), with the calls UStream makes when it is opened
// and closed. No bounds check: the list holds 8 files.
void StreamManagerIngame_AddStreamFileName(const char* szName, void (*pfnOpened)(void*),
                                           void (*pfnClosed)(void*)) {
    strcpy(gpStreamManagerLists->aParams[0].aszName[gpStreamManagerLists->aParams[0].nNumFiles], szName);
    gpStreamManagerLists->aParams[0].apfnOpened[gpStreamManagerLists->aParams[0].nNumFiles] = pfnOpened;
    gpStreamManagerLists->aParams[0].apfnClosed[gpStreamManagerLists->aParams[0].nNumFiles] = pfnClosed;
    gpStreamManagerLists->aParams[0].nNumFiles++;
}

void StreamManagerIngame_ClearStreamFileNames(void) {
    gpStreamManagerLists->aParams[0].nNumFiles = 0;
}

// Add file szName to the FE character stream list (3), with the calls UStream makes when it is
// opened and closed. No bounds check: the list holds 8 files.
void StreamManagerFEChar_AddStreamFileName(const char* szName, void (*pfnOpened)(void*),
                                           void (*pfnClosed)(void*)) {
    strcpy(gpStreamManagerLists->aParams[3].aszName[gpStreamManagerLists->aParams[3].nNumFiles], szName);
    gpStreamManagerLists->aParams[3].apfnOpened[gpStreamManagerLists->aParams[3].nNumFiles] = pfnOpened;
    gpStreamManagerLists->aParams[3].apfnClosed[gpStreamManagerLists->aParams[3].nNumFiles] = pfnClosed;
    gpStreamManagerLists->aParams[3].nNumFiles++;
}

void StreamManagerFEChar_ClearStreamFileNames(void) {
    gpStreamManagerLists->aParams[3].nNumFiles = 0;
}

// The current hole's number on its course (Game_CurHoleIndex gives its 0..17 place in the round).
int Game_GetCurHoleNum(void) {
    return gpGame->nCurHoleNum;
}
