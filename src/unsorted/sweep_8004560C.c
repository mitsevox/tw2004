// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80202898[];

s32 fn_8004560C(void);
s32 fn_8004561C(void);
s32 fn_8004560C(void) {
    return *(u8*)(lbl_80202898 + 0x12);
}

s32 fn_8004561C(void) {
    return *(u8*)(lbl_80202898 + 0x10);
}
