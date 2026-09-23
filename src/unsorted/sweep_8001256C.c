// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80280DE0;

void fn_8001256C(void);
void fn_8001256C(void) {
    *(s32*)(((u8*)lbl_80280DE0) + 0xB8) = 0;
    *(s32*)(((u8*)lbl_80280DE0) + 0xBC) = 0;
    *(s32*)(((u8*)lbl_80280DE0) + 0xC0) = 0;
    *(s32*)(((u8*)lbl_80280DE0) + 0xC4) = 0;
    *(s32*)(((u8*)lbl_80280DE0) + 0xC8) = 0;
    *(s32*)(((u8*)lbl_80280DE0) + 0xCC) = 0;
    *(s32*)(((u8*)lbl_80280DE0) + 0xD4) = *(s32*)(((u8*)lbl_80280DE0) + 0xD0);
    *(s32*)(((u8*)lbl_80280DE0) + 0x1C0) = *(s32*)(((u8*)lbl_80280DE0) + 0x1BC);
}
