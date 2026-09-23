// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_800B4908();
extern void* lbl_80282220;

s32 fn_800C6CCC(void);
s32 fn_800C6CCC(void) {
    u8 var_r31;

    if ((void* ) lbl_80282220 == NULL) {
        return 0U;
    }
    var_r31 = 0;
    if (((u8) (*(u8*)((u8*)(lbl_80282220) + 0x56)) != 0) && (fn_800B4908() != 0)) {
        var_r31 = 1;
    }
    return var_r31;
}