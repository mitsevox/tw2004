// Small functions found by the sweep (sweep.py). Original file: FEgolferanim.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

s32 fn_8008DD50(s32);
s32 fn_8008E244();
u8 fn_80103B80();
extern void* lbl_80281EE0;

void fn_8008E860(s32 arg0);
void fn_8008E860(s32 arg0) {
    s32 temp_r4;

    if (fn_80103B80() != 0) {
        temp_r4 = (*(s32*)((u8*)(lbl_80281EE0) + 8));
        (*(s32*)((u8*)(lbl_80281EE0) + 8)) = arg0;
        (*(s32*)((u8*)(lbl_80281EE0) + 0xC)) = 0;
        (*(s8*)((u8*)(lbl_80281EE0) + 0x80)) = 0;
        if (temp_r4 != (s32) (*(s32*)((u8*)(lbl_80281EE0) + 8))) {
            fn_8008E244();
            fn_8008DD50(0);
        }
    }
}