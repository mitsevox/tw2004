// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_8007D6E0(void* arg0, s32* arg1);
void fn_8007D6E0(void* arg0, s32* arg1) {
    if ((s32) (*(s32*)((u8*)(arg0) + 4)) != 0) {
        *arg1 = 0;
        return;
    }
    *arg1 = (*(s32*)((u8*)(arg0) + 0)) + 0x1E;
}