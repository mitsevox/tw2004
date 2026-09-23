// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_800350B4(void*, f32, f32);
s32 fn_800350D0(void*, f32);
s32 fn_800350EC(s32, s32, s32);
extern void* lbl_802811E0;
extern f32 lbl_80282FB0;
extern f32 lbl_80282FB4;
extern f32 lbl_80282FB8;

void fn_80035398(void);
void fn_80035398(void) {
    fn_800350EC((s32) (*(f32*)((u8*)(lbl_802811E0) + 0x44)), (s32) (*(f32*)((u8*)(lbl_802811E0) + 0x48)), (s32) (*(f32*)((u8*)(lbl_802811E0) + 0x4C)));
    fn_800350D0(lbl_802811E0, lbl_80282FB0 * (*(f32*)((u8*)(lbl_802811E0) + 0x50)));
    fn_800350B4(lbl_802811E0, lbl_80282FB4 * (lbl_80282FB8 + (*(f32*)((u8*)(lbl_802811E0) + 0x50))), lbl_80282FB4);
}