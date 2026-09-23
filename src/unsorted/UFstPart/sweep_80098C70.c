// Small functions found by the sweep (sweep.py). Original file: UFstPart.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

extern u8 lbl_801DB888[];

void fn_80098C70(void);
void fn_80098C70(void) {
    *(s32*)(((u8*)*(s32*)lbl_801DB888) + 0xB8) = (*(s32*)(((u8*)*(s32*)lbl_801DB888) + 0xB8) | 0x80000000);
    *(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0x4)) + 0xB8) = (*(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0x4)) + 0xB8) | 0x80000000);
    *(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0x8)) + 0xB8) = (*(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0x8)) + 0xB8) | 0x80000000);
    *(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0xC)) + 0xB8) = (*(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0xC)) + 0xB8) | 0x80000000);
    *(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0x10)) + 0xB8) = (*(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0x10)) + 0xB8) | 0x80000000);
    *(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0x14)) + 0xB8) = (*(s32*)(((u8*)*(s32*)(lbl_801DB888 + 0x14)) + 0xB8) | 0x80000000);
}
