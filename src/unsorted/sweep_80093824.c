// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8001F004();
s32 fn_8003532C();
s32 fn_80093A04(s32, s32);

void fn_80093824(void);
void fn_80093824(void) {
    fn_8003532C();
    fn_80093A04(0, fn_8001F004());
}