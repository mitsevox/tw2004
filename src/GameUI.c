// GameUI.c (EA's name: the GUI_ functions it shares with TW07's GameUI.c come in the same order):
// the in-game HUD, after GameRound.c - showing and hiding each view's HUD, the pause menu,
// post-shot messages, the twelve message queues, the end-of-hole and end-of-round scorecards, and
// the flags that say one of them is up.

#include "golfer.h"
#include "game.h"
#include "engine.h"

void  fn_8001437C(void);
void  fn_8006A8B0(void);

void  GUI_UpdateAllUIData(void);

// GameUI.c's data (declared in game.h), defined last address first: CodeWarrior lays each section
// out in reverse order of definition.
UIQueueItem lbl_802030BC[UI_QUEUE_LEN];
UIQueueItem lbl_80203044[UI_QUEUE_LEN];
UIQueueItem lbl_80202FCC[UI_QUEUE_LEN];
UIQueueItem lbl_80202F54[UI_QUEUE_LEN];
UIQueueItem lbl_80202EDC[UI_QUEUE_LEN];
UIQueueItem lbl_80202E64[UI_QUEUE_LEN];
UIQueueItem lbl_80202DEC[UI_QUEUE_LEN];
UIQueueItem lbl_80202D74[UI_QUEUE_LEN];
UIQueueItem lbl_80202CFC[UI_QUEUE_LEN];
UIQueueItem lbl_80202C84[UI_QUEUE_LEN];
UIQueueItem lbl_80202C0C[UI_QUEUE_LEN];
UIQueueItem lbl_80202B94[UI_QUEUE_LEN];
u8          lbl_80202B88[9];    // only GameMessages.c uses it, but it lies in this file's .bss

char lbl_80281640[8] = "";      // an empty string sent with message 0x53

u8  lbl_802822DF;
u8  lbl_802822DC[3];
u8  lbl_802822DB;
u8  lbl_802822DA;
u8  lbl_802822D9;
u8  lbl_802822D8;
u8  lbl_802822D7;
u8  lbl_802822D6;
u8  lbl_802822D5;
u8  lbl_802822D4;
u32 lbl_802822D0;
u32 lbl_802822CC;
u32 lbl_802822C8;
u8  lbl_802822C4;
u8  lbl_802822C3;
u8  lbl_802822C2;
u8  lbl_802822C1;
u8  lbl_802822C0;
u8  lbl_802822BF;
u8  lbl_802822BE;
u8  lbl_802822BD;
u8  lbl_802822BC;
s32 lbl_802822B8;
s32 lbl_802822B4;
s32 lbl_802822B0;
s32 lbl_802822AC;
s32 lbl_802822A8;
s32 lbl_802822A4;
s32 lbl_802822A0;
s32 lbl_8028229C;
s32 lbl_80282298;
s32 lbl_80282294;
s32 lbl_80282290;
s32 lbl_8028228C;
s32 lbl_80282288;
s32 lbl_80282284;
u8  lbl_80282282;
u8  lbl_80282281;
u8  lbl_80282280;

// Clears every display flag, queue count and pending message at the start of a hole
// (GM_InitForHole) and when it restarts (GM_RestartHole); also clears GameMessages' lbl_80203138
// flags (fn_800E5DA0) and sends UI message 31 (fn_800E3B04).
void GUI_Init(void) {
    lbl_802822DF = 0;
    lbl_802822DC[0] = 0;
    lbl_802822DC[1] = 0;
    lbl_802822DC[2] = 0;
    lbl_802822DB = 0;
    lbl_802822DA = 0;
    lbl_802822D9 = 0;
    lbl_802822D8 = 0;
    lbl_802822D7 = 0;
    lbl_802822D6 = 0;
    lbl_802822D5 = 0;
    lbl_802822D4 = 0;
    lbl_802822B8 = 0;
    lbl_802822B4 = 0;
    lbl_802822B0 = 0;
    lbl_802822AC = 0;
    lbl_802822A8 = 0;
    lbl_802822A4 = 0;
    lbl_802822A0 = 0;
    lbl_8028229C = 0;
    lbl_80282298 = 0;
    lbl_80282294 = 0;
    lbl_80282290 = 0;
    lbl_8028228C = 0;
    lbl_80282288 = 0;
    lbl_80282284 = 0;
    lbl_80282282 = 0;
    lbl_80282281 = 0;
    lbl_802822C4 = 0;
    lbl_802822C3 = 0;
    lbl_802822C2 = 0;
    lbl_802822C1 = 0;
    lbl_802822C0 = 0;
    lbl_802822BF = 0;
    lbl_802822BE = 0;
    lbl_802822BD = 0;
    lbl_802822BC = 0;
    lbl_80282280 = 0;
    fn_800E5DA0();
    fn_800E3B04();
}

