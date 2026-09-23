// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_80281EE0;

void fn_8008E718(s32 v);
void fn_8008E718(s32 v) {
    *(s32*)(lbl_80281EE0 + 0x1BC) = v;
}
