// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern void* lbl_80282220;

u8 fn_800C6D64(void);
u8 fn_800C6D64(void) {
    if ((void* ) lbl_80282220 == NULL) {
        return 0U;
    }
    return (*(u8*)((u8*)(lbl_80282220) + 0x58));
}