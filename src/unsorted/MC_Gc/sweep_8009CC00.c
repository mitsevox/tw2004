// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u32 fn_80009B34(s32, s32, s32, s32*, s32);
s32 fn_8009EF98();
u32 fn_800B6564();
extern s32 lbl_802813DC;
extern s32 lbl_80281FC0;
extern u32 lbl_80281FC4;
extern u32 lbl_80281FD8;
extern u32 lbl_80281FDC;
extern u32 lbl_80281FE0;
extern u32 lbl_80281FE4;
extern u32 lbl_80281FE8;
extern u32 lbl_80281FEC;

void fn_8009CC00(void);
void fn_8009CC00(void) {
    u32 temp_r3;
    u32 temp_r3_2;

    lbl_80281FC0 = 0x50020;
    if ((u32) lbl_80281FC4 == 0U) {
        lbl_80281FC4 = fn_800B6564();
    }
    temp_r3 = fn_80009B34(0x50000, 1, 0x20, &lbl_802813DC, 0x81);
    lbl_80281FE8 = temp_r3;
    lbl_80281FE4 = temp_r3;
    lbl_80281FEC = temp_r3;
    temp_r3_2 = fn_80009B34(0x50000, 1, 0x20, &lbl_802813DC, 0x85);
    lbl_80281FDC = temp_r3_2;
    lbl_80281FD8 = temp_r3_2;
    lbl_80281FE0 = temp_r3_2;
    fn_8009EF98();
}