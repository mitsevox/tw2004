// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8009EE28(s32, s32, void*);

void fn_80082DBC(void* arg0, s32* arg1);
void fn_80082DBC(void* arg0, s32* arg1) {
    if (fn_8009EE28((*(s32*)((u8*)(arg0) + 0)), (*(s32*)((u8*)(arg0) + 4)), arg0) == -0x12) {
        *arg1 = 1;
        return;
    }
    *arg1 = 0;
}