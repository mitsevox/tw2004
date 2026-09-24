// LLFont.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): not yet decompiled;
// the sweep code below is the matched small functions.

#include "engine.h"

// ---- sweep code (not yet cleaned up) ----

void fn_80011164(LLFont* pFont);
extern u8 lbl_801A3478[];
extern u8 lbl_801A3494[];
void GXSetProjectionv();
void fn_80012444();
void fn_800112DC(void);

// UFont.c passes its state; this build does nothing with it.
void fn_80011160(UFontState* pState) {
}

void fn_80011164(LLFont* pFont) {
    if (pFont->p470 != NULL) {
        fn_80009E70(pFont->p470);
        pFont->p470 = NULL;
    }
}

void fn_800111A4(LLFont* pFont) {
    fn_80011164(pFont);
    fn_80009E70(pFont);
}

void fn_800112DC(void) {
    fn_80012444(lbl_801A3494);
    GXSetProjectionv(lbl_801A3478);
}

// UFont.c passes the font; this build does nothing with it.
void fn_80011C8C(LLFont* pFont) {
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

void GXSetTexCoordGen2();
void fn_8001247C(s32 p0, s32 p1, s32 p2, s32 p3);
void fn_800124CC();
void fn_800124A4(void);
void fn_800124A8(void);
void GXBegin();
void fn_80012520(void);
void fn_80012540(f32 farg0, f32 farg1);
void fn_80012550(s32 p0);
void fn_8001255C(f32 farg0, f32 farg1);

void fn_80012438(LLFont* pFont) {
    pFont->n474 = 0;
}

void fn_8001247C(s32 p0, s32 p1, s32 p2, s32 p3) {
    GXSetTexCoordGen2(p0, p1, p2, p3, 0, 125);
}

void fn_800124A4(void) {
}

void fn_800124A8(void) {
    fn_800124A4();
    fn_800124CC();
}

void fn_80012520(void) {
    GXBegin();
}

void fn_80012540(f32 farg0, f32 farg1) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
}

void fn_80012550(s32 p0) {
    *(volatile s32*)0xCC008000 = p0;
}

void fn_8001255C(f32 farg0, f32 farg1) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
}

// ---- end of sweep code ----
