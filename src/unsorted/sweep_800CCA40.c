// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_800CCA40(void** arg0);
s32 fn_800CCA40(void** arg0) {
    void* temp_r3;
    void* temp_r3_2;

    temp_r3_2 = *arg0;
    if (temp_r3_2 == NULL) {
        return 0;
    }
    temp_r3 = (*(void**)((u8*)(temp_r3_2) + 0x48));
    if (temp_r3 != NULL) {
        return (*(s32*)((u8*)(temp_r3) + 0x48));
    }
    return 0;
}