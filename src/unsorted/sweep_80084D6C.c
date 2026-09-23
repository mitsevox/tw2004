// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 strcpy(s32, s32);
u32 strlen(s32);

void fn_80084D6C(void* arg0);
void fn_80084D6C(void* arg0) {
    strcpy((*(s32*)((u8*)((*(void**)((u8*)(arg0) + 4))) + 8)), (*(s32*)((u8*)((*(void**)((u8*)(arg0) + 0))) + 8)));
    if (strlen((*(s32*)((u8*)((*(void**)((u8*)(arg0) + 0))) + 8))) > 0xCU) {
        (*(s8*)((u8*)((*(void**)((u8*)((*(void**)((u8*)(arg0) + 4))) + 8))) + 0xB)) = 0;
        (*(s8*)((u8*)((*(void**)((u8*)((*(void**)((u8*)(arg0) + 4))) + 8))) + 0xA)) = 0x2E;
        (*(s8*)((u8*)((*(void**)((u8*)((*(void**)((u8*)(arg0) + 4))) + 8))) + 9)) = 0x2E;
        (*(s8*)((u8*)((*(void**)((u8*)((*(void**)((u8*)(arg0) + 4))) + 8))) + 8)) = 0x2E;
    }
}