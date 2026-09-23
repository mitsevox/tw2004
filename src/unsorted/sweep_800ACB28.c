// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800B0748();
s32 fn_800ACA94();
void fn_800ACA5C(void* arg0, u8 arg1);

void fn_800ACB28(void* arg0);
void fn_800ACB28(void* arg0) {
    fn_800ACA94();
    (*(s32*)((u8*)(arg0) + 0x1C)) = 0;
    (*(s32*)((u8*)(arg0) + 0x20)) = 0;
    (*(s32*)((u8*)(arg0) + 0x24)) = 0;
    if (((u8) (*(u8*)((u8*)(arg0) + 0xA)) >> 4U) & 1) {
        fn_800ACA5C(arg0, 1);
        if ((u32) (*(u32*)((u8*)(arg0) + 0x28)) != 0U) {
            fn_800B0748((*(u32*)((u8*)(arg0) + 0x28)));
            (*(u32*)((u8*)(arg0) + 0x28)) = 0U;
            (*(s32*)((u8*)(arg0) + 0x2C)) = 0;
        }
    }
}