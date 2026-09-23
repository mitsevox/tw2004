// SkinMorph.c (EA's name, from its asserts): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8011C46C(void* arg0);
void fn_8011C1FC();
void fn_8011C4D4(s32 p0, s32 p1);
void fn_8011C580(s32 p0, s32 p1);
s32 fn_8011C58C(s32 p0, s32 p1, s32 p2);
void fn_8011C59C(s32 p0, s32 p1);
void fn_8011C5A8(s32 p0, s32 p1);
void fn_8011C84C(void);

void fn_8011C46C(void* arg0) {
    s32 temp_r4;

    temp_r4 = (*(s32*)((u8*)(arg0) + 0x10020));
    (*(s32*)((u8*)(arg0) + 0x10020)) = (s32) (*(s32*)((u8*)(arg0) + 0x10024));
    (*(s32*)((u8*)(arg0) + 0x10024)) = temp_r4;
}

void fn_8011C4D4(s32 p0, s32 p1) {
    fn_8011C1FC(p0, p1, (p1 + (*(u16*)(((u8*)(p0 + 0x10000)) + 0x18) << 3)), *(u16*)(((u8*)(p0 + 0x10000)) + 0x18));
}

void fn_8011C580(s32 p0, s32 p1) {
    *(s32*)(((u8*)(p0 + 0x10000)) + 0x228) = p1;
}

s32 fn_8011C58C(s32 p0, s32 p1, s32 p2) {
    *(s32*)(((u8*)(p0 + 0x10000)) + 0x230) = p1;
    *(s32*)(((u8*)(p0 + 0x10000)) + 0x22C) = p2;
    return (p0 + 0x10000);
}

void fn_8011C59C(s32 p0, s32 p1) {
    *(s32*)(((u8*)(p0 + 0x10000)) + 0x238) = p1;
}

void fn_8011C5A8(s32 p0, s32 p1) {
    *(s32*)(((u8*)(p0 + 0x10000)) + 0x234) = p1;
}

void fn_8011C84C(void) {
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

s32 fn_80009E70(void*);
void fn_8011CD84(void* arg0);

void fn_8011CD84(void* arg0) {
    void* temp_r3;

    temp_r3 = (*(void**)((u8*)(arg0) + 0x10C8));
    if (temp_r3 != NULL) {
        fn_80009E70((*(void**)((u8*)(temp_r3) + 4)));
        fn_80009E70((*(void**)((u8*)((*(void**)((u8*)(arg0) + 0x10C8))) + 8)));
        fn_80009E70((*(void**)((u8*)((*(void**)((u8*)(arg0) + 0x10C8))) + 0xC)));
        fn_80009E70((*(void**)((u8*)(arg0) + 0x10C8)));
        (*(void**)((u8*)(arg0) + 0x10C8)) = NULL;
    }
}

// ---- end of sweep code ----
