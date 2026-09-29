// comiccam.h (our name): GoComicCam.c's state, the comic-book camera that splits the screen into
// panels (lbl_80282178, 0x204 bytes, allocated by ComicCam_InitComicCam). Only the fields read so far.

#ifndef COMICCAM_H
#define COMICCAM_H

#include "camera.h"

// A panel on screen (0x28 bytes).
typedef struct ComicPanel {
    s32  n0;                    // 0x00
    f32  f4;                    // 0x04
    s32  nNext;                 // 0x08  the panel shown after this one, -1 for the last
    f32  fTop;                  // 0x0C  } where it sits on screen, in fractions of the screen
    f32  fLeft;                 // 0x10  }   (ComicCam_SetViewportToSize hands them to VM_vSetViewportRect as x, y, w, h)
    f32  fWidth;                // 0x14  }
    f32  fHeight;               // 0x18  }
    f32  f1C;                   // 0x1C
    f32  fTime;                 // 0x20  how long it stays (copied to ComicCam.a24/a4C)
    s32  n24;                   // 0x24
} ComicPanel;
LAYOUT_ASSERT(ComicPanel, 0x28);

typedef struct ComicCam {
    s32  nPanel;                // 0x000  the current panel
    s32  nPlayer;               // 0x004
    s32  n8;                    // 0x008  counts to 2: ComicCam_UpdateComicCam draws the whole screen until then
    f32  fTime;                 // 0x00C  time on the current panel
    s32  n10;                   // 0x010  frames into the move to the next panel
    u8   bNext;                 // 0x014  moving on to the next panel
    u8   bDone;                 // 0x015  the last panel has been reached (kind 0)
    u8   unk16[2];
    s32  nKind;                 // 0x018  0 or 1: which layout ComicCam_StartComicCam set up
    s32  nShown;                // 0x01C  panels moved on to so far
    s32  nPanels;               // 0x020
    f32  a24[10];               // 0x024  per panel: its time left (-1: not shown)
    f32  a4C[10];               // 0x04C  per panel: the time it started with
    ComicPanel aPanel[10];      // 0x074
} ComicCam;
LAYOUT_ASSERT(ComicCam, 0x204);

extern ComicCam* lbl_80282178;

void ComicCam_InitComicCam(void);                                         // allocate the state
void ComicCam_CloseComicCam(void);                                         // free it
void ComicCam_StartComicCam(int nKind, View* pView, int nPlayer);          // start with layout nKind
u8   ComicCam_UpdateComicCam(View* pView, int nPlayer, f32 fFrameTime);     // a frame; 1 when finished
u8   ComicCam_HasBallBeenHit(void);

#endif
