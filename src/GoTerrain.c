// GoTerrain.c (EA's name, from its asserts; also in EA's 2002 source tree): the terrain renderer
// (Ter_TerrainRendererMgr, terrain.h): loads the hole's ground and objects from the 'ter ', 'tgd '
// and 'tLOD' chunks, sorts its patches and objects into draw lists by distance and level of
// detail, animates the objects (trees, the crowd, the flag) and draws them. The file ends with
// small setters of the renderer's state (RenderState), which share its constant pool.

#include "golfer.h"
#include "ball.h"
#include "game.h"
#include "engine.h"
#include "game_types.h"
#include "lighting.h"
#include "camera.h"

void* fn_800073B4(u8* pData, int n);
void  fn_800075CC(void* p);         // frees what fn_800073B4 made
void  fn_80019358(Character* pChar, f32* pDir, f32 f);
void  fn_8001BE88(Character* pChar, void* pClip, int n, f32 f);
void  fn_800F199C(f32 x, f32 y, f32 z);
void  fn_80030894(void);
void  fn_80030A40(void* p, int n);
f32   fn_800351D8(u32 n, f32 fPeriod);
void  fn_8003519C(int nRow, void* pData);   // calls row nRow's function of lbl_80188E88 with pData
s32   fn_800318AC(const void* pA, const void* pB);
void  fn_8003272C(int n);
void  fn_80031938(Ter_LODPlane* pPlanes, f32 fStep, s32 a, s32 b, s32 c, s32 d);
void  fn_80032B7C(void* pGround, s32 eClipMethod, s32 nPass, s32 n1C, s32 n18, s32 n20, u8* pbFirst, u8 b1,
                  u8 b2, f32 fNear, f32 fFar);
void  fn_80032F88(Ter_ObjectDrawData* pList, s32 nCount, s32 eFilterMin, s32 eFilterMag);
void  fn_800341A4(UStreamObject* pObject);
void  fn_800342B4(UStreamObject* pObject);
void  fn_800342F0(UStreamObject* pObject);
void  fn_80035098(u8 b);

// ---- sweep code (not yet cleaned up) ----

s32 fn_800354BC(s32);
void* fn_800354C4();
s32 fn_800354D0(s32, s32);
s32 fn_800354E4(s32, s32);
s32 fn_800354F4(s32);

void fn_80031084(s32 arg0, s32 arg1, s32 arg2, void* arg3, f32 farg0) {
    s32 temp_r31;

    (*(f32*)((u8*)(arg3) + 8)) = farg0;
    (*(f32*)((u8*)(arg3) + 0xC)) = (f32) (*(f32*)((u8*)(fn_800354C4()) + 0xC));
    (*(s32*)((u8*)(arg3) + 0x10)) = arg1;
    (*(s32*)((u8*)(arg3) + 0x14)) = arg2;
    temp_r31 = fn_800354F4(arg0);
    (*(s32*)((u8*)(arg3) + 0)) = fn_800354E4(arg0, 0);
    (*(s32*)((u8*)(arg3) + 0x18)) = fn_800354D0((*(s32*)((u8*)(arg3) + 0)), 3);
    (*(s32*)((u8*)(arg3) + 0x1C)) = fn_800354D0((*(s32*)((u8*)(arg3) + 0)), 2);
    (*(s32*)((u8*)(arg3) + 0x20)) = fn_800354D0(arg0, 1);
    if (temp_r31 >= 2) {
        (*(s32*)((u8*)(arg3) + 4)) = fn_800354BC((*(s32*)((u8*)(arg3) + 0)));
        return;
    }
    (*(s32*)((u8*)(arg3) + 4)) = 0;
}

// ---- end of sweep code ----

void fn_800306B8(void) {
    fn_80009E70(lbl_801D3CB0.pPatchList);
    fn_80009E70(lbl_801D3CB0.pPostDrawTerrainList);
    fn_80009E70(lbl_801D3CB0.pObjectSortList);
    fn_80009E70(lbl_801D3CB0.pOpaqueObjectList);
    fn_80009E70(lbl_801D3CB0.pTranslucentObjectList);
    fn_80009E70(lbl_801D3CB0.pNearbyObjectList);
    fn_80009E70(lbl_801D3CB0.pDeferredItemsList);
    fn_80009E70(lbl_801D3CB0.pPanoramaItemsList);
    fn_80009E70(lbl_801D3CB0.pPostDrawItemsList);
    fn_80009E70(lbl_801D3CB0.pObjectStateList);
    if (lbl_801D3CB0.pCurrentHoleData != NULL) {
        fn_800075CC(lbl_801D3CB0.pCurrentHoleData);
        lbl_801D3CB0.pCurrentHoleData = NULL;
    }
    if (lbl_801D3CB0.pCourse != NULL) {
        fn_80009E70(lbl_801D3CB0.pCourse);
        lbl_801D3CB0.pCourse = NULL;
    }
}

