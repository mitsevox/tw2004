// CharSliders.c (EA's name, from its asserts; TW06): not yet decompiled; the sweep code below is
// the matched small functions.

#include "game_types.h"

// ---- sweep code (not yet cleaned up) ----

void fn_8010C714(void);
extern u8 gSession[];
extern s32 lbl_80281764;
u8* fn_8010C718(void);
void fn_80125910();
void fn_8010C73C(void);
void fn_8010C740(void);
void fn_8010D330(void);
void fn_8010C8B8(void);
s32 fn_8010C8D8(void);
s32 fn_8010C8E0(void);
void fn_8010CA2C();
u32 fn_8010D33C(s32* arg0);
void fn_8010C934(void);
void fn_8010C958(void);
void fn_8010D278();
void fn_8010D230(void);
void fn_8010D250(void);
extern s32 lbl_80281760;
void fn_8010D32C(void);
void fn_8010D334(s32 v);
s32 Game_GetMode();
s32 fn_8010D364(void);
extern s32 lbl_8028176C;
s32 fn_8010D390(void);
void fn_800A7664();
void fn_8010D428(s32 p0, s32 p1);

void fn_8010C714(void) {
}

u8* fn_8010C718(void) {
    *(s32*)(gSession + 0x58) = 0;
    *(s32*)(gSession + 0x5C) = 0;
    *(s32*)(gSession + 0xE98) = 0;
    lbl_80281764 = 5;
    return gSession;
}

void fn_8010C73C(void) {
}

void fn_8010C740(void) {
    fn_80125910(1);
}

void fn_8010C8B8(void) {
    fn_8010D330();
}

s32 fn_8010C8D8(void) {
    return 0;
}

s32 fn_8010C8E0(void) {
    return 5;
}

void fn_8010C934(void) {
    fn_8010D33C(0);
}

void fn_8010C958(void) {
    fn_8010CA2C();
}

void fn_8010D230(void) {
    fn_8010D278();
}

void fn_8010D250(void) {
    fn_8010D278();
    lbl_80281764 = 5;
}

void fn_8010D32C(void) {
}

void fn_8010D330(void) {
}

void fn_8010D334(s32 v) {
    lbl_80281760 = v;
}

u32 fn_8010D33C(s32* arg0) {
    if (arg0 != NULL) {
        *arg0 = lbl_80281764;
    }
    return (u32) ((5 - lbl_80281764) | (lbl_80281764 - 5)) >> 0x1FU;
}

s32 fn_8010D364(void) {
    s32 t0;
    t0 = Game_GetMode();
    return (((u32)__cntlzw((26 - t0)) >> 5) & 0xFF);
}

s32 fn_8010D390(void) {
    if (((s32) lbl_80281764 != 5) && ((s32) lbl_8028176C > 0)) {
        return 1;
    }
    return 0;
}

void fn_8010D428(s32 p0, s32 p1) {
    fn_800A7664(8, p0, p1);
}

// ---- end of sweep code ----
