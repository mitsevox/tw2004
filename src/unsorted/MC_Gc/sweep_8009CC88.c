// Small functions found by the sweep (sweep.py). Original file: MC_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree; placed by the file map).

#include "game_types.h"

s32 fn_80009E70();
s32 fn_800A1BE0();
s32 fn_800B6594();
extern u32 lbl_80281FB8;
extern u32 lbl_80281FBC;
extern u32 lbl_80281FC4;
extern u32 lbl_80281FE4;
extern u32 lbl_80281FE8;
extern u32 lbl_80281FEC;

void fn_8009CC88(void);
void fn_8009CC88(void) {
    if ((u32) lbl_80281FB8 != 0U) {
        fn_80009E70(lbl_80281FB8);
        lbl_80281FB8 = 0U;
    }
    if ((u32) lbl_80281FBC != 0U) {
        fn_80009E70(lbl_80281FBC);
        lbl_80281FBC = 0U;
    }
    fn_800A1BE0();
    if ((u32) lbl_80281FE8 != 0U) {
        fn_80009E70(lbl_80281FE8);
        lbl_80281FEC = 0U;
        lbl_80281FE4 = 0U;
        lbl_80281FE8 = 0U;
    }
    if ((u32) lbl_80281FC4 != 0U) {
        fn_800B6594(lbl_80281FC4);
        lbl_80281FC4 = 0U;
    }
}