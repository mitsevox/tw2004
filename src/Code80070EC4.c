// Code80070EC4.c (our name): made by fold.py from 6 sweep files; not yet described.

#include "game_types.h"
#include "gx.h"

// ---- sweep code (not yet cleaned up) ----

void fn_80074DA8();
void fn_80070F00(u8* p0, s32 p1);
void fn_8007524C();
void fn_80076E24();
void fn_80070F34(u8* p0);
s32 fn_80070168(s32, s32);
void fn_80070F5C(void* arg0);
extern f32 lbl_80281E78;
void fn_80070FB0(u8* p0);
void fn_80070FBC(u8* p0, s32 p1);
void fn_80097624();
void fn_80070FF0(u8* p0);

void fn_80070F00(u8* p0, s32 p1) {
    fn_80074DA8(*(s32*)(p0 + 0x24), *(s32*)(p0 + 0x0), (p0 + 0x4), p1);
}

void fn_80070F34(u8* p0) {
    fn_8007524C((p0 + 0x4));
    fn_80076E24();
}

void fn_80070F5C(void* arg0) {
    if ((u8) (*(u8*)((u8*)(arg0) + 0xC)) != 0) {
        GXSetArray(9, *(*(void***)((u8*)(arg0) + 0x24)), 0xC);
    }
    fn_80070168((*(s32*)((u8*)(arg0) + 8)), (*(s32*)((u8*)(arg0) + 4)));
}

void fn_80070FB0(u8* p0) {
    lbl_80281E78 = *(f32*)p0;
}

void fn_80070FBC(u8* p0, s32 p1) {
    fn_80074DA8(*(s32*)(p0 + 0x24), *(s32*)(p0 + 0x0), (p0 + 0x4), p1);
}

void fn_80070FF0(u8* p0) {
    fn_80097624((p0 + 0x4));
    fn_8007524C((p0 + 0x4));
}

// ---- end of sweep code ----
