// Small functions found by the sweep (sweep.py). Original file: hwsMaterial_Gc.c (EA's name, from its asserts; placed by the file map).

#include "game_types.h"

extern u8 lbl_80223C48[];
s32 fn_80117510();

void fn_80112254(s32 p0, u8* p1);
void fn_80112254(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_80117510(*(u16*)lbl_80223C48);
    *(s32*)p1 = t0;
}
