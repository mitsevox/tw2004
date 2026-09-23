// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80009E70();

void fn_800977CC(void* arg0);
void fn_800977CC(void* arg0) {
    if ((u32) (*(u32*)((u8*)(arg0) + 0xC0)) != 0U) {
        fn_80009E70((*(u32*)((u8*)(arg0) + 0xC0)));
    }
}