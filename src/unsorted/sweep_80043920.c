// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800DB1C4(s32);

s32 fn_80043920(void* arg0, s32 arg1);
s32 fn_80043920(void* arg0, s32 arg1) {
    u8 temp_r0;
    u8 temp_r0_2;
    void* temp_r3;
    void* temp_r3_2;

    temp_r3 = (*(void**)((u8*)(arg0) + 0xAC));
    if (temp_r3 == NULL) {
        return 0;
    }
    temp_r0 = (*(u8*)((u8*)(temp_r3) + 0xAC));
    if (((temp_r0 == 0) || (temp_r0 == 0xD)) && (fn_800DB1C4(arg1) < 1)) {
        return 1;
    }
    temp_r3_2 = (*(void**)((u8*)(arg0) + 0xB0));
    if ((temp_r3_2 == NULL) || ((s32) (*(s32*)((u8*)(arg0) + 0xBC)) == 5)) {
        return 0;
    }
    temp_r0_2 = (*(u8*)((u8*)(temp_r3_2) + 0xAC));
    if (((temp_r0_2 == 0) || (temp_r0_2 == 0xD)) && (fn_800DB1C4(arg1) < 1)) {
        return 1;
    }
    return 0;
}