// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800A2100(s32, s32, void*);

void fn_8007C698(void* arg0, s32* arg1);
void fn_8007C698(void* arg0, s32* arg1) {
    s32 temp_r3;
    s32 var_r0;

    temp_r3 = fn_800A2100((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 4)), arg0);
    var_r0 = 1;
    if (temp_r3 != 0) {
        var_r0 = temp_r3;
    }
    *arg1 = var_r0;
}