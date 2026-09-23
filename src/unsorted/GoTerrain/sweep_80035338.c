// Small functions found by the sweep (sweep.py). Original file: GoTerrain.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

extern s32 lbl_80281380;

void fn_80035338(s32 p0);
void fn_80035338(s32 p0) {
    *(s32*)(((u8*)lbl_80281380) + 0x230) = (lbl_80281380 + (p0 * 140));
}
