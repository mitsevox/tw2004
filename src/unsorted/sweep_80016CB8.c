// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801B8BA8[];
extern s32 lbl_80281CA0;

u8* fn_80016CB8(void);
u8* fn_80016CB8(void) {
    *(u8*)(lbl_801B8BA8 + 0x274) = 0;
    *(u8*)(lbl_801B8BA8 + 0x4FC) = 0;
    *(u8*)(lbl_801B8BA8 + 0x784) = 0;
    *(u8*)(lbl_801B8BA8 + 0xA0C) = 0;
    return lbl_801B8BA8;
}
