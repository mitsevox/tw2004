// DepthField.c (TW06's golf/sfx/depthfield.c, its DF_ functions; our spelling): the depth-of-field
// blur. It copies the screen into a half-size texture and draws it back over the scene
// (DF_vDrawBufferToScreen). Its state is lbl_801D5110; not decompiled yet beyond the functions below.

#include "engine.h"
#include "gx.h"

void DF_vDrawBufferToScreen(int n);
void fn_800458B4(void);

void fn_80045880(int n) {
    fn_800458B4();
    DF_vDrawBufferToScreen(n);
}

// Copies the 512 x 448 screen into the 256 x 224 screen-copy texture.
void fn_800458B4(void) {
    GXSetTexCopySrc(0, 0, 512, 448);
    GXSetTexCopyDst(256, 224, 4, 1);
    GXCopyTex(fn_8002A624(), 0);
    GXInvalidateTexAll();
}
