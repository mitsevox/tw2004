// Small functions found by the sweep (sweep.py). Original file: startUp.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

s32 fn_8009527C(s32);
extern s32 lbl_80282104;
extern u8 lbl_80282110;

s32 fn_800B0624(void);
s32 fn_800B0624(void) {
    if ((u8) lbl_80282110 != 0) {
        lbl_80282110 = 0;
        fn_8009527C(lbl_80282104);
    }
    return 1;
}