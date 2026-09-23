// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_80281988;

void fn_80125910(u8 v);
u8 fn_8012591C(void);
void fn_80125910(u8 v) {
    *(u8*)(lbl_80281988 + 0xD) = v;
}

u8 fn_8012591C(void) {
    return *(u8*)(lbl_80281988 + 0xD);
}
