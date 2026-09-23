// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern s32 lbl_802821FC;
extern u8 lbl_80282200;

void fn_800BB0A8(void);
void fn_800BB0C8(void);
u8 fn_800BB0D4(void);
void fn_800BB0DC(void);
void fn_800BB0A8(void) {
    fn_800BB0DC();
}

void fn_800BB0C8(void) {
    lbl_802821FC = 0;
}

u8 fn_800BB0D4(void) {
    return lbl_80282200;
}

void fn_800BB0DC(void) {
    lbl_80282200 = 0;
}
