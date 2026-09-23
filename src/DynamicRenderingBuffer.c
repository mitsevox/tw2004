// DynamicRenderingBuffer.c (our name): the dynamic rendering buffer, vertices a shader object
// rewrites every frame (engine.h's DynRenderBuffer). Its allocations name the header
// "GoShaderObjectCommon_DynamicRenderingBuffer_Gc.h"; the .c file that holds this code is not known.
// The file ends with three GX index writes the compiler did not inline (0x80070B50..0x80070B74).

#include "game_types.h"
#include "engine.h"
#include "gx.h"
#include "core/startup.h"

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

// ---- sweep code (not yet cleaned up) ----

void fn_80070B74(void);
void fn_80074DA8();
void fn_8007524C();
void fn_80070B78(u8* p0, s32 p1);
void fn_80070BAC(u8* p0);
s32 fn_80070168(s32, s32);
void fn_80070BD0(void* arg0);
void fn_80070C24(u8* p0, s32 p1);
void fn_80070C58(u8* p0);
void fn_80070C7C(void* arg0);
void fn_80070CD0(u8* p0, s32 p1);
void fn_80070D04(u8* p0);
void fn_80070D28(void* arg0);
extern f32 lbl_80281E70;
void fn_80070D7C(u8* p0);
void fn_80070D88(u8* p0, s32 p1);
void fn_80097624();
void fn_80070DBC(u8* p0);

void fn_80070B74(void) {
}

void fn_80070B78(u8* p0, s32 p1) {
    fn_80074DA8(*(s32*)(p0 + 0x24), *(s32*)(p0 + 0x0), (p0 + 0x4), p1);
}

void fn_80070BAC(u8* p0) {
    fn_8007524C((p0 + 0x4));
}

void fn_80070BD0(void* arg0) {
    if ((u8) (*(u8*)((u8*)(arg0) + 0xC)) != 0) {
        GXSetArray(9, *(*(void***)((u8*)(arg0) + 0x24)), 0xC);
    }
    fn_80070168((*(s32*)((u8*)(arg0) + 8)), (*(s32*)((u8*)(arg0) + 4)));
}

void fn_80070C24(u8* p0, s32 p1) {
    fn_80074DA8(*(s32*)(p0 + 0x24), *(s32*)(p0 + 0x0), (p0 + 0x4), p1);
}

void fn_80070C58(u8* p0) {
    fn_8007524C((p0 + 0x4));
}

void fn_80070C7C(void* arg0) {
    if ((u8) (*(u8*)((u8*)(arg0) + 0xC)) != 0) {
        GXSetArray(9, *(*(void***)((u8*)(arg0) + 0x24)), 0xC);
    }
    fn_80070168((*(s32*)((u8*)(arg0) + 8)), (*(s32*)((u8*)(arg0) + 4)));
}

void fn_80070CD0(u8* p0, s32 p1) {
    fn_80074DA8(*(s32*)(p0 + 0x24), *(s32*)(p0 + 0x0), (p0 + 0x4), p1);
}

void fn_80070D04(u8* p0) {
    fn_8007524C((p0 + 0x4));
}

void fn_80070D28(void* arg0) {
    if ((u8) (*(u8*)((u8*)(arg0) + 0xC)) != 0) {
        GXSetArray(9, *(*(void***)((u8*)(arg0) + 0x24)), 0xC);
    }
    fn_80070168((*(s32*)((u8*)(arg0) + 8)), (*(s32*)((u8*)(arg0) + 4)));
}

void fn_80070D7C(u8* p0) {
    lbl_80281E70 = *(f32*)p0;
}

void fn_80070D88(u8* p0, s32 p1) {
    fn_80074DA8(*(s32*)(p0 + 0x24), *(s32*)(p0 + 0x0), (p0 + 0x4), p1);
}

void fn_80070DBC(u8* p0) {
    fn_80097624((p0 + 0x4));
    fn_8007524C((p0 + 0x4));
}

// ---- end of sweep code ----
