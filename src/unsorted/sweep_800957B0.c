// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800957B0(void* arg0, s32 arg1);
void fn_800957B0(void* arg0, s32 arg1) {
    if ((arg0 != NULL) && (arg1 != (s32) (*(s32*)((u8*)(arg0) + 0x2C)))) {
        (*(s32*)((u8*)(arg0) + 0x2C)) = arg1;
        (*(s32*)((u8*)(arg0) + 0x28)) = (s32) ((*(s32*)((u8*)(arg0) + 0x28)) | 1);
    }
}