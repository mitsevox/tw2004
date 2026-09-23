// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80009E70();
s32 fn_800B920C();
s32 fn_800B9808();

void fn_8002FF38(void* arg0, void* arg1);
void fn_8002FF38(void* arg0, void* arg1) {
    if ((u32) (*(u32*)((u8*)(arg0) + 0x60)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(arg0) + 0x60)));
    }
    if ((u32) (*(u32*)((u8*)(arg1) + 4)) != 0U) {
        fn_800B9808((*(s32*)((u8*)(arg1) + 0)), (*(u32*)((u8*)(arg1) + 4)));
    }
    fn_800B920C((*(s32*)((u8*)(arg1) + 0)));
    fn_80009E70((*(s32*)((u8*)(arg1) + 0)));
}