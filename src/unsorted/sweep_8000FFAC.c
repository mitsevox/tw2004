// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80009E70();

void fn_8000FFAC(void* arg0);
void fn_8000FFAC(void* arg0) {
    u32 temp_r3;

    if (((u32) (*(u32*)((u8*)(arg0) + 0x18)) != 0U) && ((u8) (*(u8*)((u8*)(arg0) + 0x2D)) != 1)) {
        fn_80009E70((*(u32*)((u8*)(arg0) + 0x18)));
        (*(u32*)((u8*)(arg0) + 0x18)) = 0U;
    }
    temp_r3 = (*(u32*)((u8*)(arg0) + 0x20));
    if ((temp_r3 != 0U) && (temp_r3 != (u32) (*(u32*)((u8*)(arg0) + 0x18))) && ((u8) (*(u8*)((u8*)(arg0) + 0x2D)) != 1)) {
        fn_80009E70();
        (*(u32*)((u8*)(arg0) + 0x20)) = 0U;
    }
}