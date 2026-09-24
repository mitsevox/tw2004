// UFont.c (EA's name, from its asserts; also in EA's 2002 source tree): the text layer over
// LLFont.c. It keeps six font slots and the current text settings (UFontContext). In mode 0 a
// drawn string is queued with a copy of the settings and drawn in fn_80012A78; in mode 1 it is
// drawn at once.

#include "engine.h"

void fn_8001256C(void);
void fn_800125BC(UStreamObject* pObject);
void fn_8001273C(void);
void fn_800127D8(u32 uSlot, void* pData, s32 bFlag);
void fn_80012868(s32 nFont);
void fn_80012898(s32 nMode);
char* fn_800128B0(char* sz);
f32  fn_80012BDC(char* sz, int nFont);
void fn_80012D88(void);
void fn_80012DCC(void);
void fn_80012E00(UFontContext* pCtx);
void fn_80012E1C(LLFont* pFont, s32 v);
void fn_80012E24(LLFont* pFont, UFontContext* pCtx);
s32  fn_80012E4C(LLFont* pFont);

// Empties every font's queue and both pools.
void fn_8001256C(void) {
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
void fn_800125BC(UStreamObject* pObject) {
    s32 bFlag;
    u32 uId;

    bFlag = 0;
    uId = pObject->uId;
    if (uId & 0x80) {
        bFlag = 1;
    }
    fn_800127D8(uId & ~0x80, pObject->pData, bFlag);
    fn_80009E70(pObject);
}

void fn_8001260C(void) {
    UStream_RegisterHandler('sfn ', fn_800125BC);
    fn_80011034(lbl_80280DE0);
    lbl_80280DE0->pQueuePool = fn_80009B34(50 * sizeof(UFontContext), 2, 0x10, "UFont.c", 0x125);
    lbl_80280DE0->pStrings = fn_80009B34(0x1F4, 2, 0x10, "UFont.c", 0x12F);
    fn_8001256C();
    fn_8001273C();
    lbl_80280DE0->apFonts[0] = NULL;
    lbl_80280DE0->apFonts[1] = NULL;
    lbl_80280DE0->apFonts[2] = NULL;
    lbl_80280DE0->apFonts[3] = NULL;
    lbl_80280DE0->apFonts[4] = NULL;
    lbl_80280DE0->apFonts[5] = NULL;
}

void fn_800126C0(void) {
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

// Resets the text settings.
void fn_8001273C(void) {
    UFontContext* pCtx;

    fn_80012898(0);
    pCtx = fn_80012EC4();
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
    fn_80012E00(pCtx);
}

// bFlag is read from the stream id but not used.
void fn_800127D8(u32 uSlot, void* pData, s32 bFlag) {
    lbl_80280DE0->apFonts[uSlot] = FO_spLoadFontFromStream(pData, lbl_80280DE0);
}

void fn_80012820(int nSlot) {
    fn_800111A4(lbl_80280DE0->apFonts[nSlot]);
    lbl_80280DE0->apFonts[nSlot] = NULL;
}

void fn_80012868(s32 nFont) {
    UFontContext* pCtx;

    pCtx = fn_80012EC4();
    pCtx->nFont = nFont;
}

void fn_80012898(s32 nMode) {
    lbl_80280DE0->n1B8 = nMode;
}

s32 fn_800128A4(void) {
    return lbl_80280DE0->n1B8;
}

// Copies a string into the text pool and returns the copy.
char* fn_800128B0(char* sz) {
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
void fn_800128F8(char* sz, f32 fX, f32 fY) {
    UFontContext* pCtx;
    UFontContext* pRec;
    LLFont* pFont;
    s32 nOld;

    switch (lbl_80280DE0->n1B8) {
    case 1:
        pCtx = fn_80012EC4();
        pFont = lbl_80280DE0->apFonts[pCtx->nFont];
        pCtx->f70 = fX;
        pCtx->f74 = fY;
        nOld = fn_80012E4C(pFont);
        fn_80012E24(pFont, pCtx);
        fn_80012D88();
        fn_80011310(pFont, lbl_80280DE0);
        fn_8001144C(pFont, pCtx, sz);
        fn_80011C8C(pFont);
        fn_80012E1C(pFont, nOld);
        fn_80012DCC();
        break;
    case 0:
        pCtx = fn_80012EC4();
        pRec = lbl_80280DE0->pQueueNext;
        lbl_80280DE0->pQueueNext = pRec + 1;
        Mem_cpy(pRec, pCtx, sizeof(UFontContext));
        pRec->pNext = lbl_80280DE0->apQueue[pCtx->nFont];
        lbl_80280DE0->apQueue[pCtx->nFont] = pRec;
        pRec->szText = fn_800128B0(sz);
        pRec->f70 = fX;
        pRec->f74 = fY;
        fn_80012E00(pRec);
        fn_80012E24(lbl_80280DE0->apFonts[pCtx->nFont], pCtx);
        break;
    }
}

// Draws every queued string, font by font, then empties the queues.
void fn_80012A78(void) {
    int i;
    UFontContext* pRec;
    LLFont* pFont;

    fn_80012D88();
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
    fn_8001256C();
    fn_80012DCC();
}

void fn_80012B2C(f32 x0, f32 x1) {
    UFontContext* pCtx;

    pCtx = fn_80012EC4();
    pCtx->f84 = x0;
    pCtx->f88 = x1;
}

void fn_80012B6C(f32 x0) {
    UFontContext* pCtx;

    pCtx = fn_80012EC4();
    pCtx->f78 = x0;
}

void fn_80012B9C(f32 x0, f32 x1) {
    UFontContext* pCtx;

    pCtx = fn_80012EC4();
    pCtx->f7C = x0;
    pCtx->f80 = x1;
}

// A string's width in font slot nFont (which becomes the current font).
f32 fn_80012BDC(char* sz, int nFont) {
    UFontContext* pCtx;

    fn_80012868(nFont);
    pCtx = fn_80012EC4();
    return fn_80011C90(lbl_80280DE0->apFonts[pCtx->nFont], pCtx, sz);
}

f32 fn_80012C30(char* sz) {
    return fn_80012BDC(sz, 4);
}

void fn_80012C54(s32 v) {
    UFontContext* pCtx;

    pCtx = fn_80012EC4();
    pCtx->uA8 = v;
}

void fn_80012C84(s32 v) {
    UFontContext* pCtx;

    pCtx = fn_80012EC4();
    pCtx->n9C = v;
}

void fn_80012CB4(f32 f0, f32 f1, f32 f2, f32 f3, s32 n) {
    UFontContext* pCtx;

    pCtx = fn_80012EC4();
    pCtx->a8C[0] = f0;
    pCtx->a8C[1] = f1;
    pCtx->a8C[2] = f2;
    pCtx->a8C[3] = f3;
}

// The first free font slot, or -1.
s32 fn_80012D04(void) {
    int i;

    for (i = 0; i < 6; i++) {
        if (lbl_80280DE0->apFonts[i] == NULL) {
            return i;
        }
    }
    return -1;
}

void fn_80012D88(void) {
    fn_800111D8();
    fn_80012F50(1, 6, 1);
    fn_80012F34(0);
    fn_80012F18(7);
    fn_80012EF8();
}

void fn_80012DCC(void) {
    fn_800112DC();
    fn_80012F34(1);
    fn_80012F18(3);
    fn_80012EF8();
}

void fn_80012E00(UFontContext* pCtx) {
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
void fn_80012E54(f32* pColor, u8* pOut) {
    f32 fR;
    f32 fG;
    f32 fB;
    f32 fA;

    fR = 255.0f * pColor[0];
    fG = 255.0f * pColor[1];
    fB = 255.0f * pColor[2];
    fA = 255.0f * pColor[3];
    pOut[0] = fR;
    pOut[1] = fG;
    pOut[2] = fB;
    pOut[3] = fA;
}

UFontContext* fn_80012EC4(void) {
    return &lbl_80280DE0->ctx;
}
