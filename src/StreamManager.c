// StreamManager.c: the stream manager. It keeps a list of files for each kind of stream (the round,
// the front end, a loading screen, the menu golfer, the global data, the start-up, a hole), registers
// the chunk handlers each kind needs, and streams a list to its end while the loading screen runs.

#include "ustream.h"
#include "camera.h"
#include "game.h"
#include "golfer.h"
#include "character.h"
#include "dynobj.h"
#include "sitdev.h"
#include "psmgr.h"
#include "terrain.h"
#include "trax.h"
#include "grassshader.h"
#include "frontend/fe.h"
#include "game/frontend.h"
#include "game/earnings.h"
#include "game/modes/pgatour.h"
#include "game/modes/rte.h"
#include "game/modes/ladder.h"
#include "game/modes/pgatoursim.h"
#include "core/memcard.h"
#include "core/startup.h"

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

u8          gStreamManagerCharAdded[0x38];
StreamLists gStreamManagerLists;

StreamLists* gpStreamManagerLists = &gStreamManagerLists;

static void StreamManager_InitModule(void);
static void StreamManagerHole_InitModule(void);
static void StreamManagerFEChar_InitModule(void);
static void StreamManagerIngame_BeginStreamCallbackIGChar(void* pData);
static void StreamManagerFrontend_BeginStreamCallback(void* pData);
static void StreamManagerLoadScreen_BeginStreamCallback(void* pData);
static void StreamManagerFEChar_BeginStreamCallback(void* pData);
static void StreamManagerGlobals_BeginStreamCallback(void* pData);
static void StreamManagerHole_BeginStreamCallback(void* pData);
static void StreamManagerIngame_EndStreamCallbackIGChar(void* pData);
static void StreamManagerFrontend_EndStreamCallback(void* pData);
static void StreamManagerLoadScreen_EndStreamCallback(void* pData);
static void StreamManagerFEChar_EndStreamCallback(void* pData);
static void StreamManagerGlobals_EndStreamCallback(void* pData);
static void StreamManagerHole_EndStreamCallback(void* pData);
static void StreamManagerStartup_AddStreamFileName(const char* fileName, StreamBeginCallback callbackBegin,
                                                   StreamEndCallback callbackEnd);
static void StreamManagerGlobals_AddStreamFileName(const char* fileName, StreamBeginCallback callbackBegin,
                                                   StreamEndCallback callbackEnd);
static void StreamManagerFrontend_AddStreamFileName(const char* fileName, StreamBeginCallback callbackBegin,
                                                    StreamEndCallback callbackEnd);
static void StreamManagerLoadScreen_AddStreamFileName(const char* fileName,
                                                      StreamBeginCallback callbackBegin,
                                                      StreamEndCallback callbackEnd);
static void StreamManagerIngame_CloseStreamFiles(void);
static void StreamManagerIngame_OpenStreamFiles(void);
static void StreamManagerFrontend_CloseStreamFiles(void);
static void StreamManagerFrontend_OpenStreamFiles(void);
static void StreamManagerLoadScreen_CloseStreamFiles(void);
static void StreamManagerLoadScreen_OpenStreamFiles(void);
static void StreamManagerStartup_CloseStreamFiles(void);
static void StreamManagerStartup_OpenStreamFiles(void);
static void StreamManagerGlobals_OpenStreamFiles(void);
static void StreamManagerHole_CloseStreamFiles(void);
static void StreamManagerHole_OpenStreamFiles(void);
static void StreamManagerHole_AddStreamFileName(const char* fileName, StreamBeginCallback callbackBegin,
                                                StreamEndCallback callbackEnd);
static void StreamManagerHole_ClearStreamFileNames(void);
static void StreamManagerIngame_AddStreamFileName(const char* fileName, StreamBeginCallback callbackBegin,
                                                  StreamEndCallback callbackEnd);
static void StreamManagerIngame_ClearStreamFileNames(void);
static void StreamManagerFEChar_AddStreamFileName(const char* fileName, StreamBeginCallback callbackBegin,
                                                  StreamEndCallback callbackEnd);
static void StreamManagerFEChar_ClearStreamFileNames(void);

