// stateFunc.c (EA's name: TW07's stateFunc.c holds the same STATEFUNC_* callbacks, 16 of them
// paired in the same order from STATEFUNC_ShotSetupInit to STATEFUNC_PlaceBallUpdate): split off
// Swing.c at 0x8005D44C. Its .sdata2 starts on the 8-aligned address after the padding at
// 0x8028367C..0x80283680 and its .sbss after Code8005D2E4.c's at 0x80281E0A..0x80281E10.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "dynobj.h"

// ---- golfer states: the small ones----------------------------------------------------------------
// sGolferStateEngineTable is a table of 27 (enter, update, exit) callbacks; the current state is the
// top of the player's SwingStack. Most of these drive the camera, HUD and events around the swing.

typedef struct Vec4 { f32 x, y, z, w; } Vec4;

// Defined here, last address first (CodeWarrior lays out .sbss in reverse).
u8            lbl_80281E13;
u8            lbl_80281E12;
u8            lbl_80281E11;
u8            lbl_80281E10;

void  SW_vUIBlurReset(int nPlayer);
void  SW_vImpact(int nPlayer);
void  SW_UpdateVibration(int nPlayer);
void  fn_80098C70(void);
void  GOLFERSTATE_Pop(int nPlayer);
void  fn_80067710(int nPlayer, int a, u8 nEvent);
s8    GOLFERSTATE_GetPreviousState(int nPlayer);  // the state below the top of the stack
void  fn_80017158(int nView);
void  fn_80039344(int nView, f32 f);             // a per-view float (0x801D5010[nView])
void  fn_80062B64(int nPlayer);
void  fn_80062B60(int nPlayer);
void  fn_80045824(int nPlayer);
u8    fn_80062B90(void);
u8    fn_80062B88(int nPlayer);
void  fn_80062B84(int a);
u8    fn_80062B7C(void);
void  fn_800A5980(u8 nPlayer);
void  TARGET_ResetMomentums(int nPlayer);
f32   fn_800D04AC(int nPlayer);
void  fn_800170F4(int nView);
void  fn_8003349C(f32 a, f32 b, f32 c);
void  fn_800D8D10(int nPlayer);
void  PlaceBall_ResetMomentums(int nPlayer);
void  fn_80062D98(void);
void  DynObj_TeeAdd(Ball* pBall, int nPlayer, int a);  // tee the ball up
void  fn_800A573C(u8 nPlayer);
void  fn_800A3CB0(f32* pPos, int nPlayer);
void  fn_800A3D6C(f32* pPos, int nPlayer);
void  fn_800A3DF4(int nPlayer);
void  fn_80062DDC(f32* pA, f32* pB, f32* pOut);  // a - b
void  fn_8006BB5C(int nPlayer);
void  fn_800A562C(u8 nPlayer);
void  fn_800D1DAC(int nPlayer);
void  fn_80062B68(int nPlayer);
void  fn_80062B6C(int nPlayer);
void  fn_80062DB8(View* pView, int a);
void  fn_80062DC0(View* pView);
f32   fn_80062DCC(View* pView);
u8    fn_80062DD4(View* pView);

#define CPU_TOLERANCE (0.05f * 0.05f)       // 0.05 yd squared: land within 1.8 in of the target

void  STATEFUNC_PreShotInit(int nPlayer);
void  STATEFUNC_PreShotUpdate(int nPlayer);
void  STATEFUNC_PreShotExit(int nPlayer);
void  STATEFUNC_ShotSetupInit(int nPlayer);
void  STATEFUNC_ShotSetupUpdate(int nPlayer);
void  STATEFUNC_ShotSetupExit(int nPlayer);
void  STATEFUNC_ZoomInit(int nPlayer);
void  STATEFUNC_ZoomUpdate(int nPlayer);
void  STATEFUNC_ZoomExit(int nPlayer);
void  STATEFUNC_ElevatorInit(int nPlayer);
void  STATEFUNC_ElevatorUpdate(int nPlayer);
void  STATEFUNC_ElevatorExit(int nPlayer);
void  STATEFUNC_GreenInit(int nPlayer);
void  STATEFUNC_GreenUpdate(int nPlayer);
void  STATEFUNC_GreenExit(int nPlayer);
void  STATEFUNC_GreenWatchRollInit(int nPlayer);
void  STATEFUNC_GreenWatchRollUpdate(int nPlayer);
void  STATEFUNC_GreenWatchRollExit(int nPlayer);
void  STATEFUNC_GreenReversePuttInit(int nPlayer);
void  STATEFUNC_GreenReversePuttUpdate(int nPlayer);
void  STATEFUNC_GreenReversePuttExit(int nPlayer);
void  STATEFUNC_KneeCamInit(int nPlayer);
void  STATEFUNC_KneeCamUpdate(int nPlayer);
void  STATEFUNC_KneeCamExit(int nPlayer);
void  STATEFUNC_GreenMorphInit(int nPlayer);
void  STATEFUNC_GreenMorphUpdate(int nPlayer);
void  STATEFUNC_GreenMorphExit(int nPlayer);
void  STATEFUNC_SwingInit(int nPlayer);
void  STATEFUNC_SwingUpdate(int nPlayer);
void  STATEFUNC_SwingExit(int nPlayer);
void  STATEFUNC_ReplaySwingInit(int nPlayer);
void  STATEFUNC_ReplaySwingUpdate(int nPlayer);
void  STATEFUNC_ReplaySwingExit(int nPlayer);
void  STATEFUNC_InTheHoleInit(int nPlayer);
void  STATEFUNC_InTheHoleUpdate(int nPlayer);
void  STATEFUNC_ShowYardageInit(int nPlayer);
void  STATEFUNC_ShowYardageUpdate(int nPlayer);
void  STATEFUNC_ShowYardageExit(int nPlayer);
void  STATEFUNC_FadeToTapInInit(int nPlayer);
void  STATEFUNC_FadeToTapInUpdate(int nPlayer);
void  STATEFUNC_FadeToTapInExit(int nPlayer);
void  STATEFUNC_TapInInit(int nPlayer);
void  STATEFUNC_TapInUpdate(int nPlayer);
void  STATEFUNC_TapInExit(int nPlayer);
void  STATEFUNC_FadeToRemoveBallInit(int nPlayer);
void  STATEFUNC_FadeToRemoveBallUpdate(int nPlayer);
void  STATEFUNC_FadeToRemoveBallExit(int nPlayer);
void  STATEFUNC_RemoveBallInit(int nPlayer);
void  STATEFUNC_RemoveBallUpdate(int nPlayer);
void  STATEFUNC_RemoveBallExit(int nPlayer);
void  STATEFUNC_WaitInit(int nPlayer);
void  STATEFUNC_WaitUpdate(int nPlayer);
void  STATEFUNC_InitialFlyByInit(int nPlayer);
void  STATEFUNC_InitialFlyByUpdate(int nPlayer);
void  STATEFUNC_InitialFlyByExit(int nPlayer);
void  STATEFUNC_MidHoleFlyByInit(int nPlayer);
void  STATEFUNC_MidHoleFlyByUpdate(int nPlayer);
void  STATEFUNC_MidHoleFlyByExit(int nPlayer);
void  STATEFUNC_PlaceBallInit(int nPlayer);
void  STATEFUNC_PlaceBallUpdate(int nPlayer);
void  STATEFUNC_PlaceBallExit(int nPlayer);
void  STATEFUNC_ConcededInit(int nPlayer);
void  STATEFUNC_ConcededUpdate(int nPlayer);
void  STATEFUNC_ConcededExit(int nPlayer);

ModeStateDef lbl_801883C0[2] = {
    {NULL, NULL, NULL},
    {NULL, GOLFERSTATE_Update, NULL}
};

SwingStateDef sGolferStateEngineTable[GS_NUM] = {
    {NULL, NULL, NULL},
    {STATEFUNC_PreShotInit, STATEFUNC_PreShotUpdate, STATEFUNC_PreShotExit},
    {STATEFUNC_ShotSetupInit, STATEFUNC_ShotSetupUpdate, STATEFUNC_ShotSetupExit},
    {STATEFUNC_ZoomInit, STATEFUNC_ZoomUpdate, STATEFUNC_ZoomExit},
    {STATEFUNC_ElevatorInit, STATEFUNC_ElevatorUpdate, STATEFUNC_ElevatorExit},
    {STATEFUNC_GreenInit, STATEFUNC_GreenUpdate, STATEFUNC_GreenExit},
    {STATEFUNC_GreenWatchRollInit, STATEFUNC_GreenWatchRollUpdate, STATEFUNC_GreenWatchRollExit},
    {STATEFUNC_GreenReversePuttInit, STATEFUNC_GreenReversePuttUpdate, STATEFUNC_GreenReversePuttExit},
    {STATEFUNC_KneeCamInit, STATEFUNC_KneeCamUpdate, STATEFUNC_KneeCamExit},
    {STATEFUNC_GreenMorphInit, STATEFUNC_GreenMorphUpdate, STATEFUNC_GreenMorphExit},
    {STATEFUNC_SwingInit, STATEFUNC_SwingUpdate, STATEFUNC_SwingExit},
    {STATEFUNC_ReplaySwingInit, STATEFUNC_ReplaySwingUpdate, STATEFUNC_ReplaySwingExit},
    {STATEFUNC_SimulateInit, STATEFUNC_SimulateUpdate, STATEFUNC_SimulateExit},
    {STATEFUNC_InTheHoleInit, STATEFUNC_InTheHoleUpdate, NULL},
    {STATEFUNC_ShowYardageInit, STATEFUNC_ShowYardageUpdate, STATEFUNC_ShowYardageExit},
    {STATEFUNC_FadeToTapInInit, STATEFUNC_FadeToTapInUpdate, STATEFUNC_FadeToTapInExit},
    {STATEFUNC_TapInInit, STATEFUNC_TapInUpdate, STATEFUNC_TapInExit},
    {STATEFUNC_FadeToRemoveBallInit, STATEFUNC_FadeToRemoveBallUpdate, STATEFUNC_FadeToRemoveBallExit},
    {STATEFUNC_RemoveBallInit, STATEFUNC_RemoveBallUpdate, STATEFUNC_RemoveBallExit},
    {STATEFUNC_WaitInit, STATEFUNC_WaitUpdate, NULL},
    {STATEFUNC_InitialFlyByInit, STATEFUNC_InitialFlyByUpdate, STATEFUNC_InitialFlyByExit},
    {STATEFUNC_MidHoleFlyByInit, STATEFUNC_MidHoleFlyByUpdate, STATEFUNC_MidHoleFlyByExit},
    {STATEFUNC_PlaceBallInit, STATEFUNC_PlaceBallUpdate, STATEFUNC_PlaceBallExit},
    {STATEFUNC_ConcededInit, STATEFUNC_ConcededUpdate, STATEFUNC_ConcededExit},
    {NULL, NULL, NULL},
    {NULL, NULL, NULL},
    {NULL, NULL, NULL}
};

// fake match: stands in for a function the original linker stripped. The file's .sdata2 pool
// starts with 1.0f (0x80283680), before the 0.0f STATEFUNC_PreShotInit uses first; its body is
// unknown.
static f32 stateFunc_StrippedFn(f32 x) {
    return x + 1.0f;
}

