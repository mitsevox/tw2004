// gomainloop.c (TW06's gomainloop.c, golf/mainloop/gomainloop.c; GO_vInitIG): the game's main
// loop: starting up and shutting down every system, the frame update and the render passes.
// gSession.nGameType picks the loop's work: 1 the start-up screens, 3 the front end, 6 a round,
// 13 leaving the game.

#include "game.h"
#include "psmgr.h"
#include "lighting.h"
#include "core/memcard.h"
#include "game/frontend.h"
#include "llpict.h"

// The other systems' start-up, shut-down and per-frame calls, from their files (most are not
// decompiled yet; the types are from the calls here).
u8   OSGetResetButtonState(void);
void fn_80007254(void);
void fn_80007260(void);
void fn_800080D0(void);
void fn_800080D4(void);
void fn_800081C4(void);
void fn_80008380(void);
void fn_800083A0(void);
void fn_8000989C(void);
void fn_80009918(void);
void fn_8000F060(void);
void fn_8000F0E8(void);
void fn_800103C0(void);
void fn_8001049C(void);
void fn_8001058C(void);
void UFont_Init(void);
void UFont_Shutdown(void);
void UFont_DrawQueue(void);
void fn_800136F4(void);
void fn_80013718(void);
void fn_800137D0(void* pCamera);
void fn_80013808(void* pCamera, int n, void** ppSlot);
void fn_80013D5C(void* pCamera);        // the current render camera (lbl_80280DF0)
void fn_800140E8(int a, int nWidth, int nHeight, int nField, int b, int c);
void fn_80014594(void);
void fn_800145E0(void);
void fn_8001462C(void);
void fn_80014668(void);
void fn_800146C4(void);
void fn_80014718(void);
void fn_800147D4(void);
void fn_80014804(void);
void fn_80014834(void);
void fn_80014864(void);
void fn_800148A8(void);
void fn_800148EC(void);
void fn_80014A64(void);
void fn_80015540(void);
void fn_80015620(void);
void fn_80016198(void);
void fn_800162A0(void);
void fn_800162A4(int nField);
void fn_80016CB8(void);
void fn_80016CD8(void);
void fn_80016E3C(int nView);
void fn_80016E90(int nView);
void fn_8001A488(void);
void fn_8001A920(void);
void fn_8001B878(Character* pChar, int nPlayer);
void fn_8001BA74(void);
void fn_8001BBD8(int n);
void fn_8001BC8C(f32 fFrameTime);
void fn_8001C254(void);
void fn_8001C2B4(void);
void fn_8001C304(void);
void fn_8001C350(void);
void fn_8001C37C(void);
void fn_8001C468(void);
void fn_8001C518(void);
void fn_8001D290(void);
void fn_8001D384(void);
void fn_8001D63C(void);
void fn_8001D6F0(void);
void fn_8001E7DC(void);
void fn_80029FC8(void);
void fn_8002A020(void);
void fn_8002E258(void);
void fn_8002E25C(void);
void fn_8002F180(void);
void fn_8002F32C(s32 nSurface);
void fn_8002F4FC(void);
void fn_8002F540(void);
void fn_80030254(void);
void fn_800329CC(void);
void fn_80032AEC(void);
void fn_80033744(void);
void fn_800349CC(int nView);
void fn_800350B4(f32 f);
void fn_800350D0(f32 f);
void fn_800350EC(u8 r, u8 g, u8 b);
void fn_800355E0(s32 nField);
void fn_80035E98(int nView);
void fn_80035F1C(void);
void fn_80037F80(void);
void fn_80038128(void);
void fn_800382E0(void);
void fn_80038968(void);
void fn_800389C0(void);
void fn_80039358(int nView);
void fn_80039FF8(void);
void fn_8003A074(void);
void fn_80045660(void);
void fn_80045848(int nPlayer);
void fn_80045D18(void);
void fn_80045D5C(void);
void fn_800461A8(void);
void fn_80046264(void);
void fn_800467B4(void);
void fn_80046828(s32 nView);
void fn_80046B8C(int nView);
void fn_800486EC(void);
void fn_800486F0(void);
void fn_80048DD0(void);
void fn_80048E7C(void);
void fn_8004950C(void);
void fn_80049510(void);
void fn_8004B1A4(void);
void fn_80055D3C(void);
void fn_80055D54(void);
void fn_80055D6C(void);
void fn_80055F14(void);
void fn_80055F18(void);
void fn_800563C4(void);
void fn_80056454(void);
void GOLFERSTATE_OpenONCE(void);
void Swing_Init(void);
void fn_80058DB4(void);
void fn_8005A0FC(int nPlayer);
void fn_8005A478(int nPlayer);
void fn_8005A7A0(int nPlayer);
void fn_8005A850(int nPlayer);
void fn_8005CD94(void);
void fn_8005D2E4(void);
void fn_8005D2F8(void);
void fn_8005D348(void);
void fn_8005D3A8(s8 nState);
void fn_80062E00(void);
void fn_80062E20(void);
int  fn_80063758(void);
void fn_80064E2C(void);
void fn_80064E74(void);
void fn_80065B44(void);
void fn_80067608(void);
void fn_8006765C(void);
void fn_80067B80(void);
void fn_80067CD4(int nPlayer);
void fn_8006795C(void);
void fn_8006E1C0(void);
void fn_8006E1C4(void);
void fn_8006E2A4(void);
void fn_8006E424(void);
void fn_8006F14C(void);
void fn_8006F150(void);
void fn_8006F608(void);
void fn_8006F64C(void);
void fn_8007185C(void);
void fn_80071890(void);
void fn_800718C4(void);
void fn_800757B8(void);
void fn_800763B4(void);
void fn_800763B8(void);
void VM_vInitModule(void);
void VM_vCloseModule(void);
void fn_80076E48(void);
void fn_800773F8(void);
void fn_80077428(void);
void fn_80085120(void);
void fn_8008B00C(void);
void fn_8008B864(void);
void fn_8008B9A0(void);
void fn_8008C844(void);
void fn_8008C8C4(void);
void fn_8008DC10(void);
u8   fn_8008EB10(void);
u8   fn_8008F39C(void);
void fn_8008F648(int n);
void fn_8008FD60(int n);
void fn_800905A8(void);
void fn_80090628(void);
void fn_80090664(void);
void fn_8009069C(void);
void fn_80090A60(void);
void fn_80091778(void);
void fn_80091870(void);
void fn_80093AD4(void);
void fn_80093D14(void);
void fn_80093D3C(void);
void fn_800940FC(void);
void fn_80095364(void);
void fn_80095550(void);
void CameraTuning_Init(void);
void fn_80097E98(void);
void fn_80098A98(void);
void fn_80098B5C(void);
void fn_80099344(f32 fFrameTime);
void fn_80099BA0(void);
void fn_8009A16C(void);
u8   fn_8009A180(void);
void fn_8009A1F4(void);
void fn_8009A928(int n);
void fn_8009A968(void);
void fn_8009A990(int nView);
void fn_8009A9CC(int nView);
void fn_8009B134(void);
void fn_8009B898(void);
void fn_8009BE08(int nView);
void fn_8009C914(int nView);
void fn_800A2064(void);
void fn_800A2E14(void);
void fn_800A3A84(void);
void fn_800A4C54(void);
void fn_800A4E34(void);
void fn_800A4FD8(void);
void fn_800A7A34(int a, int b, u8 c, int d);
void fn_800AE338(void);
void fn_800B1608(void);
void fn_800B1D78(void);
void fn_800B2724(void);
void fn_800B2734(void);
void fn_800B28D4(Character* pChar, int a, int b);
void fn_800B2FB0(Character* pChar, int a, int b);
void fn_800B34F0(void);
void fn_800B352C(void);
void fn_800B5C38(void);
void fn_800B5C3C(void);
void fn_800B655C(void);
void fn_800B6560(void);
void fn_800B9A88(void);
void fn_800B9AAC(void);
void fn_800B9B48(void);
void fn_800BA1A4(void);
void fn_800BA940(void);
void fn_800BAA4C(void);
void fn_800BAA50(int nPlayer);
void fn_800BAB80(int nPlayer);
void fn_800BB0E8(void);
void fn_800C808C(void);
void fn_800C8108(void);
void fn_800C9970(void);
u8   fn_800D3004(void);
void fn_800DAE44(void);
f32  fn_800DAF98(f32 fFrameTime);
void fn_800DBF34(void);
void fn_800DC664(int nPlayer);
void fn_800DCBA8(void);
void fn_800DCBB8(void);
void fn_800DCBBC(void);
void fn_800DCC04(void);
void fn_800DCC30(void);
void fn_800E2F14(void);
void fn_800E5314(void);
s32  fn_800FD6A4(int nPlayer);
void fn_801020BC(void);
void fn_801037F8(void);
void fn_80103A64(void);
void fn_8010A448(int nSize);
void fn_8010A4E8(void);
void fn_8010BF68(void);
void fn_8010F6C8(void);
void fn_8010F718(void);
void fn_8010F748(void);
void fn_8010F794(void);
void fn_8010FA00(void);
void fn_80110390(void);
void fn_8011407C(void);
void fn_8011E170(void);
void fn_8011E3B0(void);
void fn_8011E6E8(void);
void fn_8011E974(void);
void fn_8012402C(void);
s32  fn_80124094(void);
void fn_801242D0(void);
void EASBio_InitOnce(void);
void fn_80124B54(void);
void fn_801250C0(void);
void fn_80124C10(void);
void AI_TargetsInit(void);
void BreakLine_Update(int nView);
void FE_UpdateMovieQueue(void);
void fn_800DFC18(void);
void GM_Update(void);
void GR_vInit(void);
void Luck_ResetAllOdds2(void);
void Player_SetGolfer(int nPlayer, int nGolfer, int nController, u32 uBag, int bRightSide);
void Players_Reset(void);
void Players_SetupAll(void);
void fn_80037DD8(void);
void PsBallFx_InitModule(void);
void SH_vSetShadowIntensity(int n);
void UI_Obj_InitModule(void);
void UStream_CloseAll(void);
void UStream_Init(void);
void sFE_AdjustAndSetGolferPosition(void);