// Empties every list, then fills the ones that never change: the front end's (FEnd.gcb,
// FEChar.gcb), the global data's (LoadOnce.gcb) and the start-up's (LoadOnce.gcb, startup.gcb).
static void StreamManager_InitModule(void) {
    int i;

    STREAM_LIST(STREAM_FILE_INGAME).nNumFiles = 0;
    STREAM_LIST(STREAM_FILE_FRONTEND).nNumFiles = 0;
    STREAM_LIST(STREAM_FILE_LOADSCREEN).nNumFiles = 0;
    STREAM_LIST(STREAM_FILE_FECHAR).nNumFiles = 0;
    STREAM_LIST(STREAM_FILE_GLOBALS).nNumFiles = 0;
    STREAM_LIST(STREAM_FILE_STARTUP).nNumFiles = 0;
    STREAM_LIST(STREAM_FILE_HOLE).nNumFiles = 0;
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
    for (i = 0; i < STREAM_NUM_CHAR_ADDED; i++) {
        gStreamManagerCharAdded[i] = 0;
    }
    StreamManagerFEChar_InitModule();
    StreamManagerHole_InitModule();
}

// Run once at boot.
void StreamManager_InitOnce(void) {
    StreamManager_InitModule();
}

// Adds loading-screen file nFile (data/Load/Load<nFile>.gcb) to the loading screen's list.
// FEtm_StreamLoadScreen (FETextureMgr.c) picks nFile for the game mode or course, streams the list
// right after and then empties it.
void StreamManager_AddLoadScreenFile(int nFile) {
    char szName[64];

    sprintf(szName, gszStreamLoadScreenFileFmt, nFile);
    StreamManagerLoadScreen_AddStreamFileName(szName, StreamManagerLoadScreen_BeginStreamCallback,
                                              StreamManagerLoadScreen_EndStreamCallback);
}

// Run once at shutdown; empty in this build.
void StreamManager_CloseOnce(void) {
}

// The chunk handlers of the round's files: texture groups, cameras, the UI, dynamic objects, clubs,
// the animation libraries, golfers, skeletons, the memory card, the 'sfxd' names and the
// situations. GO_vInitIG registers them around the round's list.
void StreamManagerIngame_RegisterStreamClients(void) {
    TG_vRegisterStreamClients();
    DynamicCam_RegisterStreamClients();
    UI_RegisterStreamClients();
    DynObj_RegisterStreamClients();
    Character_RegisterClubStreamClientIG();
    Skalib_Register();
    MtaLib_Register();
    Character_RegisterGolferStreamClientIG();
    SkeletalObject_RegisterStreamClient();
    MC_RegisterStreamClients();
    PsMgr_RegisterStreamClients();
    SitDev_vRegisterStreamClients();
}

void StreamManagerIngame_UnregisterStreamClients(void) {
    TG_vUnregisterStreamClients();
    DynamicCam_UnRegisterStreamClients();
    UI_UnregisterStreamClients();
    DynObj_UnRegisterStreamClients();
    Character_UnregisterClubStreamClient();
    Skalib_Unregister();
    MtaLib_Unregister();
    Character_UnregisterGolferStreamClient();
    SkeletalObject_UnregisterStreamClient();
    MC_UnRegisterStreamClients();
    PsMgr_UnregisterStreamClients();
    SitDev_vUnregisterStreamClients();
}

// Streams the round's list (set up by StreamManagerIngame_SetupFileStream) to its end, with the
// loading screen.
void StreamManagerIngame_StreamFiles(void) {
    UI_InitLoadingBar();
    StreamManagerIngame_OpenStreamFiles();
    do {
        UI_DrawLoadingScreenAndProgressBar(UI_LOADSCREEN_STEP);
    } while (Stream_Progress() != 0);
    StreamManagerIngame_CloseStreamFiles();
}

// The chunk handlers of the front end's files: texture groups, the menu golfer's clubs, the
// animation libraries, the UI, the front end's cameras and lighting, the CrAP ball, the menu golfer,
// the memory card, the golfers' bios, the front end manager, the CrAP database, the Bio icon, the
// 'txf2' texture banks and EA Trax. GO_vInitFE registers them around the front end's list.
void StreamManagerFrontend_RegisterStreamClients(void) {
    TG_vRegisterStreamClients();
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
    EASBio_RegisterStreamClients();
    FEtm_vRegisterStreamClients();
    UI_vEATraxRegisterStreamClients();
}