// State 1 begins: addressing the ball. Camera 25 and the game's 0x20C hook; a fresh Shot_Plan
// when the game asks (gpGame+0x276, and it ends any replay); the glove comes off for a putt;
// both views attached; the ball teed up on the tee and kept as it lies; fA64 stored; events 0x2A
// and 3; fn_800E3D38(n, 1) for a CPU outside game type 8; and if GM_DoPreshotAnimation says so,
// the pre-shot animation - 10 with a low-IQ penalty away from the tee (unless fn_80100294()),
// else 1 - and camera 11.
void STATEFUNC_PreShotInit(int nPlayer) {
    Ball* pBall;
    int   nView, k;

    fn_80062F1C(fn_80017028(gPlayers[nPlayer].nView[0]));
    nView = gPlayers[nPlayer].nView[0];
    CameraController_SetCameraMode(fn_80017028(nView), 0x19, nPlayer, nView);
    gpGame->pfn20C(nPlayer);
    fn_80062B64(nPlayer);
    fn_80062B60(nPlayer);
    GM_MovePlayerToBall(nPlayer);
    if (gpGame->b276 != 0) {
        gSession.bReplay = 0;
        Shot_Plan(nPlayer, 1);
    }
    if (gPlayers[nPlayer].nClub == CLUB_PUTTER_e) {
        fn_800CC5C0(gPlayers[nPlayer].pChar, "Glove", "GloveOff");
    } else {
        fn_800CC5C0(gPlayers[nPlayer].pChar, "Glove", "GloveOn");
    }
    gPlayers[nPlayer].fThinkTime = 0.0f;
    for (k = 0; k < 2; k++) {
        fn_8001704C(gPlayers[nPlayer].nView[k], nPlayer);
    }
    EVENT_Trigger(nPlayer, 0x2A, 0, -1);
    fn_8001D8DC(nPlayer);
    fn_8001C804(nPlayer, 1, 1);
    if (gpGame->b277 != 0) {
        fn_800957D8(gPlayers[nPlayer].pChar);
    }
    GameEffects_ResetGameEffectSettings();
    fn_800D8D10(nPlayer);
    TARGET_ResetMomentums(nPlayer);
    fn_800957FC(gPlayers[nPlayer].pChar, 1);
    Emotion_UpdatePlayerEmotion(nPlayer);
    if (gPlayers[nPlayer].pChar->n2C == 0) {
        fn_800957B0(gPlayers[nPlayer].pChar, 1);
    }
    fn_80045824(nPlayer);
    SW_vClearBoosts(nPlayer);
    BreakLine_Reset(gPlayers[nPlayer].nView[0]);
    fn_8009B970(gPlayers[nPlayer].nView[0]);
    if (gPlayers[nPlayer].ball.nLie == 0) {
        DynObj_TeeAdd(&gPlayers[nPlayer].ball, nPlayer, 1);
    }
    fn_80016CFC(gPlayers[nPlayer].nView[0])->bFlagOut = 1;
    if (gPlayers[nPlayer].ball.nLie != 10 && gPlayers[nPlayer].ball.nLie != LIE_GREEN_e &&
        gPlayers[nPlayer].ball.nLie != LIE_INCUP_e) {
        fn_80016CFC(gPlayers[nPlayer].nView[0])->bFlagOut = 0;
    }
    pBall = &gPlayers[nPlayer].ball;
    Physics_InitShotData(pBall);
    Mem_cpy(&gPlayers[nPlayer].ballBefore, pBall, sizeof(Ball));
    gPlayers[nPlayer].fA64 = fn_800D04AC(nPlayer);
    EVENT_Trigger(nPlayer, 3, 0, -1);
    if (gSession.nGameType != 8 && Player_IsCPU(nPlayer)) {
        fn_800E3D38(nPlayer, 1);
    }
    if (GM_DoPreshotAnimation(nPlayer) != 0) {
        SKATime_UnPause(gPlayers[nPlayer].pChar->anim);
        if (gPlayers[nPlayer].bLowIQPenalty != 0 && gPlayers[nPlayer].ball.nLie != 0 && !fn_80100294()) {
            fn_80095744(gPlayers[nPlayer].pChar, 10);
        } else {
            fn_80095744(gPlayers[nPlayer].pChar, 1);
        }
        CharacterState_UpdateSKAState(gPlayers[nPlayer].pChar);
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0xB, nPlayer, nView);
    }
}

// State 1 update: addressing the ball. The game's 0x238 hook can skip to state 2 (unless the
// camera is 0 or 11). The golfer's animation places the ball: while its event 3 is pending the ball
// rides in the hand, and once it fires the ball is set down (on the tee: 2 units up and teed);
// events 0x10 and 0x11 drive fn_800A3CB0/fn_800A3D6C/fn_800A3DF4. A ball still moving is stepped.
// Button 0 (any pad for a CPU) starts a fade out; a CPU also rehearses here once its ball is still.
// Once fn_80063C7C(view) holds (and the camera allows) the ball is put in place and it is state 2;
// after 10 s on a camera it is state 2 anyway.
void STATEFUNC_PreShotUpdate(int nPlayer) {
    Vec4    vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    u8      bInHand = 0;
    View*   pV;
    GoDynObjPlayerA* pSlot;
    int     nSteps, i;
    f32     vHand[4];

    pV    = fn_80017028(gPlayers[nPlayer].nView[0]);
    pSlot = &lbl_80281DA0->aA[nPlayer];
    if (pV->nCurCamera != 11 && pV->nCurCamera != 0 && gpGame->pfn238(nPlayer)) {
        GOLFERSTATE_Switch(GS_SHOT_SETUP, nPlayer);
        return;
    }
    if (fn_80095780(gPlayers[nPlayer].pChar) == 10) {
        if (fn_80048574(gPlayers[nPlayer].pChar, 3)) {
            if (fn_80062BB0(gPlayers[nPlayer].pChar, 3)) {
                fn_80062B98(gPlayers[nPlayer].pChar, 3);
                if (!Physics_SetBallPosition(&gPlayers[nPlayer].ball, gPlayers[nPlayer].ball.vPos)) {
                    Physics_DropBall(&gPlayers[nPlayer].ball, gPlayers[nPlayer].ball.vPos);
                }
            } else {
                Character_GetBallOnFingerPosition(gPlayers[nPlayer].pChar, gPlayers[nPlayer].ball.vPos);
                bInHand = 1;
            }
        }
    } else if (fn_80048574(gPlayers[nPlayer].pChar, 0x10)) {
        if (fn_80062BB0(gPlayers[nPlayer].pChar, 0x10)) {
            Character_GetBallOnFingerPosition(gPlayers[nPlayer].pChar, vHand);
            fn_80062B98(gPlayers[nPlayer].pChar, 0x10);
            fn_800A3CB0(vHand, nPlayer);
        }
    } else if (fn_80048574(gPlayers[nPlayer].pChar, 0x11)) {
        if (fn_80062BB0(gPlayers[nPlayer].pChar, 0x11)) {
            fn_80062B98(gPlayers[nPlayer].pChar, 0x11);
            fn_800A3DF4(nPlayer);
        } else {
            Character_GetBallOnFingerPosition(gPlayers[nPlayer].pChar, vHand);
            fn_800A3D6C(vHand, nPlayer);
        }
    }
    if (gPlayers[nPlayer].ball.nLie == 0 && fn_80048574(gPlayers[nPlayer].pChar, 3)) {
        if (fn_80062BB0(gPlayers[nPlayer].pChar, 3)) {
            fn_80062B98(gPlayers[nPlayer].pChar, 3);
            gPlayers[nPlayer].ball.vPos[1] += 2.0f;
            Physics_InitBall(&gPlayers[nPlayer].ball, gPlayers[nPlayer].ball.vPos, nPlayer);
            DynObj_TeeAdd(&gPlayers[nPlayer].ball, nPlayer, 0);
        } else {
            Character_GetBallOnFingerPosition(gPlayers[nPlayer].pChar, gPlayers[nPlayer].ball.vPos);
            fn_8001DA04(gPlayers[nPlayer].pChar, pSlot->v30, pSlot->v50);
            bInHand = 1;
        }
    } else if (gPlayers[nPlayer].ball.nLie == 0) {
        DynObj_TeeAdd(&gPlayers[nPlayer].ball, nPlayer, 0);
    }
    if (gPlayers[nPlayer].ball.nState != 0 && gPlayers[nPlayer].ball.nCollideCount == 0) {
        nSteps = GameEffects_BallUpdatesThisFrame(nPlayer);
        fn_80050D24_SetSimulating(1);
        for (i = 0; i < nSteps; i++) {
            Physics_Simulate(&gPlayers[nPlayer].ball, 20);
        }
        fn_80050D24_SetSimulating(0);
    } else if (gPlayers[nPlayer].ball.nState != 0) {
        Physics_DropBall(&gPlayers[nPlayer].ball, gPlayers[nPlayer].ball.vPos);
    }
    if (!Player_IsCPU(nPlayer)) {
        if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0, 0)) && !fn_80100294()) {
            fn_800C70F8(pV, 1);
            if (!fn_80063C90(pV)) {
                CameraController_FadeOut(pV, 0.25f, (f32*)&vOffset);
                fn_800C7080(pV);
            }
        }
    } else {
        if (gPlayers[nPlayer].ball.nState == 0 && !bInHand) {
            AI_RehearseShot(nPlayer, NULL, 0, CPU_TOLERANCE);
        }
        if (!fn_80100294() && fn_80014300(fn_800142AC(0, 0))) {
            fn_800C70F8(pV, 1);
            if (!fn_80063C90(pV)) {
                CameraController_FadeOut(pV, 0.25f, (f32*)&vOffset);
                fn_800C7080(pV);
            }
        }
    }
    if (fn_80095780(gPlayers[nPlayer].pChar) != 10 && fn_80095780(gPlayers[nPlayer].pChar) != 1 &&
        fn_800C6E88(pV, nPlayer)) {
        fn_80095744(gPlayers[nPlayer].pChar, 5);
        CharacterState_UpdateSKAState(gPlayers[nPlayer].pChar);
        fn_80063CF0(pV, 0x17, nPlayer);
    }
    if (fn_80063C7C(pV) && (fn_800C6F7C(pV, nPlayer, 0.25f) || !fn_800C6E88(pV, nPlayer))) {
        if (bInHand) {
            if (fn_80095780(gPlayers[nPlayer].pChar) == 10) {
                Physics_DropBall(&gPlayers[nPlayer].ball, gPlayers[nPlayer].vBall);
            } else {
                Physics_InitBall(&gPlayers[nPlayer].ball, gPlayers[nPlayer].ball.vPrev, nPlayer);
            }
        }
        GOLFERSTATE_Switch(GS_SHOT_SETUP, nPlayer);
        return;
    }
    if (pV->script.fCamTime > 10.0f) {
        GOLFERSTATE_Switch(GS_SHOT_SETUP, nPlayer);
    }
    if (fn_80095780(gPlayers[nPlayer].pChar) != 10 && fn_80095780(gPlayers[nPlayer].pChar) != 1 &&
        fn_800C6F7C(pV, nPlayer, 0.25f) && !fn_80063C90(pV)) {
        CameraController_FadeOut(pV, 0.25f, (f32*)&vOffset);
    } else if ((fn_80095780(gPlayers[nPlayer].pChar) == 10 || fn_80095780(gPlayers[nPlayer].pChar) == 1) &&
               gPlayers[nPlayer].pChar->fAnimTime > gPlayers[nPlayer].pChar->fAnimEnd - 0.25f &&
               !fn_80063C90(pV)) {
        CameraController_FadeOut(pV, 0.25f, (f32*)&vOffset);
    }
}

// Leaving pre-shot: the ball is put back from ballBefore; on script cameras 1, 3 and 4 a
// quarter-second fade in.
void STATEFUNC_PreShotExit(int nPlayer) {
    Vec4  vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    View* pView   = fn_80017028(gPlayers[nPlayer].nView[0]);
    int   nCamera;
    Mem_cpy(&gPlayers[nPlayer].ball, &gPlayers[nPlayer].ballBefore, sizeof(Ball));
    nCamera = pView->script.nCamera;
    if (nCamera == 1 || nCamera == 3 || nCamera == 4) {
        CameraController_FadeIn(pView, 0.25f, (f32*)&vOffset);
    }
}

// State 2, a shot begins. A CPU takes camera 0 (with gpGame+0x290) or 12; with that flag the
// golfer plays animation 2. The ball is kept as it lies and marked stopped, the live ball's state
// cleared, the swing reset (SW_vInitSwing), the target set up, fA64 stored and event 6 fired.
void STATEFUNC_ShotSetupInit(int nPlayer) {
    s32   nView;
    Ball* pBall;
    if (Player_IsCPU(nPlayer) && gpGame->n290 != 0) {
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0, nPlayer, nView);
    } else if (Player_IsCPU(nPlayer)) {
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0xC, nPlayer, nView);
    }
    if (gpGame->n290 != 0) {
        fn_80095744(gPlayers[nPlayer].pChar, 2);
        CharacterState_UpdateSKAState(gPlayers[nPlayer].pChar);
    }
    fn_80062C38();
    pBall = &gPlayers[nPlayer].ball;
    Mem_cpy(&gPlayers[nPlayer].ballBefore, pBall, sizeof(Ball));
    gPlayers[nPlayer].ballBefore.nState = 1;
    gPlayers[nPlayer].ball.nState       = 0;
    GameEffects_ResetGameEffectSettings();
    SW_vInitSwing(nPlayer);
    gPlayers[nPlayer].swing.unk630 = 1;
    TARGET_ResetMomentums(nPlayer);
    if (gpGame->b276 != 0) {
        TARGET_SetupTarget(nPlayer);
    }
    if (gPlayers[nPlayer].pChar->n2C == 4 ||
        gPlayers[nPlayer].pChar->n2C == 5) {
        fn_800957FC(gPlayers[nPlayer].pChar, 1);
        fn_800957B0(gPlayers[nPlayer].pChar, 1);
    }
    Physics_InitShotData(pBall);
    gPlayers[nPlayer].fA64 = fn_800D04AC(nPlayer);
    EVENT_Trigger(nPlayer, 6, 0, -1);
}

// ---- state 2: thinking ----------------------------------------------------------------------------

