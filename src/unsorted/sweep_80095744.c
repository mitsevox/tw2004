// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_80095744(void* arg0, s32 arg1);
void fn_80095744(void* arg0, s32 arg1) {
    if ((arg0 != NULL) && (arg1 != 0xD) && (arg1 != (s32) (*(s32*)((u8*)(arg0) + 0x1C)))) {
        (*(s32*)((u8*)(arg0) + 0x1C)) = arg1;
        (*(s32*)((u8*)(arg0) + 0x18)) = (s32) ((*(s32*)((u8*)(arg0) + 0x18)) | 1);
        (*(s32*)((u8*)(arg0) + 0x168)) = (s32) ((*(s32*)((u8*)(arg0) + 0x168)) & 0xFFFFFFFC);
    }
}