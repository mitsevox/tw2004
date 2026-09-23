// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 OSGetResetButtonState();
s32 OSResetSystem(s32, s32, s32);
s32 fn_80123CBC(s32, s32);
s32 fn_80123E34();
extern s32 lbl_80282540;

void fn_8012402C(void);
void fn_8012402C(void) {
    fn_80123E34();
    fn_80123CBC(0, 0);
    if (OSGetResetButtonState() != 0) {
        lbl_80282540 = 1;
        return;
    }
    if ((s32) lbl_80282540 != 0) {
        OSResetSystem(0, 1, 0);
    }
}