// A human goes straight on to state 10 (the swing). A CPU rehearses its shot here, one frame at a
// time, and moves on once the rehearsal is done, more than a second has passed (3 s in mode 11) and
// fn_800C7100(view) agrees - or when its time is up: 4 s; in modes 6 and 7 (which wait while
// fn_800FA118) 1.5 s on the hole's first stroke and 3.5 s after. Out of time, an unfinished
// rehearsal is finished (nRehearseState 3); then AI_ApplyError and state 10.
void STATEFUNC_ShotSetupUpdate(int nPlayer) {
    View*   pView;
    u8      bDone;
    f32     fMax;
    f32     fMin;

    pView = fn_80017028(gPlayers[nPlayer].nView[0]);
    if (!Player_IsCPU(nPlayer)) {
        GOLFERSTATE_Switch(GS_SWING, nPlayer);
        return;
    }
    if (Player_IsCPU(nPlayer)) {
        gPlayers[nPlayer].fThinkTime += gSession.fFrameTime;
        bDone = AI_RehearseShot(nPlayer, NULL, 0, CPU_TOLERANCE);
        fMin  = 1.0f;
        switch (Game_GetMode()) {
        case 6:
        case 7:
            if (fn_800FA118(0, 0)) return;
            if (gPlayers[nPlayer].nStrokes[Game_CurHoleIndex()] <= 0) {
                fMax = 1.5f;
            } else {
                fMax = 3.5f;
            }
            break;
        case 11:
            fMin = 3.0f;
            fMax = 4.0f;
            break;
        default:
            fMax = 4.0f;
            break;
        }
        if ((bDone && gPlayers[nPlayer].fThinkTime > fMin && fn_800C7100(pView)) ||
            gPlayers[nPlayer].fThinkTime > fMax) {
            if (!bDone) {
                gPlayers[nPlayer].nRehearseState = 3;
                AI_RehearseShot(nPlayer, NULL, 0, CPU_TOLERANCE);
            }
            AI_ApplyError(nPlayer);
            GOLFERSTATE_Switch(GS_SWING, nPlayer);
        }
    }
}

void STATEFUNC_ShotSetupExit(int nPlayer) {
    if (gSession.nGameType != 8 && gPlayers[nPlayer].ball.nLie != LIE_INCUP_e && !Player_IsCPU(nPlayer)) {
        fn_800E3D38(nPlayer, 1);
    }
}

// State 10 begins: the swing. The address animation (5) with the club and shot kind set on the
// character; the swing reset unless the shot setup did it (unk630); camera 12; first-time tips for
// a human (fn_800E505C 0/1/2 or fn_800D1DAC, with gpGame+0x281); the caddie starts; on the tee the
// tee goes in and every player's ball markers are set; the shot flags are cleared; event 7.
void STATEFUNC_SwingInit(int nPlayer) {
    View* pV    = fn_80017028(gPlayers[nPlayer].nView[0]);
    int   nView;
    Ball* pBall;
    Ball* pOwnBall;
    int   i;

    if (pV->nCurCamera == 0 && Player_IsCPU(nPlayer) && Game_GetMode() != 11) {
        fn_800957D8(gPlayers[nPlayer].pChar);
    }
    fn_80095744(gPlayers[nPlayer].pChar, 5);
    Character_SelectGameClub(gPlayers[nPlayer].pChar, gPlayers[nPlayer].nClub);
    Character_SelectGameShotType(gPlayers[nPlayer].pChar, gPlayers[nPlayer].nShotKind);
    fn_80062BFC(gPlayers[nPlayer].pChar);
    fn_80062BE8(gPlayers[nPlayer].pChar);
    SKATime_UnPause(gPlayers[nPlayer].pChar->anim);
    if (gPlayers[nPlayer].swing.unk630 == 0) {
        SW_vInitSwing(nPlayer);
        gPlayers[nPlayer].swing.unk630 = 1;
    }
    nView = gPlayers[nPlayer].nView[0];
    CameraController_SetCameraMode(fn_80017028(nView), 0xC, nPlayer, nView);
    GameEffects_ResetGameEffectSettings();
    fn_8001C804(nPlayer, 1, 1);
    TARGET_ResetMomentums(nPlayer);
    fn_800A562C((u8)nPlayer);
    fn_8006BAA8(nPlayer);
    EVENT_Trigger(nPlayer, 0x2A, 0, -1);
    Emotion_UpdatePlayerEmotion(nPlayer);
    fn_8006ACF8(nPlayer, 5);
    gPlayers[nPlayer].fC20 = 0.0f;
    if (gPlayers[nPlayer].bC2E == 0 && gpGame->b281 != 0 && !Player_IsCPU(nPlayer)) {
        if (fn_800DA264() && fn_800DA174()) {
            fn_800E505C(0);
            fn_800E3D38(nPlayer, 0);
            fn_80062C80(gPlayers[nPlayer].nC58, 0);
        } else if (fn_800DA264() && fn_800DA1D4()) {
            fn_800E505C(1);
            fn_800E3D38(nPlayer, 0);
            fn_80062C80(gPlayers[nPlayer].nC58, 0);
        } else if (fn_800DA264() && fn_800DA234()) {
            fn_800E505C(2);
            fn_800E3D38(nPlayer, 0);
            fn_80062C80(gPlayers[nPlayer].nC58, 0);
        } else {
            fn_800D1DAC(nPlayer);
        }
    }
    if (gPlayers[nPlayer].bC2E == 0 && !Player_IsCPU(nPlayer) && fn_800EC550() && fn_800ED540()) {
        if (fn_800F0818()) {
            fn_800E502C(fn_800EAC7C());
        } else {
            fn_800E4FFC(fn_800EAC7C());
        }
        fn_800E3D38(nPlayer, 0);
        fn_80062C80(gPlayers[nPlayer].nC58, 0);
        fn_800ED548();
    }
    Caddie_Start(nPlayer);
    fn_80062C5C();
    if (gPlayers[nPlayer].ball.nLie == 0) {
        DynObj_TeeAdd(&gPlayers[nPlayer].ball, nPlayer, 1);
        for (i = 0; i < gSession.nNumPlayers; i++) {
            pBall = &PLAYER(i)->ball;
            fn_80047B6C(pBall, i);
            fn_80047BC0(pBall, i);
        }
    }
    pOwnBall = &gPlayers[nPlayer].ball;
    fn_80047B6C(pOwnBall, nPlayer);
    fn_80047BC0(pOwnBall, nPlayer);
    gPlayers[nPlayer].bC2E = 0;
    gPlayers[nPlayer].bPlanReady = 0;
    gPlayers[nPlayer].uFlags     = 0;
    EVENT_Trigger(nPlayer, 7, 0, -1);
    fn_800DB4E8(nPlayer);
    fn_80062B68(nPlayer);
}

// State 10 every frame: the swing. The swing phase runs; an idle timer (reset by fn_80014300 or a
// swing phase past 0, wraps at 10 s). When the ball is hit: GM_BallHit, the RNG stream 1 is
// reseeded from the session seed, the view told, and it is state 11 (the swing animation) on a
// special camera (with gpGame+0x283) - or state 12 straight away. Otherwise, for a human outside a
// replay before the swing starts: the caddie updates; button 6 re-plans the shot (the 0x264 hook
// or a putt uses AI_DefaultTarget, else AI_ChooseTarget) with the break line and HUD redone;
// button 5 on a putt pushes the reverse-putt camera (state 7); else GM_CheckForShotChanges.
void STATEFUNC_SwingUpdate(int nPlayer) {
    int   nClub = gPlayers[nPlayer].nClub;
    u8    bSwung;

    if (fn_800E5098()) return;
    bSwung = fn_80058F5C(nPlayer);
    gPlayers[nPlayer].fC20 += gSession.fFrameTime;
    if (fn_80014300(gPlayers[nPlayer].nController) || gPlayers[nPlayer].swing.nState != 0) {
        gPlayers[nPlayer].fC20 = 0.0f;
    }
    if (gPlayers[nPlayer].fC20 >= 10.0f) {
        gPlayers[nPlayer].fC20 = 0.0f;
    }
    if (nClub != CLUB_PUTTER_e) {
        fn_800A573C((u8)nPlayer);
    }
    if (gpGame->b282 != 0) {
        if (fn_800E3DDC(nPlayer)) {
            if (gPlayers[nPlayer].swing.nState != 0) {
                fn_800E3D38(nPlayer, 0);
            }
        } else {
            if (gPlayers[nPlayer].swing.nState == 0) {
                fn_800E3D38(nPlayer, 1);
            }
        }
    }
    gpGame->pfn228(nPlayer);
    if (bSwung) {
        View* pV;
        GM_BallHit(nPlayer);
        fn_80062B6C(nPlayer);
        Misc_SetSeedFunc(1, gSession.nSeed);
        pV = fn_80017028(gPlayers[nPlayer].nView[0]);
        GolfCamera_ChooseSpecialSwing(pV, nPlayer);
        fn_800A5980((u8)nPlayer);
        if (fn_800C7138(pV) == 0) {
            EVENT_Trigger(nPlayer, 0x3B, 0, 0);
        }
        if (gpGame->b283 != 0 &&
            (fn_800C441C(pV, nPlayer) || GolfCamera_Choose3ScreenCam(pV, nPlayer) ||
             GolfCamera_ChooseHeartBeatCam(pV, nPlayer) || GolfCamera_ChooseShutterCam(pV, nPlayer))) {
            GOLFERSTATE_Switch(GS_REPLAY_SWING, nPlayer);
        } else {
            GOLFERSTATE_Switch(GS_SIMULATE, nPlayer);
        }
        return;
    }
    if (Player_IsCPU(nPlayer)) return;
    if (gSession.bReplay != 0) return;
    if (gPlayers[nPlayer].swing.nState != 0) return;
    Caddie_Update(nPlayer);
    if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(6, 0)) {
        if (gpGame->b284 != 0) {
            if (gpGame->pfn264(nPlayer)) {
                AI_DefaultTarget(nPlayer);
            } else if (gPlayers[nPlayer].nShotKind == SHOT_TYPE_PUTT_e) {
                AI_DefaultTarget(nPlayer);
            } else {
                AI_ChooseTarget(nPlayer);
            }
            Shot_Prepare(nPlayer, 1);
            BreakLine_Reset(gPlayers[nPlayer].nView[0]);
            fn_8009B970(gPlayers[nPlayer].nView[0]);
            fn_8001C804(nPlayer, 1, 1);
            fn_800957D8(gPlayers[nPlayer].pChar);
            fn_80095744(gPlayers[nPlayer].pChar, 5);
            TARGET_SetupTarget(nPlayer);
            fn_80062C38();
            if (gSession.nSplitScreen != 0) {
                fn_80062CB0(gPlayers[nPlayer].nC58, 1);
            }
            TARGET_ResetMomentums(nPlayer);
            fn_800642D0_ReapplyCurrentShot(fn_80017028(gPlayers[nPlayer].nView[0]), nPlayer);
            fn_800E3D38(nPlayer, 1);
        } else {
            TARGET_ResetMomentums(nPlayer);
        }
        gpGame->pfn22C(nPlayer);
    } else {
        if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(5, 0)) &&
            gPlayers[nPlayer].nShotKind == SHOT_TYPE_PUTT_e) {
            GOLFERSTATE_Push(GS_GREEN_REVERSE_PUTT, nPlayer);
        } else {
            GM_CheckForShotChanges(nPlayer);
        }
    }
}

void STATEFUNC_SwingExit(int nPlayer) {
    if (gPlayers[nPlayer].swing.unk630 != 0) {
        gPlayers[nPlayer].swing.unk630 = 0;
    }
}

