// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gSession[];

u8* fn_80089B78(void);
u8* fn_80089B78(void) {
    *(s32*)(gSession + 0xC) = 2;
    return gSession;
}
