// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"
#include "gx.h"

s32 fn_80070168(s32, s32);

void fn_80070BD0(void* arg0);
void fn_80070BD0(void* arg0) {
    if ((u8) (*(u8*)((u8*)(arg0) + 0xC)) != 0) {
        GXSetArray(9, *(*(void***)((u8*)(arg0) + 0x24)), 0xC);
    }
    fn_80070168((*(s32*)((u8*)(arg0) + 8)), (*(s32*)((u8*)(arg0) + 4)));
}