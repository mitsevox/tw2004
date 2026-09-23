// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 OSGetResetButtonState();
s32 fn_80007254();
s32 fn_800A4BDC();
s32 fn_8012402C();
s32 fn_80124094();
extern u8 lbl_80281B8E;

void fn_8006C63C(void);
void fn_8006C63C(void) {
    fn_800A4BDC();
    if ((fn_80124094() != -1) && (fn_80124094() != 0x12) && (fn_80124094() != 0x11)) {
        fn_8012402C();
    }
    if ((u8) lbl_80281B8E == 0) {
        lbl_80281B8E = OSGetResetButtonState();
    }
    fn_80007254();
}