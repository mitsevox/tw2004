// streammanagerhole.c (our name, after TW06's golf/streaming/streammanagerhole.cpp; the 2003 game
// is C): loads a hole's stream files, registering and unregistering every system's stream handlers
// around each load. Not yet decompiled; the code below is the matched small functions.

#include "ustream.h"
#include "camera.h"
#include "game.h"
#include "golfer.h"
#include "character.h"
#include "frontend/fe.h"
#include "gx.h"
#include "unsorted/cull.h"

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
void fn_8000ADC0(f32 (*m)[4]);          // identity matrix
void fn_800141CC(void);
s32  fn_800072E0(void);
void fn_8000A0E8(f32 (*pSrc)[4], f32 (*pDst)[4]);  // UMemPool.c: copy a 4x4 matrix
void fn_80016C28(f32* pSrc, f32* pDst);             // negate four floats
void fn_80012520(u32 ePrim, u32 eFormat, u16 nVerts);  // LLFont.c: GXBegin
void fn_800124A8(void);                                // LLFont.c: end the primitive
void fn_800162AC(f32* pPos, f32* pColour, f32* pUV, int nVerts);
void fn_800169AC(void);     // apply lbl_80280E08's viewport
void fn_80016208(void);
void fn_80016978(f32 x0, f32 y0, f32 x1, f32 y1);
void fn_80016B54(int nWidth, int nHeight, f32 fX, f32 fY);
void fn_80016B6C(f32 fX, f32 fY);
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
void fn_800BA0A0();
void fn_800BA0E4();
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
void fn_80102310();
void fn_80102354();
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
s32 UStream_Open();
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
void fn_80015620(void);
void fn_80016124(s32 p0, s32 p1, s32 p2, s32 p3);
void fn_800162A0(void);
void fn_800162A4(void);
void fn_800162A8(void);
void fn_80016C7C(s32 p0, s32 p1, s32 p2, s32 p3);
void fn_80016C94(f32 farg0, f32 farg1, f32 farg2);
void fn_80016CA8(f32 farg0, f32 farg1);

void fn_80014524(void) {
    fn_800143B8();
}

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
    fn_800BA0A0();
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
    fn_800BA0E4();
}

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
    fn_800BA0A0();
    fn_800D29EC();
    fn_800EAE74();
    fn_800D33F4();
    fn_80102310();
    fn_8011763C();
    GameModeDriverPGATour_RegisterStreamClients();
    GameModeDriverRTE_RegisterStreamClients();
}

void fn_800148A8(void) {
    Golfer_UnregisterStatsHandler();
    Session_UnregisterRecordsHandler();
    fn_800BA0E4();
    fn_800D2A30();
    fn_800EAEB8();
    fn_800D3424();
    fn_80102354();
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

void fn_80014A60(void) {
}

void fn_80014DC0(void) {
    fn_800150E0();
    do {
        fn_8009198C(0);
    } while ((u8)UStream_Update() != 0);   // fake match: this file tests the result as a byte
    fn_800150B8();
}

void fn_80014DF8(void) {
}

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

void fn_800150B8(void) {
    UStream_Close(lbl_80280DF8->nStream);
}

void fn_800150E0(void) {
    s32 t0;
    t0 = UStream_Open(&lbl_80280DF8->aParams[0]);
    lbl_80280DF8->nStream = t0;
}

void fn_8001510C(void) {
    UStream_Close(lbl_80280DF8->nStream);
}

void fn_80015134(void) {
    s32 t0;
    t0 = UStream_Open(&lbl_80280DF8->aParams[1]);
    lbl_80280DF8->nStream = t0;
}

void fn_80015164(void) {
    UStream_Close(lbl_80280DF8->nStream);
}

void fn_8001518C(void) {
    s32 t0;
    t0 = UStream_Open(&lbl_80280DF8->aParams[2]);
    lbl_80280DF8->nStream = t0;
}

void fn_800151BC(void) {
    UStream_Close(lbl_80280DF8->nStream);
}

void fn_800151E4(void) {
    s32 t0;
    t0 = UStream_Open(&lbl_80280DF8->aParams[5]);
    lbl_80280DF8->nStream = t0;
}

void fn_80015214(void) {
    s32 t0;
    t0 = UStream_Open(&lbl_80280DF8->aParams[4]);
    lbl_80280DF8->nStream = t0;
}

void fn_80015244(void) {
    UStream_Close(lbl_80280DF8->nStream);
}

void fn_8001526C(void) {
    s32 t0;
    t0 = UStream_Open(&lbl_80280DF8->aParams[6]);
    lbl_80280DF8->nStream = t0;
}

void fn_80015324(void) {
    lbl_80280DF8->aParams[6].nNumFiles = 0;
}

void fn_800153BC(void) {
    lbl_80280DF8->aParams[0].nNumFiles = 0;
}

void fn_80015454(void) {
    lbl_80280DF8->aParams[3].nNumFiles = 0;
}

int fn_80015464(void) {
    return gpGame->nCurHoleNum;
}

void fn_80015620(void) {
}

void fn_80016124(s32 p0, s32 p1, s32 p2, s32 p3) {
    GXSetTexCoordGen2(p0, p1, p2, p3, 0, 125);
}

void* fn_8001614C(void) {
    return *lbl_80280DF0;
}

void fn_8001618C(u8 v) {
    *(u8*)(lbl_80280DC8 + 0x0) = v;
}

void fn_800162A0(void) {
}

void fn_800162A4(void) {
}

void fn_800162A8(void) {
}

void fn_80016C7C(s32 p0, s32 p1, s32 p2, s32 p3) {
    *(volatile u8*)0xCC008000 = p0;
    *(volatile u8*)0xCC008000 = p1;
    *(volatile u8*)0xCC008000 = p2;
    *(volatile u8*)0xCC008000 = p3;
}

void fn_80016C94(f32 farg0, f32 farg1, f32 farg2) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
    *(f32* )0xCC008000 = farg2;
}

