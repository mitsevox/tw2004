// TerrainData.c (our name): made by fold.py from 6 sweep files; not yet described.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern s32 lbl_80281C10;
void fn_8000BF8C(void);
extern s32 lbl_80280DB0;
void fn_8000C0F0(void);
f32 fn_8000C244(u8* p0, u8* p1, u8* p2);
extern u8 lbl_801D3CB0[];
s32 fn_8000C594(void);
void fn_800BAE5C();
void fn_8000C5A4(u8* p0);
f32 fn_8000C5FC(u8* p0, u8* p1);

void fn_8000BF8C(void) {
    lbl_80281C10 = (lbl_80281C10 - 1);
}

void fn_8000C0F0(void) {
    lbl_80281C10 = 0;
    lbl_80280DB0 = -1;
}

f32 fn_8000C244(u8* p0, u8* p1, u8* p2) {
    return ((*(f32*)p1 - *(f32*)p0) * (*(f32*)(p2 + 0x8) - *(f32*)(p0 + 0x8)) - ((*(f32*)p2 - *(f32*)p0) * (*(f32*)(p1 + 0x8) - *(f32*)(p0 + 0x8))));
}

s32 fn_8000C594(void) {
    return *(s32*)(lbl_801D3CB0 + 0x8);
}

void fn_8000C5A4(u8* p0) {
    fn_800BAE5C((p0 + 0x40), p0, (p0 + 0x80), 3);
}

f32 fn_8000C5FC(u8* p0, u8* p1) {
    return (*(f32*)(p0 + 0x8) * *(f32*)(p1 + 0x8) + (*(f32*)p0 * *(f32*)p1 + (*(f32*)(p0 + 0x4) * *(f32*)(p1 + 0x4))));
}

// ---- end of sweep code ----
