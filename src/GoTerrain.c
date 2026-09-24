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
#include "glows.h"
#include "dynobj.h"
#include "unsorted/cull.h"

void* fn_800073B4(u8* pData, int n);
void  fn_800075CC(void* p);         // frees what fn_800073B4 made
void  fn_80019358(Character* pChar, f32* pDir, f32 f);
void  fn_8001BE88(Character* pChar, void* pClip, int n, f32 f);
void  fn_800F199C(f32 x, f32 y, f32 z);
CamLens* fn_8001F004(void);         // the current camera's lens
f32   fn_8001414C(u8* p);
f32   fn_80014280(f32 x);           // tan
void  fn_80030894(void);
void  fn_80030A40(void* pHoleData, int nView);
void  fn_80030CC8(void* pHoleData);
void  fn_8003185C(void);
void  fn_800318D8(void);
void  fn_80031AB4(void);
void  fn_80031E58(void);
void  fn_80032518(int nRenderPass);
void  fn_80032770(void);
void  fn_80032954(void);
void  fn_80033F94(void* pHoleData, int nList);
void  fn_8003546C(f32* pA, f32* pB, f32* pOut);
f32   fn_8001EFFC(CamLens* pLens);
f32   fn_800351D8(u32 n, f32 fPeriod);
void  fn_8003519C(int nRow, void* pData);   // calls row nRow's function of lbl_80188E88 with pData
s32   fn_800318AC(const void* pA, const void* pB);
void  fn_8003272C(int n);
void  fn_80035154(u8 b);
void  fn_80035170(u32 uClear, u32 uSet);
void  fn_80035294(void);
void  fn_800352BC(void);
void  fn_800354B4(u8* p, f32 v);        // sets the lens's f32 at 0xAC (fn_80014268 reads it)
f32   fn_80014268(u8* p);
void  fn_80031938(Ter_LODPlane* pPlanes, f32 fStep, s32 a, s32 b, s32 c, s32 d);
void  fn_80032B7C(void* pGround, s32 eClipMethod, s32 nPass, s32 n1C, s32 n18, s32 n20, u8* pbFirst, u8 b1,
                  u8 b2, f32 fNear, f32 fFar);
void  fn_80032F88(Ter_ObjectDrawData* pList, s32 nCount, s32 eFilterMin, s32 eFilterMag);
void  fn_800341A4(UStreamObject* pObject);
void  fn_800342B4(UStreamObject* pObject);
void  fn_800342F0(UStreamObject* pObject);
void  fn_80035098(u8 b);
void  fn_80034CAC(int nRenderPass);
void  fn_80034DE4(void);
void  fn_80034F28(void* pUnused);
void  fn_80035490(f32* pA, f32* pB, f32* pOut);

void*     fn_800354BC(UObjMesh* pNode);
f32*      fn_800354C4(UObjMesh* pNode);
s32       fn_800354D0(UObjMesh* pNode, s32 n);
UObjMesh* fn_800354E4(UObjMesh* pNode, s32 n);
s32       fn_800354F4(UObjMesh* pNode);
UObjMesh* fn_80035500(u8* pHoleData);
UObjMesh* fn_8003556C(UObjMesh* pGround);
s32       fn_80035554(UObjMesh* pMesh);

// Fills pPatch from a patch's node: its ground is node 0, its objects come with a second node.
void fn_80031084(UObjMesh* pNode, s32 eClipMethod, s32 iRenderPass, Ter_PatchReference* pPatch,
                 f32 fDistance) {
    s32 nNodes;

    pPatch->fDistance = fDistance;
    pPatch->fBoundingRadius = fn_800354C4(pNode)[3];
    pPatch->eClipMethod = eClipMethod;
    pPatch->iRenderPass = iRenderPass;
    nNodes = fn_800354F4(pNode);
    pPatch->pGround = fn_800354E4(pNode, 0);
    pPatch->n18 = fn_800354D0(pPatch->pGround, 3);
    pPatch->n1C = fn_800354D0(pPatch->pGround, 2);
    pPatch->n20 = fn_800354D0(pNode, 1);
    if (nNodes >= 2) {
        pPatch->pObjects = fn_800354BC(pPatch->pGround);
        return;
    }
    pPatch->pObjects = NULL;
}

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