f32  fn_8006C630(void);
void fn_8006C63C(void);
void fn_8006C69C(void);
void fn_8006C6F0(void);
void fn_8006C720(void);
void fn_8006C770(void);
void fn_8006C7A8(void);
void fn_8006C854(void);
void fn_8006C8EC(int nView);
void fn_8006C968(void);
void fn_8006C9EC(void);
void fn_8006CB2C(void);
void GO_vInitIG(void);
void fn_8006CDC4(void);
void fn_8006CEFC(void);
void fn_8006CFC8(void);
u8   fn_8006D01C(void);
u8   fn_8006D1C0(int nView);
void fn_8006D27C(void);
void fn_8006D7E8(void);
void fn_8006D838(void);
void fn_8006D8E8(void);
void fn_8006DC20(f32 f);
u8   fn_8006DC34(void);
void fn_8006DC40(int nField);
void fn_8006DC44(void);
void fn_8006DC48(void);
void fn_8006DC4C(int n);
void fn_8006DC78(void);
void fn_8006DCA0(int n);
void fn_8006DCA4(int n);
void fn_8006DCA8(s32 nWidth, s32 nHeight, s32 nC, s32 nKind);
void fn_8006DD44(void);
void fn_8006DD84(void);
void fn_8006DDA8(void);
void fn_8006DDE8(s32 nRow);
void fn_8006DE28(void);
void fn_8006DE68(s32 nRow);
void fn_8006DEA8(void);
void fn_8006DEE8(s32 nRow);
void fn_8006DF28(void);
void fn_8006DF68(s32 nRow);
void fn_8006DFA8(void);
void fn_8006DFE8(s32 nRow);
void fn_8006E028(void);
void fn_8006E068(s32 nRow);
void fn_8006E0A8(void);
void fn_8006E0AC(void);
void fn_8006E0B0(void);
void fn_8006E0B4(void);
void fn_8006E0B8(void);
void fn_8006E0BC(void);
u8   fn_8006E0C0(void);
void fn_8006E0F8(void);

