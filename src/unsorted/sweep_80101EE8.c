// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gPlayers[];
void fn_80100160();

u8* fn_80101EE8(void);
u8* fn_80101EE8(void) {
    fn_80100160();
    *(s32*)(gPlayers + 0xA08) = 0;
    return gPlayers;
}
