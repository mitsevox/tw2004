// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern f32 lbl_802842C4;

f32 fn_800C7394(void* arg0);
f32 fn_800C7394(void* arg0) {
    if ((u32) (*(u32*)((u8*)(arg0) + 0x134)) == 0U) {
        return lbl_802842C4;
    }
    return (*(f32*)((u8*)(arg0) + 0x110)) - (*(f32*)((u8*)(arg0) + 0x104));
}