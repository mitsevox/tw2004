// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"
#include "lldyntex.h"

void fn_8000FBAC();
void fn_8010BC64(u8* p);

void fn_8010BC64(u8* p) {
    fn_8000FBAC(*(s32*)(p + 0x4));
}

void fn_8010BC88(s32 n) {
    lbl_80282488->n8 = n;
}
