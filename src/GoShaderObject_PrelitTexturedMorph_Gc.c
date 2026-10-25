// GoShaderObject_PrelitTexturedMorph_Gc.c: the static hooks of row 2 of the shader-object table,
// textured model parts drawn from a display list whose vertices morph (morphanim.h) by a
// weight Ter_DrawObjectList hands in.

#include "shaderdata.h"
#include "golfer.h"

static f32 SD_gfPrelitTexturedMorphWeight;     // the weight the next objects' morphs are drawn at

// Keep the morph weight; Ter_DrawObjectList hands one in before it draws each animated course
// object of this type.
void SD_vShaderObject_PrelitTexturedMorph_Type_SetParameters(f32* pfWeight) {
    SD_gfPrelitTexturedMorphWeight = *pfWeight;
}

// Build the object's display list and its morph animation from pCmds.
void SD_vShaderObject_PrelitTexturedMorph_Static_Init(StaticShaderObject* pObj, ShaderCmds* pCmds) {
    ShaderData_BuildDisplayList(pObj->pArrays, pObj->eType, &pObj->anim, pCmds);
}

// Free the object's morph animation and close the object.
void SD_vShaderObject_PrelitTexturedMorph_Static_Close(StaticShaderObject* pObj) {
    MorphAnim_Free(&pObj->anim);
    ShaderData_CloseDisplayList(&pObj->anim);
}

// Draw the object's display list, first pointing GX at its first frame's positions when the list
// sets no position array. While any morph animation exists, the object's vertices are first morphed
// to SD_gfPrelitTexturedMorphWeight: in one frame of every two (three in split screen), each
// animation on its own frame of the cycle, and at most once a frame.
void SD_vShaderObject_PrelitTexturedMorph_Static_Render(StaticShaderObject* pObj) {
    MorphAnim* pAnim = &pObj->anim;
    int nEvery = (gSession.nSplitScreen != 0) + 2;

    if (pAnim->bNoPosArray != 0) {
        GXSetArray(GX_VA_POS, pObj->pArrays->apPos[0], 12);
    }
    if (MorphAnim_GetCount() != 0 && (u32)gSession.nFrameCount % nEvery == pAnim->nIndex % nEvery &&
        (u32)gSession.nFrameCount > MorphAnim_GetLastUpdateFrame(pAnim->nIndex)) {
        MorphAnim_Update(pAnim, SD_gfPrelitTexturedMorphWeight);
    }
    SD_CallDisplayList(pObj->anim.pList, pObj->anim.nListSize);
}
