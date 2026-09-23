#include "game_types.h"
#include "gx.h"

void fn_8002A020(void);
extern u8 lbl_80281D28;
void fn_80012EF8();
void fn_80012F50();
void fn_8002A2FC(void);
u8 fn_8002A3A4(void);
extern u8* lbl_80281100;
s32 fn_8002A624(void);

void fn_8002A020(void) {
}

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

u8 fn_8002A3A4(void) {
    return lbl_80281D28;
}

s32 fn_8002A624(void) {
    return *(s32*)(lbl_80281100 + 0x0);
}
