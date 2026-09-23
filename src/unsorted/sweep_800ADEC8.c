// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800ADEC8(void* arg0, void* arg1);
void fn_800ADEC8(void* arg0, void* arg1) {
    if ((u16) (*(u16*)((u8*)(arg0) + 0xA)) != 0) {
        (*(void**)((u8*)((*(void**)((u8*)(arg0) + 4))) + 4)) = arg1;
    } else {
        (*(void**)((u8*)(arg0) + 0)) = arg1;
    }
    (*(void**)((u8*)(arg1) + 0)) = (void* ) (*(void**)((u8*)(arg0) + 4));
    (*(s32*)((u8*)(arg1) + 4)) = 0;
    (*(void**)((u8*)(arg0) + 4)) = arg1;
    (*(u16*)((u8*)(arg0) + 0xA)) = (u16) ((*(u16*)((u8*)(arg0) + 0xA)) + 1);
}