// Draws the terrain in view nView: takes the camera's position and look direction, the flat
// distance to the nearest ball, the followed player's distance to the pin and the smaller half
// field of view's tangent, then builds the lists and draws them pass by pass.
void fn_80030A40(void* pHoleData, int nView) {
    int i;
    CamLens* pLens;
    f32 vToPin[4];
    f32 vDiff[4];
    f32 fDist;
    f32 fTan;
    f32 fWideTan;

    lbl_801D3CB0.iCurrentViewContext = nView;
    pLens = fn_8001F004();
    lbl_801D3CB0.fFOVScale = 1.0f / fn_8001EFFC(pLens);
    lbl_801D3CB0.xCameraReferencePos[0] = pLens->v34[0];
    lbl_801D3CB0.xCameraReferencePos[1] = pLens->v34[1];
    lbl_801D3CB0.xCameraReferencePos[2] = pLens->v34[2];
    lbl_801D3CB0.xCameraReferencePos[3] = 1.0f;
    lbl_801D3CB0.xCameraLookVector[0] = pLens->v24[0];
    lbl_801D3CB0.xCameraLookVector[1] = pLens->v24[1];
    lbl_801D3CB0.xCameraLookVector[2] = pLens->v24[2];
    lbl_801D3CB0.xCameraLookVector[3] = 1.0f;
    lbl_801D3CB0.fXZDistanceToClosestBallSquared = 1000000.0f;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        fn_80035490(gPlayers[i].ball.vPos, lbl_801D3CB0.xCameraReferencePos, vDiff);
        vDiff[1] = 0.0f;
        fDist = fn_80009744(vDiff);
        if (fDist < lbl_801D3CB0.fXZDistanceToClosestBallSquared) {
            lbl_801D3CB0.fXZDistanceToClosestBallSquared = fDist;
        }
    }
    fn_8003546C(gPlayers[fn_8001707C(lbl_801D3CB0.iCurrentViewContext)].vBall,
                &fn_8000C594()->pin[Game_CurrentPinSet()].x, vToPin);
    vToPin[1] = 0.0f;
    lbl_801D3CB0.fGolferDistanceToCup = fn_80009680(fn_80009744(vToPin));
    fTan = fn_80014280(0.5f * pLens->fFov);
    fWideTan = fn_80014280(0.5f * (0.75f * pLens->fFov * fn_8001414C((u8*)fn_8003526C())));
    lbl_801D3CB0.fCameraMinHalfFieldOfViewTan =
        (fTan <= fWideTan / fn_80017028(nView)->f54) ? fTan : fWideTan / fn_80017028(nView)->f54;
    fn_80012EF8();
    if (!lbl_801D3CB0.bObjectTestMode) {
        fn_80033F94(pHoleData, 0);
    }
    fn_80030CC8(pHoleData);
    fn_8003185C();
    fn_800318D8();
    fn_80031AB4();
    fn_80031E58();
    fn_80032518(0);
    fn_80032770();
    if (!lbl_801D3CB0.bObjectTestMode) {
        fn_80033F94(pHoleData, 2);
    }
    fn_80032518(1);
    fn_80032518(2);
    fn_80032954();
    fn_80035118(4, 5);
    fn_80012F50(1, 6, 0x80);
    fn_80014118(0x70);
    fn_80012EF8();
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

// Whether a ball has settled inside pModel's bounding sphere: a ball that has left where its shot
// started, has hit something (nCollideCount) and moves slower than 10.
u8 fn_80032330(UObjMesh* pModel) {
    f32* pSphere = fn_800354C4(pModel);
    f32 fRadiusSq = pSphere[3] * pSphere[3];
    int i;

    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (fn_800BB028(gPlayers[i].ball.vPos, gPlayers[i].ball.vStart) > 0.0f
            && fn_800BB028(pSphere, gPlayers[i].ball.vPos) < fRadiusSq
            && gPlayers[i].ball.nCollideCount != 0 && gPlayers[i].ball.fSpeed < 10.0f) {
            return 1;
        }
    }
    return 0;
}

