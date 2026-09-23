// Small functions found by the sweep (sweep.py). Original file: MC.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

s32 fn_8009D74C();
s32 fn_8009DBAC(s32, s32);
s32 fn_8009E918(s32, s32);

s32 fn_800A2100(s32 arg0, s32 arg1);
s32 fn_800A2100(s32 arg0, s32 arg1) {
    s32 temp_r31;
    s32 temp_r3;

    temp_r3 = fn_8009D74C();
    if ((temp_r3 != 0) && (temp_r3 != -0x16) && (temp_r3 != -0x1D) && (temp_r3 != -0x18)) {
        return temp_r3;
    }
    temp_r31 = fn_8009E918(arg0, arg1);
    if (temp_r3 == 0) {
        fn_8009DBAC(arg0, arg1);
    }
    return temp_r31;
}