// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 gpSaveData;
extern u8 lbl_801D7148[];

u8* fn_8007FCC0(s32 p0, u8* p1);
u8* fn_8007FCD4(u8* p0);
u8* fn_8007FCC0(s32 p0, u8* p1) {
    *(s32*)p1 = *(s32*)(lbl_801D7148 + 0x14);
    return lbl_801D7148;
}

u8* fn_8007FCD4(u8* p0) {
    *(s32*)(lbl_801D7148 + 0x14) = *(s32*)p0;
    return lbl_801D7148;
}
