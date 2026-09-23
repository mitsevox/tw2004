// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281E30;
void fn_800E58B4();

void fn_8006A89C(void);
f32 fn_8006A8A8(u8* p);
void fn_8006A8B0(void);
void fn_8006A89C(void) {
    lbl_80281E30 = 0;
}

f32 fn_8006A8A8(u8* p) {
    return *(f32*)(p + 0x0);
}

void fn_8006A8B0(void) {
    fn_800E58B4(32);
}
