// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80282278;
extern u8 lbl_80282370;
void GOLFERSTATE_Switch();
void fn_800A6278();
void fn_800E3D90();
void fn_800F39CC();

void fn_800F3800(void);
void fn_800F3828(void);
void fn_800F3800(void) {
    fn_800F39CC(-1);
    fn_800A6278();
}

void fn_800F3828(void) {
    GOLFERSTATE_Switch(12, lbl_80282278);   // GS_SIMULATE
    fn_800E3D90();
    fn_800A6278();
    lbl_80282370 = 1;
}
