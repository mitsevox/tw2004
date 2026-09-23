// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80009E70(void*);

void fn_8002FE70(void* arg0);
void fn_8002FE70(void* arg0) {
    if (arg0 != NULL) {
        fn_80009E70((*(void**)((u8*)(arg0) + 0x60)));
        fn_80009E70(arg0);
    }
}