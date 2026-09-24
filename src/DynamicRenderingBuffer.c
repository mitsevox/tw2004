// DynamicRenderingBuffer.c (our name): the dynamic rendering buffer, vertices a shader object
// rewrites every frame (engine.h's DynRenderBuffer). Its allocations name the header
// "GoShaderObjectCommon_DynamicRenderingBuffer_Gc.h"; the .c file that holds this code is not known.
// The file ends with three GX index writes the compiler did not inline (0x80070B50..0x80070B74).

#include "game_types.h"
#include "engine.h"
#include "golfer.h"
#include "gx.h"
#include "core/startup.h"
#include "shaderdata.h"

// The header's __FILE__, which the allocations name.
#define DRB_FILE "GoShaderObjectCommon_DynamicRenderingBuffer_Gc.h"

void fn_80012520(u32 ePrim, u32 eFormat, u16 nVerts);  // LLFont.c: GXBegin
void fn_800124A8(void);                                // LLFont.c: end the primitive
void fn_80070B50(u16 nIndex);
void fn_80070B5C(u16 nIndex);
void fn_80070B68(u16 nIndex);

DynRenderBuffer* fn_8007018C(int nMaxVerts, int nMaxDraws) {
    DynRenderBuffer* pBuf;

    pBuf = fn_80009B34(sizeof(DynRenderBuffer), 2, 32, DRB_FILE, 163);
    // room for 1.6 indices per vertex
    pBuf->pIndices = fn_80009B34((int)(1.6f * (nMaxVerts * sizeof(u16))), 2, 32, DRB_FILE, 173);
    pBuf->apPos[0] = fn_80009B34(nMaxVerts * 12, 2, 32, DRB_FILE, 181);
    pBuf->apPos[1] = fn_80009B34(nMaxVerts * 12, 2, 32, DRB_FILE, 188);
    pBuf->apColour[0] = fn_80009B34(nMaxVerts * 4, 2, 32, DRB_FILE, 195);
    pBuf->apColour[1] = fn_80009B34(nMaxVerts * 4, 2, 32, DRB_FILE, 202);
    pBuf->apTexCoord[0] = fn_80009B34(nMaxVerts * 8, 2, 32, DRB_FILE, 209);
    pBuf->apTexCoord[1] = fn_80009B34(nMaxVerts * 8, 2, 32, DRB_FILE, 216);
    pBuf->pDraws = fn_80009B34(nMaxDraws * sizeof(DynRenderDraw) + 4, 2, 32, DRB_FILE, 223);
    pBuf->nMaxVerts = nMaxVerts;
    pBuf->nMaxDraws = nMaxDraws;
    pBuf->nBuffer = 0;
    pBuf->nIndices = 0;
    pBuf->nVerts = 0;
    return pBuf;
}

void fn_80070348(DynRenderBuffer* pBuf) {
    fn_80009E70(pBuf->pDraws);
    fn_80009E70(pBuf->apTexCoord[1]);
    fn_80009E70(pBuf->apTexCoord[0]);
    fn_80009E70(pBuf->apColour[1]);
    fn_80009E70(pBuf->apColour[0]);
    fn_80009E70(pBuf->apPos[1]);
    fn_80009E70(pBuf->apPos[0]);
    fn_80009E70(pBuf->pIndices);
    fn_80009E70(pBuf);
}