// The terrain's chunk loaders.
void fn_800307C0(void) {
    UStream_RegisterHandler('ter ', fn_800342B4);
    UStream_RegisterHandler('tgd ', fn_800342F0);
    UStream_RegisterHandler('tLOD', fn_800341A4);
}

void fn_80030818(void) {
    UStream_UnregisterHandler('ter ');
    UStream_UnregisterHandler('tgd ');
}

void fn_8003084C(void) {
    fn_80035098(0);
    fn_80012F50(1, 6, 128);
    fn_80012F18(3);
    fn_80012F34(1);
    fn_80012EF8();
}

// Sets the renderer up for the terrain, then hands rows 4 and 5 of lbl_80188E88 the frame count,
// row 4 with four waves between 0 and 1 whose cycles are 1591.2 x (5.5 + i) / 1000 seconds.
void fn_80030894(void) {
    TerWaveData wave;
    u32 nFrame;
    int i;

    fn_80035240(NULL);
    fn_80016B9C();
    fn_80016B9C();
    fn_80016B9C();
    fn_80035118(4, 5);
    fn_80012F50(1, 6, 1);
    fn_80012F18(3);
    fn_80035338(2);
    fn_80035308();
    fn_800352E4();
    fn_80014118(0x70);
    fn_80013EEC(fn_8001614C());
    fn_80012EF8();
    for (i = 0; i < 4; i++) {
        // fake match: the original's 1591.2 is one bit above the literal 1591.2f, as a folded float
        // product gives it; 26.52 x 60 is one such product (3 x 530.4 and 12 x 132.6 are others)
        f32 fPeriod = 26.52f * 60.0f * (5.5f + (f32)i) / 1000.0f;

        wave.aWave[i] = 0.5f * fn_800095F0(6.2831855f * fn_800351D8(gSession.nFrameCount, fPeriod) / fPeriod)
                        + 0.5f;
    }
    wave.nFrame = gSession.nFrameCount;
    fn_8003519C(4, &wave);
    nFrame = gSession.nFrameCount;
    fn_8003519C(5, &nFrame);
}

// Sorts pObjectSortList by distance, except when gSession.b11 is set.
void fn_8003185C(void) {
    if (gSession.b11 == 0) {
        qsort(lbl_801D3CB0.pObjectSortList, lbl_801D3CB0.iTotalSortObjects, sizeof(Ter_ObjectReference),
              fn_800318AC);
    }
}

// fn_8003185C's comparison: nearest first.
s32 fn_800318AC(const void* pA, const void* pB) {
    f32 fA = ((const Ter_ObjectReference*)pA)->fDistanceSquared;
    f32 fB = ((const Ter_ObjectReference*)pB)->fDistanceSquared;

    if (fA > fB) return 1;
    if (fA < fB) return -1;
    return 0;
}

void fn_800318D8(void) {
    fn_80031938(lbl_801D3CB0.LODPlanes, lbl_802810D8, lbl_802810D0, lbl_802810D4, lbl_802810DC,
                lbl_802810E0);
    lbl_801D3CB0.fDistanceCullFrameYardsSquared =
        lbl_801D3CB0.fFOVScale * lbl_801D3CB0.fDistanceCullYardsBase;
    lbl_801D3CB0.fDistanceCullFrameYardsSquared =
        lbl_801D3CB0.fDistanceCullFrameYardsSquared * lbl_801D3CB0.fDistanceCullFrameYardsSquared;
}

// The three levels of detail's ranges, in steps of fStep: LOD 0 from 0 to (a x fFOVScale + 1)
// steps, LOD 1 from c steps before that end to b steps after, LOD 2 from d steps before that on.
void fn_80031938(Ter_LODPlane* pPlanes, f32 fStep, s32 a, s32 b, s32 c, s32 d) {
    s32 n = (f32)a * lbl_801D3CB0.fFOVScale;

    pPlanes[0].fBegin = 0.0f;
    pPlanes[2].fEnd = 0.0f;
    pPlanes[0].fEnd = (f32)(n + 1) * fStep;
    pPlanes[1].fBegin = pPlanes[0].fEnd - (f32)c * fStep;
    pPlanes[1].fEnd = pPlanes[1].fBegin + (f32)b * fStep;
    pPlanes[2].fBegin = pPlanes[1].fEnd - (f32)d * fStep;
}

