// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* lbl_80282220;

s32 fn_800C6D9C(void);
s32 fn_800C6D9C(void) {
    if ((void* ) lbl_80282220 == NULL) {
        return 0;
    }
    if (((u8) (*(u8*)((u8*)(lbl_80282220) + 0x54)) != 0) || ((u8) (*(u8*)((u8*)(lbl_80282220) + 0x58)) != 0) || ((u8) (*(u8*)((u8*)(lbl_80282220) + 0x55)) != 0)) {
        return 1;
    }
    return 0;
}