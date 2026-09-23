// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_8004562C(void* arg0);
s32 fn_8004562C(void* arg0) {
    u8 temp_r3;

    temp_r3 = (*(u8*)((u8*)(arg0) + 0xAC));
    if ((temp_r3 == 0) || ((u8) (temp_r3 - 0xD) <= 2U) || (temp_r3 == 0x17)) {
        return 1;
    }
    return 0;
}