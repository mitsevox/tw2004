// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_80128F58(u32 arg0, u32 arg1, u32 arg2, u32 arg3);
void fn_80128F58(u32 arg0, u32 arg1, u32 arg2, u32 arg3) {
    u32 temp_r5;
    u32 var_r7;
    u32 var_r8;

    var_r7 = 0;
    var_r8 = 0;
    if (arg3 != 0) {
        temp_r5 = arg0 / arg2;
        if ((u32) (-1U / arg3) >= temp_r5) {
            var_r8 = temp_r5 * arg3;
        } else {
            var_r8 = -1U;
        }
    }
    if (arg1 != 0) {
        if ((u32) (-1U / arg1) >= arg0) {
            var_r7 = arg0 * arg1;
        } else {
            var_r7 = -1U;
        }
    }
    fn_80128468(var_r8, var_r7);
}