// State 11 begins: the swing animation. The camera is 12 in a replay, else one of the three
// special swing cameras (20..22) the view offers, else 13. A tap-in (player flag 8) takes the
// tap-in state, anything else a swing blend (CharacterState_AddSKABlendData, two variants by
// camera) with full IK weight; the ball is teed up on the tee, super slow-mo goes on and the live
// ball's state is cleared for the launch.
void STATEFUNC_ReplaySwingInit(int nPlayer) {
    View* pV    = fn_80017028(gPlayers[nPlayer].nView[0]);
    Ball* pBall;

    if (gSession.bReplay != 0) {
        CameraController_SetCameraMode(pV, 0xC, nPlayer, gPlayers[nPlayer].nView[0]);
    } else {
        fn_80062D98();
        if (GolfCamera_Choose3ScreenCam(pV, nPlayer)) {
            CameraController_SetCameraMode(pV, 0x14, nPlayer, gPlayers[nPlayer].nView[0]);
        } else if (GolfCamera_ChooseHeartBeatCam(pV, nPlayer)) {
            CameraController_SetCameraMode(pV, 0x15, nPlayer, gPlayers[nPlayer].nView[0]);
        } else if (GolfCamera_ChooseShutterCam(pV, nPlayer)) {
            CameraController_SetCameraMode(pV, 0x16, nPlayer, gPlayers[nPlayer].nView[0]);
        } else {
            CameraController_SetCameraMode(pV, 0xD, nPlayer, gPlayers[nPlayer].nView[0]);
        }
    }
    fn_8001C804(nPlayer, 1, 1);
    fn_800957D8(gPlayers[nPlayer].pChar);
    gPlayers[nPlayer].pChar->n20 = 7;
    gPlayers[nPlayer].pChar->nAnim = 7;
    if (gPlayers[nPlayer].uFlags & 8) {
        CharacterState_SetTapInState(gPlayers[nPlayer].pChar);
    } else {
        if (GolfCamera_IsSlowMoSwingCamActive() || GolfCamera_Choose3ScreenCam(pV, nPlayer) ||
            GolfCamera_ChooseHeartBeatCam(pV, nPlayer) || GolfCamera_ChooseShutterCam(pV, nPlayer)) {
            CharacterState_AddSKABlendData(gPlayers[nPlayer].pChar, 1, 0, fn_80072ACC, 1, 8,
                                           -20000.0f, -30000.0f, -10000.0f, 0.0f, -10000.0f);
        } else {
            CharacterState_AddSKABlendData(gPlayers[nPlayer].pChar, 1, 0, fn_80072ACC, 1, 8,
                                           -20000.0f, -90000.0f, -10000.0f, 0.0f, -10000.0f);
        }
        SKEL_SetIKSolutionWeight(gPlayers[nPlayer].pChar->pModel->pSkel, 1.0f);
    }
    SW_vUIBlurReset(nPlayer);
    if (gPlayers[nPlayer].ball.nLie == 0) {
        DynObj_TeeAdd(&gPlayers[nPlayer].ball, nPlayer, 1);
    }
    pBall = &gPlayers[nPlayer].ball;
    fn_80047B6C(pBall, nPlayer);
    fn_80047BC0(pBall, nPlayer);
    if (gSession.bReplay != 0) {
        GameEffects_ResetGameEffectSettings();
    } else {
        fn_800DAF74();
    }
    GameEffects_SetSuperSlowMo(1, nPlayer, GolfCamera_ReplaySwingSpeed(pV));
    gPlayers[nPlayer].ball.nState = 0;
    if (gSession.bReplay != 0) {
        fn_80062CE0(1);
    }
}

// State 11: the swing animation in slow motion. Nothing more until it passes its ball-hit event
// (2). Then in a replay the ball launches and it is state 12; otherwise once fn_800C4518(view)
// reaches fn_800C6B38(view) event 0xA fires, the ball launches and it is state 12; before that the
// slow-mo camera advances (fn_800C5CEC) and one of two swing blends restarts.
void STATEFUNC_ReplaySwingUpdate(int nPlayer) {
    View* pV    = fn_80017028(gPlayers[nPlayer].nView[0]);
    u8    bSpecial = 0;
    GameEffects_SetSuperSlowMo(1, nPlayer,
                               GolfCamera_ReplaySwingSpeed(fn_80017028(gPlayers[nPlayer].nView[0])));
    if (!fn_80048574(gPlayers[nPlayer].pChar, 2) ||
        fn_8005CB78(gPlayers[nPlayer].pChar, 2) < gPlayers[nPlayer].pChar->fAnimTime) {
        if (gSession.bReplay) {
            SKEL_RelaxIK(gPlayers[nPlayer].pChar->pModel->pSkel);
            SW_vImpact(nPlayer);
            GOLFERSTATE_Switch(GS_SIMULATE, nPlayer);
        } else if (fn_800C4518(pV) >= fn_800C6B38(pV)) {
            EVENT_Trigger(nPlayer, 0xA, &gPlayers[nPlayer].ball, 1);
            GolfCamera_Choose3ScreenCam(pV, nPlayer);
            SKEL_RelaxIK(gPlayers[nPlayer].pChar->pModel->pSkel);
            SW_vImpact(nPlayer);
            GOLFERSTATE_Switch(GS_SIMULATE, nPlayer);
        } else {
            fn_800C5CEC(pV, nPlayer);
            GameEffects_SetSuperSlowMo(1, nPlayer, GolfCamera_ReplaySwingSpeed(pV));
            if (fn_800C4518(pV) >= fn_800C6B38(pV)) {
                bSpecial = fn_800C5FE4(pV, nPlayer);
            }
            if (bSpecial) {
                CharacterState_AddSKABlendData(gPlayers[nPlayer].pChar, 1, 0x12, fn_80072ACC, 1, 8,
                                               -20000.0f, -30000.0f, -10000.0f, 0.0f, -10000.0f);
            } else {
                CharacterState_AddSKABlendData(gPlayers[nPlayer].pChar, 1, 0, fn_80072ACC, 1, 8,
                                               -20000.0f, -90000.0f, -10000.0f, 0.0f, -10000.0f);
            }
            fn_800957D8(gPlayers[nPlayer].pChar);
            SW_vUIBlurReset(nPlayer);
            fn_80062D98();
        }
        fn_800A5980(nPlayer);
    }
    if (gPlayers[nPlayer].nClub != CLUB_PUTTER_e) {
        fn_800A573C(nPlayer);
    }
}

void STATEFUNC_ReplaySwingExit(int nPlayer) {
    fn_80045494(0, nPlayer);
    GameEffects_SetSuperSlowMo(0, nPlayer, 0.0f);
    fn_800C6E14();
    if (gSession.bReplay != 0) {
        EVENT_Trigger(nPlayer, 0x3B, 0, 0);
    } else {
        EVENT_Trigger(nPlayer, 0x3B, 0, 1);
    }
}

void STATEFUNC_ZoomInit(int nPlayer) {
    int nView;
    if (gPlayers[nPlayer].nShotKind == SHOT_TYPE_PUTT_e) {
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 2, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 1, nPlayer, nView);
    }
    fn_80062D38(0x62, 1, nPlayer);
    lbl_80281E11 = 1;
}

// State 3: held while button 8 is down (the caddie keeps updating). Buttons 9/10, 30 and 11..14
// fire events 0xD/0xE, 0xF and 0x12..0x15; once fn_800C7340 reports (flag from the enter) message
// 0x67 is posted, once.
void STATEFUNC_ZoomUpdate(int nPlayer) {
    Caddie_Update(nPlayer);
    if (!(fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(8, 1))) {
        GOLFERSTATE_Pop(nPlayer);
    } else {
        if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(9, 0))) {
            EVENT_Trigger(nPlayer, 0xD, 0, -1);
        } else {
            if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xA, 0))) {
                EVENT_Trigger(nPlayer, 0xE, 0, -1);
            }
        }
        if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x1E, 0))) {
            EVENT_Trigger(nPlayer, 0xF, 0, -1);
        }
        if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xB, 1))) {
            EVENT_Trigger(nPlayer, 0x12, 0, -1);
        } else {
            if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xC, 1))) {
                EVENT_Trigger(nPlayer, 0x13, 0, -1);
            }
        }
        if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xD, 1))) {
            EVENT_Trigger(nPlayer, 0x14, 0, -1);
        } else {
            if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xE, 1))) {
                EVENT_Trigger(nPlayer, 0x15, 0, -1);
            }
        }
    }
    TARGET_UpdateMomentums(nPlayer);
    if (lbl_80281E11 != 0) {
        if (fn_800C7340(fn_80017028(gPlayers[nPlayer].nView[0]), nPlayer)) {
            fn_80062D6C(0x67, nPlayer);
            lbl_80281E11 = 0;
        }
    }
}

void STATEFUNC_ZoomExit(int nPlayer) {
    int nView;
    EVENT_Trigger(nPlayer, 0x31, 0, -1);
    if (GOLFERSTATE_GetPreviousState(nPlayer) == GS_SWING) {
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0, nPlayer, nView);
    }
    fn_800E5714(0x10);
    fn_800E5724(nPlayer);
}

void STATEFUNC_ElevatorInit(int nPlayer) {
    int nView;
    if (lbl_80281F78->n1C0 != 0) {
        if (GolfCamera_NeedSteepSlopeCam(fn_80017028(gPlayers[nPlayer].nView[0]), nPlayer)) {
            nView = gPlayers[nPlayer].nView[0];
            CameraController_SetCameraMode(fn_80017028(nView), 0x13, nPlayer, nView);
            return;
        }
    }
    nView = gPlayers[nPlayer].nView[0];
    CameraController_SetCameraMode(fn_80017028(nView), 3, nPlayer, nView);
}

// State 4: held while button 7 is down (the caddie keeps updating). Buttons 9/10 and 11..14 fire
// events 0xD/0xE and 0x12..0x15.
void STATEFUNC_ElevatorUpdate(int nPlayer) {
    Caddie_Update(nPlayer);
    if (!(fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(7, 1))) {
        GOLFERSTATE_Pop(nPlayer);
    } else {
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(9, 0)) {
            EVENT_Trigger(nPlayer, 0xD, 0, -1);
        } else {
            if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xA, 0)) {
                EVENT_Trigger(nPlayer, 0xE, 0, -1);
            }
        }
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xB, 1)) {
            EVENT_Trigger(nPlayer, 0x12, 0, -1);
        } else {
            if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xC, 1)) {
                EVENT_Trigger(nPlayer, 0x13, 0, -1);
            }
        }
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xD, 1)) {
            EVENT_Trigger(nPlayer, 0x14, 0, -1);
        } else {
            if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xE, 1)) {
                EVENT_Trigger(nPlayer, 0x15, 0, -1);
            }
        }
    }
    TARGET_UpdateMomentums(nPlayer);
}

// Leaving the aiming states 4, 5 and 7: back to camera 12 if state 10 is underneath, else 0.
void STATEFUNC_ElevatorExit(int nPlayer) {
    int nView;
    if (GOLFERSTATE_GetPreviousState(nPlayer) == GS_SWING) {
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0, nPlayer, nView);
    }
    fn_800E3D38(nPlayer, 1);
}

void STATEFUNC_GreenInit(int nPlayer) {
    int nView = gPlayers[nPlayer].nView[0];
    CameraController_SetCameraMode(fn_80017028(nView), 4, nPlayer, nView);
    fn_800E3D38(nPlayer, 0);
}

// Pop the state once neither button 2 nor button 3 is held.
void STATEFUNC_GreenUpdate(int nPlayer) {
    if (!(fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(2, 1))) {
        if (!(fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(3, 1))) {
            GOLFERSTATE_Pop(nPlayer);
        }
    }
}

void STATEFUNC_GreenExit(int nPlayer) {
    int nView;
    if (GOLFERSTATE_GetPreviousState(nPlayer) == GS_SWING) {
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0, nPlayer, nView);
    }
    fn_800E3D38(nPlayer, 1);
}

// State 6: the putt preview. The caddie's solved shot is taken, the player is made a CPU for
// one call so SW_vImpact fires it clean (no error), and the launched ball is
// kept in ballBefore as nobody's ball - the ghost that draws the preview. Everything the
// player had (shot block, ball, controller) is put back afterwards.
void STATEFUNC_GreenWatchRollInit(int nPlayer) {
    u8    shotSaved[0x5C];
    Ball  ballSaved;
    Ball* pBall;
    u8*   pShot;
    int   nController;

    CameraController_SetCameraMode(fn_80017028(gPlayers[nPlayer].nView[0]), 5, nPlayer,
                                   gPlayers[nPlayer].nView[0]);
    pShot =(u8*)&gPlayers[nPlayer].nClub;
    Mem_cpy(shotSaved, pShot, 0x5C);   // port: the shot block, nClub..unk3AD (no pointers)
    Caddie_ApplyTip(nPlayer);
    pBall = &gPlayers[nPlayer].ball;
    Mem_cpy(&ballSaved, pBall, sizeof(Ball));
    nController  = gPlayers[nPlayer].nController;
    gPlayers[nPlayer].nController = CONTROLLER_CPU;
    SW_vImpact(nPlayer);
    gPlayers[nPlayer].nController = nController;
    Mem_cpy(&gPlayers[nPlayer].ballBefore, pBall, sizeof(Ball));
    gPlayers[nPlayer].ballBefore.nPlayer = -1;
    Mem_cpy(pBall, &ballSaved, sizeof(Ball));
    Mem_cpy(pShot, shotSaved, 0x5C);   // port: as above
    REPLAY_Save(nPlayer);
    fn_800E3D38(nPlayer, 0);
}

