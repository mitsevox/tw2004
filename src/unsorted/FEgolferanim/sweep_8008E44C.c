// Small functions found by the sweep (sweep.py). Original file: FEgolferanim.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

extern s32 lbl_80281EE0;

s32 fn_8008E44C(void);
s32 fn_8008E44C(void) {
    return ((u32)((-*(u8*)(((u8*)*(s32*)(((u8*)lbl_80281EE0) + 0xB4)) + 0x18)) | *(u8*)(((u8*)*(s32*)(((u8*)lbl_80281EE0) + 0xB4)) + 0x18)) >> 31);
}
