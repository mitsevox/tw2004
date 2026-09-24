// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281F78;
void fn_80009E70();
void fn_800988A0();
void fn_800988B8();
void fn_800988CC();

void fn_80097E98(void);
void fn_80097EC4(f32* pPos);
void fn_80097E98(void) {
    fn_80009E70(lbl_80281F78);
    lbl_80281F78 = 0;
}

// Two vertices at pPos: through matrix 3 in white, through matrix 0 transparent black.
void fn_80097EC4(f32* pPos) {
    fn_800988CC(3);
    fn_800988B8(pPos[0], pPos[1], pPos[2]);
    fn_800988A0(255, 255, 255, 255);
    fn_800988CC(0);
    fn_800988B8(pPos[0], pPos[1], pPos[2]);
    fn_800988A0(0, 0, 0, 0);
}
