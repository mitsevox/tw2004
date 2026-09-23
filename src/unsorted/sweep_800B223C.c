// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gSession[];

u8* fn_800B223C(void);
u8* fn_800B223C(void) {
    *(s32*)(gSession + 0xC) = 2;
    return gSession;
}
