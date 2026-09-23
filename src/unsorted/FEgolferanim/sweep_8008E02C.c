// Small functions found by the sweep (sweep.py). Original file: FEgolferanim.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

s32 Char_SetClip(s32, s32, s32, s32);
extern void* lbl_80281EE0;

void fn_8008E02C(void);
void fn_8008E02C(void) {
    s32 temp_r0;

    temp_r0 = (*(s32*)((u8*)(lbl_80281EE0) + 4));
    if (temp_r0 == 1) {
        Char_SetClip((*(s32*)((u8*)((*(void**)((u8*)(lbl_80281EE0) + 0xB4))) + 8)), 0xB, 0, 0);
        return;
    }
    if (temp_r0 == 2) {
        Char_SetClip((*(s32*)((u8*)((*(void**)((u8*)(lbl_80281EE0) + 0xB4))) + 8)), 7, 0, 0);
        return;
    }
    Char_SetClip((*(s32*)((u8*)((*(void**)((u8*)(lbl_80281EE0) + 0xB4))) + 8)), 1, 0, 0);
}