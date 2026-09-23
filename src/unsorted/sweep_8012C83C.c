// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_802825B0;

u8 fn_8012C83C(void);
u8 fn_8012C848(void);
u8 fn_8012C83C(void) {
    return *(u8*)(lbl_802825B0 + 0x91);
}

u8 fn_8012C848(void) {
    return *(u8*)(lbl_802825B0 + 0x92);
}
