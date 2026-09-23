// Small functions found by the sweep (sweep.py). Original file: FEgolferanim.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

extern s32 lbl_80281EE0;
void fn_8008DD50();

void fn_8008E824(void);
void fn_8008E824(void) {
    fn_8008DD50(0);
    *(s32*)(((u8*)lbl_80281EE0) + 0x50) = 0;
    *(s32*)(((u8*)lbl_80281EE0) + 0x1C0) = 4;
}
