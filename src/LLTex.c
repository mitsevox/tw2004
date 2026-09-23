// LLTex.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): texture banks, made
// from the 'txf ' objects of the stream files. Not yet decompiled; the code below is the matched
// small functions.

#include "engine.h"

// Builds a bank from a 'txf ' object's data, into pBank or, when it is NULL, a new allocation.
TexBank* TX_spParseTextureGroupFromStream(u8* pData, TexBank* pBank, int n);

void fn_8000F0E8(void) {
}

TexBank* fn_8000FB88(UStreamObject* pObject, TexBank* pBank, int n) {
    return TX_spParseTextureGroupFromStream(pObject->pData, pBank, n);
}

void fn_8000FBAC(void) {
}
