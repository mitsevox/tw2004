// Small functions found by the sweep (sweep.py). Original file: GoTerrain.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

extern u8* lbl_80281380;
void fn_80035398();
void fn_8003541C();
void fn_80035440();

void fn_800352E4(void);
void fn_80035308(void);
s32 fn_8003532C(void);
void fn_800352E4(void) {
    fn_8003541C();
    fn_80035398();
}

void fn_80035308(void) {
    fn_8003532C();
    fn_80035440();
}

s32 fn_8003532C(void) {
    return *(s32*)(lbl_80281380 + 0x230);
}
