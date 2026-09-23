// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_80077ACC();
s32 fn_80106244(s16);

void fn_80108494(s32* arg0, s32* arg1);
void fn_80108494(s32* arg0, s32* arg1) {
    fn_80077ACC();
    if (fn_80106244((s16) *arg0) >= 0) {
        *arg1 = 1;
        return;
    }
    *arg1 = 0;
}