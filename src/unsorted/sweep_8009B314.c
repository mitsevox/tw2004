// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_802813B8;
void fn_80009E70();

void fn_8009B314(u8 v);
void fn_8009B320(void);
void fn_8009B314(u8 v) {
    *(u8*)(lbl_802813B8 + 0x1BF0) = v;
}

void fn_8009B320(void) {
    fn_80009E70();
}
