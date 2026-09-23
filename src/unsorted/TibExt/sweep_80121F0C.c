// Small functions found by the sweep (sweep.py). Original file: TibExt.c (EA's name, from its asserts; also in TW06; placed by the file map).

#include "game_types.h"

s32 fn_80077B08();
u8 fn_80102204();
u8 fn_80102228();

s32 fn_80121F0C(s32 arg0);
s32 fn_80121F0C(s32 arg0) {
    s32 temp_r30;
    u8 var_r31;

    temp_r30 = fn_80077B08();
    var_r31 = 0;
    if ((fn_80102204(temp_r30, arg0) == 0) && (fn_80102228(temp_r30, arg0) != 0)) {
        var_r31 = 1;
    }
    return var_r31;
}