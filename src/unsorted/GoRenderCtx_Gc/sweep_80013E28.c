// Small functions found by the sweep (sweep.py). Original file: GoRenderCtx_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"


void fn_80013E28(u8* p, s32 v);
void fn_80013E30(u8* p, s32 v);
void fn_80013E38(u8* p, s32 v);
s32 fn_80013E40(u8* p);
void fn_80013E28(u8* p, s32 v) {
    *(s32*)(p + 0x14) = v;
}

void fn_80013E30(u8* p, s32 v) {
    *(s32*)(p + 0x18) = v;
}

void fn_80013E38(u8* p, s32 v) {
    *(s32*)(p + 0x10) = v;
}

s32 fn_80013E40(u8* p) {
    return *(s32*)(p + 0x18);
}
