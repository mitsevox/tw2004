// Small functions found by the sweep (sweep.py). Original file: FEgolferanim.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

extern u8* lbl_80281EE0;

void fn_8008E718(s32 v);
void fn_8008E718(s32 v) {
    *(s32*)(lbl_80281EE0 + 0x1BC) = v;
}
