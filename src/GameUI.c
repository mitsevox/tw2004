// GameUI.c (our name): the in-round display flow after GameRound.c - HUD messages, the end-of-hole
// and end-of-round screens, and the flags that say one of them is up. No TW06 counterpart found.

#include "golfer.h"

void  fn_8001437C(void);
void  fn_800E542C(void);
void  fn_800E5450(void);
void  fn_80062C38(void);
void  fn_80062C5C(void);
void  fn_8006A8B0(void);
void  fn_80062D6C(int a, int nPlayer);
void  fn_800E0AC4(int a);
void  fn_800E0A98(int a);
void  fn_800E5474(int a);
void  fn_800E4204(void);
u8    fn_800E4254(int nPlayer);
int   fn_80062D38(int a, int b, int c);
void  EVENT_Trigger(int nPlayer, int nEvent, int a, int b);
void  fn_800A72EC(int a, int b);
void  GameEffects_ResetGameEffectSettings(void);

extern u8  lbl_80282281;                    // the end-of-round screen is up
extern u8  lbl_80282282;                    // the end-of-hole screen is up
extern s32 lbl_80282288;
extern s32 lbl_8028228C;
extern s32 lbl_80282290;
extern s32 lbl_80282294;
extern s32 lbl_80282298;
extern s32 lbl_8028229C;
extern s32 lbl_802822A4;
extern s32 lbl_802822A8;
extern s32 lbl_802822AC;
extern s32 lbl_802822B0;
extern s32 lbl_802822B4;
extern s32 lbl_802822B8;
extern u8  lbl_802822BE;
extern u8  lbl_802822BF;
extern u8  lbl_802822C0;
extern u8  lbl_802822C1;
extern u8  lbl_802822C2;
extern u8  lbl_802822C3;
extern u8  lbl_802822C4;
extern u8  lbl_802822DB;
extern u8  lbl_802822DC[3];
extern u8  lbl_802822DF;

extern u8  lbl_80282280;
extern s32 lbl_80282284;
extern s32 lbl_802822A0;
extern u8  lbl_802822BC;
extern u8  lbl_802822BD;
extern u32 lbl_802822C8;                    // frame counts when the HUDs last changed
extern u32 lbl_802822CC;
extern u32 lbl_802822D0;
extern u8  lbl_802822D4;
extern u8  lbl_802822D5;
extern u8  lbl_802822D6;
extern u8  lbl_802822D7;                    // the HUD on screen 3 (split screen, player 2)
extern u8  lbl_802822D8;                    // the HUD on screen 2 (split screen, player 1)
extern u8  lbl_802822D9;                    // the HUD on the single screen
extern u8  lbl_802822DA;
void  fn_800E5DA0(u8* p);
void  fn_800E3B04(void);
void  fn_800E3E0C(void);
void  fn_800E5A4C(int a, int b, int* pA, f32* pF, int* pB);

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
    fn_800E5DA0(lbl_802822DC);
    fn_800E3B04();
}

u8    fn_8010D364(void);
u8    fn_8010D390(void);
u8    fn_80126FD8(void);
u8    fn_80127004(void);

// A queued display item: three values whose meaning depends on the queue.
typedef struct UIQueueItem {
    s32 a;
    s32 b;
    s32 c;
} UIQueueItem;

extern UIQueueItem lbl_80203044[10];        // queue 0 (count lbl_802822B4)
extern UIQueueItem lbl_802030BC[10];        // queue 1 (lbl_802822B8)
extern UIQueueItem lbl_80202FCC[10];        // queue 2 (lbl_802822B0)
extern UIQueueItem lbl_80202F54[10];        // queue 3 (lbl_802822AC)
extern UIQueueItem lbl_80202EDC[10];        // queue 4 (lbl_802822A8)
extern UIQueueItem lbl_80202E64[10];        // queue 5 (lbl_802822A0), no duplicates
extern UIQueueItem lbl_80202DEC[10];        // queue 6 (lbl_8028229C)
extern UIQueueItem lbl_80202D74[10];        // queue 7 (lbl_80282298)
extern UIQueueItem lbl_80202CFC[10];        // queue 8 (lbl_80282294)
extern UIQueueItem lbl_80202C84[10];        // queue 9 (lbl_80282290)
extern UIQueueItem lbl_80202C0C[10];        // queue 10 (lbl_8028228C)
extern UIQueueItem lbl_80202B94[10];        // queue 11 (lbl_80282288)

