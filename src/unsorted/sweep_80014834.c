// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

u8 UStream_Update();
s32 fn_800151BC();
s32 fn_800151E4();

void fn_80014834(void);
void fn_80014834(void) {
    fn_800151E4();
    do {

    } while (UStream_Update() != 0);
    fn_800151BC();
}