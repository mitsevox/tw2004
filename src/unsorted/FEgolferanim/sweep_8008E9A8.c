// Small functions found by the sweep (sweep.py). Original file: FEgolferanim.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

extern u8* lbl_80281EE0;

s32 fn_8008E9A8(void);
s32 fn_8008E9A8(void) {
    return *(s32*)(lbl_80281EE0 + 0x8);
}
