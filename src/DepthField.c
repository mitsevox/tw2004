#include "game_types.h"

void GXCopyTex();
void GXInvalidateTexAll();
void GXSetTexCopyDst();
void GXSetTexCopySrc();
s32 fn_8002A624();
void DF_vDrawBufferToScreen();
void fn_80045880(s32 p0);
void fn_800458B4(void);

void fn_80045880(s32 p0) {
    fn_800458B4();
    DF_vDrawBufferToScreen(p0);
}

void fn_800458B4(void) {
    s32 t2;
    GXSetTexCopySrc(0, 0, 512, 448);
    GXSetTexCopyDst(256, 224, 4, 1);
    t2 = fn_8002A624();
    GXCopyTex(t2, 0);
    GXInvalidateTexAll();
}
