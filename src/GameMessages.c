// GameMessages.c (our name): the game's messages to the front end (the HUD and menu screens):
// each sends a message id plus up to eight int-or-float values, and the display queues' handlers.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "game/frontend.h"
#include "frontend/fe.h"

void  UISProcessHint(void* pHandler, int nMsg, int nArgs, MsgArg* pArgs);
void  GUI_CaddieTipWindowIsOpen(void);
void  GUI_CaddieTipWindowClosed(void);
void  GUI_ShowLessonText(int n);
void  GUI_SetControllerPulled(int n);
void  GUI_SetMessageQueHeld(u8 b);
void  GUI_StartAwardUI(void);
void  GUI_MuteForScoreCard(void);
void  GUI_SetUnreadFlag(u8 b);
void  fn_800E5708(void);
void  fn_800E572C(int n);
void  fn_800E573C(void);
void  fn_800E5908(int nMsg);
u8    fn_800E5D90(void);

// GameMessages.c's data, defined last address first (CodeWarrior lays each section out in reverse).
u8  lbl_802822E4;           // pending-message flags, each sent once
s32 lbl_802822E0;           // the value sent with some of them
u8  lbl_80203138[14];       // the tips already shown (game.h)

// Opens the intro popup of a Play Now challenge in the caddie tip window: message 48 with the
// challenge's group (fn_800EAC7C). STATEFUNC_SwingInit calls it at a human's first swing while the
// intro is pending; the swing is held until the UI closes the window (GUI_IsCaddieTipWindowOpen).
void GUI_ShowChallengeIntro(int n) {
    GameMsg_SendInt(48, n);
    lbl_802822BE = 1;
}

// The same intro for a real-time event (mode 24): message 97 with the challenge's group, in the
// caddie tip window, holding the swing.
void GUI_ShowRealtimeEventIntro(int n) {
    GameMsg_SendInt(97, n);
    lbl_802822BE = 1;
}

// Opens a hole contest's intro in the caddie tip window (message 57): n 0 the longest drive, 1
// closest to the pin, 2 the hole-in-one prize. STATEFUNC_SwingInit calls it at a human's tee shot
// on a contest hole, in place of a caddie tip; the swing is held until the UI closes the window
// (GUI_IsCaddieTipWindowOpen).
void GUI_ShowHoleContestIntro(int n) {
    GameMsg_SendInt(57, n);
    lbl_802822BE = 1;
}

// The UI reports the caddie tip window open (menu command 74 with an argument other than 1, after
// it hides the HUD and the target info): the swing is held (GUI_IsCaddieTipWindowOpen).
// GUI_ShowSwingTip's full tip and the intro popups set the same flag.
void GUI_CaddieTipWindowIsOpen(void) {
    lbl_802822BE = 1;
}

// Whether the caddie tip window is up: while it is, STATEFUNC_SwingUpdate skips the swing and the
// aim marker (fn_80067CD4) and green grid are not drawn. On the first call after
// GUI_CaddieTipWindowClosed it instead brings the HUD back (GUI_ShowToggleFullScreenUI(1), the
// single-screen one even in split screen) and the current player's target info (message 0x1E
// through fn_80062C80), and returns 1 once more.
u8 GUI_IsCaddieTipWindowOpen(void) {
    if (lbl_802822BD) {
        GUI_ShowToggleFullScreenUI(1);
        fn_80062C80(gPlayers[lbl_80282278].nC58, 1);
        lbl_802822BD = 0;
        return 1;
    }
    return lbl_802822BE;
}

// The UI reports the caddie tip window closed (menu command 74 with 1): the hold ends, and the next
// GUI_IsCaddieTipWindowOpen brings the HUD and the target info back.
void GUI_CaddieTipWindowClosed(void) {
    lbl_802822BE = 0;
    lbl_802822BD = 1;
}

// Whether the end-of-round scorecard is up (the flag GUI_EndOfGameScorecard and
// GUI_SetEndOfGamePending set; GUI_PauseMenuClosed ends the round on it and clears it).
u8 GUI_IsEndGameUiShowing(void) {
    return lbl_80282282;
}

