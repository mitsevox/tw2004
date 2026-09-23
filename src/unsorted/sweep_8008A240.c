// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 fn_800E8128(s32, s32, void*);

void fn_8008A240(void* arg0, s32* arg1);
void fn_8008A240(void* arg0, s32* arg1) {
    if (fn_800E8128((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 4)), arg0) != 0) {
        *arg1 = 1;
        return;
    }
    *arg1 = 0;
}