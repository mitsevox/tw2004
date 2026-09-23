// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80136058(s32, s32);
extern s32 lbl_802820DC;
extern s32 lbl_802820E0;

void fn_800AF2DC(u8 arg0);
void fn_800AF2DC(u8 arg0) {
    if (arg0 != 0) {
        fn_80136058(0, 0);
        return;
    }
    fn_80136058(lbl_802820DC, lbl_802820E0);
}