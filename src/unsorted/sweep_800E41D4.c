// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void fn_80062D6C();
void fn_800E0A98();
void fn_800E0AC4();
void fn_800E5474();

void fn_800E41D4(u8* p0);
void fn_800E4204(void);
void fn_800E41D4(u8* p0) {
    fn_80062D6C(42, (p0 + 0x1));
    fn_800E4204();
}

void fn_800E4204(void) {
    fn_800E0AC4(0);
    fn_800E0A98(0);
    fn_800E5474(0);
}