// Places the target info box (message 0x18): x and y on a 512 x 416 screen (TARGET_RenderBallTarget
// keeps y at most 285) and the HUD view it belongs to: 0 on one screen, 2 for player 0 and 3 for
// player 1 in split screen.
void GUI_MoveTargetInfo(int a, int b, int nPlayer) {
    int nView;
    if (gSession.nSplitScreen) {
        if (nPlayer == 0) {
            nView = 2;
        } else {
            nView = 3;
        }
    } else {
        nView = 0;
    }
    GameMsg_Send3Ints(0x18, a, b, nView);
}

// The target info box's readouts (message 0x19, four floats and an int): the shot's distance, the
// target's height above the ball in feet and in inches (TARGET_RenderBallTarget: 3 and 36 times the
// rise, 0 within 0.015), the share of the club's range (1..100), and the view: 0 on one screen, 0
// for player 0 and 1 for player 1 in split screen (not GUI_MoveTargetInfo's 2 and 3). TW07 takes
// the four readouts first and the player last.
void GUI_UpdateTargetInfo(int nPlayer, f32 a, f32 b, f32 c, f32 d) {
    int nView;
    if (gSession.nSplitScreen) {
        if (nPlayer == 0) {
            nView = 0;
        } else {
            nView = 1;
        }
    } else {
        nView = 0;
    }
    GameMsg_Send5(0x19, 0xF, &a, &b, &c, &d, &nView);
}

// The lessons' text on the HUD (mode 11): message 60 with the text's index (GameMode11's
// lbl_802823F4: the lesson's number less one, or a variant it picks), -1 to hide it.
void GUI_ShowLessonText(int n) {
    GameMsg_SendInt(60, n);
}

// Empty in this build: GM_CheckControllerPulled calls it every frame of a round. Nothing else marks
// a controller pulled (GUI_SetControllerPulled is called only from GUI_OnControllerPresent), so the
// controller-pulled pause never starts.
void GUI_DetectControllerPull(void) {
}

// Marks controller n as pulled out (lbl_80202B88: nine slots, 0..3 used), for
// GUI_OnControllerPresent to clear when it is back.
void GUI_SetControllerPulled(int n) {
    lbl_80202B88[n] = 1;
}

// Controller i is plugged in: uiProcessInterface's input loop calls it every frame for each
// controller present in game type 6, and menu command 61 too. If the controller was marked pulled,
// the mark is cleared; while another is still marked nothing more happens. With none left the
// controller message goes (GUI_SendMessage31) and the pause ends: gSession.nPaused 2 (paused for
// the controllers) is lifted - timer 1 restarted, the pause-menu flag cleared, a GameBreaker
// resumed, the save images parked (fn_8009EF98) - and any other value becomes 1, the pause menu's
// pause. In this build nothing marks a controller pulled (GUI_DetectControllerPull is empty).
void GUI_OnControllerPresent(int i) {
    int k;
    u8* p;
    if (lbl_80202B88[i]) {
        if (i >= 0) {
            lbl_80202B88[i] = 0;
        }
        for (k = 0, p = lbl_80202B88; k < 9; k++, p++) {
            if (*p) {
                GUI_SetControllerPulled(k);
                return;
            }
        }
        GUI_SendMessage31();
        if (gSession.nPaused == 2) {
            if (!TI_bCounterIsRunning(1)) {
                TI_vStartCounter(1);
            }
            lbl_802822DF = 0;
            GameEffects_Pause(0);
            fn_8009EF98();
            gSession.nPaused = 0;
            return;
        }
        gSession.nPaused = 1;
    }
}

// Clears every controller-pulled mark (all nine slots), when the in-game code starts (GO_vInitIG).
void GUI_ClearControllersPulled(void) {
    lbl_80202B88[0] = 0;
    lbl_80202B88[1] = 0;
    lbl_80202B88[2] = 0;
    lbl_80202B88[3] = 0;
    lbl_80202B88[4] = 0;
    lbl_80202B88[5] = 0;
    lbl_80202B88[6] = 0;
    lbl_80202B88[7] = 0;
    lbl_80202B88[8] = 0;
}

// Whether a trophy or record message waits for the HUD: a trophy ball award (display queue 2) or a
// PGA TOUR award (queue 6), or a record in queue 1 (kind below 10; 10 and 11 are the hole contests'
// results). GameEffects' fn_800DC818 reads it.
u8 GUI_AreTrophysOrRecordsQueued(void) {
    int i;
    if (lbl_802822B0 > 0 || lbl_8028229C > 0) {
        return 1;
    }
    if (lbl_802822B8 > 0) {
        for (i = 0; i < lbl_802822B8; i++) {
            if (lbl_802030BC[i].n0 < 10) {
                return 1;
            }
        }
    }
    return 0;
}

