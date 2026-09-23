// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80187910[];
extern s32 lbl_80281D88;
s32 fn_80009B34();

void fn_80039FF8(void);
void fn_80039FF8(void) {
    s32 t0;
    t0 = fn_80009B34(40, 2, 0, lbl_80187910, 938);
    lbl_80281D88 = t0;
    *(s32*)(((u8*)t0) + 0x4) = 0;
    *(s32*)((u8*)lbl_80281D88) = 0;
    *(s32*)(((u8*)lbl_80281D88) + 0x8) = 0;
    *(s32*)(((u8*)lbl_80281D88) + 0xC) = 0;
    *(s32*)(((u8*)lbl_80281D88) + 0x10) = 0;
    *(s32*)(((u8*)lbl_80281D88) + 0x14) = 0;
    *(s32*)(((u8*)lbl_80281D88) + 0x18) = 0;
    *(s32*)(((u8*)lbl_80281D88) + 0x20) = 0;
}
