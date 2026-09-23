// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_80280DC8;

void fn_8001618C(u8 v);
void fn_8001618C(u8 v) {
    *(u8*)(lbl_80280DC8 + 0x0) = v;
}
