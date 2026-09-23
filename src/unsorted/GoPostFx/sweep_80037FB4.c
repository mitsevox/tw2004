// Small functions found by the sweep (sweep.py). Original file: GoPostFx.c (EA's name, from its asserts; also in TW06; placed by the file map).

#include "game_types.h"

void fn_80038010(u8 arg0, s32 arg1, s32 arg2);

void fn_80037FB4(u8 arg0, s32 arg1);
void fn_80037FB4(u8 arg0, s32 arg1) {
    s32 var_r31;

    var_r31 = 0;
    do {
        fn_80038010(arg0, var_r31, arg1);
        var_r31 += 1;
    } while (var_r31 < 4);
}