// State 6, the putt preview playing. Button 0 ends it (any pad for a CPU). Otherwise the ghost
// ball (in ballBefore) is stepped GameEffects_BallUpdatesThisFrame times, 20 ticks each, with the
// simulating flag up; a quarter-second fade-out starts once the ghost has stopped, come within 0.5
// of the pin or started running away from it (0.1 past its closest), and the state pops when the
// fade completes.
void STATEFUNC_GreenWatchRollUpdate(int nPlayer) {
    Vec4        vOffset = {0.45f, 0.45f, 0.45f, 0.5f};
    CourseInfo* pCourse = fn_8000C594();
    int         nSteps;
    int         i;

    Caddie_Update(nPlayer);
    if (!Player_IsCPU(nPlayer)) {
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0, 0)) {
            GOLFERSTATE_Pop(nPlayer);
            return;
        }
    } else {
        if (fn_80014300(fn_800142AC(0, 0))) {
            GOLFERSTATE_Pop(nPlayer);
            return;
        }
    }
    nSteps = GameEffects_BallUpdatesThisFrame(nPlayer);
    if (gPlayers[nPlayer].ballBefore.nState == 1 || gPlayers[nPlayer].ballBefore.nState == 5) {
        if (fn_80063C50(fn_80017028(gPlayers[nPlayer].nView[0]))) {
            GOLFERSTATE_Pop(nPlayer);
        }
        return;
    }
    if (fn_80063C50(fn_80017028(gPlayers[nPlayer].nView[0]))) {
        GOLFERSTATE_Pop(nPlayer);
        return;
    }
    for (i = 0; i < nSteps; i++) {
        fn_80050D24_SetSimulating(1);
        Physics_Simulate(&gPlayers[nPlayer].ballBefore, 20);
        fn_80050D24_SetSimulating(0);
        if (gPlayers[nPlayer].ballBefore.nState == 1 || gPlayers[nPlayer].ballBefore.nState == 5) {
            if (!fn_80063C90(fn_80017028(gPlayers[nPlayer].nView[0]))) {
                CameraController_FadeOut(fn_80017028(gPlayers[nPlayer].nView[0]), 0.25f, (f32*)&vOffset);
            }
        } else if (!fn_80063C90(fn_80017028(gPlayers[nPlayer].nView[0])) && pCourse != NULL) {
            int nPinSet = Game_CurrentPinSet();
            if (gPlayers[nPlayer].ballBefore.fClosest < 0.5f ||
                gPlayers[nPlayer].ballBefore.fClosest <
                    Vec_Distance(gPlayers[nPlayer].ballBefore.vPos, &pCourse->pin[nPinSet].x) - 0.1f) {
                CameraController_FadeOut(fn_80017028(gPlayers[nPlayer].nView[0]), 0.25f, (f32*)&vOffset);
            }
        }
    }
}

void STATEFUNC_GreenWatchRollExit(int nPlayer) {
    int nView;
    Mem_cpy(&gPlayers[nPlayer].ballBefore, &gPlayers[nPlayer].ball, sizeof(Ball));
    gPlayers[nPlayer].ballBefore.nPlayer = -1;     // nobody's ball
    gPlayers[nPlayer].ballBefore.nState = 1;      // stopped
    if (GOLFERSTATE_GetPreviousState(nPlayer) == GS_SWING) {
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0, nPlayer, nView);
    }
    fn_800E3D38(nPlayer, 1);
}

void STATEFUNC_GreenReversePuttInit(int nPlayer) {
    int nView = gPlayers[nPlayer].nView[0];
    CameraController_SetCameraMode(fn_80017028(nView), 6, nPlayer, nView);
}

void STATEFUNC_GreenReversePuttUpdate(int nPlayer) {
    if (!Player_IsCPU(nPlayer) && !(fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(5, 1))) {
        GOLFERSTATE_Pop(nPlayer);
    } else {
        fn_800E3D38(nPlayer, 0);
    }
}

void STATEFUNC_GreenReversePuttExit(int nPlayer) {
    int nView;
    if (GOLFERSTATE_GetPreviousState(nPlayer) == GS_SWING) {
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0, nPlayer, nView);
    }
    fn_800E3D38(nPlayer, 1);
}

void STATEFUNC_KneeCamInit(int nPlayer) {
    int nView = gPlayers[nPlayer].nView[0];
    CameraController_SetCameraMode(fn_80017028(nView), 7, nPlayer, nView);
    Emotion_UpdatePlayerEmotion(nPlayer);
}

// State 8 (camera 7): pops once button 19 is released; while it is held, buttons 11/12 and 13/14
// fire events 0x12..0x15. Button 4 held calls fn_800C6010, else fn_800C60E8.
void STATEFUNC_KneeCamUpdate(int nPlayer) {
    if (!(fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x13, 1))) {
        GOLFERSTATE_Pop(nPlayer);
    } else {
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xB, 1)) {
            EVENT_Trigger(nPlayer, 0x12, 0, -1);
        } else {
            if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xC, 1)) {
                EVENT_Trigger(nPlayer, 0x13, 0, -1);
            }
        }
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xD, 1)) {
            EVENT_Trigger(nPlayer, 0x14, 0, -1);
        } else {
            if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0xE, 1)) {
                EVENT_Trigger(nPlayer, 0x15, 0, -1);
            }
        }
    }
    if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(4, 1)) {
        fn_800C6010(fn_80017028(gPlayers[nPlayer].nView[0]), nPlayer);
    } else {
        fn_800C60E8(fn_80017028(gPlayers[nPlayer].nView[0]), nPlayer);
    }
    TARGET_UpdateMomentums(nPlayer);
}

void STATEFUNC_KneeCamExit(int nPlayer) {
    int nView;
    if (GOLFERSTATE_GetPreviousState(nPlayer) == GS_SWING) {
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0, nPlayer, nView);
    }
}

void STATEFUNC_GreenMorphInit(int nPlayer) {
    int nView = gPlayers[nPlayer].nView[0];
    CameraController_SetCameraMode(fn_80017028(nView), 4, nPlayer, nView);
    fn_800E3D38(nPlayer, 0);
}

// State 9: the putt-line view. Pops when neither button 46 nor 48 is held and the view's f54
// is 1.0. For a human outside a replay, before the swing starts, the caddie updates and
// button 6 re-plans: the game's 0x264 callback (with gpGame+0x284) or a putt aims at the pin,
// anything else re-chooses a target; then Shot_Prepare, the break line, animation 5.
void STATEFUNC_GreenMorphUpdate(int nPlayer) {
    if (!(fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x2E, 1))) {
        if (!(fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x30, 1))) {
            if (1.0f == fn_80017028(gPlayers[nPlayer].nView[0])->f54) {
                GOLFERSTATE_Pop(nPlayer);
                return;
            }
        }
    }
    if (Player_IsCPU(nPlayer)) return;
    if (gSession.bReplay != 0) return;
    if (gPlayers[nPlayer].swing.nState != 0) return;
    Caddie_Update(nPlayer);
    if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(6, 0)) {
        if (gpGame->b284 != 0) {
            if (gpGame->pfn264(nPlayer)) {
                AI_DefaultTarget(nPlayer);
            } else if (gPlayers[nPlayer].nShotKind == SHOT_TYPE_PUTT_e) {
                AI_DefaultTarget(nPlayer);
            } else {
                AI_ChooseTarget(nPlayer);
            }
            Shot_Prepare(nPlayer, 1);
            BreakLine_Reset(gPlayers[nPlayer].nView[0]);
            fn_8001C804(nPlayer, 1, 1);
            fn_800957D8(gPlayers[nPlayer].pChar);
            fn_80095744(gPlayers[nPlayer].pChar, 5);
            fn_80062C38();
            if (gSession.nSplitScreen != 0) {
                fn_80062CB0(gPlayers[nPlayer].nC58, 1);
            }
        }
        gpGame->pfn22C(nPlayer);
    } else {
        GM_CheckForShotChanges(nPlayer);
    }
}

void STATEFUNC_GreenMorphExit(int nPlayer) {
    int  nView;
    fn_80017028(gPlayers[nPlayer].nView[0])->f50 = 1.0f;
    fn_80017028(gPlayers[nPlayer].nView[0])->f54 = 1.0f;
    fn_80017028(gPlayers[nPlayer].nView[0])->f58 = 1.0f;
    fn_80039344(gPlayers[nPlayer].nView[0], 0.0f);
    if (GOLFERSTATE_GetPreviousState(nPlayer) == GS_SWING) {
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0xC, nPlayer, nView);
    } else {
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0, nPlayer, nView);
    }
    fn_800E3D38(nPlayer, 1);
}

// State 12: the ball is away. In a replay with the kept ball unset, a special path; otherwise
// the ball as it lies is kept. Camera 14 unless the swing animation is 11 or the view says no.
void STATEFUNC_SimulateInit(int nPlayer) {
    int     nView;
    Player* p;
    View*   pV;

    pV = fn_80017028(gPlayers[nPlayer].nView[0]);
    p  = &gPlayers[nPlayer];
    if (gSession.bReplay != 0 && p->ballBefore.nState == 0) {
        fn_8006B2C4(nPlayer, 1);
    } else {
        Mem_cpy(&p->ballBefore, &p->ball, sizeof(Ball));
    }
    p->ballBefore.nPlayer = -1;
    if (fn_80095780(gPlayers[nPlayer].pChar) != 11 && fn_80101738() && !fn_800C6CB0()) {
        nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0xE, nPlayer, nView);
    }
    fn_8006ACF8(nPlayer, 0);
    fn_800DB714(nPlayer);
    gPlayers[nPlayer].bRehearsalDone = 0;
    SW_UpdateVibration(nPlayer);
    fn_80062DC0(pV);
    fn_80062DB8(pV, 0);
}

// State 12: the ball is in flight. Once the view's latch (fn_80062DD4) is set and its value
// (fn_80062DCC) passes 0.5: GM_PlayerTookShot, the replay stops, holed -> state 13, else state 14.
// Before that a ball that has come to rest sets the latch (a tap-in, flag 8, becomes LIE_INCUP_e).
// The swing phase keeps running (the spin window); buttons 22/23 (any pad for a CPU) drive
// fn_80045558/fn_80045494. A live human outside split screen (and not while fn_800E430C or
// fn_80100294): button 24 with a replay recorded (and the game allowing it) replays the shot
// (REPLAY_Play, state 11); button 25 with the mulligan allowed takes the shot back.
void STATEFUNC_SimulateUpdate(int nPlayer) {
    u8    bA = 0;
    u8    bB = 0;
    View* pV;

    pV = fn_80017028(gPlayers[nPlayer].nView[0]);
    if (GolfCamera_IsFreezeTimeActive()) return;
    GM_SimulateBallMovement(nPlayer);
    SW_UpdateVibration(nPlayer);
    if (fn_80062DD4(pV) && fn_80062DCC(pV) > 0.5f) {
        GM_PlayerTookShot(nPlayer);
        REPLAY_Stop();
        fn_800DBDA8(nPlayer);
        if (gPlayers[nPlayer].ball.nLie == LIE_INCUP_e) {
            GOLFERSTATE_Switch(GS_IN_THE_HOLE, nPlayer);
        } else {
            GOLFERSTATE_Switch(GS_SHOW_YARDAGE, nPlayer);
        }
        return;
    }
    if (!fn_80062DD4(pV)) {
        s32* pState = &gPlayers[nPlayer].ball.nState;
        if (*pState == 1 || *pState == 5 || *pState == 0) {
            if (*pState != 5) {
                *pState = 0;
            }
            if (gPlayers[nPlayer].uFlags & 8) {
                gPlayers[nPlayer].ball.nLie = LIE_INCUP_e;
            }
            fn_80062DB8(pV, 1);
            return;
        }
    }
    fn_80058F5C(nPlayer);
    fn_8006BB5C(nPlayer);
    if (gpGame->b286 != 0 && !fn_8004560C()) {
        if (Player_IsCPU(nPlayer)) {
            if (fn_80014300(fn_800142AC(0x16, 1))) {
                bA = 1;
            } else if (fn_80014300(fn_800142AC(0x17, 1))) {
                bB = 1;
            }
        } else {
            if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x16, 1)) {
                bA = 1;
            } else {
                if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x17, 1)) {
                    bB = 1;
                }
            }
        }
        fn_80045558(bA, nPlayer);
        if (bA) {
            bB = 0;
        }
        fn_80045494(bB, nPlayer);
    }
    if (Player_IsCPU(nPlayer)) return;
    if (gSession.nSplitScreen != 0) return;
    if (fn_800E430C(nPlayer)) return;
    if (fn_80100294()) return;
    if (gReplayData.bF10 != 0 && gpGame->b287 != 0) {
        if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x18, 0)) &&
            !(gPlayers[nPlayer].uFlags & 8) &&
            (s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_CONCEDED && !fn_800E53B8() &&
            !(gPlayers[nPlayer].pChar->u10 & 0x40)) {
            if (gPlayers[nPlayer].ballBefore.nState != 0) {
                gPlayers[nPlayer].ballBefore.nState = 1;
            }
            if (fn_800C6D28()) {
                fn_800C6DE4();
            }
            if (GolfCamera_IsSuperZoomCamActive()) {
                fn_800C6DFC();
            }
            REPLAY_Play(nPlayer);
            GOLFERSTATE_Switch(GS_REPLAY_SWING, nPlayer);
            return;
        }
    }
    if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x19, 0)) &&
        !(gPlayers[nPlayer].uFlags & 8) && GM_PlayerTakeMulligan(nPlayer)) {
        REPLAY_Stop();
        if (fn_800C6D28()) {
            fn_800C6DE4();
        }
        if (GolfCamera_IsSuperZoomCamActive()) {
            fn_800C6DFC();
        }
    }
}

