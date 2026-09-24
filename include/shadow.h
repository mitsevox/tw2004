// shadow.h (our name): shadow.c's state, the golfer's shadow (lbl_801F5E20, 0x500 bytes, reached
// through lbl_802814A8): the render camera the golfer's shadow is drawn with and the 256x256 palette
// texture it is drawn into. Only the fields read so far.

#ifndef SHADOW_H
#define SHADOW_H

#include "camera.h"
#include "gx.h"

typedef struct Shadow {
    u8          aLut[256][4];   // 0x000  the texture's palette: entry 0 clear, the rest the shadow's
                                //        intensity (fn_800B2360)
    void*       pImage;         // 0x400  the texture's pixels: the screen copy's buffer
    CamLens*    pLens;          // 0x404  } the render camera the shadow is drawn with,
    f32*        pRect;          // 0x408  }   made from these three
    GoFrameBuf* pFrameBuf;      // 0x40C  }
    void*       pCamera;        // 0x410  }
    f32         v414[4];        // 0x414  } set by fn_800B28D4 each time the shadow is drawn
    f32         f424;           // 0x424  }
    f32         f428;           // 0x428  }
    u8          unk42C[0x454 - 0x42C];
    s32         nWidth;         // 0x454  } the texture's size in pixels (256 x 256)
    s32         nHeight;        // 0x458  }
    GxTexture   tex;            // 0x45C
    u8          aMesh[2][0x28]; // 0x48C  a mesh object per view (Skin.c's fn_80036054 sets it up,
                                //        fn_800360A0 frees it)
    f32         f4DC;           // 0x4DC  } set up by SH_vSetShadowIntensity
    f32         f4E0;           // 0x4E0  }
    s32         n4E4;           // 0x4E4  }
    f32         f4E8;           // 0x4E8  }
    s32         n4EC;           // 0x4EC  }
    s32         n4F0;           // 0x4F0  cleared by fn_800B2724
    u8          unk4F4[0x500 - 0x4F4];
} Shadow;
LAYOUT_ASSERT(Shadow, 0x500);

extern Shadow* lbl_802814A8;
extern void* lbl_80282160;      // } buffers SH_vSetShadowIntensity allocates
extern void* lbl_80282164;      // }   (0x1000, 0x1000, 0x4000, 0x2000 and 0x6000 bytes)
extern void* lbl_80282168;      // }
extern void* lbl_8028216C;      // }
extern void* lbl_80282170;      // }

#endif
