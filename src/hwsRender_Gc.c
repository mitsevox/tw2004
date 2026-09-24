// hwsRender_Gc.c (EA's name, from its asserts): the GameCube renderer's setup: a screen buffer
// sized for one or two views, and the state (lbl_80223BB0) the skin drawing reads.

#include "engine.h"
#include "charstate.h"
#include "golfer.h"
#include "game.h"
#include "gx.h"
#include "core/startup.h"
#include "unsorted/cull.h"

void fn_80036460(int n);                // Skin.c
void PostFx_CopyScreenToBuffer(void);   // gomainloop.c
void fn_8011EB80(void);
void fn_80112B34(void);                 // hwsOverride_Gc.c
void fn_80112DD8(s32 nEntry);
void fn_800BADB4(f32 (*pMtx)[4], f32* pIn, f32* pOut);  // VecMath.c
void fn_80113BCC(SkinIter* pIter);
void fn_80113C70(SkinIter* pIter);
void fn_80113D28(SkinIter* pIter);

// The step functions of the three iterators (fn_80113910, fn_80113A9C, fn_80113B34).
void (*lbl_802817E8[1])(SkinIter* pIter) = { fn_80113BCC };
void (*lbl_802817EC[1])(SkinIter* pIter) = { fn_80113C70 };
void (*lbl_802817F0[2])(SkinIter* pIter) = { fn_80113D28, NULL };

void* lbl_802824E0;                     // the screen buffer (fn_80112C64)
u32 lbl_802824E4;                       // the next free offset in it (fn_801132C4)
int lbl_802824E8;                       // set: fn_801132C4 skins the next mesh's vertices again
u32 lbl_802824EC;                       // the buffer's size
void* lbl_802824F0;                     // the vertices fn_801132C4 last wrote

// Whether the extra pass runs: one view, at most three players (two on course 14's hole 11).
u8 fn_80112B80(void) {
    int nHole = fn_80015464();

    if (Game_GetCourse() == 14 && nHole == 11 && gSession.nNumPlayers > 2) {
        return 0;
    }
    if (gSession.nNumPlayers > 3) {
        return 0;
    }
    return gSession.nSplitScreen == 0;
}

// The same limit for course 14's hole 11 alone: at most three players.
int fn_80112C04(void) {
    int nHole = fn_80015464();

    if (Game_GetCourse() == 14 && nHole == 11 && gSession.nNumPlayers > 3) {
        return 0;
    }
    return 1;
}

