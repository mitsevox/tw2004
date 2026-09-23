// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_8003A76C(void* arg0);
s32 fn_8003A76C(void* arg0) {
    u8 temp_r0;

    temp_r0 = (*(u8*)((u8*)(arg0) + 0xAD));
    if ((temp_r0 == 3) || (temp_r0 == 1) || ((temp_r0 >= 0x1DU) && (temp_r0 <= 0x21U)) || ((temp_r0 >= 0x28U) && (temp_r0 <= 0x2DU)) || (temp_r0 == 0xD) || (temp_r0 == 0x1C) || (temp_r0 == 0x22)) {
        return 1;
    }
    return 0;
}