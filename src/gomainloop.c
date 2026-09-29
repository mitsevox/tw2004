// gomainloop.c (TW06's gomainloop.c, golf/mainloop/gomainloop.c; GO_vInitIG): the game's main
// loop: starting up and shutting down every system, the frame update and the render passes.
// gSession.nGameType picks the loop's work: 1 the start-up screens, 3 the front end, 6 a round,
// 13 leaving the game.

#include "game.h"
#include "psmgr.h"
#include "lighting.h"
#include "core/memcard.h"
#include "core/gbacable.h"
#include "game/frontend.h"
#include "frontend/fe.h"
#include "llpict.h"

void*       lbl_80281E60;
GoFrameBuf* lbl_80281E5C;
void*       lbl_80281E58;
void*       lbl_80281E54;
u8          lbl_80281E50;

// The other systems' start-up, shut-down and per-frame calls, from their files (most are not
// decompiled yet; the types are from the calls here).
void fn_80007254(void);
void fn_80007260(void);
void fn_800080D0(void);
void fn_800080D4(void);
void fn_800081C4(void);
void fn_80008380(void);
void fn_800083A0(void);
void StaticMem_Reset(void);
void StaticMem_Checkpoint(void);
void fn_8000F060(void);
void fn_8000F0E8(void);
void fn_800103C0(void);
void fn_8001049C(void);
void fn_8001058C(void);
void FO_vInitModule(void);
void FO_vCloseModule(void);
void UFont_DrawQueue(void);
void fn_800136F4(void);
void fn_80013718(void);
void fn_800137D0(void* pCamera);
void fn_80013808(void* pCamera, int n, void** ppSlot);
void RC_vSetCurrentRenderCtx(void* pCamera);        // the current render camera (lbl_80280DF0)
void RenderState_SetRenderSurface(int a, int nWidth, int nHeight, int nField, int b, int c);
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
void DS_vInitModule(void);
void DS_vCloseModule(void);
void fn_80016198(void);
void fn_800162A0(void);
void fn_800162A4(int nField);
void ViewController_ResetAll(void);
void ViewController_SetCurrentViewController(int nView);
void ViewController_Delete(int nView);
void ViewController_Update(int nView);
void CharacterTex_TextureLoader(void);
void Character_PostInit(void);
void Character_ClipTest(Character* pChar, int nPlayer);
void Character_PreRenderAll(void);
void Character_RenderAll(int n);
void Character_UpdateAll(f32 fFrameTime);
void Character_InitIG(void);
void Character_CloseIG(void);
void Character_InitFE(void);
void Character_CloseFE(void);
void Legacy_Character_InitModule(void);
void Legacy_Character_CloseModule(void);
void Character_FreeFEGolfers(void);
void SkeletalObject_RenderShadowsAll(void);
void SkeletalObject_ClipTestAll(void);
void Character_UpdateClothesFE(void);
void Character_UpdateClothesIG(void);
void Character_ResetTimeScalesOnCombo(void);
void fn_80029FC8(void);
void fn_8002A020(void);
void fn_8002E258(void);
void fn_8002E25C(void);
void fn_8002F180(void);
void fn_8002F32C(s32 nSurface);
void Pict_InitModule(void);
void Pict_CloseModule(void);
void fn_80030254(void);
void fn_800329CC(void);
void fn_80032AEC(void);
void fn_80033744(void);
void fn_800349CC(int nView);
void fn_800350B4(f32 f);
void fn_800350D0(f32 f);
void fn_800350EC(u8 r, u8 g, u8 b);
void fn_800355E0(s32 nField);
void SKN_DrawBoneTris(int nView);
void RC_ApplyCurrentViewport(void);
void fn_80037F80(void);
void fn_80038128(void);
void fn_800382E0(void);
void fn_80038968(void);
void fn_800389C0(void);
void fn_80039358(int nView);
void DynamicCam_Init(void);
void DynamicCam_DeInit(void);
void fn_80045660(void);
void fn_80045848(int nPlayer);
void fn_80045D18(void);
void fn_80045D5C(void);
void DynObj_InitModule(void);
void DynObj_CloseModule(void);
void DynObj_UpdateDynamicObjects(void);
void DynObj_RenderDynamicObjects(s32 nView);
void DynObj_RenderBalls(int nView);
void DynObj_InitModuleEmpty(void);
void DynObj_CloseModuleEmpty(void);
void fn_80048DD0(void);
void fn_80048E7C(void);
void fn_8004950C(void);
void fn_80049510(void);
void Ter_Init(void);
void fn_80055D3C(void);
void fn_80055D54(void);
void fn_80055D6C(void);
void Wind_vInitModule(void);
void Wind_vCloseModule(void);
void fn_800563C4(void);
void fn_80056454(void);
void GOLFERSTATE_OpenONCE(void);
void SW_vInitModule(void);
void SW_vCloseModule(void);
void SW_vUIUpdateBlurBuffer(int nPlayer);
void SW_vUIUpdateIK(int nPlayer);
void SW_vUIRender2D(int nPlayer);
void SW_vUIRender3D(int nPlayer);
void GOLFERSTATE_CloseONCE(void);
void fn_8005D2E4(void);
void fn_8005D2F8(void);
void fn_8005D348(void);
void fn_8005D3A8(s8 nState);
void fn_80062E00(void);
void fn_80062E20(void);
int  CameraController_GetClippedShadow(void);
void StaticCam_Init(void);
void StaticCam_DeInit(void);
void EVENT_InitForGame(void);
void SitDev_vInitModule(void);
void SitDev_vCloseModule(void);
void TARGET_Init(void);
void fn_80067CD4(int nPlayer);
void SitDev_ProcessEventQueue(void);
void FB_vInitModule(void);
void FB_vCloseModule(void);
void fn_8006E2A4(void);
void fn_8006E424(void);
void fn_8006F14C(void);
void fn_8006F150(void);
void fn_8006F608(void);
void fn_8006F64C(void);
void fn_8007185C(void);
void fn_80071890(void);
void fn_800718C4(void);
void LLVideo_InitModule(void);
void fn_800763B4(void);
void fn_800763B8(void);
void VM_vInitModule(void);
void VM_vCloseModule(void);
void FE_vOpenONCE(void);
void FE_vInitModule(void);
void FE_vCloseModule(void);
void IG_InitGameMessages(void);
void FE_CharMgrClose(void);
void FE_StreamUpdateState(void);
void FE_SetupCamera(void);
void FE_vRenderGolferAllPhase1(void);
void FE_vRenderGolferAllPhase2(void);
void FE_vFreeUnusedCharacters(void);
u8   FE_IsGolferRenderAllowed(void);
u8   UI_IsClosed(void);
void UI_vInitModule(void);
void UI_SendPendingMessages(void);
void UI_vCloseModule(void);
void UI_ExitFade(void);
void UI_FreeMarkedEntryPictures(void);
void UI_LoadLoadingBarTexture(void);
void UI_FreeLoadingBarTexture(void);
void fn_80093AD4(void);
void fn_80093D14(void);
void BS_vInit(void);
void BS_vClose(void);
void TI_vCloseModule(void);
void fn_80095550(void);
void CameraTuning_Init(void);
void fn_80097E98(void);
void fn_80098A98(void);
void fn_80098B5C(void);
void fn_80099344(f32 fFrameTime);
void fn_80099BA0(void);
void fn_8009A16C(void);
u8   fn_8009A180(void);
void DEMO_Restore(void);
void GLW_vInitModule(int n);
void GLW_vCloseModule(void);
void GLW_vUpdateGlows(int nView);
void GLW_vRenderGlows(int nView);
void fn_8009B134(void);
void fn_8009B898(void);
void fn_8009BE08(int nView);
void fn_8009C914(int nView);
void fn_800A2064(void);
void fn_800A2E14(void);
void fn_800A3A84(void);
void Gaud_Monitor(void);
void Gaud_InitFE(void);
void Gaud_ExitFE(void);
void Aud_InitSession(int a, int b, u8 c, int d);
void UI_Obj_CloseModule(void);
void Startup_Update(void);
void Startup_InitGameMessages(void);
void fn_800B2724(void);
void fn_800B2734(void);
void fn_800B28D4(Character* pChar, int a, int b);
void fn_800B2FB0(Character* pChar, int a, int b);
void ComicCam_InitComicCam(void);
void ComicCam_CloseComicCam(void);
void AudMem_InitModule(void);
void AudMem_CloseModule(void);
void fn_800B655C(void);
void fn_800B6560(void);
void UI_EATraxDraw(void);
void fn_800BA940(void);
void fn_800BAA4C(void);
void fn_800BAA50(int nPlayer);
void fn_800BAB80(int nPlayer);
void SitDev_ThrowBallHitDelayedEvent(void);
void BreakLine_InitModule(void);
void fn_800C8108(void);
void AnimStream_Update(void);
u8   fn_800D3004(void);
void GameEffects_InitGameEffectSettings(void);
f32  GameEffects_AdjustTimeRate(f32 fFrameTime);
void GameEffects_RenderGameBreakerEffects(void);
void GameEffects_UpdateGameEffects(int nPlayer);
void GM_vInitModuleONCE(void);
void GM_vCloseModuleONCE(void);
void GM_InitModule_PreDataStream(void);
void GM_InitModule_PostDataStream(void);
void GM_DeInitModule(void);
void GM_SetupDefaultProfile(void);
void GUI_ClearControllersPulled(void);
s32  SpeedGolf_IsRunning(int nPlayer);
void GameMode4_CloseFE(void);
void FE_CrAP_InitModule(void);
void FE_CrAP_CloseModule(void);
void fn_8010A448(int nSize);
void fn_8010A4E8(void);
void fn_8010BF68(void);
void FE_LogoDesign_OpenOnce(void);
void FE_LogoDesign_CloseOnce(void);
void FE_LogoDesign_InitModule(void);
void FE_LogoDesign_CloseModule(void);
void FE_LogoDesign_UploadCustomLogo(void);
void fn_80110390(void);
void fn_8011407C(void);
void fn_8011E170(void);
void fn_8011E3B0(void);
void fn_8011E6E8(void);
void fn_8011E974(void);
void EASBio_InitOnce(void);
void fn_80124B54(void);
void fn_801250C0(void);
void fn_80124C10(void);
void AI_TargetsInit(void);
void BreakLine_Update(int nView);
void FE_movieFade(void);
void GM_CheckControllerPulled(void);
void GM_Update(void);
void GR_vInit(void);
void Luck_InitIG(void);
void Player_SetGolfer(int nPlayer, int nGolfer, int nController, u32 uBag, int bRightSide);
void Players_Reset(void);
void Players_SetupAll(void);
void fn_80037DD8(void);
void PsBallFx_InitModule(void);
void fn_800B251C_ShadowInit(int n);
void UI_Obj_InitModule(void);
void UStream_CloseAll(void);
void UStream_Init(void);
void FE_vUpdateGolferAll(void);

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
void GO_vInitFE(void);
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

