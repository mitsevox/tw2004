// DiscError.c (our name): the disc-error screens ("The Disc Cover is open.", "Please insert the
// ... Game Disc", "An error has occurred.") shown from fn_800B7490 when the drive reports a
// problem. They are drawn straight into the frame buffer (YCbCr pixels, every row doubled) with
// the game's own 8x8 font, one nibble per pixel.

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "platform.h"
#include "game.h"
#include "discerror.h"
#include "core/startup.h"

void fn_8001437C(void);
void fn_800A3F38(u8 b, u8 b2);
void VIWaitForRetrace(void);
void fn_800B6924(void);
void fn_800B6C14(s16 nX, s16 nY, const char* pFmt, ...);
void fn_800B6CD0(u16 nColor);
void fn_800B6CD8(int nX, int nY, const char* szText);
int  fn_800B6DA4(const char* szText);
void fn_800B6E1C(int nX, int nY, int nRow, DiscGlyph* pGlyph);
void fn_800B6FCC(int nLines);
void fn_800B70E8(int nLine, int nLines, int nSpan, const char* szText);
void fn_800B7210(s32 nStatus);
void fn_800B7684(u8 b);
void fn_800B768C(u8 b);
void fn_800B7694(u8 b);

// Write the drawn screen back from the CPU cache.
void fn_800B6924(void) {
    DCStoreRange(lbl_80282194, lbl_80282198);
}

// Set the frame buffer's size and measure every glyph of the font: its first and last lit
// columns, with one column of space each side. The characters with descenders sit two rows lower.
void fn_800B694C(s32 nWidth, s32 nHeight, u32 uSize) {
    int nLeft;
    int nRight;
    int nRow;
    int n;
    int i;
    u32 uBits;
    u32 u;
    int bDrop;

    lbl_8028218C = nWidth;
    lbl_80282188 = nHeight;
    lbl_80282198 = uSize;
    for (nRow = n = 0; n < 107; nRow += 8, n++) {
        uBits = 0;
        for (i = 0; i < 8; i++) {
            uBits |= lbl_8018FFE0[nRow + i];
        }
        if (uBits != 0) {
            u = uBits;
            for (i = 7; i >= 0; i--) {
                if (u & 0xF) {
                    nRight = i;
                    break;
                }
                u >>= 4;
            }
            u = uBits;
            for (i = 7; i >= 0; i--) {
                if (u & 0xF0000000) {
                    nLeft = 7 - i;
                    break;
                }
                u <<= 4;
            }
            if (nLeft > 0) {
                nLeft--;
            }
            if (nRight < 7) {
                nRight++;
            }
        } else {
            nLeft = 0;
            nRight = 3;
        }
        lbl_801F66A8[n].nLeft = nLeft;
        lbl_801F66A8[n].nRight = nRight;
        lbl_801F66A8[n].nWidth = nRight + 1 - nLeft;
        bDrop = 0;
        if (n == 'g' - 0x20 || n == ',' - 0x20 || n == ';' - 0x20 || n == 'j' - 0x20 ||
            n == 'p' - 0x20 || n == 'q' - 0x20 || n == 'y' - 0x20) {
            bDrop = 1;
        }
        lbl_801F66A8[n].nDrop = bDrop ? 2 : 0;
    }
}

void fn_800B6C0C(void* pFrameBuffer) {
    lbl_80282194 = pFrameBuffer;
}

// printf onto the screen at (nX, nY).
void fn_800B6C14(s16 nX, s16 nY, const char* pFmt, ...) {
    char szText[256];           // size not proven by the frame (0x100..0x110 bytes)
    va_list args;

    va_start(args, pFmt);
    vsprintf(szText, pFmt, args);
    va_end(args);
    fn_800B6CD8(nX, nY, szText);
}

void fn_800B6CD0(u16 nColor) {
    lbl_80282190 = nColor;
}

// Draw a string, characters outside 0x20..0x8A as spaces.
void fn_800B6CD8(int nX, int nY, const char* szText) {
    DiscGlyph* pGlyph;
    int n;
    u8 c;

    if (lbl_802814D2) {
        nX *= 8;
    }
    c = *szText;
    while (c != 0) {
        if (c < 0x20) {
            c = 0x20;
        } else if (c >= 0x8B) {
            c = 0x20;
        }
        n = c - 0x20;
        pGlyph = &lbl_801F66A8[n];
        if (lbl_802814D1) {
            pGlyph = NULL;
        }
        fn_800B6E1C(nX, nY, n * 8, pGlyph);
        if (lbl_802814D1) {
            nX += 8;
        } else {
            nX += pGlyph->nWidth;
        }
        c = *++szText;
    }
}

