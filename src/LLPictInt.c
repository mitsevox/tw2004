// LLPictInt.c (EA's name, from its asserts; also in TW06 and EA's 2002 source tree): decodes a
// "MADk" picture file (EA's MAD codec, maddec.c) into a new picture, 16x16 pixels at a
// time, for LLPict_Gc.c; and that file's module start and stop hooks, empty in this build.

#include "llpict.h"

// Empty in this build. Pict_InitModule calls it at boot.
void PictInt_InitModule(void) {
}

// Empty in this build. Pict_CloseModule calls it at shutdown.
void PictInt_CloseModule(void) {
}

// Decodes the 'MADk' key-frame picture in pFile into a new picture (three planes, Y then U and V,
// decoded 16x16 pixels at a time), or NULL when it is not a MADk file or the LLPict cannot be
// allocated (the pixels' allocation is not checked). The file's header is little-endian: it is
// swapped for the decode and swapped back after.
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
    // the 32-bit swaps are written in C, which the compiler turns into lwz + stwbrx after register
    // allocation: that gives EA's registers and order, __stwbrx() does not
    pFile->uC = ((pFile->uC & 0xFF000000) >> 24) | ((pFile->uC & 0xFF0000) >> 8) |
                ((pFile->uC & 0xFF00) << 8) | ((pFile->uC & 0xFF) << 24);
    pFile->nWidth = ((u16)pFile->nWidth >> 8) | (((u16)pFile->nWidth & 0xFF) << 8);
    // fake match: the height's swap spelled high byte first with u16 casts gives EA's clrlslwi and
    // rlwimi; the width's spelling does not
    pFile->nHeight = (((u16)pFile->nHeight >> 8) & 0xFF) | (((u16)pFile->nHeight & 0xFF) << 8);
    pPict =StaticMem_Alloc(sizeof(LLPict), 1, 32, "LLPictInt.c", 142);
    if (pPict == NULL) {
        return NULL;
    }
    MAD_initdecode(pFile->aData, 0, pFile->n15);
    pPict->nWidth = pFile->nWidth;
    pPict->nHeight = pFile->nHeight;
    pPict->fMaxU = 1.0f;
    pPict->fMaxV = 1.0f;
    pPict->pPixels = StaticMem_Alloc(pPict->nWidth * pPict->nHeight * 3 / 2, 1, 32, "LLPictInt.c", 150);
    pY = Pict_GetPlaneY(pPict);
    pU = Pict_GetPlaneU(pPict);
    pV = Pict_GetPlaneV(pPict);
    for (y = 0; y < pFile->nHeight; y += 16) {
        // a block is 16x16 Y pixels and 8x8 U and V ones
        for (x = 0, xc = 0; x < pFile->nWidth; xc += 8, x += 16) {
            MAD_decodemacroblock(NULL, NULL, NULL, &pY[x + y * pFile->nWidth],
                                 &pU[xc + y * pFile->nWidth / 4], &pV[xc + y * pFile->nWidth / 4],
                                 pFile->nWidth);
        }
    }
    pFile->uC = ((pFile->uC & 0xFF000000) >> 24) | ((pFile->uC & 0xFF0000) >> 8) |
                ((pFile->uC & 0xFF00) << 8) | ((pFile->uC & 0xFF) << 24);
    pFile->nWidth = ((u16)pFile->nWidth >> 8) | (((u16)pFile->nWidth & 0xFF) << 8);
    pFile->nHeight = (((u16)pFile->nHeight >> 8) & 0xFF) | (((u16)pFile->nHeight & 0xFF) << 8);
    return pPict;
}
