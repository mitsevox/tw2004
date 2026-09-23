// Small functions found by the sweep (sweep.py). Original file: MC.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

s32 fn_8009D74C();
s32 fn_8009DBAC(s32, s32);
s32 fn_8009F6A0(s32, s32);
s32 fn_8009F734(s32, s32);

s32 fn_800A2194(s32 arg0, s32 arg1);
s32 fn_800A2194(s32 arg0, s32 arg1) {
    s32 temp_r31;
    s32 temp_r3;
    s32 temp_r3_2;

    temp_r3_2 = fn_8009D74C();
    if ((temp_r3_2 != 0) && (temp_r3_2 != -0x16)) {
        return temp_r3_2;
    }
    temp_r3 = fn_8009F734(arg0, arg1);
    if (temp_r3 != 0) {
        if (temp_r3_2 == 0) {
            fn_8009DBAC(arg0, arg1);
        }
        return temp_r3;
    }
    temp_r31 = fn_8009F6A0(arg0, arg1);
    if (temp_r3_2 == 0) {
        fn_8009DBAC(arg0, arg1);
    }
    return temp_r31;
}