// Adds a draw of pModel to a draw list: fills pDraw and counts it in *pCount. A model whose flags
// (bytes 0 and 3) ask for it is skipped in modes 6-8 (fn_800E3A54); one with bits 0 and 1 of byte 0
// is otherwise drawn as its node chosen by the object's state (n18).
void fn_8003241C(Ter_ObjectDrawData* pDraw, s32* pCount, s32 nUnused, UObjMesh* pModel, s32 iObject,
                 s32 eClipMethod, u8 bUseFog, u8 bSetsPrimField, f32 fAlpha, f32 fMipmapBias,
                 f32 fDistanceSquared) {
    s32 uFlags0;
    s32 uFlags3;

    // nUnused: every caller passes a number (0x28A, 0xC8, 0x46) this function does not read
    pDraw->pObject = pModel;
    pDraw->fAlpha = fAlpha;
    pDraw->fMipmapBias = fMipmapBias;
    pDraw->fDistanceSquared = fDistanceSquared;
    pDraw->iGlobalObjectIndex = iObject;
    pDraw->eClipMethod = eClipMethod;
    pDraw->bUseFog = bUseFog;
    pDraw->bSetsPrimField = bSetsPrimField;
    uFlags0 = fn_800354D0(pModel, 0);
    uFlags3 = fn_800354D0(pModel, 3);
    if (uFlags0 & 1) {
        if (uFlags0 & 2) {
            if (fn_800E3A54()) return;
            pDraw->pObject = fn_800354E4(pModel, lbl_801D3CB0.pObjectStateList[iObject].n18);
        }
    } else if ((uFlags3 & 4) || (uFlags3 & 0x10) || (uFlags3 & 0x20)) {
        if (fn_800E3A54()) return;
    }
    if (pDraw->pObject->p18 != NULL) {
        pDraw->eShaderObjectType = pDraw->pObject->p18->n0;
    }
    (*pCount)++;
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

// Draws render pass 0's fourth patch lists (pSortedPatchList[0][3]), if it has any, without z
// writes; the lens's value at 0xAC is raised by 25 while the renderer is set up, then put back. In
// split screen, patches with bit 0x8 of n1C are left out.
void fn_80032770(void) {
    u8 bFirst = 1;
    u8 bAny;
    int nClip;
    Ter_PatchReference** ppHead;
    Ter_PatchReference* pPatch;
    CamLens* pLens;
    f32 fAC;

    bAny = 0;
    if (lbl_801D3CB0.pSortedPatchList[0][3][0] != NULL) {
        bAny = 1;
    } else if (lbl_801D3CB0.pSortedPatchList[0][3][1] != NULL) {
        bAny = 1;
    } else if (lbl_801D3CB0.pSortedPatchList[0][3][2] != NULL) {
        bAny = 1;
    }
    if (bAny) {
        fn_8003272C(0);
        pLens = ((Camera*)*lbl_80280DF0)->unk10;
        fAC = fn_80014268((u8*)pLens);
        fn_800354B4((u8*)pLens, 25.0f + fAC);
        fn_800352BC();
        fn_80035294();
        fn_80016B9C();
        fn_80016B9C();
        fn_80016B9C();
        fn_800354B4((u8*)pLens, fAC);
        fn_80012EF8();
        fn_80014118(0x70);
        for (nClip = 0; nClip <= 2; nClip++) {
            ppHead = &lbl_801D3CB0.pSortedPatchList[0][3][nClip];
            if (*ppHead != NULL) {
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
                for (pPatch = *ppHead; pPatch != NULL; pPatch = pPatch->pNext[3]) {
                    if (!gSession.nSplitScreen || !(pPatch->n1C & 8)) {
                        fn_80032B7C(pPatch->pGround, nClip, 3, pPatch->n1C, pPatch->n18, pPatch->n20, &bFirst,
                                    0, 0, pPatch->fDistance,
                                    pPatch->fDistance + 2.0f * pPatch->fBoundingRadius);
                    }
                }
            }
        }
        fn_80012F50(1, 6, 1);
        fn_80012EF8();
        fn_8003272C(1);
        fn_80012EF8();
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

// Sets the renderer up for an object's draw, unless its alpha and z writes are those of the last one
// (lbl_802810C8, lbl_802810CC) and bForce is clear; returns whether it did. An opaque object gets
// the mesh's own blend flags and z writes, a faded one draws with its alpha.
u8 fn_80033308(Ter_ObjectDrawData* pDraw, u8 bForce) {
    f32 fAlpha = pDraw->fAlpha;
    s32 nRef;
    u32 uFlags;
    u8 bZWrite;

    if (fn_80035554(pDraw->pObject) & 2) {
        nRef = 1;
        uFlags = 0x40;
        bZWrite = 0;
    } else {
        nRef = 0x50;
        uFlags = 0;
        bZWrite = 1;
    }
    if (fAlpha != lbl_802810C8 || (s8)bZWrite != lbl_802810CC || bForce) {
        if (1.0f == fAlpha) {
            fn_80012F50(1, 6, nRef);
            fn_80035170(0x40, ((uFlags & 0x40) ? 0x40 : 0)
                                  | (((uFlags & 0x10) ? 0x10 : 0) | ((uFlags & 0x20) ? 0x20 : 0)));
            fn_8003272C(bZWrite);
            fn_80035098(0);
            fn_80035118(4, 5);
        } else {
            fn_80012F50(1, 6, fAlpha * nRef);
            fn_8003272C(1);
            fn_80035098(1);
            fn_80035154(255.0f * (0.5f * fAlpha));
        }
        fn_80012EF8();
        lbl_802810C8 = fAlpha;
        return 1;
    }
    return 0;
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

// The model of object list nObjList of patch nPatch: node 1 of the hole data's tree holds one node
// per patch, and a patch's node 1 holds, in its node 0, its object lists. NULL when out of range.
UObjMesh* fn_80034A20(u16 nPatch, u16 nObjList) {
    UObjMesh* pModel = NULL;
    UObjMesh* pNode;

    pNode = fn_800354E4(fn_80035500(lbl_801D3CB0.pCurrentHoleData), 1);
    if (nPatch < fn_800354F4(pNode)) {
        pNode = fn_800354E4(pNode, nPatch);
        if (fn_800354F4(pNode) >= 2) {
            pNode = fn_800354E4(fn_800354E4(pNode, 1), 0);
            if (nObjList < fn_800354F4(pNode)) {
                pModel = fn_800354E4(pNode, nObjList);
            }
        }
    }
    return pModel;
}

// Draws the terrain: sets the renderer up, takes the camera's position and look direction, the
// flat distance to the nearest ball and the smaller half field of view's tangent, then draws the
// objects (fn_80034F28) and the grass (fn_80034CAC, fn_80034DE4).
void fn_80034AE4(void) {
    int i;
    void* pHoleData = lbl_801D3CB0.pCurrentHoleData;
    CamLens* pLens = fn_8001F004();
    f32 vDiff[4];
    f32 fDist;
    f32 fTan;
    f32 fWideTan;

    fn_80035240(NULL);
    fn_80016B9C();
    fn_80016B9C();
    fn_80016B9C();
    fn_80035118(4, 5);
    fn_80012F50(1, 6, 1);
    fn_80014118(0x70);
    fn_80012EF8();
    lbl_801D3CB0.xCameraReferencePos[0] = pLens->v34[0];
    lbl_801D3CB0.xCameraReferencePos[1] = pLens->v34[1];
    lbl_801D3CB0.xCameraReferencePos[2] = pLens->v34[2];
    lbl_801D3CB0.xCameraReferencePos[3] = 1.0f;
    lbl_801D3CB0.xCameraLookVector[0] = pLens->v24[0];
    lbl_801D3CB0.xCameraLookVector[1] = pLens->v24[1];
    lbl_801D3CB0.xCameraLookVector[2] = pLens->v24[2];
    lbl_801D3CB0.xCameraLookVector[3] = 1.0f;
    lbl_801D3CB0.fXZDistanceToClosestBallSquared = 1000000.0f;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        fn_80035490(gPlayers[i].ball.vPos, lbl_801D3CB0.xCameraReferencePos, vDiff);
        vDiff[1] = 0.0f;
        fDist = fn_80009744(vDiff);
        if (fDist < lbl_801D3CB0.fXZDistanceToClosestBallSquared) {
            lbl_801D3CB0.fXZDistanceToClosestBallSquared = fDist;
        }
    }
    fTan = fn_80014280(0.5f * pLens->fFov);
    fWideTan = fn_80014280(0.5f * (0.75f * pLens->fFov * fn_8001414C((u8*)fn_8003526C())));
    lbl_801D3CB0.fCameraMinHalfFieldOfViewTan =
        (fTan <= fWideTan / fn_80017028(0)->f54) ? fTan : fWideTan / fn_80017028(0)->f54;
    fn_80012EF8();
    fn_80034F28(pHoleData);
    fn_80034CAC(0);
    fn_80034DE4();
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

// Builds the grass list: every patch in the first pass's lists (bit 0x80 of n1C) or whose ground has
// flag 8 of byte 3, and that is not off screen, is copied to xpGrassPatchList with its clip method
// and its distance from the camera (less its radius, at least 0).
void fn_80034F28(void* pUnused) {
    Ter_PatchReference* pPatch;
    void* pCamera;
    Ter_PatchReference* pGrass;
    int i;
    View* pView;
    f32 fRadius;
    f32 fDist;
    s32 nClip;

    // pUnused: the one caller, fn_80034AE4, passes the hole data, which this function does not read
    pCamera = fn_8001614C();
    lbl_801D3CB0.iNumGrassPatches = 0;
    pGrass = lbl_801D3CB0.xpGrassPatchList;
    for (i = 0; i < lbl_801D3CB0.iTotalPatches; i++) {
        pPatch = &lbl_801D3CB0.pPatchList[i];
        if ((pPatch->n1C & 0x80) || (fn_800354D0(fn_8003556C(pPatch->pGround), 3) & 8)) {
            fRadius = fn_800354C4(fn_8003556C(pPatch->pGround))[3];
            fDist = Vec_Distance(lbl_801D3CB0.xCameraReferencePos, fn_800354C4(fn_8003556C(pPatch->pGround)))
                    - fRadius;
            if (fDist < 0.0f) {
                fDist = 0.0f;
            }
            pView = fn_80017028(lbl_801D3CB0.iCurrentViewContext);
            nClip = fn_80007B2C(fn_8003556C(pPatch->pGround), pCamera, fDist,
                                lbl_801D3CB0.fCameraMinHalfFieldOfViewTan, pView->f54);
            if (nClip != 3) {
                Mem_cpy(pGrass, pPatch, sizeof(Ter_PatchReference));
                pGrass->eClipMethod = nClip;
                pGrass->fDistance = fDist;
                pGrass++;
                lbl_801D3CB0.iNumGrassPatches++;
            }
        }
    }
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

void fn_8003519C(int nRow, void* pData) {
    lbl_80188E88[nRow].pfn8(pData);
}

// Where n frames falls in a cycle of fPeriod seconds, in seconds.
f32 fn_800351D8(u32 n, f32 fPeriod) {
    return FRAME_TIME * (f32)(n % (u32)(FRAME_RATE * (0.5f / FRAME_RATE + fPeriod)));
}

// ---- sweep code (not yet cleaned up) ----

void fn_80013D68();
void fn_80013D9C();
void fn_80035398(void);
void fn_8003541C();
void fn_80035440(TerSettings* pSettings);

void fn_80035240(f32 (*pMtx)[4]) {
    fn_80013D9C(*(s32*)((u8*)lbl_80280DF0), pMtx, lbl_80280DF0);
}

f32* fn_8003526C(void) {
    return fn_80012EF0(*(void**)lbl_80280DF0);
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
s32 fn_80035508(u8* p0);
f32 fn_80035560(u8* p0);
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

// ---- end of sweep code ----

void* fn_800354BC(UObjMesh* pNode) {
    return pNode->p14;
}

// The node's bounding sphere: centre (v58), then radius (f64).
f32* fn_800354C4(UObjMesh* pNode) {
    return pNode->pInfo->v58;
}

// A flag byte of the node (a24); the patch code reads bytes 1-3.
s32 fn_800354D0(UObjMesh* pNode, s32 n) {
    return pNode->pInfo->a24[n];
}

UObjMesh* fn_800354E4(UObjMesh* pNode, s32 n) {
    return pNode->p8[n];
}

// How many nodes the node holds.
s32 fn_800354F4(UObjMesh* pNode) {
    return pNode->pInfo->n0;
}

// The root of the hole data's model tree. The hole data's layout is not known yet, so the offset
// stays raw.
UObjMesh* fn_80035500(u8* pHoleData) {
    return *(UObjMesh**)(pHoleData + 0xEC);
}

// ---- sweep code (not yet cleaned up) ----

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

s32 fn_80035554(UObjMesh* pMesh) {
    return pMesh->pInfo->b8B;
}

f32 fn_80035560(u8* p0) {
    return *(f32*)(((u8*)*(s32*)p0) + 0x54);
}

// The mesh drawn for a patch's ground.
UObjMesh* fn_8003556C(UObjMesh* pGround) {
    return pGround->pC;
}

u8 fn_80035574(void) {
    return lbl_802811F0->uFlags & 2;
}

void fn_80035584(s32 v) {
    lbl_802813B8->n1930 = v;
}

void fn_80035590(f32* p0) {
    Vec_Copy(p0, lbl_802813B8->v4);
}

void fn_800355B8(f32* p0) {
    Vec_Copy(p0, lbl_802813B8->v14);
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