s32 lbl_801888D0[4] = {0, 1, 2, 3};     // the order the views are drawn in

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x802838C0), before the constants fn_8006C968 uses first; its body is unknown.
static f32 gomainloop_StrippedFn(f32 x) {
    return x + 1.0f;
}

f32 fn_8006C630(void) {
    return lbl_802811F0->f18;
}

// Run inside wait loops (memory card, AnimStream): the sound (fn_800A4BDC), the GBA cable
// (fn_8012402C, unless fn_80124094 is -1, 0x11 or 0x12) and the reset button (latched).
void fn_8006C63C(void) {
    fn_800A4BDC();
    if (fn_80124094() != -1 && fn_80124094() != 0x12 && fn_80124094() != 0x11) {
        fn_8012402C();
    }
    if (!lbl_80281B8E) {
        lbl_80281B8E = OSGetResetButtonState();
    }
    fn_80007254();
}

// Hands the frame parity (lbl_80281B88 & 1) to fn_8006DC40 and fn_800162A4 (both empty) and,
// outside start-up (game type 1), to fn_800355E0.
void fn_8006C69C(void) {
    fn_8006DC40(lbl_80281B88 & 1);
    fn_800162A4(lbl_80281B88 & 1);
    if (gSession.nGameType != 1) {
        fn_800355E0(lbl_80281B88 & 1);
    }
}

void fn_8006C6F0(void) {
    fn_8006C7A8();
    fn_80014864();
    fn_800148EC();
    fn_800148A8();
    fn_8006C854();
}

void fn_8006C720(void) {
    fn_8000989C();
    fn_8002E258();
    fn_80076E48();
    fn_800763B4();
    fn_80055D3C();
    fn_800563C4();
    fn_8005D2E4();
    GOLFERSTATE_OpenONCE();
    fn_800DCBA8();
    fn_800A2064();
    fn_8010F6C8();
    EASBio_InitOnce();
    fn_80009918();
}

void fn_8006C770(void) {
    fn_800DCBB8();
    fn_8005D348();
    fn_8005CD94();
    fn_800763B8();
    fn_80056454();
    fn_8002E25C();
    fn_8010F718();
}

// Starts the systems every mode needs; the session gets a new random seed.
void fn_8006C7A8(void) {
    u32 uSeed;

    fn_8006BF4C();
    fn_8000989C();
    fn_800B5C38();
    fn_800080D4();
    fn_8000B46C();
    UStream_Init();
    fn_800B655C();
    fn_8000F060();
    fn_8006DC44();
    fn_80016198();
    UFont_Init();
    fn_80015540();
    fn_8006E1C0();
    fn_8007185C();
    VM_vInitModule();
    fn_800136F4();
    fn_800103C0();
    fn_800952D8();
    fn_8001C37C();
    fn_8002F180();
    uSeed = fn_8000B244();
    gSession.nSeed = uSeed;
    fn_8000B2B8(uSeed);
    fn_80055F14();
    fn_80045D18();
    fn_800486EC();
    fn_8004950C();
    fn_8006E2A4();
    fn_8006F14C();
    fn_80093524();
    fn_800461A8();
    fn_8000C104();
    fn_8002F4FC();
    fn_800757B8();
}

// Shuts down what fn_8006C7A8 started.
void fn_8006C854(void) {
    fn_800A7A98(0);
    fn_8000C0F0();
    fn_80046264();
    fn_80093580();
    fn_8006F150();
    fn_8006E424();
    fn_80049510();
    fn_80045D5C();
    fn_800486F0();
    fn_80055F18();
    fn_8000B30C();
    fn_8001C468();
    fn_80095364();
    fn_8001049C();
    fn_80013718();
    VM_vCloseModule();
    fn_8006E1C4();
    fn_80015620();
    UFont_Shutdown();
    fn_800162A0();
    fn_8006DC48();
    fn_8000F0E8();
    fn_800B6560();
    UStream_CloseAll();
    fn_8000B63C();
    fn_80071890();
    fn_800081C4();
    fn_8002F540();
    fn_800B5C3C();
    fn_80009918();
}

