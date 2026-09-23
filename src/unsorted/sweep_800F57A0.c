// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gPlayers[];
void AI_DefaultTarget();
void fn_800F1EE4();
void fn_800F57C8();

void fn_800F57A0(void);
void fn_800F57A0(void) {
    fn_800F57C8();
    AI_DefaultTarget(0);
}
