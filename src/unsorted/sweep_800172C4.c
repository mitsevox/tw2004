// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_800172C4(void* arg0);
s32 fn_800172C4(void* arg0) {
    void* temp_r4;

    temp_r4 = (*(void**)((u8*)(arg0) + 0x130));
    if (temp_r4 == NULL) {
        return 1;
    }
    if ((u8) (*(u8*)((u8*)(temp_r4) + 0xAD)) != 0) {
        return 1;
    }
    if (((u32) (*(u32*)((u8*)(temp_r4) + 0x40)) == 0U) && ((s32) (*(s32*)((u8*)(arg0) + 0x144)) != 1)) {
        return 1;
    }
    return 0;
}