#define UI_PUSH(q, n)       \
    {                       \
        int i = (n)++;      \
        (q)[i].a = a;       \
        (q)[i].b = b;       \
        (q)[i].c = c;       \
    }

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
    lbl_802822D0 = gSession.unk24;
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
    lbl_802822CC = gSession.unk24;
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
    lbl_802822C8 = gSession.unk24;
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

void fn_800E3ECC(void) {
    lbl_80282282 = 1;
    lbl_80282281 = 0;
}

u8 fn_800E415C(void) {
    return lbl_802822DF;
}

// A HUD message: message nMsg for a player, with a number (a distance, strokes over par); not
// in mode 11.
void fn_800E4164(int nMsg, int nPlayer, f32 f) {
    int nWho;
    int nId = nMsg;
    f32 fVal = f;
    if (Game_GetMode() != 11) {
        nWho = nPlayer + 1;
        fn_800E5A4C(5, 2, &nId, &fVal, &nWho);
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
    lbl_802822DA = 0;
    lbl_802822DC[i] = 1;
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

int fn_800E430C(int nPlayer) {
    u8 b = 0;
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
            if (a == lbl_80202E64[i].a) {
                return;
            }
        }
        lbl_80202E64[lbl_802822A0].a = a;
        lbl_80202E64[lbl_802822A0].b = b;
        lbl_80202E64[lbl_802822A0].c = c;
        lbl_802822A0++;
        return;
    }
}

void fn_800E45C0(void) {
    lbl_802822A4 = 14;
}

// Whether any of the display timers or flags is still running.
int fn_800E45CC(void) {
    if (lbl_802822B8 != 0 || lbl_802822B4 != 0 || lbl_802822B0 != 0 || lbl_802822AC != 0 || lbl_802822A8 != 0 ||
        lbl_802822A4 != 0 || lbl_802822C3 != 0 || lbl_802822C4 != 0 || lbl_802822C1 != 0 || lbl_802822C2 != 0 ||
        lbl_8028229C != 0 || lbl_80282298 != 0 || lbl_80282294 != 0 || lbl_80282290 != 0 || lbl_8028228C != 0 ||
        lbl_80282288 != 0 || lbl_802822C0 != 0 || lbl_802822BF != 0) {
        return 1;
    }
    return 0;
}

int fn_800E4BF8(void) {
    u8 b = 0;
    if (lbl_80282282 || lbl_80282281) {
        b = 1;
    }
    return b;
}

// The end-of-round screen: if something is still on screen it is queued (for the human or the
// CPU side); otherwise it opens, with effects reset and event 0x41 for a human.
void fn_800E4C20(u8 bHuman) {
    if (lbl_802822DC[0] || lbl_802822DC[1] || lbl_802822DC[2] || lbl_802822B8 != 0 || lbl_802822B4 != 0 ||
        lbl_802822B0 != 0 || lbl_802822AC != 0 || lbl_802822A8 != 0 || lbl_802822A4 != 0 || lbl_8028229C != 0 ||
        lbl_80282298 != 0 || lbl_80282294 != 0 || lbl_80282290 != 0 || lbl_8028228C != 0 || lbl_80282288 != 0) {
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

void fn_800E4F88(int nPlayer) {
    fn_80062D6C(59, nPlayer);
}

void fn_800E4FB0(u8 a, int b) {
    fn_80062D38(0x22, a, b);
    if (a == 1) {
        lbl_802822BE = 1;
    }
}
