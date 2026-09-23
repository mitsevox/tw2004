// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80189D30[];
extern s32 lbl_80281F70;
void fn_80005AE8();
s32 fn_80009B34();
void fn_80009E70();

void fn_80097208(void);
void fn_80097250(void);
void fn_80097208(void) {
    s32 t0;
    t0 = fn_80009B34(2408, 2, 32, lbl_80189D30, 85);
    lbl_80281F70 = t0;
    fn_80005AE8(t0, 0, 2408);
}

void fn_80097250(void) {
    fn_80009E70(lbl_80281F70);
    lbl_80281F70 = 0;
}
