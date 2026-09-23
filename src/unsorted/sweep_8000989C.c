// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80005AE8(s32, s32, s32);
extern s32 lbl_80281BBC;
extern s32 lbl_80281BC0;
extern s32 lbl_80281BCC;
extern s32 lbl_80281BD0;
extern void* lbl_80281BD4;

void fn_8000989C(void);
void fn_8000989C(void) {
    s32 temp_r31;

    temp_r31 = lbl_80281BBC;
    fn_80005AE8(lbl_80281BC0, 0x77, temp_r31);
    (*(s32*)((u8*)(lbl_80281BD4) + 0)) = (s32) lbl_80281BC0;
    (*(s32*)((u8*)(lbl_80281BD4) + 4)) = (s32) lbl_80281BC0;
    (*(s32*)((u8*)(lbl_80281BD4) + 8)) = (s32) (lbl_80281BC0 + temp_r31);
    (*(s32*)((u8*)(lbl_80281BD4) + 0xC)) = (s32) (*(s32*)((u8*)(lbl_80281BD4) + 8));
    lbl_80281BD0 = 3;
    lbl_80281BCC = 1;
}