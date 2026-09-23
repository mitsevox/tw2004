// Small functions found by the sweep (sweep.py). Original file: FEgolferanim.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

u8 fn_80103B80();
extern void* lbl_80281EE0;

void fn_8008E8D0(s32 arg0);
void fn_8008E8D0(s32 arg0) {
    if (fn_80103B80() != 0) {
        (*(s32*)((u8*)(lbl_80281EE0) + 0xC)) = arg0;
        (*(s8*)((u8*)(lbl_80281EE0) + 0x80)) = 1;
    }
}