// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800B9864();

void fn_800B9808(s32 arg0, void* arg1);
void fn_800B9808(s32 arg0, void* arg1) {
    (*(u32*)((u8*)(arg1) + 0xC)) = (u32) ((*(u32*)((u8*)(arg1) + 0xC)) - 1);
    if ((u32) (*(u32*)((u8*)(arg1) + 0xC)) == 0U) {
        fn_800B9864(arg0, arg1);
        fn_800B9624(arg0 + 0x38, arg1);
    }
}