void fn_80031A08(s32* pA, s32* pB, s32 a, s32 b) {
    // fake match: the (s32) and (int) casts of lbl_802810D8 are two conversions (the original
    // stores the one fctiwz result twice); the same cast everywhere shares one.
    s32 n = a + lbl_802810DC * (s32)lbl_802810D8;

    *pA = (f32)(n / (int)lbl_802810D8 - 1) / lbl_801D3CB0.fFOVScale;
    *pB = lbl_802810DC + (b + lbl_802810E0 * (s32)lbl_802810D8 - n) / (int)lbl_802810D8;
}

// Whether the 'tLOD' chunk has been loaded.
u8 fn_80031E40(void) {
    return lbl_802810E4 != -1;
}

// Draws render pass nRenderPass's sorted patches, if it has any: each list (the pass a patch is
// drawn in, 0..2) one clip method at a time, then the deferred items with the terrain's filters.
void fn_80032518(int nRenderPass) {
    u8 bFirst = 1;
    u8 bAny;
    int nList;
    int nClip;
    Ter_PatchReference* pPatch;

    bAny = 0;
    for (nList = 0; nList < 3; nList++) {
        for (nClip = 0; nClip < 3; nClip++) {
            if (lbl_801D3CB0.pSortedPatchList[nRenderPass][nList][nClip] != NULL) {
                bAny = 1;
                break;
            }
        }
    }
    if (bAny) {
        if (nRenderPass == 2) {
            fn_8003272C(0);
        }
        fn_80012EF8();
        fn_80014118(0x70);
        fn_80012F50(0, 6, 1);
        for (nList = 0; nList <= 2; nList++) {
            if (nRenderPass != 2) {
                if (nList == 0) {
                    fn_8003272C(1);
                } else {
                    fn_8003272C(0);
                }
            }
            for (nClip = 0; nClip <= 2; nClip++) {
                if (lbl_801D3CB0.pSortedPatchList[nRenderPass][nList][nClip] != NULL) {
                    switch (nClip) {
                    case 2:
                        fn_80035138(1);
                        break;
                    case 1:
                        fn_80035138(1);
                        break;
                    default:
                        fn_80035138(0);
                        break;
                    }
                    fn_80012EF8();
                    for (pPatch = lbl_801D3CB0.pSortedPatchList[nRenderPass][nList][nClip]; pPatch != NULL;
                         pPatch = pPatch->pNext[nList]) {
                        fn_80032B7C(pPatch->pGround, nClip, nList, pPatch->n1C, pPatch->n18, pPatch->n20,
                                    &bFirst, 0, 0, pPatch->fDistance,
                                    pPatch->fDistance + 2.0f * pPatch->fBoundingRadius);
                    }
                }
            }
        }
        fn_80032F88(lbl_801D3CB0.pDeferredItemsList, lbl_801D3CB0.iDeferredItems,
                    lbl_801D3CB0.eTerrainFilterMin, lbl_801D3CB0.eTerrainFilterMag);
        lbl_801D3CB0.iDeferredItems = 0;
        fn_80012F50(1, 6, 1);
        fn_80012EF8();
        fn_8003272C(1);
        fn_80012EF8();
    }
}

void fn_8003272C(int n) {
    if (lbl_801D3CB0.boManageZUpdate) {
        fn_80012F34(n);
        lbl_802810CC = n;
    }
}

void fn_80032954(void) {
    fn_80032F88(lbl_801D3CB0.pOpaqueObjectList, lbl_801D3CB0.iOpaqueObjects, lbl_801D3CB0.eObjectFilterMin,
                lbl_801D3CB0.eObjectFilterMag);
    if (lbl_801D3CB0.iTranslucentObjects != 0) {
        fn_80032F88(lbl_801D3CB0.pTranslucentObjectList, lbl_801D3CB0.iTranslucentObjects,
                    lbl_801D3CB0.eObjectFilterMin, lbl_801D3CB0.eObjectFilterMag);
    }
    fn_80012F50(1, 6, 128);
    fn_80012EF8();
}

// Draws the post-draw terrain patches, each in every pass (bits 0..2 of n1C) it takes part in.
void fn_800329CC(void) {
    u8 bFirst = 1;
    int i;
    int nPass;
    int nPassBit;

    fn_80030894();
    fn_80014118(0x70);
    for (i = 0; i < lbl_801D3CB0.iTotalPostDrawTerrainPatches; i++) {
        switch (lbl_801D3CB0.pPostDrawTerrainList[i].eClipMethod) {
        case 2:
            fn_80035138(1);
            break;
        case 1:
            fn_80035138(1);
            break;
        default:
            fn_80035138(0);
            break;
        }
        fn_80012EF8();
        for (nPass = 0, nPassBit = 1; nPass <= 2; nPass++, nPassBit <<= 1) {
            if (nPassBit & lbl_801D3CB0.pPostDrawTerrainList[i].n1C) {
                fn_80032B7C(lbl_801D3CB0.pPostDrawTerrainList[i].pGround,
                            lbl_801D3CB0.pPostDrawTerrainList[i].eClipMethod, nPass,
                            lbl_801D3CB0.pPostDrawTerrainList[i].n1C,
                            lbl_801D3CB0.pPostDrawTerrainList[i].n18,
                            lbl_801D3CB0.pPostDrawTerrainList[i].n20, &bFirst, 1, 0,
                            lbl_801D3CB0.pPostDrawTerrainList[i].fDistance,
                            lbl_801D3CB0.pPostDrawTerrainList[i].fDistance
                                + 2.0f * lbl_801D3CB0.pPostDrawTerrainList[i].fBoundingRadius);
            }
        }
    }
    fn_8003084C();
}

