// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

extern u8 gReplayData[];
void Session_SetGolfer();
void Mem_cpy();
void fn_80010284();
void fn_8007744C();
void fn_80079DAC();
void fn_80079EA8();
void fn_8008D8CC();
void fn_800E0B38();
void fn_800E14E0();

void fn_8007739C(s32 p0);
void fn_800773F8(void);
void fn_8007739C(s32 p0) {
    Mem_cpy(gReplayData, p0, 3880);
    fn_800E0B38(10);
    Session_SetGolfer(*(u8*)(gReplayData + 0x10), 0);
    fn_800E14E0(*(s32*)(gReplayData + 0xF00));
}

void fn_800773F8(void) {
    fn_80010284();
    fn_80079EA8();
    fn_8007744C();
    fn_8008D8CC();
    fn_80079DAC();
}
