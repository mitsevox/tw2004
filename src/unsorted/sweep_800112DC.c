// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_801A3478[];
extern u8 lbl_801A3494[];
void GXSetProjectionv();
void fn_80012444();

void fn_800112DC(void);
void fn_800112DC(void) {
    fn_80012444(lbl_801A3494);
    GXSetProjectionv(lbl_801A3478);
}
