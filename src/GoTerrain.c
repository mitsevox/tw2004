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

void  fn_80019358(Character* pChar, f32* pDir, f32 f);
void  fn_800F199C(f32 x, f32 y, f32 z);
f32   fn_8001414C(u8* p);
f32   fn_80014280(f32 x);           // tan
void  fn_80030894(void);
void  fn_80030A40(void* pHoleData, int nView);
void  fn_80030CC8(void* pHoleData);
void  fn_80031154(Ter_PatchReference* pPatch, s32 nFirstObject);
void  fn_8003185C(void);
void  fn_800318D8(void);
void  fn_80031AB4(void);
u8    fn_80031E40(void);
void  fn_80031E58(void);
u8    fn_80032330(UObjMesh* pModel);
void  fn_8003241C(Ter_ObjectDrawData* pDraw, s32* pCount, s32 nUnused, UObjMesh* pModel, s32 iObject,
                  s32 eClipMethod, u8 bUseFog, u8 bSetsPrimField, f32 fAlpha, f32 fMipmapBias,
                  f32 fDistanceSquared);
void  fn_80032518(int nRenderPass);
void  fn_80032770(void);
void  fn_80032954(void);
void  fn_80033F94(void* pHoleData, u32 nList);
void  fn_8003546C(f32* pA, f32* pB, f32* pOut);
f32   fn_8001EFFC(CamLens* pLens);
f32   fn_800351D8(u32 n, f32 fPeriod);
void  fn_8003519C(int nRow, void* pData);   // calls row nRow's function of lbl_80188E88 with pData
s32   fn_800318AC(const void* pA, const void* pB);
void  fn_8003272C(int n);
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
void  fn_80035584(s32 v);
void  fn_80035590(f32* p0);
void  fn_800355B8(f32* p0);
void  fn_80034648(int n);
void  fn_80035514(u8* pObject);
void  fn_800332F4(void);
u8    fn_8003505C(u8 b);
f32*  fn_80035508(UObjMesh* pMesh);
u8    fn_80033308(Ter_ObjectDrawData* pDraw, u8 bForce);
void  fn_8000ADC0(f32 (*pMtx)[4]);  // identity matrix
void  fn_80035370(void);
void  fn_80034CAC(int nRenderPass);
void  fn_80034DE4(void);
void  fn_80034F28(void* pUnused);
void  fn_80035490(f32* pA, f32* pB, f32* pOut);

UObjMesh* fn_800354BC(UObjMesh* pNode);
f32       fn_80035560(UObjMesh* pMesh);
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

// Sets up the terrain renderer: allocates its lists, gives every object state its starting values
// (each tree its own period from a stepped random number), sets the renderer's defaults and fills
// the bit-count table lbl_801D3A30.
void fn_80030254(void) {
    s32 i;
    s32 n;
    s32 k;
    s32 j;
    s32 nBits;

    lbl_801D3CB0.pPatchList = fn_80009B34(1024 * sizeof(Ter_PatchReference), 2, 16, "GoTerrain.c", 567);
    lbl_801D3CB0.pPostDrawTerrainList =
        fn_80009B34(20 * sizeof(Ter_PatchReference), 2, 16, "GoTerrain.c", 572);
    lbl_801D3CB0.pObjectSortList = fn_80009B34(650 * sizeof(Ter_ObjectReference), 2, 16, "GoTerrain.c", 577);
    lbl_801D3CB0.pOpaqueObjectList = fn_80009B34(650 * sizeof(Ter_ObjectDrawData), 2, 16, "GoTerrain.c", 582);
    lbl_801D3CB0.pTranslucentObjectList =
        fn_80009B34(200 * sizeof(Ter_ObjectDrawData), 2, 16, "GoTerrain.c", 587);
    lbl_801D3CB0.pNearbyObjectList = fn_80009B34(70 * sizeof(Ter_ObjectDrawData), 2, 16, "GoTerrain.c", 592);
    lbl_801D3CB0.pDeferredItemsList = fn_80009B34(50 * sizeof(Ter_ObjectDrawData), 2, 16, "GoTerrain.c", 597);
    lbl_801D3CB0.pPanoramaItemsList =
        fn_80009B34(300 * sizeof(Ter_ObjectDrawData), 2, 16, "GoTerrain.c", 602);
    lbl_801D3CB0.pPostDrawItemsList =
        fn_80009B34(400 * sizeof(Ter_ObjectDrawData), 2, 16, "GoTerrain.c", 607);
    lbl_801D3CB0.pObjectStateList =
        fn_80009B34(TER_NUM_OBJECTS * sizeof(Ter_ObjectState), 2, 16, "GoTerrain.c", 612);
    lbl_801D3CB0.xpGrassPatchList = fn_80009B34(128 * sizeof(Ter_PatchReference), 2, 16, "GoTerrain.c", 633);
    lbl_801D3CB0.fTreeMinPeriod = 3.7f;
    lbl_801D3CB0.fTreeDiffPeriod = 1.6f;
    lbl_801D3CB0.fTreeOverdrive = 1.0f;
    lbl_801D3CB0.fTreeNoisePeriodScale = 1.0f;
    lbl_801D3CB0.fTreeNoiseAmplitudeScale = 0.0f;
    for (i = 0; i < TER_NUM_OBJECTS; i++) {
        lbl_801D3CB0.pObjectStateList[i].a20[0] = 0;
        lbl_801D3CB0.pObjectStateList[i].a20[1] = 0;
        lbl_801D3CB0.pObjectStateList[i].a20[2] = 0;
        lbl_801D3CB0.pObjectStateList[i].a20[3] = 0;
        lbl_801D3CB0.pObjectStateList[i].f0 =
            lbl_80281D60 * lbl_801D3CB0.fTreeDiffPeriod + lbl_801D3CB0.fTreeMinPeriod;
        lbl_80281D60 *= 131.2934f;
        lbl_80281D60 += 82.459f;
        lbl_80281D60 -= fn_80035074(lbl_80281D60);
        lbl_801D3CB0.pObjectStateList[i].f4 = lbl_801D3CB0.fTreeOverdrive;
        lbl_801D3CB0.pObjectStateList[i].f8 = 0.0f;
        lbl_801D3CB0.pObjectStateList[i].nC = 0;
        lbl_801D3CB0.pObjectStateList[i].n18 = 0;
        lbl_801D3CB0.pObjectStateList[i].n1C = 0;
        lbl_801D3CB0.pObjectStateList[i].f10 = lbl_80281D60;
        lbl_801D3CB0.pObjectStateList[i].aView[0].n4 = 3;
        lbl_801D3CB0.pObjectStateList[i].aView[0].f0 = 1.0f;
        lbl_801D3CB0.pObjectStateList[i].aView[1].n4 = 3;
        lbl_801D3CB0.pObjectStateList[i].aView[1].f0 = 1.0f;
    }
    lbl_801D3CB0.pCurrentHoleData = NULL;
    lbl_801D3CB0.pCourse = NULL;
    lbl_801D3CB0.fDetailMipmapBias = 0.0f;
    lbl_801D3CB0.fLakeSurfaceMipmapBias = 0.0f;
    lbl_801D3CB0.iLowLODListOffset = -1;
    lbl_801D3CB0.fCrowdAnimationDelayedStartTimer = -1.0f;
    lbl_801D3CB0.fCrowdAnimationDelayedStartPercentage = 0.0f;
    lbl_801D3CB0.fCrowdAnimationDelayedStartDuration = 0.0f;
    lbl_801D3CB0.fCrowdAnimationCountdown = -1.0f;
    lbl_801D3CB0.fCrowdFadeDistanceMin = 4.0f;
    lbl_801D3CB0.fCrowdFadeDistanceMax = 5.0f;
    lbl_801D3CB0.iCrowdPose = 0;
    lbl_801D3CB0.fCrowdInterpValue = 0.5f;
    lbl_801D3CB0.fTreeDampingDistance = 5625.0f;
    lbl_801D3CB0.fTreeDampingMaxForce = 0.5f;
    lbl_801D3CB0.fDistanceCullYardsBase = 150.0f;
    lbl_801D3CB0.eObjectFilterMin = 4;
    lbl_801D3CB0.eObjectFilterMag = 1;
    lbl_801D3CB0.eTerrainFilterMin = 5;
    lbl_801D3CB0.eTerrainFilterMag = 1;
    lbl_801D3CB0.fCrowdFullMaxDistanceFromGolfer = 60.0f;
    lbl_801D3CB0.fCrowdHalfMaxDistanceFromGolfer = 350.0f;
    fn_8003505C(1);
    for (n = 0; n < 5; n++) {
        for (k = 0; k < 32; k++) {
            nBits = 0;
            for (j = 0; j < n; j++) {
                if (k & (1 << j)) {
                    nBits++;
                }
            }
            lbl_801D3A30[n][k] = nBits;
        }
    }
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
    Stream_RegisterLoadChunkCallback('ter ', fn_800342B4);
    Stream_RegisterLoadChunkCallback('tgd ', fn_800342F0);
    Stream_RegisterLoadChunkCallback('tLOD', fn_800341A4);
}

