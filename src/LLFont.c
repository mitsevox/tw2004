// LLFont.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the font renderer
// under UFont.c: loads 'sfn ' fonts, sets GX up for text and draws glyphs.

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

void fn_80012444(const f32* pViewport);
void fn_8001247C(s32 eDst, s32 eFunc, s32 eSrc, s32 nMtx);
u32 fn_8001208C(UFontContext* pCtx, u8 r, u8 g, u8 b, u8 a, f32 fXScale, f32 fYScale, f32 fX,
                f32 fY);
void fn_800124A8(void);
void fn_80012520(u32 ePrim, u32 eFormat, u16 nVerts);
void fn_80012540(f32 farg0, f32 farg1);
void fn_80012550(s32 p0);
void fn_8001255C(f32 farg0, f32 farg1);

static u8 lbl_80281C88;                 // set while fn_8001144C word-wraps (fn_80011D0C calls back in)

// Byte swaps for a stream stored little-endian.
#define LLFONT_SWAP32(x) (((u32)(x) >> 24) + (((x) & 0xFF0000) >> 8) + (((x) << 8) & 0xFF0000) + ((x) << 24))
#define LLFONT_SWAP16(x) ((((x) & 0xFF00) >> 8) + (((x) << 8) & 0xFF00))

// Makes a font from an 'sfn ' stream object: swaps a little-endian stream, allocates the font with
// its glyph records and glyphs, builds each glyph's texture coordinates and sizes, and reorders the
// bitmap's rows of 4-bit texels into the 8x8 tiles of a C4 texture.
LLFont* FO_spLoadFontFromStream(void* pData, UFontState* pState) {
    LLFontFile* pFile;
    u8* pBytes;
    LLFont* pFont;
    u8* pRec;
    int i;
    int nPalette;
    int nRowBytes;
    int nTexHeight;
    int nTileRow;
    int nTileCol;
    int nRow;
    int nByte;
    int nX;
    int nY;
    int k;
    f32 fX;
    f32 fY;
    u8* pSrc;
    LLTexelPair* pDst;
    LLGlyphRec* pGlyphRec;

    pFile = pData;
    pBytes = pData;
    if (pFile->n0C > 100) {
        pFile->n00 = LLFONT_SWAP32(pFile->n00);
        pFile->u04 = LLFONT_SWAP32(pFile->u04);
        pFile->uVersion = LLFONT_SWAP16(pFile->uVersion);
        pFile->nGlyphs = LLFONT_SWAP16(pFile->nGlyphs);
        pFile->n0C = LLFONT_SWAP32(pFile->n0C);
        pFile->uGlyphs = LLFONT_SWAP32(pFile->uGlyphs);
        pFile->u18 = LLFONT_SWAP32(pFile->u18);
        pFile->uBitmap = LLFONT_SWAP32(pFile->uBitmap);
    }
    pFont = fn_80009B34(sizeof(LLFont) + pFile->nGlyphs * sizeof(LLGlyphRec) +
                            pFile->nGlyphs * sizeof(LLGlyph),
                        2, 16, "LLFont.c", 630);
    pFont->pRecs = (LLGlyphRec*)(pFont + 1);
    pFont->pGlyphs = (LLGlyph*)((LLGlyphRec*)(pFont + 1) + pFile->nGlyphs);
    fn_80012438(pFont);
    for (i = 0; i < 256; i++) {
        pFont->apGlyphs[i] = NULL;
    }
    nPalette = 0;
    switch (pFile->n0C & 6) {
    case 2:
        nPalette = 2;
        break;
    case 4:
        nPalette = 1;
        break;
    }
    pFont->nPalette = nPalette;
    pFont->u464 = pState->a00[nPalette];

    // The glyph records: 12 bytes each from version 200, 11 before.
    pRec = pBytes + pFile->uGlyphs;
    for (i = 0; i < pFile->nGlyphs; i++) {
        pFont->pRecs[i] = *(LLGlyphRec*)pRec;
        if (pFile->uVersion >= 200) {
            pRec += 12;
        } else {
            pRec += 11;
        }
        pGlyphRec = &pFont->pRecs[i];
        pFont->apGlyphs[pGlyphRec->aCode[0] + (pGlyphRec->aCode[1] << 8)] = &pFont->pGlyphs[i];
    }

    pBytes += pFile->uBitmap;
    Mem_cpy(&pFont->bitmap, pBytes, sizeof(LLFontBitmap));
    pFont->bitmap.nWidth = LLFONT_SWAP16(pFont->bitmap.nWidth);
    pFont->bitmap.nHeight = LLFONT_SWAP16(pFont->bitmap.nHeight);
    pFont->bitmap.n08 = LLFONT_SWAP16(pFont->bitmap.n08);
    pFont->bitmap.n0A = LLFONT_SWAP16(pFont->bitmap.n0A);
    pFont->bitmap.n0C = LLFONT_SWAP16(pFont->bitmap.n0C);
    pFont->bitmap.n0E = LLFONT_SWAP16(pFont->bitmap.n0E);
    nRowBytes = (pFont->bitmap.nWidth + 7) / 8 * 8 / 2;
    nTexHeight = (pFont->bitmap.nHeight + 7) / 8 * 8;
    pFont->p470 = fn_80009B34(nRowBytes * nTexHeight, 2, 32, "LLFont.c", 795);

    // Rows of texels (two per byte, first in the high nibble) become 8x8 tiles, first texel low.
    pDst = pFont->p470;
    nY = 0;
    for (nTileRow = 0; nTileRow < nTexHeight / 8; nTileRow++) {
        nByte = 0;
        for (nTileCol = 0; nTileCol < nRowBytes / 4; nTileCol++) {
            pSrc = pBytes + sizeof(LLFontBitmap) + pFont->bitmap.nWidth / 2 * nY + nByte;
            for (nRow = 0; nRow < 8; nRow++) {
                // EA compares the byte position with the width in texels.
                nX = nByte;
                for (k = 0; k < 2; k++) {
                    if (nX >= pFont->bitmap.nWidth) {
                        pDst[0].uFirst = 0;
                        pDst[0].uSecond = 0;
                    } else {
                        pDst[0].uSecond = *pSrc >> 4;
                        pDst[0].uFirst = *pSrc++;
                    }
                    nX++;
                    if (nX >= pFont->bitmap.nWidth) {
                        pDst[1].uFirst = 0;
                        pDst[1].uSecond = 0;
                    } else {
                        pDst[1].uSecond = *pSrc >> 4;
                        pDst[1].uFirst = *pSrc++;
                    }
                    pDst += 2;
                    nX++;
                }
                pSrc += pFont->bitmap.nWidth / 2 - 4;
            }
            nByte += 4;
        }
        nY += 8;
    }

    pFont->n42C = 0;
    pFont->n430 = 0;
    pFont->n434 = pFont->bitmap.nWidth;
    pFont->n438 = pFont->bitmap.nWidth;
    pFont->n43C = pFont->bitmap.nHeight;
    for (i = 0; i < pFile->nGlyphs; i++) {
        pFont->pGlyphs[i].pRec = &pFont->pRecs[i];
        pGlyphRec = &pFont->pRecs[i];
        fX = (u16)(pGlyphRec->aX[0] + (pGlyphRec->aX[1] << 8));
        fY = (u16)(pGlyphRec->aY[0] + (pGlyphRec->aY[1] << 8));
        pFont->pGlyphs[i].fU0 = fX / (nRowBytes * 2);
        pFont->pGlyphs[i].fU1 = (fX + pGlyphRec->uWidth) / (nRowBytes * 2);
        pFont->pGlyphs[i].fV0 = fY / nTexHeight;
        pFont->pGlyphs[i].fV1 = (fY + pGlyphRec->uHeight) / nTexHeight;
        pFont->pGlyphs[i].f18 = pFont->pRecs[i].n08 * (1.0f / 512.0f);
        pFont->pGlyphs[i].f1C = pFont->pRecs[i].n09 * (1.0f / 512.0f);
        pFont->pGlyphs[i].f20 = pFont->pRecs[i].n0A / 448.0f;
        pFont->pGlyphs[i].fWidth = pFont->pRecs[i].uWidth * (1.0f / 512.0f);
        pFont->pGlyphs[i].fHeight = pFont->pRecs[i].uHeight / 448.0f;
    }
    pFont->f00 = pFile->n13 / 448.0f;
    pFont->n04 = 0;
    GXInitTexObjCI(&pFont->tex, pFont->p470, nRowBytes * 2, nTexHeight, 8, 0, 0, 0, 0);
    return pFont;
}

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

