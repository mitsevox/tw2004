// LLFont.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): not yet decompiled;
// the sweep code below is the matched small functions.

#include "engine.h"
#include "gx.h"

static f32 lbl_801A3478[7];             // the projection saved while fonts draw
static f32 lbl_801A3494[6];             // the viewport saved while fonts draw

void fn_80012444(f32* pViewport);

// ---- sweep code (not yet cleaned up) ----

void fn_80011164(LLFont* pFont);
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

// The width of sz in pFont at pCtx's scale: each glyph's advance, or pCtx->fB0 for every character
// when pCtx->nAC is set (a fixed pitch). A character with no glyph uses glyph 0xAC; with neither, it
// adds nothing.
f32 fn_80011C90(LLFont* pFont, UFontContext* pCtx, char* sz) {
    f32 fScale;
    f32 fWidth;
    LLGlyph* pGlyph;
    f32 fAdvance;

    if (pFont == NULL || pCtx == NULL) {
        return 0.0f;
    }
    fScale = pCtx->f78 * pCtx->f7C;
    fWidth = 0.0f;
    for (; *sz != '\0'; sz++) {
        pGlyph = pFont->apGlyphs[(u8)*sz];
        if (pGlyph == NULL) {
            pGlyph = pFont->apGlyphs[0xAC];
            if (pGlyph == NULL) {
                continue;
            }
        }
        if (pCtx->nAC == 0) {
            fAdvance = pGlyph->f18;
        } else {
            fAdvance = pCtx->fB0;
        }
        fWidth += fScale * fAdvance;
    }
    return fWidth;
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

void GXSetTexCoordGen2();
void fn_8001247C(s32 p0, s32 p1, s32 p2, s32 p3);
void fn_800124CC(void);
void fn_800124A4(void);
void fn_800124A8(void);
void fn_80012520(u32 ePrim, u32 eFormat, u16 nVerts);
void fn_80012540(f32 farg0, f32 farg1);
void fn_80012550(s32 p0);
void fn_8001255C(f32 farg0, f32 farg1);

void fn_80012438(LLFont* pFont) {
    pFont->n474 = 0;
}

void fn_80012444(f32* pViewport) {
    GXSetViewport(pViewport[0], pViewport[1], pViewport[2], pViewport[3], pViewport[4], pViewport[5]);
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

// Raises lbl_80281B9C to the FIFO's current fill if that is more.
void fn_800124CC(void) {
    u32 uCount;
    u8 bOverHigh;
    u8 bUnderLow;
    u8 bCpuWrite;
    u8 bGpRead;
    u8 bWrapped;

    GXGetFifoStatus(lbl_80281BA0, &bOverHigh, &bUnderLow, &uCount, &bCpuWrite, &bGpRead, &bWrapped);
    lbl_80281B9C = (lbl_80281B9C <= uCount) ? uCount : lbl_80281B9C;
}

void fn_80012520(u32 ePrim, u32 eFormat, u16 nVerts) {
    GXBegin(ePrim, eFormat, nVerts);
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
