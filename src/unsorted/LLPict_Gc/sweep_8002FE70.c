// Small functions found by the sweep (sweep.py). Original file: LLPict_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

s32 fn_80009E70(void*);

void fn_8002FE70(void* arg0);
void fn_8002FE70(void* arg0) {
    if (arg0 != NULL) {
        fn_80009E70((*(void**)((u8*)(arg0) + 0x60)));
        fn_80009E70(arg0);
    }
}