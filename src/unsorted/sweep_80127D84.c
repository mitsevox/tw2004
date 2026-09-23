// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80127F88();
s32 fn_80128CA0();

s32 fn_80127D84(void* arg0, void* arg1);
s32 fn_80127D84(void* arg0, void* arg1) {
    s32 var_r3;
    u32 temp_r0;
    u32 temp_r3;
    u8 var_r31;

    var_r31 = 1;
    if (fn_80127F88() != 0) {
        var_r31 = 0;
    } else if (fn_80127F88(arg1) != 0) {
        var_r31 = 1;
    } else {
        temp_r0 = (*(u32*)((u8*)(arg0) + 0x4C));
        temp_r3 = (*(u32*)((u8*)(arg1) + 0x4C));
        if (temp_r0 < temp_r3) {
            var_r31 = 0;
        } else if (temp_r3 < temp_r0) {
            var_r31 = 1;
        } else if (fn_80128CA0(arg0, arg1, 0) > 0) {
            var_r31 = 0;
        }
    }
    var_r3 = 1;
    if (var_r31 != 0) {
        var_r3 = -1;
    }
    return var_r3;
}