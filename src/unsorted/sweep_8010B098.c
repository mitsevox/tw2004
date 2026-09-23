// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_8010B098(void* arg0);
void fn_8010B098(void* arg0) {
    if (arg0 != NULL) {
        (*(s32*)((u8*)(arg0) + 8)) = 0;
        (*(s16*)((u8*)((*(void**)((u8*)(arg0) + 4))) + 2)) = 0;
        (*(s16*)((u8*)((*(void**)((u8*)(arg0) + 4))) + 4)) = 0;
        (*(s32*)((u8*)(arg0) + 0x14)) = 0;
    }
}