// GoShaderObjectContainer_OBFData_Gc.c (EA's name, from its asserts; also in EA's 2002 source
// tree): the sweep code below is the matched small functions.

#include "game.h"
#include "ball.h"
#include "psmgr.h"
#include "obfdata.h"
#include "shaderdata.h"
#include "core/startup.h"

// Copies pChunk's data into a new 32-byte aligned buffer (flushed for the graphics chip) and keeps
// it in pData by the chunk's kind, with the chunk's n02.
void fn_8006FCDC(OBFData* pData, OBFChunk* pChunk) {
    u8 nKind;
    u32 uSize;
    u8* pBuf;

    uSize = pChunk->n02 * pChunk->n04;
    pBuf = Circular_MemAllocFunc((uSize + 31) & ~31, MEMTYPE_LOW, 32,
                                 "GoShaderObjectContainer_OBFData_Gc.c", 0x41);
    nKind = pChunk->nKind;
    Mem_cpy(pBuf, pChunk->aData, uSize);
    DCFlushRange(pBuf, uSize);
    switch (nKind) {
    case 2:
        pData->aBufs[pChunk->nIndex] = pBuf;
        pData->an20[pChunk->nIndex] = pChunk->n02;
        break;
    case 0:
        pData->pBuf0 = pBuf;
        pData->n2C = pChunk->n02;
        break;
    case 1:
        pData->pBuf1 = pBuf;
        pData->n2A = pChunk->n02;
        break;
    case 3:
        pData->pBuf3 = pBuf;
        pData->n2E = pChunk->n02;
        break;
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_8006FDCC(void);
void fn_8006FDD0(void);
void fn_8006FED4(StaticShaderObject* pObj, ShaderCmds* pCmds);
void fn_8006FF08(StaticShaderObject* pObj);

void fn_8006FDCC(void) {
}

void fn_8006FDD0(void) {
}

// ---- end of sweep code ----

// Loads each chunk of pList into pData.
void fn_8006FDD4(OBFData* pData, OBFChunkList* pList) {
    OBFChunkList* const pListCopy = pList;      // fake match: a const copy gets its own register
    s32 i;

    for (i = 0; i < pListCopy->nChunks; i++) {
        fn_8006FCDC(pData, pListCopy->apChunks[i]);
    }
}

// Frees pData's buffers.
void fn_8006FE44(OBFData* pData) {
    s32 i;

    for (i = 0; i < 5; i++) {
        if (pData->aBufs[i] != NULL) {
            Circular_MemFreeFunc(pData->aBufs[i]);
        }
    }
    if (pData->pBuf1 != NULL) {
        Circular_MemFreeFunc(pData->pBuf1);
    }
    if (pData->pBuf0 != NULL) {
        Circular_MemFreeFunc(pData->pBuf0);
    }
    if (pData->pBuf3 != NULL) {
        Circular_MemFreeFunc(pData->pBuf3);
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_8006FED4(StaticShaderObject* pObj, ShaderCmds* pCmds) {
    ShaderData_BuildDisplayList(pObj->pArrays, pObj->eType, &pObj->anim, pCmds);
}

void fn_8006FF08(StaticShaderObject* pObj) {
    ShaderData_CloseDisplayList(&pObj->anim);
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

void fn_8006FF2C(StaticShaderObject* pObj);

void fn_8006FF2C(StaticShaderObject* pObj) {
    if (pObj->anim.bNoPosArray != 0) {
        GXSetArray(GX_VA_POS, pObj->pArrays->apPos[0], 12);
    }
    SD_CallDisplayList(pObj->anim.pList, pObj->anim.nListSize);
}

// ---- end of sweep code ----
