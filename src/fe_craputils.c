// fe_craputils.c (TW06's golf/ui core/frontend/fe_craputils.c): the Create-A-Player (CrAP) data in
// a save profile. FE_CrAP_InitCrAPInfo resets it; the rest unlock golfers, courses and rewards,
// rate the profile, and compare CrAP names. Not decompiled yet beyond the functions below.

#include "game/save.h"
#include "charstate.h"

// The 26 bytes at 0x5EB4 start at 50, set one by one (the last two in EA's order: 25, then 24).
void fn_80058208(SaveProfile* pProfile) {
    pProfile->choices.a9B4[0] = 50;
    pProfile->choices.a9B4[1] = 50;
    pProfile->choices.a9B4[2] = 50;
    pProfile->choices.a9B4[3] = 50;
    pProfile->choices.a9B4[4] = 50;
    pProfile->choices.a9B4[5] = 50;
    pProfile->choices.a9B4[6] = 50;
    pProfile->choices.a9B4[7] = 50;
    pProfile->choices.a9B4[8] = 50;
    pProfile->choices.a9B4[9] = 50;
    pProfile->choices.a9B4[10] = 50;
    pProfile->choices.a9B4[11] = 50;
    pProfile->choices.a9B4[12] = 50;
    pProfile->choices.a9B4[13] = 50;
    pProfile->choices.a9B4[14] = 50;
    pProfile->choices.a9B4[15] = 50;
    pProfile->choices.a9B4[16] = 50;
    pProfile->choices.a9B4[17] = 50;
    pProfile->choices.a9B4[18] = 50;
    pProfile->choices.a9B4[19] = 50;
    pProfile->choices.a9B4[20] = 50;
    pProfile->choices.a9B4[21] = 50;
    pProfile->choices.a9B4[22] = 50;
    pProfile->choices.a9B4[23] = 50;
    pProfile->choices.a9B4[25] = 50;
    pProfile->choices.a9B4[24] = 50;
}

u8 fn_80058304(SaveProfile* pProfile, int nBit) {
    return fn_8001E9CC(pProfile->a10548, nBit);
}

void fn_800588D4(s16 n) {
    lbl_80281DF0.b = 1;
    lbl_80281DF0.n = n;
}

int fn_800588E8(void) {
    return lbl_80281DF0.n;
}