// Unregisters the front end's handlers, all but the lighting's and the front end manager's (which
// registers none).
void StreamManagerFrontend_UnregisterStreamClients(void) {
    TG_vUnregisterStreamClients();
    Character_UnregisterClubStreamClient();
    Skalib_Unregister();
    MtaLib_Unregister();
    UI_UnregisterStreamClients();
    DynamicCam_UnRegisterStreamClientsFE();
    FEtm_vUnregisterStreamClients();
    FE_CrAPBall_UnRegisterStreamClients();
    Character_UnregisterGolferStreamClient();
    FE_CharBios_UnRegisterStreamClients();
    FE_CrAP_UnRegisterStreamClients();
    EASBio_UnregisterStreamClients();
    MC_UnRegisterStreamClients();
    UI_vEATraxUnRegisterStreamClients();
}

// Streams the front end's list to its end, with the loading screen except the first time the front
// end starts (gFEState.bFirstTime).
void StreamManagerFrontend_StreamFiles(void) {
    if (gFEState.bFirstTime == 0) {
        UI_LoadLoadingBarTexture();
        UI_InitLoadingBar();
    }
    StreamManagerFrontend_OpenStreamFiles();
    do {
        if (gFEState.bFirstTime == 0) {
            UI_DrawLoadingScreenAndProgressBar(UI_LOADSCREEN_STEP);
        }
    } while (Stream_Progress() != 0);
    if (gFEState.bFirstTime == 0) {
        UI_DrawLoadingScreenAndProgressBar(UI_LOADSCREEN_DONE);
    }
    StreamManagerFrontend_CloseStreamFiles();
    if (gFEState.bFirstTime == 0) {
        UI_FreeLoadingPicture();
    }
}

// Streams the loading screen's list to its end, drawing nothing: it is the loading screen's own
// picture.
void StreamManagerLoadScreen_StreamFiles(void) {
    StreamManagerLoadScreen_OpenStreamFiles();
    do {

    } while (Stream_Progress() != 0);
    StreamManagerLoadScreen_CloseStreamFiles();
}

// The chunk handlers of the start-up files: texture groups, the UI, the memory card, the start-up's
// own and the global data's.
void StreamManagerStartup_RegisterStreamClients(void) {
    TG_vRegisterStreamClients();
    UI_RegisterStreamClients();
    MC_RegisterStreamClients();
    startup_RegisterStreamClients();
    StreamManagerGlobals_RegisterStreamClients();
}

void StreamManagerStartup_UnregisterStreamClients(void) {
    TG_vUnregisterStreamClients();
    UI_UnregisterStreamClients();
    MC_UnRegisterStreamClients();
    startup_UnregisterStreamClients();
    StreamManagerGlobals_UnregisterStreamClients();
}

// Streams the start-up's list to its end, with no loading screen.
void StreamManagerStartup_StreamFiles(void) {
    StreamManagerStartup_OpenStreamFiles();
    do {

    } while (Stream_Progress() != 0);
    StreamManagerStartup_CloseStreamFiles();
}

// The chunk handlers of the global data: the golfers' stats, the records, EA Trax, the course
// table, Play Now's challenges, the prize table, the ladder's events, the PGA Tour season's and
// the real-time events' data.
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

// Streams the global data's list to its end, with no loading screen. Every list's close is the
// same, and this one closes through the front end's.
void StreamManagerGlobals_StreamFiles(void) {
    StreamManagerGlobals_OpenStreamFiles();
    do {

    } while (Stream_Progress() != 0);
    StreamManagerFrontend_CloseStreamFiles();
}

// The chunk handlers of a hole's file: texture groups, the terrain, dynamic objects, the fixed
// cameras and the grass.
void StreamManagerHole_RegisterStreamClients(void) {
    TG_vRegisterStreamClients();
    Ter_vRegisterStreamClients();
    DynObj_RegisterStreamClients();
    StaticCam_RegisterStreamClients();
    Grass_RegisterStreamClients();
}

