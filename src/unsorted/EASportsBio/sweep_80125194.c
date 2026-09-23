// Small functions found by the sweep (sweep.py). Original file: EASportsBio.c (EA's name, from its asserts; placed by the file map).

#include "game_types.h"

s32 fn_80125354();
s32 fn_801253F0();

s32 fn_80125194(s32 arg0, s32 arg1);
s32 fn_80125194(s32 arg0, s32 arg1) {
    s32 temp_r3;

    temp_r3 = fn_80125354();
    if (temp_r3 == 0) {
        fn_801253F0(arg0, arg1);
    }
    return temp_r3;
}