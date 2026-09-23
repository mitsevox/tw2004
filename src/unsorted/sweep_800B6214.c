// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* lbl_802814C8;

void fn_800B6214(void* arg0);
void fn_800B6214(void* arg0) {
    void* temp_r0;
    void* temp_r4;
    void* temp_r5;

    (*(s32*)((u8*)(arg0) + 0x28)) = 0;
    temp_r5 = (*(void**)((u8*)(arg0) + 0x3C));
    if (arg0 == temp_r5) {
        (*(void**)((u8*)(lbl_802814C8) + 0x10)) = NULL;
    } else {
        if (arg0 == (void* ) (*(void**)((u8*)(lbl_802814C8) + 0x10))) {
            (*(void**)((u8*)(lbl_802814C8) + 0x10)) = temp_r5;
        }
        (*(void**)((u8*)((*(void**)((u8*)(arg0) + 0x40))) + 0x3C)) = (void* ) (*(void**)((u8*)(arg0) + 0x3C));
        (*(void**)((u8*)((*(void**)((u8*)(arg0) + 0x3C))) + 0x40)) = (void* ) (*(void**)((u8*)(arg0) + 0x40));
        (*(void**)((u8*)(arg0) + 0x40)) = arg0;
        (*(void**)((u8*)(arg0) + 0x3C)) = arg0;
    }
    (*(s32*)((u8*)(lbl_802814C8) + 0x20)) = (s32) ((*(s32*)((u8*)(lbl_802814C8) + 0x20)) - 1);
    temp_r0 = (*(void**)((u8*)(lbl_802814C8) + 0x14));
    if (temp_r0 != NULL) {
        (*(void**)((u8*)(arg0) + 0x3C)) = temp_r0;
        (*(void**)((u8*)(arg0) + 0x40)) = (void* ) (*(void**)((u8*)((*(void**)((u8*)(lbl_802814C8) + 0x14))) + 0x40));
        temp_r4 = (*(void**)((u8*)((*(void**)((u8*)(lbl_802814C8) + 0x14))) + 0x40));
        if (temp_r4 != NULL) {
            (*(void**)((u8*)(temp_r4) + 0x3C)) = arg0;
        }
        (*(void**)((u8*)((*(void**)((u8*)(lbl_802814C8) + 0x14))) + 0x40)) = arg0;
    }
    (*(void**)((u8*)(lbl_802814C8) + 0x14)) = arg0;
    (*(s32*)((u8*)(lbl_802814C8) + 0x24)) = (s32) ((*(s32*)((u8*)(lbl_802814C8) + 0x24)) + 1);
}