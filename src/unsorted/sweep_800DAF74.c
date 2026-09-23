// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80202898[];

u8* fn_800DAF74(void);
u8* fn_800DAF74(void) {
    *(u8*)(lbl_80202898 + 0x10) = 0;
    *(u8*)(lbl_80202898 + 0x11) = 0;
    *(u8*)lbl_80202898 = 0;
    *(u8*)(lbl_80202898 + 0x9) = 0;
    *(s32*)(lbl_80202898 + 0x28) = 0;
    return lbl_80202898;
}