// Makes view nView's camera the current render camera and applies it, then draws the full-screen
// quad (fn_8006DC4C) with flags 3 in a round for views 0 and 1 while fn_800642B0 is 0, else 1.
void fn_8006C8EC(int nView) {
    fn_80016CD8();
    fn_80013D5C(fn_80017004(nView));
    fn_8006DC78();
    fn_80035F1C();
    if (nView < 2 && !fn_800642B0() && gSession.nGameType == 6) {
        fn_8006DC4C(3);
        return;
    }
    fn_8006DC4C(1);
}

// Each frame's render set-up, in every game type, before its frame is drawn: a 512 x 448 screen.
void fn_8006C968(void) {
    // fake match: the colour goes through float locals; the original converts them at run time
    f32 fRG = 100.0f;
    f32 fB = 128.0f;

    fn_80012F34(1);
    fn_800140E8(0, 512, 448, lbl_80281B88 & 1, 8, 1);
    fn_800350D0(46.875f);
    fn_800350B4(560.25f);
    fn_800350EC(fRG, fRG, fB);
    fn_80012EF8();
}

// Starts the front end (game type 3).
void fn_8006C9EC(void) {
    int nView;

    fn_8006C7A8();
    fn_8006DCA8(384, 528, 4, 1);
    fn_80029FC8();
    fn_8009CC00();
    fn_800905A8();
    fn_801037F8();
    fn_8010A448(0x18000);
    lbl_80281E60 = CA_spCreateCamera();
    CameraTuning_Init();
    fn_80062E00();
    fn_80039FF8();
    lbl_80281E5C = fn_8006E1C8();
    lbl_80281E58 = VM_spCreateViewport();
    lbl_80281E54 = fn_8001371C(lbl_80281E60, lbl_80281E5C, lbl_80281E58);
    fn_80013D5C(lbl_80281E54);
    fn_80095504(2);
    fn_800953C8(2);
    fn_800A7A34(0, 0, 1, 0);
    fn_8000B884();
    fn_80124B54();
    fn_800773F8();
    fn_80014668();
    fn_8001C304();
    fn_800B9A88();
    fn_80014718();
    fn_800146C4();
    Player_SetGolfer(0, 0, 0, 0, 0);
    nView = gPlayers[0].nView[0];
    View_SetCamera(fn_80017028(nView), 0x17, 0, nView);
    fn_8005D3A8(0);
    GOLFERSTATE_Set(0, 0);
    fn_800A4E34();
    fn_800B9B48();
    fn_80037DD8();
    fn_8010F748();
    fn_8006DCA0(1);
}

// Shuts the front end down.
void fn_8006CB2C(void) {
    fn_8006DCA4(1);
    fn_800137B0(lbl_80281E54);
    fn_8010F794();
    fn_8008B00C();
    fn_80077428();
    VM_vReleaseViewport(lbl_80281E58);
    fn_8006E214(lbl_80281E5C);
    CA_vDestroyCamera(lbl_80281E60);
    fn_80062E20();
    fn_8003A074();
    fn_80097E98();
    fn_8001C350();
    fn_8001C518();
    Players_Reset();
    fn_80095444(2);
    fn_80103A64();
    fn_8010A4E8();
    fn_80090664();
    fn_8000B8F4();
    fn_801250C0();
    fn_80016E3C(0);
    fn_8009CC88();
    fn_80037F80();
    fn_800B9AAC();
    fn_801020BC();
    fn_800A7A98(1);
    fn_8002A020();
    fn_8006DD44();
    fn_8006C854();
}

// Starts a round (game type 6). TW06: GO_vInitIG ("in game").
void GO_vInitIG(void) {
    fn_8006C7A8();
    fn_8006DCA8(256, 224, 2, 4);
    fn_80029FC8();
    fn_8010A448(0x6000);
    SH_vSetShadowIntensity(0);
    fn_800905A8();
    Session_SetupProfiles();
    fn_8001C254();
    fn_80055D54();
    lbl_80281E60 = CA_spCreateCamera();
    lbl_80281E5C = fn_8006E1C8();
    lbl_80281E58 = VM_spCreateViewport();
    lbl_80281E54 = fn_8001371C(lbl_80281E60, lbl_80281E5C, lbl_80281E58);
    fn_80013D5C(lbl_80281E54);
    fn_80030254();
    fn_8004B1A4();
    fn_800A7A34(1, 0, (gSession.nSplitScreen != 0) + 1, 0);
    fn_8006F608();
    fn_80014594();
    fn_80085120();
    fn_8005D3A8(1);
    fn_800DCBBC();
    fn_80067608();
    CameraTuning_Init();
    fn_80062E00();
    fn_80039FF8();
    fn_80064E2C();
    fn_800B34F0();
    fn_80048DD0();
    fn_8006DCA0(0);
    fn_80014A64();
    fn_80091778();
    fn_8001462C();
    fn_800145E0();
    fn_80045660();
    fn_80067B80();
    AI_TargetsInit();
    UI_Obj_InitModule();
    fn_80016CB8();
    Players_SetupAll();
    fn_800DCC04();
    fn_8001A920();
    if (!gSession.nSplitScreen) {
        fn_8009A928(1);
    } else {
        fn_8009A928(2);
    }
    fn_800C808C();
    BFX_vInit();
    fn_800BA940();
    fn_800A2934();
    fn_80098A98();
    fn_8009CC00();
    fn_80093D3C();
    GR_vInit();
    fn_8006F4B4();
    fn_8009005C("ingame");
    fn_8006DC20(1.0f);
    Swing_Init();
    fn_8006BED4();
    fn_800DAE44();
    PsBallFx_InitModule();
    fn_8011E170();
    fn_80095504(1);
    fn_800953C8(1);
    if (gSession.nC == 3) {
        fn_800E0B38(5);
        fn_800EAF7C();
        gSession.nC = 0;
    }
    fn_800E2F14();
    fn_800E5314();
    Luck_ResetAllOdds2();
    fn_8011407C();
    fn_80065B44();
}

