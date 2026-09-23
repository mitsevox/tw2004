// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_80281EE0;

void fn_8008EA38(u8 v);
void fn_8008EA38(u8 v) {
    *(u8*)(lbl_80281EE0 + 0x81) = v;
}
