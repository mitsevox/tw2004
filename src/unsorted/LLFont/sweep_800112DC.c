// Small functions found by the sweep (sweep.py). Original file: LLFont.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

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