void StreamManagerHole_UnregisterStreamClients(void) {
    Grass_UnRegisterStreamClients();
    DynObj_UnRegisterStreamClients();
    Ter_vUnregisterStreamClients();
    TG_vUnregisterStreamClients();
    StaticCam_UnRegisterStreamClients();
}

// Makes the hole's list the current hole's file, data/<course>/<hole>/hole.hog (or the session's
// szHoleFileOverride), and streams it to its end with the loading screen.
void StreamManagerHole_StreamFiles(void) {
    char temp[128];
    const char* course;
    const char* holetxt;

    UI_InitLoadingBar();
    StreamManagerHole_ClearStreamFileNames();
    if (gSession.bHoleFileOverride != 0) {
        StreamManagerHole_AddStreamFileName(gSession.szHoleFileOverride,
                                            StreamManagerHole_BeginStreamCallback,
                                            StreamManagerHole_EndStreamCallback);
    } else {
        course = GM_GetCourseName();
        holetxt = GameManager_GetHoleName(Game_GetCurHoleNum());
        sprintf(temp, gszStreamCourseDirFmt, course);
        strcat(temp, holetxt);
        strcat(temp, gszStreamHoleFileName);
        StreamManagerHole_AddStreamFileName(temp, StreamManagerHole_BeginStreamCallback,
                                            StreamManagerHole_EndStreamCallback);
    }
    StreamManagerHole_OpenStreamFiles();
    do {
        UI_DrawLoadingScreenAndProgressBar(UI_LOADSCREEN_STEP);
    } while (Stream_Progress() != 0);
    UI_DrawLoadingScreenAndProgressBar(UI_LOADSCREEN_DONE);
    StreamManagerHole_CloseStreamFiles();
    UI_FreeLoadingPicture();
}

// Empty in this build.
static void StreamManagerHole_InitModule(void) {
}

// Makes the round's list GlbData.gcb, GlbChar.gcb and each player's golfer's character file
// (data/Chars/<model id + 1>char.gcb). GO_vInitIG calls it before StreamManagerIngame_StreamFiles.
void StreamManagerIngame_SetupFileStream(void) {
    char newCharBuff[128];
    int nPlayer;
    int i;

    gSacReloading = 0;
    StreamManagerIngame_ClearStreamFileNames();
    StreamManagerIngame_AddStreamFileName(gszStreamGlbDataFile, StreamManagerGlobals_BeginStreamCallback,
                                          StreamManagerGlobals_EndStreamCallback);
    StreamManagerIngame_AddStreamFileName(gszStreamGlbCharFile, StreamManagerIngame_BeginStreamCallbackIGChar,
                                          StreamManagerIngame_EndStreamCallbackIGChar);
    for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
        sprintf(newCharBuff, gszStreamCharFileFmt, Character_GetGolferModelID(nPlayer) + 1);
        StreamManagerIngame_AddStreamFileName(newCharBuff, StreamManagerIngame_BeginStreamCallbackIGChar,
                                              StreamManagerIngame_EndStreamCallbackIGChar);
    }
    for (i = 0; i < STREAM_NUM_CHAR_ADDED; i++) {
        gStreamManagerCharAdded[i] = 0;
    }
}

// Makes the round's list the sac files: malesac.gcb if the male slot has overlays, femsac.gcb if the
// female slot has, and each player's golfer's CharSac file. Character_LoadSacFiles streams it.
void StreamManagerIngame_SetupSacFiles(void) {
    char newCharBuff[128];
    int nPlayer;

    StreamManagerIngame_ClearStreamFileNames();
    if (Skalib_HasOverlays(ANIM_SLOT_MALE) != 0) {
        StreamManagerIngame_AddStreamFileName(gszStreamMaleSacFile, StreamManagerGlobals_BeginStreamCallback,
                                              StreamManagerGlobals_EndStreamCallback);
    }
    if (Skalib_HasOverlays(ANIM_SLOT_FEMALE) != 0) {
        StreamManagerIngame_AddStreamFileName(gszStreamFemaleSacFile,
                                              StreamManagerGlobals_BeginStreamCallback,
                                              StreamManagerGlobals_EndStreamCallback);
    }
    for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
        sprintf(newCharBuff, gszStreamCharSacFileFmt, Character_GetGolferModelID(nPlayer) + 1);
        StreamManagerIngame_AddStreamFileName(newCharBuff, StreamManagerIngame_BeginStreamCallbackIGChar,
                                              StreamManagerIngame_EndStreamCallbackIGChar);
    }
}

