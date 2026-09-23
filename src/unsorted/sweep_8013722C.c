// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80282708;
extern s32 lbl_8028270C;
extern s32 lbl_80282710;
extern s32 lbl_80282714;
extern s32 lbl_80282718;
extern s32 lbl_8028271C;
extern s32 lbl_80282720;
extern s32 lbl_80282724;
extern s32 lbl_80282728;

void fn_8013722C(void* arg0);
void fn_8013722C(void* arg0) {
    lbl_80282708 += (*(s16*)((u8*)(arg0) + 0x52));
    lbl_80282714 += (*(s16*)((u8*)(arg0) + 0x54));
    lbl_80282720 += (*(s16*)((u8*)(arg0) + 0x56));
    lbl_8028270C += (*(s16*)((u8*)(arg0) + 0x58));
    lbl_80282718 += (*(s16*)((u8*)(arg0) + 0x5A));
    lbl_80282724 += (*(s16*)((u8*)(arg0) + 0x5C));
    lbl_80282710 += (*(s16*)((u8*)(arg0) + 0x5E));
    lbl_8028271C += (*(s16*)((u8*)(arg0) + 0x60));
    lbl_80282728 += (*(s16*)((u8*)(arg0) + 0x62));
}