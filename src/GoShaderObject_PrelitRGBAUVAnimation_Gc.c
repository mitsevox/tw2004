// GoShaderObject_PrelitRGBAUVAnimation_Gc.c: the static hooks of row 4 of the shader-object table,
// model parts drawn from a display list whose texture coordinates go through a texture
// animation's matrix, so the texture scrolls.

#include "shaderdata.h"
#include "terrain.h"

// Move every texture animation to the frame count Ter_BeginRender hands in; the four waves beside
// it are not read.
void SD_vShaderObject_PrelitRGBAUVAnimation_Type_SetParameters(TerWaveData* pWave) {
    TexAnim_Update(pWave->nFrame);
}

// Build the object's display list from pCmds.
void SD_vShaderObject_PrelitRGBAUVAnimation_Static_Init(StaticShaderObject* pObj, ShaderCmds* pCmds) {
    ShaderData_BuildDisplayList(pObj->pArrays, pObj->eType, &pObj->anim, pCmds);
}

// Close the object, forget every texture animation (every object's, not only this one's) and set
// the texture animation clock back to 0.
void SD_vShaderObject_PrelitRGBAUVAnimation_Static_Close(StaticShaderObject* pObj) {
    ShaderData_CloseDisplayList(&pObj->anim);
    TexAnim_RemoveAll();
}

// Draw the object's display list, first pointing GX at its first frame's positions when the list
// sets no position array.
void SD_vShaderObject_PrelitRGBAUVAnimation_Static_Render(StaticShaderObject* pObj) {
    if (pObj->anim.bNoPosArray != 0) {
        GXSetArray(GX_VA_POS, pObj->pArrays->apPos[0], 12);
    }
    SD_CallDisplayList(pObj->anim.pList, pObj->anim.nListSize);
}
