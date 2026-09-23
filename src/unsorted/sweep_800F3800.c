// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80282278;
extern u8 lbl_80282370;
void SwingStack_Push();
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
    SwingStack_Push(12, lbl_80282278);
    fn_800E3D90();
    fn_800A6278();
    lbl_80282370 = 1;
}
