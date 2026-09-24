// Code80095564.c (our name; the file it belongs to is not known): with a single view, only one
// golfer's body skin is kept loaded. Its extent is the gap 0x80095564-0x80095744 between
// GoShaderObject_Particle_Gc.c's sweep of empty functions and fn_80095744.

#include "golfer.h"
#include "character.h"
#include "charstate.h"

// With a single view, frees every golfer's body skin and clears their models' skin matrices.
void fn_80095564(void) {
    int i;

    if (gSession.nSplitScreen == 0) {
        for (i = 0; i < gSession.nNumPlayers; i++) {
            fn_80037708(gPlayers[i].pChar->pSkin);
            fn_80029A7C(gPlayers[i].pChar->pModel, NULL, 0);
        }
    }
}

// With a single view, makes nPlayer's golfer the only one whose body skin is loaded (flag 2 of its
// u10D4): if any golfer is the wrong way, the others' skins are freed and nPlayer's is loaded and
// its matrices given to the model.
void fn_800955F0(int nPlayer) {
    int i;

    if (gSession.nSplitScreen == 0) {
        for (i = 0; i < gSession.nNumPlayers; i++) {
            if (i == nPlayer && !(gPlayers[i].pChar->pSkin->u10D4 & 2)) {
                break;
            }
            if (i != nPlayer && (gPlayers[i].pChar->pSkin->u10D4 & 2)) {
                break;
            }
        }
        if (i != gSession.nNumPlayers) {
            for (i = 0; i < gSession.nNumPlayers; i++) {
                if (i != nPlayer) {
                    fn_80037708(gPlayers[i].pChar->pSkin);
                    // EA: clears nPlayer's model, not player i's (it is set again below)
                    fn_80029A7C(gPlayers[nPlayer].pChar->pModel, NULL, 0);
                }
            }
            fn_800375AC(gPlayers[nPlayer].pChar->pSkin, 1);
            gPlayers[nPlayer].pChar->u10 &= ~0x2000;
            fn_80029A7C(gPlayers[nPlayer].pChar->pModel, gPlayers[nPlayer].pChar->pSkin->p108C,
                        gPlayers[nPlayer].pChar->pSkin->pModel->n14);
            fn_80029AF8(gPlayers[nPlayer].pChar->pModel);
        }
    }
}
