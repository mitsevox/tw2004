// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void GXCopyTex();
void GXInvalidateTexAll();
void GXPixModeSync();
void GXSetTexCopyDst();
void GXSetTexCopySrc();
s32 fn_8002A624();

void fn_8008CE2C(void);
void fn_8008CE2C(void) {
    s32 t3;
    GXPixModeSync();
    GXSetTexCopySrc(128, 0, 384, 448);
    GXSetTexCopyDst(384, 448, 6, 0);
    t3 = fn_8002A624();
    GXCopyTex(t3, 0);
    GXPixModeSync();
    GXInvalidateTexAll();
}
