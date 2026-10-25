// DynamicRenderingBuffer.c (our name): the dynamic rendering buffer, vertices a shader object
// rewrites every frame (engine.h's DynRenderBuffer). Its allocations name the header
// "GoShaderObjectCommon_DynamicRenderingBuffer_Gc.h"; the .c file that holds this code is not known.
// Three GX index writes the compiler did not inline (0x80070B50..0x80070B74) follow its code.

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "gx.h"
#include "core/startup.h"
#include "shaderdata.h"

f32 lbl_80281E70;

// The header's __FILE__, which the allocations name.
#define DRB_FILE "GoShaderObjectCommon_DynamicRenderingBuffer_Gc.h"

void fn_80012520(u32 ePrim, u32 eFormat, u16 nVerts);  // LLFont.c: GXBegin
void fn_800124A8(void);                                // LLFont.c: end the primitive
void fn_80070B50(u16 nIndex);
void fn_80070B5C(u16 nIndex);
void fn_80070B68(u16 nIndex);

// ---- sweep code (not yet cleaned up) ----

void fn_800124CC();

// ---- end of sweep code ----

// Draw the display list pList of nSize bytes, then note how full the GX FIFO got.
void SD_CallDisplayList(void* pList, u32 nSize) {
    GXCallDisplayList(pList, nSize);
    fn_800124CC();
}

// Make a buffer for nMaxVerts vertices (two pairs of position, colour and texture-coordinate
// arrays), 1.6 indices per vertex and nMaxDraws draws.
DynRenderBuffer* DynRenderBuffer_Create(int nMaxVerts, int nMaxDraws) {
    DynRenderBuffer* pBuf;

    pBuf = Circular_MemAllocFunc(sizeof(DynRenderBuffer), MEMTYPE_LOW, 32, DRB_FILE, 163);
    // room for 1.6 indices per vertex
    pBuf->pIndices = Circular_MemAllocFunc((int)(1.6f * (nMaxVerts * sizeof(u16))), MEMTYPE_LOW, 32,
                                           DRB_FILE, 173);
    pBuf->apPos[0] = Circular_MemAllocFunc(nMaxVerts * 12, MEMTYPE_LOW, 32, DRB_FILE, 181);
    pBuf->apPos[1] = Circular_MemAllocFunc(nMaxVerts * 12, MEMTYPE_LOW, 32, DRB_FILE, 188);
    pBuf->apColour[0] = Circular_MemAllocFunc(nMaxVerts * 4, MEMTYPE_LOW, 32, DRB_FILE, 195);
    pBuf->apColour[1] = Circular_MemAllocFunc(nMaxVerts * 4, MEMTYPE_LOW, 32, DRB_FILE, 202);
    pBuf->apTexCoord[0] = Circular_MemAllocFunc(nMaxVerts * 8, MEMTYPE_LOW, 32, DRB_FILE, 209);
    pBuf->apTexCoord[1] = Circular_MemAllocFunc(nMaxVerts * 8, MEMTYPE_LOW, 32, DRB_FILE, 216);
    pBuf->pDraws = Circular_MemAllocFunc(nMaxDraws * sizeof(DynRenderDraw) + 4, MEMTYPE_LOW, 32, DRB_FILE,
                                         223);
    pBuf->nMaxVerts = nMaxVerts;
    pBuf->nMaxDraws = nMaxDraws;
    pBuf->nBuffer = 0;
    pBuf->nIndices = 0;
    pBuf->nVerts = 0;
    return pBuf;
}

// Free the buffer and each of its arrays.
void DynRenderBuffer_Free(DynRenderBuffer* pBuf) {
    Circular_MemFreeFunc(pBuf->pDraws);
    Circular_MemFreeFunc(pBuf->apTexCoord[1]);
    Circular_MemFreeFunc(pBuf->apTexCoord[0]);
    Circular_MemFreeFunc(pBuf->apColour[1]);
    Circular_MemFreeFunc(pBuf->apColour[0]);
    Circular_MemFreeFunc(pBuf->apPos[1]);
    Circular_MemFreeFunc(pBuf->apPos[0]);
    Circular_MemFreeFunc(pBuf->pIndices);
    Circular_MemFreeFunc(pBuf);
}

// Draw nCount of the buffer's indices from nStart as one primitive, from the vertex arrays last
// filled.
void DynRenderBuffer_Draw(DynRenderBuffer* pBuf, u32 nStart, u16 nCount, int nPrim) {
    u32 i;
    u8 nBuffer = pBuf->nBuffer;

    GXInvalidateVtxCache();
    GXSetArray(GX_VA_POS, pBuf->apPos[nBuffer], 12);
    GXSetArray(13, pBuf->apTexCoord[nBuffer], 8);     // GX_VA_TEX0
    GXSetArray(11, pBuf->apColour[nBuffer], 4);       // GX_VA_CLR0
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, 3);                        // 16-bit indices
    GXSetVtxDesc(11, 3);
    GXSetVtxDesc(13, 3);
    fn_80012520(lbl_8018C7C8[nPrim], 2, nCount);
    for (i = nStart; i < nStart + nCount; i++) {
        fn_80070B68(pBuf->pIndices[i]);
        fn_80070B5C(pBuf->pIndices[i]);
        fn_80070B50(pBuf->pIndices[i]);
    }
    fn_800124A8();
}

