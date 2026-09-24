// GoViewport.c (EA's name, from its asserts; also in EA's 2002 source tree): a viewport, 13
// floats (0x34 bytes): its position and size on screen, a scale, and values derived from them.

#include "camera.h"

void  fn_80076A54(f32* pRect);
void  fn_80076AC4(void);
void  fn_80076AC8(void);
void  fn_80076B38(f32* pRect);

// A viewport's derived values, from its size ([2], [3]) and scale ([4], [5]): the scaled size,
// the inverse scale, the size over the scale, and the height over the width.
void fn_80076A54(f32* pRect) {
    pRect[6] = pRect[2] * pRect[4];
    pRect[7] = pRect[3] * pRect[5];
    pRect[10] = 1.0f / pRect[4];
    pRect[11] = 1.0f / pRect[5];
    pRect[8] = pRect[2] * pRect[10];
    pRect[9] = pRect[3] * pRect[11];
    pRect[12] = pRect[3] / pRect[2];
}

void fn_80076AC4(void) {
}

void fn_80076AC8(void) {
}

// A new viewport, set to the default.
f32* fn_80076ACC(void) {
    f32* pRect = fn_80009B34(13 * sizeof(f32), 2, 16, "GoViewport.c", 137);

    fn_80076B38(pRect);
    return pRect;
}

void fn_80076B18(f32* pRect) {
    fn_80009E70(pRect);
}

// The default viewport: at 0,0, size 1 by 1, scale 1 and 0.75.
void fn_80076B38(f32* pRect) {
    pRect[0] = 0.0f;
    pRect[1] = 0.0f;
    pRect[2] = 1.0f;
    pRect[3] = 1.0f;
    pRect[4] = 1.0f;
    pRect[5] = 0.75f;
    fn_80076A54(pRect);
}
