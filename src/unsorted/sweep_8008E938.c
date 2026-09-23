// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8* lbl_80281EE0;

u8 fn_8008E938(void);
u8 fn_8008E938(void) {
    return *(u8*)(lbl_80281EE0 + 0x78);
}
