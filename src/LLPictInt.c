// LLPictInt.c (EA's name, from its asserts; also in TW06 and EA's 2002 source tree): decodes a
// "MADk" picture file (EA's MAD codec) into a new picture, 16x16 pixels at a time.

#include "llpict.h"

void fn_800B8528(u8* pData, int a, int n);
void fn_800B8618(int a, int b, int c, u8* pY, u8* pU, u8* pV);

// The picture in pFile, or NULL when it is not a MADk file or there is no memory. The file's
// header is little-endian: it is swapped for the decode and swapped back after.
// port: the swaps assume a big-endian machine; a little-endian port reads the header as it is.
LLPict* PictInt_Decode(PictFile* pFile) {
    LLPict* pPict;
    u8* pY;
    u8* pU;
    u8* pV;
    int xc;
    int y;
    int x;

    if (pFile->uMagic != 'MADk') {
        return NULL;
    }
    __stwbrx(pFile->uC, &pFile->uC, 0);
    pFile->nWidth = (pFile->nWidth << 8) | ((pFile->nWidth >> 8) & 0xFF);
    pFile->nHeight = (pFile->nHeight << 8) | ((pFile->nHeight >> 8) & 0xFF);
    pPict = fn_80009B34(sizeof(LLPict), 1, 32, "LLPictInt.c", 142);
    if (pPict == NULL) {
        return NULL;
    }
    fn_800B8528(pFile->aData, 0, pFile->n15);
    pPict->nWidth = pFile->nWidth;
    pPict->nHeight = pFile->nHeight;
    pPict->f6C = 1.0f;
    pPict->f70 = 1.0f;
    pPict->pPixels = fn_80009B34(pPict->nWidth * pPict->nHeight * 3 / 2, 1, 32, "LLPictInt.c", 150);
    pY = fn_8003020C(pPict);
    pU = fn_800301F4(pPict);
    pV = fn_800301D0(pPict);
    for (y = 0; y < pFile->nHeight; y += 16) {
        // a block is 16x16 Y pixels and 8x8 U and V ones
        for (x = 0, xc = 0; x < pFile->nWidth; xc += 8, x += 16) {
            fn_800B8618(0, 0, 0, &pY[x + y * pFile->nWidth], &pU[xc + y * pFile->nWidth / 4],
                        &pV[xc + y * pFile->nWidth / 4]);
        }
    }
    __stwbrx(pFile->uC, &pFile->uC, 0);
    pFile->nWidth = (pFile->nWidth << 8) | ((pFile->nWidth >> 8) & 0xFF);
    pFile->nHeight = (pFile->nHeight << 8) | ((pFile->nHeight >> 8) & 0xFF);
    return pPict;
}
