// Small functions found by the sweep (sweep.py). Original file: Skin.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

s32 Vec_Copy(s32);

void fn_8003614C(void* arg0);
void fn_8003614C(void* arg0) {
    if (arg0 != NULL) {
        Vec_Copy((*(s32*)((u8*)((*(void**)((u8*)(arg0) + 0x38))) + 4)) + 0x1C);
    }
}