// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80281D28;
void GXSetAlphaUpdate();
void GXSetTevAlphaIn();
void GXSetTevAlphaOp();
void GXSetTevColorIn();
void GXSetTevColorOp();
void fn_80012EF8();
void fn_80012F50();

void fn_8002A2FC(void);
void fn_8002A2FC(void) {
    GXSetAlphaUpdate(0);
    fn_80012F50(1, 6, 128);
    fn_80012EF8();
    lbl_80281D28 = 0;
    GXSetTevColorIn(1, 15, 0, 12, 15);
    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(1, 7, 4, 5, 7);
    GXSetTevAlphaOp(1, 0, 0, 1, 1, 0);
}