u8    fn_8010D364(void);
u8    fn_8010D390(void);
u8    fn_80126FD8(void);
u8    fn_80127004(void);

#define UI_PUSH(q, n)       \
    {                       \
        int i = (n)++;      \
        (q)[i].n0 = a;       \
        (q)[i].n4 = b;       \
        (q)[i].n8 = c;       \
    }

void  fn_800A7350(int a);

// Shuts the in-game HUD down: it only stops every controller's rumble (fn_8001437C).
void GUI_DeInit(void) {
    fn_8001437C();
}

// Shows (b = 1: readouts refreshed by GUI_UpdateAllUIData, then UI message 2) or hides (message 1)
// the single-screen HUD, view 1; keeps the state for GUI_UIVisible and the frame count of the
// toggle.
void GUI_ShowToggleFullScreenUI(u8 b) {
    if (b) {
        GUI_UpdateAllUIData();
        GameMsg_SendInt(2, 1);
    } else {
        GameMsg_SendInt(1, 1);
    }
    lbl_802822D9 = b;
    lbl_802822D0 = gSession.nFrameCount;
}

// The same for split screen's first view.
void GUI_ShowTogglePlayer1UI(u8 b) {
    if (b) {
        GUI_UpdateAllUIData();
        GameMsg_SendInt(2, 2);
    } else {
        GameMsg_SendInt(1, 2);
    }
    lbl_802822D8 = b;
    lbl_802822CC = gSession.nFrameCount;
}

// And its second view.
void GUI_ShowTogglePlayer2UI(u8 b) {
    if (b) {
        GUI_UpdateAllUIData();
        GameMsg_SendInt(2, 3);
    } else {
        GameMsg_SendInt(1, 3);
    }
    lbl_802822D7 = b;
    lbl_802822C8 = gSession.nFrameCount;
}

// Shows or hides a player's HUD.
void GUI_ToggleUI(int nPlayer, u8 b) {
    if (gSession.nSplitScreen) {
        if (nPlayer == 0) {
            GUI_ShowTogglePlayer1UI(b);
            return;
        }
        GUI_ShowTogglePlayer2UI(b);
        return;
    }
    GUI_ShowToggleFullScreenUI(b);
}

// Hides every HUD.
void GUI_HideAllToggleUI(void) {
    if (gSession.nSplitScreen) {
        GUI_ShowTogglePlayer1UI(0);
        GUI_ShowTogglePlayer2UI(0);
        return;
    }
    GUI_ShowToggleFullScreenUI(0);
}

// Whether a player's HUD is up.
u8 GUI_UIVisible(int nPlayer) {
    if (gSession.nSplitScreen) {
        if (nPlayer == 0) {
            return lbl_802822D8;
        }
        return lbl_802822D7;
    }
    return lbl_802822D9;
}

// Refreshes every HUD readout (five updates; TW07 inlines the course, golfer, lie, shot and wind
// ones here); the show functions call it before a HUD comes up.
void GUI_UpdateAllUIData(void) {
    fn_800E5450();
    fn_800E542C();
    fn_80062C38();
    fn_80062C5C();
    fn_8006A8B0();
}

