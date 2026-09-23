// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800B1CF4();

void fn_800B1AB0(u8* arg0, u32 arg1, f32* arg2);
void fn_800B1AB0(u8* arg0, u32 arg1, f32* arg2) {
    u8* temp_r6;
    u8** temp_r31;

    temp_r31 = (*(u8***)((u8*)((*(u8**)((u8*)(arg0) + 0x100))) + 0x14));
    if (arg1 != 0U) {
        temp_r6 = *temp_r31;
        fn_800B1CF4(arg0 + 0xC0, temp_r6 + 0x58, arg1, temp_r6);
    }
    if (arg2 != NULL) {
        *arg2 = (*(f32*)((u8*)(*temp_r31) + 0x64));
    }
}