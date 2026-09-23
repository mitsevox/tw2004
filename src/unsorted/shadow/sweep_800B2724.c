// Small functions found by the sweep (sweep.py). Original file: shadow.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

extern s32 lbl_802814A8;

void fn_800B2724(void);
void fn_800B2724(void) {
    *(s32*)(((u8*)lbl_802814A8) + 0x4F0) = 0;
}