// Opens the pause menu, once (nothing while already paused): pending UI message 0x31 flagged
// (fn_800E5714(4)), the controllers' rumble stopped, message 0x23 with 0, timer 1 stopped, the
// pause flag GUI_IsPauseMenuOpen returns and gSession.nPaused set, a GameBreaker paused, EASBio's
// play state 0, and in a GameMode5 challenge fn_800ECBE4.
void GUI_OpenPauseMenu(void) {
    if (gSession.nPaused == 0) {
        fn_800E5714(4);
        fn_8001437C();
        fn_80062CE0(0);
        if (TI_bCounterIsRunning(1)) {
            TI_sStopCounter(1);
        }
        lbl_802822DF = 1;
        gSession.nPaused = 1;
        fn_800DC9D4(1);
        EASBio_SetGamePlayState(0);
        if (GM5_IsChallengeRunning()) {
            fn_800ECBE4();
        }
    }
}

// Marks the end-of-round screen as up (and the end-of-hole one not) without showing it, so the next
// GUI_PauseMenuClosed ends the round; a menu command uses it in mode 9.
void GUI_SetEndOfGamePending(void) {
    lbl_80282282 = 1;
    lbl_80282281 = 0;
}

// Closes the pause menu (nothing when not paused) and finishes what the pause was covering. Unless
// a menu screen is still up (fn_800E5C84) it unpauses: timer 1 restarted, the pause flags cleared,
// a GameBreaker resumed, sound unpaused, EASBio's play state 1. Then the save images are parked
// (fn_8009EF98) and message 0x23 with 1 goes out in a replay outside modes 10 and 11. After the
// end-of-round screen gSession.b12 = 1 ends the round. After the end-of-hole screen: event 0x46 and
// the mode's pfn214, then in mode 12 every player's scores for the hole cleared and a hole load
// asked for (fn_8006F4B4), the same load in a playoff (bD4) or with b134, otherwise
// GM_GotoNextSelectedHole; every ball's lie goes back to 0. The game's sounds come back on after
// either screen (and, with the music, in the skill-zone modes unless the round is ending);
// commentary stops after either screen, and whenever options byte 4 is 0.
void GUI_PauseMenuClosed(void) {
    int i;
    int j;
    if (gSession.nPaused != 0) {
        fn_800E5714(8);
        if (fn_80100294()) {
            fn_80101EDC();
        }
        if (!fn_800E5C84()) {
            if (!TI_bCounterIsRunning(1)) {
                TI_vStartCounter(1);
            }
            lbl_802822DF = 0;
            gSession.nPaused = 0;
            fn_800DC9D4(0);
            fn_800A7350(0);
            EASBio_SetGamePlayState(1);
        }
        fn_8009EF98();
        if (gSession.bReplay && Game_GetMode() != 11 && Game_GetMode() != 10) {
            fn_80062CE0(1);
        }
        if (GM_Currently_SkillZoneMode() && !lbl_80282282) {
            fn_800A72EC(0, 1);
        }
        if (lbl_80282282) {
            fn_800A72EC(0, 0);
            gSession.b12 = 1;
            lbl_80282282 = 0;
            Gaud_StopComment();
        }
        if (lbl_80282281) {
            fn_800A72EC(0, 0);
            EVENT_Trigger(0, 0x46, 0, -1);
            gpGame->pfn214();
            if (Game_GetMode() == 12) {
                for (i = 0; i < 5; i++) {
                    GM_ClearPlayerHoleData(i, Game_CurHoleIndex());
                }
                fn_8006F4B4();
            } else if (gpGame->bD4 || gpGame->b134) {
                fn_8006F4B4();
            } else {
                GM_GotoNextSelectedHole();
            }
            lbl_80282281 = 0;
            for (j = 0; j < gNumPlayersSetUp; j++) {
                gPlayers[j].ball.nLie = 0;
            }
            Gaud_StopComment();
        }
        if ((s8)gSession.options.a0[4] == 0) {
            Gaud_StopComment();
        }
    }
}

u8 GUI_IsPauseMenuOpen(void) {
    return lbl_802822DF;
}

// Shows a post-shot message on the HUD (TW07: type, yardage, player): UI message 5 with the message
// type nMsg, the float f (a distance, strokes over par) and the player counted from 1, and a
// post-shot display flagged as requested (GUI_IsPostShotUIAnimating). Nothing in mode 11 (the
// lessons).
void GUI_StartPostShotUI(int nMsg, int nPlayer, f32 f) {
    int nWho;
    if (Game_GetMode() != 11) {
        nWho = nPlayer + 1;
        fn_800E5A4C(5, 2, &nMsg, &f, &nWho);
        lbl_802822DB = 1;
    }
}

