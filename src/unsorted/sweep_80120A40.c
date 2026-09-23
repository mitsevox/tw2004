// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281908;
void Mem_cpy();

void fn_80120A40(s32 p0, s32 p1);
void fn_80120A40(s32 p0, s32 p1) {
    Mem_cpy(*(s32*)(((u8*)lbl_80281908) + 0x354), p0, (p1 << 4));
    *(s32*)(((u8*)lbl_80281908) + 0x354) = (*(s32*)(((u8*)lbl_80281908) + 0x354) + (p1 << 4));
}
