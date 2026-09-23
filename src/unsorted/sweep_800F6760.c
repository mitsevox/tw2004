// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gPlayers[];
void AI_DefaultTarget();
void fn_800F1EE4();

void fn_800F6760(void);
u8* fn_800F6788(void);
void fn_800F6760(void) {
    fn_800F6788();
    AI_DefaultTarget(0);
}

u8* fn_800F6788(void) {
    fn_800F1EE4();
    *(u8*)(gPlayers + 0xE9C) = 0;
    *(s32*)(gPlayers + 0xDC0) = 5;
    *(u8*)(gPlayers + 0x1D94) = 0;
    *(s32*)(gPlayers + 0x1CB8) = 5;
    *(u8*)(gPlayers + 0x2C8C) = 0;
    *(s32*)(gPlayers + 0x2BB0) = 5;
    *(u8*)(gPlayers + 0x3B84) = 0;
    *(s32*)(gPlayers + 0x3AA8) = 5;
    *(u8*)(gPlayers + 0x4A7C) = 0;
    *(s32*)(gPlayers + 0x49A0) = 5;
    return gPlayers;
}