void fn_80032AEC(void) {
    fn_80030894();
    fn_80032F88(lbl_801D3CB0.pPostDrawItemsList, lbl_801D3CB0.iPostDrawItems, lbl_801D3CB0.eObjectFilterMin,
                lbl_801D3CB0.eObjectFilterMag);
    fn_8003272C(0);
    fn_80032F88(lbl_801D3CB0.pNearbyObjectList, lbl_801D3CB0.iNearbyObjects, lbl_801D3CB0.eObjectFilterMin,
                lbl_801D3CB0.eObjectFilterMag);
    fn_8003272C(1);
    fn_80035098(0);
    fn_8003084C();
    fn_80012F50(1, 6, 128);
    fn_80012EF8();
}

void fn_800332F4(void) {
    lbl_802810CC = -1;
    lbl_802810C8 = -1.0f;
}

// Starts the crowd's animation: after fDelay seconds when that is above 0 (fn_80033744 counts it
// down, then calls here again), otherwise now, for fDuration, each crowd object from a
// pseudo-random point of its cycle. EA passes the share of the crowd that starts, but sets it to 1.
void fn_8003349C(f32 fPercentage, f32 fDuration, f32 fDelay) {
    f32 fRand = 0.0f;
    int i;

    fPercentage = 1.0f;
    if (fDelay > fRand) {
        lbl_801D3CB0.fCrowdAnimationDelayedStartTimer = fDelay;
        lbl_801D3CB0.fCrowdAnimationDelayedStartPercentage = fPercentage;
        lbl_801D3CB0.fCrowdAnimationDelayedStartDuration = fDuration;
        return;
    }
    lbl_801D3CB0.fCrowdAnimationCountdown = fDuration;
    lbl_801D3CB0.fCrowdAnimationDelayedStartPercentage = fPercentage;
    for (i = 0; i < TER_NUM_OBJECTS; i++) {
        s32 nFlags = lbl_801D3CB0.pObjectStateList[i].a20[0];
        s32 nFlags3 = lbl_801D3CB0.pObjectStateList[i].a20[3];

        if (nFlags & 1) {
            if ((nFlags & 2) && !(nFlags3 & 0x40)) {
                if (fPercentage >= fRand) {
                    lbl_801D3CB0.pObjectStateList[i].n1C = 3;
                    lbl_801D3CB0.pObjectStateList[i].f14 = lbl_801D3CB0.pObjectStateList[i].f10;
                }
                fRand *= 131.2934f;
                fRand += 82.459f;
                fRand -= fn_80035074(fRand);
            } else if ((nFlags & 2) && (nFlags3 & 0x40)) {
                if (fPercentage >= fRand) {
                    lbl_801D3CB0.pObjectStateList[i].n1C = 1;
                    lbl_801D3CB0.pObjectStateList[i].f14 = lbl_801D3CB0.pObjectStateList[i].f10;
                }
                fRand *= 131.2934f;
                fRand += 82.459f;
                fRand -= fn_80035074(fRand);
            }
        }
    }
}

// Stops the crowd's animation and puts every object with bits 0 and 1 of a20[0] back: n1C to 0,
// and f14 to f10, or with bReset f14 and n18 to 0.
void fn_800335F8(u8 bReset) {
    int i;

    lbl_801D3CB0.fCrowdAnimationDelayedStartTimer = -1.0f;
    lbl_801D3CB0.fCrowdAnimationCountdown = 0.0f;
    for (i = 0; i < TER_NUM_OBJECTS; i++) {
        s32 nFlags = lbl_801D3CB0.pObjectStateList[i].a20[0];
        s32 nFlags3 = lbl_801D3CB0.pObjectStateList[i].a20[3];

        if (nFlags & 1) {
            if ((nFlags & 2) && !(nFlags3 & 0x40)) {
                lbl_801D3CB0.pObjectStateList[i].n1C = 0;
                if (bReset) {
                    lbl_801D3CB0.pObjectStateList[i].f14 = 0.0f;
                    lbl_801D3CB0.pObjectStateList[i].n18 = 0;
                } else {
                    lbl_801D3CB0.pObjectStateList[i].f14 = lbl_801D3CB0.pObjectStateList[i].f10;
                }
            } else if ((nFlags & 2) && (nFlags3 & 0x40)) {
                lbl_801D3CB0.pObjectStateList[i].n1C = 0;
                if (bReset) {
                    lbl_801D3CB0.pObjectStateList[i].f14 = 0.0f;
                    lbl_801D3CB0.pObjectStateList[i].n18 = 0;
                } else {
                    lbl_801D3CB0.pObjectStateList[i].f14 = lbl_801D3CB0.pObjectStateList[i].f10;
                }
            }
        }
    }
}

