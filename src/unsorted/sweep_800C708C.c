// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_8003DC78();

s32 fn_800C708C(void* arg0);
s32 fn_800C708C(void* arg0) {
    u32 temp_r0;

    temp_r0 = (*(u32*)((u8*)(arg0) + 0x134));
    if (temp_r0 != 0U) {
        return fn_8003DC78(temp_r0) != 0;
    }
    if ((u32) (*(u32*)((u8*)(arg0) + 0x130)) != 0U) {
        return fn_8003DC78((*(u32*)((u8*)(arg0) + 0x130))) != 0;
    }
    return 0;
}