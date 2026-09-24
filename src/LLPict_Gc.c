// LLPict_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled; the
// sweep code below is the matched small functions.

#include "llpict.h"
#include "core/startup.h"

void fn_8002F898(u8* pSrc, u8* pDst, int nWidth, int nHeight);   // copies one plane
void fn_8002FB98(LLPict* pPict);
void fn_8002F56C(u8* pPlane, void* pWork, int nWidth, int nHeight);  // reorders one plane through pWork
PictFrame* fn_800B94CC(void* pDecoder, int n);                  // the decoder's next frame, or NULL
void fn_800B9808(void* pDecoder, PictFrame* pFrame);            // gives a frame back

// ---- sweep code (not yet cleaned up) ----

extern u8 lbl_801876C8[];
void fn_80056204();
void fn_80056208();
void fn_8002F4FC(void);
void fn_8002F540(void);
void fn_800B90F4();
void fn_800B91B8();
s32 fn_800B920C();

void fn_8002F4FC(void) {
    void* t1;
    fn_80056204();
    t1 = fn_80009B34(2048, 2, 32, (const char*)lbl_801876C8, 68);
    *lbl_802810C0 = t1;
}

void fn_8002F540(void) {
    fn_80056208();
    fn_80009E70(*lbl_802810C0);
}

// ---- end of sweep code ----

// fn_8002F898 in place: each band of four rows is copied to pWork first and tiled back from there.
// The first 8 bytes are already where they belong.
void fn_8002F56C(u8* pPlane, void* pWork, int nWidth, int nHeight) {
    int nRow = nWidth * 4;      // a band of four rows
    int i;
    int y;
    int nOff;

    for (y = 0; y < nHeight; y += 4) {
        memcpy(pWork, pPlane, nRow);
        for (i = 1; i < nWidth / 2; i++) {
            nOff = (i / 4) * 2 + (nWidth / 4) * (i % 4);
            ((u32*)pPlane)[i * 2] = ((u32*)pWork)[nOff];
            ((u32*)pPlane)[i * 2 + 1] = ((u32*)pWork)[nOff + 1];
        }
        pPlane += nRow;
    }
}

// Copies a plane of nWidth x nHeight bytes into GameCube I8 tile order: tiles of 8 x 4 bytes, each
// row of a tile being 8 bytes of one source row.
void fn_8002F898(u8* pSrc, u8* pDst, int nWidth, int nHeight) {
    int i;
    int y;
    int nOff;

    for (y = 0; y < nHeight; y += 4) {
        for (i = 0; i < nWidth / 2; i++) {
            nOff = (i / 4) * 2 + (nWidth / 4) * (i % 4);
            ((u32*)pDst)[i * 2] = ((u32*)pSrc)[nOff];
            ((u32*)pDst)[i * 2 + 1] = ((u32*)pSrc)[nOff + 1];
        }
        pSrc += nWidth * 4;
        pDst += nWidth * 4;
    }
}

