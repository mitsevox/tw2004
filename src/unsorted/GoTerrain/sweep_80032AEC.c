// Small functions found by the sweep (sweep.py). Original file: GoTerrain.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

extern u8 lbl_801D3CB0[];
void fn_80012EF8();
void fn_80012F50();
void fn_8003084C();
void fn_80030894();
void fn_8003272C();
void fn_80032F88();
void fn_80035098();

void fn_80032AEC(void);
void fn_80032AEC(void) {
    fn_80030894();
    fn_80032F88(*(s32*)(lbl_801D3CB0 + 0x30), *(s32*)(lbl_801D3CB0 + 0x10E4), *(s32*)(lbl_801D3CB0 + 0x11B8), *(s32*)(lbl_801D3CB0 + 0x11BC));
    fn_8003272C(0);
    fn_80032F88(*(s32*)(lbl_801D3CB0 + 0x24), *(s32*)(lbl_801D3CB0 + 0x10DC), *(s32*)(lbl_801D3CB0 + 0x11B8), *(s32*)(lbl_801D3CB0 + 0x11BC));
    fn_8003272C(1);
    fn_80035098(0);
    fn_8003084C();
    fn_80012F50(1, 6, 128);
    fn_80012EF8();
}