// Set by the UI (menu command 145, b 0 or 1): while set, GUI_CheckMessageQue shows nothing and
// reports the HUD busy. GUI_Init clears it.
void GUI_SetMessageQueHeld(u8 b) {
    lbl_802822BC = b;
}

// The UI reports an award display starting (menu command 162, after it hides the HUD prompts):
// GUI_IsAwardUIAnimating is set until a post-shot display starts or finishes (GUI_PostShotUIStart,
// GUI_PostShotUIFinished) or GUI_Init.
void GUI_StartAwardUI(void) {
    lbl_802822DA = 1;
}

// Whether an award display is running (GUI_StartAwardUI): while it is, no mulligan is taken
// (GM_PlayerTakeMulligan) and the replay button is ignored (the show-yardage and simulate states,
// GM_DoPostShotInHoleUI).
u8 GUI_IsAwardUIAnimating(void) {
    return lbl_802822DA;
}

// The UI reports the scorecard up (menu command 163): the game's sounds go off as for the scorecard
// (Gaud_OnScoreCard(1, 0)); GUI_PauseMenuClosed turns them back on.
void GUI_MuteForScoreCard(void) {
    Gaud_OnScoreCard(1, 0);
}

// Set by the UI (menu command 208, b 0 or 1) and cleared by GUI_Init; nothing in this build reads
// the flag (lbl_80282280).
void GUI_SetUnreadFlag(u8 b) {
    lbl_80282280 = b;
}

// Sends front-end message nMsg with three int values.
void GameMsg_Send3Ints(int nMsg, int a, int b, int c) {
    fn_800E5A4C(nMsg, 0, &a, &b, &c);
}

// One of the five HUD readout refreshes GUI_UpdateAllUIData sends before a HUD comes up: message 6,
// no values. TW07 inlines GUI_UpdateGolferUIData and GUI_UpdateCourseUIData among them; which of
// messages 4 and 6 is which is not known.
void GUI_UpdateUIData6(void) {
    fn_800E58B4(6);
}

// The first of the five HUD readout refreshes GUI_UpdateAllUIData sends: message 4, no values (see
// GUI_UpdateUIData6).
void GUI_UpdateUIData4(void) {
    fn_800E58B4(4);
}

// Shows or hides the HUD's tap-in prompt (message 47 with n as a byte); GUI_HideAllHelpTips hides
// it with the mulligan and replay prompts.
void GUI_ToggleTapin(int n) {
    GameMsg_SendInt(47, (n & 0xFF));
}

// Shows an item of display queue 11 (GUI_CheckMessageQue): message 96 with its three values.
// GameMode4 queues its ladder milestones there (kinds 3, 4, 5, 7, 8 and 13, by event).
void GUI_ShowLadderMessage(int nA, int nB, int nC) {
    GameMsg_Send3Ints(96, nA, nB, nC);
}

// Shows an item of display queue 10 (GUI_CheckMessageQue): message 95 with its three values.
// GameModeDriverRTE_QueueWinMessages queues a won real-time event's messages there.
void GUI_ShowEventWonMessage(int nA, int nB, int nC) {
    GameMsg_Send3Ints(95, nA, nB, nC);
}

// Shows an item of display queue 9 (GUI_CheckMessageQue): message 94 with its three values. Nothing
// queues to queue 9 in this build.
void GUI_ShowQueue9Message(int nA, int nB, int nC) {
    GameMsg_Send3Ints(94, nA, nB, nC);
}

// Shows an item of display queue 8 (GUI_CheckMessageQue, which reads its third value one item too
// far): message 93 with its three values. Nothing queues to queue 8 in this build.
void GUI_ShowQueue8Message(int nA, int nB, int nC) {
    GameMsg_Send3Ints(93, nA, nB, nC);
}

// Shows an item of display queue 7 (GUI_CheckMessageQue): message 92 with its three values.
// PlayNow_QueueMedalMessage queues a challenge's medal message there.
void GUI_ShowMedalMessage(int nA, int nB, int nC) {
    GameMsg_Send3Ints(92, nA, nB, nC);
}

