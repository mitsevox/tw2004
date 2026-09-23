// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80049514();

void fn_80049A14(void* arg0, void* arg1);
void fn_80049A14(void* arg0, void* arg1) {
    void* temp_r31;

    temp_r31 = (*(void**)((u8*)(arg1) + 8));
    fn_80049514();
    (*(f32*)((u8*)(arg0) + 0x16C)) = (f32) (*(f32*)((u8*)(temp_r31) + 0x1C));
}