// Small functions found by the sweep (sweep.py). Original file: PsBallFx.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

extern s32 lbl_80281408;
void fn_80009E70();
void fn_800360A0();

void fn_800A2E14(void);
void fn_800A2E14(void) {
    fn_800360A0(lbl_80281408);
    fn_80009E70(*(s32*)(((u8*)lbl_80281408) + 0x28));
    fn_80009E70(*(s32*)(((u8*)lbl_80281408) + 0x2C));
    fn_80009E70(*(s32*)(((u8*)lbl_80281408) + 0x30));
    fn_80009E70(*(s32*)(((u8*)lbl_80281408) + 0x50));
}
