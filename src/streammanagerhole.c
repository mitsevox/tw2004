// streammanagerhole.c (our name, after TW06's golf/streaming/streammanagerhole.cpp; the 2003 game
// is C): the stream file lists (front end, startup, in game, characters, loading screens, the
// hole) and their loads, the lists of stream handlers each part registers and unregisters, and
// (from 0x80015470) the renderer's state cache, the 2D view and the vertex output.

#include "ustream.h"
#include "camera.h"
#include "game.h"
#include "golfer.h"
#include "character.h"
#include "frontend/fe.h"
#include "gx.h"
#include "llpict.h"
#include "unsorted/cull.h"

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
u8          gStreamManagerCharAdded[0x38];
StreamLists gStreamManagerLists;

StreamLists* gpStreamManagerLists = &gStreamManagerLists;

// ---- sweep code (not yet cleaned up) ----

void fn_800143B8();
void fn_80014524(void);
void fn_80014590(void);
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
void fn_80014594(void);
void fn_800145E0(void);
void UI_InitLoadingBar(void);     // uiProcessPolygon.c: set up the loading screen
void UI_DrawLoadingScreenAndProgressBar(int nMode);    // uiProcessPolygon.c: update the loading screen
void UI_FreeLoadingPicture(void);     // uiProcessPolygon.c
void UI_LoadLoadingBarTexture(void);     // uiProcessPolygon.c
void fn_8001529C(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*));
void fn_8001462C(void);
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
void fn_80014668(void);
void fn_800146C4(void);
void fn_800147A4(void);
void startup_RegisterStreamClients();
void startup_UnregisterStreamClients();
void fn_800147D4(void);
void fn_80014804(void);
void fn_80014834(void);
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
void fn_80014864(void);
void fn_800148A8(void);
void fn_800148EC(void);
void Ter_RegisterStreamClients();
void Ter_UnRegisterStreamClients();
void StaticCam_RegisterStreamClients();
void StaticCam_UnRegisterStreamClients();
void fn_8011E468();
void fn_8011E4A4();
void fn_8001491C(void);
void fn_8001494C(void);
void fn_80014A60(void);
void fn_80014DC0(void);
void fn_80014DF8(void);
void fn_80014E68(void* pArg);
void fn_80014E6C(void* pArg);
void fn_80014E70(void* pArg);
void fn_80014E74(void* pArg);
void fn_80014E78(void* pArg);
void fn_80014E7C(void* pArg);
void fn_80014E80(void* pArg);
void fn_80014E84(void* pArg);
void fn_80014E88(void* pArg);
void fn_80014E8C(void* pArg);
void fn_800153CC(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*));
void fn_80015030(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*));
void fn_80015334(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*));
u32  Skalib_CurSlot(void);          // skalib.c
int  Skalib_HasOverlays(int nSlot); // skalib.c
void fn_80014E90(void* pArg);
void fn_80014E94(void* pArg);
void UStream_Close();
s32 Stream_OpenStreamFiles();
void fn_800150B8(void);
void fn_800150E0(void);
void fn_8001510C(void);
void fn_80015134(void);
void fn_80015164(void);
void fn_8001518C(void);
void fn_800151BC(void);
void fn_800151E4(void);
void fn_80015214(void);
void fn_80015244(void);
void fn_8001526C(void);
void fn_80015324(void);
void fn_800153BC(void);
void fn_80015454(void);

// ---- end of sweep code ----

void fn_80014E98(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*));
void fn_80014F20(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*));
void fn_80014FA8(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*));

