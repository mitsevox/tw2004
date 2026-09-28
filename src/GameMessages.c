// GameMessages.c (our name; probably EA's gameui_istudio.c, which TW06 has beside gameui.c in
// golf/gamemode): the in-game UI's side of EA UI Studio, after GameUI.c. Every message the game
// sends the front end (the HUD and menu screens) goes out here, as a message number with up to
// eight int, float or string values handed to UISProcessHint (GameMsg_Send and its siblings).
// Around the senders: the HUD pieces those messages drive (the target info box, the caddie tip
// window that holds the swing, tips, the long-drive panel, the lessons' text), the handler
// GUI_CheckMessageQue gives each display queue's item to, the messages held for the UI's next
// update (GameMsg_SetPending), and the flags the UI's commands set (award display, scorecard,
// controllers pulled). A file of its own: its 0.0f is pooled apart from GameUI.c's.

#include "golfer.h"
#include "game.h"
#include "engine.h"
#include "game/frontend.h"
#include "frontend/fe.h"

void  UISProcessHint(void* pHandler, int nMsg, int nArgs, MsgArg* pArgs);
void  GUI_CaddieTipWindowIsOpen(void);
void  GUI_CaddieTipWindowClosed(void);
void  GUI_ShowLessonText(int nText);
void  GUI_SetControllerPulled(int nController);
void  GUI_SetMessageQueHeld(u8 bHeld);
void  GUI_StartAwardUI(void);
void  GUI_MuteForScoreCard(void);
void  GUI_SetUnreadFlag(u8 b);
void  GameMsg_ClearPending(void);
void  GameMsg_TogglePending(int nBits);
void  GameMsg_SendPendingMenus(void);
void  GameMsg_OnSend(int nMsg);
u8    GUI_GetFadeToBlack(void);

// GameMessages.c's data, defined last address first (CodeWarrior lays each section out in reverse).
u8  gGameMsgPending;        // messages held for the UI's next update (GameMsg_SetPending's bits)
s32 gGameMsgPendingValue;   // the one value sent with pending bits 1, 0x10 and 0x20
u8  gTipShown[14];          // per statistic tip: already shown this hole (GUI_QueueTip, GameAnalysis)

// Opens the intro popup of a Play Now challenge in the caddie tip window: message 48 with the
// challenge's group (fn_800EAC7C). STATEFUNC_SwingInit calls it at a human's first swing while the
// intro is pending; the swing is held until the UI closes the window (GUI_IsCaddieTipWindowOpen).
void GUI_ShowChallengeIntro(int nGroup) {
    GameMsg_SendInt(48, nGroup);
    lbl_802822BE = 1;
}

// The same intro for a real-time event (mode 24): message 97 with the challenge's group, in the
// caddie tip window, holding the swing.
void GUI_ShowRealtimeEventIntro(int nGroup) {
    GameMsg_SendInt(97, nGroup);
    lbl_802822BE = 1;
}

