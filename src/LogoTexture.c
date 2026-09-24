// LogoTexture.c (our name): the tail of FE_LogoDesign.c (the logo editor), kept as its own unit
// until its one function is exact. FE_LogoDesign.c is linked and calls it, and the function after
// it (sweep_8010FF5C) copies into FE_LogoDesign.c's lbl_80212B60; it folds back into
// FE_LogoDesign.c once exact.

#include "engine.h"
#include "gx.h"
#include "core/startup.h"
#include "frontend/fe.h"

// Copy a logo between its plain layout (rows of nWidth colour indexes) and the texture layout
// (tiles of 8 x 4 pixels, 32 bytes each, a row of tiles after another): bToTexture 1 from the
// logo in pSrc to the texture in pDst, 0 the other way. Then flush pDst for the GPU.
// Not exact yet (78%): the same instructions, but nBase takes a volatile register where EA's
// has a saved one, which reshuffles the unrolled loops' registers and schedule.
void fn_8010FC3C(u8* pDst, u8* pSrc, int bToTexture, int nWidth, int nHeight) {
    int nTileRow;
    int i;
    int nRow;
    int y;
    int x;
    int nBase;
    int n;
    int nPos;

    nTileRow = (nWidth == 64) ? 64 * 4 : 128 * 4;
    nRow = 0;
    for (y = 0; y < nHeight; y++) {
        nBase = (y % 4) * 8 + (y / 4) * nTileRow;
        i = nRow;
        x = 0;
        if (bToTexture) {
            n = nWidth;
            while (n-- > 0) {
                nPos = nBase + x % 8;
                nPos += (x / 8) * 32;
                pDst[nPos] = pSrc[i];
                i++;
                x++;
            }
        } else {
            n = nWidth;
            while (n-- > 0) {
                nPos = nBase + x % 8;
                nPos += (x / 8) * 32;
                pDst[i] = pSrc[nPos];
                i++;
                x++;
            }
        }
        nRow += nWidth;
    }
    DCFlushRange(pDst, 64 * 64);
    GXInvalidateTexAll();
}
