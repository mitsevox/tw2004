// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


s32 fn_800C6E44(void* arg0);
s32 fn_800C6E44(void* arg0) {
    void* temp_r4;

    if (((s32) (*(s32*)((u8*)(arg0) + 0x194)) == 1) && ((u32) (*(u32*)((u8*)(arg0) + 0x134)) == 0U) && ((temp_r4 = (*(void**)((u8*)(arg0) + 0x130)), ((temp_r4 == NULL) != 0)) || ((*(f32*)((u8*)(temp_r4) + 0x4C)) < (*(f32*)((u8*)(arg0) + 0x104))))) {
        return 1;
    }
    return 0;
}