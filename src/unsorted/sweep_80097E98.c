// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281F78;
void fn_80009E70();
void fn_800988A0();
void fn_800988B8();
void fn_800988CC();

void fn_80097E98(void);
void fn_80097EC4(u8* p0);
void fn_80097E98(void) {
    fn_80009E70(lbl_80281F78);
    lbl_80281F78 = 0;
}

void fn_80097EC4(u8* p0) {
    fn_800988CC(3);
    fn_800988B8(*(f32*)p0, *(f32*)(p0 + 0x4), *(f32*)(p0 + 0x8));
    fn_800988A0(255, 255, 255, 255);
    fn_800988CC(0);
    fn_800988B8(*(f32*)p0, *(f32*)(p0 + 0x4), *(f32*)(p0 + 0x8));
    fn_800988A0(0, 0, 0, 0);
}
