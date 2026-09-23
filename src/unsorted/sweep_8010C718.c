// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gSession[];
extern s32 lbl_80281764;

u8* fn_8010C718(void);
u8* fn_8010C718(void) {
    *(s32*)(gSession + 0x58) = 0;
    *(s32*)(gSession + 0x5C) = 0;
    *(s32*)(gSession + 0xE98) = 0;
    lbl_80281764 = 5;
    return gSession;
}
