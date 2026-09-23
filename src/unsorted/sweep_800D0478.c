// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gPlayers[];
void fn_800D05A4();

void fn_800D0478(s32 p0);
void fn_800D04AC(s32 p0);
void fn_800D04E0(s32 p0);
void fn_800D0478(s32 p0) {
    fn_800D05A4(((gPlayers + (p0 * 3832)) + 0xA90), (p0 * 3832));
}

void fn_800D04AC(s32 p0) {
    fn_800D05A4(((gPlayers + (p0 * 3832)) + 0xAD0), (p0 * 3832));
}

void fn_800D04E0(s32 p0) {
    fn_800D05A4(((gPlayers + (p0 * 3832)) + 0xB5C), (p0 * 3832));
}
