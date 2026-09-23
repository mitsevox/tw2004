// Small functions found by the sweep (sweep.py). Original file: EASportsBio.c (EA's name, from its asserts; placed by the file map).

#include "game_types.h"

s32 fn_8012D6C8();
s32 fn_80124A98();
s32 fn_80124AAC();
s32 fn_80125354();
s32 fn_801253F0();

s32 fn_80125434(s32 arg0, s32 arg1);
s32 fn_80125434(s32 arg0, s32 arg1) {
    s32 temp_r31;
    s32 temp_r3;

    temp_r3 = fn_80125354();
    if (temp_r3 != 0) {
        return temp_r3;
    }
    if (fn_8012D6C8() != 0) {
        return fn_80124A98();
    }
    temp_r31 = fn_80124AAC();
    fn_801253F0(arg0, arg1);
    return temp_r31 & ((s32) (-temp_r31 | temp_r31) >> 0x1F);
}