// Shows an item of display queue 6 (GUI_CheckMessageQue): message 91 with its three values.
// Earnings.c queues a PGA TOUR award won there (its message index, 0, the profile).
void GUI_ShowTourAwardMessage(int nA, int nB, int nC) {
    GameMsg_Send3Ints(91, nA, nB, nC);
}

void fn_800E55F0(int nA, int nB, int nC) {
    GameMsg_Send3Ints(27, nA, nB, nC);
}

void fn_800E5628(int nA, int nB, int nC) {
    GameMsg_Send3Ints(12, nA, nB, nC);
}

void fn_800E5660(int nA, int nB, int nC) {
    GameMsg_Send3Ints(11, nA, nB, nC);
}

void fn_800E5698(int nA, int nB, int nC) {
    GameMsg_Send3Ints(13, nA, nB, nC);
}

void fn_800E56D0(int nA, int nB, int nC) {
    GameMsg_Send3Ints(10, nA, nB, nC);
}

void fn_800E5708(void) {
    lbl_802822E4 = 0;
}

void fn_800E5714(int n) {
    lbl_802822E4 = (lbl_802822E4 | n);
}

void fn_800E5724(int n) {
    lbl_802822E0 = n;
}

void fn_800E572C(int n) {
    lbl_802822E4 = (lbl_802822E4 ^ n);
}

void fn_800E573C(void) {
    if ((s8) lbl_802822E4 != 0) {
        if (lbl_802822E4 & 1) {
            fn_800E58B4(0x8D);
            fn_800E572C(1);
        }
        if (lbl_802822E4 & 2) {
            fn_800E58B4(0x54);
            fn_800E572C(2);
        }
    }
}

// Sends each pending-message flag that is set, once, and clears it.
void fn_800E5798(void) {
    MsgArg args[3];
    if ((s8)lbl_802822E4 != 0) {
        if (lbl_802822E4 & 1) {
            Mem_set(args, 0, sizeof(args));
            args[0].i = 15;
            args[1].f = 0.0f;
            args[2].i = lbl_802822E0;
            UISProcessHint(lbl_80281F1C->pHandler, 5, 3, args);
            fn_800E572C(1);
        }
        if (lbl_802822E4 & 2) {
            fn_800E58B4(0x3D);
            fn_800E572C(2);
            GameMsg_SendInt(1, 1);
        }
        if (lbl_802822E4 & 4) {
            fn_800E58B4(0x31);
            fn_800E572C(4);
        }
        if (lbl_802822E4 & 8) {
            fn_800E58B4(0x27);
            fn_800E572C(8);
        }
        if (lbl_802822E4 & 0x10) {
            GameMsg_Send2Ints(0x62, 0, lbl_802822E0);
            fn_800E572C(0x10);
        }
        if (lbl_802822E4 & 0x20) {
            GameMsg_SendInt(0x21, lbl_802822E0);
            fn_800E572C(0x20);
        }
    }
}

// Sends a message with no values.
void fn_800E58B4(int nMsg) {
    MsgArg arg;
    fn_800E5908(nMsg);
    Mem_set(&arg, 0, sizeof(arg));
    UISProcessHint(lbl_80281F1C->pHandler, nMsg, 0, &arg);
}

void fn_800E5908(int nMsg) {
}

// Sends a message with one value (bit 0 of uFloats: a float).
void GameMsg_Send1(int nMsg, u32 uFloats, void* pA) {
    MsgArg args[1];
    fn_800E5908(nMsg);
    Mem_set(args, 0, sizeof(args));
    if (uFloats & 1) {
        args[0].f = *(f32*)pA;
    }
    if (!(uFloats & 1)) {
        args[0].i = *(s32*)pA;
    }
    UISProcessHint(lbl_80281F1C->pHandler, nMsg, 1, args);
}

// Two values.
void GameMsg_Send2(int nMsg, u32 uFloats, void* pA, void* pB) {
    MsgArg args[2];
    fn_800E5908(nMsg);
    Mem_set(args, 0, sizeof(args));
    if (uFloats & 1) {
        args[0].f = *(f32*)pA;
    }
    if (!(uFloats & 1)) {
        args[0].i = *(s32*)pA;
    }
    if (uFloats & 2) {
        args[1].f = *(f32*)pB;
    } else {
        args[1].i = *(s32*)pB;
    }
    UISProcessHint(lbl_80281F1C->pHandler, nMsg, 2, args);
}