// Draw nCount of the buffer's indices from nStart as one primitive, from the vertex arrays last
// filled.
void fn_800703B8(DynRenderBuffer* pBuf, u32 nStart, u16 nCount, int nPrim) {
    u32 i;
    u8 nBuffer = pBuf->nBuffer;

    GXInvalidateVtxCache();
    GXSetArray(9, pBuf->apPos[nBuffer], 12);          // GX_VA_POS
    GXSetArray(13, pBuf->apTexCoord[nBuffer], 8);     // GX_VA_TEX0
    GXSetArray(11, pBuf->apColour[nBuffer], 4);       // GX_VA_CLR0
    GXClearVtxDesc();
    GXSetVtxDesc(9, 3);                                // 16-bit indices
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
void fn_800704C4(DynRenderBuffer* pBuf, const void* pPos, const void* pColour, const void* pTexCoord,
                 int nVerts, u8 bFlip) {
    u8* pDst;
    u32 uSize;
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
void fn_800705F0(DynRenderBuffer* pBuf, u16* pIndices, u32 nCount, u8 bRestart) {
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
void fn_80070764(DynRenderBuffer* pBuf, const DynRenderDrawIn* pIn, u16 nCount, int nPrim, u8 bRestart) {
    DynRenderDrawList* pList = pBuf->pDraws;
    DynRenderDraw* pDraw;
    u32 nOld;
    u32 i;

    if (bRestart) {
        if (pIn == NULL) {
            pList->nDraws = 1;
            pList->aDraws[0].nStart = 0;
            pList->aDraws[0].nCount = nCount;
            pList->aDraws[0].nPrim = nPrim;
            return;
        }
        pList->nDraws = nCount;
        pDraw = pList->aDraws;
        for (i = 0; i < nCount; i++) {
            pDraw->nStart = pIn[i].nStart;
            pDraw->nCount = pIn[i].nCount;
            pDraw->nPrim = pIn[i].nPrim;
            pDraw++;
        }
    } else {
        if (pIn == NULL) {
            nOld = pList->nDraws;
            pList->nDraws = nOld + 1;
            pDraw = pList->aDraws;
            for (i = 0; i < nOld; i++) {
                pDraw++;
            }
            pDraw->nStart = pBuf->nIndices;
            pDraw->nCount = nCount;
            pDraw->nPrim = nPrim;
            return;
        }
        nOld = pList->nDraws;
        pList->nDraws = nOld + nCount;
        pDraw = pList->aDraws;
        for (i = 0; i < nOld; i++) {
            pDraw++;
        }
        for (i = 0; i < nCount; i++) {
            pDraw->nStart = pBuf->nIndices + pIn[i].nStart;
            pDraw->nCount = pIn[i].nCount;
            pDraw->nPrim = pIn[i].nPrim;
            pDraw++;
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
void fn_80070168(void);     // sweep_80070168.c: calls a display list
void fn_8007524C(void);     // GoShaderObjectCommon_ShaderObjectsData_Gc.c: empty
s32 fn_80097688(void);      // GoShaderObjectCommon_MorphAnimManager_Gc.c: how many animations
void fn_80097624(MorphAnim* pAnim);     // GoShaderObjectCommon_MorphAnimManager_Gc.c: free its data

// Row 7's data hook: nothing.
void fn_80070B74(void) {
}

// Row 7's static init: build the object's display list from pCmds.
void fn_80070B78(StaticShaderObject* pObj, ShaderCmds* pCmds) {
    fn_80074DA8(pObj->pArrays, pObj->eType, &pObj->anim, pCmds);
}

// Row 7's static close.
void fn_80070BAC(StaticShaderObject* pObj) {
    // port: EA passes an argument fn_8007524C ignores
    ((void (*)(MorphAnim*))fn_8007524C)(&pObj->anim);
}

// Row 7's static render: the display list (with the object's positions when the list sets none).
void fn_80070BD0(StaticShaderObject* pObj) {
    if (pObj->anim.b8 != 0) {
        GXSetArray(9, pObj->pArrays->apPos[0], 12);
    }
    // port: fn_80070168 (sweep_80070168.c) is defined without parameters but hands r3 and r4
    // on to GXCallDisplayList
    ((void (*)(void*, u32))fn_80070168)(pObj->anim.p4, pObj->anim.n0);
}

// Row 8's static init.
void fn_80070C24(StaticShaderObject* pObj, ShaderCmds* pCmds) {
    fn_80074DA8(pObj->pArrays, pObj->eType, &pObj->anim, pCmds);
}

// Row 8's static close.
void fn_80070C58(StaticShaderObject* pObj) {
    // port: EA passes an argument fn_8007524C ignores
    ((void (*)(MorphAnim*))fn_8007524C)(&pObj->anim);
}

// Row 8's static render.
void fn_80070C7C(StaticShaderObject* pObj) {
    if (pObj->anim.b8 != 0) {
        GXSetArray(9, pObj->pArrays->apPos[0], 12);
    }
    // port: see fn_80070BD0
    ((void (*)(void*, u32))fn_80070168)(pObj->anim.p4, pObj->anim.n0);
}

// Row 1's static init.
void fn_80070CD0(StaticShaderObject* pObj, ShaderCmds* pCmds) {
    fn_80074DA8(pObj->pArrays, pObj->eType, &pObj->anim, pCmds);
}

// Row 1's static close.
void fn_80070D04(StaticShaderObject* pObj) {
    // port: EA passes an argument fn_8007524C ignores
    ((void (*)(MorphAnim*))fn_8007524C)(&pObj->anim);
}

// Row 1's static render.
void fn_80070D28(StaticShaderObject* pObj) {
    if (pObj->anim.b8 != 0) {
        GXSetArray(9, pObj->pArrays->apPos[0], 12);
    }
    // port: see fn_80070BD0
    ((void (*)(void*, u32))fn_80070168)(pObj->anim.p4, pObj->anim.n0);
}

// Row 3's data hook: the morph weight.
void fn_80070D7C(f32* pfWeight) {
    lbl_80281E70 = *pfWeight;
}

// Row 3's static init.
void fn_80070D88(StaticShaderObject* pObj, ShaderCmds* pCmds) {
    fn_80074DA8(pObj->pArrays, pObj->eType, &pObj->anim, pCmds);
}

// Row 3's static close: free the morph animation's data.
void fn_80070DBC(StaticShaderObject* pObj) {
    fn_80097624(&pObj->anim);
    // port: EA passes an argument fn_8007524C ignores
    ((void (*)(MorphAnim*))fn_8007524C)(&pObj->anim);
}

// Row 3's static render: the display list, with the morph animation brought up to date first once
// in every three frames, each animation on its own frame of the cycle.
void fn_80070DF4(StaticShaderObject* pObj) {
    MorphAnim* pAnim = &pObj->anim;

    if (pObj->anim.b8 != 0) {
        GXSetArray(9, pObj->pArrays->apPos[0], 12);
    }
    if (fn_80097688() != 0 && (u32)gSession.nFrameCount % 3 == pAnim->nIndex % 3 &&
        (u32)gSession.nFrameCount > fn_80097694(pAnim->nIndex)) {
        fn_800976A8(pAnim, lbl_80281E70);
    }
    // port: fn_80070168 (sweep_80070168.c) is defined without parameters but hands r3 and r4
    // on to GXCallDisplayList
    ((void (*)(void*, u32))fn_80070168)(pAnim->p4, pAnim->n0);
}
