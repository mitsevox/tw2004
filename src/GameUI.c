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

void fn_800E3BEC(void) {
    fn_8001437C();
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

int fn_800E430C(int nPlayer) {
    u8 b = 0;
    if (fn_800E415C() || fn_800E4254(nPlayer)) {
        b = 1;
    }
    return b;
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
