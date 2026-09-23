// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8001EED8(s32, s32);
s32 fn_8001EEE4(s32, s32);

void fn_80018484(void* arg0, s32 arg1);
void fn_80018484(void* arg0, s32 arg1) {
    if (arg0 != NULL) {
        (*(s32*)((u8*)(arg0) + 0x38)) = arg1;
        (*(s32*)((u8*)(arg0) + 0x16A0)) = fn_8001EED8((*(s32*)((u8*)(arg0) + 0x38)), 0x53);
        (*(s32*)((u8*)(arg0) + 0x16A4)) = fn_8001EED8((*(s32*)((u8*)(arg0) + 0x38)), 0x52);
        (*(s32*)((u8*)(arg0) + 0x16A8)) = fn_8001EEE4((*(s32*)((u8*)(arg0) + 0x38)), 0x15);
    }
}