// Makes the picture's three planes into I8 textures (U and V at half the width and height).
void fn_8002FB98(LLPict* pPict) {
    GXInitTexObj(&pPict->aTex[0], fn_8003020C(pPict), pPict->nWidth, pPict->nHeight, 1, 0, 0, 0);
    GXInitTexObjLOD(&pPict->aTex[0], 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXInitTexObj(&pPict->aTex[1], fn_800301F4(pPict), pPict->nWidth / 2, pPict->nHeight / 2, 1, 0, 0, 0);
    GXInitTexObjLOD(&pPict->aTex[1], 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXInitTexObj(&pPict->aTex[2], fn_800301D0(pPict), pPict->nWidth / 2, pPict->nHeight / 2, 1, 0, 0, 0);
    GXInitTexObjLOD(&pPict->aTex[2], 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
}

// Decodes a "MADk" file into a new picture and makes its textures (NULL: no file, or it did not
// decode).
LLPict* fn_8002FD00(u8* pData, u32 uSize) {
    LLPict* pPict;

    if (pData == NULL) {
        return NULL;
    }
    pPict = PictInt_Decode((PictFile*)pData);
    if (pPict == NULL) {
        return NULL;
    }
    DCFlushRange(fn_8003020C(pPict), 1.5f * (pPict->nWidth * pPict->nHeight));
    fn_8002F56C(fn_8003020C(pPict), *lbl_802810C0, pPict->nWidth, pPict->nHeight);
    fn_8002F56C(fn_800301F4(pPict), *lbl_802810C0, pPict->nWidth / 2, pPict->nHeight / 2);
    fn_8002F56C(fn_800301D0(pPict), *lbl_802810C0, pPict->nWidth / 2, pPict->nHeight / 2);
    DCFlushRange(fn_8003020C(pPict), 1.5f * (pPict->nWidth * pPict->nHeight));
    fn_8002FB98(pPict);
    return pPict;
}

// ---- sweep code (not yet cleaned up) ----

void fn_8002FE70(LLPict* pPict) {
    if (pPict != NULL) {
        fn_80009E70(pPict->pPixels);
        fn_80009E70(pPict);
    }
}

void fn_8002FEAC(void) {
}

void fn_8002FEB0(LLPict* pPict, PictStream* pStream, void* (*pfnRead)(void* pArg), void* pArg) {
    pPict->pPixels = NULL;
    pStream->pDecoder = fn_80009B34(80, 1, 32, (const char*)lbl_801876C8, 278);
    pStream->pFrame = NULL;
    fn_800B90F4(pfnRead, pArg);
    fn_800B91B8(pStream->pDecoder);
}

// ---- end of sweep code ----

// Frees the picture's pixels and the stream's frame and decoder.
void fn_8002FF38(LLPict* pPict, PictStream* pStream) {
    if (pPict->pPixels != NULL) {
        fn_80009E70(pPict->pPixels);
    }
    if (pStream->pFrame != NULL) {
        fn_800B9808(pStream->pDecoder, pStream->pFrame);
    }
    fn_800B920C(pStream->pDecoder);
    fn_80009E70(pStream->pDecoder);
}

// ---- sweep code (not yet cleaned up) ----

void fn_8002FF94(LLPict* pPict, PictStream* pStream) {
}

// ---- end of sweep code ----

// Sizes the picture to the stream's frame and allocates its planes.
void fn_8002FF98(LLPict* pPict, PictStream* pStream) {
    pPict->nWidth = pStream->pFrame->nWidth;
    pPict->nHeight = pStream->pFrame->nHeight;
    pPict->pPixels =
        fn_80009B34(pPict->nWidth * pPict->nHeight * 3 / 2, 1, 32, (const char*)lbl_801876C8, 346);
    pPict->f6C = 1.0f;
    pPict->f70 = 1.0f;
}

// ---- sweep code (not yet cleaned up) ----

u8 fn_800B9930(u8* p0);

u8 fn_8003001C(LLPict* pPict, PictStream* pStream) {
    return fn_800B9930(pStream->pDecoder);
}

// ---- end of sweep code ----

// Takes the decoder's next frame, giving back the one held; 1 when there is one.
u8 fn_80030040(LLPict* pPict, PictStream* pStream) {
    if (pStream->pFrame != NULL) {
        fn_800B9808(pStream->pDecoder, pStream->pFrame);
    }
    pStream->pFrame = fn_800B94CC(pStream->pDecoder, 0);
    return pStream->pFrame != NULL;
}

// ---- sweep code (not yet cleaned up) ----

void fn_8003009C(LLPict* pPict, PictStream* pStream, int n2) {
}

// ---- end of sweep code ----

// Copies the stream's frame into the picture, plane by plane (U and V at half the width and
// height), and hands the picture on to fn_8002FB98.
void fn_800300A0(LLPict* pPict, PictStream* pStream) {
    DCFlushRange(fn_8003024C(pStream->pFrame), pPict->nWidth * pPict->nHeight * 3 / 2);
    fn_8002F898(fn_8003024C(pStream->pFrame), fn_8003020C(pPict), pPict->nWidth, pPict->nHeight);
    fn_8002F898(fn_80030234(pStream->pFrame), fn_800301F4(pPict), pPict->nWidth / 2, pPict->nHeight / 2);
    fn_8002F898(fn_80030214(pStream->pFrame), fn_800301D0(pPict), pPict->nWidth / 2, pPict->nHeight / 2);
    DCFlushRange(fn_8003020C(pPict), pPict->nWidth * pPict->nHeight * 3 / 2);
    fn_8002FB98(pPict);
}

u8* fn_800301D0(LLPict* pPict) {
    return pPict->pPixels + pPict->nWidth * pPict->nHeight * 5 / 4;
}

u8* fn_800301F4(LLPict* pPict) {
    return pPict->pPixels + pPict->nWidth * pPict->nHeight;
}

u8* fn_8003020C(LLPict* pPict) {
    return pPict->pPixels;
}

u8* fn_80030214(PictFrame* pFrame) {
    return pFrame->pPixels + ((u32)(pFrame->nWidth * pFrame->nHeight * 5) >> 2);
}

u8* fn_80030234(PictFrame* pFrame) {
    return pFrame->pPixels + pFrame->nWidth * pFrame->nHeight;
}

u8* fn_8003024C(PictFrame* pFrame) {
    return pFrame->pPixels;
}
