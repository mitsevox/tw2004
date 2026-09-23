// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void* fn_8000B70C(s32, s32);
u32 fn_80048808(s32);
extern u32 lbl_802821D8;
extern u32 lbl_802821DC;
extern u32 lbl_802821E0;

void fn_800B9B48(void);
void fn_800B9B48(void) {
    void* temp_r3;
    void* temp_r3_2;
    void* temp_r3_3;

    if ((u32) lbl_802821E0 == 0U) {
        temp_r3 = fn_8000B70C(0x54454F20, 0x2710);
        if (temp_r3 != NULL) {
            lbl_802821E0 = fn_80048808((*(s32*)((u8*)(temp_r3) + 4)));
        }
    }
    if ((u32) lbl_802821DC == 0U) {
        temp_r3_2 = fn_8000B70C(0x54454F20, 0x272E);
        if (temp_r3_2 != NULL) {
            lbl_802821DC = fn_80048808((*(s32*)((u8*)(temp_r3_2) + 4)));
        }
    }
    if ((u32) lbl_802821D8 == 0U) {
        temp_r3_3 = fn_8000B70C(0x54454F20, 0x2738);
        if (temp_r3_3 != NULL) {
            lbl_802821D8 = fn_80048808((*(s32*)((u8*)(temp_r3_3) + 4)));
        }
    }
}