// breakline.h (our name): GoBreakLine.c's state, the putt's break line (lbl_80282228, 0xAB40
// bytes, allocated by fn_800C808C). Only the fields read so far.

#ifndef BREAKLINE_H
#define BREAKLINE_H

#include "engine.h"

typedef struct BreakLine {
    u8   aMesh[2][0x28];        // 0x0000  a mesh object per view (Skin.c's fn_80036054 sets it up,
                                //         fn_800360A0 frees it); the second only in split screen
    u8   unk50[0x54B0 - 0x50];
    f32  vPin[4];               // 0x54B0  the pin, from the course's pin set (fn_800C8134)
    u8   unk54C0[0xA921 - 0x54C0];
    u8   abSkip[2];             // 0xA921  per view: set by fn_800C8134; BreakLine_Update clears it
                                //         instead of stepping the line once
    u8   unkA923[0xA928 - 0xA923];
    s32  anA928[2];             // 0xA928  per view: cleared by fn_800C8134
    u8   unkA930[0xAAB4 - 0xA930];
    f32  aViewPoint[2][4];      // 0xAAB4  a point per view (fn_800C8C3C hands it out)
    u8   unkAAD4[0xAAE0 - 0xAAD4];
    f32  fAAE0;                 // 0xAAE0  } set by fn_800C8134
    f32  fAAE4;                 // 0xAAE4  }
    u8   unkAAE8[0xAAEC - 0xAAE8];
    s32  nAAEC;                 // 0xAAEC  }
    u8   unkAAF0[0xAB04 - 0xAAF0];
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
