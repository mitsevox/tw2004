// Small functions found by the sweep (sweep.py). Original file: LLVideo.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

extern s32 lbl_80281200;

void fn_800757B8(void);
void fn_800757B8(void) {
    *(s32*)((u8*)lbl_80281200) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0x4) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0x8) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0xC) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0x10) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0x14) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0x18) = 0;
    *(s32*)(((u8*)lbl_80281200) + 0x1C) = 0;
}