// Append nVerts vertices (any array may be NULL); bFlip first switches to the other pair of arrays
// and starts them over.
void DynRenderBuffer_AddVertices(DynRenderBuffer* pBuf, const void* pPos, const void* pColour,
                                 const void* pTexCoord, int nVerts, u8 bFlip) {
    u8* pDst;
    int uSize;
    u8 nBuffer;

    if (bFlip) {
        pBuf->nBuffer = (pBuf->nBuffer + 1) % 2;
        pBuf->nVerts = 0;
    }
    nBuffer = pBuf->nBuffer;
    if (pPos != NULL) {
        pDst = (u8*)pBuf->apPos[nBuffer];
        uSize = nVerts * 12;
        pDst += pBuf->nVerts * 12;
        Mem_cpy(pDst, pPos, uSize);
        DCFlushRange(pDst, uSize);
    }
    if (pColour != NULL) {
        pDst = (u8*)pBuf->apColour[nBuffer];
        uSize = nVerts * 4;
        pDst += pBuf->nVerts * 4;
        Mem_cpy(pDst, pColour, uSize);
        DCFlushRange(pDst, uSize);
    }
    if (pTexCoord != NULL) {
        pDst = (u8*)pBuf->apTexCoord[nBuffer];
        uSize = nVerts * 8;
        pDst += pBuf->nVerts * 8;
        Mem_cpy(pDst, pTexCoord, uSize);
        DCFlushRange(pDst, uSize);
    }
    pBuf->nVerts += nVerts;
}

// Append nCount indices, offset by the indices already there; bRestart starts the list over and
// copies them as they are.
void DynRenderBuffer_AddIndices(DynRenderBuffer* pBuf, u16* pIndices, u32 nCount, u8 bRestart) {
    u16* pDst = pBuf->pIndices;
    u32 i;

    if (!bRestart) {
        pDst += pBuf->nIndices;
    } else {
        pBuf->nIndices = 0;
    }
    if (!bRestart) {
        for (i = 0; i < nCount; i++) {
            pDst[i] = pBuf->nIndices + pIndices[i];
        }
    } else {
        Mem_cpy(pDst, pIndices, nCount * sizeof(u16));
    }
    pBuf->nIndices += nCount;
}

// Append nCount draws (or, with no list, one draw of nCount indices of kind nPrim); bRestart starts
// the list over. Appended draws start after the indices already there.
// EA writes the list (DynRenderDrawList) as a stream of words through one cursor: the count, then
// three words per draw (nStart, nCount, nPrim).
void DynRenderBuffer_AddDraws(DynRenderBuffer* pBuf, const DynRenderDrawIn* pIn, u16 nCount, int nPrim,
                              u8 bRestart) {
    u32* pWord = (u32*)pBuf->pDraws;
    u32 nOld;
    u32 i;
    u32 j;

    if (bRestart) {
        if (pIn == NULL) {
            pWord[0] = 1;
            pWord[1] = 0;
            pWord[2] = nCount;
            pWord[3] = nPrim;
            return;
        }
        *pWord++ = nCount;
        for (i = 0; i < nCount; i++) {
            pWord[0] = pIn[i].nStart;
            pWord[1] = pIn[i].nCount;
            pWord[2] = pIn[i].nPrim;
            pWord += 3;
        }
    } else {
        if (pIn == NULL) {
            nOld = *pWord;
            *pWord++ = nOld + 1;
            for (j = 0; j < nOld; j++) {
                pWord += 3;
            }
            pWord[0] = pBuf->nIndices;
            pWord[1] = nCount;
            pWord[2] = nPrim;
            return;
        }
        nOld = *pWord;
        *pWord++ = nOld + nCount;
        for (j = 0; j < nOld; j++) {
            pWord += 3;
        }
        for (i = 0; i < nCount; i++) {
            pWord[0] = pBuf->nIndices + pIn[i].nStart;
            pWord[1] = pIn[i].nCount;
            pWord[2] = pIn[i].nPrim;
            pWord += 3;
        }
    }
}

// ---- GX index writes, not inlined ----

void fn_80070B50(u16 nIndex) {
    *(volatile u16*)0xCC008000 = nIndex;
}

void fn_80070B5C(u16 nIndex) {
    *(volatile u16*)0xCC008000 = nIndex;
}

void fn_80070B68(u16 nIndex) {
    *(volatile u16*)0xCC008000 = nIndex;
}