f32 fn_800336E4(void) {
    return lbl_801D3CB0.fCrowdAnimationCountdown;
}

f32 fn_800336F4(void) {
    return lbl_801D3CB0.fCrowdAnimationDelayedStartPercentage;
}

// Sets object nObject of patch nPatch's n1C to 1 when bit 0 of its a20[3] is set.
void fn_80033704(u16 nPatch, u16 nObject) {
    Ter_ObjectState* pState =
        &lbl_801D3CB0.pObjectStateList[lbl_801D3CB0.iPatchFirstObjectInstanceIndex[nPatch] + nObject];

    if (pState->a20[3] & 1) {
        pState->n1C = 1;
    }
}

// The 'tLOD' chunk arrived: two values, made whole numbers and, when the second is the larger and
// both are at least lbl_802810DC x lbl_802810D8, turned into lbl_802810D0 and lbl_802810D4.
void fn_800341A4(UStreamObject* pObject) {
    TerLODData* pData = (TerLODData*)pObject->pData;
    s32 nRem;

    lbl_802810E4 = pData->fC;
    lbl_802810E8 = pData->f10;
    fn_80009E70(pObject);
    // A value that is not a multiple of lbl_802810D8 gets its remainder added (a multiple only
    // when the remainder was half of it); EA's rounding as it is.
    nRem = lbl_802810E4 % (s32)lbl_802810D8;
    if (nRem != 0) {
        lbl_802810E4 += nRem;
    }
    nRem = lbl_802810E8 % (s32)lbl_802810D8;
    if (nRem != 0) {
        lbl_802810E8 += nRem;
    }
    if (lbl_802810E8 > lbl_802810E4) {
        f32 fLimit = lbl_802810DC * lbl_802810D8;

        if (lbl_802810E4 < fLimit || lbl_802810E8 < fLimit) {
            return;
        }
        fn_80031A08(&lbl_802810D0, &lbl_802810D4, lbl_802810E4, lbl_802810E8);
    }
}

// The 'ter ' chunk arrived.
void fn_800342B4(UStreamObject* pObject) {
    lbl_801D3CB0.pCurrentHoleDataStreamData = pObject;
    lbl_801D3CB0.pCurrentHoleData = fn_800073B4(pObject->pData, 0);
}

void fn_80034648(int n) {
    lbl_801D3CB0.fDefaultObjectMipmapBias[0] = lbl_801876D8[n][0];
    lbl_801D3CB0.fDefaultObjectMipmapBias[1] = lbl_801876D8[n][1];
    lbl_801D3CB0.fDefaultObjectMipmapBias[2] = lbl_801876D8[n][2];
}

// Unloads the terrain.
void fn_8003467C(void) {
    fn_800335F8(1);
    if (lbl_801D3CB0.pCurrentHoleData != NULL) {
        fn_800075CC(lbl_801D3CB0.pCurrentHoleData);
        lbl_801D3CB0.pCurrentHoleData = NULL;
        fn_80009E70(lbl_801D3CB0.pCurrentHoleDataStreamData);
    }
    if (lbl_801D3CB0.pCourse != NULL) {
        fn_80009E70(lbl_801D3CB0.pCourseStreamData);
        lbl_801D3CB0.pCourse = NULL;
    }
    lbl_801D3CB0.iLowLODListOffset = -1;
    lbl_802810E4 = -1;
    lbl_802810E8 = -1;
    lbl_802810D0 = 26;
    lbl_802810D4 = 16;
}

// A tee's position arrived: into its row of the course's tees, if the course is loaded.
void fn_80034720(UStreamObject* pObject) {
    TerPosData* pTee = (TerPosData*)pObject->pData;

    if (lbl_801D3CB0.pCourse != NULL) {
        lbl_801D3CB0.pCourse->tee[pTee->nIndex].x = pTee->vPos[0];
        lbl_801D3CB0.pCourse->tee[pTee->nIndex].y = pTee->vPos[1];
        lbl_801D3CB0.pCourse->tee[pTee->nIndex].z = pTee->vPos[2];
        lbl_801D3CB0.pCourse->tee[pTee->nIndex].w = 1.0f;
    }
    fn_80009E70(pObject);
}

