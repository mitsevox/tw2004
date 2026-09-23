// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8001EEE4(s32);
s32 fn_8001ECA8(void* arg0, s32 arg1);

void fn_8001EC6C(void* arg0);
void fn_8001EC6C(void* arg0) {
    fn_8001ECA8(arg0, fn_8001EEE4((*(s32*)((u8*)(arg0) + 0x38))));
}