// ---- the static shader object hooks of rows 7, 8, 1 and 3 of lbl_80188E88 ----

void fn_80070B74(void);
void fn_80070B78(StaticShaderObject* pObj, ShaderCmds* pCmds);
void fn_80070BAC(StaticShaderObject* pObj);
void fn_80070BD0(StaticShaderObject* pObj);
void fn_80070C24(StaticShaderObject* pObj, ShaderCmds* pCmds);
void fn_80070C58(StaticShaderObject* pObj);
void fn_80070C7C(StaticShaderObject* pObj);
void fn_80070CD0(StaticShaderObject* pObj, ShaderCmds* pCmds);
void fn_80070D04(StaticShaderObject* pObj);
void fn_80070D28(StaticShaderObject* pObj);
void fn_80070D7C(f32* pfWeight);
void fn_80070D88(StaticShaderObject* pObj, ShaderCmds* pCmds);
void fn_80070DBC(StaticShaderObject* pObj);
void fn_80070DF4(StaticShaderObject* pObj);

// Row 7's data hook: nothing.
void fn_80070B74(void) {
}

// Row 7's static init: build the object's display list from pCmds.
void fn_80070B78(StaticShaderObject* pObj, ShaderCmds* pCmds) {
    ShaderData_BuildDisplayList(pObj->pArrays, pObj->eType, &pObj->anim, pCmds);
}

// Row 7's static close.
void fn_80070BAC(StaticShaderObject* pObj) {
    ShaderData_CloseDisplayList(&pObj->anim);
}

// Row 7's static render: the display list (with the object's positions when the list sets none).
void fn_80070BD0(StaticShaderObject* pObj) {
    if (pObj->anim.bNoPosArray != 0) {
        GXSetArray(GX_VA_POS, pObj->pArrays->apPos[0], 12);
    }
    SD_CallDisplayList(pObj->anim.pList, pObj->anim.nListSize);
}

// Row 8's static init.
void fn_80070C24(StaticShaderObject* pObj, ShaderCmds* pCmds) {
    ShaderData_BuildDisplayList(pObj->pArrays, pObj->eType, &pObj->anim, pCmds);
}

// Row 8's static close.
void fn_80070C58(StaticShaderObject* pObj) {
    ShaderData_CloseDisplayList(&pObj->anim);
}

// Row 8's static render.
void fn_80070C7C(StaticShaderObject* pObj) {
    if (pObj->anim.bNoPosArray != 0) {
        GXSetArray(GX_VA_POS, pObj->pArrays->apPos[0], 12);
    }
    SD_CallDisplayList(pObj->anim.pList, pObj->anim.nListSize);
}

// Row 1's static init.
void fn_80070CD0(StaticShaderObject* pObj, ShaderCmds* pCmds) {
    ShaderData_BuildDisplayList(pObj->pArrays, pObj->eType, &pObj->anim, pCmds);
}

// Row 1's static close.
void fn_80070D04(StaticShaderObject* pObj) {
    ShaderData_CloseDisplayList(&pObj->anim);
}

// Row 1's static render.
void fn_80070D28(StaticShaderObject* pObj) {
    if (pObj->anim.bNoPosArray != 0) {
        GXSetArray(GX_VA_POS, pObj->pArrays->apPos[0], 12);
    }
    SD_CallDisplayList(pObj->anim.pList, pObj->anim.nListSize);
}

// Row 3's data hook: the morph weight.
void fn_80070D7C(f32* pfWeight) {
    lbl_80281E70 = *pfWeight;
}

// Row 3's static init.
void fn_80070D88(StaticShaderObject* pObj, ShaderCmds* pCmds) {
    ShaderData_BuildDisplayList(pObj->pArrays, pObj->eType, &pObj->anim, pCmds);
}

// Row 3's static close: free the morph animation's data.
void fn_80070DBC(StaticShaderObject* pObj) {
    MorphAnim_Free(&pObj->anim);
    ShaderData_CloseDisplayList(&pObj->anim);
}

// Row 3's static render: the display list, with the morph animation brought up to date first once
// in every three frames, each animation on its own frame of the cycle.
void fn_80070DF4(StaticShaderObject* pObj) {
    MorphAnim* pAnim = &pObj->anim;

    if (pObj->anim.bNoPosArray != 0) {
        GXSetArray(GX_VA_POS, pObj->pArrays->apPos[0], 12);
    }
    if (MorphAnim_GetCount() != 0 && (u32)gSession.nFrameCount % 3 == pAnim->nIndex % 3 &&
        (u32)gSession.nFrameCount > MorphAnim_GetLastUpdateFrame(pAnim->nIndex)) {
        MorphAnim_Update(pAnim, lbl_80281E70);
    }
    SD_CallDisplayList(pAnim->pList, pAnim->nListSize);
}