// Shuts a round down.
void fn_8006CDC4(void) {
    fn_800137B0(lbl_80281E54);
    VM_vReleaseViewport(lbl_80281E58);
    fn_8006E214(lbl_80281E5C);
    CA_vDestroyCamera(lbl_80281E60);
    fn_80091870();
    fn_8011E3B0();
    fn_800A2E14();
    fn_80093D14();
    fn_800C8108();
    if (fn_80095430(1)) {
        fn_80095444(1);
    }
    fn_8006765C();
    fn_8006DCA4(0);
    Players_Reset();
    fn_8009B898();
    fn_800940FC();
    fn_8009CC88();
    if (gSession.nSplitScreen) {
        fn_80016E3C(0);
        fn_80016E3C(1);
    } else {
        fn_80016E3C(0);
        fn_80016E3C(2);
    }
    fn_800AE338();
    fn_80098B5C();
    fn_800A2958();
    fn_80055D6C();
    fn_800BAA4C();
    fn_8009A968();
    fn_8006DD84();
    fn_800B2734();
    fn_8006BF20();
    fn_80058DB4();
    fn_80090664();
    fn_8001C2B4();
    fn_8001058C();
    fn_80048E7C();
    fn_800B352C();
    fn_80062E20();
    fn_8003A074();
    fn_80064E74();
    fn_80097E98();
    fn_800DCC30();
    fn_8006F64C();
    fn_800306B8();
    fn_8010A4E8();
    gSession.nSplitScreen = 0;
    fn_8002A020();
    fn_8006DD44();
    fn_8006C854();
    fn_8009A1F4();
}

// Starts the start-up screens (game type 1).
void fn_8006CEFC(void) {
    int nView;

    fn_8006C7A8();
    fn_8009CC00();
    fn_800905A8();
    lbl_80281E60 = CA_spCreateCamera();
    lbl_80281E5C = fn_8006E1C8();
    lbl_80281E58 = VM_spCreateViewport();
    lbl_80281E54 = fn_8001371C(lbl_80281E60, lbl_80281E5C, lbl_80281E58);
    fn_80013D5C(lbl_80281E54);
    fn_800B1D78();
    fn_800147D4();
    fn_80014834();
    fn_80014804();
    Player_SetGolfer(0, 0, 0, 0, 0);
    fn_8006DCA8(0, 0, 0, 4);
    fn_80037DD8();
    nView = gPlayers[0].nView[0];
    View_SetCamera(fn_80017028(nView), 0x19, 0, nView);
    fn_8010F748();
    fn_8010FF9C();
}

// Shuts the start-up screens down.
void fn_8006CFC8(void) {
    fn_80090664();
    fn_8010F794();
    fn_800137B0(lbl_80281E54);
    VM_vReleaseViewport(lbl_80281E58);
    fn_8006E214(lbl_80281E5C);
    fn_80016E3C(0);
    fn_8009CC88();
    fn_80037F80();
    fn_8006DD44();
    fn_8006C854();
}

// Whether the main loop should end this frame, by the game type's own tests: gSession's b12 and
// nC, lbl_802811E8[1], the pads, fn_8009A180 and fn_8008F39C.
u8 fn_8006D01C(void) {
    u8 bDone = 0;

    if (gSession.nGameType == 6 && (gSession.b12 || gSession.nC == 2)) {
        bDone = 1;
        gSession.b12 = 0;
    } else if (gSession.nGameType == 6 && fn_8006DC34()) {
        bDone = 1;
    } else if (gSession.nGameType == 6 && !gSession.a8[0] && (gSession.uFlags & 0x4000)) {
        if (fn_80014300(0)) {
            fn_8009A16C();
        }
        if (fn_8009A180()) {
            bDone = 1;
        }
    } else if (gSession.nGameType == 6 && gSession.a8[0]) {
        if (fn_80014300(0)) {
            bDone = 1;
        }
        if ((gSession.uFlags & 0x4000) && fn_8009A180()) {
            bDone = 1;
        }
    } else if (gSession.nGameType == 3) {
        if (gSession.nC == 2) {
            bDone = 1;
        }
        if (fn_8008F39C()) {
            bDone = 1;
        }
    } else if (gSession.nGameType == 1 && (gSession.b12 || gSession.nC == 2)) {
        fn_800BA74C(1);
        bDone = 1;
        gSession.b12 = 0;
    }
    return bDone;
}