// A string's width in pixels.
int fn_800B6DA4(const char* szText) {
    int nWidth;
    int n;
    u8 c;

    nWidth = 0;
    while ((c = *szText++) != 0) {
        if (lbl_802814D1) {
            nWidth += 8;
        } else {
            if (c < 0x20) {
                c = 0x20;
            } else if (c >= 0x8B) {
                c = 0x20;
            }
            n = c - 0x20;
            nWidth += lbl_801F66A8[n].nWidth;
        }
    }
    return nWidth;
}

// Draw one glyph (its bitmap rows from lbl_8018FFE0[nRow]) at twice the height; without a
// DiscGlyph every column is drawn (fixed width).
void fn_800B6E1C(int nX, int nY, int nRow, DiscGlyph* pGlyph) {
    u32* pBits;
    u16* pPixel;
    u32 uBits;
    u16 uFore;
    u16 uBack;
    u16 uForeCb;
    u16 uBackCb;
    u16 uForeCr;
    u16 uBackCr;
    int nX2;
    int nY2;
    int nCol;
    int nLine;
    int bOdd;

    if (lbl_802814D2) {
        nY *= 8;
    }
    if (pGlyph != NULL) {
        nY += pGlyph->nDrop;
    }
    uBackCb = (lbl_80190D40[7].uY << 8) + lbl_80190D40[7].uCb;
    uForeCb = (lbl_80190D40[lbl_80282190].uY << 8) + lbl_80190D40[lbl_80282190].uCb;
    uForeCr = (lbl_80190D40[lbl_80282190].uY << 8) + lbl_80190D40[lbl_80282190].uCr;
    uBackCr = (lbl_80190D40[7].uY << 8) + lbl_80190D40[7].uCr;
    uFore = uForeCb;
    uBack = uBackCb;
    pBits = &lbl_8018FFE0[nRow];
    nY2 = nY * 2;
    bOdd = 0;
    for (nLine = 0; nLine < 8; nLine++) {
        nX2 = nX * 2;
        uBits = *pBits;
        for (nCol = 0; nCol < 8; nCol++) {
            if (pGlyph == NULL || (nCol >= pGlyph->nLeft && nCol <= pGlyph->nRight)) {
                if (uBits & 0xF0000000) {
                    pPixel = (u16*)((u8*)lbl_80282194 + nX2 + lbl_8028218C * nY2 * 2);
                    pPixel[0] = uFore;
                    pPixel[lbl_8028218C] = uFore;
                } else if (lbl_802814D0) {
                    pPixel = (u16*)((u8*)lbl_80282194 + nX2 + lbl_8028218C * nY2 * 2);
                    pPixel[0] = uBack;
                    pPixel[lbl_8028218C] = uBack;
                }
            }
            if (pGlyph == NULL || (nCol >= pGlyph->nLeft && nCol <= pGlyph->nRight)) {
                nX2 += 2;
            }
            bOdd = 1 - bOdd;
            uBits <<= 4;
            if (bOdd) {
                uBack = uBackCb;
                uFore = uForeCb;
            } else {
                uBack = uBackCr;
                uFore = uForeCr;
            }
        }
        pBits++;
        nY2 += 2;
    }
}

// The screen's frame: nLines rows of 40 cells, box-drawing characters 0x82..0x89, centred on the
// 28 rows.
void fn_800B6FCC(int nLines) {
    char szLine[0x29];
    int nY;
    int i;

    fn_800B7684(1);
    szLine[0x28] = 0;
    fn_800B6CD0(2);
    nY = (28 - nLines) / 2;
    for (i = 0; i < nLines; i++) {
        if (i == 0 || i == nLines - 1) {
            if (i == 0) {
                szLine[0] = 0x82;
                szLine[0x27] = 0x83;
                fn_80005AE8(&szLine[1], 0x86, sizeof(szLine) - 3);
            } else {
                szLine[0] = 0x84;
                szLine[0x27] = 0x85;
                fn_80005AE8(&szLine[1], 0x88, sizeof(szLine) - 3);
            }
        } else {
            fn_80005AE8(&szLine[1], ' ', sizeof(szLine) - 3);
            szLine[0] = 0x89;
            szLine[0x27] = 0x87;
        }
        fn_800B6C14(12, nY, szLine);
        nY++;
    }
}

// Print line nLine of nLines, centred across the screen and spread over the frame's inner nSpan
// rows.
void fn_800B70E8(int nLine, int nLines, int nSpan, const char* szText) {
    int nWidth;
    f32 fStep;

    fn_800B7694(0);
    fn_800B7684(0);
    fn_800B6CD0(8);
    fn_800B768C(0);
    nWidth = fn_800B6DA4(szText);
    fStep = ((nSpan - 1) * 8) / nLines;
    fn_800B6C14((0x200 - (nWidth + 3)) / 2,
                ((fStep - 8.0f) + (224.0f - nLines * fStep)) / 2.0f + (nLine - 1) * fStep, szText);
    fn_800B7694(1);
    fn_800B768C(1);
}

