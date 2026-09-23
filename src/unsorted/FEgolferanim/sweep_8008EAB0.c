// Small functions found by the sweep (sweep.py). Original file: FEgolferanim.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

extern u8* lbl_80281EE0;

u8 fn_8008EAB0(void);
void fn_8008EABC(u8 v);
void fn_8008EAC8(u8 v);
u8 fn_8008EAD4(void);
void fn_8008EAE0(s32 v);
s32 fn_8008EAEC(void);
void fn_8008EAF8(s32 v);
s32 fn_8008EB04(void);
u8 fn_8008EAB0(void) {
    return *(u8*)(lbl_80281EE0 + 0x1D1);
}

void fn_8008EABC(u8 v) {
    *(u8*)(lbl_80281EE0 + 0x1D1) = v;
}

void fn_8008EAC8(u8 v) {
    *(u8*)(lbl_80281EE0 + 0x1D2) = v;
}

u8 fn_8008EAD4(void) {
    return *(u8*)(lbl_80281EE0 + 0x1D2);
}

void fn_8008EAE0(s32 v) {
    *(s32*)(lbl_80281EE0 + 0x1D4) = v;
}

s32 fn_8008EAEC(void) {
    return *(s32*)(lbl_80281EE0 + 0x1D4);
}

void fn_8008EAF8(s32 v) {
    *(s32*)(lbl_80281EE0 + 0x1D8) = v;
}

s32 fn_8008EB04(void) {
    return *(s32*)(lbl_80281EE0 + 0x1D8);
}
