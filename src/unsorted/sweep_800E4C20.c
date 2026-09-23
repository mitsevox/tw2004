// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 Game_GetMode();
s32 fn_80062D38(s32, s32, s32);
s32 EVENT_Trigger(s32, s32, s32, s32);
s32 fn_800A72EC(s32, s32);
s32 GameEffects_ResetGameEffectSettings();
extern s8 lbl_80282281;
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
extern s8 lbl_802822C3;
extern s8 lbl_802822C4;
extern s32 lbl_802822DC;

void fn_800E4C20(u8 arg0);
void fn_800E4C20(u8 arg0) {
    if (((u8) (*(u8*)((u8*)(&lbl_802822DC) + 0)) != 0) || ((u8) (*(u8*)((u8*)(&lbl_802822DC) + 1)) != 0) || ((u8) (*(u8*)((u8*)(&lbl_802822DC) + 2)) != 0) || ((s32) lbl_802822B8 != 0) || ((s32) lbl_802822B4 != 0) || ((s32) lbl_802822B0 != 0) || ((s32) lbl_802822AC != 0) || ((s32) lbl_802822A8 != 0) || ((s32) lbl_802822A4 != 0) || ((s32) lbl_8028229C != 0) || ((s32) lbl_80282298 != 0) || ((s32) lbl_80282294 != 0) || ((s32) lbl_80282290 != 0) || ((s32) lbl_8028228C != 0) || ((s32) lbl_80282288 != 0)) {
        if (arg0 != 0) {
            lbl_802822C3 = 1;
        }
        if (arg0 == 0) {
            lbl_802822C4 = 1;
        }
    } else {
        if (Game_GetMode() != 7) {
            fn_800A72EC(1, 0);
        }
        lbl_80282281 = 1;
        GameEffects_ResetGameEffectSettings();
        if (arg0 != 0) {
            fn_80062D38(0xE, 1, 1);
            EVENT_Trigger(0xFF, 0x41, 0, -1);
            return;
        }
        fn_80062D38(0xE, 1, 0);
    }
}