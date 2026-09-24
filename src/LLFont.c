// LLFont.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the font renderer
// under UFont.c: loads 'sfn ' fonts, sets GX up for text and draws glyphs. The loader
// (FO_spLoadFontFromStream) and the glyph drawing are not decompiled yet.

#include "engine.h"
#include "gx.h"

static f32 lbl_801A3478[7];             // the projection saved while fonts draw
static f32 lbl_801A3494[6];             // the viewport saved while fonts draw

// The three glyph palettes: a grey level and an alpha (0x80 = opaque) per glyph pixel value.
static GXColor lbl_801869C0[3][16] = {
    {
        { 0xFF, 0xFF, 0xFF, 0x00 }, { 0xFF, 0xFF, 0xFF, 0x08 }, { 0xFF, 0xFF, 0xFF, 0x11 },
        { 0xFF, 0xFF, 0xFF, 0x19 }, { 0xFF, 0xFF, 0xFF, 0x22 }, { 0xFF, 0xFF, 0xFF, 0x2A },
        { 0xFF, 0xFF, 0xFF, 0x33 }, { 0xFF, 0xFF, 0xFF, 0x3B }, { 0xFF, 0xFF, 0xFF, 0x44 },
        { 0xFF, 0xFF, 0xFF, 0x4C }, { 0xFF, 0xFF, 0xFF, 0x55 }, { 0xFF, 0xFF, 0xFF, 0x5D },
        { 0xFF, 0xFF, 0xFF, 0x66 }, { 0xFF, 0xFF, 0xFF, 0x6E }, { 0xFF, 0xFF, 0xFF, 0x77 },
        { 0xFF, 0xFF, 0xFF, 0x80 },
    },
    {
        { 0x80, 0x80, 0x80, 0x00 }, { 0x6F, 0x6F, 0x6F, 0x07 }, { 0x5F, 0x5F, 0x5F, 0x0F },
        { 0x50, 0x50, 0x50, 0x17 }, { 0x3F, 0x3F, 0x3F, 0x20 }, { 0x2F, 0x2F, 0x2F, 0x4F },
        { 0x20, 0x20, 0x20, 0x5F }, { 0x0F, 0x0F, 0x0F, 0x6F }, { 0x00, 0x00, 0x00, 0x80 },
        { 0x12, 0x12, 0x12, 0x80 }, { 0x24, 0x24, 0x24, 0x80 }, { 0x36, 0x36, 0x36, 0x80 },
        { 0x48, 0x48, 0x48, 0x80 }, { 0x5B, 0x5B, 0x5B, 0x80 }, { 0x6D, 0x6D, 0x6D, 0x80 },
        { 0x80, 0x80, 0x80, 0x80 },
    },
    {
        { 0xFF, 0xFF, 0xFF, 0x00 }, { 0x00, 0x00, 0x00, 0x80 }, { 0x3F, 0x3F, 0x3F, 0x5F },
        { 0x7F, 0x7F, 0x7F, 0x3F }, { 0xBF, 0xBF, 0xBF, 0x1F }, { 0xFF, 0xFF, 0xFF, 0x19 },
        { 0xFF, 0xFF, 0xFF, 0x33 }, { 0xFF, 0xFF, 0xFF, 0x4C }, { 0xFF, 0xFF, 0xFF, 0x66 },
        { 0xFF, 0xFF, 0xFF, 0x80 }, { 0xCB, 0xCB, 0xCB, 0x80 }, { 0x98, 0x98, 0x98, 0x80 },
        { 0x65, 0x65, 0x65, 0x80 }, { 0x32, 0x32, 0x32, 0x80 }, { 0x00, 0x00, 0x00, 0x80 },
        { 0x00, 0x00, 0x00, 0x80 },
    },
};

void fn_80012444(f32* pViewport);
void fn_8001247C(s32 eDst, s32 eFunc, s32 eSrc, s32 nMtx);

// Builds the three glyph palettes as IA8 (alpha doubled from the table's 0x80 scale, the grey level
// from its red) and their TLUT objects.
void fn_80011034(UFontState* pState) {
    int i;
    int j;
    int nAlpha;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 16; j++) {
            nAlpha = lbl_801869C0[i][j].a * 2;
            if (nAlpha > 255) {
                nAlpha = 255;
            }
            pState->aaPalettes[i][j] = lbl_801869C0[i][j].r | (nAlpha << 8);
        }
        GXInitTlutObj(&pState->aTluts[i], pState->aaPalettes[i], 0, 16);
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80011164(LLFont* pFont);

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

