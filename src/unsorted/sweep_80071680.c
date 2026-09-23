// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 GXSetArray(s32, s32, s32);
s32 fn_80070168(s32, s32);

void fn_80071680(void* arg0);
void fn_80071680(void* arg0) {
    if ((u8) (*(u8*)((u8*)(arg0) + 0xC)) != 0) {
        GXSetArray(9, *(*(s32**)((u8*)(arg0) + 0x24)), 0xC);
    }
    fn_80070168((*(s32*)((u8*)(arg0) + 8)), (*(s32*)((u8*)(arg0) + 4)));
}