void fn_80030818(void) {
    Stream_UnregisterLoadChunkCallback('ter ');
    Stream_UnregisterLoadChunkCallback('tgd ');
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
    lbl_801D3CB0.xCameraReferencePos[0] = pLens->m4[3][0];
    lbl_801D3CB0.xCameraReferencePos[1] = pLens->m4[3][1];
    lbl_801D3CB0.xCameraReferencePos[2] = pLens->m4[3][2];
    lbl_801D3CB0.xCameraReferencePos[3] = 1.0f;
    lbl_801D3CB0.xCameraLookVector[0] = pLens->m4[2][0];
    lbl_801D3CB0.xCameraLookVector[1] = pLens->m4[2][1];
    lbl_801D3CB0.xCameraLookVector[2] = pLens->m4[2][2];
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

// Builds the patch lists: clears the counts and pSortedPatchList, then takes each patch of the hole
// data that is used with the current pin position (or with any) and is not off screen, fills its
// Ter_PatchReference (fn_80031084) and objects (fn_80031154) and chains it into the lists its n1C
// bits ask for. In object test mode the whole object tree goes in as one patch.
void fn_80030CC8(void* pHoleData) {
    Ter_PatchReference* pPatch;
    s32 nCount;
    s32 iRenderPass;
    UObjMesh* pMesh;
    UObjMesh* pRoot;
    UObjMesh* pList;
    s32 eClipMethod;
    s32 nFirstObject = 0;
    u32 uPinBit = 1 << Game_CurrentPinSet();
    void* pCamera = fn_8001614C();
    s32 uFlags;
    f32 fRadius;
    f32 fDist;
    int i;
    int j;
    int k;

    lbl_801D3CB0.iTotalPatches = 0;
    lbl_801D3CB0.iTotalPostDrawTerrainPatches = 0;
    lbl_801D3CB0.iTotalSortObjects = 0;
    lbl_801D3CB0.iOpaqueObjects = 0;
    lbl_801D3CB0.iTranslucentObjects = 0;
    lbl_801D3CB0.iNearbyObjects = 0;
    lbl_801D3CB0.iDeferredItems = 0;
    lbl_801D3CB0.iPostDrawItems = 0;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 4; j++) {
            for (k = 0; k < 3; k++) {
                lbl_801D3CB0.pSortedPatchList[i][j][k] = NULL;
            }
        }
    }
    pRoot = fn_80035500(pHoleData);
    if (lbl_801D3CB0.bObjectTestMode) {
        pPatch = &lbl_801D3CB0.pPatchList[lbl_801D3CB0.iTotalPatches++];
        pPatch->fDistance = 0.0f;
        pPatch->fBoundingRadius = 0.0f;
        pPatch->eClipMethod = 1;
        pPatch->pGround = NULL;
        pPatch->pObjects = pRoot;
        fn_80031154(pPatch, 0);
        return;
    }
    if (fn_800354F4(pRoot) >= 1) {
        pList = fn_800354E4(pRoot, 1);
        nCount = fn_800354F4(pList);
        pMesh = fn_800354E4(pList, 0);
        for (i = nCount; i > 0; i--) {
            lbl_801D3CB0.iPatchFirstObjectInstanceIndex[nCount - i] = nFirstObject;
            uFlags = fn_800354D0(pMesh, 1);
            if ((uFlags & uPinBit) || !(uFlags & 0xF)) {
                if (uFlags & 0x40) {
                    iRenderPass = 1;
                } else if (uFlags & 0x80) {
                    iRenderPass = 2;
                } else {
                    iRenderPass = 0;
                }
                fRadius = fn_800354C4(pMesh)[3];
                fDist = Vec_Distance(lbl_801D3CB0.xCameraReferencePos, fn_800354C4(pMesh)) - fRadius;
                if (fDist < 0.0f) {
                    fDist = 0.0f;
                }
                eClipMethod = fn_80007B2C(pMesh, pCamera, fDist, lbl_801D3CB0.fCameraMinHalfFieldOfViewTan,
                                          fn_80017028(lbl_801D3CB0.iCurrentViewContext)->f54);
                if (eClipMethod != 3) {
                    pPatch = &lbl_801D3CB0.pPatchList[lbl_801D3CB0.iTotalPatches++];
                    fn_80031084(pMesh, eClipMethod, iRenderPass, pPatch, fDist);
                    if (pPatch->pObjects != NULL) {
                        fn_80031154(pPatch, nFirstObject);
                    }
                    if (pPatch->n1C & 1) {
                        pPatch->pNext[0] = lbl_801D3CB0.pSortedPatchList[iRenderPass][0][eClipMethod];
                        lbl_801D3CB0.pSortedPatchList[iRenderPass][0][eClipMethod] = pPatch;
                    }
                    if (pPatch->n1C & 2) {
                        pPatch->pNext[1] = lbl_801D3CB0.pSortedPatchList[iRenderPass][1][eClipMethod];
                        lbl_801D3CB0.pSortedPatchList[iRenderPass][1][eClipMethod] = pPatch;
                    }
                    if (pPatch->n1C & 4) {
                        pPatch->pNext[2] = lbl_801D3CB0.pSortedPatchList[iRenderPass][2][eClipMethod];
                        lbl_801D3CB0.pSortedPatchList[iRenderPass][2][eClipMethod] = pPatch;
                    }
                    if (pPatch->n1C & 0x80) {
                        pPatch->pNext[3] = lbl_801D3CB0.pSortedPatchList[0][3][eClipMethod];
                        lbl_801D3CB0.pSortedPatchList[0][3][eClipMethod] = pPatch;
                    }
                }
            }
            if (fn_800354F4(pMesh) >= 2) {
                nFirstObject += fn_800354F4(fn_800354E4(fn_800354E4(pMesh, 1), 0));
            }
            pMesh = fn_800354BC(pMesh);
        }
    }
}

// Sorts pObjectSortList by distance, except when gSession.b11 is set.
void fn_8003185C(void) {
    if (gSession.b11 == 0) {
        qsort(lbl_801D3CB0.pObjectSortList, lbl_801D3CB0.iTotalSortObjects, sizeof(Ter_ObjectReference),
              fn_800318AC);
    }
}

