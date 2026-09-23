// Small functions found by the sweep (sweep.py). Original file: Skin.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"


void fn_800360A0(void* arg0);
void fn_800360A0(void* arg0) {
    s32 (*temp_r12)();

    temp_r12 = (*(s32 (**)())((u8*)((*(void**)((u8*)(arg0) + 0x24))) + 4));
    if (temp_r12 != NULL) {
        temp_r12(arg0);
    }
}