// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 UStream_RegisterHandler(s32, void (*)(void*), s32);
s32 fn_800075CC(s32);
void fn_800460F8(void* arg0);

void fn_800460F8(void* arg0);
void fn_800460F8(void* arg0) {
    void* temp_r31;

    temp_r31 = (*(void**)((u8*)(arg0) + 4));
    fn_800075CC((*(s32*)((u8*)(temp_r31) + 0x10)));
    fn_80009E70(temp_r31);
}