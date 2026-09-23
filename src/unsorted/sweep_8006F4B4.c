// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_802811E8;
void fn_8006F5FC();

void fn_8006F4B4(void);
void fn_8006F4B4(void) {
    fn_8006F5FC();
    *(u8*)(((u8*)lbl_802811E8) + 0x1) = 1;
}
