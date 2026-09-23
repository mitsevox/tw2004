// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8011389C();

void fn_800CE128(void* arg0);
void fn_800CE128(void* arg0) {
    if (((u32) (*(u32*)((u8*)((*(void**)((u8*)(arg0) + 0))) + 0x48)) != 0U) && ((*(s32*)((u8*)(arg0) + 0x10D4)) & 2)) {
        fn_8011389C();
    }
}