// Makes the round's list the current animation slot's sac file (malesac.gcb or femsac.gcb) and the
// CharSac file of each player whose golfer has an overlay in that slot. Character_ReloadSacFiles
// streams it.
void StreamManagerIngame_SetupCurSlotSacFiles(void) {
    char newCharBuff[128];
    u32 nSlot;
    int nPlayer;
    LibSlot* pSlot;
    int nModel;
    int i;

    StreamManagerIngame_ClearStreamFileNames();
    nSlot = Skalib_CurSlot();
    if (nSlot == ANIM_SLOT_MALE) {
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
            sprintf(newCharBuff, gszStreamCharSacFileFmt, nModel + 1);
            StreamManagerIngame_AddStreamFileName(newCharBuff, StreamManagerIngame_BeginStreamCallbackIGChar,
                                                  StreamManagerIngame_EndStreamCallbackIGChar);
        }
    }
}

// Streams the round's list to its end, drawing the loading screen its caller set up.
void StreamManagerIngame_StreamSacFiles(void) {
    StreamManagerIngame_OpenStreamFiles();
    do {
        UI_DrawLoadingScreenAndProgressBar(UI_LOADSCREEN_STEP);
    } while (Stream_Progress() != 0);
    StreamManagerIngame_CloseStreamFiles();
}

// Empty in this build.
static void StreamManagerFEChar_InitModule(void) {
}

// Makes the FE character list golfer nGolferId's front-end file (data/FEChars/<nGolferId +
// 1>charfe.gcb) and marks the golfer being loaded as not streamed yet. FE_StreamFunc_SkinInit
// calls it, then opens the list's stream itself.
void StreamManagerFEChar_SetupFileStream(s32 nGolferId, s32 nIndex) {
    char newCharBuff[256];

    StreamManagerFEChar_ClearStreamFileNames();
    sprintf(newCharBuff, gszStreamFECharFileFmt, nGolferId + 1);
    gpCrAPState->pLoadingGolfer->nStreamedId = -1;
    StreamManagerFEChar_AddStreamFileName(newCharBuff, StreamManagerFEChar_BeginStreamCallback,
                                          StreamManagerFEChar_EndStreamCallback);
}

// The opened and closed calls of each list's files; all empty in this build.
static void StreamManagerIngame_BeginStreamCallbackIGChar(void* pData) {
}

static void StreamManagerFrontend_BeginStreamCallback(void* pData) {
}

static void StreamManagerLoadScreen_BeginStreamCallback(void* pData) {
}

static void StreamManagerFEChar_BeginStreamCallback(void* pData) {
}

static void StreamManagerGlobals_BeginStreamCallback(void* pData) {
}

static void StreamManagerHole_BeginStreamCallback(void* pData) {
}

static void StreamManagerIngame_EndStreamCallbackIGChar(void* pData) {
}

static void StreamManagerFrontend_EndStreamCallback(void* pData) {
}

static void StreamManagerLoadScreen_EndStreamCallback(void* pData) {
}

static void StreamManagerFEChar_EndStreamCallback(void* pData) {
}

static void StreamManagerGlobals_EndStreamCallback(void* pData) {
}

static void StreamManagerHole_EndStreamCallback(void* pData) {
}

// Each list's add: file fileName goes on the end of the list, with the calls UStream makes when it
// opens it and when it is done with it. Nothing checks the list's USTREAM_MAX_FILES; no list is
// given more than its callers' files.
static void StreamManagerStartup_AddStreamFileName(const char* fileName, StreamBeginCallback callbackBegin,
                                                   StreamEndCallback callbackEnd) {
    strcpy(STREAM_LIST(STREAM_FILE_STARTUP).aszName[STREAM_LIST(STREAM_FILE_STARTUP).nNumFiles], fileName);
    STREAM_LIST(STREAM_FILE_STARTUP).apfnOpened[STREAM_LIST(STREAM_FILE_STARTUP).nNumFiles] = callbackBegin;
    STREAM_LIST(STREAM_FILE_STARTUP).apfnClosed[STREAM_LIST(STREAM_FILE_STARTUP).nNumFiles] = callbackEnd;
    STREAM_LIST(STREAM_FILE_STARTUP).nNumFiles++;
}