// A pin position arrived (UKernel.c hands it on). With fn_800E39F0 set it goes to fn_800F199C;
// otherwise a pin the course already has (w not 0) is copied into the chunk, and a missing one is
// taken from it.
u8 fn_800347B4(UStreamObject* pObject) {
    TerPosData* pPin = (TerPosData*)pObject->pData;

    if (lbl_801D3CB0.pCourse != NULL) {
        if (fn_800E39F0()) {
            fn_800F199C(pPin->vPos[0], pPin->vPos[1], pPin->vPos[2]);
        } else if (0.0f != lbl_801D3CB0.pCourse->pin[pPin->nIndex].w) {
            pPin->vPos[0] = lbl_801D3CB0.pCourse->pin[pPin->nIndex].x;
            pPin->vPos[1] = lbl_801D3CB0.pCourse->pin[pPin->nIndex].y;
            pPin->vPos[2] = lbl_801D3CB0.pCourse->pin[pPin->nIndex].z;
        } else {
            lbl_801D3CB0.pCourse->pin[pPin->nIndex].x = pPin->vPos[0];
            lbl_801D3CB0.pCourse->pin[pPin->nIndex].y = pPin->vPos[1];
            lbl_801D3CB0.pCourse->pin[pPin->nIndex].z = pPin->vPos[2];
            lbl_801D3CB0.pCourse->pin[pPin->nIndex].w = 1.0f;
        }
    }
    fn_80009E70(pObject);
    return 0;
}

// The flag follows the wind: it turns to face it and plays "flagcalm" below 5, "flagbrzy" below
// 13, else "flagwind".
void fn_800348DC(void) {
    f32 vWind[3];
    f32 fSpeed = Wind_Get(vWind);
    Character* pFlag = fn_8001D324(100);

    if (pFlag != NULL) {
        const char* aClips[3] = {"flagcalm", "flagbrzy", "flagwind"};
        int n;

        fn_80019358(pFlag, vWind, 0.0f);
        if (fSpeed < 5.0f) {
            n = 0;
        } else if (fSpeed < 13.0f) {
            n = 1;
        } else {
            n = 2;
        }
        if (n >= 3) {
            n = 2;
        }
        if (n < 0) {
            n = 0;
        }
        fn_8001BE88(pFlag, Char_SetClip(pFlag, 3, 0, aClips[n]), 1, 0.0f);
    }
}

void fn_800349CC(int n) {
    if (lbl_801D3CB0.pCurrentHoleData != NULL) {
        fn_80030894();
        fn_80030A40(lbl_801D3CB0.pCurrentHoleData, n);
        fn_8003084C();
    }
}

// Draws the grass patches of render pass nRenderPass that take part in the first pass (bit 0 of
// n1C), farthest first, one clip method at a time.
// Not exact (97.1%): the original tests bit 0 with `and.` against a register holding 1 (one more
// saved register); every spelling tried folds the 1 into `clrlwi.` (a local, s32/int counters).
void fn_80034CAC(int nRenderPass) {
    u8 bFirst = 1;
    int i;
    s32 nClip;

    fn_80012EF8();
    fn_80014118(0x70);
    for (nClip = 0; nClip <= 2; nClip++) {
        switch (nClip) {
        case 2:
            fn_80035138(1);
            break;
        case 1:
            fn_80035138(1);
            break;
        default:
            fn_80035138(0);
            break;
        }
        fn_80012EF8();
        for (i = lbl_801D3CB0.iNumGrassPatches - 1; i >= 0; i--) {
            if (lbl_801D3CB0.xpGrassPatchList[i].eClipMethod == nClip
                && lbl_801D3CB0.xpGrassPatchList[i].iRenderPass == nRenderPass
                && (lbl_801D3CB0.xpGrassPatchList[i].n1C & 1)) {
                fn_80032B7C(lbl_801D3CB0.xpGrassPatchList[i].pGround, nClip, 0,
                            lbl_801D3CB0.xpGrassPatchList[i].n1C, lbl_801D3CB0.xpGrassPatchList[i].n18,
                            lbl_801D3CB0.xpGrassPatchList[i].n20, &bFirst, 0, 1,
                            lbl_801D3CB0.xpGrassPatchList[i].fDistance,
                            lbl_801D3CB0.xpGrassPatchList[i].fDistance
                                + 2.0f * lbl_801D3CB0.xpGrassPatchList[i].fBoundingRadius);
            }
        }
    }
    fn_80012F50(1, 6, 1);
    fn_80012EF8();
    fn_8003272C(1);
    fn_80012EF8();
}

