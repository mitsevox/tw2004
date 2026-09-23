// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_801288DC(s32, s32, u32, u32, u32, u32, u32);
s32 fn_8012CCD8(s32);

s32 fn_8012E8A8(s32 arg0, s32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5, u32 arg6);
s32 fn_8012E8A8(s32 arg0, s32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5, u32 arg6) {
    s32 temp_r3;

    if ((arg1 == 0) || (arg2 == 0U) || (arg3 == 0U) || (arg4 == 0U) || (arg5 == 0U) || (arg6 == 0U)) {
        return 3;
    }
    temp_r3 = fn_8012CCD8(2);
    if (temp_r3 != 0) {
        return temp_r3;
    }
    return fn_801288DC(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
}