static void StreamManagerGlobals_AddStreamFileName(const char* fileName, StreamBeginCallback callbackBegin,
                                                   StreamEndCallback callbackEnd) {
    strcpy(STREAM_LIST(STREAM_FILE_GLOBALS).aszName[STREAM_LIST(STREAM_FILE_GLOBALS).nNumFiles], fileName);
    STREAM_LIST(STREAM_FILE_GLOBALS).apfnOpened[STREAM_LIST(STREAM_FILE_GLOBALS).nNumFiles] = callbackBegin;
    STREAM_LIST(STREAM_FILE_GLOBALS).apfnClosed[STREAM_LIST(STREAM_FILE_GLOBALS).nNumFiles] = callbackEnd;
    STREAM_LIST(STREAM_FILE_GLOBALS).nNumFiles++;
}

static void StreamManagerFrontend_AddStreamFileName(const char* fileName, StreamBeginCallback callbackBegin,
                                                    StreamEndCallback callbackEnd) {
    strcpy(STREAM_LIST(STREAM_FILE_FRONTEND).aszName[STREAM_LIST(STREAM_FILE_FRONTEND).nNumFiles], fileName);
    STREAM_LIST(STREAM_FILE_FRONTEND).apfnOpened[STREAM_LIST(STREAM_FILE_FRONTEND).nNumFiles] = callbackBegin;
    STREAM_LIST(STREAM_FILE_FRONTEND).apfnClosed[STREAM_LIST(STREAM_FILE_FRONTEND).nNumFiles] = callbackEnd;
    STREAM_LIST(STREAM_FILE_FRONTEND).nNumFiles++;
}

static void StreamManagerLoadScreen_AddStreamFileName(const char* fileName,
                                                      StreamBeginCallback callbackBegin,
                                                      StreamEndCallback callbackEnd) {
    strcpy(STREAM_LIST(STREAM_FILE_LOADSCREEN).aszName[STREAM_LIST(STREAM_FILE_LOADSCREEN).nNumFiles],
           fileName);
    STREAM_LIST(STREAM_FILE_LOADSCREEN).apfnOpened[STREAM_LIST(STREAM_FILE_LOADSCREEN).nNumFiles] =
        callbackBegin;
    STREAM_LIST(STREAM_FILE_LOADSCREEN).apfnClosed[STREAM_LIST(STREAM_FILE_LOADSCREEN).nNumFiles] =
        callbackEnd;
    STREAM_LIST(STREAM_FILE_LOADSCREEN).nNumFiles++;
}

// Each list's close and open: the list's stream is kept in gpStreamManagerLists->nStream.
static void StreamManagerIngame_CloseStreamFiles(void) {
    UStream_Close(gpStreamManagerLists->nStream);
}

static void StreamManagerIngame_OpenStreamFiles(void) {
    s32 streamID;
    streamID = Stream_OpenStreamFiles(&STREAM_LIST(STREAM_FILE_INGAME));
    gpStreamManagerLists->nStream = streamID;
}

static void StreamManagerFrontend_CloseStreamFiles(void) {
    UStream_Close(gpStreamManagerLists->nStream);
}

static void StreamManagerFrontend_OpenStreamFiles(void) {
    s32 streamID;
    streamID = Stream_OpenStreamFiles(&STREAM_LIST(STREAM_FILE_FRONTEND));
    gpStreamManagerLists->nStream = streamID;
}

static void StreamManagerLoadScreen_CloseStreamFiles(void) {
    UStream_Close(gpStreamManagerLists->nStream);
}

static void StreamManagerLoadScreen_OpenStreamFiles(void) {
    s32 streamID;
    streamID = Stream_OpenStreamFiles(&STREAM_LIST(STREAM_FILE_LOADSCREEN));
    gpStreamManagerLists->nStream = streamID;
}