// Marks a post-shot display as requested (the flag GUI_StartPostShotUI sets), so
// GUI_IsPostShotUIAnimating reports it until the UI starts or finishes it.
void GUI_FlagPostShotRequest(void) {
    lbl_802822DB = 1;
}

// Moves a player's post-shot display on (message 42) and hides the HUD prompts.
void GUI_AdvancePostShotUI(int nPlayer) {
    GameMsg_SendInt(42, nPlayer + 1);
    GUI_HideAllHelpTips();
}

// Hides the HUD's button prompts: mulligan (message 29), replay (46) and tap-in (47); TW07 inlines
// GUI_ToggleMulligan, GUI_ToggleReplay and GUI_ToggleTapin here.
void GUI_HideAllHelpTips(void) {
    fn_800E0AC4(0);
    fn_800E0A98(0);
    fn_800E5474(0);
}

// The UI reports a post-shot display showing in screen slot i (the player's in split screen, else
// 0); the request flags clear.
void GUI_PostShotUIStart(int i) {
    lbl_802822DC[i] = 1;
    lbl_802822DA = 0;
    lbl_802822DB = 0;
}

// Whether a post-shot display still holds the player: one has been requested (GUI_StartPostShotUI,
// GUI_FlagPostShotRequest), mode 26's or mode 22's end-of-game countdown is running, or the UI
// reports one showing in the player's screen slot (slot 0 outside split screen).
u8 GUI_IsPostShotUIAnimating(int nPlayer) {
    if (lbl_802822DB) {
        return 1;
    }
    if (fn_8010D364() && fn_8010D390()) {
        return 1;
    }
    if (fn_80126FD8() && fn_80127004()) {
        return 1;
    }
    if (gSession.nSplitScreen) {
        return lbl_802822DC[nPlayer];
    }
    return lbl_802822DC[0];
}

// The UI reports the post-shot display in screen slot i gone; the request flags clear too.
void GUI_PostShotUIFinished(int i) {
    lbl_802822DC[i] = 0;
    lbl_802822DA = 0;
    lbl_802822DB = 0;
}

u8 GUI_IsPausedOrPostShotUIAnimating(int nPlayer) {
    int b = 0;
    if (GUI_IsPauseMenuOpen() || GUI_IsPostShotUIAnimating(nPlayer)) {
        b = 1;
    }
    return b;
}

// Adds an item (three values a, b, c) to display queue nQueue, 0..11 (TW07:
// GUI_QueUIMessageMessage). Queue 5 skips an item whose a is already queued; no queue checks for
// room (UI_QUEUE_LEN items each). GUI_CheckMessageQue shows the items, newest first.
void GUI_QueueMessage(u32 nQueue, int a, int b, int c) {
    int i;
    switch (nQueue) {
    case 0:
        UI_PUSH(lbl_80203044, lbl_802822B4);
        return;
    case 1:
        UI_PUSH(lbl_802030BC, lbl_802822B8);
        return;
    case 2:
        UI_PUSH(lbl_80202FCC, lbl_802822B0);
        return;
    case 3:
        UI_PUSH(lbl_80202F54, lbl_802822AC);
        return;
    case 4:
        UI_PUSH(lbl_80202EDC, lbl_802822A8);
        return;
    case 6:
        UI_PUSH(lbl_80202DEC, lbl_8028229C);
        return;
    case 7:
        UI_PUSH(lbl_80202D74, lbl_80282298);
        return;
    case 8:
        UI_PUSH(lbl_80202CFC, lbl_80282294);
        return;
    case 9:
        UI_PUSH(lbl_80202C84, lbl_80282290);
        return;
    case 10:
        UI_PUSH(lbl_80202C0C, lbl_8028228C);
        return;
    case 11:
        UI_PUSH(lbl_80202B94, lbl_80282288);
        return;
    case 5:
        for (i = 0; i < lbl_802822A0; i++) {
            if (a == lbl_80202E64[i].n0) {
                return;
            }
        }
        lbl_80202E64[lbl_802822A0].n0 = a;
        lbl_80202E64[lbl_802822A0].n4 = b;
        lbl_80202E64[lbl_802822A0].n8 = c;
        lbl_802822A0++;
        return;
    }
}

