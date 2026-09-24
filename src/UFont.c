// UFont.c (EA's name, from its asserts; also in EA's 2002 source tree): the text layer over
// LLFont.c. It keeps six font slots and the current text settings (UFontContext). In mode 0 a
// drawn string is queued with a copy of the settings and drawn in UFont_DrawQueue; in mode 1 it is
// drawn at once.

#include "engine.h"

void FO_vClearPacketLists(void);
void UFont_LoadFontFromStream(UStreamObject* pObject);
void UFont_ResetContext(void);
void UFont_LoadFont(u32 uSlot, void* pData, s32 bFlag);
void UFont_SetFont(s32 nFont);
void FO_vSetCurrentAddMode(s32 nMode);
char* FO_chpCopyString(char* sz);
f32  UFont_GetStringWidth(char* sz, int nFont);
void FO_vSetFontContext(void);
void FO_vRestoreFontContext(void);
void fn_80012E00_CalcGradientScale(UFontContext* pCtx);
void fn_80012E1C(LLFont* pFont, s32 v);
void fn_80012E24(LLFont* pFont, UFontContext* pCtx);
s32  fn_80012E4C(LLFont* pFont);

UFontState lbl_801A34C0;
UFontState* lbl_80280DE0 = &lbl_801A34C0;

// Empties every font's queue and both pools.
void FO_vClearPacketLists(void) {
    lbl_80280DE0->apQueue[0] = NULL;
    lbl_80280DE0->apQueue[1] = NULL;
    lbl_80280DE0->apQueue[2] = NULL;
    lbl_80280DE0->apQueue[3] = NULL;
    lbl_80280DE0->apQueue[4] = NULL;
    lbl_80280DE0->apQueue[5] = NULL;
    lbl_80280DE0->pQueueNext = lbl_80280DE0->pQueuePool;
    lbl_80280DE0->pStringNext = lbl_80280DE0->pStrings;
}

// The 'sfn ' stream handler: loads the font into the slot its id names (bit 0x80 is a flag).
void UFont_LoadFontFromStream(UStreamObject* pObject) {
    s32 bFlag;
    u32 uId;

    bFlag = 0;
    uId = pObject->uId;
    if (uId & 0x80) {
        bFlag = 1;
    }
    UFont_LoadFont(uId & ~0x80, pObject->pData, bFlag);
    fn_80009E70(pObject);
}

void FO_vInitModule(void) {
    Stream_RegisterLoadChunkCallback('sfn ', UFont_LoadFontFromStream);
    fn_80011034(lbl_80280DE0);
    lbl_80280DE0->pQueuePool = fn_80009B34(50 * sizeof(UFontContext), 2, 0x10, "UFont.c", 0x125);
    lbl_80280DE0->pStrings = fn_80009B34(0x1F4, 2, 0x10, "UFont.c", 0x12F);
    FO_vClearPacketLists();
    UFont_ResetContext();
    lbl_80280DE0->apFonts[0] = NULL;
    lbl_80280DE0->apFonts[1] = NULL;
    lbl_80280DE0->apFonts[2] = NULL;
    lbl_80280DE0->apFonts[3] = NULL;
    lbl_80280DE0->apFonts[4] = NULL;
    lbl_80280DE0->apFonts[5] = NULL;
}

void FO_vCloseModule(void) {
    int i;

    for (i = 0; i < 6; i++) {
        if (lbl_80280DE0->apFonts[i] != NULL) {
            fn_800111A4(lbl_80280DE0->apFonts[i]);
        }
    }
    fn_80011160(lbl_80280DE0);
    fn_80009E70(lbl_80280DE0->pQueuePool);
    fn_80009E70(lbl_80280DE0->pStrings);
}

