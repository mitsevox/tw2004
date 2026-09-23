// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80195498[];
void fn_80126EC0();

u8* fn_80126E88(void);
u8* fn_80126E88(void) {
    fn_80126EC0();
    *(s32*)(lbl_80195498 + 0x8) = 5;
    *(u8*)(lbl_80195498 + 0xC) = 0;
    return lbl_80195498;
}