// Whether view nView's golfer is drawn normally: in mode 9 except while placing the ball (state
// 22); never while paused; in modes 7 and 8 not when nC3C has bit 0x1 or 0x02000000.
u8 fn_8006D1C0(int nView) {
    int nPlayer = fn_8001707C(nView);

    if (Game_GetMode() == 9) {
        return (s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_PLACE_BALL;
    }
    if (gSession.nPaused) {
        return 0;
    }
    if ((Game_GetMode() == 7 || Game_GetMode() == 8)
        && ((gPlayers[nPlayer].nC3C & 1) || (gPlayers[nPlayer].nC3C & 0x02000000))) {
        return 0;
    }
    return 1;
}

// A round's frame: the world, then each active view (lbl_801888D0 gives their order) with its
// golfers and effects, then the screen-wide passes.
void fn_8006D27C(void) {
    int  i;
    s32  nView;
    s8   nState;
    int  nPlayer;

    fn_8006E028();
    fn_800A2BA8();
    if (gSession.nSplitScreen && fn_800170A0(0) && fn_800170A0(1) && fn_800D3004()) {
        gSession.b11 = 1;
    } else {
        gSession.b11 = 0;
    }
    fn_80093AD4();
    fn_8001BC8C(gSession.fFrameTime);
    fn_80033744();
    fn_800467B4();
    fn_80099344(gSession.fFrameTime);
    fn_800B2724();
    for (i = 0; i < 4; i++) {
        nView = lbl_801888D0[i];
        if (!fn_800170A0(nView)) continue;
        nState = GOLFERSTATE_GetCurrentState(fn_8001707C(nView));
        fn_8006DFA8();
        fn_8006C8EC(nView);
        fn_80016E90(nView);
        if (nView < 2) {
            fn_800A2BBC(nView);
            if (!gSession.nSplitScreen && !gSession.b11 && nView < 2) {
                fn_8011E6E8();
            }
            fn_800349CC(nView);
            if (nState != GS_GREEN_MORPH) {
                fn_8009BE08(nView);
                fn_8009C914(nView);
            }
            if (nState != GS_GREEN_MORPH) {
                BreakLine_Update(nView);
            }
            fn_80046828(nView);
            if (nState != GS_GREEN_MORPH) {
                fn_80046B8C(nView);
            }
            fn_80017004(nView);
            fn_80099BA0();
            fn_800A3A84();
            fn_8006DF28();
            fn_80045848(fn_8001707C(nView));
            if (Game_GetMode() == 7 || Game_GetMode() == 8) {
                if ((u8)fn_800FD6A4(fn_8001707C(nView))) {
                    fn_800BAB80(fn_8001707C(nView));
                    fn_800BAA50(fn_8001707C(nView));
                }
            }
        }
        if (!fn_8006D1C0(nView) || nState == GS_GREEN_MORPH) {
            gPlayers[fn_8001707C(nView)].pChar->u10 |= 1;
        }
        for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
            fn_8001B878(gPlayers[nPlayer].pChar, nPlayer);
        }
        fn_8001D384();
        if (nView < 2) {
            fn_8005A478(fn_8001707C(nView));
        }
        fn_8006DEA8();
        fn_8001BA74();
        fn_80035E98(nView);
        if (!gSession.b11 && nView < 2) {
            for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
                fn_8001B878(gPlayers[nPlayer].pChar, nPlayer);
                if (fn_8001EE88(gPlayers[nPlayer].pChar) != 2 && nPlayer != fn_80063758()
                    && !(gPlayers[nPlayer].pChar->u10 & 0x40) && !(gPlayers[nPlayer].pChar->u10 & 1)) {
                    fn_800B28D4(gPlayers[nPlayer].pChar, 0, 0);
                }
            }
        }
        if (nView >= 2) {
            fn_8001BBD8(4);
        } else {
            fn_8001BBD8(0);
        }
        if (!gSession.b11 && nView < 2) {
            for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
                fn_8001B878(gPlayers[nPlayer].pChar, nPlayer);
                if (fn_8001EE88(gPlayers[nPlayer].pChar) != 2 && nPlayer != fn_80063758()
                    && !(gPlayers[nPlayer].pChar->u10 & 0x40) && !(gPlayers[nPlayer].pChar->u10 & 1)) {
                    fn_800B2FB0(gPlayers[nPlayer].pChar, 0, 0);
                }
            }
            if (!gSession.nSplitScreen) {
                fn_8001D290();
            }
        }
        fn_8006DF28();
        fn_800A2C08(nView);
        fn_8006DEA8();
        if (nView < 2) {
            fn_8005A0FC(fn_8001707C(nView));
            fn_8005A850(fn_8001707C(nView));
        }
        if (!fn_8006D1C0(nView) || nState == GS_GREEN_MORPH) {
            gPlayers[fn_8001707C(nView)].pChar->u10 &= ~1;
        }
        if (!gSession.nSplitScreen && !gSession.b11 && nView < 2) {
            fn_8011E974();
        }
        fn_80039358(nView);
        if (nView < 2) {
            fn_800329CC();
        }
        if (nView < 2) {
            fn_80032AEC();
        }
        fn_8006DF28();
        if (nView < 2) {
            fn_80067CD4(fn_8001707C(nView));
        }
        fn_8006DEA8();
        if (nView < 2 && (!fn_80035574() || (fn_80035574() && fn_8006E0C0()))) {
            fn_8009A990(nView);
            fn_8009A9CC(nView);
        }
        fn_8006DF28();
        if (nView < 2) {
            fn_80035574();
        }
        fn_8006DEA8();
        if (nView < 2) {
            fn_8005A7A0(fn_8001707C(nView));
        }
        fn_8006DE28();
    }
    fn_80013D5C(lbl_80281E54);
    fn_8006DC78();
    fn_80035F1C();
    fn_80038968();
    if (gSession.nSplitScreen && fn_800170A0(0) && fn_800170A0(1)) {
        fn_80038128();
    }
    fn_8008F648(1);
    UFont_DrawQueue();
    fn_800DC664(fn_8001707C(0));
    fn_800DBF34();
    fn_800389C0();
    fn_8009069C();
    fn_800382E0();
    fn_8008FD60(1);
    fn_800BA1A4();
    fn_8006DDA8();
}

