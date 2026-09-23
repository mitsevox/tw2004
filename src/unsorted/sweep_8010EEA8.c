// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80223C48[];
void fn_80077808();
void fn_800EFAD0();
void fn_80117188();

void fn_8010EEA8(void);
void fn_8010EEA8(void) {
    *(u8*)(lbl_80223C48 + 0x20) = 0;
    fn_800EFAD0(lbl_80223C48);
    fn_80117188();
    fn_80077808(0);
}
