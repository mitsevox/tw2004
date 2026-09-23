// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"


void fn_800B1A88(f32* arg0, f32* arg1);
void fn_800B1A88(f32* arg0, f32* arg1) {
    f32 temp_f1;

    temp_f1 = *arg1;
    *arg1 = *arg0;
    *arg0 = temp_f1;
}