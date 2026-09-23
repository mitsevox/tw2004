// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void* fn_80012EF0();
s32* fn_80016E28();

void fn_800170F4(s32 arg0);
void fn_800170F4(s32 arg0) {
    s32* temp_r31;
    void* temp_r3;

    temp_r31 = fn_80016E28();
    fn_80017004(arg0);
    temp_r3 = fn_80012EF0();
    (*(f32*)((u8*)(temp_r31) + 0x284)) = (f32) (*(f32*)((u8*)(temp_r3) + 0));
    (*(f32*)((u8*)(temp_r31) + 0x280)) = (f32) (*(f32*)((u8*)(temp_r3) + 4));
    (*(f32*)((u8*)(temp_r31) + 0x27C)) = (f32) (*(f32*)((u8*)(temp_r3) + 8));
    (*(f32*)((u8*)(temp_r31) + 0x278)) = (f32) (*(f32*)((u8*)(temp_r3) + 0xC));
}