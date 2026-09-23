// GameUI.c (our name): the in-round display flow after GameRound.c - HUD messages, the end-of-hole
// and end-of-round screens, and the flags that say one of them is up. No TW06 counterpart found.

#include "golfer.h"
#include "game.h"
#include "engine.h"

void  fn_8001437C(void);
void  fn_8006A8B0(void);

void  fn_800E3E0C(void);

// Clears every display flag and timer at the start of a round.
void fn_800E3B28(void) {
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

typedef struct Vec4 { f32 x, y, z, w; } Vec4;
extern Vec4 lbl_80184D90;
extern char lbl_80281640[8];

void  fn_80095444(int a);
void  fn_80125814(int a);
void  fn_800A7350(int a);

void fn_800E3BEC(void) {
    fn_8001437C();
}

// Show (b = 1) or hide the HUD on the single screen, and when.
void fn_800E3C0C(u8 b) {
    if (b) {
        fn_800E3E0C();
        fn_80062D6C(2, 1);
    } else {
        fn_80062D6C(1, 1);
    }
    lbl_802822D9 = b;
    lbl_802822D0 = gSession.nFrameCount;
}

// The same for split screen's first view.
void fn_800E3C70(u8 b) {
    if (b) {
        fn_800E3E0C();
        fn_80062D6C(2, 2);
    } else {
        fn_80062D6C(1, 2);
    }
    lbl_802822D8 = b;
    lbl_802822CC = gSession.nFrameCount;
}

// And its second view.
void fn_800E3CD4(u8 b) {
    if (b) {
        fn_800E3E0C();
        fn_80062D6C(2, 3);
    } else {
        fn_80062D6C(1, 3);
    }
    lbl_802822D7 = b;
    lbl_802822C8 = gSession.nFrameCount;
}

// Shows or hides a player's HUD.
void fn_800E3D38(int nPlayer, u8 b) {
    if (gSession.nSplitScreen) {
        if (nPlayer == 0) {
            fn_800E3C70(b);
            return;
        }
        fn_800E3CD4(b);
        return;
    }
    fn_800E3C0C(b);
}

// Hides every HUD.
void fn_800E3D90(void) {
    if (gSession.nSplitScreen) {
        fn_800E3C70(0);
        fn_800E3CD4(0);
        return;
    }
    fn_800E3C0C(0);
}

// Whether a player's HUD is up.
u8 fn_800E3DDC(int nPlayer) {
    if (gSession.nSplitScreen) {
        if (nPlayer == 0) {
            return lbl_802822D8;
        }
        return lbl_802822D7;
    }
    return lbl_802822D9;
}

void fn_800E3E0C(void) {
    fn_800E5450();
    fn_800E542C();
    fn_80062C38();
    fn_80062C5C();
    fn_8006A8B0();
}

// Pauses the game (gSession.unk14): the pause menu, sounds held, the HUD's pause flag.
void fn_800E3E3C(void) {
    if (gSession.unk14 == 0) {
        fn_800E5714(4);
        fn_8001437C();
        fn_80062CE0(0);
        if (fn_80095430(1)) {
            fn_80095444(1);
        }
        lbl_802822DF = 1;
        gSession.unk14 = 1;
        fn_800DC9D4(1);
        fn_80125814(0);
        if (fn_800EC550()) {
            fn_800ECBE4();
        }
    }
}

void fn_800E3ECC(void) {
    lbl_80282282 = 1;
    lbl_80282281 = 0;
}

// Unpauses, and finishes whatever the pause was covering: the end-of-hole screen (the hole is
// marked done) or the end-of-round screen (the mode is told; mode 12 replays the same hole,
// otherwise the next selected hole, and every ball goes back on the tee lie).
void fn_800E3EE0(void) {
    int i;
    int j;
    if (gSession.unk14 != 0) {
        fn_800E5714(8);
        if (fn_80100294()) {
            fn_80101EDC();
        }
        if (!fn_800E5C84()) {
            if (!fn_80095430(1)) {
                fn_800953C8(1);
            }
            lbl_802822DF = 0;
            gSession.unk14 = 0;
            fn_800DC9D4(0);
            fn_800A7350(0);
            fn_80125814(1);
        }
        fn_8009EF98();
        if (gSession.bReplay && Game_GetMode() != 11 && Game_GetMode() != 10) {
            fn_80062CE0(1);
        }
        if (fn_800E39F0() && !lbl_80282282) {
            fn_800A72EC(0, 1);
        }
        if (lbl_80282282) {
            fn_800A72EC(0, 0);
            gSession.b12 = 1;
            lbl_80282282 = 0;
            fn_800A76E4();
        }
        if (lbl_80282281) {
            fn_800A72EC(0, 0);
            EVENT_Trigger(0, 0x46, 0, -1);
            gpGame->pfn214();
            if (Game_GetMode() == 12) {
                for (i = 0; i < 5; i++) {
                    fn_800E1018(i, Game_CurHoleIndex());
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
            fn_800A76E4();
        }
        if ((s8)gSession.options.a0[4] == 0) {
            fn_800A76E4();
        }
    }
}

u8 fn_800E415C(void) {
    return lbl_802822DF;
}

// A HUD message: message nMsg for a player, with a number (a distance, strokes over par); not
// in mode 11.
void fn_800E4164(int nMsg, int nPlayer, f32 f) {
    int nWho;
    if (Game_GetMode() != 11) {
        nWho = nPlayer + 1;
        fn_800E5A4C(5, 2, &nMsg, &f, &nWho);
        lbl_802822DB = 1;
    }
}

void fn_800E41C8(void) {
    lbl_802822DB = 1;
}

void fn_800E41D4(int nPlayer) {
    fn_80062D6C(42, nPlayer + 1);
    fn_800E4204();
}

void fn_800E4204(void) {
    fn_800E0AC4(0);
    fn_800E0A98(0);
    fn_800E5474(0);
}

void fn_800E4238(int i) {
    lbl_802822DC[i] = 1;
    lbl_802822DA = 0;
    lbl_802822DB = 0;
}

// Whether a message or screen still holds a player: the message flag, two other screens, or the
// player's own screen slot (slot 0 outside split screen).
u8 fn_800E4254(int nPlayer) {
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

void fn_800E42F4(int i) {
    lbl_802822DC[i] = 0;
    lbl_802822DA = 0;
    lbl_802822DB = 0;
}

u8 fn_800E430C(int nPlayer) {
    int b = 0;
    if (fn_800E415C() || fn_800E4254(nPlayer)) {
        b = 1;
    }
    return b;
}

// Adds an item to one of the twelve display queues (queue 5 ignores an item already queued).
void fn_800E4364(u32 nQueue, int a, int b, int c) {
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

void fn_800E45C0(void) {
    lbl_802822A4 = 14;
}

// Whether any of the display timers or flags is still running.
u8 fn_800E45CC(void) {
    if (lbl_802822B8 != 0 || lbl_802822B4 != 0 || lbl_802822B0 != 0 || lbl_802822AC != 0 ||
        lbl_802822A8 != 0 || lbl_802822A4 != 0 || lbl_802822C3 != 0 || lbl_802822C4 != 0 ||
        lbl_802822C1 != 0 || lbl_802822C2 != 0 || lbl_8028229C != 0 || lbl_80282298 != 0 ||
        lbl_80282294 != 0 || lbl_80282290 != 0 || lbl_8028228C != 0 || lbl_80282288 != 0 ||
        lbl_802822C0 != 0 || lbl_802822BF != 0) {
        return 1;
    }
    return 0;
}

// The display pump, every frame: while a screen slot is up nothing else runs; otherwise pending
// screens and messages go first, then the newest item of the first non-empty queue goes to that
// queue's handler. Returns nonzero while anything is still showing.
//
// Two copy-and-paste slips in the original are kept: queue 4 reads its item's second and third
// values with queue 3's count (always 0 here, so from the entry before the queue), and queue 8
// reads its third value from one entry past the item.
u8 fn_800E46B4(void) {
    u8 bBusy = 0;
    if (lbl_802822BC) {
        return 1;
    }
    if (!lbl_802822DC[0] && !lbl_802822DC[1] && !lbl_802822DC[2]) {
        if (lbl_802822B8 != 0 || lbl_802822B4 != 0 || lbl_802822B0 != 0 || lbl_802822AC != 0 ||
            lbl_802822A8 != 0 || lbl_8028229C != 0 || lbl_80282298 != 0 || lbl_80282294 != 0 ||
            lbl_80282290 != 0 || lbl_8028228C != 0 || lbl_80282288 != 0) {
            if (!lbl_802822DB) {
                fn_800E4164(16, 0, 0.0f);
            }
            return 1;
        }
        if (lbl_802822A4 != 0 && !lbl_802822DB) {
            fn_800E4164(lbl_802822A4, 0, 0.0f);
            lbl_802822A4 = 0;
            return 1;
        }
        if (lbl_802822C3 && !lbl_802822DB) {
            lbl_802822C3 = 0;
            fn_800E4C20(1);
            return 1;
        }
        if (lbl_802822C4 && !lbl_802822DB) {
            lbl_802822C4 = 0;
            fn_800E4C20(0);
            return 1;
        }
        if (lbl_802822C1 && !lbl_802822DB) {
            lbl_802822C1 = 0;
            fn_800E4D94(1);
            return 1;
        }
        if (lbl_802822C2 && !lbl_802822DB) {
            lbl_802822C2 = 0;
            fn_800E4D94(0);
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

u8 fn_800E4BF8(void) {
    int b = 0;
    if (lbl_80282282 || lbl_80282281) {
        b = 1;
    }
    return b;
}

// The end-of-round screen: if something is still on screen it is queued (for the human or the
// CPU side); otherwise it opens, with effects reset and event 0x41 for a human.
void fn_800E4C20(u8 bHuman) {
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
            fn_80062D38(0xE, 1, 1);
            EVENT_Trigger(0xFF, 0x41, 0, -1);
            return;
        }
        fn_80062D38(0xE, 1, 0);
    }
}

void fn_800E4D88(void) {
    lbl_80282281 = 1;
}

// The end-of-hole screen, as fn_800E4C20 does the end-of-round one; in the side-by-side modes
// 22 and 26 both players' cameras are moved first.
void fn_800E4D94(u8 bHuman) {
    Vec4 v;
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
            v = lbl_80184D90;
            fn_80063B98(fn_80017028(gPlayers[0].nView0), 0.0f, (f32*)&v);
            fn_80063B98(fn_80017028(gPlayers[1].nView0), 0.0f, (f32*)&v);
        }
        if (bHuman) {
            fn_80062D38(0xE, 2, 1);
            EVENT_Trigger(0xFF, 0x41, 0, -1);
            return;
        }
        fn_80062D38(0xE, 2, 0);
    }
}

void fn_800E4F88(int nPlayer) {
    fn_80062D6C(59, nPlayer);
}

void fn_800E4FB0(u8 a, int b) {
    fn_80062D38(0x22, a, b);
    if (a == 1) {
        lbl_802822BE = 1;
    }
}
