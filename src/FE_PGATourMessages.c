// FE_PGATourMessages.c (EA's name, from its asserts): not yet decompiled; the sweep code below is
// the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_80077B08();
s32 fn_80118664();
void fn_8010E85C(s32 p0, u8* p1);

void fn_8010E85C(s32 p0, u8* p1) {
    s32 t1;
    fn_80077B08();
    t1 = fn_80118664();
    *(s32*)p1 = t1;
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_80223C48[];
void fn_80077808();
void fn_800EFAD0();
void fn_80117188();
void fn_8010EEA8(void);
void fn_8010EF80(s32 a, u8* p);
s32 fn_801070F4();
void fn_8010F248(s32 p0, u8* p1);
void fn_80107294();
void fn_8010F2CC(u8* p0);
void fn_8011C058();
void fn_8011C060();
void fn_8010F52C(void);
void fn_8010F550(void);
s32 fn_80117DE0();
void fn_8010F574(s32 p0, u8* p1);

void fn_8010EEA8(void) {
    *(u8*)(lbl_80223C48 + 0x20) = 0;
    fn_800EFAD0(lbl_80223C48);
    fn_80117188();
    fn_80077808(0);
}

void fn_8010EF80(s32 a, u8* p) {
    *(s32*)(p + 0x0) = 25;
}

void fn_8010F248(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_801070F4();
    *(s32*)p1 = t0;
}

void fn_8010F2CC(u8* p0) {
    fn_80107294(((s16)*(s32*)p0), *(s32*)(((u8*)*(s32*)(p0 + 0x4)) + 0x8));
}

void fn_8010F52C(void) {
    fn_8011C058(1);
}

void fn_8010F550(void) {
    fn_8011C060(1);
}

void fn_8010F574(s32 p0, u8* p1) {
    s32 t0;
    t0 = fn_80117DE0(0);
    *(s32*)p1 = (t0 & 0xFF);
}

// ---- end of sweep code ----
