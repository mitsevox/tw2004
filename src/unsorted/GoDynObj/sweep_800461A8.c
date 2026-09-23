// Small functions found by the sweep (sweep.py). Original file: GoDynObj.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

extern u8 lbl_80187D2C[];
extern s32 lbl_80281DA0;
s32 fn_80009B34();

void fn_800461A8(void);
void fn_800461A8(void) {
    s32 t0;
    t0 = fn_80009B34(2736, 2, 16, lbl_80187D2C, 283);
    lbl_80281DA0 = t0;
    *(s32*)(((u8*)t0) + 0xA54) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA58) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA5C) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA60) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA64) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA68) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA6C) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA70) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA74) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA78) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA7C) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA80) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA84) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA88) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA8C) = 0;
    *(s32*)(((u8*)lbl_80281DA0) + 0xA90) = 0;
}
