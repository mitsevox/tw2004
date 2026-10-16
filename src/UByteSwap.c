// UByteSwap.c: copying values out of a loaded file while reversing their byte order, for the files
// that are little-endian on disc: the animations, skins and skeletons, the camera and situation
// scripts, the golfer table, the Create-A-Player data and others.

#include "game_types.h"
#include "endian.h"

// Copy nBytes from *pSrc to pDest as values elmSize bytes wide (2, 4 or 8), reversing each value's
// bytes, and move *pSrc past them; pDest may be *pSrc, to swap in place. A negative elmSize copies
// the values without swapping. Any other width copies single bytes, nBytes / elmSize of them. At
// least one value is always copied, even when nBytes is less than elmSize.
void BYTESWAP_SWAPDATA(u8** pSrc, u8* pDest, int nBytes, int elmSize) {
    int noSwap = 0;
    int cnt;
    u8 c0;
    u8 c1;
    u8 c2;
    u8 c3;

    if (elmSize < 0) {
        noSwap = 1;
        elmSize = -elmSize;
    }
    cnt = nBytes / elmSize;
    switch (elmSize) {
    case 2:
        if (noSwap) {
            do {
                pDest[0] = (*pSrc)[0];
                pDest[1] = (*pSrc)[1];
                pDest += 2;
                *pSrc += 2;
            } while (--cnt > 0);
        } else {
            do {
                c0 = (*pSrc)[0];
                pDest[0] = (*pSrc)[1];
                pDest[1] = c0;
                pDest += 2;
                *pSrc += 2;
            } while (--cnt > 0);
        }
        break;
    case 4:
        if (noSwap) {
            do {
                pDest[0] = (*pSrc)[0];
                pDest[1] = (*pSrc)[1];
                pDest[2] = (*pSrc)[2];
                pDest[3] = (*pSrc)[3];
                pDest += 4;
                *pSrc += 4;
            } while (--cnt > 0);
        } else {
            do {
                c0 = (*pSrc)[0];
                c1 = (*pSrc)[1];
                pDest[0] = (*pSrc)[3];
                pDest[1] = (*pSrc)[2];
                pDest[2] = c1;
                pDest[3] = c0;
                pDest += 4;
                *pSrc += 4;
            } while (--cnt > 0);
        }
        break;
    case 8:
        if (noSwap) {
            do {
                pDest[0] = (*pSrc)[0];
                pDest[1] = (*pSrc)[1];
                pDest[2] = (*pSrc)[2];
                pDest[3] = (*pSrc)[3];
                pDest[4] = (*pSrc)[4];
                pDest[5] = (*pSrc)[5];
                pDest[6] = (*pSrc)[6];
                pDest[7] = (*pSrc)[7];
                pDest += 8;
                *pSrc += 8;
            } while (--cnt > 0);
        } else {
            do {
                c0 = (*pSrc)[0];
                c1 = (*pSrc)[1];
                c2 = (*pSrc)[2];
                c3 = (*pSrc)[3];
                pDest[0] = (*pSrc)[7];
                pDest[1] = (*pSrc)[6];
                pDest[2] = (*pSrc)[5];
                pDest[3] = (*pSrc)[4];
                pDest[4] = c3;
                pDest[5] = c2;
                pDest[6] = c1;
                pDest[7] = c0;
                pDest += 8;
                *pSrc += 8;
            } while (--cnt > 0);
        }
        break;
    case 1:
    default:
        do {
            pDest[0] = (*pSrc)[0];
            pDest += 1;
            *pSrc += 1;
        } while (--cnt > 0);
        break;
    }
}
