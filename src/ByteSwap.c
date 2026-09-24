// ByteSwap.c (our name): copying data out of a loaded file while swapping its byte order, for
// the files that are little-endian on disc (the golfer table, the animation libraries and clip
// banks, the skeletons, the Create-A-Player sliders). Also an empty init / term pair the game's
// start-up and shutdown lists call. It sits between LLVideo.c and GoCamera.c; nothing ties it to
// either.

#include "game_types.h"
#include "endian.h"

void fn_800763B4(void);
void fn_800763B8(void);

// Copy nBytes from *ppSrc to pDst as values nWidth bytes wide (2, 4 or 8), reversing each value's
// bytes, and move *ppSrc past them. A negative nWidth copies the values without swapping; a width
// of 1, or any other, copies bytes.
void BYTESWAP_SWAPDATA(u8** ppSrc, u8* pDst, int nBytes, int nWidth) {
    int bNoSwap = 0;
    int nCount;
    u8 b0;
    u8 b1;
    u8 b2;
    u8 b3;

    if (nWidth < 0) {
        bNoSwap = 1;
        nWidth = -nWidth;
    }
    nCount = nBytes / nWidth;
    switch (nWidth) {
    case 2:
        if (bNoSwap) {
            do {
                pDst[0] = (*ppSrc)[0];
                pDst[1] = (*ppSrc)[1];
                pDst += 2;
                *ppSrc += 2;
            } while (--nCount > 0);
        } else {
            do {
                b0 = (*ppSrc)[0];
                pDst[0] = (*ppSrc)[1];
                pDst[1] = b0;
                pDst += 2;
                *ppSrc += 2;
            } while (--nCount > 0);
        }
        break;
    case 4:
        if (bNoSwap) {
            do {
                pDst[0] = (*ppSrc)[0];
                pDst[1] = (*ppSrc)[1];
                pDst[2] = (*ppSrc)[2];
                pDst[3] = (*ppSrc)[3];
                pDst += 4;
                *ppSrc += 4;
            } while (--nCount > 0);
        } else {
            do {
                b0 = (*ppSrc)[0];
                b1 = (*ppSrc)[1];
                pDst[0] = (*ppSrc)[3];
                pDst[1] = (*ppSrc)[2];
                pDst[2] = b1;
                pDst[3] = b0;
                pDst += 4;
                *ppSrc += 4;
            } while (--nCount > 0);
        }
        break;
    case 8:
        if (bNoSwap) {
            do {
                pDst[0] = (*ppSrc)[0];
                pDst[1] = (*ppSrc)[1];
                pDst[2] = (*ppSrc)[2];
                pDst[3] = (*ppSrc)[3];
                pDst[4] = (*ppSrc)[4];
                pDst[5] = (*ppSrc)[5];
                pDst[6] = (*ppSrc)[6];
                pDst[7] = (*ppSrc)[7];
                pDst += 8;
                *ppSrc += 8;
            } while (--nCount > 0);
        } else {
            do {
                b0 = (*ppSrc)[0];
                b1 = (*ppSrc)[1];
                b2 = (*ppSrc)[2];
                b3 = (*ppSrc)[3];
                pDst[0] = (*ppSrc)[7];
                pDst[1] = (*ppSrc)[6];
                pDst[2] = (*ppSrc)[5];
                pDst[3] = (*ppSrc)[4];
                pDst[4] = b3;
                pDst[5] = b2;
                pDst[6] = b1;
                pDst[7] = b0;
                pDst += 8;
                *ppSrc += 8;
            } while (--nCount > 0);
        }
        break;
    case 1:
    default:
        do {
            pDst[0] = (*ppSrc)[0];
            pDst += 1;
            *ppSrc += 1;
        } while (--nCount > 0);
        break;
    }
}

void fn_800763B4(void) {
}

void fn_800763B8(void) {
}
