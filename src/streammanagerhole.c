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

char lbl_80186BD8[] = "data/FEnd/FEnd.gcb";
char lbl_80186BEC[] = "FEChar.gcb";
char lbl_80186BF8[] = "LoadOnce.gcb";
char lbl_80186C08[] = "startup.gcb";
char lbl_80186C14[] = "data/Load/Load%d.gcb";
char lbl_80186C2C[] = "data/%s/";
char lbl_80186C38[] = "/hole.hog";
char lbl_80186C44[] = "GlbData.gcb";
char lbl_80186C50[] = "GlbChar.gcb";
char lbl_80186C5C[] = "data/Chars/%02dchar.gcb";
char lbl_80186C74[] = "malesac.gcb";
char lbl_80186C80[] = "femsac.gcb";
char lbl_80186C8C[] = "data/CharSac/%02dchrsac.gcb";
char lbl_80186CA8[] = "data/FEChars/%02dcharfe.gcb";

// Defined here, last address first (CodeWarrior lays out .bss in reverse).
u8          lbl_801A48C8[0x38];
StreamLists lbl_801A3728;

StreamLists* lbl_80280DF8 = &lbl_801A3728;

// ---- sweep code (not yet cleaned up) ----

void fn_800143B8();
void fn_80014524(void);
void fn_80014590(void);
void Skalib_Register();
void Skalib_Unregister();
void fn_80010284();
void fn_800102B4();
void fn_8001CDD4();
void fn_8001CE34();
void fn_8001CFF0();
void fn_8001D268();
void fn_8001D44C();
void fn_8001D47C();
void fn_8001FAA8();
void fn_8001FAD8();
void fn_80039454();
void fn_800394AC();
void fn_80046130();
void fn_80046174();
void fn_800676B8();
void fn_800676E8();
void fn_8008EC68();
void fn_8008ED28();
void fn_8009EA98();
void fn_8009EAF0();
void fn_800A295C();
void fn_800A298C();
void fn_80014594(void);
void fn_800145E0(void);
void fn_800918A4(void);     // fe_movies.c: set up the loading screen
void fn_8009198C(int nMode);    // fe_movies.c: update the loading screen
void fn_80091818(void);     // fe_movies.c
void fn_80091778(void);     // fe_movies.c
void fn_8001529C(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*));
void fn_8001462C(void);
void fn_8000B9E4();
void fn_8000BA14();
void fn_8001CE04();
void fn_8001D238();
void fn_800394F0();
void fn_80039520();
void fn_80076F24();
void fn_80076F54();
void fn_80076F58();
void fn_8008D9AC();
void fn_800B9944();
void fn_800B9988();
void UI_vEATraxRegisterStreamClients();
void UI_vEATraxUnRegisterStreamClients();
void fn_8010508C();
void fn_80105154();
void fn_80124A40();
void fn_80124A70();
void fn_80014668(void);
void fn_800146C4(void);
void fn_800147A4(void);
void fn_800B160C();
void fn_800B1644();
void fn_800147D4(void);
void fn_80014804(void);
void fn_80014834(void);
void Golfer_RegisterStatsHandler();
void Golfer_UnregisterStatsHandler();
void Session_RegisterRecordsHandler();
void Session_UnregisterRecordsHandler();
void fn_800D29EC();
void fn_800D2A30();
void fn_800D33F4();
void fn_800D3424();
void fn_800EAE74();
void fn_800EAEB8();
void GameModeDriverPGATour_RegisterStreamClients();
void GameModeDriverPGATour_UnregisterStreamClients();
void GameModeDriverRTE_RegisterStreamClients();
void GameModeDriverRTE_UnregisterStreamClients();
void GameMode4_RegisterStreamClients();
void GameMode4_UnregisterStreamClients();
void fn_8011763C();
void fn_8011766C();
void fn_80014864(void);
void fn_800148A8(void);
void fn_800148EC(void);
void fn_800307C0();
void fn_80030818();
void fn_8006449C();
void fn_800644CC();
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

    lbl_80280DF8->aParams[0].nNumFiles = 0;
    lbl_80280DF8->aParams[1].nNumFiles = 0;
    lbl_80280DF8->aParams[2].nNumFiles = 0;
    lbl_80280DF8->aParams[3].nNumFiles = 0;
    lbl_80280DF8->aParams[4].nNumFiles = 0;
    lbl_80280DF8->aParams[5].nNumFiles = 0;
    lbl_80280DF8->aParams[6].nNumFiles = 0;
    fn_80014FA8(lbl_80186BD8, fn_80014E6C, fn_80014E84);
    fn_80014FA8(lbl_80186BEC, fn_80014E6C, fn_80014E84);
    fn_80014F20(lbl_80186BF8, fn_80014E78, fn_80014E90);
    fn_80014E98(lbl_80186BF8, fn_80014E78, fn_80014E90);
    fn_80014E98(lbl_80186C08, fn_80014E78, fn_80014E90);
    for (i = 0; i < 30; i++) {
        lbl_801A48C8[i] = 0;
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

    sprintf(szName, lbl_80186C14, nFile);
    fn_80015030(szName, fn_80014E70, fn_80014E88);
}

