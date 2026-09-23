// SitDevFile.c (EA's name, from its asserts; TW06): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

extern s32 lbl_802821FC;
extern u8 lbl_80282200;
void fn_800BB0A8(void);
void fn_800BB0C8(void);
u8 fn_800BB0D4(void);
void fn_800BB0DC(void);
extern u8 gSession[];
extern s32 lbl_802821F8;
u8* fn_800BB1A8(s32 p0);
extern u8 lbl_801FA1AC[];
s32 fn_800BB1F8(s32 p0);
void fn_800BB4B0(void);
extern s32 lbl_802811B8;
void fn_80067B1C();
void fn_800BBADC(s32 p0);

void fn_800BB0A8(void) {
    fn_800BB0DC();
}

void fn_800BB0C8(void) {
    lbl_802821FC = 0;
}

u8 fn_800BB0D4(void) {
    return lbl_80282200;
}

void fn_800BB0DC(void) {
    lbl_80282200 = 0;
}

u8* fn_800BB1A8(s32 p0) {
    lbl_802821FC = p0;
    lbl_802821F8 = *(s32*)(gSession + 0x24);
    return gSession;
}

s32 fn_800BB1F8(s32 p0) {
    return (((u32)__cntlzw((1 - *(s32*)(lbl_801FA1AC + (p0 << 2)))) >> 5) & 0xFF);
}

void fn_800BB4B0(void) {
}

void fn_800BBADC(s32 p0) {
    fn_80067B1C(lbl_802811B8, 5, (p0 & 0xFFFF), (lbl_802811B8 + 192));
}

// ---- end of sweep code ----
