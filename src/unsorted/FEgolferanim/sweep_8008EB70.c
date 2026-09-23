// Small functions found by the sweep (sweep.py). Original file: FEgolferanim.c (EA's name, from its asserts; also in EA's 2002 source tree and TW06; placed by the file map).

#include "game_types.h"

s32 fn_8008DD50(s32);
s32 fn_8008E724(s32, s32, s32, s32);
s32 fn_8008E944(s32, f32);
extern f32 lbl_80283B40;

void fn_8008EB70(void);
void fn_8008EB70(void) {
    fn_8008E724(0, 0, 0, 0);
    fn_8008DD50(0);
    fn_8008E944(0, lbl_80283B40);
}