// Opens a hole contest's intro in the caddie tip window (message 57): nContest 0 the longest drive,
// 1 closest to the pin, 2 the hole-in-one prize. STATEFUNC_SwingInit calls it at a human's tee shot
// on a contest hole, in place of a caddie tip; the swing is held until the UI closes the window
// (GUI_IsCaddieTipWindowOpen).
void GUI_ShowHoleContestIntro(int nContest) {
    GameMsg_SendInt(57, nContest);
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
void GUI_MoveTargetInfo(int nX, int nY, int nPlayer) {
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
    GameMsg_Send3Ints(0x18, nX, nY, nView);
}

// The target info box's readouts (message 0x19, four floats and an int): the shot's distance, the
// target's height above the ball in feet and in inches (TARGET_RenderBallTarget: 3 and 36 times the
// rise, 0 within 0.015), the share of the club's range (1..100), and the view: 0 on one screen, 0
// for player 0 and 1 for player 1 in split screen (not GUI_MoveTargetInfo's 2 and 3). TW07 takes
// the four readouts first and the player last.
void GUI_UpdateTargetInfo(int nPlayer, f32 fYardage, f32 fElevFeet, f32 fElevInches, f32 fPowerPercent) {
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
    GameMsg_Send5(0x19, 0xF, &fYardage, &fElevFeet, &fElevInches, &fPowerPercent, &nView);
}

// The lessons' text on the HUD (mode 11): message 60 with the text's index (GameMode11's
// lbl_802823F4: the lesson's number less one, or a variant it picks), -1 to hide it.
void GUI_ShowLessonText(int nText) {
    GameMsg_SendInt(60, nText);
}

// Empty in this build: GM_CheckControllerPulled calls it every frame of a round. Nothing else marks
// a controller pulled (GUI_SetControllerPulled is called only from GUI_OnControllerPresent), so the
// controller-pulled pause never starts.
void GUI_DetectControllerPull(void) {
}

// Marks controller nController as pulled out (lbl_80202B88: nine slots, 0..3 used), for
// GUI_OnControllerPresent to clear when it is back.
void GUI_SetControllerPulled(int nController) {
    lbl_80202B88[nController] = 1;
}

// Controller nController is plugged in: uiProcessInterface's input loop calls it every frame for
// each controller present in game type 6, and menu command 61 too. If it was marked pulled,
// the mark is cleared; while another is still marked nothing more happens. With none left the
// controller message goes (GUI_SendMessage31) and the pause ends: gSession.nPaused 2 (paused for
// the controllers) is lifted - timer 1 restarted, the pause-menu flag cleared, a GameBreaker
// resumed, the save images parked (fn_8009EF98) - and any other value becomes 1, the pause menu's
// pause. In this build nothing marks a controller pulled (GUI_DetectControllerPull is empty).
void GUI_OnControllerPresent(int nController) {
    int i;
    u8* p;
    if (lbl_80202B88[nController]) {
        if (nController >= 0) {
            lbl_80202B88[nController] = 0;
        }
        for (i = 0, p = lbl_80202B88; i < 9; i++, p++) {
            if (*p) {
                GUI_SetControllerPulled(i);
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
// results). GameEffects' GameEffects_ScriptedGBDidIt reads it.
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

// Set by the UI (menu command 145, bHeld 0 or 1): while set, GUI_CheckMessageQue shows nothing and
// reports the HUD busy. GUI_Init clears it.
void GUI_SetMessageQueHeld(u8 bHeld) {
    lbl_802822BC = bHeld;
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
void GameMsg_Send3Ints(int nMsg, int nA, int nB, int nC) {
    GameMsg_Send3(nMsg, 0, &nA, &nB, &nC);
}

// One of the five HUD readout refreshes GUI_UpdateAllUIData sends before a HUD comes up: message 6,
// no values. TW07 inlines GUI_UpdateGolferUIData and GUI_UpdateCourseUIData among them; which of
// messages 4 and 6 is which is not known.
void GUI_UpdateUIData6(void) {
    GameMsg_Send(6);
}

// The first of the five HUD readout refreshes GUI_UpdateAllUIData sends: message 4, no values (see
// GUI_UpdateUIData6).
void GUI_UpdateUIData4(void) {
    GameMsg_Send(4);
}

// Shows or hides the HUD's tap-in prompt (message 47 with bShow as a byte); GUI_HideAllHelpTips
// hides it with the mulligan and replay prompts.
void GUI_ToggleTapin(int bShow) {
    GameMsg_SendInt(47, (bShow & 0xFF));
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

// Shows an item of display queue 4 (GUI_CheckMessageQue, which takes its second and third values at
// queue 3's count): message 27 with its three values. GameMode4_WinEvent queues a golfer a ladder
// event unlocks there (the golfer, 0, the profile).
void GUI_ShowGolferUnlockMessage(int nA, int nB, int nC) {
    GameMsg_Send3Ints(27, nA, nB, nC);
}

// Shows an item of display queue 3 (GUI_CheckMessageQue): message 12 with its three values.
// Earnings.c queues the courses the money earned unlocks there (the course, 0, the profile; kind 7
// with 2 or 3), and GameMode4_WinEvent a ladder event's reward (0x16, the reward, the profile).
void GUI_ShowUnlockMessage(int nA, int nB, int nC) {
    GameMsg_Send3Ints(12, nA, nB, nC);
}

// Shows an item of display queue 1 (GUI_CheckMessageQue): message 11 with its three values.
// Earnings.c queues the records a shot, putt or round sets there (the record kind, its place, the
// profile), and GameManager the hole contests' results (kind 10 the longest drive, 11 closest to
// the pin).
void GUI_ShowRecordMessage(int nA, int nB, int nC) {
    GameMsg_Send3Ints(11, nA, nB, nC);
}

// Shows an item of display queue 2 (GUI_CheckMessageQue): message 13 with its three values.
// Earnings.c queues the trophy balls won there (the award's message index, its money, the profile);
// GameMode4 the ladder's final prize.
void GUI_ShowTrophyMessage(int nA, int nB, int nC) {
    GameMsg_Send3Ints(13, nA, nB, nC);
}

// Shows an item of display queue 0 (GUI_CheckMessageQue shows this queue first): message 10 with
// its three values. The modes and Earnings.c queue prize money there (the prize's message kind, the
// amount, the profile), and the tour card level reached.
void GUI_ShowPrizeMessage(int nA, int nB, int nC) {
    GameMsg_Send3Ints(10, nA, nB, nC);
}

// Drops every pending message (uiProcessInterface's controller reset, fn_800905A8).
void GameMsg_ClearPending(void) {
    gGameMsgPending = 0;
}

// Flags messages to send on the UI's next update (GameMsg_SendPending): bit 1 a conceded hole, 2 a
// restarted hole, 4 the pause menu opened, 8 it closed, 0x10 the zoom camera left, 0x20 a tip.
void GameMsg_SetPending(int nBits) {
    gGameMsgPending = (gGameMsgPending | nBits);
}

// The value sent with pending messages 1 (the conceding player), 0x10 (the player) and 0x20 (the
// tip). There is one slot: two set before the next update both go with the last value.
void GameMsg_SetPendingValue(int nValue) {
    gGameMsgPendingValue = nValue;
}

// Flips pending bits nBits (an exclusive or); the senders call it with a bit they have just sent,
// which clears it.
void GameMsg_TogglePending(int nBits) {
    gGameMsgPending = (gGameMsgPending ^ nBits);
}

// GameMsg_SendPending's counterpart in the menus (game type 3; fn_80090628 picks one): pending bit
// 1 sends message 0x8D and bit 2 message 0x54, no values, each once; the other bits wait.
void GameMsg_SendPendingMenus(void) {
    if ((s8) gGameMsgPending != 0) {
        if (gGameMsgPending & 1) {
            GameMsg_Send(0x8D);
            GameMsg_TogglePending(1);
        }
        if (gGameMsgPending & 2) {
            GameMsg_Send(0x54);
            GameMsg_TogglePending(2);
        }
    }
}

// Sends each pending message once and clears its bit, on the UI's update in a round (fn_80090628):
// bit 1 a conceded hole's post-shot message (message 5 built as GUI_StartPostShotUI does: type 15,
// 0.0, then the value, the player not counted from 1 as GUI_StartPostShotUI counts it), 2 a
// restarted hole (message 0x3D, then message 1 with 1 hides the single-screen HUD), 4 the pause
// menu opened (0x31), 8 closed (0x27), 0x10 the zoom camera left (0x62 with 0 and the player), 0x20
// a tip (0x21 with the tip, GUI_QueueTip).
void GameMsg_SendPending(void) {
    MsgArg args[3];
    if ((s8)gGameMsgPending != 0) {
        if (gGameMsgPending & 1) {
            Mem_set(args, 0, sizeof(args));
            args[0].i = 15;
            args[1].f = 0.0f;
            args[2].i = gGameMsgPendingValue;
            UISProcessHint(lbl_80281F1C->pHandler, 5, 3, args);
            GameMsg_TogglePending(1);
        }
        if (gGameMsgPending & 2) {
            GameMsg_Send(0x3D);
            GameMsg_TogglePending(2);
            GameMsg_SendInt(1, 1);
        }
        if (gGameMsgPending & 4) {
            GameMsg_Send(0x31);
            GameMsg_TogglePending(4);
        }
        if (gGameMsgPending & 8) {
            GameMsg_Send(0x27);
            GameMsg_TogglePending(8);
        }
        if (gGameMsgPending & 0x10) {
            GameMsg_Send2Ints(0x62, 0, gGameMsgPendingValue);
            GameMsg_TogglePending(0x10);
        }
        if (gGameMsgPending & 0x20) {
            GameMsg_SendInt(0x21, gGameMsgPendingValue);
            GameMsg_TogglePending(0x20);
        }
    }
}

// Sends front-end message nMsg with no values.
void GameMsg_Send(int nMsg) {
    MsgArg arg;
    GameMsg_OnSend(nMsg);
    Mem_set(&arg, 0, sizeof(arg));
    UISProcessHint(lbl_80281F1C->pHandler, nMsg, 0, &arg);
}

// Empty in this build: every GameMsg_ sender calls it with the message number before it sends.
void GameMsg_OnSend(int nMsg) {
}

// Sends front-end message nMsg with one value, read through pA: a float when bit 0 of uFloats is
// set, else an int. Every GameMsg_ sender hands its values to the front end's handler
// (lbl_80281F1C->pHandler) through UISProcessHint.
void GameMsg_Send1(int nMsg, u32 uFloats, void* pA) {
    MsgArg args[1];
    GameMsg_OnSend(nMsg);
    Mem_set(args, 0, sizeof(args));
    if (uFloats & 1) {
        args[0].f = *(f32*)pA;
    }
    if (!(uFloats & 1)) {
        args[0].i = *(s32*)pA;
    }
    UISProcessHint(lbl_80281F1C->pHandler, nMsg, 1, args);
}

// Sends front-end message nMsg with two values, read through pA and pB; bits 0 and 1 of uFloats
// mark which are floats (else ints).
void GameMsg_Send2(int nMsg, u32 uFloats, void* pA, void* pB) {
    MsgArg args[2];
    GameMsg_OnSend(nMsg);
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

// Sends front-end message nMsg with three values, read through pA, pB and pC; bits 0..2 of uFloats
// mark which are floats (else ints).
void GameMsg_Send3(int nMsg, u32 uFloats, void* pA, void* pB, void* pC) {
    MsgArg args[3];
    Mem_set(args, 0, sizeof(args));
    GameMsg_OnSend(nMsg);
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

// Sends front-end message nMsg with five values, read through pA..pE; bits 0..4 of uFloats mark
// which are floats (else ints).
void GameMsg_Send5(int nMsg, u32 uFloats, void* pA, void* pB, void* pC, void* pD, void* pE) {
    MsgArg args[5];
    GameMsg_OnSend(nMsg);
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

// Sends front-end message nMsg with one string value (a MsgString: the text and its length, on the
// stack for the call).
void GameMsg_SendString(int nMsg, char* pStr) {
    MsgString str;
    MsgArg arg;
    GameMsg_OnSend(nMsg);
    Mem_set(&arg, 0, sizeof(arg));
    str.pStr = pStr;
    arg.p = &str;
    ((MsgString*)arg.p)->nLen = strlen(pStr);
    UISProcessHint(lbl_80281F1C->pHandler, nMsg, 1, &arg);
}

// Whether the front end's fade to black is running (GUI_GetFadeToBlack): GUI_PauseMenuClosed leaves
// the game paused while it is (quitting the round from the pause menu starts it; fn_8009069C
// unpauses once the screen is black).
u8 GUI_IsFadingToBlack(void) {
    return GUI_GetFadeToBlack();
}

// The long-drive contests' score panel (modes 22 and 26): message 0x42 with eight values in this
// order: the player, the score, the shot's length, its kind, nValue5 (0 from every caller), the
// float fValue6 (0.0), nValue7 (0), and the points the shot scored.
void GUI_UpdateLongDriveScore(int nPlayer, int nScore, int nLength, int nKind, int nValue5, int nValue7,
                              int nPoints, f32 fValue6) {
    MsgArg args[8];
    GameMsg_OnSend(0x42);
    Mem_set(args, 0, sizeof(args));
    args[0].i = nPlayer;
    args[1].i = nScore;
    args[2].i = nLength;
    args[3].i = nKind;
    args[4].i = nValue5;
    args[5].f = fValue6;
    args[6].i = nValue7;
    args[7].i = nPoints;
    UISProcessHint(lbl_80281F1C->pHandler, 0x42, 8, args);
}

// Mode 22's scoring variant for the UI (message 89), once when the mode asks: 1 for variant 0, 2
// for variant 1 (a shot scores only what it adds to the player's best).
void GUI_SendLongDriveVariant(int nVariant) {
    GameMsg_SendInt(89, nVariant);
}

// Mode 22's text for the UI (message 90 with a string): GameMode22_ShowDrivesLeft sends the number
// n4 less the current player's nEA0.
void GUI_SendLongDriveText(char* pStr) {
    GameMsg_SendString(90, pStr);
}

// The front end's fade-to-black flag (lbl_801D87C0.b0): set when the round or the menus are left,
// and fn_8009069C darkens the screen while it is.
u8 GUI_GetFadeToBlack(void) {
    return lbl_801D87C0.b0;
}

// Marks every statistic tip as not shown yet (gTipShown), from GUI_Init at the start or restart
// of a hole: each tip can come once a hole.
void GUI_ClearShownTips(void) {
    gTipShown[0] = 0;
    gTipShown[1] = 0;
    gTipShown[2] = 0;
    gTipShown[3] = 0;
    gTipShown[4] = 0;
    gTipShown[5] = 0;
    gTipShown[6] = 0;
    gTipShown[7] = 0;
    gTipShown[8] = 0;
    gTipShown[9] = 0;
    gTipShown[10] = 0;
    gTipShown[11] = 0;
    gTipShown[12] = 0;
    gTipShown[13] = 0;
}

// Flags tip nTip to be shown on the UI's next update (pending bit 0x20: message 0x21 with nTip).
// Below 15 a statistic tip (fn_800E5E54's pick, through the menu's tip command): dropped during a
// playoff (gpGame->bD4), else marked shown (tip 12 excepted) so it is not picked again this hole. 15
// and up, a GameBreaker's tip (event.c: 15 plus the lowest effect bit set): always flagged.
void GUI_QueueTip(int nTip) {
    if (!gpGame->bD4 || nTip >= 15) {
        if (nTip < 15 && nTip != 12) {
            gTipShown[nTip] = 1;
        }
        GameMsg_SetPending(0x20);
        GameMsg_SetPendingValue(nTip);
    }
}
