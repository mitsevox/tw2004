// streammanagerhole.c (our name, after TW06's golf/streaming/streammanagerhole.cpp; the 2003 game
// is C): loads a hole's stream files, registering and unregistering every system's stream handlers
// around each load. Not yet decompiled; the code below is the matched small functions.

#include "ustream.h"

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
s32 fn_800918A4();
s32 fn_8009198C(s32);
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
void fn_80014E68(void);
void fn_80014E6C(void);
void fn_80014E70(void);
void fn_80014E74(void);
void fn_80014E78(void);
void fn_80014E7C(void);
void fn_80014E80(void);
void fn_80014E84(void);
void fn_80014E88(void);
void fn_80014E8C(void);
void fn_80014E90(void);
void fn_80014E94(void);
extern s32 lbl_80280DF8;
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
extern u8* gpGame;
s32 fn_80015464(void);
void fn_80015620(void);
void GXSetTexCoordGen2();
void fn_80016124(s32 p0, s32 p1, s32 p2, s32 p3);
extern u8* lbl_80280DF0;
s32 fn_8001614C(void);
extern u8* lbl_80280DC8;
void fn_8001618C(u8 v);
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

void fn_80014E68(void) {
}

void fn_80014E6C(void) {
}

void fn_80014E70(void) {
}

void fn_80014E74(void) {
}

void fn_80014E78(void) {
}

void fn_80014E7C(void) {
}

void fn_80014E80(void) {
}

void fn_80014E84(void) {
}

void fn_80014E88(void) {
}

void fn_80014E8C(void) {
}

void fn_80014E90(void) {
}

void fn_80014E94(void) {
}

void fn_800150B8(void) {
    UStream_Close(*(s32*)(((u8*)lbl_80280DF8) + 0x119C));
}

void fn_800150E0(void) {
    s32 t0;
    t0 = UStream_Open(lbl_80280DF8);
    *(s32*)(((u8*)lbl_80280DF8) + 0x119C) = t0;
}

void fn_8001510C(void) {
    UStream_Close(*(s32*)(((u8*)lbl_80280DF8) + 0x119C));
}

void fn_80015134(void) {
    s32 t0;
    t0 = UStream_Open((lbl_80280DF8 + 644));
    *(s32*)(((u8*)lbl_80280DF8) + 0x119C) = t0;
}

void fn_80015164(void) {
    UStream_Close(*(s32*)(((u8*)lbl_80280DF8) + 0x119C));
}

void fn_8001518C(void) {
    s32 t0;
    t0 = UStream_Open((lbl_80280DF8 + 1288));
    *(s32*)(((u8*)lbl_80280DF8) + 0x119C) = t0;
}

void fn_800151BC(void) {
    UStream_Close(*(s32*)(((u8*)lbl_80280DF8) + 0x119C));
}

void fn_800151E4(void) {
    s32 t0;
    t0 = UStream_Open((lbl_80280DF8 + 3220));
    *(s32*)(((u8*)lbl_80280DF8) + 0x119C) = t0;
}

void fn_80015214(void) {
    s32 t0;
    t0 = UStream_Open((lbl_80280DF8 + 2576));
    *(s32*)(((u8*)lbl_80280DF8) + 0x119C) = t0;
}

void fn_80015244(void) {
    UStream_Close(*(s32*)(((u8*)lbl_80280DF8) + 0x119C));
}

void fn_8001526C(void) {
    s32 t0;
    t0 = UStream_Open((lbl_80280DF8 + 3864));
    *(s32*)(((u8*)lbl_80280DF8) + 0x119C) = t0;
}

void fn_80015324(void) {
    *(s32*)(((u8*)lbl_80280DF8) + 0xF18) = 0;
}

void fn_800153BC(void) {
    *(s32*)((u8*)lbl_80280DF8) = 0;
}

void fn_80015454(void) {
    *(s32*)(((u8*)lbl_80280DF8) + 0x78C) = 0;
}

s32 fn_80015464(void) {
    return *(s32*)(gpGame + 0x60);
}

void fn_80015620(void) {
}

void fn_80016124(s32 p0, s32 p1, s32 p2, s32 p3) {
    GXSetTexCoordGen2(p0, p1, p2, p3, 0, 125);
}

s32 fn_8001614C(void) {
    return *(s32*)(lbl_80280DF0 + 0x0);
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
