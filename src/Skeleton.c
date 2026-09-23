// Skeleton.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled; the
// sweep code below is the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_802810A6;
void fn_80027738(u8 v);
void fn_8002703C();
void fn_80027740(u8* p0);
extern u8 lbl_801C6498[];
void fn_80009710();
void fn_80113E60();
void fn_80029530(void);
void fn_80114080();
void fn_8002955C(void);
void fn_8011443C();
void fn_80029948(void);
void fn_80029A74(u8* p, s32 v);
void fn_80029A7C(u8* p0, s32 p1, s32 p2);
void fn_80029A88(u8* p, s32 v);

void fn_80027738(u8 v) {
    lbl_802810A6 = v;
}

void fn_80027740(u8* p0) {
    fn_8002703C(p0, *(s32*)(p0 + 0xC));
}

void fn_80029530(void) {
    fn_80009710(lbl_801C6498);
    fn_80113E60();
}

void fn_8002955C(void) {
    fn_80114080();
}

void fn_80029948(void) {
    fn_8011443C();
}

void fn_80029A74(u8* p, s32 v) {
    *(s32*)(p + 0x760) = v;
}

void fn_80029A7C(u8* p0, s32 p1, s32 p2) {
    *(s32*)(p0 + 0x768) = p1;
    *(s32*)(p0 + 0x76C) = p2;
}

void fn_80029A88(u8* p, s32 v) {
    *(s32*)(p + 0x764) = v;
}

// ---- end of sweep code ----
