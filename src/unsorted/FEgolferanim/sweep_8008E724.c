// Small functions found by the sweep (sweep.py). Original file: FEgolferanim.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

s32 strncpy();
extern u8* lbl_80281EE0;
extern f32 lbl_80283B3C;

void fn_8008E724(u32 arg0, u32 arg1, s8 arg2, u8 arg3);
void fn_8008E724(u32 arg0, u32 arg1, s8 arg2, u8 arg3) {
    if (arg0 == 0U) {
        (*(s8*)((u8*)(lbl_80281EE0) + 0x20)) = 0;
        (*(s32*)((u8*)(lbl_80281EE0) + 0x1C0)) = 4;
    } else {
        strncpy(lbl_80281EE0 + 0x20, arg0, 0x10, lbl_80281EE0);
        (*(f32*)((u8*)(lbl_80281EE0) + 0x1CC)) = (f32) lbl_80283B3C;
        if (arg3 != 0) {
            (*(s32*)((u8*)(lbl_80281EE0) + 0x1C0)) = 0;
        } else {
            (*(s32*)((u8*)(lbl_80281EE0) + 0x1C0)) = 1;
        }
        (*(s8*)((u8*)(lbl_80281EE0) + 0x1C8)) = 1;
        (*(s32*)((u8*)(lbl_80281EE0) + 0x1C4)) = 3;
    }
    if (arg1 == 0U) {
        (*(s8*)((u8*)(lbl_80281EE0) + 0x30)) = 0;
    } else {
        strncpy(lbl_80281EE0 + 0x30, arg1, 0x20);
    }
    (*(s8*)((u8*)(lbl_80281EE0) + 0x1D0)) = arg2;
}