// Allocate the screen buffer: bigger for split screen.
void fn_80112C64(int bSplit) {
    if (bSplit == 0) {
        lbl_802824E0 = fn_80009B34(0x30C00, 2, 32, "hwsRender_Gc.c", 266);
        lbl_802824EC = 0x30C00;
    } else {
        lbl_802824E0 = fn_80009B34(0x5DC00, 2, 32, "hwsRender_Gc.c", 271);
        lbl_802824EC = 0x5DC00;
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80112CEC(void);
s32 fn_80036464();
s32 fn_80037F80();
s32 fn_8011EBF8();
void fn_80112DA0(void);

void fn_80112CEC(void) {
    if (lbl_802824E0 != NULL) {
        fn_80009E70(lbl_802824E0);
        lbl_802824E0 = NULL;
    }
}

// ---- end of sweep code ----

void fn_80112D20(void) {
    if (gSession.nSplitScreen) {
        fn_80112C64(1);
    } else {
        fn_80112C64(0);
    }
    if (fn_80112B80()) {
        fn_8011EB80();
    }
    PostFx_CopyScreenToBuffer();
    if (gSession.nSplitScreen) {
        fn_80036460(3600);
        return;
    }
    fn_80036460(1800);
}

// ---- sweep code (not yet cleaned up) ----

void fn_80112DA0(void) {
    fn_80112CEC();
    if (fn_80112B80() != 0) {
        fn_8011EBF8();
    }
    fn_80037F80();
    fn_80036464();
}

// ---- end of sweep code ----

// ---- sweep code (not yet cleaned up) ----

u32* fn_80113764(void);
void fn_80113840(void);
void fn_80113844(void);
void fn_8011387C(void);
void fn_8011389C(void);
void fn_801138D8(HwsOverrideTable* pTable);
void fn_801138E8(void);
void fn_801138EC(s32 p0);
void fn_801138F8(s32 p0);
void fn_80113904(s32 p0);
void fn_80113B14(SkinIter* pIter);
void fn_80113E54(SkinIter* pIter, void (**ppfnNext)(SkinIter* pIter));
void fn_80113E5C(SkinIter* pIter);

// ---- end of sweep code ----

// Draw a mesh's triangle strip. When lbl_802824E8 is set, its vertices are first skinned into the
// screen buffer (positions as floats, normals as bytes) and set up as GX's vertex arrays.
void fn_801132C4(SkinMeshRefs* pRefs) {
    Vec4 vNormalIn;
    Vec4 vPosIn;
    Vec4 vPosOut;
    Vec4 vNormalOut;
    s16* pPos;
    s8* pNormal;
    f32* pPosOut;
    s8* pNormalOut;
    f32 (*pMatrix)[4];
    u32 i;
    f32 (*pMatrices)[4][4];
    u32 uSize;
    f32* pPosBuf;
    s8* pNormalBuf;
    int n;
    void* pBuf;
    s32 n10;
    s16* pVerts;
    void* pTexCoords;
    u16* pIndices;
    u16 nIndex;

    if (lbl_802824E8 != 0) {
        lbl_802824E8 = 0;
        // Room for n10 positions (12 bytes) and normals (3 bytes), in whole 32-byte blocks.
        uSize = pRefs->n10 * 15;
        uSize += 32 - (uSize & 31);
        if (lbl_802824E4 + uSize >= lbl_802824EC) {
            lbl_802824E4 = 0;
        }
        pBuf = (u8*)lbl_802824E0 + lbl_802824E4;
        lbl_802824E4 += uSize;
        pPosBuf = pBuf;
        lbl_802824F0 = pBuf;
        pPosOut = pBuf;
        n10 = pRefs->n10;
        pVerts = pRefs->p14;
        pMatrices = pRefs->pC;
        pPos = pVerts;
        pNormal = (s8*)(pVerts + n10 * 4);
        pNormalBuf = (s8*)pBuf + n10 * 12;
        pNormalOut = pNormalBuf;
        for (i = 0; i < pRefs->n10; pPos += 4, pNormal += 4, i++) {
            // Each position is x, y, z and the index of its matrix.
            pMatrix = pMatrices[(u16)pPos[3]];
            vPosIn.x = pPos[0] / 16384.0f;
            vPosIn.y = pPos[1] / 16384.0f;
            vPosIn.z = pPos[2] / 16384.0f;
            vPosIn.w = 1.0f;
            vPosIn.x *= pRefs->f20;
            vPosIn.y *= pRefs->f20;
            vPosIn.z *= pRefs->f20;
            vNormalIn.x = pNormal[0] / 64.0f;
            vNormalIn.y = pNormal[1] / 64.0f;
            vNormalIn.z = pNormal[2] / 64.0f;
            vNormalIn.w = 1.0f;
            fn_800BAD60(pMatrix, &vPosIn, &vPosOut);
            fn_800BADB4(pMatrix, &vNormalIn.x, &vNormalOut.x);
            pPosOut[0] = vPosOut.x;
            pPosOut[1] = vPosOut.y;
            pPosOut[2] = vPosOut.z;
            pPosOut += 3;
            pNormalOut[0] = 64.0f * vNormalOut.x;
            pNormalOut[1] = 64.0f * vNormalOut.y;
            pNormalOut[2] = 64.0f * vNormalOut.z;
            pNormalOut += 3;
        }
        DCFlushRange(lbl_802824F0, uSize);
        pTexCoords = pRefs->p1C;
        GXInvalidateVtxCache();
        GXSetVtxAttrFmt(1, 9, 1, 4, 0);         // positions: xyz floats
        GXSetVtxAttrFmt(1, 10, 0, 1, 6);        // normals: s8, 6 fraction bits
        GXSetVtxAttrFmt(1, 13, 1, 3, 12);       // texture coordinates: s16 pairs, 12 fraction bits
        GXSetArray(9, pPosBuf, 12);
        GXSetArray(10, pNormalBuf, 3);
        GXSetArray(13, pTexCoords, 4);
        GXClearVtxDesc();
        GXSetVtxDesc(9, 3);                     // all three by 16-bit index
        GXSetVtxDesc(10, 3);
        GXSetVtxDesc(13, 3);
    }
    pIndices = pRefs->p4;
    GXBegin(0x98, 1, pRefs->n2);                // a triangle strip
    for (n = 0; n < pRefs->n2; n++) {
        nIndex = pIndices[n + pRefs->n0];
        fn_80113904(nIndex);
        fn_801138F8(nIndex);
        fn_801138EC(nIndex);
    }
    fn_801138E8();
}

void fn_801136C4(SkinMesh* pMesh, void* pData, SkinMeshRefs* pOut, u16 n0, s16 n2);

// fn_801136C4 with the mesh's own data.
void fn_8011368C(SkinMesh* pMesh, SkinMeshRefs* pOut, u16 n0, s16 n2) {
    fn_801136C4(pMesh, pMesh->pBits, pOut, n0, n2);
}

// Point pOut at pData by the mesh's flags (nothing for an empty mesh).
void fn_801136C4(SkinMesh* pMesh, void* pData, SkinMeshRefs* pOut, u16 n0, s16 n2) {
    if (pMesh->n8 == 0) {
        return;
    }
    if (pMesh->uFlags & 1) {
        if (pMesh->uFlags & 0x10) {
            pOut->n10 = pMesh->n8;
            pOut->p14 = pData;
            if (pMesh->uFlags & 0x40) {
                // The third part follows n10 bits (8 bytes each) and n10 words.
                pOut->p1C = (u8*)pMesh->pBits + (pOut->n10 * 4 + pOut->n10 * 8);
                pOut->n18 = pMesh->n8;
            }
        } else if (pMesh->uFlags & 0x40) {
            pOut->n18 = pMesh->n8;
            pOut->p1C = pMesh->pBits;
        }
    } else if (pMesh->uFlags & 2) {
        pOut->n0 = n0;
        pOut->n2 = n2;
        pOut->p4 = pData;
    } else if (pMesh->uFlags & 0x200000) {
        pOut->n8 = pMesh->n8;
        pOut->pC = pData;
    }
}

// ---- sweep code (not yet cleaned up) ----

u32* fn_80113764(void) {
    return lbl_80223BB0.s10.p48;
}

// ---- end of sweep code ----

// fn_80112DD8 on each SkinDesc.p6C entry of option nOption of variant nVariant of part nPart of
// the description being drawn (nothing when the part or variant does not exist).
void fn_80113774(int nPart, int nVariant, int nOption) {
    SkinDesc* pDesc = lbl_80223BB0.pDesc;
    SkinPartDef* pPart;
    SkinDesc5C* pOption;
    s32 nCount;
    s32 nFirst;
    int i;

    if (nPart >= pDesc->nParts) {
        return;
    }
    pPart = &pDesc->pParts[nPart];
    if (nVariant >= pPart->nVariants) {
        return;
    }
    pOption = &pDesc->p5C[nOption + pDesc->pVariants[nVariant + pPart->nFirst].nFirstOption];
    nCount = pOption->n0;
    nFirst = pOption->n4;
    for (i = 0; i < nCount; i++) {
        fn_80112DD8(lbl_80223BB0.pDesc->p6C[nFirst + i]);
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80113840(void) {
}

// ---- end of sweep code ----

// Clear the renderer's state.
void fn_80113844(void) {
    lbl_80223BB0.pDesc = NULL;
    lbl_80223BB0.n4 = 0;
    lbl_80223BB0.pOverride = NULL;
    lbl_80223BB0.n8 = 0;
    fn_80112B34();
}

// ---- sweep code (not yet cleaned up) ----

void fn_8011387C(void) {
    fn_80113844();
}

void fn_8011389C(void) {
    fn_80113840();
    lbl_80223BB0.s10.n50 = 0;
}

// ---- end of sweep code ----

void fn_801138CC(SkinDesc* pDesc) {
    lbl_80223BB0.pDesc = pDesc;
}

// ---- sweep code (not yet cleaned up) ----

// Make pTable the current mesh overrides.
void fn_801138D8(HwsOverrideTable* pTable) {
    lbl_80223BB0.pOverride = pTable;
}

void fn_801138E8(void) {
}

void fn_801138EC(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_801138F8(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

void fn_80113904(s32 p0) {
    *(volatile u16*)0xCC008000 = p0;
}

// ---- end of sweep code ----

// An iterator in pBuf over the SkinDesc.p34 meshes of pArgs's SkinDesc.p44 entry (first step
// taken). It takes as many steps as the n1s of the entry's SkinDesc28 add up to.
SkinIter* fn_80113910(u8* pBuf, SkinIterArgs* pArgs) {
    SkinMeshIter* pIter = (SkinMeshIter*)pBuf;
    SkinDesc28* p28;
    s32 nCount;
    int i;

    fn_80113E54(&pIter->iter, lbl_802817E8);
    pIter->pDesc = pArgs->pDesc;
    pIter->pEntry = &pIter->pDesc->p44[pArgs->n];
    if (pIter->pEntry->nC >= 0) {
        p28 = &pIter->pDesc->p28[pIter->pEntry->nC];
        nCount = 0;
        for (i = 0; i < p28->n0; i++) {
            nCount += p28->a8[i].n1;
        }
    } else {
        nCount = 1;
    }
    pIter->nCount = nCount;
    pIter->n1C = -1;
    fn_800CEEC8(&pIter->iter);
    return &pIter->iter;
}

// ---- sweep code (not yet cleaned up) ----

void fn_80113A7C(SkinIter* pIter) {
    fn_80113E5C(pIter);
}

// ---- end of sweep code ----

// An iterator in pBuf over the meshes of pArgs's SkinDesc.p5C entry (first step taken).
SkinIter* fn_80113A9C(u8* pBuf, SkinIterArgs* pArgs) {
    SkinDescIter* pIter = (SkinDescIter*)pBuf;

    fn_80113E54(&pIter->iter, lbl_802817EC);
    pIter->pDesc = pArgs->pDesc;
    pIter->pEntry = &pArgs->pDesc->p5C[pArgs->n];
    pIter->pSub = NULL;
    pIter->n18 = -1;
    fn_800CEEC8(&pIter->iter);
    return &pIter->iter;
}

// ---- sweep code (not yet cleaned up) ----

void fn_80113B14(SkinIter* pIter) {
    fn_80113E5C(pIter);
}

// ---- end of sweep code ----

// The same with the other step function.
SkinIter* fn_80113B34(u8* pBuf, SkinIterArgs* pArgs) {
    SkinDescIter* pIter = (SkinDescIter*)pBuf;

    fn_80113E54(&pIter->iter, lbl_802817F0);
    pIter->pDesc = pArgs->pDesc;
    pIter->pEntry = &pArgs->pDesc->p5C[pArgs->n];
    pIter->pSub = NULL;
    pIter->n18 = -1;
    fn_800CEEC8(&pIter->iter);
    return &pIter->iter;
}

// ---- sweep code (not yet cleaned up) ----

void fn_80113BAC(SkinIter* pIter) {
    fn_80113E5C(pIter);
}

// ---- end of sweep code ----

// fn_80113910's step: the next of the entry's SkinDesc.p3C entries that has a mesh.
void fn_80113BCC(SkinIter* pIter) {
    SkinMeshIter* p = (pIter->ppfnNext == lbl_802817E8) ? (SkinMeshIter*)pIter : NULL;

    do {
        p->n1C++;
        p->iter.bValid = p->n1C < p->nCount;
        if (!p->iter.bValid) {
            return;
        }
        p->iter.nCur = p->pDesc->p3C[p->pEntry->n0 + p->n1C];
        if (p->iter.nCur >= 0) {
            p->iter.pCur = &p->pDesc->p34[p->iter.nCur];
        }
    } while (p->iter.nCur < 0);
}

// fn_80113A9C's step: the next of the entry's SkinDesc.p6C entries that is not -1. Its pCur is a
// SkinDesc.p44 entry, not a mesh (the callers know which iterator they made).
void fn_80113C70(SkinIter* pIter) {
    SkinDescIter* p = (pIter->ppfnNext == lbl_802817EC) ? (SkinDescIter*)pIter : NULL;

    do {
        p->n18++;
        p->iter.bValid = p->n18 < p->pEntry->n0;
        if (p->iter.bValid) {
            p->iter.nCur = p->pDesc->p6C[p->pEntry->n4 + p->n18];
            p->iter.pCur = (SkinMesh*)&p->pDesc->p44[p->iter.nCur];
            if (p->iter.nCur >= 0) {
                return;
            }
        }
    } while (p->iter.bValid && p->iter.nCur < 0);
}

// fn_80113B34's step: the next mesh of the current entry's iterator, else of the next entry's
// SkinDesc.p44 entry that has any.
void fn_80113D28(SkinIter* pIter) {
    SkinDescIter* p = (pIter->ppfnNext == lbl_802817F0) ? (SkinDescIter*)pIter : NULL;
    SkinIterArgs args;

    if (p->pSub != NULL) {
        fn_800CEEC8(p->pSub);
        if (!fn_800CEEC0(p->pSub)) {
            fn_80113A7C(p->pSub);
            p->pSub = NULL;
        }
    }
    while (p->pSub == NULL) {
        p->n18++;
        if (p->n18 >= p->pEntry->n0) {
            break;
        }
        args.pDesc = p->pDesc;
        args.n = p->pDesc->p6C[p->pEntry->n4 + p->n18];
        if (args.n >= 0) {
            p->pSub = fn_80113910((u8*)&p->sub, &args);
            if (!fn_800CEEC0(p->pSub)) {
                fn_80113A7C(p->pSub);
                p->pSub = NULL;
            }
        }
    }
    p->iter.bValid = p->pSub != NULL;
    if (p->pSub != NULL) {
        p->iter.pCur = fn_800CEEF4(p->pSub);
        p->iter.nCur = fn_800CEEFC(p->pSub);
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_80113E54(SkinIter* pIter, void (**ppfnNext)(SkinIter* pIter)) {
    pIter->ppfnNext = ppfnNext;
}

void fn_80113E5C(SkinIter* pIter) {
}

// ---- end of sweep code ----