// Run inside wait loops (memory card, AnimStream): the sound (Gaud_Cycle), the GBA cable
// (Gba_PollLink, unless Gba_GetState is -1, 0x11 or 0x12) and the reset button (latched).
void fn_8006C63C(void) {
    Gaud_Cycle();
    if (Gba_GetState() != -1 && Gba_GetState() != 0x12 && Gba_GetState() != 0x11) {
        Gba_PollLink();
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
    StaticMem_Reset();
    fn_8002E258();
    FE_vOpenONCE();
    fn_800763B4();
    fn_80055D3C();
    fn_800563C4();
    fn_8005D2E4();
    GOLFERSTATE_OpenONCE();
    GM_vInitModuleONCE();
    fn_800A2064();
    FE_LogoDesign_OpenOnce();
    EASBio_InitOnce();
    StaticMem_Checkpoint();
}

void fn_8006C770(void) {
    GM_vCloseModuleONCE();
    fn_8005D348();
    GOLFERSTATE_CloseONCE();
    fn_800763B8();
    fn_80056454();
    fn_8002E25C();
    FE_LogoDesign_CloseOnce();
}

// Starts the systems every mode needs; the session gets a new random seed.
void fn_8006C7A8(void) {
    u32 uSeed;

    REPLAY_Init();
    StaticMem_Reset();
    AudMem_InitModule();
    fn_800080D4();
    fn_8000B46C();
    UStream_Init();
    fn_800B655C();
    fn_8000F060();
    fn_8006DC44();
    fn_80016198();
    FO_vInitModule();
    DS_vInitModule();
    FB_vInitModule();
    fn_8007185C();
    VM_vInitModule();
    fn_800136F4();
    fn_800103C0();
    TI_vInitModule();
    Legacy_Character_InitModule();
    fn_8002F180();
    uSeed = Misc_CreateRandomSeed();
    gSession.nSeed = uSeed;
    Misc_InitModule(uSeed);
    Wind_vInitModule();
    fn_80045D18();
    DynObj_InitModuleEmpty();
    fn_8004950C();
    fn_8006E2A4();
    fn_8006F14C();
    fn_80093524();
    DynObj_InitModule();
    Network_InitModule();
    Pict_InitModule();
    LLVideo_InitModule();
}

// Shuts down what fn_8006C7A8 started.
void fn_8006C854(void) {
    Aud_ExitSession(0);
    Network_CloseModule();
    DynObj_CloseModule();
    fn_80093580();
    fn_8006F150();
    fn_8006E424();
    fn_80049510();
    fn_80045D5C();
    DynObj_CloseModuleEmpty();
    Wind_vCloseModule();
    Misc_CloseModule();
    Legacy_Character_CloseModule();
    TI_vCloseModule();
    fn_8001049C();
    fn_80013718();
    VM_vCloseModule();
    FB_vCloseModule();
    DS_vCloseModule();
    FO_vCloseModule();
    fn_800162A0();
    fn_8006DC48();
    fn_8000F0E8();
    fn_800B6560();
    UStream_CloseAll();
    fn_8000B63C();
    fn_80071890();
    fn_800081C4();
    Pict_CloseModule();
    AudMem_CloseModule();
    StaticMem_Checkpoint();
}

// Makes view nView's camera the current render camera and applies it, then draws the full-screen
// quad (fn_8006DC4C) with flags 3 in a round for views 0 and 1 while
// CameraController_bDontClearFrameBuffer is 0, else 1.
void fn_8006C8EC(int nView) {
    ViewController_SetCurrentViewController(nView);
    RC_vSetCurrentRenderCtx(ViewController_GetRenderContext(nView));
    fn_8006DC78();
    RC_ApplyCurrentViewport();
    if (nView < 2 && !CameraController_bDontClearFrameBuffer() && gSession.nGameType == 6) {
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

    DS_vEnableZBufferUpdate(1);
    RenderState_SetRenderSurface(0, 512, 448, lbl_80281B88 & 1, 8, 1);
    fn_800350D0(46.875f);
    fn_800350B4(560.25f);
    fn_800350EC(fRG, fRG, fB);
    RenderState_Flush();
}

// Starts the front end (game type 3).
void GO_vInitFE(void) {
    int nView;

    fn_8006C7A8();
    fn_8006DCA8(384, 528, 4, 1);
    fn_80029FC8();
    fn_8009CC00();
    UI_vInitModule();
    FE_CrAP_InitModule();
    fn_8010A448(0x18000);
    lbl_80281E60 = CA_spCreateCamera();
    CameraTuning_Init();
    fn_80062E00();
    DynamicCam_Init();
    lbl_80281E5C = FB_spCreateFrameBuffer();
    lbl_80281E58 = VM_spCreateViewport();
    lbl_80281E54 = RC_spCreateRenderCtx(lbl_80281E60, lbl_80281E5C, lbl_80281E58);
    RC_vSetCurrentRenderCtx(lbl_80281E54);
    TI_vResetCounter(2);
    TI_vStartCounter(2);
    Aud_InitSession(0, 0, 1, 0);
    fn_8000B884();
    fn_80124B54();
    FE_vInitModule();
    fn_80014668();
    Character_InitFE();
    FE_CrAPBall_Init();
    fn_80014718();
    fn_800146C4();
    Player_SetGolfer(0, 0, 0, 0, 0);
    nView = gPlayers[0].nView[0];
    CameraController_SetCameraMode(ViewController_GetCameraControl(nView), 0x17, 0, nView);
    fn_8005D3A8(0);
    GOLFERSTATE_Set(0, 0);
    Gaud_InitFE();
    FE_CrAPBall_MakeObjects();
    fn_80037DD8();
    FE_LogoDesign_InitModule();
    fn_8006DCA0(1);
}

// Shuts the front end down.
void fn_8006CB2C(void) {
    fn_8006DCA4(1);
    RC_vReleaseRenderCtx(lbl_80281E54);
    FE_LogoDesign_CloseModule();
    FE_CharMgrClose();
    FE_vCloseModule();
    VM_vReleaseViewport(lbl_80281E58);
    FB_vReleaseFrameBuffer(lbl_80281E5C);
    CA_vReleaseCamera(lbl_80281E60);
    fn_80062E20();
    DynamicCam_DeInit();
    fn_80097E98();
    Character_CloseFE();
    Character_FreeFEGolfers();
    Players_Reset();
    TI_sStopCounter(2);
    FE_CrAP_CloseModule();
    fn_8010A4E8();
    UI_vCloseModule();
    fn_8000B8F4();
    fn_801250C0();
    ViewController_Delete(0);
    fn_8009CC88();
    fn_80037F80();
    FE_CrAPBall_Free();
    GameMode4_CloseFE();
    Aud_ExitSession(1);
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
    fn_800B251C_ShadowInit(0);
    UI_vInitModule();
    Session_SetupProfiles();
    Character_InitIG();
    fn_80055D54();
    lbl_80281E60 = CA_spCreateCamera();
    lbl_80281E5C = FB_spCreateFrameBuffer();
    lbl_80281E58 = VM_spCreateViewport();
    lbl_80281E54 = RC_spCreateRenderCtx(lbl_80281E60, lbl_80281E5C, lbl_80281E58);
    RC_vSetCurrentRenderCtx(lbl_80281E54);
    fn_80030254();
    Ter_Init();
    Aud_InitSession(1, 0, (gSession.nSplitScreen != 0) + 1, 0);
    fn_8006F608();
    fn_80014594();
    IG_InitGameMessages();
    fn_8005D3A8(1);
    GM_InitModule_PreDataStream();
    SitDev_vInitModule();
    CameraTuning_Init();
    fn_80062E00();
    DynamicCam_Init();
    StaticCam_Init();
    ComicCam_InitComicCam();
    fn_80048DD0();
    fn_8006DCA0(0);
    fn_80014A64();
    UI_LoadLoadingBarTexture();
    fn_8001462C();
    fn_800145E0();
    fn_80045660();
    TARGET_Init();
    AI_TargetsInit();
    UI_Obj_InitModule();
    ViewController_ResetAll();
    Players_SetupAll();
    GM_InitModule_PostDataStream();
    Character_PostInit();
    if (!gSession.nSplitScreen) {
        GLW_vInitModule(1);
    } else {
        GLW_vInitModule(2);
    }
    BreakLine_InitModule();
    BFX_vInit();
    fn_800BA940();
    fn_800A2934();
    fn_80098A98();
    fn_8009CC00();
    BS_vInit();
    GR_vInit();
    fn_8006F4B4();
    UI_OpenInterface("ingame");
    fn_8006DC20(1.0f);
    SW_vInitModule();
    REPLAY_InitModule();
    GameEffects_InitGameEffectSettings();
    PsBallFx_InitModule();
    fn_8011E170();
    TI_vResetCounter(1);
    TI_vStartCounter(1);
    if (gSession.nC == 3) {
        GM_SetModeType(5);
        PlayNow_StartChallenge();
        gSession.nC = 0;
    }
    GM_SetupDefaultProfile();
    GUI_ClearControllersPulled();
    Luck_InitIG();
    fn_8011407C();
    EVENT_InitForGame();
}

// Shuts a round down.
void fn_8006CDC4(void) {
    RC_vReleaseRenderCtx(lbl_80281E54);
    VM_vReleaseViewport(lbl_80281E58);
    FB_vReleaseFrameBuffer(lbl_80281E5C);
    CA_vReleaseCamera(lbl_80281E60);
    UI_FreeLoadingBarTexture();
    fn_8011E3B0();
    fn_800A2E14();
    fn_80093D14();
    fn_800C8108();
    if (TI_bCounterIsRunning(1)) {
        TI_sStopCounter(1);
    }
    SitDev_vCloseModule();
    fn_8006DCA4(0);
    Players_Reset();
    fn_8009B898();
    BS_vClose();
    fn_8009CC88();
    if (gSession.nSplitScreen) {
        ViewController_Delete(0);
        ViewController_Delete(1);
    } else {
        ViewController_Delete(0);
        ViewController_Delete(2);
    }
    UI_Obj_CloseModule();
    fn_80098B5C();
    fn_800A2958();
    fn_80055D6C();
    fn_800BAA4C();
    GLW_vCloseModule();
    fn_8006DD84();
    fn_800B2734();
    REPLAY_CloseModule();
    SW_vCloseModule();
    UI_vCloseModule();
    Character_CloseIG();
    fn_8001058C();
    fn_80048E7C();
    ComicCam_CloseComicCam();
    fn_80062E20();
    DynamicCam_DeInit();
    StaticCam_DeInit();
    fn_80097E98();
    GM_DeInitModule();
    fn_8006F64C();
    fn_800306B8();
    fn_8010A4E8();
    gSession.nSplitScreen = 0;
    fn_8002A020();
    fn_8006DD44();
    fn_8006C854();
    DEMO_Restore();
}

// Starts the start-up screens (game type 1).
void fn_8006CEFC(void) {
    int nView;

    fn_8006C7A8();
    fn_8009CC00();
    UI_vInitModule();
    lbl_80281E60 = CA_spCreateCamera();
    lbl_80281E5C = FB_spCreateFrameBuffer();
    lbl_80281E58 = VM_spCreateViewport();
    lbl_80281E54 = RC_spCreateRenderCtx(lbl_80281E60, lbl_80281E5C, lbl_80281E58);
    RC_vSetCurrentRenderCtx(lbl_80281E54);
    Startup_InitGameMessages();
    fn_800147D4();
    fn_80014834();
    fn_80014804();
    Player_SetGolfer(0, 0, 0, 0, 0);
    fn_8006DCA8(0, 0, 0, 4);
    fn_80037DD8();
    nView = gPlayers[0].nView[0];
    CameraController_SetCameraMode(ViewController_GetCameraControl(nView), 0x19, 0, nView);
    FE_LogoDesign_InitModule();
    fn_8010FF9C();
}

// Shuts the start-up screens down.
void fn_8006CFC8(void) {
    UI_vCloseModule();
    FE_LogoDesign_CloseModule();
    RC_vReleaseRenderCtx(lbl_80281E54);
    VM_vReleaseViewport(lbl_80281E58);
    FB_vReleaseFrameBuffer(lbl_80281E5C);
    ViewController_Delete(0);
    fn_8009CC88();
    fn_80037F80();
    fn_8006DD44();
    fn_8006C854();
}

// Whether the main loop should end this frame, by the game type's own tests: gSession's bEndLoop
// and nC, lbl_802811E8[1], the pads, fn_8009A180 and UI_IsClosed.
u8 fn_8006D01C(void) {
    u8 bDone = 0;

    if (gSession.nGameType == 6 && (gSession.bEndLoop || gSession.nC == 2)) {
        bDone = 1;
        gSession.bEndLoop = 0;
    } else if (gSession.nGameType == 6 && fn_8006DC34()) {
        bDone = 1;
    } else if (gSession.nGameType == 6 && !gSession.bDemo && (gSession.uFlags & 0x4000)) {
        if (Controller_AnyPadHasButtons(0)) {
            fn_8009A16C();
        }
        if (fn_8009A180()) {
            bDone = 1;
        }
    } else if (gSession.nGameType == 6 && gSession.bDemo) {
        if (Controller_AnyPadHasButtons(0)) {
            bDone = 1;
        }
        if ((gSession.uFlags & 0x4000) && fn_8009A180()) {
            bDone = 1;
        }
    } else if (gSession.nGameType == 3) {
        if (gSession.nC == 2) {
            bDone = 1;
        }
        if (UI_IsClosed()) {
            bDone = 1;
        }
    } else if (gSession.nGameType == 1 && (gSession.bEndLoop || gSession.nC == 2)) {
        fn_800BA74C(1);
        bDone = 1;
        gSession.bEndLoop = 0;
    }
    return bDone;
}

// Whether view nView's golfer is drawn normally: in mode 9 except while placing the ball (state
// 22); never while paused; in modes 7 and 8 not when nSGFlags has bit 0x1 or 0x02000000.
u8 fn_8006D1C0(int nView) {
    int nPlayer = ViewController_GetActivePlayerNumber(nView);

    if (Game_GetMode() == 9) {
        return (s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_PLACE_BALL;
    }
    if (gSession.nPaused) {
        return 0;
    }
    if ((Game_GetMode() == 7 || Game_GetMode() == 8)
        && ((gPlayers[nPlayer].nSGFlags & 1) || (gPlayers[nPlayer].nSGFlags & 0x02000000))) {
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
    if (gSession.nSplitScreen && ViewController_IsActive(0) && ViewController_IsActive(1) && fn_800D3004()) {
        gSession.b11 = 1;
    } else {
        gSession.b11 = 0;
    }
    fn_80093AD4();
    Character_UpdateAll(gSession.fFrameTime);
    fn_80033744();
    DynObj_UpdateDynamicObjects();
    fn_80099344(gSession.fFrameTime);
    fn_800B2724();
    for (i = 0; i < 4; i++) {
        nView = lbl_801888D0[i];
        if (!ViewController_IsActive(nView)) continue;
        nState = GOLFERSTATE_GetCurrentState(ViewController_GetActivePlayerNumber(nView));
        fn_8006DFA8();
        fn_8006C8EC(nView);
        ViewController_Update(nView);
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
            DynObj_RenderDynamicObjects(nView);
            if (nState != GS_GREEN_MORPH) {
                DynObj_RenderBalls(nView);
            }
            ViewController_GetRenderContext(nView);
            fn_80099BA0();
            fn_800A3A84();
            fn_8006DF28();
            fn_80045848(ViewController_GetActivePlayerNumber(nView));
            if (Game_GetMode() == 7 || Game_GetMode() == 8) {
                if ((u8)SpeedGolf_IsRunning(ViewController_GetActivePlayerNumber(nView))) {
                    fn_800BAB80(ViewController_GetActivePlayerNumber(nView));
                    fn_800BAA50(ViewController_GetActivePlayerNumber(nView));
                }
            }
        }
        if (!fn_8006D1C0(nView) || nState == GS_GREEN_MORPH) {
            gPlayers[ViewController_GetActivePlayerNumber(nView)].pChar->uCharFlags |= 1;
        }
        for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
            Character_ClipTest(gPlayers[nPlayer].pChar, nPlayer);
        }
        SkeletalObject_ClipTestAll();
        if (nView < 2) {
            SW_vUIUpdateIK(ViewController_GetActivePlayerNumber(nView));
        }
        fn_8006DEA8();
        Character_PreRenderAll();
        SKN_DrawBoneTris(nView);
        if (!gSession.b11 && nView < 2) {
            for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
                Character_ClipTest(gPlayers[nPlayer].pChar, nPlayer);
                if (Character_GetShadowClipResult(gPlayers[nPlayer].pChar) != 2 && nPlayer
                    != CameraController_GetClippedShadow()
                    && !(gPlayers[nPlayer].pChar->uCharFlags & 0x40)
                    && !(gPlayers[nPlayer].pChar->uCharFlags & 1)) {
                    fn_800B28D4(gPlayers[nPlayer].pChar, 0, 0);
                }
            }
        }
        if (nView >= 2) {
            Character_RenderAll(4);
        } else {
            Character_RenderAll(0);
        }
        if (!gSession.b11 && nView < 2) {
            for (nPlayer = 0; nPlayer < gSession.nNumPlayers; nPlayer++) {
                Character_ClipTest(gPlayers[nPlayer].pChar, nPlayer);
                if (Character_GetShadowClipResult(gPlayers[nPlayer].pChar) != 2 && nPlayer
                    != CameraController_GetClippedShadow()
                    && !(gPlayers[nPlayer].pChar->uCharFlags & 0x40)
                    && !(gPlayers[nPlayer].pChar->uCharFlags & 1)) {
                    fn_800B2FB0(gPlayers[nPlayer].pChar, 0, 0);
                }
            }
            if (!gSession.nSplitScreen) {
                SkeletalObject_RenderShadowsAll();
            }
        }
        fn_8006DF28();
        fn_800A2C08(nView);
        fn_8006DEA8();
        if (nView < 2) {
            SW_vUIUpdateBlurBuffer(ViewController_GetActivePlayerNumber(nView));
            SW_vUIRender3D(ViewController_GetActivePlayerNumber(nView));
        }
        if (!fn_8006D1C0(nView) || nState == GS_GREEN_MORPH) {
            gPlayers[ViewController_GetActivePlayerNumber(nView)].pChar->uCharFlags &= ~1;
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
            fn_80067CD4(ViewController_GetActivePlayerNumber(nView));
        }
        fn_8006DEA8();
        if (nView < 2 && (!fn_80035574() || (fn_80035574() && fn_8006E0C0()))) {
            GLW_vUpdateGlows(nView);
            GLW_vRenderGlows(nView);
        }
        fn_8006DF28();
        if (nView < 2) {
            fn_80035574();
        }
        fn_8006DEA8();
        if (nView < 2) {
            SW_vUIRender2D(ViewController_GetActivePlayerNumber(nView));
        }
        fn_8006DE28();
    }
    RC_vSetCurrentRenderCtx(lbl_80281E54);
    fn_8006DC78();
    RC_ApplyCurrentViewport();
    fn_80038968();
    if (gSession.nSplitScreen && ViewController_IsActive(0) && ViewController_IsActive(1)) {
        fn_80038128();
    }
    UI_DrawInterface(1);
    UFont_DrawQueue();
    GameEffects_UpdateGameEffects(ViewController_GetActivePlayerNumber(0));
    GameEffects_RenderGameBreakerEffects();
    fn_800389C0();
    UI_ExitFade();
    fn_800382E0();
    UI_UpdateInterface(1);
    UI_EATraxDraw();
    fn_8006DDA8();
}

// The start-up screens' frame.
void fn_8006D7E8(void) {
    if (ViewController_IsActive(0)) {
        fn_8006C8EC(0);
        UI_DrawInterface(1);
    }
    UFont_DrawQueue();
    UI_UpdateInterface(1);
    UI_ExitFade();
    fn_800382E0();
}

// The front end's frame.
void fn_8006D838(void) {
    u8 b;

    fn_8006E028();
    fn_8006DFA8();
    fn_800A2BA8();
    FE_StreamUpdateState();
    if (ViewController_IsActive(0)) {
        fn_8006C8EC(0);
        b = FE_IsGolferRenderAllowed();
        if (b) {
            FE_vUpdateGolferAll();
            FE_SetupCamera();
            FE_vRenderGolferAllPhase1();
        }
        if (lbl_80281E50) {
            UI_DrawInterface(1);
        }
        if (b) {
            FE_vRenderGolferAllPhase2();
        }
    }
    UFont_DrawQueue();
    UI_EATraxDraw();
    UI_ExitFade();
    UI_UpdateInterface(1);
    fn_800382E0();
    FE_movieFade();
    fn_8006DE28();
    fn_8006DDA8();
    Gba_UpdateLinkState();
}

// The main loop, until fn_8006D01C says stop. Each frame's time is taken from watch 1
// (TI_sReadCounter), at most four frames (a bad reading counts as four); outside start-up it counts
// the frames and sends event 0x1A once a second; then it runs the game type's frame.
void fn_8006D8E8(void) {
    u64 uLast = TI_sReadCounter(1);
    u64 uNow;
    f32 fTime;
    f64 dTime;

    TI_sReadCounter(0);
    while (!fn_8006D01C()) {
        fn_80110390();
        fn_800B7490();
        if (gSession.nGameType == 6) {
            Character_ResetTimeScalesOnCombo();
        }
        fn_8006C69C();
        fn_80006EDC();
        uNow = TI_sReadCounter(1);
        // fake match: the double step gives the original's frsp (EA's gomainloop.c saw a double
        // return; every other file saw the float fn_8006E118 returns).
        fTime = dTime = gSession.fFrameTime = fn_8006E118(uNow, uLast);
        if (fTime > 4.0f * FRAME_TIME || fTime < 0.0f) {
            gSession.fFrameTime = 4.0f * FRAME_TIME;
        }
        TI_sReadCounter(0);
        Input_vUpdate();
        if (gSession.nGameType == 6) {
            GM_CheckControllerPulled();
        }
        UI_SendPendingMessages();
        gSession.fFrameTime = GameEffects_AdjustTimeRate(gSession.fFrameTime);
        if (gSession.nGameType != 1 && GolfCamera_IsFreezeTimeActive()) {
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
        Gaud_Cycle();
        if (gSession.nGameType != 1) {
            fn_80095550();
        }
        if (gSession.nGameType == 6) {
            CharacterTex_TextureLoader();
        }
        if (gSession.nGameType == 1) {
            Startup_Update();
        } else {
            fn_8005D2F8();
        }
        if (gSession.nGameType != 1) {
            GM_Update();
        }
        AnimStream_Update();
        RC_vSetCurrentRenderCtx(lbl_80281E54);
        fn_800718C4();
        fn_8006C968();
        fn_8006DC78();
        RC_ApplyCurrentViewport();
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
            SitDev_ThrowBallHitDelayedEvent();
            SitDev_ProcessEventQueue();
            break;
        case 1:
            fn_8006D7E8();
            break;
        }
        fn_80008380();
        fn_80006FE8();
        if (gSession.nGameType != 1) {
            if (gSession.nGameType != 3) {
                Character_UpdateClothesIG();
            } else {
                Character_UpdateClothesFE();
                FE_vFreeUnusedCharacters();
                FE_LogoDesign_UploadCustomLogo();
            }
        }
        UI_FreeMarkedEntryPictures();
        Pict_AfterFree();
        fn_80007260();
        Gaud_Monitor();
        if (!gSession.nSplitScreen && gSession.nGameType != 1) {
            fn_8006E0F8();
        }
        fn_800083A0();
        if (gSession.nGameType == 13) {
            if (gSession.uFlags & 0x1000) {
                Gaud_ExitFE();
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
    if (gpFrontEnd != NULL) {
        gpFrontEnd->f18 = f;
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
    GameMsg_Send(3);
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

// 1 when gSession.options.nWeather is 2 and lbl_802811F0's n10 equals its n0C, else 0.
u8 fn_8006E0C0(void) {
    int b = 0;

    if (gSession.options.nWeather == 2 && lbl_802811F0->n10 == lbl_802811F0->n0C) {
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
