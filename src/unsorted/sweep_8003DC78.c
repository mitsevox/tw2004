// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_8003DC78(void* arg0);
s32 fn_8003DC78(void* arg0) {
    u8 temp_r3;

    temp_r3 = (*(u8*)((u8*)(arg0) + 0xAC));
    if ((temp_r3 == 1) || ((u8) (temp_r3 - 2) <= 4U) || (temp_r3 == 7)) {
        return 1;
    }
    return 0;
}