// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800A4080();
s32 fn_800A7968(u8, s32, u8, u16, s32);
s32 fn_800AD1C8();
s32 fn_800AD698(u8, s32, s32);
extern u8 lbl_80281419;
extern u8 lbl_80282038;
extern s32 lbl_8028204C;
extern u16 lbl_80282050;
extern u8 lbl_80282052;
extern u32 lbl_80282054;

void fn_800A4BDC(void);
void fn_800A4BDC(void) {
    u32 temp_r0;

    fn_800AD1C8();
    fn_800A4080();
    if ((u32) lbl_80282054 != 0U) {
        temp_r0 = lbl_80282054 - 1;
        lbl_80282054 = temp_r0;
        if ((temp_r0 == 0U) && ((u8) lbl_80282038 != 0)) {
            fn_800A7968(lbl_80281419, 0, lbl_80282052, lbl_80282050, lbl_8028204C);
            fn_800AD698(lbl_80281419, 0, 1);
            lbl_80282038 = 0;
        }
    }
}