// ---- sweep code (not yet cleaned up) ----

void fn_80014590(void) {
}

void fn_80014594(void) {
    fn_80010284();
    fn_80039454();
    fn_8008EC68();
    fn_80046130();
    fn_8001CDD4();
    Skalib_Register();
    fn_8001FAA8();
    fn_8001CFF0();
    fn_8001D44C();
    fn_8009EA98();
    fn_800A295C();
    fn_800676B8();
}

void fn_800145E0(void) {
    fn_800102B4();
    fn_800394AC();
    fn_8008ED28();
    fn_80046174();
    fn_8001CE34();
    Skalib_Unregister();
    fn_8001FAD8();
    fn_8001D268();
    fn_8001D47C();
    fn_8009EAF0();
    fn_800A298C();
    fn_800676E8();
}

void fn_8001462C(void) {
    fn_800918A4();
    fn_800150E0();
    do {
        fn_8009198C(0);
    } while ((u8)UStream_Update() != 0);   // fake match: this file tests the result as a byte
    fn_800150B8();
}

void fn_80014668(void) {
    fn_80010284();
    fn_8001CE04();
    Skalib_Register();
    fn_8001FAA8();
    fn_8008EC68();
    fn_800394F0();
    fn_8008D9AC();
    fn_800B9944();
    fn_8001D238();
    fn_8009EA98();
    fn_80076F24();
    fn_80076F54();
    fn_8010508C();
    fn_80124A40();
    fn_8000B9E4();
    UI_vEATraxRegisterStreamClients();
}

void fn_800146C4(void) {
    fn_800102B4();
    fn_8001CE34();
    Skalib_Unregister();
    fn_8001FAD8();
    fn_8008ED28();
    fn_80039520();
    fn_8000BA14();
    fn_800B9988();
    fn_8001D268();
    fn_80076F58();
    fn_80105154();
    fn_80124A70();
    fn_8009EAF0();
    UI_vEATraxUnRegisterStreamClients();
}

// ---- end of sweep code ----

// Stream list 1 (the front end's files), with the loading screen unless the front end's b0F is set.
void fn_80014718(void) {
    if (lbl_801D7148.b0F == 0) {
        fn_80091778();
        fn_800918A4();
    }
    fn_80015134();
    do {
        if (lbl_801D7148.b0F == 0) {
            fn_8009198C(0);
        }
    } while (UStream_Update() != 0);
    if (lbl_801D7148.b0F == 0) {
        fn_8009198C(1);
    }
    fn_8001510C();
    if (lbl_801D7148.b0F == 0) {
        fn_80091818();
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
    fn_8008EC68();
    fn_8009EA98();
    fn_800B160C();
    fn_80014864();
}

void fn_80014804(void) {
    fn_800102B4();
    fn_8008ED28();
    fn_8009EAF0();
    fn_800B1644();
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
    fn_800D29EC();
    fn_800EAE74();
    fn_800D33F4();
    GameMode4_RegisterStreamClients();
    fn_8011763C();
    GameModeDriverPGATour_RegisterStreamClients();
    GameModeDriverRTE_RegisterStreamClients();
}

void fn_800148A8(void) {
    Golfer_UnregisterStatsHandler();
    Session_UnregisterRecordsHandler();
    UI_vEATraxUnRegisterStreamClients();
    fn_800D2A30();
    fn_800EAEB8();
    fn_800D3424();
    GameMode4_UnregisterStreamClients();
    fn_8011766C();
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
    fn_800307C0();
    fn_80046130();
    fn_8006449C();
    fn_8011E468();
}

void fn_8001494C(void) {
    fn_8011E4A4();
    fn_80046174();
    fn_80030818();
    fn_800102B4();
    fn_800644CC();
}

// ---- end of sweep code ----

// Stream the current hole's file (data/<course>/<hole>/hole.hog, or the session's override) as
// list 6, updating the loading screen until it is all read.
void StreamManagerHole_StreamFiles(void) {
    char szPath[0x80];  // size unknown: the frame allows up to 0x84 bytes
    char* szCourse;
    char* szHole;

    fn_800918A4();
    fn_80015324();
    if (gSession.n5B34 != 0) {
        fn_8001529C(gSession.p5B30, fn_80014E7C, fn_80014E94);
    } else {
        szCourse = fn_800E2680();
        szHole = GameManager_GetHoleName(fn_80015464());
        sprintf(szPath, lbl_80186C2C, szCourse);
        strcat(szPath, szHole);
        strcat(szPath, lbl_80186C38);
        fn_8001529C(szPath, fn_80014E7C, fn_80014E94);
    }
    fn_8001526C();
    do {
        fn_8009198C(0);
    } while (UStream_Update() != 0);
    fn_8009198C(1);
    fn_80015244();
    fn_80091818();
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

    lbl_80281CE4 = 0;
    fn_800153BC();
    fn_80015334(lbl_80186C44, fn_80014E78, fn_80014E90);
    fn_80015334(lbl_80186C50, fn_80014E68, fn_80014E80);
    for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
        sprintf(szName, lbl_80186C5C, fn_8001C558(nPlayer) + 1);
        fn_80015334(szName, fn_80014E68, fn_80014E80);
    }
    for (i = 0; i < 30; i++) {
        lbl_801A48C8[i] = 0;
    }
}

