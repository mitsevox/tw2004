// char_skin.c: in a single view, one golfer at a time has its body skin's render data (what
// SKN_AllocRenderData gives a skin for posing and drawing): the player readied for its turn
// (Character_PrepareForRendering). When a hole ends (Character_ExitHole), every golfer's is freed.

#include "golfer.h"
#include "character.h"
#include "charstate.h"

// In a single view (no split screen), frees the render data of every player's golfer's body skin
// and takes the skinning matrices back from the golfer's model.
void CharacterSkin_FreeAll(void) {
    int i;

    if (gSession.nSplitScreen == 0) {
        for (i = 0; i < gSession.nNumPlayers; i++) {
            SKN_FreeRenderData(gPlayers[i].pChar->pSkin);
            SKEL_SetSkinningMatrices(gPlayers[i].pChar->pModel, NULL, 0);
        }
    }
}

// In a single view, makes nPlayer's golfer the only one whose body skin has its render data. When
// that is not so already, every other player's skin's render data is freed, nPlayer's is allocated,
// its character loses CHARACTER_CLEARED_ON_SKIN_LOAD, and its model is handed the skin's skinning
// matrices, one per bone, which are then filled in from the model's bone matrices.
void CharacterSkin_LoadOnlyPlayer(int nPlayer) {
    int i;

    if (gSession.nSplitScreen == 0) {
        for (i = 0; i < gSession.nNumPlayers; i++) {
            if (i == nPlayer && !(gPlayers[i].pChar->pSkin->uFlags & SKIN_HAS_RENDER_DATA)) {
                break;
            }
            if (i != nPlayer && (gPlayers[i].pChar->pSkin->uFlags & SKIN_HAS_RENDER_DATA)) {
                break;
            }
        }
        if (i != gSession.nNumPlayers) {
            for (i = 0; i < gSession.nNumPlayers; i++) {
                if (i != nPlayer) {
                    SKN_FreeRenderData(gPlayers[i].pChar->pSkin);
                    SKEL_SetSkinningMatrices(gPlayers[nPlayer].pChar->pModel, NULL, 0);
                }
            }
            SKN_AllocRenderData(gPlayers[nPlayer].pChar->pSkin, 1);
            gPlayers[nPlayer].pChar->uCharFlags &= ~CHARACTER_CLEARED_ON_SKIN_LOAD;
            SKEL_SetSkinningMatrices(gPlayers[nPlayer].pChar->pModel,
                                     gPlayers[nPlayer].pChar->pSkin->pSkinMtx,
                                     gPlayers[nPlayer].pChar->pSkin->pModel->nBones);
            SKEL_UpdateAllSkinningMatrices(gPlayers[nPlayer].pChar->pModel);
        }
    }
}