// Draws the grass patches in list 0x80 of n1C (pass 3), farthest first, one clip method at a time.
void fn_80034DE4(void) {
    u8 bFirst = 1;
    int i;
    s32 nClip;

    fn_8003272C(0);
    fn_80012EF8();
    fn_80014118(0x70);
    for (nClip = 0; nClip <= 2; nClip++) {
        switch (nClip) {
        case 2:
            fn_80035138(1);
            break;
        case 1:
            fn_80035138(1);
            break;
        default:
            fn_80035138(0);
            break;
        }
        fn_80012EF8();
        for (i = lbl_801D3CB0.iNumGrassPatches - 1; i >= 0; i--) {
            if (nClip == lbl_801D3CB0.xpGrassPatchList[i].eClipMethod
                && (lbl_801D3CB0.xpGrassPatchList[i].n1C & 0x80)) {
                fn_80032B7C(lbl_801D3CB0.xpGrassPatchList[i].pGround, nClip, 3,
                            lbl_801D3CB0.xpGrassPatchList[i].n1C, lbl_801D3CB0.xpGrassPatchList[i].n18,
                            lbl_801D3CB0.xpGrassPatchList[i].n20, &bFirst, 0, 1,
                            lbl_801D3CB0.xpGrassPatchList[i].fDistance,
                            lbl_801D3CB0.xpGrassPatchList[i].fDistance
                                + 2.0f * lbl_801D3CB0.xpGrassPatchList[i].fBoundingRadius);
            }
        }
    }
    fn_80012F50(1, 6, 1);
    fn_80012EF8();
    fn_8003272C(1);
    fn_80012EF8();
}

// Sets boManageZUpdate and returns what it was.
u8 fn_8003505C(u8 b) {
    u8 bOld = lbl_801D3CB0.boManageZUpdate;

    lbl_801D3CB0.boManageZUpdate = b;
    return bOld;
}

f32 fn_80035074(f32 x) {
    return floor(x);
}

// ---- the renderer's state ----

void fn_80035098(u8 b) {
    lbl_801B8980.b1D = b;
    lbl_801B8980.u110 |= 0x80;
}

void fn_800350B4(f32 f) {
    lbl_801B8980.f2C = f;
    lbl_801B8980.u110 |= 0x8;
}

void fn_800350D0(f32 f) {
    lbl_801B8980.f28 = f;
    lbl_801B8980.u110 |= 0x8;
}

void fn_800350EC(u8 r, u8 g, u8 b) {
    lbl_801B8980.a30[0] = r;
    lbl_801B8980.a30[1] = g;
    lbl_801B8980.a30[2] = b;
    lbl_801B8980.a30[3] = 0x80;
    lbl_801B8980.u110 |= 0x8;
}

void fn_80035118(int a, int b) {
    lbl_801B8980.n10 = a;
    lbl_801B8980.n14 = b;
    lbl_801B8980.u110 |= 0x10;
}

void fn_80035138(int a) {
    lbl_801B8980.nFC = a;
    lbl_801B8980.u110 |= 0x400;
}

void fn_80035154(u8 b) {
    lbl_801B8980.b1C = b;
    lbl_801B8980.u110 |= 0x80;
}

void fn_80035170(u32 uClear, u32 uSet) {
    lbl_801B8980.u20 &= ~uClear;
    lbl_801B8980.u20 |= uSet;
    lbl_801B8980.u110 |= 0x20;
}

// Where n frames falls in a cycle of fPeriod seconds, in seconds.
f32 fn_800351D8(u32 n, f32 fPeriod) {
    return FRAME_TIME * (f32)(n % (u32)(FRAME_RATE * (0.5f / FRAME_RATE + fPeriod)));
}

// ---- sweep code (not yet cleaned up) ----

void fn_80013D68();
void fn_80013D9C();
void fn_8003526C(void);
void fn_80035294(void);
void fn_800352BC(void);
void fn_80035398(void);
void fn_8003541C();
void fn_80035440(TerSettings* pSettings);

void fn_80035240(f32 (*pMtx)[4]) {
    fn_80013D9C(*(s32*)((u8*)lbl_80280DF0), pMtx, lbl_80280DF0);
}

void fn_8003526C(void) {
    fn_80012EF0(*(void**)lbl_80280DF0);
}

void fn_80035294(void) {
    fn_80013CCC(*(void**)lbl_80280DF0);
}

void fn_800352BC(void) {
    fn_80013D68(*(s32*)((u8*)lbl_80280DF0));
}

void fn_800352E4(void) {
    fn_8003541C();
    fn_80035398();
}

// ---- end of sweep code ----

// Hands the current light set's terrain colours to the renderer.
void fn_80035308(void) {
    fn_80035440(&fn_8003532C()->settings);
}

LightSet* fn_8003532C(void) {
    return lbl_80281380->pCur;
}

void fn_80035338(s32 nSet) {
    lbl_80281380->pCur = &lbl_80281380->aSet[nSet];
}

