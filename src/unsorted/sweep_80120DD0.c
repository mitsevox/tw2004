// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281908;
void fn_8000B0D4();

void fn_80120DD0(s32 p0, s32 p1);
void fn_80120DF8(u8* p0);
void fn_80120DD0(s32 p0, s32 p1) {
    *(s32*)(((u8*)lbl_80281908) + 0x354) = p0;
    *(s32*)(((u8*)lbl_80281908) + 0x350) = p0;
    *(s32*)(((u8*)lbl_80281908) + 0x358) = (*(s32*)(((u8*)lbl_80281908) + 0x350) + (p1 << 4));
}

void fn_80120DF8(u8* p0) {
    fn_8000B0D4(*(s32*)(((u8*)lbl_80281908) + 0x368), *(s32*)(p0 + 0x4), lbl_80281908);
}