// Set up the stream lists: empty them all, then put the front end's files in list 1 and the
// load-once and startup files in lists 4 and 5.
void fn_800143B8(void) {
    int i;

    gpStreamManagerLists->aParams[0].nNumFiles = 0;
    gpStreamManagerLists->aParams[1].nNumFiles = 0;
    gpStreamManagerLists->aParams[2].nNumFiles = 0;
    gpStreamManagerLists->aParams[3].nNumFiles = 0;
    gpStreamManagerLists->aParams[4].nNumFiles = 0;
    gpStreamManagerLists->aParams[5].nNumFiles = 0;
    gpStreamManagerLists->aParams[6].nNumFiles = 0;
    fn_80014FA8(gszStreamFrontendFile, fn_80014E6C, fn_80014E84);
    fn_80014FA8(gszStreamFECharFile, fn_80014E6C, fn_80014E84);
    fn_80014F20(gszStreamLoadOnceFile, fn_80014E78, fn_80014E90);
    fn_80014E98(gszStreamLoadOnceFile, fn_80014E78, fn_80014E90);
    fn_80014E98(gszStreamStartupFile, fn_80014E78, fn_80014E90);
    for (i = 0; i < 30; i++) {
        gStreamManagerCharAdded[i] = 0;
    }
    fn_80014DF8();
    fn_80014A60();
}

// ---- sweep code (not yet cleaned up) ----

void fn_80014524(void) {
    fn_800143B8();
}

// ---- end of sweep code ----

// Add loading file nFile (data/Load/Load<n>.gcb) to stream list 2.
void fn_80014544(int nFile) {
    char szName[0x40];   // size unknown: the frame allows 0x40..0x48 bytes

    sprintf(szName, gszStreamLoadScreenFileFmt, nFile);
    fn_80015030(szName, fn_80014E70, fn_80014E88);
}

// ---- sweep code (not yet cleaned up) ----

void fn_80014590(void) {
}

