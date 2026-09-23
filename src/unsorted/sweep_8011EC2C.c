// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80282510;
void GXCopyTex();
void GXInvalidateTexAll();
void GXPixModeSync();
void GXSetTexCopyDst();
void GXSetTexCopySrc();

void fn_8011EC2C(void);
void fn_8011EC2C(void) {
    GXSetTexCopySrc(0, 0, 256, 256);
    GXSetTexCopyDst(256, 256, 4, 0);
    GXCopyTex(lbl_80282510, 0);
    GXPixModeSync();
    GXInvalidateTexAll();
}
