// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gPlayers[];
extern u8 lbl_802124B8[];
void fn_800584DC();

void fn_801020C0(void);
void fn_801020C0(void) {
    fn_800584DC(*(s32*)gPlayers);
}
