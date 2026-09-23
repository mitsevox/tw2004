// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

s32 fn_8011908C();
void fn_8011937C();
s32 fn_801197CC();

void fn_800F009C(void);
void fn_800F009C(void) {
    s32 t0;
    s32 t1;
    t0 = fn_801197CC(0, 0);
    t1 = fn_8011908C(0, t0);
    fn_8011937C(0, t0, (((u32)__cntlzw((t1 & 0xFF)) >> 5) & 0xFF));
}
