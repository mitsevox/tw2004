// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 lbl_80281BAC;
void AISetStreamVolLeft();
void AISetStreamVolRight();
void DVDCancelAll();

void fn_800072E8(void);
void fn_800072E8(void) {
    DVDCancelAll();
    AISetStreamVolLeft(0);
    AISetStreamVolRight(0);
    lbl_80281BAC = 1;
}
