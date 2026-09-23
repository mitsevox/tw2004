// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


u32 fn_800AE03C(void* arg0);
u32 fn_800AE03C(void* arg0) {
    u32 temp_r4;
    u32 temp_r5;
    u32 temp_r6;

    temp_r6 = (*(u32*)((u8*)(arg0) + 0xC));
    (*(u16*)((u8*)(arg0) + 0x12)) = (u16) ((*(u16*)((u8*)(arg0) + 0x12)) + 1);
    (*(u32*)((u8*)(arg0) + 0xC)) = (u32) ((*(u32*)((u8*)(arg0) + 0xC)) + (*(s32*)((u8*)(arg0) + 0x14)));
    temp_r4 = (*(u32*)((u8*)(arg0) + 0xC));
    temp_r5 = (*(u32*)((u8*)(arg0) + 4));
    if (temp_r4 >= temp_r5) {
        (*(u32*)((u8*)(arg0) + 0xC)) = (u32) (temp_r4 - (temp_r5 - (*(s32*)((u8*)(arg0) + 0))));
    }
    return temp_r6;
}