void STATEFUNC_SimulateExit(int nPlayer) {
    View* pViewObj = fn_80017028(gPlayers[nPlayer].nView[0]);
    fn_80045558(0, nPlayer);
    fn_80045494(0, nPlayer);
    REPLAY_RecordStop();
    fn_80062CE0(0);
    SW_KillVibration(nPlayer);
    fn_800C1790(fn_80017028(gPlayers[nPlayer].nView[0]), nPlayer);
    fn_80062DB8(pViewObj, 0);
}

void STATEFUNC_InTheHoleInit(int nPlayer) {
    fn_80017028(gPlayers[nPlayer].nView[0]);
    Mem_cpy(&gPlayers[nPlayer].ballBefore, &gPlayers[nPlayer].ball, sizeof(Ball));
    Emotion_UpdatePlayerEmotion(nPlayer);
    fn_80067710(nPlayer, 0, 0x21);
    lbl_80281E12 = 1;
    if ((gSession.uFlags & 0x4000) && (gSession.uFlags & 0x8000)) {
        fn_8006AAB4(nPlayer, 2);
    }
}

// State 13: holed out, the golfer reacts. On the first frame the view gets the remove-ball choice
// (GM_ChooseRemoveBallState) and the post-shot animation choice; with gpGame+0x294 (not on script
// cameras 1/4) the reaction animation 9 plays or camera 16 is cut to. When the remove-ball choice
// is set and the camera is not 16, flag bit 2 is set and it is state 17 (fade to remove ball).
// While animation 9 plays, camera 16 is taken once the animation allows, and the reaction shot
// lines up.
void STATEFUNC_InTheHoleUpdate(int nPlayer) {
    View* pV    = fn_80017028(gPlayers[nPlayer].nView[0]);

    if (lbl_80281E12 != 0) {
        lbl_80281E12 = 0;
        fn_800C7168(pV, GM_ChooseRemoveBallState(nPlayer));
        if (fn_800C7170(pV)) {
            GolfCamera_SetPostShowPostShotAnimations(pV, 1);
            return;
        }
        GolfCamera_SetPostShowPostShotAnimations(pV, GM_ShowPostShotAnimation(nPlayer));
        if (gpGame->n294 != 0 && pV->script.nCamera != 1 && pV->script.nCamera != 4) {
            if (!(gPlayers[nPlayer].uFlags & 8) || fn_8006AA9C(nPlayer) == 2) {
                if (fn_80095780(gPlayers[nPlayer].pChar) != 9 && fn_80095798(gPlayers[nPlayer].pChar) != 9 &&
                    GolfCamera_ShowPostShotAnimations(pV)) {
                    fn_80095744(gPlayers[nPlayer].pChar, 9);
                } else {
                    CameraController_SetCameraMode(pV, 0x10, nPlayer, gPlayers[nPlayer].nView[0]);
                }
            }
        }
    }
    if (fn_800C7170(pV) && pV->nCurCamera != 0x10 && pV->script.nCamera != 1 && pV->script.nCamera != 4) {
        gPlayers[nPlayer].uFlags |= 2;
        GOLFERSTATE_Switch(GS_FADE_TO_REMOVE_BALL, nPlayer);
        return;
    }
    if (fn_80095780(gPlayers[nPlayer].pChar) == 9 && pV->nCurCamera != 0x10 && Game_GetMode() != 11 &&
        !fn_800734A0(&gPlayers[nPlayer].pChar->blend)) {
        CameraController_SetCameraMode(pV, 0x10, nPlayer, gPlayers[nPlayer].nView[0]);
    }
    if (fn_80095780(gPlayers[nPlayer].pChar) == 9) {
        if ((fn_80062C1C(gPlayers[nPlayer].pChar) != 0 || fn_80062C10(gPlayers[nPlayer].pChar) != 0) &&
            !fn_800C6604(pV)) {
            GolfCamera_CutToGolferDoneAnimatingCam(pV, nPlayer);
        }
    }
    GM_DoPostShotInHoleUI(nPlayer);
}

void STATEFUNC_ConcededInit(int nPlayer) {
    Vec4  vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    View* pV      = fn_80017028(gPlayers[nPlayer].nView[0]);
    fn_800C7178(pV, nPlayer);
    CameraController_SetCameraMode(pV, 0x19, nPlayer, gPlayers[nPlayer].nView[0]);
    if (pV->script.nCamera == 1 || pV->script.nCamera == 3 || pV->script.nCamera == 4) {
        CameraController_FadeIn(pV, 0.25f, (f32*)&vOffset);
    }
}

void STATEFUNC_ConcededUpdate(int nPlayer) {
    GM_DoPostShotInHoleUI(nPlayer);
}

void STATEFUNC_ConcededExit(int nPlayer) {
}

// Show-yardage enter: keep a copy of the ball where it came to rest, update the emotion and raise
// the first-frame flag (lbl_80281E13).
void STATEFUNC_ShowYardageInit(int nPlayer) {
    fn_80017028(gPlayers[nPlayer].nView[0]);
    Mem_cpy(&gPlayers[nPlayer].ballBefore, &gPlayers[nPlayer].ball, sizeof(Ball));
    Emotion_UpdatePlayerEmotion(nPlayer);
    fn_80067710(nPlayer, 0, 0x21);
    lbl_80281E13 = 1;
}

// .rodata note: defined here, between the constants of the functions around it, to keep the
// original's .rodata order.
const Vec4    lbl_80183620 = {0.0f, 0.0f, 0.0f, 0.5f};   // 0, 0, 0, 0.5 (assigned)

// State 14: the ball has come to rest, not holed. First frame: the post-shot animation choice to
// the view, the reaction (animation 9 or camera 15) and a rumble tick. Camera 15 once the animation
// allows; the reaction shot lines up. Then, unless fn_800E46B4: without fn_800E4254, once the fade
// has completed an OOB ball is replaced (low-IQ penalty) and the golfer's turn ends; before that,
// when the reaction animation is far enough along, it is state 15 (fade to tap-in) if a gimme is
// allowed, else a fade out starts. With fn_800E4254: a human outside split screen can take a
// mulligan (button 25, if allowed), watch the replay (button 24, if recorded and allowed) or
// continue (button 0, fn_800E41D4); a CPU continues on any pad's button 0.
void STATEFUNC_ShowYardageUpdate(int nPlayer) {
    View* pV    = fn_80017028(gPlayers[nPlayer].nView[0]);
    Vec4  vOffset;

    if (lbl_80281E13 != 0) {
        GolfCamera_SetPostShowPostShotAnimations(pV, GM_ShowPostShotAnimation(nPlayer));
        if (fn_80095780(gPlayers[nPlayer].pChar) != 9 && fn_80095798(gPlayers[nPlayer].pChar) != 9 &&
            GolfCamera_ShowPostShotAnimations(pV)) {
            fn_80095744(gPlayers[nPlayer].pChar, 9);
        } else {
            CameraController_SetCameraMode(pV, 0xF, nPlayer, gPlayers[nPlayer].nView[0]);
        }
        SW_UpdateVibration(nPlayer);
        lbl_80281E13 = 0;
    }
    vOffset = lbl_80183620;
    if (fn_80095780(gPlayers[nPlayer].pChar) == 9 && pV->nCurCamera != 0xF && Game_GetMode() != 11 &&
        !fn_800734A0(&gPlayers[nPlayer].pChar->blend)) {
        CameraController_SetCameraMode(pV, 0xF, nPlayer, gPlayers[nPlayer].nView[0]);
    }
    if ((fn_80062C1C(gPlayers[nPlayer].pChar) != 0 || fn_80062C10(gPlayers[nPlayer].pChar) != 0) &&
        fn_80095780(gPlayers[nPlayer].pChar) == 9 && !fn_800C6604(pV)) {
        GolfCamera_CutToGolferDoneAnimatingCam(pV, nPlayer);
    }
    if (fn_800E46B4()) return;
    if (!fn_800E4254(nPlayer)) {
        if (fn_80063C7C(pV)) {
            if (gPlayers[nPlayer].bLowIQPenalty != 0) {
                GM_ReplaceOOBBall(nPlayer);
            }
            GM_EndOfGolferTurn(nPlayer);
            return;
        }
        if (fn_80063C90(pV)) return;
        if (!fn_800C6604(pV) && fn_80095780(gPlayers[nPlayer].pChar) == 9) {
            if (!(fn_80062C28(gPlayers[nPlayer].pChar) >= lbl_80281F78->f170 / 2.0f ||
                  pV->script.f98 > 1.0f)) {
                return;
            }
        }
        fn_80062B78(nPlayer);
        fn_80062B74(nPlayer);
        fn_80062B70();
        if (Gimme_Allowed(nPlayer)) {
            GOLFERSTATE_Switch(GS_FADE_TO_TAP_IN, nPlayer);
        } else {
            CameraController_FadeOut(pV, lbl_80281F78->f170, (f32*)&vOffset);
        }
        return;
    }
    if (!Player_IsCPU(nPlayer) && gSession.nSplitScreen == 0) {
        if (gSession.bReplay == 0) {
            if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x19, 0)) {
                if (!GM_PlayerTakeMulligan(nPlayer)) return;
                fn_80062D0C(nPlayer);
                return;
            }
        }
        if (gReplayData.bF10 != 0) {
            if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x18, 0)) && gpGame->b287 != 0 &&
                !(gPlayers[nPlayer].uFlags & 8) && (s8)GOLFERSTATE_GetCurrentState(nPlayer) != GS_CONCEDED &&
                !fn_800E53B8() &&
                !(gPlayers[nPlayer].pChar->u10 & 0x40)) {
                lbl_80281E10 = 1;
                fn_80062D0C(nPlayer);
                REPLAY_Play(nPlayer);
                GOLFERSTATE_Switch(GS_REPLAY_SWING, nPlayer);
                return;
            }
        }
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0, 0)) {
            fn_800E41D4(nPlayer);
        }
    } else if (!Player_IsCPU(nPlayer)) {
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0, 0)) {
            fn_800E41D4(nPlayer);
        }
    } else {
        if (fn_80014300(fn_800142AC(0, 0))) {
            fn_800E41D4(nPlayer);
        }
    }
}

void STATEFUNC_ShowYardageExit(int nPlayer) {
    if (gPlayers[nPlayer].bC2E == 0 && lbl_80281E10 == 0) {
        GM_BumpBallForObstructions(nPlayer);
    }
    lbl_80281E10 = 0;
}

// State 15 (fade to tap-in), entered only when a gimme is allowed: plan the tap-in from where the
// ball lies. The ball position is saved, the player's ball is moved to the lie, Shot_Plan runs,
// and the ball position is put back.
void STATEFUNC_FadeToTapInInit(int nPlayer) {
    Vec4 vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    f32  vSaved[4];
    fn_80062B64(nPlayer);
    fn_80062B60(nPlayer);
    CameraController_FadeOut(fn_80017028(gPlayers[nPlayer].nView[0]), 0.75f, (f32*)&vOffset);
    Vec3Copy(gPlayers[nPlayer].vBall, vSaved);
    Vec3Copy(gPlayers[nPlayer].ball.vPos, gPlayers[nPlayer].vBall);
    Shot_Plan(nPlayer, 0);
    Vec3Copy(vSaved, gPlayers[nPlayer].vBall);
    gPlayers[nPlayer].bPlanReady = 0;
    gPlayers[nPlayer].bRehearsalDone = 0;
}

// State 15 (fade to tap-in): while the camera moves, the CPU's rehearsal runs on the player -
// human or not, made a CPU for the call, with fn_80050D2C on - until it solves the tap-in. When the
// camera finishes: solved -> camera move and state 16 (the tap-in is played for the player);
// not yet -> the turn ends (GM_EndOfGolferTurn) unless it had been solved before.
void STATEFUNC_FadeToTapInUpdate(int nPlayer) {
    Vec4  vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    f32   vSaved[4];

    fn_80050D2C(1);
    if (gPlayers[nPlayer].bRehearsalDone == 0) {
        int  nController = gPlayers[nPlayer].nController;
        gPlayers[nPlayer].nController = CONTROLLER_CPU;
        Vec3Copy(gPlayers[nPlayer].vBall, vSaved);
        Vec3Copy(gPlayers[nPlayer].ball.vPos, gPlayers[nPlayer].vBall);
        gPlayers[nPlayer].bRehearsalDone = AI_RehearseShot(nPlayer, NULL, 1, CPU_TOLERANCE);
        Vec3Copy(vSaved, gPlayers[nPlayer].vBall);
        gPlayers[nPlayer].nController = nController;
    }
    if (fn_80063C50(fn_80017028(gPlayers[nPlayer].nView[0]))) {
        if (gPlayers[nPlayer].bRehearsalDone != 0) {
            fn_80063CBC(fn_80017028(gPlayers[nPlayer].nView[0]), (f32*)&vOffset);
            GOLFERSTATE_Switch(GS_TAP_IN, nPlayer);
            gPlayers[nPlayer].bPlanReady = 1;
        } else {
            if (gPlayers[nPlayer].bPlanReady == 0) {
                GM_EndOfGolferTurn(nPlayer);
            }
            fn_80063CBC(fn_80017028(gPlayers[nPlayer].nView[0]), (f32*)&vOffset);
        }
    }
    fn_80050D2C(0);
}

