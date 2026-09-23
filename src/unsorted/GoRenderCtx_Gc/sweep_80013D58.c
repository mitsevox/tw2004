// Small functions found by the sweep (sweep.py). Original file: GoRenderCtx_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

extern u8* lbl_80280DF0;

void fn_80013D58(void);
void fn_80013D5C(s32 v);
void fn_80013D58(void) {
}

void fn_80013D5C(s32 v) {
    *(s32*)(lbl_80280DF0 + 0x0) = v;
}
