// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281F70;

void fn_800975FC(u8* p0);
void fn_800975FC(u8* p0) {
    *(s32*)((u8*)lbl_80281F70) = (*(s32*)((u8*)lbl_80281F70) - 1);
    *(s32*)(((u8*)lbl_80281F70) + 0x4) = (*(s32*)(((u8*)lbl_80281F70) + 0x4) - *(u16*)(p0 + 0xA));
}