// The start-up screens' frame.
void fn_8006D7E8(void) {
    if (fn_800170A0(0)) {
        fn_8006C8EC(0);
        fn_8008F648(1);
    }
    UFont_DrawQueue();
    fn_8008FD60(1);
    fn_8009069C();
    fn_800382E0();
}

// The front end's frame.
void fn_8006D838(void) {
    u8 b;

    fn_8006E028();
    fn_8006DFA8();
    fn_800A2BA8();
    fn_8008B864();
    if (fn_800170A0(0)) {
        fn_8006C8EC(0);
        b = fn_8008EB10();
        if (b) {
            sFE_AdjustAndSetGolferPosition();
            fn_8008B9A0();
            fn_8008C844();
        }
        if (lbl_80281E50) {
            fn_8008F648(1);
        }
        if (b) {
            fn_8008C8C4();
        }
    }
    UFont_DrawQueue();
    fn_800BA1A4();
    fn_8009069C();
    fn_8008FD60(1);
    fn_800382E0();
    FE_UpdateMovieQueue();
    fn_8006DE28();
    fn_8006DDA8();
    fn_801242D0();
}

// The main loop, until fn_8006D01C says stop. Each frame's time is taken from watch 1
// (fn_800954A4), at most four frames (a bad reading counts as four); outside start-up it counts
// the frames and sends event 0x1A once a second; then it runs the game type's frame.
void fn_8006D8E8(void) {
    u64 uLast = fn_800954A4(1);
    u64 uNow;
    f32 fTime;
    f64 dTime;

    fn_800954A4(0);
    while (!fn_8006D01C()) {
        fn_80110390();
        fn_800B7490();
        if (gSession.nGameType == 6) {
            fn_8001E7DC();
        }
        fn_8006C69C();
        fn_80006EDC();
        uNow = fn_800954A4(1);
        // fake match: the double step gives the original's frsp (EA's gomainloop.c saw a double
        // return; every other file saw the float fn_8006E118 returns).
        fTime = dTime = gSession.fFrameTime = fn_8006E118(uNow, uLast);
        if (fTime > 4.0f * FRAME_TIME || fTime < 0.0f) {
            gSession.fFrameTime = 4.0f * FRAME_TIME;
        }
        fn_800954A4(0);
        fn_80013400();
        if (gSession.nGameType == 6) {
            fn_800DFC18();
        }
        fn_80090628();
        gSession.fFrameTime = fn_800DAF98(gSession.fFrameTime);
        if (gSession.nGameType != 1 && fn_800C6D9C()) {
            gSession.fFrameTime = 0.0f;
        }
        if (gSession.nGameType != 1) {
            gSession.n20 += (u32)(16.0f * (FRAME_RATE * gSession.fFrameTime));
            uLast = uNow;
            gSession.nFrameCount = (u32)gSession.n20 >> 4;
            gSession.f1C += gSession.fFrameTime;
            if (gSession.f1C >= 1.0f) {
                EVENT_Trigger(0xFF, 0x1A, NULL, -1);
                gSession.f1C = 0.0f;
            }
        }
        fn_800080D0();
        fn_800A4BDC();
        if (gSession.nGameType != 1) {
            fn_80095550();
        }
        if (gSession.nGameType == 6) {
            fn_8001A488();
        }
        if (gSession.nGameType == 1) {
            fn_800B1608();
        } else {
            fn_8005D2F8();
        }
        if (gSession.nGameType != 1) {
            GM_Update();
        }
        fn_800C9970();
        fn_80013D5C(lbl_80281E54);
        fn_800718C4();
        fn_8006C968();
        fn_8006DC78();
        fn_80035F1C();
        if (gSession.nGameType != 3) {
            lbl_80281E50 = 0;
        }
        switch (gSession.nGameType) {
        case 3:
            fn_8006D838();
            fn_80124C10();
            lbl_80281E50 = 1;
            break;
        case 6:
            fn_8006D27C();
            fn_800BB0E8();
            fn_8006795C();
            break;
        case 1:
            fn_8006D7E8();
            break;
        }
        fn_80008380();
        fn_80006FE8();
        if (gSession.nGameType != 1) {
            if (gSession.nGameType != 3) {
                fn_8001D6F0();
            } else {
                fn_8001D63C();
                fn_8008DC10();
                fn_8010FA00();
            }
        }
        fn_80090A60();
        fn_8002FEAC();
        fn_80007260();
        fn_800A4C54();
        if (!gSession.nSplitScreen && gSession.nGameType != 1) {
            fn_8006E0F8();
        }
        fn_800083A0();
        if (gSession.nGameType == 13) {
            if (gSession.uFlags & 0x1000) {
                fn_800A4FD8();
                fn_8010BF68();
            }
            return;
        }
    }
}