// Resets the current light set's terrain colours to the defaults.
void fn_8003534C(void) {
    fn_8006F334(&fn_8003532C()->settings);
}

void fn_80035370(void) {
    fn_8006EDC0(&fn_8003532C()->group);
}

// Hands the renderer the colour and the two distances made from the current settings.
void fn_80035398(void) {
    fn_800350EC(lbl_802811E0->f44, lbl_802811E0->f48, lbl_802811E0->f4C);
    fn_800350D0(0.375f * lbl_802811E0->f50);
    fn_800350B4(4.15f * (10.0f + lbl_802811E0->f50));
}

// ---- sweep code (not yet cleaned up) ----

void fn_8006F154();
void fn_800082CC(void* p);
void fn_800354B4(u8* p, f32 v);
s32 fn_80035500(u8* p);
s32 fn_80035508(u8* p0);
s32 fn_80035554(u8* p0);
f32 fn_80035560(u8* p0);
s32 fn_8003556C(u8* p);
extern u8* lbl_802813B8;
void fn_80035584(s32 v);
void fn_80035590(f32* p0);
void fn_800355B8(f32* p0);
extern s32 lbl_80281B88;
extern s32 lbl_80281D68;
void fn_800355E0(s32 arg0);
void fn_80035600(void);

void fn_8003541C(void) {
    fn_8001614C();
    fn_8006F154();
}

// ---- end of sweep code ----

// Takes a copy of the settings the renderer's colour and distances are made from.
void fn_80035440(TerSettings* pSettings) {
    Mem_cpy(lbl_802811E0, pSettings, sizeof(TerSettings));
}

// a - b into out, three floats; the same helper as Ball.c's fn_80055EA0.
#ifdef __MWERKS__
asm void fn_8003546C(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_8003546C(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// a - b into out, four floats.
#ifdef __MWERKS__
asm void fn_80035490(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 0, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 0, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_80035490(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
    pOut[3] = pA[3] - pB[3];
}
#endif

// ---- sweep code (not yet cleaned up) ----

void fn_800354B4(u8* p, f32 v) {
    *(f32*)(p + 0xAC) = v;
}

s32 fn_800354BC(s32 p) {
    return *(s32*)((u8*)p + 0x14);
}

void* fn_800354C4(u8** p0) {
    return *p0 + 88;
}

// A byte of the object's model (from 0x24 on); the patch code reads bytes 1-3. The object's type
// is not described yet, so this keeps the handle-style prototype its callers above use.
s32 fn_800354D0(s32 pObject, s32 n) {
    return (*(s8**)pObject)[n + 0x24];
}

s32 fn_800354E4(s32 p0, s32 p1) {
    return (*(s32**)((u8*)p0 + 0x8))[p1];
}

s32 fn_800354F4(s32 p0) {
    return **(s16**)p0;
}

s32 fn_80035500(u8* p) {
    return *(s32*)(p + 0xEC);
}

s32 fn_80035508(u8* p0) {
    return (*(s32*)p0 + 104);
}

// If the object's current entry (n28) is switched on, hands its 0x2C-byte record to LLObj_Gc.c's
// fn_800082CC. Raw offsets until the object's type is described.
void fn_80035514(u8* pObject) {
    s32 n = *(s32*)(pObject + 0x28);

    if (pObject[n + 0x1C] != 0) {
        fn_800082CC(*(u8**)(pObject + 0x18) + n * 0x2C);
    }
}

s32 fn_80035554(u8* p0) {
    return *(u8*)(((u8*)*(s32*)p0) + 0x8B);
}

f32 fn_80035560(u8* p0) {
    return *(f32*)(((u8*)*(s32*)p0) + 0x54);
}

s32 fn_8003556C(u8* p) {
    return *(s32*)(p + 0xC);
}

u8 fn_80035574(void) {
    return lbl_802811F0->uFlags & 2;
}

void fn_80035584(s32 v) {
    *(s32*)(lbl_802813B8 + 0x1930) = v;
}

void fn_80035590(f32* p0) {
    Vec_Copy(p0, (f32*)(lbl_802813B8 + 4));
}

void fn_800355B8(f32* p0) {
    Vec_Copy(p0, (f32*)(lbl_802813B8 + 20));
}

void fn_800355E0(s32 arg0) {
    s32 var_r3;

    var_r3 = arg0;
    if (var_r3 == (s32) (lbl_80281B88 & 1)) {
        var_r3 = lbl_80281D68 ^ 1;
    }
    lbl_80281D68 = var_r3;
}

void fn_80035600(void) {
}

void fn_80035604(void) {
    fn_80035240(NULL);
    fn_80035294();
    fn_80016B9C();
    fn_80035118(4, 5);
    fn_80012EF8();
}

// ---- end of sweep code ----
