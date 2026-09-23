// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80128054();
s32 fn_80128DD0();

s32 fn_80127E44(void* arg0, void* arg1, s32 arg2);
s32 fn_80127E44(void* arg0, void* arg1, s32 arg2) {
    s32 var_r3;
    u32 temp_r0_2;
    u32 temp_r3_2;
    u8 temp_r0;
    u8 temp_r3;
    u8 var_r0;

    if (fn_80128054() != 0) {
        var_r0 = 0;
    } else if (fn_80128054(arg1) != 0) {
        var_r0 = 1;
    } else {
        temp_r0 = (*(u8*)((u8*)(arg0) + 0x86));
        temp_r3 = (*(u8*)((u8*)(arg1) + 0x86));
        if ((temp_r0 > temp_r3) && (arg2 == 0)) {
            var_r0 = 1;
        } else if ((temp_r3 > temp_r0) && (arg2 == 0)) {
            var_r0 = 0;
        } else {
            temp_r0_2 = (*(u32*)((u8*)(arg0) + 0x80));
            temp_r3_2 = (*(u32*)((u8*)(arg1) + 0x80));
            if (temp_r0_2 > temp_r3_2) {
                var_r0 = 1;
            } else if (temp_r3_2 > temp_r0_2) {
                var_r0 = 0;
            } else if (fn_80128DD0(arg0, arg1) > 0) {
                var_r0 = 1;
            } else {
                var_r0 = 0;
            }
        }
    }
    var_r3 = 1;
    if (var_r0 != 0) {
        var_r3 = -1;
    }
    return var_r3;
}