// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_800588F4(s32, s32, s32, s32);
s32 fn_800E1CE8(s32, s32);
extern s32 gpSaveData;

s32 fn_800E22E4(s32 arg0, s32 arg1, s32 arg2);
s32 fn_800E22E4(s32 arg0, s32 arg1, s32 arg2) {
    s32 temp_r3;

    temp_r3 = fn_800E1CE8(arg1, arg2);
    if (temp_r3 != -1) {
        return fn_800588F4(gpSaveData + (arg0 * 0x10600), 0, temp_r3, gpSaveData);
    }
    return 0U;
}