// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern f32 lbl_802842EC;

s32 fn_800C7100(void* arg0);
s32 fn_800C7100(void* arg0) {
    if (((u32) (*(u32*)((u8*)(arg0) + 0x130)) == 0U) || ((u32) (*(u32*)((u8*)(arg0) + 0x134)) == 0U) || ((*(f32*)((u8*)(arg0) + 0x104)) > lbl_802842EC)) {
        return 1;
    }
    return 0;
}