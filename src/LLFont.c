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
    pFont = fn_80009B34(pFile->nGlyphs * sizeof(LLGlyphRec) + pFile->nGlyphs * sizeof(LLGlyph) +
                            sizeof(LLFont),
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
    char c;
    char* pCut;
    char cSaved;
    int n;

    if (pCtx->a8C[2] <= 0.0f || pCtx->a8C[3] <= 0.0f) {
        return 0;
    }
    nLines = 0;
    pCtx->f70 = pCtx->a8C[2] * pCtx->f70 + pCtx->a8C[0];
    fLine = pCtx->fB4 * (pFont->f00 * pCtx->f80);
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
                pCut = pLine + n;
                for (; n > 0; n--, pCut--) {
                    cSaved = *pCut;
                    *pCut = '\0';
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
                    *pCut = cSaved;
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
                for (; *pBreak != '\0' && (*pBreak == ' ' || *pBreak == '\n'); pBreak++) {
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
