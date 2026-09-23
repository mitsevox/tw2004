// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gSession[];
extern u8 lbl_80260CB8[];
s32 fn_80077B08();
void fn_801022BC();
void fn_80102468();

void fn_801217C4(void);
void fn_801217C4(void) {
    s32 t0;
    t0 = fn_80077B08();
    fn_801022BC(t0, *(s32*)(lbl_80260CB8 + 0x4));
    *(s32*)(gSession + 0x44) = 30;
    fn_80102468(gSession);
}
