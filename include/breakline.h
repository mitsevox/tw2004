// breakline.h (our name): GoBreakLine.c's state, the putt's break line (lbl_80282228, 0xAB40
// bytes, allocated by BreakLine_InitModule). Only the fields read so far.

#ifndef BREAKLINE_H
#define BREAKLINE_H

#include "engine.h"
#include "ball.h"

#define BREAKLINE_POINTS 450    // points along the line per view, two vertices each
#define BREAKLINE_VERTS  (BREAKLINE_POINTS * 2)

typedef struct BreakLine {
    u8   aMesh[2][0x28];        // 0x0000  a mesh object per view (Skin.c's fn_80036054 sets it up,
                                //         fn_800360A0 frees it); the second only in split screen
    f32  aVert[2][BREAKLINE_VERTS][3];      // 0x0050  per view: the line's vertices, a pair per point
    f32  vPin[3];               // 0x54B0  the pin, from the course's pin set (fn_800C8134)
    u8   aColor[2][BREAKLINE_VERTS][4];     // 0x54BC  per view: the vertices' colours (anColor)
    f32  aUV[2][BREAKLINE_VERTS][2];        // 0x70DC  per view: the vertices' texture coordinates
    u8   abA91C[2];             // 0xA91C  per view: set by BreakLine_Start
    u8   unkA91E[0xA921 - 0xA91E];
    u8   abSkip[2];             // 0xA921  per view: set by fn_800C8134; BreakLine_Update clears it
                                //         instead of stepping the line once
    u8   unkA923[0xA928 - 0xA923];
    s32  anVerts[2];            // 0xA928  per view: the vertices laid so far (BreakLine_Step)
    u8   unkA930[0xA93C - 0xA930];
    Ball aBall[2];              // 0xA93C  per view: a copy of the player's ball, rolled by the line
    f32  aViewPoint[2][4];      // 0xAAB4  per view: where the ball started (fn_800C8C3C hands it out)
    f32  afAAD4[2];             // 0xAAD4  per view: 1000000 at the start
    u8   abAADC[2];             // 0xAADC  per view: cleared at the start
    u8   unkAADE[0xAAE0 - 0xAADE];
    f32  fAAE0;                 // 0xAAE0  } set by fn_800C8134
    f32  fAAE4;                 // 0xAAE4  }
    u8   unkAAE8[0xAAEC - 0xAAE8];
    s32  nAAEC;                 // 0xAAEC  } the texture repeats every nAAEC points
    s32  anAAF0[2];             // 0xAAF0  per view: cleared at the start
    u8   unkAAF8[0xAB04 - 0xAAF8];
    TexBank*  pBank;            // 0xAB04  the "brkline" texture's bank
    TexEntry* pTex;             // 0xAB08  and the texture
    f32  fAB0C;                 // 0xAB0C  } set by fn_800C8134
    f32  fAB10;                 // 0xAB10  }
    f32  fAB14;                 // 0xAB14  }
    f32  fAB18;                 // 0xAB18  }
    f32  fAB1C;                 // 0xAB1C  }
    s32  anColor[4];            // 0xAB20  red, green, blue, alpha (0x80, 0x80, 0x80, 0x33)
    f32  fAB30;                 // 0xAB30  } set when the state is allocated
    f32  fAB34;                 // 0xAB34  }
    f32  fAB38;                 // 0xAB38  }
    f32  fAB3C;                 // 0xAB3C  }
} BreakLine;
LAYOUT_ASSERT(BreakLine, 0xAB40);

extern BreakLine* lbl_80282228;
extern u8 lbl_8028222C;         // the line is on

void BreakLine_Step(int nView);
void fn_800C8C3C(int nView, f32* pOut);
void fn_800C9038(int nView, f32* pLong, f32* pSide);

#endif