// Refill stream list 0 with the sac files: malesac for animation slot 0 and femsac for slot 1
// when that slot has overlays, and every player's golfer's CharSac file.
void fn_80014BB4(void) {
    char szName[0x80];  // size unknown: the frame allows up to 0x88 bytes
    int nPlayer;

    fn_800153BC();
    if (Skalib_HasOverlays(0) != 0) {
        fn_80015334(lbl_80186C74, fn_80014E78, fn_80014E90);
    }
    if (Skalib_HasOverlays(1) != 0) {
        fn_80015334(lbl_80186C80, fn_80014E78, fn_80014E90);
    }
    for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
        sprintf(szName, lbl_80186C8C, fn_8001C558(nPlayer) + 1);
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
        fn_80015334(lbl_80186C74, fn_80014E78, fn_80014E90);
    } else {
        fn_80015334(lbl_80186C80, fn_80014E78, fn_80014E90);
    }
    pSlot = &lbl_801C6068[nSlot];
    for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
        nModel = fn_8001C558(nPlayer);
        for (i = 0; i < pSlot->nOverlays; i++) {
            if (pSlot->overlays[i].n14 == nModel) {
                break;
            }
        }
        if (i < pSlot->nOverlays) {
            sprintf(szName, lbl_80186C8C, nModel + 1);
            fn_80015334(szName, fn_80014E68, fn_80014E80);
        }
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80014DC0(void) {
    fn_800150E0();
    do {
        fn_8009198C(0);
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
    sprintf(szName, lbl_80186CA8, nChar + 1);
    lbl_80281EE0->pB8->n14 = -1;
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
    strcpy(lbl_80280DF8->aParams[5].aszName[lbl_80280DF8->aParams[5].nNumFiles], szName);
    lbl_80280DF8->aParams[5].apfnOpened[lbl_80280DF8->aParams[5].nNumFiles] = pfnOpened;
    lbl_80280DF8->aParams[5].apfnClosed[lbl_80280DF8->aParams[5].nNumFiles] = pfnClosed;
    lbl_80280DF8->aParams[5].nNumFiles++;
}

// The same for list 4.
void fn_80014F20(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*)) {
    strcpy(lbl_80280DF8->aParams[4].aszName[lbl_80280DF8->aParams[4].nNumFiles], szName);
    lbl_80280DF8->aParams[4].apfnOpened[lbl_80280DF8->aParams[4].nNumFiles] = pfnOpened;
    lbl_80280DF8->aParams[4].apfnClosed[lbl_80280DF8->aParams[4].nNumFiles] = pfnClosed;
    lbl_80280DF8->aParams[4].nNumFiles++;
}

// The same for list 1.
void fn_80014FA8(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*)) {
    strcpy(lbl_80280DF8->aParams[1].aszName[lbl_80280DF8->aParams[1].nNumFiles], szName);
    lbl_80280DF8->aParams[1].apfnOpened[lbl_80280DF8->aParams[1].nNumFiles] = pfnOpened;
    lbl_80280DF8->aParams[1].apfnClosed[lbl_80280DF8->aParams[1].nNumFiles] = pfnClosed;
    lbl_80280DF8->aParams[1].nNumFiles++;
}

