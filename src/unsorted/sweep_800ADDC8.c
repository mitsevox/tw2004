// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void* fn_800AD674();

void fn_800ADDC8(s32 arg0, u8 arg1, s32 arg2);
void fn_800ADDC8(s32 arg0, u8 arg1, s32 arg2) {
    s32 (*temp_r12)(s32, u8, s32);
    s32 temp_r0;
    void* temp_r3;

    temp_r3 = fn_800AD674();
    if (temp_r3 != NULL) {
        temp_r0 = 1 << arg1;
        (*(u8*)((u8*)(temp_r3) + 0x22)) = (u8) ((*(u8*)((u8*)(temp_r3) + 0x22)) & (u8) ~(temp_r0 | temp_r0));
        temp_r12 = (*(s32 (**)(s32, u8, s32))((u8*)(temp_r3) + 0x30));
        if (temp_r12 != NULL) {
            temp_r12(arg0, arg1, arg2);
        }
    }
}