// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800B6214(void*);

void fn_800B61B4(void* arg0);
void fn_800B61B4(void* arg0) {
    s32 (*temp_r12)(s32);

    temp_r12 = (*(s32 (**)(s32))((u8*)(arg0) + 0x20));
    if (temp_r12 != NULL) {
        temp_r12((*(s32*)((u8*)(arg0) + 0x24)));
    }
    if ((*(u8*)((u8*)(arg0) + 0x38)) & 1) {
        fn_800B6214(arg0);
        return;
    }
    (*(s32*)((u8*)(arg0) + 0x28)) = 2;
}