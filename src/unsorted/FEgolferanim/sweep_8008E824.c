// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_80281EE0;
void fn_8008DD50();

void fn_8008E824(void);
void fn_8008E824(void) {
    fn_8008DD50(0);
    *(s32*)(((u8*)lbl_80281EE0) + 0x50) = 0;
    *(s32*)(((u8*)lbl_80281EE0) + 0x1C0) = 4;
}
