// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s8 SwingStack_Top(s32);

void fn_8008823C(s32* arg0, s32* arg1);
void fn_8008823C(s32* arg0, s32* arg1) {
    if (SwingStack_Top(*arg0) == 4) {
        *arg1 = 1;
        return;
    }
    *arg1 = 0;
}