// Queues the golfers-tied HUD message (post-shot message 14), which GUI_CheckMessageQue shows once
// no screen slot is up; the modes call it when a game ends level and goes to a playoff.
void GUI_GolfersTiedUIMessage(void) {
    lbl_802822A4 = 14;
}

// Whether anything waits: a queued item (not queue 5), the pending message or a deferred screen.
u8 GUI_GetUIMessageQued(void) {
    if (lbl_802822B8 != 0 || lbl_802822B4 != 0 || lbl_802822B0 != 0 || lbl_802822AC != 0 ||
        lbl_802822A8 != 0 || lbl_802822A4 != 0 || lbl_802822C3 != 0 || lbl_802822C4 != 0 ||
        lbl_802822C1 != 0 || lbl_802822C2 != 0 || lbl_8028229C != 0 || lbl_80282298 != 0 ||
        lbl_80282294 != 0 || lbl_80282290 != 0 || lbl_8028228C != 0 || lbl_80282288 != 0 ||
        lbl_802822C0 != 0 || lbl_802822BF != 0) {
        return 1;
    }
    return 0;
}

// The HUD message pump, every frame; returns nonzero while anything is still showing (always while
// lbl_802822BC is set). With no screen slot up, queued items only raise message 16, and the pending
// message and the deferred scorecards and messages go up one at a time. With a slot up, the newest
// item of the first non-empty queue, taken in the order 0, 2, 1, 3, 4, 6..11 (queue 5 never), goes
// to that queue's handler. Two copy-and-paste slips in the original are kept (marked inside): queue
// 4 reads its item's second and third values with queue 3's count (always 0 there, so from the
// entry before the queue), and queue 8 reads its third value from one entry past the item.
u8 GUI_CheckMessageQue(void) {
    u8 bBusy = 0;
    if (lbl_802822BC) {
        return 1;
    }
    if (!lbl_802822DC[0] && !lbl_802822DC[1] && !lbl_802822DC[2]) {
        if (lbl_802822B8 != 0 || lbl_802822B4 != 0 || lbl_802822B0 != 0 || lbl_802822AC != 0 ||
            lbl_802822A8 != 0 || lbl_8028229C != 0 || lbl_80282298 != 0 || lbl_80282294 != 0 ||
            lbl_80282290 != 0 || lbl_8028228C != 0 || lbl_80282288 != 0) {
            if (!lbl_802822DB) {
                GUI_StartPostShotUI(16, 0, 0.0f);
            }
            return 1;
        }
        if (lbl_802822A4 != 0 && !lbl_802822DB) {
            GUI_StartPostShotUI(lbl_802822A4, 0, 0.0f);
            lbl_802822A4 = 0;
            return 1;
        }
        if (lbl_802822C3 && !lbl_802822DB) {
            lbl_802822C3 = 0;
            GUI_BetweenHolesScorecard(1);
            return 1;
        }
        if (lbl_802822C4 && !lbl_802822DB) {
            lbl_802822C4 = 0;
            GUI_BetweenHolesScorecard(0);
            return 1;
        }
        if (lbl_802822C1 && !lbl_802822DB) {
            lbl_802822C1 = 0;
            GUI_EndOfGameScorecard(1);
            return 1;
        }
        if (lbl_802822C2 && !lbl_802822DB) {
            lbl_802822C2 = 0;
            GUI_EndOfGameScorecard(0);
            return 1;
        }
        if (lbl_802822C0 && !lbl_802822DB) {
            lbl_802822C0 = 0;
            fn_800E58B4(0x50);
            return 1;
        }
        if (lbl_802822BF && !lbl_802822DB) {
            lbl_802822BF = 0;
            fn_800E5C08(0x53, lbl_80281640);
            return 1;
        }
    }
    if (lbl_802822B4 != 0) {
        fn_800E56D0(lbl_80203044[lbl_802822B4 - 1].n0, lbl_80203044[lbl_802822B4 - 1].n4,
                    lbl_80203044[lbl_802822B4 - 1].n8);
        bBusy = 1;
        lbl_802822B4--;
    } else if (lbl_802822B0 != 0) {
        fn_800E5698(lbl_80202FCC[lbl_802822B0 - 1].n0, lbl_80202FCC[lbl_802822B0 - 1].n4,
                    lbl_80202FCC[lbl_802822B0 - 1].n8);
        bBusy = 1;
        lbl_802822B0--;
    } else if (lbl_802822B8 != 0) {
        fn_800E5660(lbl_802030BC[lbl_802822B8 - 1].n0, lbl_802030BC[lbl_802822B8 - 1].n4,
                    lbl_802030BC[lbl_802822B8 - 1].n8);
        bBusy = 1;
        lbl_802822B8--;
    } else if (lbl_802822AC != 0) {
        fn_800E5628(lbl_80202F54[lbl_802822AC - 1].n0, lbl_80202F54[lbl_802822AC - 1].n4,
                    lbl_80202F54[lbl_802822AC - 1].n8);
        bBusy = 1;
        lbl_802822AC--;
    } else if (lbl_802822A8 != 0) {
        // EA bug: n4 and n8 are taken at queue 3's count (lbl_802822AC), not this queue's
        fn_800E55F0(lbl_80202EDC[lbl_802822A8 - 1].n0, lbl_80202EDC[lbl_802822AC - 1].n4,
                    lbl_80202EDC[lbl_802822AC - 1].n8);
        bBusy = 1;
        lbl_802822A8--;
    } else if (lbl_8028229C != 0) {
        fn_800E55B8(lbl_80202DEC[lbl_8028229C - 1].n0, lbl_80202DEC[lbl_8028229C - 1].n4,
                    lbl_80202DEC[lbl_8028229C - 1].n8);
        bBusy = 1;
        lbl_8028229C--;
    } else if (lbl_80282298 != 0) {
        fn_800E5580(lbl_80202D74[lbl_80282298 - 1].n0, lbl_80202D74[lbl_80282298 - 1].n4,
                    lbl_80202D74[lbl_80282298 - 1].n8);
        bBusy = 1;
        lbl_80282298--;
    } else if (lbl_80282294 != 0) {
        // EA bug: n8 is taken one item past the newest (no - 1)
        fn_800E5548(lbl_80202CFC[lbl_80282294 - 1].n0, lbl_80202CFC[lbl_80282294 - 1].n4,
                    lbl_80202CFC[lbl_80282294].n8);
        bBusy = 1;
        lbl_80282294--;
    } else if (lbl_80282290 != 0) {
        fn_800E5510(lbl_80202C84[lbl_80282290 - 1].n0, lbl_80202C84[lbl_80282290 - 1].n4,
                    lbl_80202C84[lbl_80282290 - 1].n8);
        bBusy = 1;
        lbl_80282290--;
    } else if (lbl_8028228C != 0) {
        fn_800E54D8(lbl_80202C0C[lbl_8028228C - 1].n0, lbl_80202C0C[lbl_8028228C - 1].n4,
                    lbl_80202C0C[lbl_8028228C - 1].n8);
        bBusy = 1;
        lbl_8028228C--;
    } else if (lbl_80282288 != 0) {
        fn_800E54A0(lbl_80202B94[lbl_80282288 - 1].n0, lbl_80202B94[lbl_80282288 - 1].n4,
                    lbl_80202B94[lbl_80282288 - 1].n8);
        bBusy = 1;
        lbl_80282288--;
    } else if (lbl_80282282 || lbl_80282281) {
        bBusy = 1;
    }
    return bBusy;
}

