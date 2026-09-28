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
            SKN_FreeRenderData(gPlayers[i].pChar->pSkin);
            SKEL_SetSkinningMatrices(gPlayers[i].pChar->pModel, NULL, 0);
        }
    }
}

// With a single view, makes nPlayer's golfer the only one whose body skin is loaded (flag 2 of its
// uFlags): if any golfer is the wrong way, the others' skins are freed and nPlayer's is loaded and
// its matrices given to the model.
void fn_800955F0(int nPlayer) {
    int i;

    if (gSession.nSplitScreen == 0) {
        for (i = 0; i < gSession.nNumPlayers; i++) {
            if (i == nPlayer && !(gPlayers[i].pChar->pSkin->uFlags & 2)) {
                break;
            }
            if (i != nPlayer && (gPlayers[i].pChar->pSkin->uFlags & 2)) {
                break;
            }
        }
        if (i != gSession.nNumPlayers) {
            for (i = 0; i < gSession.nNumPlayers; i++) {
                if (i != nPlayer) {
                    SKN_FreeRenderData(gPlayers[i].pChar->pSkin);
                    // EA: clears nPlayer's model, not player i's (it is set again below)
                    SKEL_SetSkinningMatrices(gPlayers[nPlayer].pChar->pModel, NULL, 0);
                }
            }
            SKN_AllocRenderData(gPlayers[nPlayer].pChar->pSkin, 1);
            gPlayers[nPlayer].pChar->uCharFlags &= ~0x2000;
            SKEL_SetSkinningMatrices(gPlayers[nPlayer].pChar->pModel,
                                     gPlayers[nPlayer].pChar->pSkin->pSkinMtx,
                        gPlayers[nPlayer].pChar->pSkin->pModel->n14);
            SKEL_UpdateAllSkinningMatrices(gPlayers[nPlayer].pChar->pModel);
        }
    }
}