static void StreamManagerStartup_CloseStreamFiles(void) {
    UStream_Close(gpStreamManagerLists->nStream);
}

static void StreamManagerStartup_OpenStreamFiles(void) {
    s32 streamID;
    streamID = Stream_OpenStreamFiles(&STREAM_LIST(STREAM_FILE_STARTUP));
    gpStreamManagerLists->nStream = streamID;
}

static void StreamManagerGlobals_OpenStreamFiles(void) {
    s32 streamID;
    streamID = Stream_OpenStreamFiles(&STREAM_LIST(STREAM_FILE_GLOBALS));
    gpStreamManagerLists->nStream = streamID;
}

static void StreamManagerHole_CloseStreamFiles(void) {
    UStream_Close(gpStreamManagerLists->nStream);
}

static void StreamManagerHole_OpenStreamFiles(void) {
    s32 streamID;
    streamID = Stream_OpenStreamFiles(&STREAM_LIST(STREAM_FILE_HOLE));
    gpStreamManagerLists->nStream = streamID;
}

static void StreamManagerHole_AddStreamFileName(const char* fileName, StreamBeginCallback callbackBegin,
                                                StreamEndCallback callbackEnd) {
    strcpy(STREAM_LIST(STREAM_FILE_HOLE).aszName[STREAM_LIST(STREAM_FILE_HOLE).nNumFiles], fileName);
    STREAM_LIST(STREAM_FILE_HOLE).apfnOpened[STREAM_LIST(STREAM_FILE_HOLE).nNumFiles] = callbackBegin;
    STREAM_LIST(STREAM_FILE_HOLE).apfnClosed[STREAM_LIST(STREAM_FILE_HOLE).nNumFiles] = callbackEnd;
    STREAM_LIST(STREAM_FILE_HOLE).nNumFiles++;
}

static void StreamManagerHole_ClearStreamFileNames(void) {
    STREAM_LIST(STREAM_FILE_HOLE).nNumFiles = 0;
}

static void StreamManagerIngame_AddStreamFileName(const char* fileName, StreamBeginCallback callbackBegin,
                                                  StreamEndCallback callbackEnd) {
    strcpy(STREAM_LIST(STREAM_FILE_INGAME).aszName[STREAM_LIST(STREAM_FILE_INGAME).nNumFiles], fileName);
    STREAM_LIST(STREAM_FILE_INGAME).apfnOpened[STREAM_LIST(STREAM_FILE_INGAME).nNumFiles] = callbackBegin;
    STREAM_LIST(STREAM_FILE_INGAME).apfnClosed[STREAM_LIST(STREAM_FILE_INGAME).nNumFiles] = callbackEnd;
    STREAM_LIST(STREAM_FILE_INGAME).nNumFiles++;
}

static void StreamManagerIngame_ClearStreamFileNames(void) {
    STREAM_LIST(STREAM_FILE_INGAME).nNumFiles = 0;
}

static void StreamManagerFEChar_AddStreamFileName(const char* fileName, StreamBeginCallback callbackBegin,
                                                  StreamEndCallback callbackEnd) {
    strcpy(STREAM_LIST(STREAM_FILE_FECHAR).aszName[STREAM_LIST(STREAM_FILE_FECHAR).nNumFiles], fileName);
    STREAM_LIST(STREAM_FILE_FECHAR).apfnOpened[STREAM_LIST(STREAM_FILE_FECHAR).nNumFiles] = callbackBegin;
    STREAM_LIST(STREAM_FILE_FECHAR).apfnClosed[STREAM_LIST(STREAM_FILE_FECHAR).nNumFiles] = callbackEnd;
    STREAM_LIST(STREAM_FILE_FECHAR).nNumFiles++;
}

static void StreamManagerFEChar_ClearStreamFileNames(void) {
    STREAM_LIST(STREAM_FILE_FECHAR).nNumFiles = 0;
}

// The current hole's number on its course, 0..17 (Game_CurHoleIndex is its place in the round).
int Game_GetCurHoleNum(void) {
    return gpGame->nCurHoleNum;
}