// The same for list 2.
void fn_80015030(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*)) {
    strcpy(lbl_80280DF8->aParams[2].aszName[lbl_80280DF8->aParams[2].nNumFiles], szName);
    lbl_80280DF8->aParams[2].apfnOpened[lbl_80280DF8->aParams[2].nNumFiles] = pfnOpened;
    lbl_80280DF8->aParams[2].apfnClosed[lbl_80280DF8->aParams[2].nNumFiles] = pfnClosed;
    lbl_80280DF8->aParams[2].nNumFiles++;
}

// ---- sweep code (not yet cleaned up) ----

void fn_800150B8(void) {
    UStream_Close(lbl_80280DF8->nStream);
}

void fn_800150E0(void) {
    s32 t0;
    t0 = Stream_OpenStreamFiles(&lbl_80280DF8->aParams[0]);
    lbl_80280DF8->nStream = t0;
}

void fn_8001510C(void) {
    UStream_Close(lbl_80280DF8->nStream);
}

void fn_80015134(void) {
    s32 t0;
    t0 = Stream_OpenStreamFiles(&lbl_80280DF8->aParams[1]);
    lbl_80280DF8->nStream = t0;
}

void fn_80015164(void) {
    UStream_Close(lbl_80280DF8->nStream);
}

void fn_8001518C(void) {
    s32 t0;
    t0 = Stream_OpenStreamFiles(&lbl_80280DF8->aParams[2]);
    lbl_80280DF8->nStream = t0;
}

void fn_800151BC(void) {
    UStream_Close(lbl_80280DF8->nStream);
}

void fn_800151E4(void) {
    s32 t0;
    t0 = Stream_OpenStreamFiles(&lbl_80280DF8->aParams[5]);
    lbl_80280DF8->nStream = t0;
}

void fn_80015214(void) {
    s32 t0;
    t0 = Stream_OpenStreamFiles(&lbl_80280DF8->aParams[4]);
    lbl_80280DF8->nStream = t0;
}

void fn_80015244(void) {
    UStream_Close(lbl_80280DF8->nStream);
}

void fn_8001526C(void) {
    s32 t0;
    t0 = Stream_OpenStreamFiles(&lbl_80280DF8->aParams[6]);
    lbl_80280DF8->nStream = t0;
}

// ---- end of sweep code ----

// The same for list 6.
void fn_8001529C(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*)) {
    strcpy(lbl_80280DF8->aParams[6].aszName[lbl_80280DF8->aParams[6].nNumFiles], szName);
    lbl_80280DF8->aParams[6].apfnOpened[lbl_80280DF8->aParams[6].nNumFiles] = pfnOpened;
    lbl_80280DF8->aParams[6].apfnClosed[lbl_80280DF8->aParams[6].nNumFiles] = pfnClosed;
    lbl_80280DF8->aParams[6].nNumFiles++;
}

// ---- sweep code (not yet cleaned up) ----

void fn_80015324(void) {
    lbl_80280DF8->aParams[6].nNumFiles = 0;
}

// ---- end of sweep code ----

// The same for list 0.
void fn_80015334(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*)) {
    strcpy(lbl_80280DF8->aParams[0].aszName[lbl_80280DF8->aParams[0].nNumFiles], szName);
    lbl_80280DF8->aParams[0].apfnOpened[lbl_80280DF8->aParams[0].nNumFiles] = pfnOpened;
    lbl_80280DF8->aParams[0].apfnClosed[lbl_80280DF8->aParams[0].nNumFiles] = pfnClosed;
    lbl_80280DF8->aParams[0].nNumFiles++;
}

// ---- sweep code (not yet cleaned up) ----

void fn_800153BC(void) {
    lbl_80280DF8->aParams[0].nNumFiles = 0;
}

// ---- end of sweep code ----

// The same for list 3.
void fn_800153CC(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*)) {
    strcpy(lbl_80280DF8->aParams[3].aszName[lbl_80280DF8->aParams[3].nNumFiles], szName);
    lbl_80280DF8->aParams[3].apfnOpened[lbl_80280DF8->aParams[3].nNumFiles] = pfnOpened;
    lbl_80280DF8->aParams[3].apfnClosed[lbl_80280DF8->aParams[3].nNumFiles] = pfnClosed;
    lbl_80280DF8->aParams[3].nNumFiles++;
}

// ---- sweep code (not yet cleaned up) ----

void fn_80015454(void) {
    lbl_80280DF8->aParams[3].nNumFiles = 0;
}

int fn_80015464(void) {
    return gpGame->nCurHoleNum;
}

// ---- end of sweep code ----
