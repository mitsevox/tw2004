// Small functions found by the sweep (sweep.py). Original file: LLTex.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

void TX_spParseTextureGroupFromStream();

void fn_8000FB88(u8* p);
void fn_8000FBAC(void);
void fn_8000FB88(u8* p) {
    TX_spParseTextureGroupFromStream(*(s32*)(p + 0x0));
}

void fn_8000FBAC(void) {
}
