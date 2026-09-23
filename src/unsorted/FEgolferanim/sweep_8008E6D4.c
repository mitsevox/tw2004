// Small functions found by the sweep (sweep.py). Original file: FEgolferanim.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

extern s32 lbl_80281EE0;
void fn_8001C5B4();

void fn_8008E6D4(s32 p0);
void fn_8008E6D4(s32 p0) {
    *(s32*)(((u8*)lbl_80281EE0) + 0x1B8) = p0;
    *(s32*)(((u8*)lbl_80281EE0) + 0x1BC) = -1;
    fn_8001C5B4(*(s32*)(((u8*)*(s32*)(((u8*)lbl_80281EE0) + 0xB4)) + 0x8), *(s32*)(((u8*)lbl_80281EE0) + 0x1B8));
}
