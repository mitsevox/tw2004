// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_80282460;
void fn_80103B28();
void fn_80104F68();
void fn_80104F7C();

void fn_80103B4C(void);
void fn_80103B74(u8 v);
u8 fn_80103B80(void);
void fn_80103B4C(void) {
    fn_80104F7C();
    fn_80103B28();
    fn_80104F68();
}

void fn_80103B74(u8 v) {
    *(u8*)(lbl_80282460 + 0x14) = v;
}

u8 fn_80103B80(void) {
    return *(u8*)(lbl_80282460 + 0x14);
}