// The frame of the current game type.
void fn_8006DBD4(void) {
    if (gSession.nGameType == 1) {
        fn_8006D7E8();
        return;
    }
    if (gSession.nGameType == 6) {
        fn_8006D27C();
        return;
    }
    fn_8006D838();
}

void fn_8006DC20(f32 f) {
    if (lbl_80281F1C != NULL) {
        lbl_80281F1C->f18 = f;
    }
}

u8 fn_8006DC34(void) {
    return lbl_802811E8[1];
}

void fn_8006DC40(int nField) {
}

void fn_8006DC44(void) {
}

void fn_8006DC48(void) {
}

void fn_8006DC4C(int n) {
    fn_80013808(*lbl_80280DF0, n, lbl_80280DF0);
}

void fn_8006DC78(void) {
    fn_800137D0(*lbl_80280DF0);
}

void fn_8006DCA0(int n) {
}

void fn_8006DCA4(int n) {
}

// Sets up the screen copy (lbl_80281100): with a size, render surface 1 of that size and kind
// becomes its buffer; with none, it has no buffer.
void fn_8006DCA8(s32 nWidth, s32 nHeight, s32 nC, s32 nKind) {
    lbl_80281100->nWidth = nWidth;
    lbl_80281100->nHeight = nHeight;
    lbl_80281100->nC = nC;
    if (nWidth == 0 && nHeight == 0) {
        lbl_80281100->nSize = 0;
        lbl_80281100->pPixels = NULL;
        return;
    }
    fn_8002F260(2, lbl_80281100->nWidth, lbl_80281100->nHeight, nKind, 8, 1);
    lbl_80281100->nSize = fn_8002F454(1);
    lbl_80281100->pPixels = lbl_801D3950[1].pBuffer;
}

// Frees the screen copy's surface, if it has one.
void fn_8006DD44(void) {
    if (lbl_80281100->nWidth != 0 && lbl_80281100->nHeight != 0) {
        fn_8002F32C(1);
    }
}

void fn_8006DD84(void) {
    fn_800E58B4(3);
}

// The six points of a frame at which every module's hooks run (ModuleHooks, engine.h).
void fn_8006DDA8(void) {
    s32 i;

    fn_8006E0A8();
    i = 0;
    do {
        fn_8006DDE8(i);
        i++;
    } while (i < 20);
}

void fn_8006DDE8(s32 nRow) {
    if (lbl_80188E88[nRow].pfn10 != NULL) {
        lbl_80188E88[nRow].pfn10();
    }
}

void fn_8006DE28(void) {
    s32 i;

    fn_8006E0AC();
    i = 0;
    do {
        fn_8006DE68(i);
        i++;
    } while (i < 20);
}

void fn_8006DE68(s32 nRow) {
    if (lbl_80188E88[nRow].pfn18 != NULL) {
        lbl_80188E88[nRow].pfn18();
    }
}

void fn_8006DEA8(void) {
    s32 i;

    fn_8006E0B0();
    i = 0;
    do {
        fn_8006DEE8(i);
        i++;
    } while (i < 20);
}

void fn_8006DEE8(s32 nRow) {
    if (lbl_80188E88[nRow].pfn20 != NULL) {
        lbl_80188E88[nRow].pfn20();
    }
}

void fn_8006DF28(void) {
    s32 i;

    fn_8006E0B4();
    i = 0;
    do {
        fn_8006DF68(i);
        i++;
    } while (i < 20);
}

void fn_8006DF68(s32 nRow) {
    if (lbl_80188E88[nRow].pfn1C != NULL) {
        lbl_80188E88[nRow].pfn1C();
    }
}

void fn_8006DFA8(void) {
    s32 i;

    fn_8006E0B8();
    i = 0;
    do {
        fn_8006DFE8(i);
        i++;
    } while (i < 20);
}

void fn_8006DFE8(s32 nRow) {
    if (lbl_80188E88[nRow].pfn14 != NULL) {
        lbl_80188E88[nRow].pfn14();
    }
}

void fn_8006E028(void) {
    s32 i;

    fn_8006E0BC();
    i = 0;
    do {
        fn_8006E068(i);
        i++;
    } while (i < 20);
}

void fn_8006E068(s32 nRow) {
    if (lbl_80188E88[nRow].pfnC != NULL) {
        lbl_80188E88[nRow].pfnC();
    }
}

void fn_8006E0A8(void) {
}

void fn_8006E0AC(void) {
}

void fn_8006E0B0(void) {
}

void fn_8006E0B4(void) {
}

void fn_8006E0B8(void) {
}

void fn_8006E0BC(void) {
}

// 1 when gSession.options.nC is 2 and lbl_802811F0's n10 equals its n0C, else 0.
u8 fn_8006E0C0(void) {
    int b = 0;

    if (gSession.options.nC == 2 && lbl_802811F0->n10 == lbl_802811F0->n0C) {
        b = 1;
    }
    return b;
}

void fn_8006E0F8(void) {
    fn_8009B134();
}

// Seconds between two time-base readings (40.5 million ticks a second).
f32 fn_8006E118(u64 uNow, u64 uLast) {
    return (1.0f / 40500000.0f) * (s32)(uNow - uLast);
}
