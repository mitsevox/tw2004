// GoShaderObject_PrelitUVAnimation_Gc.c (EA's name, from its asserts): not yet decompiled; the
// sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_80074DA8();
void fn_80071148(u8* p0, s32 p1);
void fn_8007524C();
void fn_80076E24();
void fn_8007117C(u8* p0);
s32 GXSetArray(s32, s32, s32);
s32 fn_80070168(s32, s32);
void fn_800711A4(void* arg0);
void fn_80009E70();
void fn_80070348();
void fn_800712B4(u8* p0);

void fn_80071148(u8* p0, s32 p1) {
    fn_80074DA8(*(s32*)(p0 + 0x24), *(s32*)(p0 + 0x0), (p0 + 0x4), p1);
}

void fn_8007117C(u8* p0) {
    fn_8007524C((p0 + 0x4));
    fn_80076E24();
}

void fn_800711A4(void* arg0) {
    if ((u8) (*(u8*)((u8*)(arg0) + 0xC)) != 0) {
        GXSetArray(9, *(*(s32**)((u8*)(arg0) + 0x24)), 0xC);
    }
    fn_80070168((*(s32*)((u8*)(arg0) + 8)), (*(s32*)((u8*)(arg0) + 4)));
}

void fn_800712B4(u8* p0) {
    fn_80009E70(*(s32*)(p0 + 0x8));
    fn_80070348(*(s32*)(p0 + 0x4));
}

// ---- end of sweep code ----
