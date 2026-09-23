// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* lbl_80282488;

void fn_8010BFA0(s32 arg0);
void fn_8010BFA0(s32 arg0) {
    (*(s32*)((u8*)(lbl_80282488) + 0x97C)) = arg0;
    (*(s32*)((u8*)(lbl_80282488) + 0x978)) = (s32) ((*(s32*)((u8*)(lbl_80282488) + 0x978)) + arg0);
    (*(s8*)((u8*)(lbl_80282488) + 0x974)) = 1;
    if ((s32) (*(s32*)((u8*)(lbl_80282488) + 0x978)) >= (s32) (*(s32*)((u8*)(lbl_80282488) + 0x984))) {
        (*(s8*)((u8*)(lbl_80282488) + 0x975)) = 1;
    }
}