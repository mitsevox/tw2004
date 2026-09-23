// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800AAB48(void* arg0, void* arg1);
void fn_800AAB48(void* arg0, void* arg1) {
    u8 temp_r3;
    void* temp_r4;

    temp_r4 = (*(void**)((u8*)(arg1) + 0x2C));
    temp_r3 = 1 << (s8) (*(u8*)((u8*)(arg0) + 3));
    if ((u8) (*(s32*)((u8*)(arg0) + 4)) != 0) {
        (*(u8*)((u8*)(temp_r4) + 0)) = (u8) ((*(u8*)((u8*)(temp_r4) + 0)) | temp_r3);
        return;
    }
    (*(u8*)((u8*)(temp_r4) + 1)) = (u8) ((*(u8*)((u8*)(temp_r4) + 1)) | temp_r3);
}