// Draws sz (NULL: pCtx->szText) in pFont with pCtx's settings, one textured quad a glyph. uA8 set:
// word-wrap it through fn_80011D0C, which calls back here a line at a time. n9C bit 0x10000
// draws a shadow first (colour nC4/uC8, moved by fCC, fD0). The low byte of n9C aligns the text
// across (1: right, 2: centre, 4: by fBC), bits 8-10 down (by fC0 for 0x400); fB8 turns it.
void fn_8001144C(LLFont* pFont, UFontContext* pCtx, char* sz) {
    f32 fAlignY;
    f32 fSizeY;
    LLGlyph* pGlyph;
    char* p;
    f32 fAlignX;
    f32 fScaleY;
    f32 fWidth;
    f32 fOffY;
    f32 fOffX;
    f32 fLeft;
    f32 fY;
    f32 fTop;
    f32 fScaleX;
    f32 fAdvance;
    f32 fSizeX;
    f32 fGradX;
    f32 fX;
    f32 fGradY;
    u32 uColor;
    u8 bMeasured;
    f32 fX0;
    f32 fY0;
    f32 fX1;
    f32 fY2;
    f32 fY1;
    f32 fX2;
    f32 fX3;
    f32 fAdvScale;
    f32 fY3;
    f32 fSin;
    f32 fCos;
    f32 fNegSin;
    f32 fRun;
    f32 fRight;
    f32 fBottom;
    s32 nSaved;
    s32 nSavedFont;
    GXColor uSavedColor;
    f32 fSavedY;
    f32 fSavedX;

    if (pCtx->uA8 != 0 && lbl_80281C88 == 0) {
        lbl_80281C88 = 1;
        fn_80011D0C(pFont, pCtx, 1, (sz == NULL) ? pCtx->szText : sz);
        lbl_80281C88 = 0;
        return;
    }
    if (pCtx->n9C & 0x10000) {
        // The shadow: the same text in the shadow's colour, moved, without a shadow of its own.
        uSavedColor = pCtx->u5C;
        nSavedFont = pCtx->nA4;
        fSavedX = pCtx->f70;
        fSavedY = pCtx->f74;
        pCtx->nA4 = pCtx->nC4;
        pCtx->u5C = pCtx->uC8;
        pCtx->f70 = pCtx->f70 + pCtx->fCC;
        pCtx->f74 = pCtx->f74 + pCtx->fD0;
        pCtx->n9C &= ~0x10000;
        fn_8001144C(pFont, pCtx, sz);
        pCtx->nA4 = nSavedFont;
        pCtx->u5C = uSavedColor;
        pCtx->f70 = fSavedX;
        pCtx->f74 = fSavedY;
        pCtx->n9C |= 0x10000;
    }
    if (sz != NULL) {
        p = sz;
    } else {
        p = pCtx->szText;
    }
    fAlignX = 0.0f;
    bMeasured = 0;
    fAlignY = fAlignX;
    fSizeY = pCtx->f80;
    fSizeX = pCtx->f7C;
    fScaleY = pCtx->f88 * (pCtx->n6C * fSizeY);
    fScaleX = pCtx->n68 * fSizeX;
    if (pCtx->n9C & 0xFF) {
        fWidth = fn_80011C90(pFont, pCtx, p);
        bMeasured = 1;
        if (pCtx->n9C & 1) {
            fAlignX = 1.0f;
        } else if (pCtx->n9C & 2) {
            fAlignX = 0.5f;
        } else if (pCtx->n9C & 4) {
            fAlignX = pCtx->fBC;
        }
    }
    if (pCtx->n9C & 0xFF00) {
        if (pCtx->n9C & 0x100) {
            fAlignY = 1.0f;
        } else if (pCtx->n9C & 0x200) {
            fAlignY = 0.5f;
        } else if (pCtx->n9C & 0x400) {
            fAlignY = pCtx->fC0;
        }
    }
    fX = pCtx->f70;
    fY = pCtx->f74;
    if (0.0f != pCtx->fB8) {
        fSin = fn_800095F0(pCtx->fB8);
        fCos = fn_80009638(pCtx->fB8);
        if (0.0f != fAlignX) {
            fWidth = fn_80011C90(pFont, pCtx, p);
            bMeasured = 1;
            fX -= fCos * (fWidth * fAlignX);
            fY += fSin * (fWidth * fAlignX);
        }
        fX -= fSin * (pFont->f00 * fSizeY * fAlignY);
        fY -= fCos * (pFont->f00 * fSizeY * fAlignY);
    } else {
        if (0.0f != fAlignX) {
            fWidth = fn_80011C90(pFont, pCtx, p);
            bMeasured = 1;
            fX -= fWidth * fAlignX;
        }
        fY -= pFont->f00 * fSizeY * fAlignY;
    }
    fAdvScale = pCtx->f78;
    fRun = 0.0f;
    fLeft = fX * pCtx->n68 + pCtx->n60;
    fTop = pCtx->n64 * pCtx->f88 + (pCtx->f88 * (fY * pCtx->n6C) + (0.5f - 0.5f * pCtx->f88));
    if (pCtx->n10 != 0) {
        if (!bMeasured && pCtx->n10 != 0) {
            fWidth = fn_80011C90(pFont, pCtx, p);
        }
        fGradX = 1.0f / (fWidth / fSizeX);
        fGradY = 1.0f / pFont->f00;
    }
    if (pCtx->nA4 == 0x12) {
        uColor = *(u32*)&pCtx->u5C; // port: the GXColor's bytes in the PowerPC's (big-endian) order
    } else {
        uColor = lbl_80186A80[pCtx->nA4];
    }
    nSaved = pFont->n418;
    fNegSin = -fSin; // EA bug: fSin is only set for turned text (and only used then)
    for (; *p != '\0'; p++) {
        pGlyph = pFont->apGlyphs[(u8)*p];
        if (pGlyph == NULL) {
            pGlyph = pFont->apGlyphs[0xAC];
            if (pGlyph == NULL) {
                continue;
            }
        }
        fOffX = pGlyph->f1C * fScaleX;
        fOffY = pGlyph->f20 * fScaleY;
        if (*p != ' ') {
            switch (pFont->n474) {
            case 0:
                fn_80012520(0x80, 7, 4);
                fX0 = fLeft + fOffX;
                fY0 = fTop + fOffY;
                fn_8001255C(fX0, fY0);
                fn_80012550(uColor);
                fn_80012540(pGlyph->fU0, pGlyph->fV0);
                fn_8001255C(pGlyph->fWidth * fScaleX + fX0, fY0);
                fn_80012550(uColor);
                fn_80012540(pGlyph->fU1, pGlyph->fV0);
                fn_8001255C(pGlyph->fWidth * fScaleX + fX0, pGlyph->fHeight * fScaleY + fY0);
                fn_80012550(uColor);
                fn_80012540(pGlyph->fU1, pGlyph->fV1);
                fn_8001255C(fX0, pGlyph->fHeight * fScaleY + fY0);
                fn_80012550(uColor);
                fn_80012540(pGlyph->fU0, pGlyph->fV1);
                fn_800124A8();
                break;
            case 1:
                if (0.0f != pCtx->fB8) {
                    fY0 = (fCos * pGlyph->f20 - fSin * pGlyph->f1C) * fScaleY + fTop;
                    fX0 = (fSin * pGlyph->f20 + fCos * pGlyph->f1C) * fScaleX + fLeft;
                    fY1 = fNegSin * pGlyph->fWidth * fScaleY + fY0;
                    fX1 = fCos * pGlyph->fWidth * fScaleX + fX0;
                    fY2 = fY1 + fCos * pGlyph->fHeight * fScaleY;
                    fX3 = fX0 + fSin * pGlyph->fHeight * fScaleX;
                    fX2 = fX1 + fSin * pGlyph->fHeight * fScaleX;
                    fY3 = fY0 + fCos * pGlyph->fHeight * fScaleY;
                } else {
                    fX0 = fLeft + fOffX;
                    fY0 = fTop + fOffY;
                    fX2 = pGlyph->fWidth * fScaleX + fX0;
                    fX3 = fX0;
                    fY2 = pGlyph->fHeight * fScaleY + fY0;
                    fY1 = fY0;
                    fX1 = fX2;
                    fY3 = fY2;
                }
                if (pCtx->n10 != 0) {
                    // port: EA calls fn_8001208C without its colour arguments and drops the
                    // colours it returns, so the gradients change nothing here.
                    if (pCtx->n10 & 4) {
                        ((u32 (*)(UFontContext*, f32, f32, f32, f32))fn_8001208C)(
                            pCtx, fGradX, fGradY, 0.5f * pGlyph->fWidth + (fRun + pGlyph->f1C),
                            0.5f * pGlyph->fHeight + (0.0f + pGlyph->f20));
                    } else {
                        f32 fGX0 = fRun + pGlyph->f1C;
                        f32 fGY0 = 0.0f + pGlyph->f20;
                        f32 fGX1 = fGX0 + pGlyph->fWidth;
                        f32 fGY1 = fGY0 + pGlyph->fHeight;

                        ((u32 (*)(UFontContext*, f32, f32, f32, f32))fn_8001208C)(pCtx, fGradX, fGradY,
                                                                                   fGX0, fGY0);
                        ((u32 (*)(UFontContext*, f32, f32, f32, f32))fn_8001208C)(pCtx, fGradX, fGradY,
                                                                                   fGX1, fGY0);
                        ((u32 (*)(UFontContext*, f32, f32, f32, f32))fn_8001208C)(pCtx, fGradX, fGradY,
                                                                                   fGX0, fGY1);
                        ((u32 (*)(UFontContext*, f32, f32, f32, f32))fn_8001208C)(pCtx, fGradX, fGradY,
                                                                                   fGX1, fGY1);
                    }
                }
                fn_80012520(0x80, 7, 4);
                fn_8001255C(fX0, fY0);
                fn_80012550(uColor);
                fn_80012540(pGlyph->fU0, pGlyph->fV0);
                fn_8001255C(fX1, fY1);
                fn_80012550(uColor);
                fn_80012540(pGlyph->fU1, pGlyph->fV0);
                fn_8001255C(fX2, fY2);
                fn_80012550(uColor);
                fn_80012540(pGlyph->fU1, pGlyph->fV1);
                fn_8001255C(fX3, fY3);
                fn_80012550(uColor);
                fn_80012540(pGlyph->fU0, pGlyph->fV1);
                fn_800124A8();
                break;
            }
            pFont->n46C++;
        }
        if (pCtx->nAC == 0) {
            fAdvance = pGlyph->f18;
        } else {
            fAdvance = pCtx->fB0;
        }
        fAdvance = fAdvScale * fAdvance;
        fRun += fAdvance;
        if (0.0f != pCtx->fB8) {
            fLeft += fScaleX * (fCos * fAdvance);
            fTop += fScaleY * (fNegSin * fAdvance);
        } else {
            fLeft += fAdvance * fScaleX;
        }
    }
    pFont->n418 = nSaved;
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

// Word-wraps sz (NULL: pCtx->szText) into the box pCtx->a8C (x, y, width, height), drawing each line
// when bDraw is set. f70/f74 come in as 0..1 within the box and are turned into screen positions;
// f74 then steps down a line at a time. Lines break at spaces and '\n' ('\r' counts as a space);
// a word wider than the box is cut where it stops fitting. Returns the number of lines broken,
// 0 for an empty box, or -1 when not even one character fits.
int fn_80011D0C(LLFont* pFont, UFontContext* pCtx, u8 bDraw, char* sz) {
    f32 fLine;
    char* pLine;
    char* p;
    char* pBreak;
    int nLines;
    char cSaved;
    char c;
    int n;

    if (pCtx->a8C[2] <= 0.0f || pCtx->a8C[3] <= 0.0f) {
        return 0;
    }
    nLines = 0;
    fLine = pCtx->fB4 * (pFont->f00 * pCtx->f80);
    pCtx->f70 = pCtx->a8C[2] * pCtx->f70 + pCtx->a8C[0];
    pCtx->f74 = pCtx->a8C[3] * pCtx->f74 + pCtx->a8C[1];
    if (sz == NULL) {
        pLine = pCtx->szText;
    } else {
        pLine = sz;
    }
    p = pLine;
    pBreak = NULL;
    for (;; p++) {
        c = *p;
        if (c != ' ' && c != '\n' && c != '\r' && c != '\0') {
            continue;
        }
        cSaved = c;
        if (c == '\r') {
            cSaved = ' ';
        }
        *p = '\0';
        if (fn_80011C90(pFont, pCtx, pLine) <= pCtx->a8C[2]) {
            // The line still fits up to here: remember the break and go on.
            *p = cSaved;
            pBreak = p;
            if (*p == '\0') {
                if (bDraw) {
                    fn_8001144C(pFont, pCtx, pLine);
                }
                break;
            }
            if (*p == '\r') {
                *p = ' ';
            }
            if (*p == '\n') {
                *p = '\0';
                if (bDraw) {
                    fn_8001144C(pFont, pCtx, pLine);
                }
                *p = cSaved;
                nLines++;
                pCtx->f74 += fLine;
                if (bDraw && pCtx->f74 + fLine > pCtx->a8C[1] + pCtx->a8C[3]) {
                    return nLines;
                }
                for (p++; *p != '\0'; p++) {
                    if (*p != ' ') {
                        break;
                    }
                }
                pLine = p;
                pBreak = NULL;
                p--;
            }
        } else {
            *p = cSaved;
            if (pBreak == NULL) {
                // One word wider than the box: cut it where it stops fitting.
                n = p - pLine - 1;
                for (; n > 0; n--) {
                    cSaved = pLine[n];
                    pLine[n] = '\0';
                    if (fn_80011C90(pFont, pCtx, pLine) <= pCtx->a8C[2]) {
                        if (bDraw) {
                            fn_8001144C(pFont, pCtx, pLine);
                        }
                        nLines++;
                        pCtx->f74 += fLine;
                        if (bDraw && pCtx->f74 + fLine > pCtx->a8C[1] + pCtx->a8C[3]) {
                            return nLines;
                        }
                        pLine[n] = cSaved;
                        pLine += n;
                        p = pLine;
                        break;
                    }
                    pLine[n] = cSaved;
                }
                if (n == 0) {
                    return -1;
                }
            } else {
                // Break the line at the last space that fitted.
                c = *pBreak;
                *pBreak = '\0';
                if (bDraw) {
                    fn_8001144C(pFont, pCtx, pLine);
                }
                *pBreak = c;
                nLines++;
                pCtx->f74 += fLine;
                if (bDraw && pCtx->f74 + fLine > pCtx->a8C[1] + pCtx->a8C[3]) {
                    return nLines;
                }
                for (; *pBreak != '\0'; pBreak++) {
                    if (*pBreak != ' ' && *pBreak != '\n') {
                        break;
                    }
                }
                pLine = pBreak;
                p = pBreak - 1;
                pBreak = NULL;
            }
        }
    }
    return nLines;
}

// The colour of a text vertex under pCtx's colour gradients, as the four bytes of a GXColor.
// Unless a gradient is on, it is the colour passed in. Across (n10 bit 1) the position runs
// fX * fXScale from f04 to f08 and wraps, and the colour blends between the two stops around it;
// down (bit 2) fY * fYScale wraps the same way and blends a14[4] into a14[5].
// (fn_8001144C's calls load only pCtx and the four floats, and drop the result.)
u32 fn_8001208C(UFontContext* pCtx, u8 r, u8 g, u8 b, u8 a, f32 fXScale, f32 fYScale, f32 fX,
                f32 fY) {
    GXColor color;
    UFontStop* pNext;
    UFontStop* pStop;
    f32 fU;
    f32 fV;
    f32 fT;
    s32 i;

    fU = fX * fXScale;
    fV = fY * fYScale;
    pStop = &pCtx->a14[0];
    pNext = &pCtx->a14[1];
    fU -= pCtx->f04;
    fU *= pCtx->f0C;
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

void fn_800124CC(void);
void fn_800124A4(void);

void fn_80012438(LLFont* pFont) {
    pFont->n474 = 0;
}

void fn_80012444(const f32* pViewport) {
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
