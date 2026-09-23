// Small functions found by the sweep (sweep.py). Original file: LLDisp_Gc.c (EA's name, from its asserts; placed by the file map).

#include "game_types.h"

extern u8 lbl_801A2350[];

u8* fn_80006EC8(void);
u8* fn_80006EC8(void) {
    *(u8*)(lbl_801A2350 + 0x13) = 1;
    return lbl_801A2350;
}
