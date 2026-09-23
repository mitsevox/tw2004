// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_80044E74(void* arg0);
s32 fn_80044E74(void* arg0) {
    u8 temp_r0;

    if (arg0 == NULL) {
        return 0;
    }
    temp_r0 = (*(u8*)((u8*)(arg0) + 0xAD));
    if ((temp_r0 >= 6U) && (temp_r0 <= 0xAU)) {
        return 1;
    }
    return 0;
}