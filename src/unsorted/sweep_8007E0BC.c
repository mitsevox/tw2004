// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gSession[];

u8* fn_8007E0BC(u8* p0);
u8* fn_8007E0BC(u8* p0) {
    *(u8*)(gSession + 0x5B38) = *(s32*)p0;
    return gSession;
}
