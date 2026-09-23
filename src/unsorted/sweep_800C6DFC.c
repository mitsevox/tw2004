// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* lbl_80282220;

void fn_800C6DFC(void);
void fn_800C6DFC(void) {
    if ((void* ) lbl_80282220 != NULL) {
        (*(s8*)((u8*)(lbl_80282220) + 0x58)) = 0;
    }
}