// Sets mode 0 (queued) and resets most text settings (u5C, a14, f70/f74, a8C and nC4 to fD0
// keep theirs).
void UFont_ResetContext(void) {
    UFontContext* pCtx;

    FO_vSetCurrentAddMode(0);
    pCtx = FO_spGetCurrentPacket();
    pCtx->f78 = 1.0f;
    pCtx->f7C = 1.0f;
    pCtx->f80 = 1.0f;
    pCtx->f84 = 1.0f;
    pCtx->f88 = 1.0f;
    pCtx->nFont = 0;
    pCtx->nA4 = 11;
    pCtx->uA8 = 0;
    pCtx->n9C = 0;
    pCtx->n60 = 0;
    pCtx->n64 = 0;
    pCtx->n68 = 1;
    pCtx->n6C = 1;
    pCtx->nAC = 0;
    pCtx->fB0 = 1.0f;
    pCtx->fB4 = 1.0f;
    pCtx->fB8 = 0.0f;
    pCtx->fBC = 0.5f;
    pCtx->fC0 = 0.5f;
    pCtx->n10 = 0;
    pCtx->f04 = 0.0f;
    pCtx->f08 = 1.0f;
    fn_80012E00_CalcGradientScale(pCtx);
}

// bFlag is read from the stream id but not used.
void UFont_LoadFont(u32 uSlot, void* pData, s32 bFlag) {
    lbl_80280DE0->apFonts[uSlot] = FO_spLoadFontFromStream(pData, lbl_80280DE0);
}

void UFont_FreeFont(int nSlot) {
    fn_800111A4(lbl_80280DE0->apFonts[nSlot]);
    lbl_80280DE0->apFonts[nSlot] = NULL;
}

void UFont_SetFont(s32 nFont) {
    UFontContext* pCtx;

    pCtx = FO_spGetCurrentPacket();
    pCtx->nFont = nFont;
}

void FO_vSetCurrentAddMode(s32 nMode) {
    lbl_80280DE0->n1B8 = nMode;
}

s32 FO_eGetCurrentAddMode(void) {
    return lbl_80280DE0->n1B8;
}

// Copies a string into the text pool and returns the copy.
char* FO_chpCopyString(char* sz) {
    char* szCopy;
    char* pDst;

    szCopy = lbl_80280DE0->pStringNext;
    pDst = szCopy;
    while (*sz != '\0') {
        *pDst++ = *sz++;
    }
    *pDst++ = '\0';
    lbl_80280DE0->pStringNext = pDst;
    return szCopy;
}

// Draws a string with the current settings: at once in mode 1, queued in mode 0.
void UFont_DrawString(char* sz, f32 fX, f32 fY) {
    s32 nOld;
    UFontContext* pCtx;
    UFontContext* pRec;
    LLFont* pFont;

    switch (lbl_80280DE0->n1B8) {
    case 1:
        pCtx = FO_spGetCurrentPacket();
        pFont = lbl_80280DE0->apFonts[pCtx->nFont];
        pCtx->f70 = fX;
        pCtx->f74 = fY;
        nOld = fn_80012E4C(pFont);
        fn_80012E24(pFont, pCtx);
        FO_vSetFontContext();
        fn_80011310(pFont, lbl_80280DE0);
        fn_8001144C(pFont, pCtx, sz);
        fn_80011C8C(pFont);
        fn_80012E1C(pFont, nOld);
        FO_vRestoreFontContext();
        break;
    case 0:
        pCtx = FO_spGetCurrentPacket();
        pRec = lbl_80280DE0->pQueueNext++;
        Mem_cpy(pRec, pCtx, sizeof(UFontContext));
        pRec->pNext = lbl_80280DE0->apQueue[pCtx->nFont];
        lbl_80280DE0->apQueue[pCtx->nFont] = pRec;
        pRec->szText = FO_chpCopyString(sz);
        pRec->f70 = fX;
        pRec->f74 = fY;
        fn_80012E00_CalcGradientScale(pRec);
        fn_80012E24(lbl_80280DE0->apFonts[pCtx->nFont], pCtx);
        break;
    }
}

