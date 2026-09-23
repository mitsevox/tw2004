// LLTex.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): not yet decompiled;
// the sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8000F0E8(void);
void TX_spParseTextureGroupFromStream();
void fn_8000FB88(u8* p);
void fn_8000FBAC(void);

void fn_8000F0E8(void) {
}

void fn_8000FB88(u8* p) {
    TX_spParseTextureGroupFromStream(*(s32*)(p + 0x0));
}

void fn_8000FBAC(void) {
}

// ---- end of sweep code ----
