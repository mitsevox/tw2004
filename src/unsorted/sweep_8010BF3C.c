// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* lbl_80282488;

s32 fn_8010BF3C(void);
s32 fn_8010BF3C(void) {
    u8 var_r3;

    var_r3 = 0;
    if (((s32) (*(s32*)((u8*)(lbl_80282488) + 0xA84)) == 0) && ((s32) (*(s32*)((u8*)(lbl_80282488) + 0x980)) == 0)) {
        var_r3 = 1;
    }
    return var_r3;
}