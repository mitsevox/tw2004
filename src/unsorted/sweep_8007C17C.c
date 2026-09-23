// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gSession[];

u8* fn_8007C17C(s32 p0, u8* p1);
u8* fn_8007C17C(s32 p0, u8* p1) {
    *(s32*)p1 = *(s32*)(gSession + 0x2C);
    return gSession;
}