void fn_80016CA8(f32 farg0, f32 farg1) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
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

// The same for list 6.
void fn_8001529C(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*)) {
    strcpy(lbl_80280DF8->aParams[6].aszName[lbl_80280DF8->aParams[6].nNumFiles], szName);
    lbl_80280DF8->aParams[6].apfnOpened[lbl_80280DF8->aParams[6].nNumFiles] = pfnOpened;
    lbl_80280DF8->aParams[6].apfnClosed[lbl_80280DF8->aParams[6].nNumFiles] = pfnClosed;
    lbl_80280DF8->aParams[6].nNumFiles++;
}

// The same for list 0.
void fn_80015334(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*)) {
    strcpy(lbl_80280DF8->aParams[0].aszName[lbl_80280DF8->aParams[0].nNumFiles], szName);
    lbl_80280DF8->aParams[0].apfnOpened[lbl_80280DF8->aParams[0].nNumFiles] = pfnOpened;
    lbl_80280DF8->aParams[0].apfnClosed[lbl_80280DF8->aParams[0].nNumFiles] = pfnClosed;
    lbl_80280DF8->aParams[0].nNumFiles++;
}

// The same for list 3.
void fn_800153CC(const char* szName, void (*pfnOpened)(void*), void (*pfnClosed)(void*)) {
    strcpy(lbl_80280DF8->aParams[3].aszName[lbl_80280DF8->aParams[3].nNumFiles], szName);
    lbl_80280DF8->aParams[3].apfnOpened[lbl_80280DF8->aParams[3].nNumFiles] = pfnOpened;
    lbl_80280DF8->aParams[3].apfnClosed[lbl_80280DF8->aParams[3].nNumFiles] = pfnClosed;
    lbl_80280DF8->aParams[3].nNumFiles++;
}

// Add loading file nFile (data/Load/Load<n>.gcb) to stream list 2.
void fn_80014544(int nFile) {
    char szName[0x40];   // size unknown: the frame allows 0x40..0x48 bytes

    sprintf(szName, lbl_80186C14, nFile);
    fn_80015030(szName, fn_80014E70, fn_80014E88);
}

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

// Free every block of the pool.
void fn_80015470(void) {
    BufferPoolBlock* pBlock;
    int i;

    lbl_80280E00->nNext = 0;
    lbl_80280E00->n4 = 0;
    pBlock = lbl_80280E00->aBlocks;
    for (i = 0; i < 20; i++) {
        pBlock->u1000 = 0;
        pBlock++;
    }
}