// Sets GX up to draw text in 2D: position, colour and texture coordinates per vertex, no culling
// or depth test, and a 0..1 orthographic projection. The viewport and projection are saved for
// fn_800112DC.
void fn_800111D8(void) {
    f32 mProj[4][4];
    f32 mView[3][4];

    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxAttrFmt(7, 9, 0, 4, 0);
    GXSetVtxDesc(11, 1);
    GXSetVtxAttrFmt(7, 11, 1, 5, 0);
    GXSetVtxDesc(13, 1);
    GXSetVtxAttrFmt(7, 13, 1, 4, 0);
    GXGetViewportv(lbl_801A3494);
    GXGetProjectionv(lbl_801A3478);
    GXSetCullMode(0);
    GXSetZMode(0, 7, 1);
    PSMTXIdentity(mView);
    C_MTXOrtho(mProj, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    GXSetProjection(mProj, 1);
    GXLoadPosMtxImm(mView, 0);
    GXSetCurrentMtx(0);
}

// Puts back the viewport and projection fn_800111D8 saved.
void fn_800112DC(void) {
    fn_80012444(lbl_801A3494);
    GXSetProjectionv(lbl_801A3478);
}

// Sets the TEV up to draw pFont's glyphs: one stage, the glyph texture's colour times the vertex
// colour, through the font's palette, alpha blended.
void fn_80011310(LLFont* pFont, UFontState* pState) {
    pFont->n46C = 0;
    fn_8001247C(0, 1, 4, 60);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetNumTevStages(1);
    GXSetNumChans(1);
    GXSetNumTexGens(1);
    GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
    GXSetTevColorIn(0, 15, 8, 10, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 4, 5, 7);
    GXSetTevAlphaOp(0, 0, 0, 1, 1, 0);
    GXLoadTlut(&pState->aTluts[pFont->nPalette], 0);
    GXLoadTexObj(&pFont->tex, 0);
    GXSetBlendMode(1, 4, 5, 15);
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

// The colour of a text vertex under pCtx's colour gradients, as the four bytes of a GXColor.
// Unless a gradient is on, it is the colour passed in. Across (n10 bit 1) the position runs
// fX * fXScale from f04 to f08 and wraps, and the colour blends between the two stops around it;
// down (bit 2) fY * fYScale wraps the same way and blends a14[4] into a14[5].
u32 fn_8001208C(UFontContext* pCtx, u8 r, u8 g, u8 b, u8 a, f32 fX, f32 fY, f32 fXScale,
                f32 fYScale) {
    GXColor color;
    UFontStop* pStop;
    UFontStop* pNext;
    f32 fU;
    f32 fV;
    f32 fT;
    s32 i;

    fU = fXScale * fX;
    fV = fYScale * fY;
    pStop = pCtx->a14;
    pNext = pStop + 1;
    fU = pCtx->f0C * (fU - pCtx->f04);
    fU -= (s32)fU;
    if (fU < 0.0f) {
        fU = 1.0f + fU;
    }
    fV -= (s32)fV;
    if (fV < 0.0f) {
        fV = 1.0f + fV;
    }
    if (pCtx->n10 & 1) {
        for (i = 0; i < 4; i++) {
            if (fU <= pNext->fPos) {
                fT = pStop->fInvSpan * (fU - pStop->fPos);
                if (1.0f == fT) {
                    r = pNext->color.r;
                    g = pNext->color.g;
                    b = pNext->color.b;
                    a = pNext->color.a;
                } else if (0.0f == fT) {
                    r = pStop->color.r;
                    g = pStop->color.g;
                    b = pStop->color.b;
                    a = pStop->color.a;
                } else {
                    r = fT * (pNext->color.r - pStop->color.r) + pStop->color.r;
                    g = fT * (pNext->color.g - pStop->color.g) + pStop->color.g;
                    b = fT * (pNext->color.b - pStop->color.b) + pStop->color.b;
                    a = fT * (pNext->color.a - pStop->color.a) + pStop->color.a;
                }
                break;
            }
            pStop++;
            pNext++;
        }
    }
    if (pCtx->n10 & 2) {
        if (1.0f == fV) {
            r = pCtx->a14[5].color.r;
            g = pCtx->a14[5].color.g;
            b = pCtx->a14[5].color.b;
            a = pCtx->a14[5].color.a;
        } else if (0.0f == fV) {
            r = pCtx->a14[4].color.r;
            g = pCtx->a14[4].color.g;
            b = pCtx->a14[4].color.b;
            a = pCtx->a14[4].color.a;
        } else {
            r = fV * (pCtx->a14[5].color.r - pCtx->a14[4].color.r) + pCtx->a14[4].color.r;
            g = fV * (pCtx->a14[5].color.g - pCtx->a14[4].color.g) + pCtx->a14[4].color.g;
            b = fV * (pCtx->a14[5].color.b - pCtx->a14[4].color.b) + pCtx->a14[4].color.b;
            a = fV * (pCtx->a14[5].color.a - pCtx->a14[4].color.a) + pCtx->a14[4].color.a;
        }
    }
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;
    return *(u32*)&color; // port: the GXColor's bytes in the PowerPC's (big-endian) order
}

// ---- sweep code (not yet cleaned up) ----

void GXSetTexCoordGen2();
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

// A texture coordinate generator with no post-transform (matrix 125: GX's identity).
void fn_8001247C(s32 eDst, s32 eFunc, s32 eSrc, s32 nMtx) {
    GXSetTexCoordGen2(eDst, eFunc, eSrc, nMtx, 0, 125);
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