// Three values.
void fn_800E5A4C(int nMsg, u32 uFloats, void* pA, void* pB, void* pC) {
    MsgArg args[3];
    Mem_set(args, 0, sizeof(args));
    fn_800E5908(nMsg);
    if (uFloats & 1) {
        args[0].f = *(f32*)pA;
    }
    if (!(uFloats & 1)) {
        args[0].i = *(s32*)pA;
    }
    if (uFloats & 2) {
        args[1].f = *(f32*)pB;
    } else {
        args[1].i = *(s32*)pB;
    }
    if (uFloats & 4) {
        args[2].f = *(f32*)pC;
    } else {
        args[2].i = *(s32*)pC;
    }
    UISProcessHint(lbl_80281F1C->pHandler, nMsg, 3, args);
}

// Five values.
void GameMsg_Send5(int nMsg, u32 uFloats, void* pA, void* pB, void* pC, void* pD, void* pE) {
    MsgArg args[5];
    fn_800E5908(nMsg);
    Mem_set(args, 0, sizeof(args));
    if (uFloats & 1) {
        args[0].f = *(f32*)pA;
    }
    if (!(uFloats & 1)) {
        args[0].i = *(s32*)pA;
    }
    if (uFloats & 2) {
        args[1].f = *(f32*)pB;
    } else {
        args[1].i = *(s32*)pB;
    }
    if (uFloats & 4) {
        args[2].f = *(f32*)pC;
    } else {
        args[2].i = *(s32*)pC;
    }
    if (uFloats & 8) {
        args[3].f = *(f32*)pD;
    } else {
        args[3].i = *(s32*)pD;
    }
    if (uFloats & 0x10) {
        args[4].f = *(f32*)pE;
    } else {
        args[4].i = *(s32*)pE;
    }
    UISProcessHint(lbl_80281F1C->pHandler, nMsg, 5, args);
}

// Sends a message with a string.
void fn_800E5C08(int nMsg, char* pStr) {
    MsgString str;
    MsgArg arg;
    fn_800E5908(nMsg);
    Mem_set(&arg, 0, sizeof(arg));
    str.pStr = pStr;
    arg.p = &str;
    ((MsgString*)arg.p)->nLen = strlen(pStr);
    UISProcessHint(lbl_80281F1C->pHandler, nMsg, 1, &arg);
}

u8 fn_800E5C84(void) {
    return fn_800E5D90();
}

// Message 0x42: seven ints and a float (the sixth value).
void fn_800E5CA4(int a, int b, int c, int d, int e, int g, int h, f32 f) {
    MsgArg args[8];
    fn_800E5908(0x42);
    Mem_set(args, 0, sizeof(args));
    args[0].i = a;
    args[1].i = b;
    args[2].i = c;
    args[3].i = d;
    args[4].i = e;
    args[5].f = f;
    args[6].i = g;
    args[7].i = h;
    UISProcessHint(lbl_80281F1C->pHandler, 0x42, 8, args);
}

void fn_800E5D40(int n) {
    GameMsg_SendInt(89, n);
}

void fn_800E5D68(char* pStr) {
    fn_800E5C08(90, pStr);
}

u8 fn_800E5D90(void) {
    return lbl_801D87C0.b0;
}

void fn_800E5DA0(void) {
    lbl_80203138[0] = 0;
    lbl_80203138[1] = 0;
    lbl_80203138[2] = 0;
    lbl_80203138[3] = 0;
    lbl_80203138[4] = 0;
    lbl_80203138[5] = 0;
    lbl_80203138[6] = 0;
    lbl_80203138[7] = 0;
    lbl_80203138[8] = 0;
    lbl_80203138[9] = 0;
    lbl_80203138[10] = 0;
    lbl_80203138[11] = 0;
    lbl_80203138[12] = 0;
    lbl_80203138[13] = 0;
}

// Queues message 0x21 with n. Below 15 (a tip): dropped while bD4 is set, and marked shown
// (except tip 12) so fn_800E5E54 does not pick it again. 15 and up: always queued.
void fn_800E5DE4(int n) {
    if (!gpGame->bD4 || n >= 15) {
        if (n < 15 && n != 12) {
            lbl_80203138[n] = 1;
        }
        fn_800E5714(0x20);
        fn_800E5724(n);
    }
}