void STATEFUNC_FadeToTapInExit(int nPlayer) {
}

void STATEFUNC_TapInInit(int nPlayer) {
    GM_MovePlayerToBall(nPlayer);
    fn_80095744(gPlayers[nPlayer].pChar, 11);
    fn_80062BFC(gPlayers[nPlayer].pChar);
    fn_80062BE8(gPlayers[nPlayer].pChar);
    Character_SelectGameClub(gPlayers[nPlayer].pChar, gPlayers[nPlayer].nClub);
    Character_SelectGameShotType(gPlayers[nPlayer].pChar, gPlayers[nPlayer].nShotKind);
    fn_8001C804(nPlayer, 1, 1);
    gPlayers[nPlayer].uFlags |= 8;
    gPlayers[nPlayer].fA64 = fn_800D0478(nPlayer);
    fn_800E4204();
    fn_80016CFC(gPlayers[nPlayer].nView[0])->bFlagOut = 1;
}

// State 16: a shot the game plays for the player. With animation 11 running and camera 12 set
// (not on script cameras 1, 3, 4), at the ball-hit event (2; at once if there is none) the launch
// is made with the controller set to the CPU for the call (so no error), then it is state 12.
void STATEFUNC_TapInUpdate(int nPlayer) {
    int        nController;
    Vec4       vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    View*      pV      = fn_80017028(gPlayers[nPlayer].nView[0]);
    Character* pChar   = gPlayers[nPlayer].pChar;

    if (fn_80095780(pChar) != 11) return;
    if (pV->nCurCamera != 0xC) {
        CameraController_SetCameraMode(pV, 0xC, nPlayer, gPlayers[nPlayer].nView[0]);
        CameraController_FadeIn(pV, 0.75f, (f32*)&vOffset);
    }
    if (pV->script.nCamera == 1) return;
    if (pV->script.nCamera == 4) return;
    if (pV->script.nCamera == 3) return;
    if (fn_80048574(pChar, 2)) {
        if (!fn_80062BB0(pChar, 2)) return;
        fn_80062B98(pChar, 2);
        nController  = gPlayers[nPlayer].nController;
        gPlayers[nPlayer].nController = CONTROLLER_CPU;
        SW_vImpact(nPlayer);
        gPlayers[nPlayer].nController = nController;
        fn_800A5980((u8)nPlayer);
        REPLAY_ResetController(nPlayer, nController);
        GOLFERSTATE_Switch(GS_SIMULATE, nPlayer);
    } else {
        nController  = gPlayers[nPlayer].nController;
        gPlayers[nPlayer].nController = CONTROLLER_CPU;
        SW_vImpact(nPlayer);
        gPlayers[nPlayer].nController = nController;
        fn_800A5980((u8)nPlayer);
        REPLAY_ResetController(nPlayer, nController);
        GOLFERSTATE_Switch(GS_SIMULATE, nPlayer);
    }
}

void STATEFUNC_TapInExit(int nPlayer) {
}

void STATEFUNC_FadeToRemoveBallInit(int nPlayer) {
    Vec4 vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    CameraController_FadeOut(fn_80017028(gPlayers[nPlayer].nView[0]), 0.5f, (f32*)&vOffset);
}

void STATEFUNC_FadeToRemoveBallUpdate(int nPlayer) {
    Vec4 vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    if (fn_80063C50(fn_80017028(gPlayers[nPlayer].nView[0]))) {
        fn_80063CBC(fn_80017028(gPlayers[nPlayer].nView[0]), (f32*)&vOffset);
        GOLFERSTATE_Switch(GS_REMOVE_BALL, nPlayer);
    }
}

void STATEFUNC_FadeToRemoveBallExit(int nPlayer) {
}

// State 18: holed out. The ball goes to the pin, the lie becomes LIE_INCUP_e, animation 12.
void STATEFUNC_RemoveBallInit(int nPlayer) {
    Ball* pBall       = &gPlayers[nPlayer].ball;
    Ball* pBallBefore = &gPlayers[nPlayer].ballBefore;
    int  nPinSet;
    CourseInfo* pCourse;
    Mem_cpy(pBallBefore, pBall, sizeof(Ball));
    Emotion_UpdatePlayerEmotion(nPlayer);
    nPinSet = Game_CurrentPinSet();
    pCourse = fn_8000C594();
    if (pCourse != NULL) {
        Character_SetPosition(gPlayers[nPlayer].pChar, &pCourse->pin[nPinSet].x, 1);
        Physics_DropBall(pBall, &pCourse->pin[nPinSet].x);
        gPlayers[nPlayer].ball.nLie = LIE_INCUP_e;
        Vec3Copy(pBall->vPos, pBallBefore->vPos);
    }
    fn_80095744(gPlayers[nPlayer].pChar, 12);
}

// State 18, holed out: animation 12, the golfer picks the ball out of the cup and tosses it.
// While the ball is in the hand it follows the hand bone (kept at least a real ball's radius,
// 0.84 in, above the ground). From the animation's event 4 it follows the hand, and at event 3
// (after 4) it is thrown: a real launch of the kept ball along the hand's motion, at 0.5 x 60 x 60
// x FRAME_RATE x (yards moved / 1760) - the hand's speed in miles per hour, halved. Then the thrown
// ball is stepped 20 ticks a frame and the live ball follows it. Camera 16 (with a fade in) once
// animation 12 runs.
void STATEFUNC_RemoveBallUpdate(int nPlayer) {
    View* pV      = fn_80017028(gPlayers[nPlayer].nView[0]);
    Vec4  vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    Ball* pBall;
    f32   vDir[4];
    f32   vPos[4];
    f32   fGround, fSpeed;

    if (fn_80095780(gPlayers[nPlayer].pChar) == 12 && pV->nCurCamera != 0x10) {
        int nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0x10, nPlayer, nView);
        CameraController_FadeIn(fn_80017028(gPlayers[nPlayer].nView[0]), 0.75f, (f32*)&vOffset);
    }
    if (fn_8001DBF4(gPlayers[nPlayer].pChar)) {
        pBall = &gPlayers[nPlayer].ball;
        Vec3Copy(gPlayers[nPlayer].pChar->pModel->pMatrices[fn_8001EED8(gPlayers[nPlayer].pChar->pModel,
                                                                         0x54)][3],
                 gPlayers[nPlayer].ball.vPos);
        Vec3Copy(pBall->vPos, vPos);
        vPos[1] += 30.0f;
        fGround = Ter_GetSupportingGroundHeight(fn_8000C594(), vPos);
        if (!(fGround < -60000.0f)) {
            if (gPlayers[nPlayer].ball.vPos[1] - gRealBallRadiusIn / 36.0f < fGround) {
                gPlayers[nPlayer].ball.vPos[1] = fGround + gRealBallRadiusIn / 36.0f;
            }
        }
    } else if (gSession.fFrameTime > 0.0f) {
        if (fn_80048574(gPlayers[nPlayer].pChar, 4) && fn_80062BB0(gPlayers[nPlayer].pChar, 4)) {
            Player* p     = &gPlayers[nPlayer];
            Ball*   pB    = &p->ball;
            f32*    pPrev = p->ball.vPrev;
            Vec3Copy(pB->vPos, pPrev);
            Character_GetBallOnFingerPosition(gPlayers[nPlayer].pChar, pB->vPos);
            if (fn_80062BB0(gPlayers[nPlayer].pChar, 3)) {
                f32 fT4 = fn_8005CB78(gPlayers[nPlayer].pChar, 4);
                if (fn_8005CB78(gPlayers[nPlayer].pChar, 3) > fT4) {
                    fn_80062DDC(pB->vPos, pPrev, vDir);
                    fSpeed = (f32)fn_80009680(fn_80009744(vDir));
                    if (0.0f != vDir[0] || 0.0f != vDir[1] || 0.0f != vDir[2]) {
                        fn_800BAF04(vDir, vDir);
                    }
                    fSpeed = 60.0f * (60.0f * (FRAME_RATE * (fSpeed / 1760.0f)));
                    fSpeed = fSpeed / 2.0f;
                    fn_80062B98(gPlayers[nPlayer].pChar, 4);
                    gPlayers[nPlayer].ballBefore.bHoled = 0;
                    Physics_ThrowBall(&p->ballBefore, vDir, fSpeed, pB->vPos);
                }
            }
        }
        if (fn_80048574(gPlayers[nPlayer].pChar, 3) && fn_80062BB0(gPlayers[nPlayer].pChar, 3)) {
            f32 fT4 = fn_8005CB78(gPlayers[nPlayer].pChar, 4);
            if (fn_8005CB78(gPlayers[nPlayer].pChar, 3) > fT4) {
                s32 nState = gPlayers[nPlayer].ballBefore.nState;
                if (nState != 1 && nState != 5 && nState != 0) {
                    Player* p;
                    Ball*   pKept;
                    fn_80050D24_SetSimulating(1);
                    p     = &gPlayers[nPlayer];
                    pKept = &p->ballBefore;
                    Physics_Simulate(pKept, 20);
                    Vec3Copy(pKept->vPos, p->ball.vPos);
                    fn_80050D24_SetSimulating(0);
                }
            }
        }
    }
    GM_DoPostShotInHoleUI(nPlayer);
}

// .rodata note: defined here to keep the original's .rodata order.
// 0, 0, 0, 0.5 (assigned, not an initialiser: as one,
//   STATEFUNC_RemoveBallExit drops to 42.5%)
const Vec4    lbl_80183690 = {0.0f, 0.0f, 0.0f, 0.5f};

void STATEFUNC_RemoveBallExit(int nPlayer) {
    View* pV;
    Vec4  vOffset;
    pV      = fn_80017028(gPlayers[nPlayer].nView[0]);
    vOffset = lbl_80183690;
    Mem_cpy(&gPlayers[nPlayer].ballBefore, &gPlayers[nPlayer].ball, sizeof(Ball));
    if (pV->script.nCamera == 3) {
        CameraController_FadeIn(fn_80017028(gPlayers[nPlayer].nView[0]), 0.75f, (f32*)&vOffset);
    }
}

void STATEFUNC_WaitInit(int nPlayer) {
    if (fn_8001707C(gPlayers[nPlayer].nView[0]) == nPlayer) {
        int nView = gPlayers[nPlayer].nView[0];
        CameraController_SetCameraMode(fn_80017028(nView), 0x19, nPlayer, nView);
    }
    Emotion_UpdatePlayerEmotion(nPlayer);
}

void STATEFUNC_WaitUpdate(int nPlayer) {
    fn_80063C90(fn_80017028(gPlayers[nPlayer].nView[0]));
}

// State 20: the hole flyover. Event 0x4A, camera 10 full screen with a fade in on this player's
// view, every other player's views detached, this player attached to both of its views, moved to
// the ball, Shot_Plan with the HUD told, animation 1.
void STATEFUNC_InitialFlyByInit(int nPlayer) {
    Vec4  vOffset = {0.0f, 0.0f, 0.0f, 0.5f};
    int   i, k;

    EVENT_Trigger(nPlayer, 0x4A, 0, -1);
    fn_800170C4(gPlayers[nPlayer].nView[0], 1);
    CameraController_SetCameraMode(fn_80017028(gPlayers[nPlayer].nView[0]), 10, nPlayer,
                                   gPlayers[nPlayer].nView[0]);
    CameraController_FadeIn(fn_80017028(gPlayers[nPlayer].nView[0]), 0.5f, (f32*)&vOffset);
    GolfCamera_SetCameraMatrixMode(0);
    fn_800170F4(gPlayers[nPlayer].nView[0]);
    fn_800171D8(fn_80012EF0(fn_80017004(gPlayers[nPlayer].nView[0])), 0.0f, 0.0f, 1.0f, 1.0f);
    for (i = 0; i < gNumPlayersSetUp; i++) {
        for (k = 0; k < 2; k++) {
            if (gPlayers[i].nView[k] != gPlayers[nPlayer].nView[0]) {
                fn_800170C4(gPlayers[i].nView[k], 0);
            }
        }
    }
    for (k = 0; k < 2; k++) {
        fn_8001704C(gPlayers[nPlayer].nView[k], nPlayer);
    }
    fn_80045824(nPlayer);
    GM_MovePlayerToBall(nPlayer);
    Shot_Plan(nPlayer, 1);
    fn_8001D8DC(nPlayer);
    fn_8001C804(nPlayer, 1, 1);
    fn_800957D8(gPlayers[nPlayer].pChar);
    fn_80095744(gPlayers[nPlayer].pChar, 1);
    fn_8003349C(1.0f, 12.0f, 0.1f);
    GameEffects_ResetGameEffectSettings();
    for (i = 0; i < gNumPlayersSetUp; i++) {
        fn_80016CFC(gPlayers[i].nView[0])->bFlagOut = 0;
    }
}