void fn_80014594(void) {
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

void fn_800145E0(void) {
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

void fn_8001462C(void) {
    UI_InitLoadingBar();
    fn_800150E0();
    do {
        UI_DrawLoadingScreenAndProgressBar(0);
    } while ((u8)UStream_Update() != 0);   // fake match: this file tests the result as a byte
    fn_800150B8();
}

void fn_80014668(void) {
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

void fn_800146C4(void) {
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

// ---- end of sweep code ----

// Stream list 1 (the front end's files), with the loading screen unless the front end's bFirstTime
// is set.
void fn_80014718(void) {
    if (gFEState.bFirstTime == 0) {
        UI_LoadLoadingBarTexture();
        UI_InitLoadingBar();
    }
    fn_80015134();
    do {
        if (gFEState.bFirstTime == 0) {
            UI_DrawLoadingScreenAndProgressBar(0);
        }
    } while (UStream_Update() != 0);
    if (gFEState.bFirstTime == 0) {
        UI_DrawLoadingScreenAndProgressBar(1);
    }
    fn_8001510C();
    if (gFEState.bFirstTime == 0) {
        UI_FreeLoadingPicture();
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_800147A4(void) {
    fn_8001518C();
    do {

    } while ((u8)UStream_Update() != 0);   // fake match: this file tests the result as a byte
    fn_80015164();
}

void fn_800147D4(void) {
    fn_80010284();
    UI_RegisterStreamClients();
    MC_RegisterStreamClients();
    startup_RegisterStreamClients();
    fn_80014864();
}

void fn_80014804(void) {
    fn_800102B4();
    UI_UnregisterStreamClients();
    MC_UnRegisterStreamClients();
    startup_UnregisterStreamClients();
    fn_800148A8();
}

void fn_80014834(void) {
    fn_800151E4();
    do {

    } while ((u8)UStream_Update() != 0);   // fake match: this file tests the result as a byte
    fn_800151BC();
}

void fn_80014864(void) {
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

void fn_800148A8(void) {
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

void fn_800148EC(void) {
    fn_80015214();
    do {

    } while ((u8)UStream_Update() != 0);   // fake match: this file tests the result as a byte
    fn_8001510C();
}

void fn_8001491C(void) {
    fn_80010284();
    Ter_RegisterStreamClients();
    DynObj_RegisterStreamClients();
    StaticCam_RegisterStreamClients();
    fn_8011E468();
}

void fn_8001494C(void) {
    fn_8011E4A4();
    DynObj_UnRegisterStreamClients();
    Ter_UnRegisterStreamClients();
    fn_800102B4();
    StaticCam_UnRegisterStreamClients();
}

// ---- end of sweep code ----

// Stream the current hole's file (data/<course>/<hole>/hole.hog, or the session's override) as
// list 6, updating the loading screen until it is all read.
void StreamManagerHole_StreamFiles(void) {
    char szPath[0x80];  // size unknown: the frame allows up to 0x84 bytes
    char* szCourse;
    char* szHole;

    UI_InitLoadingBar();
    fn_80015324();
    if (gSession.n5B34 != 0) {
        fn_8001529C(gSession.p5B30, fn_80014E7C, fn_80014E94);
    } else {
        szCourse = GM_GetCourseName();
        szHole = GameManager_GetHoleName(Game_GetCurHoleNum());
        sprintf(szPath, gszStreamCourseDirFmt, szCourse);
        strcat(szPath, szHole);
        strcat(szPath, gszStreamHoleFileName);
        fn_8001529C(szPath, fn_80014E7C, fn_80014E94);
    }
    fn_8001526C();
    do {
        UI_DrawLoadingScreenAndProgressBar(0);
    } while (UStream_Update() != 0);
    UI_DrawLoadingScreenAndProgressBar(1);
    fn_80015244();
    UI_FreeLoadingPicture();
}

// ---- sweep code (not yet cleaned up) ----

void fn_80014A60(void) {
}

// ---- end of sweep code ----

// Refill stream list 0 with the global data and character files and every player's golfer's
// character file.
void fn_80014A64(void) {
    char szName[0x80];  // size unknown: the frame allows up to 0x84 bytes
    int nPlayer;
    int i;

    gSacReloading = 0;
    fn_800153BC();
    fn_80015334(gszStreamGlbDataFile, fn_80014E78, fn_80014E90);
    fn_80015334(gszStreamGlbCharFile, fn_80014E68, fn_80014E80);
    for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
        sprintf(szName, gszStreamCharFileFmt, Character_GetGolferModelID(nPlayer) + 1);
        fn_80015334(szName, fn_80014E68, fn_80014E80);
    }
    for (i = 0; i < 30; i++) {
        gStreamManagerCharAdded[i] = 0;
    }
}

// Refill stream list 0 with the sac files: malesac for animation slot 0 and femsac for slot 1
// when that slot has overlays, and every player's golfer's CharSac file.
void fn_80014BB4(void) {
    char szName[0x80];  // size unknown: the frame allows up to 0x88 bytes
    int nPlayer;

    fn_800153BC();
    if (Skalib_HasOverlays(0) != 0) {
        fn_80015334(gszStreamMaleSacFile, fn_80014E78, fn_80014E90);
    }
    if (Skalib_HasOverlays(1) != 0) {
        fn_80015334(gszStreamFemaleSacFile, fn_80014E78, fn_80014E90);
    }
    for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
        sprintf(szName, gszStreamCharSacFileFmt, Character_GetGolferModelID(nPlayer) + 1);
        fn_80015334(szName, fn_80014E68, fn_80014E80);
    }
}

// Refill stream list 0 with the current animation slot's sac file and the CharSac file of every
// player whose golfer has an overlay loaded in that slot.
void fn_80014C9C(void) {
    char szName[0x80];  // size unknown: the frame allows up to 0x8C bytes
    u32 nSlot;
    int nPlayer;
    LibSlot* pSlot;
    int nModel;
    int i;

    fn_800153BC();
    nSlot = Skalib_CurSlot();
    if (nSlot == 0) {
        fn_80015334(gszStreamMaleSacFile, fn_80014E78, fn_80014E90);
    } else {
        fn_80015334(gszStreamFemaleSacFile, fn_80014E78, fn_80014E90);
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
            fn_80015334(szName, fn_80014E68, fn_80014E80);
        }
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80014DC0(void) {
    fn_800150E0();
    do {
        UI_DrawLoadingScreenAndProgressBar(0);
    } while ((u8)UStream_Update() != 0);   // fake match: this file tests the result as a byte
    fn_800150B8();
}

void fn_80014DF8(void) {
}

// ---- end of sweep code ----

// Make stream list 3 hold only the front-end character file for character nChar
// (data/FEChars/<nChar + 1>charfe.gcb), for the golfer the front end is loading.
void fn_80014DFC(s32 nChar, s32 nUnused) {   // port: FEgolferanim.c passes a second argument this ignores
    char szName[0x100];  // size unknown: the frame allows up to 0x100 bytes

    fn_80015454();
    sprintf(szName, gszStreamFECharFileFmt, nChar + 1);
    gpCrAPState->pB8->nStreamedId = -1;
    fn_800153CC(szName, fn_80014E74, fn_80014E8C);
}

// ---- sweep code (not yet cleaned up) ----

// Stream file callbacks that do nothing: fn_80014E68..fn_80014E7C are called when a list's file
// is opened, fn_80014E80..fn_80014E94 when it is closed.
void fn_80014E68(void* pArg) {
}

void fn_80014E6C(void* pArg) {
}

void fn_80014E70(void* pArg) {
}

void fn_80014E74(void* pArg) {
}

void fn_80014E78(void* pArg) {
}

void fn_80014E7C(void* pArg) {
}

void fn_80014E80(void* pArg) {
}

void fn_80014E84(void* pArg) {
}

void fn_80014E88(void* pArg) {
}

void fn_80014E8C(void* pArg) {
}

void fn_80014E90(void* pArg) {
}

void fn_80014E94(void* pArg) {
}

// ---- end of sweep code ----

// Add a file to stream list 5, with the calls made when it is opened and closed.
void fn_80014E98(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*)) {
    strcpy(gpStreamManagerLists->aParams[5].aszName[gpStreamManagerLists->aParams[5].nNumFiles], szName);
    gpStreamManagerLists->aParams[5].apfnOpened[gpStreamManagerLists->aParams[5].nNumFiles] = pfnOpened;
    gpStreamManagerLists->aParams[5].apfnClosed[gpStreamManagerLists->aParams[5].nNumFiles] = pfnClosed;
    gpStreamManagerLists->aParams[5].nNumFiles++;
}

// The same for list 4.
void fn_80014F20(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*)) {
    strcpy(gpStreamManagerLists->aParams[4].aszName[gpStreamManagerLists->aParams[4].nNumFiles], szName);
    gpStreamManagerLists->aParams[4].apfnOpened[gpStreamManagerLists->aParams[4].nNumFiles] = pfnOpened;
    gpStreamManagerLists->aParams[4].apfnClosed[gpStreamManagerLists->aParams[4].nNumFiles] = pfnClosed;
    gpStreamManagerLists->aParams[4].nNumFiles++;
}

// The same for list 1.
void fn_80014FA8(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*)) {
    strcpy(gpStreamManagerLists->aParams[1].aszName[gpStreamManagerLists->aParams[1].nNumFiles], szName);
    gpStreamManagerLists->aParams[1].apfnOpened[gpStreamManagerLists->aParams[1].nNumFiles] = pfnOpened;
    gpStreamManagerLists->aParams[1].apfnClosed[gpStreamManagerLists->aParams[1].nNumFiles] = pfnClosed;
    gpStreamManagerLists->aParams[1].nNumFiles++;
}

// The same for list 2.
void fn_80015030(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*)) {
    strcpy(gpStreamManagerLists->aParams[2].aszName[gpStreamManagerLists->aParams[2].nNumFiles], szName);
    gpStreamManagerLists->aParams[2].apfnOpened[gpStreamManagerLists->aParams[2].nNumFiles] = pfnOpened;
    gpStreamManagerLists->aParams[2].apfnClosed[gpStreamManagerLists->aParams[2].nNumFiles] = pfnClosed;
    gpStreamManagerLists->aParams[2].nNumFiles++;
}

// ---- sweep code (not yet cleaned up) ----

void fn_800150B8(void) {
    UStream_Close(gpStreamManagerLists->nStream);
}

void fn_800150E0(void) {
    s32 t0;
    t0 = Stream_OpenStreamFiles(&gpStreamManagerLists->aParams[0]);
    gpStreamManagerLists->nStream = t0;
}

void fn_8001510C(void) {
    UStream_Close(gpStreamManagerLists->nStream);
}

void fn_80015134(void) {
    s32 t0;
    t0 = Stream_OpenStreamFiles(&gpStreamManagerLists->aParams[1]);
    gpStreamManagerLists->nStream = t0;
}

void fn_80015164(void) {
    UStream_Close(gpStreamManagerLists->nStream);
}

void fn_8001518C(void) {
    s32 t0;
    t0 = Stream_OpenStreamFiles(&gpStreamManagerLists->aParams[2]);
    gpStreamManagerLists->nStream = t0;
}

void fn_800151BC(void) {
    UStream_Close(gpStreamManagerLists->nStream);
}

void fn_800151E4(void) {
    s32 t0;
    t0 = Stream_OpenStreamFiles(&gpStreamManagerLists->aParams[5]);
    gpStreamManagerLists->nStream = t0;
}

void fn_80015214(void) {
    s32 t0;
    t0 = Stream_OpenStreamFiles(&gpStreamManagerLists->aParams[4]);
    gpStreamManagerLists->nStream = t0;
}

void fn_80015244(void) {
    UStream_Close(gpStreamManagerLists->nStream);
}

void fn_8001526C(void) {
    s32 t0;
    t0 = Stream_OpenStreamFiles(&gpStreamManagerLists->aParams[6]);
    gpStreamManagerLists->nStream = t0;
}

// ---- end of sweep code ----

// The same for list 6.
void fn_8001529C(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*)) {
    strcpy(gpStreamManagerLists->aParams[6].aszName[gpStreamManagerLists->aParams[6].nNumFiles], szName);
    gpStreamManagerLists->aParams[6].apfnOpened[gpStreamManagerLists->aParams[6].nNumFiles] = pfnOpened;
    gpStreamManagerLists->aParams[6].apfnClosed[gpStreamManagerLists->aParams[6].nNumFiles] = pfnClosed;
    gpStreamManagerLists->aParams[6].nNumFiles++;
}

// ---- sweep code (not yet cleaned up) ----

void fn_80015324(void) {
    gpStreamManagerLists->aParams[6].nNumFiles = 0;
}

// ---- end of sweep code ----

// The same for list 0.
void fn_80015334(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*)) {
    strcpy(gpStreamManagerLists->aParams[0].aszName[gpStreamManagerLists->aParams[0].nNumFiles], szName);
    gpStreamManagerLists->aParams[0].apfnOpened[gpStreamManagerLists->aParams[0].nNumFiles] = pfnOpened;
    gpStreamManagerLists->aParams[0].apfnClosed[gpStreamManagerLists->aParams[0].nNumFiles] = pfnClosed;
    gpStreamManagerLists->aParams[0].nNumFiles++;
}

// ---- sweep code (not yet cleaned up) ----

void fn_800153BC(void) {
    gpStreamManagerLists->aParams[0].nNumFiles = 0;
}

// ---- end of sweep code ----

// The same for list 3.
void fn_800153CC(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*)) {
    strcpy(gpStreamManagerLists->aParams[3].aszName[gpStreamManagerLists->aParams[3].nNumFiles], szName);
    gpStreamManagerLists->aParams[3].apfnOpened[gpStreamManagerLists->aParams[3].nNumFiles] = pfnOpened;
    gpStreamManagerLists->aParams[3].apfnClosed[gpStreamManagerLists->aParams[3].nNumFiles] = pfnClosed;
    gpStreamManagerLists->aParams[3].nNumFiles++;
}

// ---- sweep code (not yet cleaned up) ----

void fn_80015454(void) {
    gpStreamManagerLists->aParams[3].nNumFiles = 0;
}

// The current hole's number on its course (Game_CurHoleIndex gives its 0..17 place in the round).
int Game_GetCurHoleNum(void) {
    return gpGame->nCurHoleNum;
}

// ---- end of sweep code ----
