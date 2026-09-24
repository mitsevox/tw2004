// trax.h (our name): Trax.c's data, the EA Trax music display.

#ifndef TRAX_H
#define TRAX_H

#include "engine.h"

#define TRAX_NUM_TRACKS 19

// A song of the 'TRAX' stream object (0x180 bytes; FE_MessageTable.c reads sz0 and szSong too).
typedef struct TraxTrack {
    char sz0[0x80];             // 0x000  shown first
    char szSong[0x80];          // 0x080  shown in quotes
    char sz100[0x80];           // 0x100
} TraxTrack;
LAYOUT_ASSERT(TraxTrack, 0x180);

// The display of the song playing (0x14 bytes).
typedef struct TraxState {
    u8   bShow;                 // 0x00
    s32  nFrames;               // 0x04  frames shown; it goes at 240
    s8   nTrack;                // 0x08
    s32  nFont;                 // 0x0C  2 in game type 10, else 1
    s32  nLogo;                 // 0x10  the 'TRXT' texture's slot (-1: none)
} TraxState;
LAYOUT_ASSERT(TraxState, 0x14);

extern TraxState lbl_801F8458;
extern TraxTrack lbl_801F846C[TRAX_NUM_TRACKS];

#endif