// Whether the end-of-hole or end-of-round scorecard is up (the flags GUI_BetweenHolesScorecard and
// GUI_EndOfGameScorecard set; GUI_PauseMenuClosed acts on them and clears them).
u8 GUI_ScoreCardUp(void) {
    int b = 0;
    if (lbl_80282282 || lbl_80282281) {
        b = 1;
    }
    return b;
}

// Shows the end-of-hole scorecard (UI message 14 with kind 1 and bHuman). While a screen slot is up
// or anything is queued it is deferred (GUI_CheckMessageQue shows it later); otherwise the game's
// sounds go off (not in mode 7), the end-of-hole flag is set for GUI_ScoreCardUp and
// GUI_PauseMenuClosed, the GameBreaker settings are reset, and with bHuman event 0x41 fires.
void GUI_BetweenHolesScorecard(u8 bHuman) {
    if (lbl_802822DC[0] || lbl_802822DC[1] || lbl_802822DC[2] || lbl_802822B8 != 0 || lbl_802822B4 != 0 ||
        lbl_802822B0 != 0 || lbl_802822AC != 0 || lbl_802822A8 != 0 || lbl_802822A4 != 0 ||
        lbl_8028229C != 0 || lbl_80282298 != 0 || lbl_80282294 != 0 || lbl_80282290 != 0 ||
        lbl_8028228C != 0 || lbl_80282288 != 0) {
        if (bHuman) {
            lbl_802822C3 = 1;
        }
        if (!bHuman) {
            lbl_802822C4 = 1;
        }
    } else {
        if (Game_GetMode() != 7) {
            fn_800A72EC(1, 0);
        }
        lbl_80282281 = 1;
        GameEffects_ResetGameEffectSettings();
        if (bHuman) {
            GameMsg_Send2Ints(0xE, 1, 1);
            EVENT_Trigger(0xFF, 0x41, 0, -1);
            return;
        }
        GameMsg_Send2Ints(0xE, 1, 0);
    }
}

