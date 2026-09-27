// LogoTexture.c (our name): the tail of FE_LogoDesign.c (the logo editor), kept as its own unit
// until its one function is exact. FE_LogoDesign.c is linked and calls it, and the function after
// it (fn_8010FF5C) copies into FE_LogoDesign.c's lbl_80212B60; it folds back into
// FE_LogoDesign.c once exact.

#include "engine.h"
#include "gx.h"
#include "core/startup.h"
#include "frontend/fe.h"

// Copy a logo between its plain layout (rows of nWidth colour indexes) and the texture layout
// (tiles of 8 x 4 pixels, 32 bytes each, a row of tiles after another): bToTexture 1 from the
// logo in pSrc to the texture in pDst, 0 the other way. Then flush pDst for the GPU.
// One loop with the direction test inside: the compiler unswitches it into the two unrolled copies.
void fn_8010FC3C(u8* pDst, u8* pSrc, int bToTexture, int nWidth, int nHeight) {
    int nTileRow;
    int i;
    int nRow;
    int y;
    int x;
    int nPos;

    nTileRow = (nWidth == 64) ? 64 * 4 : 128 * 4;
    for (y = 0, nRow = 0; y < nHeight; y++, nRow += nWidth) {
        i = nRow;
        for (x = 0; x < nWidth; x++) {
            nPos = (x / 8) * 32 + ((y % 4) * 8 + (y / 4) * nTileRow) + x % 8;
            if (bToTexture) {
                pDst[nPos] = pSrc[i];
            } else {
                pDst[i] = pSrc[nPos];
            }
            i++;
        }
    }
    DCFlushRange(pDst, 64 * 64);
    GXInvalidateTexAll();
}