// Move the pool's nNext past the blocks in use.
void fn_800154F4(void) {
    BufferPoolBlock* pBlock;

    pBlock = &lbl_80280E00->aBlocks[lbl_80280E00->nNext];
    for (;;) {
        if (pBlock->u1000 == 0) {
            break;
        }
        pBlock++;
        lbl_80280E00->nNext++;
    }
    lbl_80280E00->n4++;
}

// Reset the renderer's state and free the buffer pool.
void fn_80015540(void) {
    lbl_801B8980.n0 = 3;
    lbl_801B8980.b4 = 1;
    lbl_801B8980.n8 = 6;
    lbl_801B8980.bC = 100;
    lbl_801B8980.bD = 0;
    lbl_801B8980.n10 = 4;
    lbl_801B8980.n14 = 5;
    lbl_801B8980.n18 = 1;
    lbl_801B8980.b1C = 0xFF;
    lbl_801B8980.b1D = 0;
    lbl_801B8980.u20 = 0x70;
    lbl_801B8980.nFC = 0;
    lbl_801B8980.n24 = 2;
    lbl_801B8980.f28 = 100.0f;
    lbl_801B8980.f2C = 2048.0f;
    // the original stores the colour as one word (-1)
    lbl_801B8980.a30[0] = 0xFF;
    lbl_801B8980.a30[1] = 0xFF;
    lbl_801B8980.a30[2] = 0xFF;
    lbl_801B8980.a30[3] = 0xFF;
    fn_8000ADC0(lbl_801B8980.m34);
    fn_8000ADC0(lbl_801B8980.m74);
    lbl_801B8980.p100 = NULL;
    lbl_801B8980.p104 = NULL;
    lbl_801B8980.u110 = 0;
    lbl_801B8980.uFlags = 0;
    GXSetCurrentMtx(0);
    fn_80015470();
}

// Set the alpha of TEV constant colour 0.
void fn_80016158(u8 nAlpha) {
    GXColor colour;

    // EA bug: only the alpha is set; r, g and b are whatever was on the stack
    colour.a = nAlpha;
    GXSetTevKColor(0, colour);
}

// Reset the view: whole-screen viewport, a 512 x 448 screen, scales of 1.
void fn_80016198(void) {
    fn_80016B6C(1.0f, 1.0f);
    fn_8001425C(0);
    fn_800141CC();
    lbl_80280E08->fF4 = 1.0f;
    lbl_80280E08->fF8 = 1.0f;
    fn_80016978(0.0f, 0.0f, 1.0f, 1.0f);
    fn_80016B54(512, 448, 1.0f, 1.0f);
    fn_80016208();
}

