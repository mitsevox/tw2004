// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801D880C[];

u8* fn_8007C988(u8* p0);
u8* fn_8007C988(u8* p0) {
    *(s32*)(lbl_801D880C + 0x4) = *(s32*)p0;
    *(s32*)lbl_801D880C = 0;
    return lbl_801D880C;
}
