// LLPictInt.c: decodes a "MADk" picture file (one key frame of EA's MAD codec) into a new picture
// (LLPict, llpict.h), 16x16 pixels at a time, for LLPict_Gc.c's still pictures
// (Pict_CreateFromMemory); and the module's start and stop hooks, which have nothing to do.

#include "llpict.h"

// The module's start hook: Pict_InitModule calls it at boot. Nothing to set up.
void PictInt_InitModule(void) {
}

// The module's stop hook: Pict_CloseModule calls it at shutdown. Nothing to free.
void PictInt_CloseModule(void) {
}

// Decodes the 'MADk' key-frame picture in pFile into a new picture (three planes, Y then U and V,
// decoded 16x16 pixels at a time), or NULL when it is not a MADk file or the LLPict cannot be
// allocated (the pixels' allocation is not checked). The file's header is little-endian: its rate,
// width and height are swapped for the decode and swapped back after.
LLPict* PictInt_Decode(PictFile* pFile) {
    LLPict* pPict;
    u8* pY;
    u8* pU;
    u8* pV;
    int nXUV;
    int nY;
    int nX;

    if (pFile->uMagic != 'MADk') {
        return NULL;
    }
    pFile->uRate = ((pFile->uRate & 0xFF000000) >> 24) | ((pFile->uRate & 0xFF0000) >> 8) |
                   ((pFile->uRate & 0xFF00) << 8) | ((pFile->uRate & 0xFF) << 24);
    pFile->nWidth = ((u16)pFile->nWidth >> 8) | (((u16)pFile->nWidth & 0xFF) << 8);
    pFile->nHeight = (((u16)pFile->nHeight >> 8) & 0xFF) | (((u16)pFile->nHeight & 0xFF) << 8);
    pPict = Circular_MemAllocFunc(sizeof(LLPict), MEMTYPE_HIGH, 32, "LLPictInt.c", 142);
    if (pPict == NULL) {
        return NULL;
    }
    MAD_initdecode(pFile->aData, 0, pFile->nQuality);
    pPict->nWidth = pFile->nWidth;
    pPict->nHeight = pFile->nHeight;
    pPict->fMaxU = 1.0f;
    pPict->fMaxV = 1.0f;
    pPict->pPixels = Circular_MemAllocFunc(pPict->nWidth * pPict->nHeight * 3 / 2, MEMTYPE_HIGH, 32,
                                           "LLPictInt.c", 150);
    pY = Pict_GetPlaneY(pPict);
    pU = Pict_GetPlaneU(pPict);
    pV = Pict_GetPlaneV(pPict);
    for (nY = 0; nY < pFile->nHeight; nY += 16) {
        // a block is 16x16 Y pixels and 8x8 U and V ones; a key frame has no reference blocks
        for (nX = 0, nXUV = 0; nX < pFile->nWidth; nXUV += 8, nX += 16) {
            MAD_decodemacroblock(NULL, NULL, NULL, &pY[nX + nY * pFile->nWidth],
                                 &pU[nXUV + nY * pFile->nWidth / 4],
                                 &pV[nXUV + nY * pFile->nWidth / 4], pFile->nWidth);
        }
    }
    pFile->uRate = ((pFile->uRate & 0xFF000000) >> 24) | ((pFile->uRate & 0xFF0000) >> 8) |
                   ((pFile->uRate & 0xFF00) << 8) | ((pFile->uRate & 0xFF) << 24);
    pFile->nWidth = ((u16)pFile->nWidth >> 8) | (((u16)pFile->nWidth & 0xFF) << 8);
    pFile->nHeight = (((u16)pFile->nHeight >> 8) & 0xFF) | (((u16)pFile->nHeight & 0xFF) << 8);
    return pPict;
}
