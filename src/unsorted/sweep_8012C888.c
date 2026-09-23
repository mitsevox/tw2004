// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* lbl_802825B0;

s32 fn_8012C888(s32* arg0);
s32 fn_8012C888(s32* arg0) {
    s32 var_r5;

    var_r5 = 0;
    if (arg0 == NULL) {
        return 3;
    }
    if ((void* ) lbl_802825B0 == NULL) {
        return 0x10;
    }
    *arg0 = (*(s32*)((u8*)(lbl_802825B0) + 0x94));
    if ((s32) *arg0 == 0) {
        var_r5 = 1;
    }
    return var_r5;
}