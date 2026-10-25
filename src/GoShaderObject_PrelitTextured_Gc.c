// GoShaderObject_PrelitTextured_Gc.c: the hooks of row 0 of the shader-object table, a prelit,
// textured mesh (the club trail, the shadows, the ball marker, the ball effects) that its owner
// refills every frame and that is drawn through a dynamic rendering buffer (engine.h's
// DynRenderBuffer).

#include "game_types.h"
#include "engine.h"

// Make the object's buffer, at the given sizes or at 50 vertices and one draw.
void SD_vShaderObject_PrelitTextured_Dynamic_Init(DynRenderObject* pObj, const DynRenderSize* pSize) {
    if (pSize != NULL) {
        pObj->pBuf = DynRenderBuffer_Create(pSize->nMaxVerts, pSize->nMaxDraws);
        return;
    }
    pObj->pBuf = DynRenderBuffer_Create(50, 1);
}

// Free the object's buffer.
void SD_vShaderObject_PrelitTextured_Dynamic_Close(DynRenderObject* pObj) {
    DynRenderBuffer_Free(pObj->pBuf);
}

// Draw each draw in the object's buffer as one primitive of its kind.
void SD_vShaderObject_PrelitTextured_Dynamic_Render(DynRenderObject* pObj) {
    u32 nDraws;
    DynRenderDraw* pDraw;
    u32 i = 0;
    DynRenderDrawList* pList = pObj->pBuf->pDraws;

    nDraws = pList->nDraws;
    pDraw = pList->aDraws;
    for (; i < nDraws; i++) {
        u32 nStart = pDraw->nStart;
        u32 nCount = pDraw->nCount;
        int nPrim = pDraw->nPrim;
        pDraw++;
        DynRenderBuffer_Draw(pObj->pBuf, nStart, nCount, nPrim);
    }
}

// Hand the object's buffer one frame's draws, indices and vertices. bRestart replaces what the
// buffer holds and moves the vertices to its other pair of arrays; without it they are appended.
// With no draw list the nCount indices are one triangle strip; with one, the index count is the
// end of the furthest draw.
void SD_vShaderObject_PrelitTextured_Dynamic_Refresh(DynRenderObject* pObj, DynRenderFill* pFill,
                                                     u8 bRestart) {
    u16 nIndices;
    u16 i;

    DynRenderBuffer_AddDraws(pObj->pBuf, pFill->pDraws, pFill->nCount, DYNRENDER_PRIM_TRIANGLE_STRIP,
                             bRestart);
    if (pFill->pDraws == NULL) {
        DynRenderBuffer_AddIndices(pObj->pBuf, pFill->pIndices, pFill->nCount, bRestart);
    } else {
        nIndices = 0;
        for (i = 0; i < pFill->nCount; i++) {
            if (pFill->pDraws[i].nStart + pFill->pDraws[i].nCount > nIndices) {
                nIndices = pFill->pDraws[i].nStart + pFill->pDraws[i].nCount;
            }
        }
        DynRenderBuffer_AddIndices(pObj->pBuf, pFill->pIndices, nIndices, bRestart);
    }
    DynRenderBuffer_AddVertices(pObj->pBuf, pFill->pPos, pFill->pColour, pFill->pTexCoord, pFill->nVerts,
                                bRestart);
}