// Marks the end-of-hole screen as up without showing it, so the next GUI_PauseMenuClosed moves on
// (modes 5 and 9, when a hole ends early or restarts).
void GUI_SetEndOfHolePending(void) {
    lbl_80282281 = 1;
}

// Shows the end-of-round scorecard (UI message 14 with kind 2), as GUI_BetweenHolesScorecard does
// the end-of-hole one, setting the end-of-round flag; in modes 22 and 26 in split screen both
// players' views have their black fade cleared at once first (CameraController_FadeIn over 0 s).
void GUI_EndOfGameScorecard(u8 bHuman) {
    if (lbl_802822DC[0] || lbl_802822DC[1] || lbl_802822DC[2] || lbl_802822B8 != 0 || lbl_802822B4 != 0 ||
        lbl_802822B0 != 0 || lbl_802822AC != 0 || lbl_802822A8 != 0 || lbl_802822A4 != 0 ||
        lbl_8028229C != 0 || lbl_80282298 != 0 || lbl_80282294 != 0 || lbl_80282290 != 0 ||
        lbl_8028228C != 0 || lbl_80282288 != 0) {
        if (bHuman) {
            lbl_802822C1 = 1;
        }
        if (!bHuman) {
            lbl_802822C2 = 1;
        }
    } else {
        if (Game_GetMode() != 7) {
            fn_800A72EC(1, 0);
        }
        lbl_80282282 = 1;
        GameEffects_ResetGameEffectSettings();
        if ((Game_GetMode() == 26 || Game_GetMode() == 22) && gSession.nSplitScreen) {
            f32 v[4] = {0.0f, 0.0f, 0.0f, 1.0f};
            CameraController_FadeIn(ViewController_GetCameraController(gPlayers[0].nView[0]), 0.0f, v);
            CameraController_FadeIn(ViewController_GetCameraController(gPlayers[1].nView[0]), 0.0f, v);
        }
        if (bHuman) {
            GameMsg_Send2Ints(0xE, 2, 1);
            EVENT_Trigger(0xFF, 0x41, 0, -1);
            return;
        }
        GameMsg_Send2Ints(0xE, 2, 0);
    }
}

// Sends UI message 59 with a controller number: uiProcessInterface's input loop (fn_8008F820)
// calls it in game type 6 when that controller has held button 0x20 of the button table for more
// than 10 frames.
void fn_800E4F88(int nPlayer) {
    GameMsg_SendInt(59, nPlayer);
}

// Shows a swing tip: message 0x22 with the kind a and the tip number b. Kind 1 is the full tip,
// which also holds shot input (lbl_802822BE, see GameMessages.c); 2 is the short reminder.
void GUI_ShowSwingTip(u8 a, int b) {
    GameMsg_Send2Ints(0x22, a, b);
    if (a == 1) {
        lbl_802822BE = 1;
    }
}