// Reset the view's matrices: an orthographic 0..1 projection and an identity view, whose depth
// scale depends on fn_800072E0.
void fn_80016208(void) {
    int n;

    C_MTXOrtho(lbl_80280E08->m0, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    PSMTXIdentity(lbl_80280E08->m40);
    n = 1 << fn_800072E0();
    lbl_80280E08->m40[2][2] = -((f32)n - 1.0f) / (f32)n;
}

// Draw to the whole screen.
void fn_80016948(void) {
    fn_80016978(0.0f, 0.0f, 1.0f, 1.0f);
}

// Work out the viewport in pixels from its corners and the screen size, and its matrix.
void fn_800169AC(void) {
    ViewState* pView = lbl_80280E08;

    pView->n70 = pView->fD4 * pView->nE4;
    pView->n74 = pView->fD8 * pView->nE8;
    pView->n78 = pView->fDC * pView->nE4;
    pView->n7C = pView->fE0 * pView->nE8;
    pView->m40[0][0] = pView->fF4;
    pView->m40[1][1] = pView->fF8;
    fn_8000ADC0(pView->m80);
    pView->m80[3][0] += pView->n70;
    pView->m80[3][1] += pView->n74;
    pView->m80[0][0] = pView->n78 * pView->fF4;
    pView->m80[1][1] = pView->n7C * pView->fF8;
    pView->m80[2][2] = 0.0f;
}

// Send a vertex to the GPU: its position and the view's colour.
void fn_800166E8(f32* pPos) {
    fn_80016C94(pPos[0], pPos[1], pPos[2]);
    fn_80016C7C(255.0f * lbl_80280E08->aColour[0], 255.0f * lbl_80280E08->aColour[1],
                255.0f * lbl_80280E08->aColour[2], 255.0f * lbl_80280E08->aColour[3]);
}

// Send a vertex with its own colour (0..1 per channel).
void fn_80016770(f32* pPos, f32* pColour) {
    fn_80016C94(pPos[0], pPos[1], pPos[2]);
    fn_80016C7C(255.0f * pColour[0], 255.0f * pColour[1], 255.0f * pColour[2], 255.0f * pColour[3]);
}

// Send a vertex with the view's colour and a texture coordinate.
void fn_80016800(f32* pPos, f32* pUV) {
    fn_80016C94(pPos[0], pPos[1], pPos[2]);
    fn_80016C7C(255.0f * lbl_80280E08->aColour[0], 255.0f * lbl_80280E08->aColour[1],
                255.0f * lbl_80280E08->aColour[2], 255.0f * lbl_80280E08->aColour[3]);
    fn_80016CA8(pUV[0], pUV[1]);
}

// Send a vertex with its own colour and a texture coordinate.
void fn_800168A0(f32* pPos, f32* pColour, f32* pUV) {
    fn_80016C94(pPos[0], pPos[1], pPos[2]);
    fn_80016C7C(255.0f * pColour[0], 255.0f * pColour[1], 255.0f * pColour[2], 255.0f * pColour[3]);
    fn_80016CA8(pUV[0], pUV[1]);
}

// Give the renderer the current camera's matrices, with rows 0 and 2 of the first negated.
void fn_80016B9C(void) {
    fn_8000A0E8(((Camera*)fn_8001614C())->m15C, lbl_801B8980.m34);
    fn_8000A0E8(((Camera*)fn_8001614C())->m9C, lbl_801B8980.m74);
    fn_80016C28(lbl_801B8980.m34[0], lbl_801B8980.m34[0]);
    fn_80016C28(lbl_801B8980.m34[2], lbl_801B8980.m34[2]);
    lbl_801B8980.u110 |= 0x100;
}

// Set the GX viewport from six values (GXGetViewportv's layout).
void fn_80016C44(f32* pViewport) {
    GXSetViewport(pViewport[0], pViewport[1], pViewport[2], pViewport[3], pViewport[4], pViewport[5]);
}

// Draw primitive 0xA1: each pair of vertices gives the opposite corners of a rectangle (x from
// one, y from the other), drawn as a 4-vertex strip (0x98). Colours and texture coordinates are
// spread over the corners the same way.
void fn_800162AC(f32* pPos, f32* pColour, f32* pUV, int nVerts) {
    f32 aPos[4][4];
    f32 aColour[4][4];
    f32 aUV[4][4];
    f32* pQuadColour;
    f32* pQuadUV;
    f32 fX0;
    f32 fX1;
    int i;

    for (i = 0; i < nVerts - 1; i += 2) {
        fX0 = pPos[i * 4];
        fX1 = pPos[i * 4 + 4];
        Vec_Copy(&pPos[i * 4], aPos[0]);
        Vec_Copy(&pPos[i * 4], aPos[1]);
        aPos[1][0] = fX1;
        Vec_Copy(&pPos[(i + 1) * 4], aPos[2]);
        aPos[2][0] = fX0;
        Vec_Copy(&pPos[(i + 1) * 4], aPos[3]);
        if (pColour != NULL) {
            Vec_Copy(&pColour[i * 4], aColour[0]);
            Vec_Copy(&pColour[i * 4], aColour[1]);
            Vec_Copy(&pColour[(i + 1) * 4], aColour[2]);
            Vec_Copy(&pColour[(i + 1) * 4], aColour[3]);
            pQuadColour = aColour[0];
        } else {
            pQuadColour = NULL;
        }
        if (pUV != NULL) {
            fX0 = pUV[i * 4];
            fX1 = pUV[i * 4 + 4];
            Vec_Copy(&pUV[i * 4], aUV[0]);
            Vec_Copy(&pUV[i * 4], aUV[1]);
            aUV[1][0] = fX1;
            Vec_Copy(&pUV[(i + 1) * 4], aUV[2]);
            aUV[2][0] = fX0;
            Vec_Copy(&pUV[(i + 1) * 4], aUV[3]);
            pQuadUV = aUV[0];
        } else {
            pQuadUV = NULL;
        }
        fn_8001644C(0x98, aPos[0], pQuadColour, pQuadUV, 4);
    }
}

// Draw nVerts vertices as primitive ePrim in the view (0xA1 goes through fn_800162AC). Each
// vertex takes four floats of pPos, and of pColour and pUV when given; without pColour the view's
// colour is used. Unless the view's nD0 is set, its viewport and matrices are used for the draw
// and the GX state is put back afterwards.
void fn_8001644C(int ePrim, f32* pPos, f32* pColour, f32* pUV, int nVerts) {
    f32 aProjection[7];
    f32 aViewport[6];
    ViewState* pView = lbl_80280E08;
    int i;

    if (ePrim == 0xA1) {
        fn_800162AC(pPos, pColour, pUV, nVerts);
        return;
    }
    if (pView->nD0 == 0) {
        GXGetProjectionv(aProjection);
        GXGetViewportv(aViewport);
        GXSetViewport(pView->n70, pView->n74, pView->n78, pView->n7C, 0.0f, 1.0f);
        GXSetProjection(pView->m0, 1);
        GXLoadPosMtxImm(pView->m40, 0);
        GXSetCurrentMtx(0);
    }
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(11, 1);
    if (pUV != NULL) {
        GXSetVtxDesc(13, 1);
    }
    GXSetVtxAttrFmt(7, 9, 1, 4, 0);
    GXSetVtxAttrFmt(7, 11, 1, 5, 0);
    if (pUV != NULL) {
        GXSetVtxAttrFmt(7, 13, 1, 4, 0);
    }
    fn_80012520(ePrim, 7, nVerts);
    if (pUV != NULL) {
        if (pColour != NULL) {
            for (i = 0; i < nVerts; i++) {
                fn_800168A0(pPos, pColour, pUV);
                pUV += 4;
                pColour += 4;
                pPos += 4;
            }
        } else {
            for (i = 0; i < nVerts; i++) {
                fn_80016800(pPos, pUV);
                pUV += 4;
                pPos += 4;
            }
        }
    } else if (pColour != NULL) {
        for (i = 0; i < nVerts; i++) {
            fn_80016770(pPos, pColour);
            pColour += 4;
            pPos += 4;
        }
    } else {
        for (i = 0; i < nVerts; i++) {
            fn_800166E8(pPos);
            pPos += 4;
        }
    }
    fn_800124A8();
    if (pView->nD0 == 0) {
        GXSetProjectionv(aProjection);
        fn_80016C44(aViewport);
        fn_80016B9C();
        fn_80012EF8();
    }
}

// Set the viewport's corners, as fractions of the screen.
void fn_80016978(f32 x0, f32 y0, f32 x1, f32 y1) {
    ViewState* pView = lbl_80280E08;

    pView->fD4 = x0;
    pView->fD8 = y0;
    pView->fDC = x1;
    pView->fE0 = y1;
    fn_800169AC();
}

void fn_80016B54(int nWidth, int nHeight, f32 fX, f32 fY) {
    ViewState* pView = lbl_80280E08;

    pView->nE4 = nWidth;
    pView->nE8 = nHeight;
    pView->fEC = fX;
    pView->fF0 = fY;
}

void fn_80016B6C(f32 fX, f32 fY) {
    lbl_80280E08->fF4 = fX;
    lbl_80280E08->fF8 = fY;
    fn_800169AC();
}

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

// Refill stream list 0 with the swing-animation files: the male and/or female sac file for each
// animation slot that has overlays, and every player's golfer's CharSac file.
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
    LibSlot* pSlot;
    int nPlayer;
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

// Make stream list 3 hold only the front-end character file for character nChar
// (data/FEChars/<nChar + 1>charfe.gcb), for the golfer the front end is loading.
void fn_80014DFC(s32 nChar, s32 nUnused) {   // port: FEgolferanim.c passes a second argument this ignores
    char szName[0x100];  // size unknown: the frame allows up to 0x100 bytes

    fn_80015454();
    sprintf(szName, lbl_80186CA8, nChar + 1);
    lbl_80281EE0->pB8->n14 = -1;
    fn_800153CC(szName, fn_80014E74, fn_80014E8C);
}
