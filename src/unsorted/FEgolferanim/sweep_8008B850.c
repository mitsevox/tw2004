// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801D8708[];

u8* fn_8008B850(void);
u8* fn_8008B850(void) {
    *(u8*)(lbl_801D8708 + 0x8) = 1;
    return lbl_801D8708;
}
