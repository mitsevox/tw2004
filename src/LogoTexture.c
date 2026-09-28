// LogoTexture.c (our name): the tail of FE_LogoDesign.c (the logo editor), a unit of its own in
// splits.txt; it is exact and can fold back into FE_LogoDesign.c. Its code starts where
// FE_LogoDesign.c's ends (0x8010FC3C), it has no data of its own, FE_LogoDesign.c calls it twice,
// and the only other caller is the function right after it (fn_8010FF5C, unsorted/sweep_8010FF5C.c:
// a user logo laid out as a texture in lbl_80212B60, for char_tex_manager.c). TW07's FE_LogoDesign.c
// also ends, after FE_LogoDesign_GetPixelColor, with helpers that work on a logo's pixels
// (FE_LogoDesign_FixLogoEdge, FE_LogoDesign_FixEdgeInTempData).

#include "engine.h"
#include "gx.h"
#include "core/startup.h"
#include "frontend/fe.h"

// Copy a logo between its plain layout (rows of nWidth colour indexes) and the texture layout
// (tiles of 8 x 4 pixels, 32 bytes each, a row of tiles after another): bToTexture 1 from the
// logo in pSrc to the texture in pDst, 0 the other way. Then flush pDst for the GPU.
// One loop with the direction test inside: the compiler unswitches it into the two unrolled copies.
void FE_LogoDesign_CopyLogoTexturePixels(u8* pDst, u8* pSrc, int bToTexture, int nWidth, int nHeight) {
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
