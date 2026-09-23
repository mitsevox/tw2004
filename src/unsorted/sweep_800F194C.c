// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gSession[];

u8* fn_800F194C(void);
u8* fn_800F194C(void) {
    *(u8*)(gSession + 0x12) = 1;
    return gSession;
}