// State 20 (the hole flyover): pops when the option skips cameras, the camera finishes, or button 0
// is pressed on any pad (fn_80014300); the confirm hooks (fn_80062B88, fn_80062B7C) return 1.
void STATEFUNC_InitialFlyByUpdate(int nPlayer) {
    u8 bDone = 0;
    if (gSession.a8[0] == 0 || !fn_80014300(0)) {
        if (gSession.options.bSkipCameras) {
            bDone = 1;
        } else if (fn_800172C4(fn_80017028(gPlayers[nPlayer].nView[0]))) {
            bDone = 1;
        } else if (!fn_80100294()) {
            if (fn_80014300(fn_800142AC(0, 0)) || fn_80062B90()) {
                if (fn_80062B88(nPlayer)) {
                    bDone = 1;
                    fn_80062B84(0);
                    fn_800A76E4();
                } else if (fn_80062B7C()) {
                    fn_80062B84(0);
                }
            }
        }
    }
    if (bDone) {
        GOLFERSTATE_Pop(nPlayer);
    }
}

// Leaving state 20: every set-up player's view takes this view's camera.
void STATEFUNC_InitialFlyByExit(int nPlayer) {
    int   i;
    View* pSrc;
    View* pDst;
    fn_80017158(gPlayers[nPlayer].nView[0]);
    pSrc = fn_80017028(gPlayers[nPlayer].nView[0]);
    for (i = 0; i < gNumPlayersSetUp; i++) {
        pDst = fn_80017028(gPlayers[i].nView[0]);
        fn_800170C4(gPlayers[i].nView[0], 1);
        pDst->script.nCamera = pSrc->script.nCamera;
        pDst->script.f94     = pSrc->script.f94;
        Vec_Copy(pSrc->script.v40, pDst->script.v40);
        pDst->script.f90     = pSrc->script.f90;
    }
    gpGame->pfn24C(nPlayer);
}

// State 21: each of the player's views saves its camera and takes camera 10.
void STATEFUNC_MidHoleFlyByInit(int nPlayer) {
    View* pV;
    int   nView;
    int   k, j;
    u8    bShared;
    EVENT_Trigger(nPlayer, 0x4A, 0, -1);
    for (k = 0; k < 2; k++) {
        pV      = fn_80017028(gPlayers[nPlayer].nView[k]);
        bShared = 0;
        for (j = 0; j < k; j++) {
            if (pV == fn_80017028(gPlayers[nPlayer].nView[j])) {
                bShared = 1;
            }
        }
        if (!bShared) {
            pV->nSavedCamera = pV->nCurCamera;
            nView = gPlayers[nPlayer].nView[k];
            CameraController_SetCameraMode(fn_80017028(nView), 10, nPlayer, nView);
        }
    }
    GolfCamera_SetCameraMatrixMode(0);
    fn_800E3D38(nPlayer, 0);
    fn_80045824(nPlayer);
    fn_800DC9D4(1);
}

// State 21 (a camera flyover): over when the option skips cameras, the camera finishes, or a
// button is pressed (any pad for a CPU's shot). While fn_80100294() it waits
// for the camera unless cameras are skipped.
void STATEFUNC_MidHoleFlyByUpdate(int nPlayer) {
    u8  bDone = 0;
    u32 uMask;
    if (gSession.options.bSkipCameras) {
        bDone = 1;
    } else if (fn_80100294()) {
        return;
    }
    if (fn_800172C4(fn_80017028(gPlayers[nPlayer].nView[0]))) {
        bDone = 1;
    } else if (!Player_IsCPU(nPlayer)) {
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0, 0)) {
            bDone = 1;
            fn_800A76E4();
        }
    } else {
        if (fn_80014300(fn_800142AC(0, 0))) {
            bDone = 1;
            fn_800A76E4();
        }
    }
    if (bDone) {
        GOLFERSTATE_Pop(nPlayer);
    }
}

// Each of the player's two views goes back to its saved camera, unless an earlier view of the
// player's is the same view.
void STATEFUNC_MidHoleFlyByExit(int nPlayer) {
    View* pV;
    int   nView;
    int   k, j;
    u8    bShared;
    for (k = 0; k < 2; k++) {
        pV      = fn_80017028(gPlayers[nPlayer].nView[k]);
        bShared = 0;
        for (j = 0; j < k; j++) {
            if (pV == fn_80017028(gPlayers[nPlayer].nView[j])) {
                bShared = 1;
            }
        }
        if (!bShared) {
            nView = gPlayers[nPlayer].nView[k];
            CameraController_SetCameraMode(fn_80017028(nView), pV->nSavedCamera, nPlayer, nView);
        }
    }
    fn_800E3D38(nPlayer, 1);
    fn_800DC9D4(0);
}

// State 22: placing the ball (a drop). vBall is put on the ground: the upper of the
// two ground heights under it, unless there is none or it is more than 0.25 above the ball; then
// the lower one (or the ball's own height if that is missing too). Then Shot_Plan with the HUD
// told, the think timer cleared, camera 8, boost and spin reset.
void STATEFUNC_PlaceBallInit(int nPlayer) {
    f32         fTmp[4];
    f32         fLow, fHigh;
    int         i;
    CourseInfo* pCourse;

    gSession.bReplay = 0;
    Vec3Copy(gPlayers[nPlayer].ball.vPos, gPlayers[nPlayer].vBall);
    pCourse = fn_8000C594();
    if (pCourse != NULL) {
        f32 fY;
        Ter_GetEnclosingGroundHeight(pCourse, gPlayers[nPlayer].vBall, &fLow, &fHigh);
        fY = fHigh;
        if (TER_NO_GROUND == fHigh || fHigh > 0.25f + gPlayers[nPlayer].vBall[1]) {
            fY = fLow;
            if (TER_NO_GROUND == fLow) {
                fY = gPlayers[nPlayer].vBall[1];
            }
        }
        gPlayers[nPlayer].vBall[1] = fY;
    }
    Shot_Plan(nPlayer, 1);
    gPlayers[nPlayer].fThinkTime = 0.0f;
    fn_8001C804(nPlayer, 1, 1);
    fn_800957D8(gPlayers[nPlayer].pChar);
    GameEffects_ResetGameEffectSettings();
    fn_800D8D10(nPlayer);
    PlaceBall_ResetMomentums(nPlayer);
    Emotion_UpdatePlayerEmotion(nPlayer);
    fn_80016CFC(gPlayers[nPlayer].nView[0])->bFlagOut = 1;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (gPlayers[i].bPlayerCut == 0 && gPlayers[i].nView[0] == gPlayers[nPlayer].nView[0] &&
            gPlayers[i].ball.nLie != 10 && gPlayers[i].ball.nLie != LIE_GREEN_e &&
            gPlayers[i].ball.nLie != LIE_INCUP_e) {
            fn_80016CFC(gPlayers[nPlayer].nView[0])->bFlagOut = 0;
        }
    }
    fn_80045824(nPlayer);
    CameraController_SetCameraMode(fn_80017028(gPlayers[nPlayer].nView[0]), 8, nPlayer,
                                   gPlayers[nPlayer].nView[0]);
    Vec_Copy(gPlayers[nPlayer].vBall, fTmp);
    PlaceBall_Set(nPlayer, fTmp);
    PlaceBall_SetupTarget(nPlayer);
    SW_vClearBoosts(nPlayer);
}

// State 22 every frame. Button 35 with the "ball can be placed" flag: the ball is dropped at the
// placement point (a class-1 surface gets Physics_InitBall too), vBall follows it, and it is state
// 1 (pre-shot). Otherwise the cursor moves, buttons 26..29 fire events 0x16..0x19, and button 25
// off the tee takes a mulligan (if allowed).
void STATEFUNC_PlaceBallUpdate(int nPlayer) {
    if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x23, 0)) {
        Ball*   pBall;
        Player* p = &gPlayers[nPlayer];
        if (p->uFlagsEF0 & 1) {
            pBall = &p->ball;
            if (Physics_DropBall(pBall, p->vPlacement)) {
                if (gSurfaceTypes[gPlayers[nPlayer].ball.nSurface].nClass == 1) {
                    Physics_InitBall(pBall, pBall->vPos, nPlayer);
                }
                Vec_Copy(pBall->vPos, p->vBall);
                GOLFERSTATE_Switch(GS_PRE_SHOT, nPlayer);
            }
        }
    } else {
        fn_800EDAE0(nPlayer);
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x1A, 1)) {
            EVENT_Trigger(nPlayer, 0x16, 0, -1);
        } else {
            if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x1B, 1)) {
                EVENT_Trigger(nPlayer, 0x17, 0, -1);
            }
        }
        if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x1C, 1)) {
            EVENT_Trigger(nPlayer, 0x18, 0, -1);
        } else {
            if (fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x1D, 1)) {
                EVENT_Trigger(nPlayer, 0x19, 0, -1);
            }
        }
        if ((fn_800136DC(gPlayers[nPlayer].nController) & fn_800142AC(0x19, 0)) &&
            gPlayers[nPlayer].ball.nLie != 0 && GM_PlayerTakeMulligan(nPlayer)) {
            fn_8001D8DC(nPlayer);
            TARGET_SetupTarget(nPlayer);
        }
    }
    PlaceBall_UpdateMomentums(nPlayer, 1.0f);
}

void STATEFUNC_PlaceBallExit(int nPlayer) {
    fn_800E3C0C(1);
}

void fn_80062B60(int nPlayer) {
}

void fn_80062B64(int nPlayer) {
}

void fn_80062B68(int nPlayer) {
}

void fn_80062B6C(int nPlayer) {
}

void fn_80062B70(void) {
}

void fn_80062B74(int nPlayer) {
}

void fn_80062B78(int nPlayer) {
}

u8 fn_80062B7C(void) {
    return 1;
}

void fn_80062B84(int a) {
}

u8 fn_80062B88(int nPlayer) {
    return 1;
}

u8 fn_80062B90(void) {
    return 0;
}

// Clear an animation event's bSet flag.
int fn_80062B98(Character* pChar, u64 uEvent) {
    pChar->events[(int)uEvent].bSet = 0;
    return 0;
}

// An animation event has been reached: it is set and the animation time is past it.
int fn_80062BB0(Character* pChar, u64 uEvent) {
    if (pChar->events[(int)uEvent].bSet != 0 && pChar->fAnimTime >= pChar->events[(int)uEvent].fTime) {
        return 1;
    }
    return 0;
}

void fn_80062BE8(Character* pChar) {
    if (pChar != NULL) {
        pChar->p1794 = NULL;
    }
}

void fn_80062BFC(Character* pChar) {
    if (pChar != NULL) {
        pChar->p1790 = NULL;
    }
}

int fn_80062C10(Character* pChar) {
    return pChar->uFlags & 4;
}

int fn_80062C1C(Character* pChar) {
    return pChar->uFlags & 1;
}

// How far the animation still has to run.
f32 fn_80062C28(Character* pChar) {
    return pChar->fAnimEnd - pChar->fAnimTime;
}

void fn_80062C38(void) {
    fn_800E58B4(7);
}

void fn_80062C5C(void) {
    fn_800E58B4(8);
}

void fn_80062C80(int a, u8 b) {
    fn_80062D38(0x1E, a, b);
}

void fn_80062CB0(int a, u8 b) {
    fn_80062D38(0x14, a, b);
}

void fn_80062CE0(u8 a) {
    fn_80062D6C(0x23, a);
}

void fn_80062D0C(int nPlayer) {
    fn_80062D6C(9, nPlayer + 1);
}

void fn_80062D38(int nMsg, int nA, int nB) {
    fn_800E5998(nMsg, 0, &nA, &nB);
}

void fn_80062D6C(int nMsg, int nValue) {
    fn_800E590C(nMsg, 0, &nValue);
}

void fn_80062D98(void) {
    fn_80098C70();
}

void fn_80062DB8(View* pView, int a) {
    pView->bFade = a;
}

void fn_80062DC0(View* pView) {
    pView->fFade = 0.0f;
}

f32 fn_80062DCC(View* pView) {
    return pView->fFade;
}

u8 fn_80062DD4(View* pView) {
    return pView->bFade;
}

// a - b over three floats (paired singles; the third is a single).
#ifdef __MWERKS__
asm void fn_80062DDC(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_80062DDC(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

void fn_80062E00(void) {
    fn_800BD894();
}

void fn_80062E20(void) {
    fn_800BDA04();
}