// Draws every queued string, font by font, then empties the queues.
void UFont_DrawQueue(void) {
    int i;
    UFontContext* pRec;
    LLFont* pFont;

    FO_vSetFontContext();
    for (i = 0; i < 6; i++) {
        pFont = lbl_80280DE0->apFonts[i];
        if (pFont != NULL) {
            pRec = lbl_80280DE0->apQueue[i];
            if (pRec != NULL) {
                fn_80011310(pFont, lbl_80280DE0);
                do {
                    fn_8001144C(pFont, pRec, NULL);
                    pRec = pRec->pNext;
                } while (pRec != NULL);
                fn_80011C8C(pFont);
            }
            fn_80012438(pFont);
        }
    }
    FO_vClearPacketLists();
    FO_vRestoreFontContext();
}

void fn_80012B2C(f32 x0, f32 x1) {
    UFontContext* pCtx;

    pCtx = FO_spGetCurrentPacket();
    pCtx->f84 = x0;
    pCtx->f88 = x1;
}

void fn_80012B6C(f32 x0) {
    UFontContext* pCtx;

    pCtx = FO_spGetCurrentPacket();
    pCtx->f78 = x0;
}

void fn_80012B9C(f32 x0, f32 x1) {
    UFontContext* pCtx;

    pCtx = FO_spGetCurrentPacket();
    pCtx->f7C = x0;
    pCtx->f80 = x1;
}

// A string's width in font slot nFont (which becomes the current font).
f32 UFont_GetStringWidth(char* sz, int nFont) {
    UFontContext* pCtx;

    UFont_SetFont(nFont);
    pCtx = FO_spGetCurrentPacket();
    return fn_80011C90(lbl_80280DE0->apFonts[pCtx->nFont], pCtx, sz);
}

f32 fn_80012C30(char* sz) {
    return UFont_GetStringWidth(sz, 4);
}

void fn_80012C54_SetWordWrap(s32 v) {
    UFontContext* pCtx;

    pCtx = FO_spGetCurrentPacket();
    pCtx->uA8 = v;
}

void fn_80012C84_SetFlags(s32 v) {
    UFontContext* pCtx;

    pCtx = FO_spGetCurrentPacket();
    pCtx->n9C = v;
}

void fn_80012CB4_SetWordWrapBox(f32 f0, f32 f1, f32 f2, f32 f3, s32 n) {
    UFontContext* pCtx;

    pCtx = FO_spGetCurrentPacket();
    pCtx->a8C[0] = f0;
    pCtx->a8C[1] = f1;
    pCtx->a8C[2] = f2;
    pCtx->a8C[3] = f3;
}

// The first free font slot, or -1.
s32 UFont_FindFreeSlot(void) {
    int i;

    for (i = 0; i < 6; i++) {
        if (lbl_80280DE0->apFonts[i] == NULL) {
            return i;
        }
    }
    return -1;
}

void FO_vSetFontContext(void) {
    fn_800111D8();
    fn_80012F50(1, 6, 1);
    fn_80012F34(0);
    fn_80012F18(7);
    fn_80012EF8();
}

void FO_vRestoreFontContext(void) {
    fn_800112DC();
    fn_80012F34(1);
    fn_80012F18(3);
    fn_80012EF8();
}

void fn_80012E00_CalcGradientScale(UFontContext* pCtx) {
    pCtx->f0C = 1.0f / (pCtx->f08 - pCtx->f04);
}

void fn_80012E1C(LLFont* pFont, s32 v) {
    pFont->n474 = v;
}

void fn_80012E24(LLFont* pFont, UFontContext* pCtx) {
    if (pCtx->n10 != 0 || 0.0f != pCtx->fB8) {
        pFont->n474 = 1;
    }
}

s32 fn_80012E4C(LLFont* pFont) {
    return pFont->n474;
}

// A colour of four 0-1 floats as four bytes.
void UFont_PackColor(const f32* pColor, u8* pOut) {
    pOut[0] = (int)(255.0f * pColor[0]);
    pOut[1] = (int)(255.0f * pColor[1]);
    pOut[2] = (int)(255.0f * pColor[2]);
    pOut[3] = (int)(255.0f * pColor[3]);
}

UFontContext* FO_spGetCurrentPacket(void) {
    return &lbl_80280DE0->ctx;
}
