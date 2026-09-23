// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281EE0;

s32 fn_8008E44C(void);
s32 fn_8008E44C(void) {
    return ((u32)((-*(u8*)(((u8*)*(s32*)(((u8*)lbl_80281EE0) + 0xB4)) + 0x18)) | *(u8*)(((u8*)*(s32*)(((u8*)lbl_80281EE0) + 0xB4)) + 0x18)) >> 31);
}
