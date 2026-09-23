// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800A8394(s32, s32, s32, s32);
u32 fn_800AD674();

void fn_800ADA28(s32 arg0, s32 arg1, s32 arg2, s32 arg3);
void fn_800ADA28(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    if (fn_800AD674() != 0U) {
        fn_800A8394(arg0, arg1, arg2, arg3);
    }
}