// Draw the message for the drive status nStatus.
void fn_800B7210(s32 nStatus) {
    char szText[64];

    VIWaitForRetrace();
    switch (nStatus) {
    case 5:  // DVD_STATE_COVER_OPEN
        fn_800B6FCC(6);
        fn_800B70E8(1, 3, 6, lbl_802814D8[0]);
        fn_800B70E8(2, 3, 6, lbl_802814D8[1]);
        fn_800B70E8(3, 3, 6, lbl_802814D8[2]);
        break;
    case 4:  // DVD_STATE_NO_DISK
        fn_800B6FCC(6);
        fn_800B70E8(1, 2, 6, lbl_802814D8[3]);
        if (fn_80110468() == 0) {
            fn_800B70E8(2, 2, 6, lbl_802814D8[4]);
        } else {
            fn_800B70E8(2, 2, 6, lbl_802814D8[14]);
        }
        break;
    case 6:  // DVD_STATE_WRONG_DISK
        fn_800B6FCC(6);
        fn_800B70E8(1, 2, 4, lbl_802814D8[3]);
        if (fn_80110468() == 0) {
            fn_800B70E8(2, 2, 4, lbl_802814D8[4]);
        } else {
            fn_800B70E8(2, 2, 4, lbl_802814D8[14]);
        }
        break;
    case 11:  // DVD_STATE_RETRY
        fn_800B6FCC(6);
        fn_800B70E8(1, 4, 6, lbl_802814D8[5]);
        fn_800B70E8(2, 4, 6, lbl_802814D8[6]);
        fn_800B70E8(3, 4, 6, lbl_802814D8[7]);
        fn_800B70E8(4, 4, 6, lbl_802814D8[8]);
        break;
    case -1:  // DVD_STATE_FATAL_ERROR
        fn_800B6FCC(6);
        fn_800B70E8(1, 4, 6, lbl_802814D8[9]);
        fn_800B70E8(2, 4, 6, lbl_802814D8[10]);
        fn_800B70E8(3, 4, 6, lbl_802814D8[7]);
        fn_800B70E8(4, 4, 6, lbl_802814D8[11]);
        break;
    default:
        sprintf(szText, lbl_802814DC, lbl_802814D8[12], nStatus);
        fn_800B6FCC(6);
        fn_800B70E8(1, 1, 6, szText);
        break;
    }
    fn_800B6924();
}

// Check the drive (and the controllers, for a reset); while it reports a problem, redraw the
// message every frame, with the audio paused unless the drive reports no disc, the wrong disc or
// a retry. Returns 1 when the screen was shown.
u8 fn_800B7490(void) {
    u8 bShown;
    s32 nStatus;
    s32 nLast;

    bShown = 0;
    nStatus = 0;
    // 8 seconds: the bus clock (at 0x800000F8) / 4 is the timer's rate
    if (lbl_802821A0 && OSGetTick() - lbl_802821A4 > *(u32*)0x800000F8 / 4 / 1000 * 8000) {
        lbl_802821A0 = 0;
    }
    for (;;) {
        fn_80013400();
        nLast = nStatus;
        nStatus = DVDGetDriveStatus();
        if ((fn_800136DC(0) & 0x01000000) || (fn_800136DC(1) & 0x01000000) ||
            (fn_800136DC(2) & 0x01000000) || (fn_800136DC(3) & 0x01000000) || nStatus == 6 ||
            nStatus == 10) {
            if (nStatus != -1) {
                fn_800066E4(1, 1, 1, 0);
            }
        } else if (lbl_802821A0) {
            if (nStatus != -1) {
                fn_800066E4(1, 1, 1, 0);
            }
        } else if (nStatus != -1) {
            fn_800066E4(1, 0, 1, 0);
        }
        if ((nLast == 5 && nStatus != 5) || nStatus == 6) {
            lbl_802821A4 = OSGetTick();
            lbl_802821A0 = 1;
        }
        if ((u32)nStatus <= 1 || nStatus == 7) {
            break;
        }
        if (bShown == 0) {
            if (nStatus != 4 && nStatus != 6 && nStatus != 11) {
                fn_800A3F38(1, 1);
            }
            fn_8001437C();
            bShown = 1;
        }
        fn_800B7210(nStatus);
    }
    if (bShown && gSession.nPaused == 0) {
        fn_800A3F38(0, 1);
    }
    return bShown;
}

void fn_800B7684(u8 b) {
    lbl_802814D0 = b;
}

void fn_800B768C(u8 b) {
    lbl_802814D1 = b;
}

void fn_800B7694(u8 b) {
    lbl_802814D2 = b;
}
