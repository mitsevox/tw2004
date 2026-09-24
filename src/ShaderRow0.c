// ShaderRow0.c (our name): the four shader-object hooks of row 0 of lbl_80188E88 (engine.h's
// ModuleHooks, +0x34..+0x40): make and free the object's dynamic rendering buffer
// (DynamicRenderingBuffer.c), draw it and fill it. The same code as ShaderRow19.c except that the
// fill adds its draws as primitive kind 0 (row 19: 3); like that file it has no data of its own.
// Row 0's other hooks (+0x24..+0x2C, 0x8006FED4..0x8006FF80) sit just before it.

#include "game_types.h"
#include "engine.h"

void fn_8006FF80(DynRenderObject* pObj, const DynRenderSize* pSize);
void fn_8006FFD4(DynRenderObject* pObj);
void fn_8006FFF8(DynRenderObject* pObj);
void fn_80070078(DynRenderObject* pObj, DynRenderFill* pFill, u8 bRestart);

// Make the object's buffer, at the given sizes or at 50 vertices and one draw.
void fn_8006FF80(DynRenderObject* pObj, const DynRenderSize* pSize) {
    if (pSize != NULL) {
        pObj->pBuf = fn_8007018C(pSize->nMaxVerts, pSize->nMaxDraws);
        return;
    }
    pObj->pBuf = fn_8007018C(50, 1);
}

// Free the object's buffer.
void fn_8006FFD4(DynRenderObject* pObj) {
    fn_80070348(pObj->pBuf);
}

// Draw every draw in the object's buffer.
void fn_8006FFF8(DynRenderObject* pObj) {
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
        fn_800703B8(pObj->pBuf, nStart, nCount, nPrim);
    }
}

// Add a frame's draws, indices and vertices to the object's buffer (bRestart: replace what is
// there). Without a draw list, the indices form one draw; with one, the index count is the end of
// the furthest draw.
void fn_80070078(DynRenderObject* pObj, DynRenderFill* pFill, u8 bRestart) {
    u16 nIndices;
    u16 i;

    fn_80070764(pObj->pBuf, pFill->pDraws, pFill->nCount, 0, bRestart);
    if (pFill->pDraws == NULL) {
        fn_800705F0(pObj->pBuf, pFill->pIndices, pFill->nCount, bRestart);
    } else {
        nIndices = 0;
        for (i = 0; i < pFill->nCount; i++) {
            if (pFill->pDraws[i].nStart + pFill->pDraws[i].nCount > nIndices) {
                nIndices = pFill->pDraws[i].nStart + pFill->pDraws[i].nCount;
            }
        }
        fn_800705F0(pObj->pBuf, pFill->pIndices, nIndices, bRestart);
    }
    fn_800704C4(pObj->pBuf, pFill->pPos, pFill->pColour, pFill->pTexCoord, pFill->nVerts, bRestart);
}
