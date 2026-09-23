// Small functions found by the sweep (sweep.py). Original file: FEgolferanim.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

extern s32 lbl_80281EE0;
void fn_80014DFC();
void fn_8008EBB4();

void fn_8008B3CC(void);
void fn_8008B3CC(void) {
    fn_80014DFC(*(s32*)(((u8*)*(s32*)(((u8*)lbl_80281EE0) + 0xB8)) + 0xC), *(s32*)(((u8*)*(s32*)(((u8*)lbl_80281EE0) + 0xB8)) + 0x10));
    fn_8008EBB4();
    *(s32*)(((u8*)*(s32*)(((u8*)lbl_80281EE0) + 0xB8)) + 0x14) = *(s32*)(((u8*)*(s32*)(((u8*)lbl_80281EE0) + 0xB8)) + 0xC);
    *(s32*)(((u8*)lbl_80281EE0) + 0x8C) = *(s32*)(((u8*)*(s32*)(((u8*)lbl_80281EE0) + 0xB8)) + 0xC);
    *(s32*)(((u8*)lbl_80281EE0) + 0x190) = (*(s32*)(((u8*)lbl_80281EE0) + 0x190) + 1);
}
