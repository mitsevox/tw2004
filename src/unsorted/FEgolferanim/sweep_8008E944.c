// Small functions found by the sweep (sweep.py). Original file: FEgolferanim.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

extern void* lbl_80281EE0;

u8 fn_8008E944(u8 arg0, f32 farg0);
u8 fn_8008E944(u8 arg0, f32 farg0) {
    u8 temp_r0;

    temp_r0 = (*(u8*)((u8*)(lbl_80281EE0) + 0x78));
    (*(u8*)((u8*)(lbl_80281EE0) + 0x78)) = arg0;
    (*(f32*)((u8*)(lbl_80281EE0) + 0x7C)) = farg0;
    return temp_r0;
}