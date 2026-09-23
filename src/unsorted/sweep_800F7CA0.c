// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

void AI_DefaultTarget();
void fn_800A6278();
void fn_800F7CD4();
void fn_800F80D4();

void fn_800F7CA0(void);
void fn_800F7CA0(void) {
    fn_800F7CD4();
    AI_DefaultTarget(0);
    fn_800F80D4(-1);
    fn_800A6278();
}
