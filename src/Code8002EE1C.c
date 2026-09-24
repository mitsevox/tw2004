// Code8002EE1C.c (our name): own unit, split off Golfer.c at 0x8002EE1C: the session. Its
// .sdata2 starts after Code8002DB80.c's padding at 0x80282F1C..0x80282F20.

#include "golfer.h"
#include "endian.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game/save.h"
#include "frontend/fe.h"

// ---- the session and its options ---------------------------------------------------------------

extern char gszEmpty[8];            // 0x802810B8  "" (small data)
extern char lbl_80187650[];         // "cl_bbsd" ... the default name at +0x1A

void Options_SetDefaults(GameOptions* pOpt);

// A fresh session: one player, every slot a CPU on tee set 2 with an empty profile.
void Session_Init(void) {
    Session* pSession = &gSession;
    int      i, j;

    pSession->uFlags      = 0;
    pSession->nGameType   = 0;
    pSession->a8[0]     = 0;
    pSession->nC          = 0;
    pSession->nSplitScreen = 0;
    pSession->b11         = 0;
    pSession->b12         = 0;
    pSession->bReplay     = 0;
    pSession->nPaused   = 0;
    pSession->uFlags     &= ~0x40;
    pSession->fFrameTime  = 0.0f;
    pSession->f1C         = 0.0f;
    pSession->n20         = 0;
    pSession->nFrameCount = 0;
    pSession->n28         = 0;
    Options_SetDefaults(&gSession.options);
    gSession.nSeed = Misc_RandFunc(0);
    Misc_SetSeedFunc(0, gSession.nSeed);
    gSession.nNumPlayers = 1;
    gSession.nPinSet     = -1;
    gSession.bStrokeLimit = 1;
    for (i = 0; i < 5; i++) {
        gSession.nController[i] = CONTROLLER_CPU;
        gSession.nGolfer[i]     = 0;
        gSession.nTeeSet[i]     = 2;
        gSession.uBag[i]        = 0;
        gSession.aProfile[i].n0 = 0;
        gSession.aProfile[i].n1 = 0;
        gSession.aProfile[i].n2 = 0;
        for (j = 0; j < 6; j++) {
            fn_800CB700(&gSession.aProfile[i].aNames[j], gszEmpty);
        }
    }
    gSession.f5B3C = 0.0f;
    gSession.f5B40 = 150.0f;
    gSession.f5B44 = -400.0f;
    gSession.f5B48 = 1.0f;
}

// Fill each player's profile from their golfer: a created golfer's from the save, the two
// default golfers' from a fixed name, a pro's ball type from their SPIN rating (0 for the golfers
// fn_80077B18 picks).
void Session_SetupProfiles(void) {
    Session* pSession = &gSession;
    int      i, j;
    s8       nSpin;

    for (i = 0; i < pSession->nNumPlayers; i++) {
        int            nGolfer;
        nSpin = gGolferTable[pSession->nGolfer[i]].attr[ATTR_SPIN];
        gSession.aProfile[i].n1 = 0;
        for (j = 0; j < 6; j++) {
            fn_800CB700(&gSession.aProfile[i].aNames[j], gszEmpty);
        }
        gSession.aProfile[i].nOutfit = gGolferTable[pSession->nGolfer[i]].nOutfit;
        nGolfer = pSession->nGolfer[i];
        if (nGolfer >= FIRST_CREATED_GOLFER) {
            for (j = 0; j < 6; j++) {
                gSession.aProfile[i].aNames[j] = gpSaveData[nGolfer - FIRST_CREATED_GOLFER].aGolferNames[j];
            }
            gSession.aProfile[i].n2        = gpSaveData[nGolfer - FIRST_CREATED_GOLFER].n54C2;
            gSession.aProfile[i].nBallType = gpSaveData[nGolfer - FIRST_CREATED_GOLFER].nGolferBallType;
            gSession.aProfile[i].nOutfit   = gpSaveData[nGolfer - FIRST_CREATED_GOLFER].nGolferOutfit;
        } else if (nGolfer == 0 || nGolfer == 1) {
            fn_800CB700(&gSession.aProfile[i].aNames[0], lbl_80187650 + 0x1A);
            gSession.aProfile[i].n2        = 0;
            gSession.aProfile[i].nBallType = 0;
        } else if (fn_80077B18(nGolfer)) {
            gSession.aProfile[i].n2        = 0;
            gSession.aProfile[i].nBallType = 0;
        } else {
            gSession.aProfile[i].n2 = 0;
            gSession.aProfile[i].nBallType = (nSpin >= 100) ? 3 : ((nSpin >= 75) ? 2 : (nSpin >= 50));
        }
    }
}

void fn_8002F180(void) {
    TI_vResetCounter(0);
    TI_vStartCounter(0);
}

void Session_SetNumPlayers(int n) {
    gSession.nNumPlayers = n;
}

void Session_SetGolfer(int nGolfer, int nPlayer) {
    gSession.nGolfer[nPlayer] = nGolfer;
}
