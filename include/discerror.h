#ifndef DISCERROR_H
#define DISCERROR_H

// DiscError.c (our name): the disc-error screen, drawn straight into the frame buffer with its own
// 8x8 font while the drive reports a problem.

#include "game_types.h"

// A font glyph's columns (DiscError.c lbl_801F66A8, one per character 0x20..0x8A, computed from
// the bitmaps by fn_800B694C).
typedef struct DiscGlyph {
    u8   nLeft;                 // 0x00  first column drawn
    u8   nRight;                // 0x01  last column drawn
    u8   nWidth;                // 0x02  how far the pen moves
    s8   nDrop;                 // 0x03  rows the glyph sits lower (2 for , ; g j p q y)
} DiscGlyph;
LAYOUT_ASSERT(DiscGlyph, 0x4);

// A frame-buffer colour (DiscError.c lbl_80190D40): luma and the two chroma values; a pixel pair
// is written as Y|Cb then Y|Cr.
typedef struct DiscColor {
    u32  uY;                    // 0x00
    u32  uCb;                   // 0x04
    u32  uCr;                   // 0x08
} DiscColor;
LAYOUT_ASSERT(DiscColor, 0xC);

extern u32        lbl_8018FFE0[107 * 8];   // the font: 8 rows of 8 nibbles for each character 0x20..0x8A
extern DiscColor  lbl_80190D40[9];         // the colours (7 black, 8 grey)
extern DiscGlyph  lbl_801F66A8[107];       // each character's columns, from fn_800B694C
extern const char** lbl_802814D8;          // the screen's messages in the current language
extern const char lbl_802814DC[6];         // "%s %d"
extern u8         lbl_802814D0;            // fill the unset pixels with the background colour
extern u8         lbl_802814D1;            // fixed width: every character 8 pixels
extern u8         lbl_802814D2;            // positions in 8-pixel cells
extern s32        lbl_80282188;            // frame-buffer height
extern s32        lbl_8028218C;            // frame-buffer width, in pixels
extern u16        lbl_80282190;            // the text colour (lbl_80190D40 index)
extern u16*       lbl_80282194;            // the frame buffer
extern u32        lbl_80282198;            // its size in bytes
extern u8         lbl_802821A0;            // the cover was just closed: keep the screen blanked a while
extern u32        lbl_802821A4;            // when (OSGetTick)

void fn_800B694C(s32 nWidth, s32 nHeight, u32 uSize);  // set the frame buffer's size, measure the font
void fn_800B6C0C(void* pFrameBuffer);                  // set the frame buffer to draw into

#endif
