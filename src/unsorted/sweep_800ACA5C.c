// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800AFCBC(u16, u8);

void fn_800ACA5C(void* arg0, u8 arg1);
void fn_800ACA5C(void* arg0, u8 arg1) {
    if ((arg0 != NULL) && !(((u8) (*(u8*)((u8*)(arg0) + 0xB)) >> 6U) & 1)) {
        fn_800AFCBC((*(u16*)((u8*)(arg0) + 8)), arg1);
    }
}