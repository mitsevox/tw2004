// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80007328();
s32 fn_80007368();

s32 fn_800B67B4(void* arg0);
s32 fn_800B67B4(void* arg0) {
    s32 temp_r31;

    fn_80007368();
    temp_r31 = (*(s32*)((u8*)(arg0) + 0x28));
    fn_80007328();
    return temp_r31;
}