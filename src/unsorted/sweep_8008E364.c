// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8008E02C();
s32 fn_8008E724(s32, s32, s32, s32);
s32 fn_8008E944(s32, f32);
s32 strncpy(u8*, s32, s32);
extern u8* lbl_80281EE0;
extern f32 lbl_80283B40;

void fn_8008E364(s32 arg0);
void fn_8008E364(s32 arg0) {
    s32 temp_r0;
    s32 temp_r3;

    if (arg0 != (s32) (*(s32*)((u8*)(lbl_80281EE0) + 4))) {
        temp_r3 = fn_8008E02C();
        if (temp_r3 != 0) {
            temp_r0 = (*(s32*)((u8*)(lbl_80281EE0) + 0x1C0));
            if ((temp_r0 != 1) && (temp_r0 != 0)) {
                fn_8008E724(temp_r3 + 0xA0, 0, 0, 0);
                fn_8008E944(0, lbl_80283B40);
                (*(s32*)((u8*)(lbl_80281EE0) + 0x1C4)) = 2;
            } else {
                strncpy(lbl_80281EE0 + 0x20, temp_r3 + 0xA0, 0x10);
                (*(s32*)((u8*)(lbl_80281EE0) + 0x1C4)) = 2;
            }
        }
        (*(s8*)((u8*)(lbl_80281EE0) + 0x30)) = 0;
    }
    (*(s32*)((u8*)(lbl_80281EE0) + 4)) = arg0;
}