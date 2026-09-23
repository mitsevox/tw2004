// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800B6594(s32);
s32 fn_800B67EC();
s32 fn_800B68B4(s32, s32, s32);
extern s32 lbl_80281EF8;
extern s32 lbl_80281EFC;
extern s8 lbl_80281F00;
extern s32 lbl_80281F0C;

void fn_8008F35C(void);
void fn_8008F35C(void) {
    lbl_80281F00 = 0;
    fn_800B68B4(lbl_80281F0C, lbl_80281EF8, lbl_80281EFC);
    fn_800B67EC();
    fn_800B6594(lbl_80281EF8);
}