// Adds a patch's objects to pObjectSortList, numbering them from nFirstObject: each gets its three
// levels of detail (in object test mode the first list's; else lists 0, and 1 and 2 past
// iLowLODListOffset), its distance from the camera and a clip method, or is left out (clip method 3).
// Split screen leaves out objects with bit 0x8 of word 2. Hidden are: far objects with bit 0x80;
// crowd objects (word 3 bits 0x4, 0x10, 0x20) when fn_800172C4 is 0 for the view, beyond
// fCrowdHalfMaxDistanceFromGolfer from the ball, or beyond fCrowdFullMaxDistanceFromGolfer and
// farther from the pin than the ball is (every other one when nearer); and of those, tee markers
// (word 1 bits 0x1, 0x2, 0x4: tee sets 0-2) not of the player's tee set, and all of them once the
// ball is off the tee (Ball.nLie).
void fn_80031154(Ter_PatchReference* pPatch, s32 nFirstObject) {
    s32 iObject;
    f32 v48[4];
    f32 v38[4];
    f32 v28[4];
    f32 v18[4];
    f32 v8[4];
    s32 nObjects;
    s32 nLists;
    s32 i;
    s32 nLast;
    UObjMesh* pLOD0;
    UObjMesh* pLOD1;
    UObjMesh* pLOD2;
    s32 uFlags2;
    f32* pBounds;
    f32* pBall;
    f32* pPin;
    f32 fHeight;
    f32 fXZ;
    f32 fDistanceSquared;
    f32 fBallToObject;
    f32 fPinToBall;
    f32 fObjectToPin;
    u8 bHide;
    s32 eClipMethod;
    View* pView;
    Ter_ObjectReference* pRef;

    nObjects = fn_800354F4(fn_800354E4(pPatch->pObjects, 0));
    if (nObjects == 0) {
        return;
    }
    if (lbl_801D3CB0.bObjectTestMode) {
        nObjects = 1;
        nLists = fn_800354F4(fn_800354E4(pPatch->pObjects, 0));
        pLOD0 = fn_800354E4(fn_800354E4(pPatch->pObjects, 0), 0);
        pLOD1 = nLists > 1 ? fn_800354E4(fn_800354E4(pPatch->pObjects, 0), 1) : pLOD0;
        pLOD2 = nLists > 2 ? fn_800354E4(fn_800354E4(pPatch->pObjects, 0), 2) : pLOD1;
    } else {
        if (lbl_801D3CB0.iLowLODListOffset == -1) {
            if (fn_800354F4(pPatch->pObjects) == 5) {
                lbl_801D3CB0.iLowLODListOffset = 2;
            } else {
                lbl_801D3CB0.iLowLODListOffset = 0;
            }
        }
        pLOD0 = fn_800354E4(fn_800354E4(pPatch->pObjects, 0), 0);
        pLOD1 = fn_800354E4(fn_800354E4(pPatch->pObjects, lbl_801D3CB0.iLowLODListOffset + 1), 0);
        pLOD2 = fn_800354E4(fn_800354E4(pPatch->pObjects, lbl_801D3CB0.iLowLODListOffset + 2), 0);
    }
    nLast = nObjects + nFirstObject - 1;
    for (i = nObjects - 1; i >= 0; i--) {
        uFlags2 = fn_800354D0(pLOD0, 2);
        if (gSession.nSplitScreen == 0 || !(uFlags2 & 8)) {
            pBounds = fn_80035508(pLOD0);
            fHeight = fabsf(lbl_801D3CB0.xCameraReferencePos[1] - pBounds[1]) - pBounds[7];
            if (fHeight < 0.0f) {
                fHeight = 0.0f;
            }
            fn_8003546C(pBounds, lbl_801D3CB0.xCameraReferencePos, v48);
            v48[1] = 0.0f;
            fXZ = (f32)fn_80009680(fn_80009744(v48)) - pBounds[3];
            if (fXZ < 0.0f) {
                fXZ = 0.0f;
            }
            fDistanceSquared = fHeight * fHeight + fXZ * fXZ;
            if (fDistanceSquared <= 0.0f) {
                fn_80063920(lbl_801D3CB0.iCurrentViewContext, pBounds);
            }
            bHide = 0;
            if ((uFlags2 & 0x80) && fDistanceSquared > lbl_801D3CB0.fDistanceCullFrameYardsSquared) {
                bHide = 1;
            } else if ((fn_800354D0(pLOD0, 3) & 4) || (fn_800354D0(pLOD0, 3) & 0x10)
                       || (fn_800354D0(pLOD0, 3) & 0x20)) {
                pBall = gPlayers[fn_8001707C(lbl_801D3CB0.iCurrentViewContext)].vBall;
                pPin = &fn_8000C594()->pin[Game_CurrentPinSet()].x;
                fn_8003546C(pBounds, pBall, v28);
                v28[1] = 0.0f;
                fBallToObject = (f32)fn_80009680(fn_80009744(v28)) - pBounds[3];
                if (fBallToObject < 0.0f) {
                    fBallToObject = 0.0f;
                }
                fn_8003546C(pPin, pBall, v18);
                v18[1] = 0.0f;
                fPinToBall = fn_80009680(fn_80009744(v18));
                if (fPinToBall < 0.0f) {
                    fPinToBall = 0.0f;
                }
                fn_8003546C(pBounds, pPin, v8);
                v8[1] = 0.0f;
                fObjectToPin = (f32)fn_80009680(fn_80009744(v8)) - pBounds[3];
                if (fObjectToPin < 0.0f) {
                    fObjectToPin = 0.0f;
                }
                if (!fn_800172C4(fn_80017028(lbl_801D3CB0.iCurrentViewContext))) {
                    bHide = 1;
                } else if (fBallToObject > lbl_801D3CB0.fCrowdHalfMaxDistanceFromGolfer) {
                    bHide = 1;
                } else if (fBallToObject > lbl_801D3CB0.fCrowdFullMaxDistanceFromGolfer
                           && fObjectToPin > fPinToBall) {
                    bHide = 1;
                } else if (fBallToObject > lbl_801D3CB0.fCrowdFullMaxDistanceFromGolfer
                           && fObjectToPin <= fPinToBall && (nLast - i) % 2 != 0) {
                    bHide = 1;
                } else if (((fn_800354D0(pLOD0, 1) & 1)
                            && gSession.nTeeSet[fn_8001707C(lbl_801D3CB0.iCurrentViewContext)] != 0)
                           || ((fn_800354D0(pLOD0, 1) & 2)
                               && gSession.nTeeSet[fn_8001707C(lbl_801D3CB0.iCurrentViewContext)] != 1)
                           || ((fn_800354D0(pLOD0, 1) & 4)
                               && gSession.nTeeSet[fn_8001707C(lbl_801D3CB0.iCurrentViewContext)] != 2)) {
                    bHide = 1;
                } else if (((fn_800354D0(pLOD0, 1) & 1) || (fn_800354D0(pLOD0, 1) & 2)
                            || (fn_800354D0(pLOD0, 1) & 4))
                           && gPlayers[fn_8001707C(lbl_801D3CB0.iCurrentViewContext)].ball.nLie != 0) {
                    bHide = 1;
                }
            }
            if (bHide) {
                eClipMethod = 3;
            } else if (pPatch->eClipMethod == 2) {
                eClipMethod = 2;
            } else {
                pView = fn_80017028(lbl_801D3CB0.iCurrentViewContext);
                eClipMethod = fn_80007B2C(pLOD0, fn_8001614C(), fn_80009680(fDistanceSquared),
                                          lbl_801D3CB0.fCameraMinHalfFieldOfViewTan, pView->f54);
            }
            if (eClipMethod != 3) {
                pRef = &lbl_801D3CB0.pObjectSortList[lbl_801D3CB0.iTotalSortObjects];
                pRef->fDistanceSquared = fDistanceSquared;
                pRef->f14 = fn_80009744(v48);
                fn_8003546C(pBounds, lbl_801D3CB0.xCameraReferencePos, v38);
                pRef->f18 = fn_8000C5FC(lbl_801D3CB0.xCameraLookVector, v38);
                iObject = nLast - i;
                pRef->eClipMethod = eClipMethod;
                pRef->pContainerPatch = pPatch;
                pRef->iGlobalObjectIndex = iObject;
                lbl_801D3CB0.pObjectStateList[iObject].a20[0] = fn_800354D0(pLOD0, 0);
                lbl_801D3CB0.pObjectStateList[iObject].a20[1] = fn_800354D0(pLOD0, 1);
                lbl_801D3CB0.pObjectStateList[iObject].a20[2] = fn_800354D0(pLOD0, 2);
                lbl_801D3CB0.pObjectStateList[iObject].a20[3] = fn_800354D0(pLOD0, 3);
                if ((uFlags2 & 0x40) || (fn_800354D0(pLOD0, 3) & 0x10) || (fn_800354D0(pLOD0, 3) & 0x20)) {
                    pRef->nLODs = 1;
                    pRef->apObject[0] = pLOD0;
                } else {
                    pRef->nLODs = 3;
                    pRef->apObject[0] = pLOD0;
                    pRef->apObject[1] = pLOD1;
                    pRef->apObject[2] = pLOD2;
                }
                lbl_801D3CB0.iTotalSortObjects++;
            }
        }
        pLOD0 = fn_800354BC(pLOD0);
        pLOD1 = fn_800354BC(pLOD1);
        pLOD2 = fn_800354BC(pLOD2);
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

// Picks each sorted object's level of detail by its distance: the first LOD plane whose end it is
// inside. Once the 'tLOD' chunk is loaded, objects whose flags (bits 0x4, 0x10, 0x20 of the model's
// word 3) ask for it always get level 0, and the others never get level 0 when fn_800172C4 is 0
// for the view.
// Unless gSession.b11 is set, an object between two planes fades from one level into the next.
void fn_80031AB4(void) {
    Ter_LODPlane* pPlanes = lbl_801D3CB0.LODPlanes;
    f32 fT;
    f32 fAlpha;
    f32 fDistanceSquared;
    s32 uFlags;
    s32 nLast;
    s32 nTranslucent;
    s32 nLOD;
    s32 i;

    if (gSession.b11 != 0) {
        fAlpha = 0.0f;
        for (i = lbl_801D3CB0.iTotalSortObjects - 1; i >= 0; i--) {
            s32 iObject;
            iObject = lbl_801D3CB0.pObjectSortList[i].iGlobalObjectIndex;
            fDistanceSquared = lbl_801D3CB0.pObjectSortList[i].fDistanceSquared;
            nLast = lbl_801D3CB0.pObjectSortList[i].nLODs - 1;
            if (((lbl_801D3CB0.pObjectStateList[iObject].a20[3] & 4) ||
                 (lbl_801D3CB0.pObjectStateList[iObject].a20[3] & 0x10) ||
                 (lbl_801D3CB0.pObjectStateList[iObject].a20[3] & 0x20)) &&
                fn_80031E40()) {
                nLOD = 0;
            } else {
                for (nLOD = 0; nLOD < nLast; nLOD++) {
                    if (fDistanceSquared <= pPlanes[nLOD].fEnd * pPlanes[nLOD].fEnd) break;
                }
                if (!fn_800172C4(fn_80017028(lbl_801D3CB0.iCurrentViewContext))) {
                    uFlags = lbl_801D3CB0.pObjectStateList[iObject].a20[3];
                    if (!(uFlags & 4) && !(uFlags & 0x10) && !(uFlags & 0x20) && fn_80031E40() && nLOD == 0) {
                        nLOD = 1;
                    }
                }
            }
            lbl_801D3CB0.pObjectSortList[i].fAlpha = fAlpha;
            lbl_801D3CB0.pObjectSortList[i].iOpaqueLOD = nLOD;
            lbl_801D3CB0.pObjectSortList[i].iTranslucentLOD = nLOD;
        }
        return;
    }
    for (i = lbl_801D3CB0.iTotalSortObjects - 1; i >= 0; i--) {
        s32 iObject;
        fDistanceSquared = lbl_801D3CB0.pObjectSortList[i].fDistanceSquared;
        iObject = lbl_801D3CB0.pObjectSortList[i].iGlobalObjectIndex;
        nLast = lbl_801D3CB0.pObjectSortList[i].nLODs - 1;
        for (nLOD = 0; nLOD < nLast; nLOD++) {
            if (fDistanceSquared <= pPlanes[nLOD].fEnd * pPlanes[nLOD].fEnd) break;
        }
        if (!fn_800172C4(fn_80017028(lbl_801D3CB0.iCurrentViewContext))) {
            uFlags = lbl_801D3CB0.pObjectStateList[iObject].a20[3];
            if (!(uFlags & 4) && !(uFlags & 0x10) && !(uFlags & 0x20) && fn_80031E40() && nLOD == 0) {
                nLOD = 1;
            }
        }
        if (nLOD < nLast
            && fDistanceSquared > pPlanes[nLOD + 1].fBegin * pPlanes[nLOD + 1].fBegin) {
            // between this plane's end and the next one's start: fade over to the next level
            fT = (f32)fn_80009680(fDistanceSquared);
            fT = (fT - pPlanes[nLOD + 1].fBegin) / (pPlanes[nLOD].fEnd - pPlanes[nLOD + 1].fBegin);
            if (fT < 0.5f) {
                nTranslucent = nLOD + 1;
                fAlpha = 2.0f * fT;
            } else {
                nTranslucent = nLOD;
                nLOD++;
                fAlpha = 2.0f * (1.0f - fT);
            }
            if (fAlpha < 0.0f) {
                fAlpha = 0.0f;
            }
            if (fAlpha > 1.0f) {
                fAlpha = 1.0f;
            }
            uFlags = lbl_801D3CB0.pObjectStateList[iObject].a20[3];
            if (((uFlags & 4) || (uFlags & 0x10) || (uFlags & 0x20)) && fn_80031E40()) {
                fAlpha = 0.0f;
                nTranslucent = 0;
                nLOD = 0;
            }
        } else {
            fAlpha = 0.0f;
            uFlags = lbl_801D3CB0.pObjectStateList[iObject].a20[3];
            if (((uFlags & 4) || (uFlags & 0x10) || (uFlags & 0x20)) && fn_80031E40()) {
                nLOD = 0;
            }
            nTranslucent = nLOD;
        }
        lbl_801D3CB0.pObjectSortList[i].iOpaqueLOD = nLOD;
        lbl_801D3CB0.pObjectSortList[i].iTranslucentLOD = nTranslucent;
        lbl_801D3CB0.pObjectSortList[i].fAlpha = fAlpha;
    }
}

// Whether the 'tLOD' chunk has been loaded.
u8 fn_80031E40(void) {
    return lbl_802810E4 != -1;
}

// Sorts the objects (farthest first) into the draw lists: post-draw objects (bit 0x80 of the
// model's word 2) straight to pPostDrawItemsList; the others opaque when far or when they must
// stay solid, faded in over the near range (pNearbyObjectList), and their fading level into
// pTranslucentObjectList. Crowd objects (bit 0x20 of word 0, or 0x10 or 0x20 of word 3; not when
// fn_800172C4 is 0 for the view) use the crowd's fade distances.
void fn_80031E58(void) {
    s32 i;
    UObjMesh* pModel;
    s32 uFlags0;
    s32 uFlags2;
    s32 uCrowd;
    f32 fNear;
    f32 fFar;
    f32 fFarSquared;
    f32 fRange;
    f32 fDistance;
    f32 fT;
    Ter_ObjectReference* pRef;

    for (i = lbl_801D3CB0.iTotalSortObjects - 1; i >= 0; i--) {
        pRef = &lbl_801D3CB0.pObjectSortList[i];
        pModel = pRef->apObject[pRef->iOpaqueLOD];
        uFlags0 = fn_800354D0(pModel, 0);
        uFlags2 = fn_800354D0(pModel, 2);
        if ((fn_800354D0(pModel, 3) & 0x10) || (fn_800354D0(pModel, 3) & 0x20)) {
            uFlags0 |= 0x20;
            uFlags2 &= ~0x80;
            uFlags0 &= ~0x40;
        }
        if (!fn_800172C4(fn_80017028(lbl_801D3CB0.iCurrentViewContext))) {
            uFlags0 &= ~0x20;
        }
        uCrowd = uFlags0 & 0x20;
        if (uCrowd == 0 && !(uFlags0 & 0x40)) {
            fNear = 0.1f * lbl_801D3CB0.fFOVScale;
            fFar = 0.2f * lbl_801D3CB0.fFOVScale;
            fFarSquared = fFar * fFar;
            fRange = fFar - fNear;
        } else if (uCrowd != 0 && !(uFlags0 & 0x40)) {
            fNear = lbl_801D3CB0.fCrowdFadeDistanceMin * lbl_801D3CB0.fFOVScale;
            fFar = lbl_801D3CB0.fCrowdFadeDistanceMax * lbl_801D3CB0.fFOVScale;
            if (fNear < 0.1f) {
                fNear = 0.1f;
            }
            fFarSquared = fFar * fFar;
            fRange = fFar - fNear;
        } else {
            fNear = -100.0f;
            fFar = -101.0f;
            fFarSquared = 0.0f;
            fRange = 1.0f;
        }
        // every draw re-reads the list pointer, as the original does
        if (uFlags2 & 0x80) {
            pRef = &lbl_801D3CB0.pObjectSortList[i];
            fn_8003241C(&lbl_801D3CB0.pPostDrawItemsList[lbl_801D3CB0.iPostDrawItems],
                        &lbl_801D3CB0.iPostDrawItems, 400, pRef->apObject[pRef->iOpaqueLOD],
                        pRef->iGlobalObjectIndex, pRef->eClipMethod, pRef->fDistanceSquared > 0.0f, 0, 1.0f,
                        lbl_801D3CB0.fDefaultObjectMipmapBias[pRef->iOpaqueLOD], pRef->fDistanceSquared);
        } else if (lbl_801D3CB0.pObjectSortList[i].fDistanceSquared < fFarSquared) {
            fDistance = fn_80009680(lbl_801D3CB0.pObjectSortList[i].fDistanceSquared);
            pRef = &lbl_801D3CB0.pObjectSortList[i];
            if (fDistance > fFar || (uFlags0 & 0x40)
                || (uCrowd == 0
                    && ((pRef->f14 > lbl_801D3CB0.fXZDistanceToClosestBallSquared && pRef->f18 > 0.0f)
                        || fn_80032330(pRef->apObject[0])))) {
                lbl_801D3CB0.pObjectStateList[pRef->iGlobalObjectIndex]
                    .aView[lbl_801D3CB0.iCurrentViewContext].n4 = 3;
                lbl_801D3CB0.pObjectStateList[pRef->iGlobalObjectIndex]
                    .aView[lbl_801D3CB0.iCurrentViewContext].f0 = 1.0f;
                pRef = &lbl_801D3CB0.pObjectSortList[i];
                fn_8003241C(&lbl_801D3CB0.pOpaqueObjectList[lbl_801D3CB0.iOpaqueObjects],
                            &lbl_801D3CB0.iOpaqueObjects, 650, pRef->apObject[pRef->iOpaqueLOD],
                            pRef->iGlobalObjectIndex, pRef->eClipMethod, pRef->fDistanceSquared > 0.0f, 0,
                            1.0f, lbl_801D3CB0.fDefaultObjectMipmapBias[pRef->iOpaqueLOD],
                            pRef->fDistanceSquared);
            } else {
                fT = (fDistance - fNear) / fRange;
                if (fT < 0.0f) {
                    fT = 0.0f;
                }
                if (fT != 0.0f) {
                    pRef = &lbl_801D3CB0.pObjectSortList[i];
                    fn_8003241C(&lbl_801D3CB0.pNearbyObjectList[lbl_801D3CB0.iNearbyObjects],
                                &lbl_801D3CB0.iNearbyObjects, 70, pRef->apObject[pRef->iOpaqueLOD],
                                pRef->iGlobalObjectIndex, pRef->eClipMethod, pRef->fDistanceSquared > 0.0f,
                                0, fT, lbl_801D3CB0.fDefaultObjectMipmapBias[pRef->iOpaqueLOD],
                                pRef->fDistanceSquared);
                }
            }
        } else {
            pRef = &lbl_801D3CB0.pObjectSortList[i];
            lbl_801D3CB0.pObjectStateList[pRef->iGlobalObjectIndex]
                .aView[lbl_801D3CB0.iCurrentViewContext].n4 = 3;
            pRef = &lbl_801D3CB0.pObjectSortList[i];
            fn_8003241C(&lbl_801D3CB0.pOpaqueObjectList[lbl_801D3CB0.iOpaqueObjects],
                        &lbl_801D3CB0.iOpaqueObjects, 650, pRef->apObject[pRef->iOpaqueLOD],
                        pRef->iGlobalObjectIndex, pRef->eClipMethod, pRef->fDistanceSquared > 0.0f, 0, 1.0f,
                        lbl_801D3CB0.fDefaultObjectMipmapBias[pRef->iOpaqueLOD], pRef->fDistanceSquared);
        }
        if (gSession.b11 == 0) {
            pRef = &lbl_801D3CB0.pObjectSortList[i];
            if (pRef->fAlpha != 0.0f && !(uFlags0 & 0x40)) {
                fn_8003241C(&lbl_801D3CB0.pTranslucentObjectList[lbl_801D3CB0.iTranslucentObjects],
                            &lbl_801D3CB0.iTranslucentObjects, 200, pRef->apObject[pRef->iTranslucentLOD],
                            pRef->iGlobalObjectIndex, pRef->eClipMethod, pRef->fDistanceSquared > 0.0f, 0,
                            pRef->fAlpha, lbl_801D3CB0.fDefaultObjectMipmapBias[pRef->iTranslucentLOD],
                            pRef->fDistanceSquared);
            }
        }
    }
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

    // nUnused: every caller passes its list's size (650, 400, 300, 200, 70 or 50); it is not read
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

// Draws one patch's ground in render pass nPass: its mesh for the pass and the patch's bits (n1C
// bits 0-2 and 0x80, n18 bit 1), then the extra meshes its word 2 asks for: bit 0x8 drawn at once,
// bits 0x10 and 0x20 as deferred items, bit 0x40 (near enough, not in split screen) raised by
// 0.005 without z writes. Passes 1 and 2 leave out patches beyond 100 x fFOVScale unless the
// ground's bit 0x80 is set. A lake surface (n20 bit 0x80) uses fLakeSurfaceMipmapBias; when
// fn_800172C4 is 0 for the view, one with ground bit 0x80 is left out. *pbFirst tracks a renderer
// state switched by fFar; b2 keeps it, the bit 0x10 deferred mesh and the raised mesh out. b1 is
// not read.
void fn_80032B7C(void* pGround, s32 eClipMethod, s32 nPass, s32 n1C, s32 n18, s32 n20, u8* pbFirst, u8 b1,
                 u8 b2, f32 fNear, f32 fFar) {
    f32 mRaise[4][4];
    u8 bFirst;
    s32 uFlags2;
    UObjMesh* pMesh;
    u8 nMesh;
    u8 bPinSet;
    u8 bLake;
    u32 uPinBit;
    u32 uOtherPins;
    f32 fBias;

    bPinSet = 0;
    bLake = 0;
    uPinBit = 1 << Game_CurrentPinSet();
    uOtherPins = ~(uPinBit | uPinBit) & 0xF;
    if (nPass >= 1 && nPass <= 2 && fNear > 100.0f * lbl_801D3CB0.fFOVScale
        && !(fn_800354D0(pGround, 0) & 0x80)) {
        return;
    }
    nMesh = n1C & 7;
    if (n1C & 0x80) {
        nMesh |= 8;
    }
    if (n18 & 2) {
        nMesh |= 0x10;
    }
    if (n20 & 0x80) {
        if (!fn_800172C4(fn_80017028(lbl_801D3CB0.iCurrentViewContext)) && (fn_800354D0(pGround, 0) & 0x80)) {
            return;
        }
        bLake = 1;
        fBias = lbl_801D3CB0.fLakeSurfaceMipmapBias;
    }
    pGround = fn_800354E4(pGround, lbl_801D3A30[nPass][nMesh]);
    bFirst = *pbFirst;
    if (bFirst == 0 && fFar > 0.0f && b2 == 0) {
        *pbFirst = 1;
        fn_80014118(0x70);
        fn_80012EF8();
    } else if ((fFar <= 0.0f || b2 != 0) && bFirst != 0) {
        *pbFirst = 0;
        fn_80014118(0x50);
        fn_80012EF8();
    }
    uFlags2 = fn_800354D0(pGround, 2);
    pMesh = fn_800354E4(pGround, 0);
    if ((n20 & uPinBit) && !(n20 & uOtherPins)) {
        fn_80012F50(1, 6, 1);
        fn_80012EF8();
        bPinSet = 1;
    }
    if (bLake) {
        fn_80012EF8();
    }
    if (uFlags2 & 8) {
        fn_80035514((u8*)pMesh);
        pMesh = fn_800354BC(pMesh);
    }
    if (bLake) {
        fn_80012EF8();
    }
    if ((uFlags2 & 0x10) && b2 == 0) {
        if (pMesh->n20 != 0) {
            fn_8003241C(&lbl_801D3CB0.pDeferredItemsList[lbl_801D3CB0.iDeferredItems],
                        &lbl_801D3CB0.iDeferredItems, 50, pMesh, 0x289, eClipMethod, fFar > 0.0f, 0, 1.0f,
                        bLake ? fBias : 0.0f, 0.0f);
        }
        pMesh = fn_800354BC(pMesh);
    }
    if (uFlags2 & 0x20) {
        if (pMesh->n20 != 0) {
            fn_8003241C(&lbl_801D3CB0.pDeferredItemsList[lbl_801D3CB0.iDeferredItems],
                        &lbl_801D3CB0.iDeferredItems, 50, pMesh, 0x289, eClipMethod, fFar > 0.0f, 0, 1.0f,
                        bLake ? fBias : 0.0f, 0.0f);
        }
        pMesh = fn_800354BC(pMesh);
    }
    if (gSession.nSplitScreen == 0 && (uFlags2 & 0x40) && b2 == 0) {
        if (fNear < 60.0f * lbl_801D3CB0.fFOVScale && pMesh->n20 != 0) {
            fn_8000ADC0(mRaise);
            mRaise[3][1] = 0.005f;
            fn_80035240(mRaise);
            fn_80016B9C();
            fn_80012F34(0);
            fn_80012EF8();
            fn_80035514((u8*)pMesh);
            fn_80012F34(1);
            fn_80035240(NULL);
            fn_80016B9C();
            fn_80012EF8();
        }
        fn_800354BC(pMesh);
    }
    if (bPinSet) {
        fn_80012F50(0, 6, 1);
        fn_80012EF8();
    }
}

// Draws nCount objects of a draw list, switching the renderer state only when it changes from one
// object to the next: the clip method, the mipmap bias, and the flags (0x40, fog 0x20, and 0x10 for
// shader types other than 1 and 3) unless the object sets its own. Objects whose state word 0 has
// bit 0x1 hand their f4 to row 2 or 3 of fn_8003519C (by shader type); without bit 0x2 its swing
// about 0.5 is cut by up to fTreeDampingMaxForce up close, less with distance (not at all from the
// squared distance fTreeDampingDistance on). The filters are not read.
void fn_80032F88(Ter_ObjectDrawData* pList, s32 nCount, s32 eFilterMin, s32 eFilterMag) {
    f32 fWave2;
    f32 fWave3;
    Ter_ObjectDrawData* pDraw;
    s32 iObject;
    f32 fBias = 100.0f;
    s32 eClipMethod = 3;
    u8 bUseFog = 1;
    u8 bBit5 = 0;
    u8 bShaded = 1;
    u8 bDirty = 0;
    u8 bForce = 1;
    s32 i;
    u8 bNewBit5;
    u8 bNewShaded;
    u32 uState;
    f32 fDamp;

    fn_800332F4();
    pDraw = pList;
    for (i = 0; i < nCount; i++) {
        iObject = pDraw->iGlobalObjectIndex;
        if (eClipMethod != pDraw->eClipMethod) {
            eClipMethod = pDraw->eClipMethod;
            switch (eClipMethod) {
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
            bDirty = 1;
        }
        if (fBias != pDraw->fMipmapBias) {
            fBias = pDraw->fMipmapBias;
            bDirty = 1;
        }
        bNewBit5 = (lbl_801D3CB0.pObjectStateList[iObject].a20[1] >> 5) & 1;
        bNewShaded = 0;
        if (pDraw->eShaderObjectType != 1 && pDraw->eShaderObjectType != 3) {
            bNewShaded = 1;
        }
        if (pDraw->bSetsPrimField == 0
            && (bUseFog != pDraw->bUseFog || bBit5 != bNewBit5 || bShaded != bNewShaded || bForce)) {
            bUseFog = pDraw->bUseFog;
            bBit5 = bNewBit5;
            bShaded = bNewShaded;
            fn_80014118((bShaded ? 0x10 : 0) | (bUseFog ? 0x20 : 0) | 0x40);
            bForce = 0;
            bDirty = 1;
        }
        if (fn_80033308(pDraw, 0)) {
            bDirty = 1;
        }
        uState = lbl_801D3CB0.pObjectStateList[iObject].a20[0];
        if ((uState & 1) && !(uState & 2)) {
            fDamp = 1.0f;
            if (lbl_801D3CB0.fTreeDampingMaxForce) {
                fDamp = lbl_801D3CB0.fTreeDampingMaxForce
                        * (pDraw->fDistanceSquared / lbl_801D3CB0.fTreeDampingDistance)
                      + (1.0f - lbl_801D3CB0.fTreeDampingMaxForce);
                if (fDamp > 1.0f) {
                    fDamp = 1.0f;
                }
            }
            if (bDirty) {
                fn_80012EF8();
                bDirty = 0;
            }
            if (pDraw->eShaderObjectType == 2) {
                fWave2 = fDamp * (lbl_801D3CB0.pObjectStateList[iObject].f4 - 0.5f) + 0.5f;
                fn_8003519C(2, &fWave2);
            } else if (pDraw->eShaderObjectType == 3) {
                fWave3 = fDamp * (lbl_801D3CB0.pObjectStateList[iObject].f4 - 0.5f) + 0.5f;
                fn_8003519C(3, &fWave3);
            }
        } else if ((uState & 1) && (uState & 2)) {
            if (bDirty) {
                fn_80012EF8();
                bDirty = 0;
            }
            if (pDraw->eShaderObjectType == 2) {
                fWave2 = lbl_801D3CB0.pObjectStateList[iObject].f4;
                fn_8003519C(2, &fWave2);
            } else if (pDraw->eShaderObjectType == 3) {
                fWave3 = lbl_801D3CB0.pObjectStateList[iObject].f4;
                fn_8003519C(3, &fWave3);
            }
        } else if (bDirty) {
            fn_80012EF8();
            bDirty = 0;
        }
        fn_80035514((u8*)pDraw->pObject);
        if (pDraw->bSetsPrimField) {
            bForce = 1;
        }
        pDraw++;
    }
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

// Animates the course objects once a frame (the frame time capped at 1/30 s): runs the crowd
// countdowns, fades each object's views in (state 2) or out (state 0), and moves each object with
// bit 0x1 of word 0: crowd members (bit 0x2) held in iCrowdPose, swaying in their pose or easing to
// the next one through lbl_801877E0 (lbl_80187858 with bit 0x40 of word 3); bit 0x1 of word 3 rises
// to 1 once n1C is 1; the trees sway by their period with noise.
void fn_80033744(void) {
    f32 fDrop;
    f32 fTime;
    f32 fUpFast;
    f32 fDownFast;
    f32 fUp;
    f32 fDown;
    u32 nFrames;
    s32 i;
    s32 v;
    s32 k;
    u32 uFlags0;
    u32 uFlags3;
    f32 fPeriod;
    f32 fStep;
    f32 fNoise;
    f32 fSum;
    f32 fScale;
    f32 fWave;
    Ter_ObjectState* pState;

    fTime = gSession.fFrameTime;
    if (fTime > 1.0f / 30.0f) {
        fTime = 1.0f / 30.0f;
    }
    if (fTime < 0.0f) {
        fTime = 0.0f;
    }
    if (lbl_801D3CB0.fCrowdAnimationDelayedStartTimer > 0.0f) {
        lbl_801D3CB0.fCrowdAnimationDelayedStartTimer -= fTime;
        if (lbl_801D3CB0.fCrowdAnimationDelayedStartTimer <= 0.0f) {
            fn_8003349C(lbl_801D3CB0.fCrowdAnimationDelayedStartPercentage,
                        lbl_801D3CB0.fCrowdAnimationDelayedStartDuration, 0.0f);
        }
    }
    if (lbl_801D3CB0.fCrowdAnimationCountdown > 0.0f) {
        lbl_801D3CB0.fCrowdAnimationCountdown -= fTime;
        if (lbl_801D3CB0.fCrowdAnimationCountdown <= 0.0f) {
            fn_800335F8(0);
        }
    }
    nFrames = FRAME_RATE * fTime;
    fUpFast = 6.0f * fTime;
    fDownFast = -6.0f * fTime;
    fUp = 4.0f * fTime;
    fDown = -4.0f * fTime;
    fDrop = 5.0f * fTime;
    for (i = 0; i < TER_NUM_OBJECTS; i++) {
        for (v = 0; v < 2; v++) {
            if (lbl_801D3CB0.pObjectStateList[i].aView[v].n4 == 2) {
                lbl_801D3CB0.pObjectStateList[i].aView[v].f0 += 4.0f * gSession.fFrameTime;
                if (lbl_801D3CB0.pObjectStateList[i].aView[v].f0 >= 1.0f) {
                    lbl_801D3CB0.pObjectStateList[i].aView[v].f0 = 1.0f;
                    lbl_801D3CB0.pObjectStateList[i].aView[v].n4 = 3;
                }
            }
            if (lbl_801D3CB0.pObjectStateList[i].aView[v].n4 == 0) {
                lbl_801D3CB0.pObjectStateList[i].aView[v].f0 -= 4.0f * gSession.fFrameTime;
                if (lbl_801D3CB0.pObjectStateList[i].aView[v].f0 <= 0.0f) {
                    lbl_801D3CB0.pObjectStateList[i].aView[v].f0 = 0.0f;
                    lbl_801D3CB0.pObjectStateList[i].aView[v].n4 = 1;
                }
            }
        }
        pState = &lbl_801D3CB0.pObjectStateList[i];
        uFlags0 = pState->a20[0];
        uFlags3 = pState->a20[3];
        if (!(uFlags0 & 1)) {
            continue;
        }
        if ((uFlags0 & 2) && lbl_801D3CB0.iCrowdPose != 0) {
            if (lbl_801D3CB0.iCrowdPose == 1) {
                pState->n18 = 0;
                pState->n1C = 0;
            } else {
                pState->n18 = 3;
                pState->n1C = 3;
            }
            pState->f4 = lbl_801D3CB0.fCrowdInterpValue;
        } else if ((uFlags0 & 2) && !(uFlags3 & 0x40)) {
            pState->nC += nFrames;
            if (pState->n18 == pState->n1C || pState->f14 > 0.0f) {
                fPeriod = 4.0f * (pState->f0 - lbl_801D3CB0.fTreeMinPeriod) + 1.5f;
                if (pState->n18 == 3) {
                    pState->f4 =
                        0.5f * fn_800095F0(10.0f * (6.2831855f * fn_800351D8(pState->nC, fPeriod / 10.0f))
                                           / fPeriod)
                        + 0.5f;
                } else {
                    pState->f4 =
                        0.5f * fn_800095F0(0.5f * (6.2831855f * fn_800351D8(pState->nC, fPeriod / 0.5f))
                                           / fPeriod)
                        + 0.5f;
                }
                pState->f14 -= fTime;
            } else {
                for (k = 0; k < 6; k++) {
                    if (pState->n18 == lbl_801877E0[k].n0 && pState->n1C == lbl_801877E0[k].n4) {
                        fStep = lbl_801877E0[k].fC - pState->f4;
                        if (pState->n18 == 2 || pState->n18 == 3) {
                            if (fStep > fUpFast) {
                                fStep = fUpFast;
                            }
                            if (fStep < fDownFast) {
                                fStep = fDownFast;
                            }
                        } else {
                            if (fStep > fUp) {
                                fStep = fUp;
                            }
                            if (fStep < fDown) {
                                fStep = fDown;
                            }
                        }
                        pState->f4 += fStep;
                        if (fabsf(pState->f4 - lbl_801877E0[k].fC) < 0.01f) {
                            pState->n18 = lbl_801877E0[k].n8;
                            pState->f4 = lbl_801877E0[k].f10;
                            pState->nC = 0;
                            break;
                        }
                    }
                }
            }
        } else if ((uFlags0 & 2) && (uFlags3 & 0x40)) {
            pState->nC += nFrames;
            if (pState->n18 == pState->n1C || pState->f14 > 0.0f) {
                if (pState->n18 == 0) {
                    pState->f4 -= fDrop;
                    if (pState->f4 < 0.0f) {
                        pState->f4 = 0.0f;
                    }
                    pState->f14 -= fTime;
                } else {
                    fPeriod = 4.0f * (pState->f0 - lbl_801D3CB0.fTreeMinPeriod) + 1.5f;
                    if (pState->n18 == 1) {
                        pState->f4 =
                            0.5f * fn_800095F0(10.0f * (6.2831855f * fn_800351D8(pState->nC, fPeriod / 10.0f))
                                               / fPeriod)
                            + 0.5f;
                    } else {
                        pState->f4 =
                            0.5f * fn_800095F0(0.5f * (6.2831855f * fn_800351D8(pState->nC, fPeriod / 0.5f))
                                               / fPeriod)
                            + 0.5f;
                    }
                    pState->f14 -= fTime;
                }
            } else {
                for (k = 0; k < 2; k++) {
                    if (pState->n18 == lbl_80187858[k].n0 && pState->n1C == lbl_80187858[k].n4) {
                        fStep = lbl_80187858[k].fC - pState->f4;
                        if (pState->n18 == 2 || pState->n18 == 3) {
                            if (fStep > fUpFast) {
                                fStep = fUpFast;
                            }
                            if (fStep < fDownFast) {
                                fStep = fDownFast;
                            }
                        } else {
                            if (fStep > fUp) {
                                fStep = fUp;
                            }
                            if (fStep < fDown) {
                                fStep = fDown;
                            }
                        }
                        pState->f4 += fStep;
                        if (fabsf(pState->f4 - lbl_80187858[k].fC) < 0.01f) {
                            pState->n18 = lbl_80187858[k].n8;
                            pState->f4 = lbl_80187858[k].f10;
                            pState->nC = 0;
                            break;
                        }
                    }
                }
            }
        } else if (uFlags3 & 1) {
            if (pState->n1C == 1) {
                pState->f4 += fTime;
                if (pState->f4 > 1.0f) {
                    pState->f4 = 1.0f;
                }
            } else {
                pState->f4 = 0.0f;
            }
        } else {
            fNoise = lbl_801D3CB0.fTreeNoiseAmplitudeScale;
            fSum = 1.0f + fNoise;
            fScale = 1.0f / fSum;
            fWave = 0.5f * (fScale * fNoise)
                    * fn_800095F0(6.2831855f
                                  * fn_800351D8(gSession.nFrameCount,
                                                lbl_801D3CB0.fTreeNoisePeriodScale * pState->f0)
                                  / (lbl_801D3CB0.fTreeNoisePeriodScale * pState->f0));
            pState->f4 = 0.5f * fScale
                             * fn_800095F0(6.2831855f * fn_800351D8(gSession.nFrameCount, pState->f0)
                                           / pState->f0)
                       + fWave;
            pState->f4 = pState->f4 * lbl_801D3CB0.fTreeOverdrive;
            pState->f4 = pState->f4 + 0.5f;
        }
    }
}

// Draws object list nList of the hole data (0 before the patches, 2 after them) when its switch is
// on, as panorama items: each object that is not off screen, except (list 0) objects 0-1 or 2-3 by
// lbl_802811F0's flag 0x2, (list 2) the last lbl_80281D64, and in split screen those with flag 8.
void fn_80033F94(void* pHoleData, u32 nList) {
    UObjMesh* pRoot;
    UObjMesh* pList;
    UObjMesh* pMesh;
    View* pView;
    s32 nItems;
    s32 nCount;
    s32 uFlags;
    s32 nClip;
    int i;

    if ((nList != 0 || lbl_802810EC) && (nList != 2 || lbl_802810ED)) {
        pRoot = fn_80035500(pHoleData);
        nItems = 0;
        if (fn_800354F4(pRoot) >= (s32)(nList + 1)) {
            pList = fn_800354E4(pRoot, nList);
            nCount = fn_800354F4(pList);
            pMesh = fn_800354E4(pList, 0);
            for (i = 0; i < nCount; i++) {
                if (nList == 0) {
                    if (fn_80035574()) {
                        if (i >= 0 && i <= 1) {
                            pMesh = fn_800354BC(pMesh);
                            continue;
                        }
                    } else if (i >= 2 && i <= 3) {
                        pMesh = fn_800354BC(pMesh);
                        continue;
                    }
                }
                if (nList == 2 && i >= nCount - lbl_80281D64) {
                    pMesh = fn_800354BC(pMesh);
                    continue;
                }
                uFlags = fn_800354D0(pMesh, 2);
                if (!gSession.nSplitScreen || !(uFlags & 8)) {
                    pView = fn_80017028(lbl_801D3CB0.iCurrentViewContext);
                    nClip = fn_80007B2C(pMesh, fn_8001614C(), 0.0f, lbl_801D3CB0.fCameraMinHalfFieldOfViewTan,
                                        pView->f54);
                    if (nClip != 3) {
                        // fake match: uFlags is reused for bUseFog (fog unless flag 0x20); a new local
                        // is computed after the call to fn_80035560, the original before it
                        uFlags = ((uFlags & 0x20) >> 5) ^ 1;
                        fn_8003241C(&lbl_801D3CB0.pPanoramaItemsList[nItems], &nItems, 300, pMesh, 0x289 - i,
                                    nClip, uFlags, 0, 1.0f,
                                    lbl_801D3CB0.fDefaultObjectMipmapBias[0] * fn_80035560(pMesh), 0.0f);
                    }
                }
                pMesh = fn_800354BC(pMesh);
            }
        }
        fn_80032F88(lbl_801D3CB0.pPanoramaItemsList, nItems, lbl_801D3CB0.eTerrainFilterMin,
                    lbl_801D3CB0.eTerrainFilterMag);
        fn_8003272C(1);
        fn_80012EF8();
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

// The course chunk: clears the pin and tee positions (fn_80034720 and fn_800347B4 fill them), readies
// the collision data, fills light sets 2, 0 and 1 from the hole's lights and makes set 3 current, and
// hands the glows the course's values (p3C, or defaults). The light vector gSession.f5B3C..f5B48 is
// the hole's own (v60) when it has one, else the glows' vector; it is raised to at least its
// distance across the ground and made at least 2000 long.
void fn_800342F0(UStreamObject* pObject) {
    f32 v18[4];
    f32 v8[4];
    int i;
    u32 nGlow;
    CourseGlowBlock* pGlow;
    f32 fLength;

    lbl_801D3CB0.pCourseStreamData = pObject;
    lbl_801D3CB0.pCourse = (CourseInfo*)pObject->pData;
    for (i = 0; i < 4; i++) {
        lbl_801D3CB0.pCourse->tee[i].x = 0.0f;
        lbl_801D3CB0.pCourse->pin[i].x = 0.0f;
        lbl_801D3CB0.pCourse->tee[i].y = 0.0f;
        lbl_801D3CB0.pCourse->pin[i].y = 0.0f;
        lbl_801D3CB0.pCourse->tee[i].z = 0.0f;
        lbl_801D3CB0.pCourse->pin[i].z = 0.0f;
        lbl_801D3CB0.pCourse->tee[i].w = 0.0f;
        lbl_801D3CB0.pCourse->pin[i].w = 0.0f;
    }
    fn_8004B1EC(lbl_801D3CB0.pCourse);
    fn_80035338(2);
    fn_800935CC(&lbl_801D3CB0.pCourse->lights);
    fn_80093900(lbl_801D3CB0.pCourse->p38);
    fn_80035338(0);
    fn_800935CC(&lbl_801D3CB0.pCourse->lights);
    fn_80093900(lbl_801D3CB0.pCourse->p38);
    fn_80035338(1);
    fn_800935CC(&lbl_801D3CB0.pCourse->lights);
    fn_80093900(lbl_801D3CB0.pCourse->p38);
    fn_80035338(3);
    fn_80035370();
    fn_8003534C();
    pGlow = lbl_801D3CB0.pCourse->p3C;
    if (pGlow != NULL) {
        v18[0] = pGlow->v10[0];
        v18[1] = pGlow->v10[1];
        v18[2] = pGlow->v10[2];
        v18[3] = pGlow->v10[3];
        v8[0] = pGlow->v0[0];
        v8[1] = pGlow->v0[1];
        v8[2] = pGlow->v0[2];
        v8[3] = 1.0f;
        if (pGlow->nC <= 3) {
            nGlow = pGlow->nC;
        } else {
            nGlow = 1;
        }
    } else {
        nGlow = 1;
        v18[0] = 0.8f;
        v18[1] = 0.8f;
        v18[2] = 0.4f;
        v18[3] = 1.0f;
        v8[0] = 0.0f;
        v8[1] = 150.0f;
        v8[2] = -400.0f;
        v8[3] = 1.0f;
    }
    fn_800355B8(v18);
    fn_80035590(v8);
    fn_80035584(nGlow);
    if (lbl_801D3CB0.pCourse->v60[0] || lbl_801D3CB0.pCourse->v60[1] || lbl_801D3CB0.pCourse->v60[2]) {
        gSession.f5B3C = lbl_801D3CB0.pCourse->v60[0];
        gSession.f5B40 = lbl_801D3CB0.pCourse->v60[1];
        gSession.f5B44 = lbl_801D3CB0.pCourse->v60[2];
        gSession.f5B48 = 1.0f;
    } else {
        gSession.f5B3C = v8[0];
        gSession.f5B40 = v8[1];
        gSession.f5B44 = v8[2];
        gSession.f5B48 = v8[3];
    }
    fLength = fn_80009680(gSession.f5B3C * gSession.f5B3C + gSession.f5B44 * gSession.f5B44);
    if (gSession.f5B40 < fLength) {
        gSession.f5B40 = fLength;
    }
    // port: f5B3C..f5B48 are read as one vector
    fLength = fn_80009680(fn_80009744(&gSession.f5B3C));
    if (fLength < 2000.0f && fLength > 0.0f) {
        fn_8001EF34(&gSession.f5B3C, 2000.0f / fLength, &gSession.f5B3C);
    }
    i = Game_GetCourse();
    if (i >= 21) {
        i = 0;
    }
    fn_80034648(i);
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

// Draws the grass patches (GoGrass.c calls it): sets the renderer up, takes the camera's position
// and look direction, the flat distance to the nearest ball and the smaller half field of view's
// tangent (view 0's), then builds the grass list (fn_80034F28) and draws it (fn_80034CAC,
// fn_80034DE4).
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
    lbl_801D3CB0.xCameraReferencePos[0] = pLens->m4[3][0];
    lbl_801D3CB0.xCameraReferencePos[1] = pLens->m4[3][1];
    lbl_801D3CB0.xCameraReferencePos[2] = pLens->m4[3][2];
    lbl_801D3CB0.xCameraReferencePos[3] = 1.0f;
    lbl_801D3CB0.xCameraLookVector[0] = pLens->m4[2][0];
    lbl_801D3CB0.xCameraLookVector[1] = pLens->m4[2][1];
    lbl_801D3CB0.xCameraLookVector[2] = pLens->m4[2][2];
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
// n1C), from the end of the list, one clip method at a time.
// Not exact (97.2%): the original tests bit 0 with `and.` against a register holding 1 (one more
// saved register); every spelling tried folds the 1 into `clrlwi.` (a local mask of int, s32, u32 or
// u8, s32/int counters).
void fn_80034CAC(int nRenderPass) {
    u8 bFirst = 1;
    int i;
    s32 nClip;
    Ter_PatchReference* pPatch;

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
            pPatch = &lbl_801D3CB0.xpGrassPatchList[i];
            if (pPatch->eClipMethod == nClip && pPatch->iRenderPass == nRenderPass && (pPatch->n1C & 1)) {
                fn_80032B7C(pPatch->pGround, nClip, 0, pPatch->n1C, pPatch->n18, pPatch->n20, &bFirst, 0, 1,
                            pPatch->fDistance, pPatch->fDistance + 2.0f * pPatch->fBoundingRadius);
            }
        }
    }
    fn_80012F50(1, 6, 1);
    fn_80012EF8();
    fn_8003272C(1);
    fn_80012EF8();
}

// Draws the grass patches in list 0x80 of n1C (pass 3), from the end of the list, one clip method
// at a time.
void fn_80034DE4(void) {
    u8 bFirst = 1;
    int i;
    s32 nClip;
    Ter_PatchReference* pPatch;

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
            pPatch = &lbl_801D3CB0.xpGrassPatchList[i];
            if (pPatch->eClipMethod == nClip && (pPatch->n1C & 0x80)) {
                fn_80032B7C(pPatch->pGround, nClip, 3, pPatch->n1C, pPatch->n18, pPatch->n20, &bFirst, 0,
                            1, pPatch->fDistance, pPatch->fDistance + 2.0f * pPatch->fBoundingRadius);
            }
        }
    }
    fn_80012F50(1, 6, 1);
    fn_80012EF8();
    fn_8003272C(1);
    fn_80012EF8();
}

// Builds the grass list: every patch of this frame in render pass 0's fourth list (bit 0x80 of
// n1C) or whose ground's pC node (fn_8003556C) has flag 8 of byte 3, and whose pC node is not off
// screen, is copied to xpGrassPatchList with that clip method and its distance from the camera
// (less its radius, at least 0).
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
    lbl_801B8980.c30.r = r;
    lbl_801B8980.c30.g = g;
    lbl_801B8980.c30.b = b;
    lbl_801B8980.c30.a = 0x80;
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
void fn_80035398(void);
void fn_8003541C();
void fn_80035440(TerSettings* pSettings);

void fn_80035240(f32 (*pMtx)[4]) {
    fn_80013D9C(*lbl_80280DF0, pMtx);
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

// Takes the current light set's terrain colours as the current settings (fn_80035440).
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

UObjMesh* fn_800354BC(UObjMesh* pNode) {
    return pNode->p14;
}

// The node's bounding sphere: centre (v58), then radius (f64).
f32* fn_800354C4(UObjMesh* pNode) {
    return pNode->pInfo->v58;
}

// A flag byte of the node (a24); this file reads bytes 0-3.
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

// A terrain object's bounds.
f32* fn_80035508(UObjMesh* pMesh) {
    return pMesh->pInfo->a68;
}

// If the object's current entry (n28) is switched on, hands its 0x2C-byte record to LLObj_Gc.c's
// fn_800082CC. Raw offsets until the object's type is described.
void fn_80035514(u8* pObject) {
    s32 n = *(s32*)(pObject + 0x28);

    if (pObject[n + 0x1C] != 0) {
        fn_800082CC((UObjMeshPart*)(*(u8**)(pObject + 0x18) + n * 0x2C));
    }
}

s32 fn_80035554(UObjMesh* pMesh) {
    return pMesh->pInfo->b8B;
}

f32 fn_80035560(UObjMesh* pMesh) {
    return pMesh->pInfo->f54;
}

// The ground's pC node: fn_80034F28 tests its flags and bounds for the grass list.
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
