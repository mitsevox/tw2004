// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_802824D0;
extern u8 lbl_802824D1;

u8 fn_80110450(void);
void fn_80110458(u8 v);
u8 fn_80110460(void);
u8 fn_80110450(void) {
    return lbl_802824D0;
}

void fn_80110458(u8 v) {
    lbl_802824D1 = v;
}

u8 fn_80110460(void) {
    return lbl_802824D1;
}
