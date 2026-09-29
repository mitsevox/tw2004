// GoTerrain.c (EA's name, from its asserts; also in EA's 2002 source tree): the terrain renderer
// (Ter_TerrainRendererMgr gTerRenderer, terrain.h): takes in the hole's ground and objects ('ter '),
// the course data ('tgd ', with the tee and pin positions) and the LOD distances ('tLOD'); each
// view, sorts the patches and objects into draw lists by distance and level of detail and draws
// them in passes; animates the objects (trees, the crowd, the flag in the wind); draws the grass
// patches for GoGrass.c. After the renderer come small functions of other systems (TW07 has most
// of them as header inlines): the renderer's state setters (RenderState: fog, blend, clip, constant
// alpha, draw flags), the current render context (RC_, GoRenderCtx.h), the light and fog
// environment and the fog made from its settings (LF_, GoLightFogEnv.h), LLMath_Subtract3 and
// LLMath_Subtract, the terrain model tree's accessors (Ter_GetMesh*), the sun flare's setters (SF_),
// Weather_IsRaining, and last three functions of TW07's CharRend.c (CharacterRender_*), which may be
// a unit of their own before Skin.c.

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

void  Character_SetOrientationVec(Character* pChar, f32* pDir, f32 f);
void  GameModeSkillZoneBase_AddCup(f32 x, f32 y, f32 z);
f32   VM_fGetViewportHeightOverWidth(u8* p);
f32   Math_Tan(f32 x);           // tan
void  Ter_BeginRender(void);
void  Ter_RenderView(void* pHoleData, int nView);
void  Ter_BuildPatchLists(void* pHoleData);
void  Ter_FillPatchReference(UObjMesh* pNode, s32 eClipMethod, s32 iRenderPass, Ter_PatchReference* pPatch,
                  f32 fDistance);
void  Ter_AddPatchObjects(Ter_PatchReference* pPatch, s32 nFirstObject);
void  Ter_SortObjects(void);
void  Ter_UpdateLODPlanes(void);
void  Ter_SelectObjectLODs(void);
u8    Ter_IsLODDataLoaded(void);
void  Ter_BuildObjectDrawLists(void);
u8    Ter_IsBallStoppedInModel(UObjMesh* pModel);
void  Ter_AddObjectDraw(Ter_ObjectDrawData* pDraw, s32* pCount, s32 nUnused, UObjMesh* pModel, f32 fAlpha,
                  f32 fMipmapBias, f32 fDistanceSquared, s32 iObject, s32 eClipMethod, u8 bUseFog,
                  u8 bSetsPrimField);
void  Ter_DrawPatchPass(int nRenderPass);
void  Ter_DrawFarClipPatches(void);
void  Ter_DrawObjects(void);
void  Ter_DrawPanoramaList(void* pHoleData, u32 nList);
void  LLMath_Subtract3(f32* pA, f32* pB, f32* pOut);
f32   Camera_GetLensFovScale(CamLens* pLens);
f32   Ter_GetTimeInCycle(u32 n, f32 fPeriod);
// calls row nRow's function of lbl_80188E88 with pData
void  SD_SetShaderTypeParameters(int nRow, void* pData);
s32   Ter_CompareObjectDistance(const void* pA, const void* pB);
void  Ter_SetZWrite(int n);
void  RenderState_ChangeDrawFlags(u32 uClear, u32 uSet);
void  RC_vUpdateCurrentRenderCtxTransformationMatrices(void);
void  RC_UpdateCurrentScreenMatrices(void);
void  Camera_SetLensFarClip(u8* p, f32 v); // sets the lens's far clip distance, fAC (CA_fGetCameraFarZ reads it)
f32   CA_fGetCameraFarZ(u8* p);
void  Ter_SetLODPlanes(Ter_LODPlane* pPlanes, f32 fStep, s32 a, s32 b, s32 c, s32 d);
void  Ter_DrawPatchGround(void* pGround, s32 eClipMethod, s32 nPass, s32 n1C, s32 n18, s32 n20, u8* pbFirst,
                          u8 b1,
                  u8 b2, f32 fNear, f32 fFar);
void  Ter_DrawObjectList(Ter_ObjectDrawData* pList, s32 nCount, s32 eFilterMin, s32 eFilterMag);
void  Ter_LODLoadCallback(UStreamObject* pObject);
void  Ter_HoleDataLoadCallback(UStreamObject* pObject);
void  Ter_CourseLoadCallback(UStreamObject* pObject);
void  SF_vSetFlareType(s32 v);
void  SF_vSetSunPosition(f32* p0);
void  SF_vSetSunColor(f32* p0);
void  Ter_SetCourseMipmapBias(int n);
void  Ter_DrawMeshCurrentPart(u8* pObject);
void  Ter_ResetObjectRenderState(void);
u8    Ter_SetManageZUpdate(u8 b);
f32*  Ter_GetMeshBounds(UObjMesh* pMesh);
u8    Ter_SetObjectRenderState(Ter_ObjectDrawData* pDraw, u8 bForce);
void  LLMath_IdentifyMat(f32 (*pMtx)[4]);  // identity matrix
void  LF_SetCurrentDefaultLights(void);
void  Ter_DrawGrassPatches(int nRenderPass);
void  Ter_DrawGrassPatchesPass3(void);
void  Ter_BuildGrassPatchList(void* pUnused);
void  LLMath_Subtract(f32* pA, f32* pB, f32* pOut);

UObjMesh* Ter_GetMeshNext(UObjMesh* pNode);
f32       Ter_GetMeshMipmapBiasScale(UObjMesh* pMesh);
f32*      Ter_GetMeshBoundingSphere(UObjMesh* pNode);
s32       Ter_GetMeshFlags(UObjMesh* pNode, s32 n);
UObjMesh* Ter_GetMeshChild(UObjMesh* pNode, s32 n);
s32       Ter_GetMeshChildCount(UObjMesh* pNode);
UObjMesh* Ter_GetHoleModelRoot(u8* pHoleData);
UObjMesh* Ter_GetGroundDrawMesh(UObjMesh* pGround);
s32       Ter_GetMeshDrawFlags(UObjMesh* pMesh);

// .bss and .sbss in reverse address order
Ter_TerrainRendererMgr gTerRenderer;   // the terrain renderer's state
s32 gTerLowBitCounts[5][32];        // [n][k]: how many of k's lowest n bits are set
s32 gCharRendCurrentBuffer;         // written by CharacterRender_SetCurrentBuffer, read nowhere
s32 gTerPanoramaList2HiddenCount;   // Ter_DrawPanoramaList leaves out list 2's last this-many
f32 gTerTreePeriodRandom;           // Ter_vInitModule's pseudo-random number, 0..1

f32 gTerLastObjectAlpha = -1.0f;    // the last object draw's alpha and z write
s8  gTerLastZWrite = -1;            // (Ter_SetObjectRenderState); -1: none yet
// Ter_SetLODPlanes's arguments: LOD 0's and LOD 1's lengths in steps (26 and 16 until a 'tLOD'
// chunk sets them through Ter_LODStepsFromDistances), a step's length, the overlaps in steps
s32 gTerLOD0Steps = 26;
s32 gTerLOD1Steps = 16;
f32 gTerLODStepSize = 5.0f;
s32 gTerLOD1OverlapSteps = 4;
s32 gTerLOD2OverlapSteps = 4;
s32 gTerLODDataNear = -1;           // the 'tLOD' chunk's two distances (Ter_LODLoadCallback);
s32 gTerLODDataFar = -1;            // -1: none loaded (Ter_IsLODDataLoaded)
u8  gTerDrawPanoramaList0 = 1;      // 1: Ter_DrawPanoramaList draws object list 0
u8  gTerDrawPanoramaList2 = 1;      // 1: Ter_DrawPanoramaList draws object list 2

// Per course (Game_GetCourse), the three levels of detail's object mipmap biases
// (Ter_SetCourseMipmapBias)
f32 gTerCourseMipmapBias[21][3] = {
    { -5.0f, -8.0f, -8.0f },
    { -5.0f, -8.0f, -8.0f },
    { -5.0f, -8.0f, -8.0f },
    { -5.0f, -8.0f, -8.0f },
    { -5.0f, -8.0f, -8.0f },
    { -5.0f, -5.0f, -5.0f },
    { -5.0f, -8.0f, -8.0f },
    { -5.0f, -8.0f, -8.0f },
    { -5.0f, -5.0f, -5.0f },
    { -5.0f, -8.0f, -8.0f },
    { -5.0f, -8.0f, -8.0f },
    { -5.0f, -8.0f, -8.0f },
    { -5.0f, -8.0f, -8.0f },
    { -5.0f, -8.0f, -8.0f },
    { -5.0f, -8.0f, -8.0f },
    { -5.0f, -8.0f, -8.0f },
    { -5.0f, -5.0f, -5.0f },
    { -5.0f, -8.0f, -8.0f },
    { -5.0f, -5.0f, -5.0f },
    { -5.0f, -5.0f, -5.0f },
    { -5.0f, -5.0f, -5.0f },
};

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0 before Ter_vInitModule's 3.7 and 1.6; its body is unknown, this one only reproduces the order.
static f32 GoTerrain_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Sets up the terrain renderer: allocates its lists, gives every object state its starting values
// (each tree its own period from a stepped random number), sets the renderer's defaults and fills
// the bit-count table gTerLowBitCounts.
void Ter_vInitModule(void) {
    s32 i;
    s32 n;
    s32 k;
    s32 j;
    s32 nBits;

    gTerRenderer.pPatchList = StaticMem_Alloc(1024 * sizeof(Ter_PatchReference), 2, 16, "GoTerrain.c", 567);
    gTerRenderer.pPostDrawTerrainList =
        StaticMem_Alloc(20 * sizeof(Ter_PatchReference), 2, 16, "GoTerrain.c", 572);
    gTerRenderer.pObjectSortList = StaticMem_Alloc(650 * sizeof(Ter_ObjectReference), 2, 16, "GoTerrain.c",
                                                   577);
    gTerRenderer.pOpaqueObjectList = StaticMem_Alloc(650 * sizeof(Ter_ObjectDrawData), 2, 16, "GoTerrain.c",
                                                     582);
    gTerRenderer.pTranslucentObjectList =
        StaticMem_Alloc(200 * sizeof(Ter_ObjectDrawData), 2, 16, "GoTerrain.c", 587);
    gTerRenderer.pNearbyObjectList = StaticMem_Alloc(70 * sizeof(Ter_ObjectDrawData), 2, 16, "GoTerrain.c",
                                                     592);
    gTerRenderer.pDeferredItemsList = StaticMem_Alloc(50 * sizeof(Ter_ObjectDrawData), 2, 16, "GoTerrain.c",
                                                      597);
    gTerRenderer.pPanoramaItemsList =
        StaticMem_Alloc(300 * sizeof(Ter_ObjectDrawData), 2, 16, "GoTerrain.c", 602);
    gTerRenderer.pPostDrawItemsList =
        StaticMem_Alloc(400 * sizeof(Ter_ObjectDrawData), 2, 16, "GoTerrain.c", 607);
    gTerRenderer.pObjectStateList =
        StaticMem_Alloc(TER_NUM_OBJECTS * sizeof(Ter_ObjectState), 2, 16, "GoTerrain.c", 612);
    gTerRenderer.xpGrassPatchList = StaticMem_Alloc(128 * sizeof(Ter_PatchReference), 2, 16, "GoTerrain.c",
                                                    633);
    gTerRenderer.fTreeMinPeriod = 3.7f;
    gTerRenderer.fTreeDiffPeriod = 1.6f;
    gTerRenderer.fTreeOverdrive = 1.0f;
    gTerRenderer.fTreeNoisePeriodScale = 1.0f;
    gTerRenderer.fTreeNoiseAmplitudeScale = 0.0f;
    for (i = 0; i < TER_NUM_OBJECTS; i++) {
        gTerRenderer.pObjectStateList[i].a20[0] = 0;
        gTerRenderer.pObjectStateList[i].a20[1] = 0;
        gTerRenderer.pObjectStateList[i].a20[2] = 0;
        gTerRenderer.pObjectStateList[i].a20[3] = 0;
        gTerRenderer.pObjectStateList[i].f0 =
            gTerTreePeriodRandom * gTerRenderer.fTreeDiffPeriod + gTerRenderer.fTreeMinPeriod;
        gTerTreePeriodRandom *= 131.2934f;
        gTerTreePeriodRandom += 82.459f;
        gTerTreePeriodRandom -= Math_Floor(gTerTreePeriodRandom);
        gTerRenderer.pObjectStateList[i].f4 = gTerRenderer.fTreeOverdrive;
        gTerRenderer.pObjectStateList[i].f8 = 0.0f;
        gTerRenderer.pObjectStateList[i].nC = 0;
        gTerRenderer.pObjectStateList[i].n18 = 0;
        gTerRenderer.pObjectStateList[i].n1C = 0;
        gTerRenderer.pObjectStateList[i].f10 = gTerTreePeriodRandom;
        gTerRenderer.pObjectStateList[i].aView[0].n4 = 3;
        gTerRenderer.pObjectStateList[i].aView[0].f0 = 1.0f;
        gTerRenderer.pObjectStateList[i].aView[1].n4 = 3;
        gTerRenderer.pObjectStateList[i].aView[1].f0 = 1.0f;
    }
    gTerRenderer.pCurrentHoleData = NULL;
    gTerRenderer.pCourse = NULL;
    gTerRenderer.fDetailMipmapBias = 0.0f;
    gTerRenderer.fLakeSurfaceMipmapBias = 0.0f;
    gTerRenderer.iLowLODListOffset = -1;
    gTerRenderer.fCrowdAnimationDelayedStartTimer = -1.0f;
    gTerRenderer.fCrowdAnimationDelayedStartPercentage = 0.0f;
    gTerRenderer.fCrowdAnimationDelayedStartDuration = 0.0f;
    gTerRenderer.fCrowdAnimationCountdown = -1.0f;
    gTerRenderer.fCrowdFadeDistanceMin = 4.0f;
    gTerRenderer.fCrowdFadeDistanceMax = 5.0f;
    gTerRenderer.iCrowdPose = 0;
    gTerRenderer.fCrowdInterpValue = 0.5f;
    gTerRenderer.fTreeDampingDistance = 5625.0f;
    gTerRenderer.fTreeDampingMaxForce = 0.5f;
    gTerRenderer.fDistanceCullYardsBase = 150.0f;
    gTerRenderer.eObjectFilterMin = 4;
    gTerRenderer.eObjectFilterMag = 1;
    gTerRenderer.eTerrainFilterMin = 5;
    gTerRenderer.eTerrainFilterMag = 1;
    gTerRenderer.fCrowdFullMaxDistanceFromGolfer = 60.0f;
    gTerRenderer.fCrowdHalfMaxDistanceFromGolfer = 350.0f;
    Ter_SetManageZUpdate(1);
    for (n = 0; n < 5; n++) {
        for (k = 0; k < 32; k++) {
            nBits = 0;
            for (j = 0; j < n; j++) {
                if (k & (1 << j)) {
                    nBits++;
                }
            }
            gTerLowBitCounts[n][k] = nBits;
        }
    }
}

// Shuts the terrain renderer down (gomainloop): frees the lists Ter_vInitModule allocated (not
// xpGrassPatchList), the hole's data (fn_800075CC) and the course.
void Ter_vCloseModule(void) {
    StaticMem_Free(gTerRenderer.pPatchList);
    StaticMem_Free(gTerRenderer.pPostDrawTerrainList);
    StaticMem_Free(gTerRenderer.pObjectSortList);
    StaticMem_Free(gTerRenderer.pOpaqueObjectList);
    StaticMem_Free(gTerRenderer.pTranslucentObjectList);
    StaticMem_Free(gTerRenderer.pNearbyObjectList);
    StaticMem_Free(gTerRenderer.pDeferredItemsList);
    StaticMem_Free(gTerRenderer.pPanoramaItemsList);
    StaticMem_Free(gTerRenderer.pPostDrawItemsList);
    StaticMem_Free(gTerRenderer.pObjectStateList);
    if (gTerRenderer.pCurrentHoleData != NULL) {
        fn_800075CC(gTerRenderer.pCurrentHoleData);
        gTerRenderer.pCurrentHoleData = NULL;
    }
    if (gTerRenderer.pCourse != NULL) {
        // EA bug: pCourse is the 'tgd ' chunk's pData, which points past the stream object's
        // header (UStream.c), not at the block StaticMem allocated; StaticMem_Free reads the two
        // words before it as a block's start and size. Ter_UnloadHole frees pCourseStreamData
        // instead; this is harmless only when Ter_UnloadHole has already run and cleared pCourse.
        // port: free pCourseStreamData here, as Ter_UnloadHole does.
        StaticMem_Free(gTerRenderer.pCourse);
        gTerRenderer.pCourse = NULL;
    }
}

// Registers the terrain's chunk loaders (streammanagerhole.c): 'ter ' the hole's ground and objects
// (Ter_HoleDataLoadCallback), 'tgd ' the course data (Ter_CourseLoadCallback), 'tLOD' the LOD
// distances (Ter_LODLoadCallback).
void Ter_RegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback('ter ', Ter_HoleDataLoadCallback);
    Stream_RegisterLoadChunkCallback('tgd ', Ter_CourseLoadCallback);
    Stream_RegisterLoadChunkCallback('tLOD', Ter_LODLoadCallback);
}

// Unregisters the 'ter ' and 'tgd ' chunk loaders; 'tLOD' stays registered.
void Ter_UnRegisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('ter ');
    Stream_UnregisterLoadChunkCallback('tgd ');
}

// Puts the render state back after drawing terrain: constant alpha off, alpha test on (mode 6,
// reference 128), z buffer mode 3 with writes on.
void Ter_EndRender(void) {
    RenderState_SetConstantAlphaOn(0);
    DS_vSetAlphaTestMode(1, 6, 128);
    DS_vSetZBufferMode(3);
    DS_vEnableZBufferUpdate(1);
    RenderState_Flush();
}

// Sets the renderer up for the terrain, then hands rows 4 and 5 of lbl_80188E88 the frame count,
// row 4 with four waves between 0 and 1 whose cycles are 1591.2 x (5.5 + i) / 1000 seconds.
void Ter_BeginRender(void) {
    TerWaveData wave;
    u32 nFrame;
    int i;

    RC_vSetCurrentRenderCtxTransformationMatrix(NULL);
    RenderState_SetCameraMatrices();
    RenderState_SetCameraMatrices();
    RenderState_SetCameraMatrices();
    RenderState_SetBlendFactors(4, 5);
    DS_vSetAlphaTestMode(1, 6, 1);
    DS_vSetZBufferMode(3);
    LF_vSetCurrentLightFogEnvironment(2);
    LF_UseCurrentFogSettings();
    LF_UpdateFog();
    RenderState_SetDrawFlags(0x70);
    RenderState_SetViewport(RC_spGetCurrentRenderCtx());
    RenderState_Flush();
    for (i = 0; i < 4; i++) {
        // fake match: the original's 1591.2 is one bit above the literal 1591.2f, as a folded float
        // product gives it; 26.52 x 60 is one such product (3 x 530.4 and 12 x 132.6 are others)
        f32 fPeriod = 26.52f * 60.0f * (5.5f + (f32)i) / 1000.0f;

        wave.aWave[i] = 0.5f
                * Math_Sin(6.2831855f * Ter_GetTimeInCycle(gSession.nFrameCount, fPeriod) / fPeriod)
                        + 0.5f;
    }
    wave.nFrame = gSession.nFrameCount;
    SD_SetShaderTypeParameters(4, &wave);
    nFrame = gSession.nFrameCount;
    SD_SetShaderTypeParameters(5, &nFrame);
}

// fake match: these two stand in for code the original linker stripped. The file's pool has
// Ter_GetTimeInCycle's constants (1/59.94, 59.94, 0.5/59.94, the u32 conversion's) and then 0.375, 4.15
// and 10 (as LF_ApplyFogToRenderState uses them) right after Ter_BeginRender's; their bodies are
// unknown, these only reproduce the order.
static f32 GoTerrain_StrippedFn2(u32 n, f32 x) {
    return FRAME_TIME * (f32)(n % (u32)(FRAME_RATE * (0.5f / FRAME_RATE + x)));
}

static f32 GoTerrain_StrippedFn3(f32 x) {
    return 0.375f * x + 4.15f * (10.0f + x);
}

// Draws the terrain in view nView: takes the camera's position and look direction, the flat
// distance to the nearest ball, the followed player's distance to the pin and the smaller half
// field of view's tangent, then builds the lists and draws them pass by pass.
void Ter_RenderView(void* pHoleData, int nView) {
    int i;
    CamLens* pLens;
    f32 vToPin[4];
    f32 vDiff[4];
    f32 fDist;
    f32 fTan;
    f32 fWideTan;

    gTerRenderer.iCurrentViewContext = nView;
    pLens = Camera_GetCurrentLens();
    gTerRenderer.fFOVScale = 1.0f / Camera_GetLensFovScale(pLens);
    gTerRenderer.xCameraReferencePos[0] = pLens->m4[3][0];
    gTerRenderer.xCameraReferencePos[1] = pLens->m4[3][1];
    gTerRenderer.xCameraReferencePos[2] = pLens->m4[3][2];
    gTerRenderer.xCameraReferencePos[3] = 1.0f;
    gTerRenderer.xCameraLookVector[0] = pLens->m4[2][0];
    gTerRenderer.xCameraLookVector[1] = pLens->m4[2][1];
    gTerRenderer.xCameraLookVector[2] = pLens->m4[2][2];
    gTerRenderer.xCameraLookVector[3] = 1.0f;
    gTerRenderer.fXZDistanceToClosestBallSquared = 1000000.0f;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        LLMath_Subtract(gPlayers[i].ball.vPos, gTerRenderer.xCameraReferencePos, vDiff);
        vDiff[1] = 0.0f;
        fDist = Vec3_LengthSqClamped(vDiff);
        if (fDist < gTerRenderer.fXZDistanceToClosestBallSquared) {
            gTerRenderer.fXZDistanceToClosestBallSquared = fDist;
        }
    }
    LLMath_Subtract3(gPlayers[ViewController_GetActivePlayerNumber(gTerRenderer.iCurrentViewContext)].vBall,
                &Ter_GetTGD()->pin[Game_CurrentPinSet()].x, vToPin);
    vToPin[1] = 0.0f;
    gTerRenderer.fGolferDistanceToCup = Math_Sqrt(Vec3_LengthSqClamped(vToPin));
    fTan = Math_Tan(0.5f * pLens->fFov);
    fWideTan = Math_Tan(0.5f
                        * (0.75f * pLens->fFov
                           * VM_fGetViewportHeightOverWidth((u8*)RC_spGetCurrentRenderCtxViewport())));
    gTerRenderer.fCameraMinHalfFieldOfViewTan =
        (fTan <= fWideTan / ViewController_GetCameraControl(nView)->f54) ? fTan : fWideTan
                / ViewController_GetCameraControl(nView)->f54;
    RenderState_Flush();
    if (!gTerRenderer.bObjectTestMode) {
        Ter_DrawPanoramaList(pHoleData, 0);
    }
    Ter_BuildPatchLists(pHoleData);
    Ter_SortObjects();
    Ter_UpdateLODPlanes();
    Ter_SelectObjectLODs();
    Ter_BuildObjectDrawLists();
    Ter_DrawPatchPass(0);
    Ter_DrawFarClipPatches();
    if (!gTerRenderer.bObjectTestMode) {
        Ter_DrawPanoramaList(pHoleData, 2);
    }
    Ter_DrawPatchPass(1);
    Ter_DrawPatchPass(2);
    Ter_DrawObjects();
    RenderState_SetBlendFactors(4, 5);
    DS_vSetAlphaTestMode(1, 6, 0x80);
    RenderState_SetDrawFlags(0x70);
    RenderState_Flush();
}

// Builds the patch lists: clears the counts and pSortedPatchList, then takes each patch of the hole
// data that is used with the current pin position (or with any) and is not off screen, fills its
// Ter_PatchReference (Ter_FillPatchReference) and objects (Ter_AddPatchObjects) and chains it into
// the lists its n1C bits ask for. In object test mode the whole object tree goes in as one patch.
void Ter_BuildPatchLists(void* pHoleData) {
    Ter_PatchReference* pPatch;
    s32 nCount;
    s32 iRenderPass;
    UObjMesh* pMesh;
    UObjMesh* pRoot;
    UObjMesh* pList;
    s32 eClipMethod;
    s32 nFirstObject = 0;
    u32 uPinBit = 1 << Game_CurrentPinSet();
    void* pCamera = RC_spGetCurrentRenderCtx();
    s32 uFlags;
    f32 fRadius;
    f32 fDist;
    int i;
    int j;
    int k;

    gTerRenderer.iTotalPatches = 0;
    gTerRenderer.iTotalPostDrawTerrainPatches = 0;
    gTerRenderer.iTotalSortObjects = 0;
    gTerRenderer.iOpaqueObjects = 0;
    gTerRenderer.iTranslucentObjects = 0;
    gTerRenderer.iNearbyObjects = 0;
    gTerRenderer.iDeferredItems = 0;
    gTerRenderer.iPostDrawItems = 0;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 4; j++) {
            for (k = 0; k < 3; k++) {
                gTerRenderer.pSortedPatchList[i][j][k] = NULL;
            }
        }
    }
    pRoot = Ter_GetHoleModelRoot(pHoleData);
    if (gTerRenderer.bObjectTestMode) {
        pPatch = &gTerRenderer.pPatchList[gTerRenderer.iTotalPatches++];
        pPatch->fDistance = 0.0f;
        pPatch->fBoundingRadius = 0.0f;
        pPatch->eClipMethod = 1;
        pPatch->pGround = NULL;
        pPatch->pObjects = pRoot;
        Ter_AddPatchObjects(pPatch, 0);
        return;
    }
    if (Ter_GetMeshChildCount(pRoot) >= 1) {
        pList = Ter_GetMeshChild(pRoot, 1);
        nCount = Ter_GetMeshChildCount(pList);
        pMesh = Ter_GetMeshChild(pList, 0);
        for (i = nCount; i > 0; i--) {
            gTerRenderer.iPatchFirstObjectInstanceIndex[nCount - i] = nFirstObject;
            uFlags = Ter_GetMeshFlags(pMesh, 1);
            if ((uFlags & uPinBit) || !(uFlags & 0xF)) {
                if (uFlags & 0x40) {
                    iRenderPass = 1;
                } else if (uFlags & 0x80) {
                    iRenderPass = 2;
                } else {
                    iRenderPass = 0;
                }
                fRadius = Ter_GetMeshBoundingSphere(pMesh)[3];
                fDist = LLMath_DistanceBetween3(gTerRenderer.xCameraReferencePos,
                                                Ter_GetMeshBoundingSphere(pMesh))
                        - fRadius;
                if (fDist < 0.0f) {
                    fDist = 0.0f;
                }
                eClipMethod = fn_80007B2C(pMesh, pCamera, fDist, gTerRenderer.fCameraMinHalfFieldOfViewTan,
                                          ViewController_GetCameraControl(
                                                  gTerRenderer.iCurrentViewContext)->f54);
                if (eClipMethod != 3) {
                    pPatch = &gTerRenderer.pPatchList[gTerRenderer.iTotalPatches++];
                    Ter_FillPatchReference(pMesh, eClipMethod, iRenderPass, pPatch, fDist);
                    if (pPatch->pObjects != NULL) {
                        Ter_AddPatchObjects(pPatch, nFirstObject);
                    }
                    if (pPatch->n1C & 1) {
                        pPatch->pNext[0] = gTerRenderer.pSortedPatchList[iRenderPass][0][eClipMethod];
                        gTerRenderer.pSortedPatchList[iRenderPass][0][eClipMethod] = pPatch;
                    }
                    if (pPatch->n1C & 2) {
                        pPatch->pNext[1] = gTerRenderer.pSortedPatchList[iRenderPass][1][eClipMethod];
                        gTerRenderer.pSortedPatchList[iRenderPass][1][eClipMethod] = pPatch;
                    }
                    if (pPatch->n1C & 4) {
                        pPatch->pNext[2] = gTerRenderer.pSortedPatchList[iRenderPass][2][eClipMethod];
                        gTerRenderer.pSortedPatchList[iRenderPass][2][eClipMethod] = pPatch;
                    }
                    if (pPatch->n1C & 0x80) {
                        pPatch->pNext[3] = gTerRenderer.pSortedPatchList[0][3][eClipMethod];
                        gTerRenderer.pSortedPatchList[0][3][eClipMethod] = pPatch;
                    }
                }
            }
            if (Ter_GetMeshChildCount(pMesh) >= 2) {
                nFirstObject += Ter_GetMeshChildCount(Ter_GetMeshChild(Ter_GetMeshChild(pMesh, 1), 0));
            }
            pMesh = Ter_GetMeshNext(pMesh);
        }
    }
}

// Fills pPatch from a patch's node: its ground is node 0, its objects come with a second node.
void Ter_FillPatchReference(UObjMesh* pNode, s32 eClipMethod, s32 iRenderPass, Ter_PatchReference* pPatch,
                 f32 fDistance) {
    s32 nNodes;

    pPatch->fDistance = fDistance;
    pPatch->fBoundingRadius = Ter_GetMeshBoundingSphere(pNode)[3];
    pPatch->eClipMethod = eClipMethod;
    pPatch->iRenderPass = iRenderPass;
    nNodes = Ter_GetMeshChildCount(pNode);
    pPatch->pGround = Ter_GetMeshChild(pNode, 0);
    pPatch->n18 = Ter_GetMeshFlags(pPatch->pGround, 3);
    pPatch->n1C = Ter_GetMeshFlags(pPatch->pGround, 2);
    pPatch->n20 = Ter_GetMeshFlags(pNode, 1);
    if (nNodes >= 2) {
        pPatch->pObjects = Ter_GetMeshNext(pPatch->pGround);
        return;
    }
    pPatch->pObjects = NULL;
}

// Adds a patch's objects to pObjectSortList, numbering them from nFirstObject: each gets its three
// levels of detail (in object test mode the first list's; else lists 0, and 1 and 2 past
// iLowLODListOffset), its distance from the camera and a clip method, or is left out (clip method 3).
// Split screen leaves out objects with bit 0x8 of word 2. Hidden are: far objects with bit 0x80;
// crowd objects (word 3 bits 0x4, 0x10, 0x20) when CameraController_IsFlybyDone is 0 for the view, beyond
// fCrowdHalfMaxDistanceFromGolfer from the ball, or beyond fCrowdFullMaxDistanceFromGolfer and
// farther from the pin than the ball is (every other one when nearer); and of those, tee markers
// (word 1 bits 0x1, 0x2, 0x4: tee sets 0-2) not of the player's tee set, and all of them once the
// ball is off the tee (Ball.nLie).
void Ter_AddPatchObjects(Ter_PatchReference* pPatch, s32 nFirstObject) {
    f32 v48[4];
    s32 iObject;
    UObjMesh* pLOD1;
    s32 uFlags2;
    f32 fHeight;
    f32 v38[4];
    f32 v28[4];
    f32 fDistanceSquared;
    f32 v18[4];
    f32 fBallToObject;
    int nLast;
    f32* pBall;
    f32 fPinToBall;
    View* pView;
    f32 v8[4];
    f32 fXZ;
    UObjMesh* pLOD2;
    f32* pBounds;
    s32 eClipMethod;
    UObjMesh* pLOD0;
    f32 fObjectToPin;
    f32* pPin;
    Ter_ObjectReference* pRef;
    s32 i;
    s32 nLists;
    u8 bHide;
    s32 nObjects;

    nObjects = Ter_GetMeshChildCount(Ter_GetMeshChild(pPatch->pObjects, 0));
    if (nObjects == 0) {
        return;
    }
    if (gTerRenderer.bObjectTestMode) {
        nObjects = 1;
        nLists = Ter_GetMeshChildCount(Ter_GetMeshChild(pPatch->pObjects, 0));
        pLOD0 = Ter_GetMeshChild(Ter_GetMeshChild(pPatch->pObjects, 0), 0);
        pLOD1 = nLists > 1 ? Ter_GetMeshChild(Ter_GetMeshChild(pPatch->pObjects, 0), 1) : pLOD0;
        if (nLists > 2) {
            pLOD2 = Ter_GetMeshChild(Ter_GetMeshChild(pPatch->pObjects, 0), 2);
        } else {
            pLOD2 = pLOD1;
        }
    } else {
        if (gTerRenderer.iLowLODListOffset == -1) {
            if (Ter_GetMeshChildCount(pPatch->pObjects) == 5) {
                gTerRenderer.iLowLODListOffset = 2;
            } else {
                gTerRenderer.iLowLODListOffset = 0;
            }
        }
        pLOD0 = Ter_GetMeshChild(Ter_GetMeshChild(pPatch->pObjects, 0), 0);
        pLOD1 = Ter_GetMeshChild(Ter_GetMeshChild(pPatch->pObjects, gTerRenderer.iLowLODListOffset + 1), 0);
        pLOD2 = Ter_GetMeshChild(Ter_GetMeshChild(pPatch->pObjects, gTerRenderer.iLowLODListOffset + 2), 0);
    }
    nLast = nObjects - 1 + nFirstObject;
    for (i = nObjects - 1; i >= 0; i--) {
        uFlags2 = Ter_GetMeshFlags(pLOD0, 2);
        if (gSession.nSplitScreen == 0 || !(uFlags2 & 8)) {
            pBounds = Ter_GetMeshBounds(pLOD0);
            fHeight = fabsf(gTerRenderer.xCameraReferencePos[1] - pBounds[1]) - pBounds[7];
            if (fHeight < 0.0f) {
                fHeight = 0.0f;
            }
            LLMath_Subtract3(pBounds, gTerRenderer.xCameraReferencePos, v48);
            v48[1] = 0.0f;
            fXZ = (f32)Math_Sqrt(Vec3_LengthSqClamped(v48)) - pBounds[3];
            if (fXZ < 0.0f) {
                fXZ = 0.0f;
            }
            fDistanceSquared = fHeight * fHeight + fXZ * fXZ;
            if (fDistanceSquared <= 0.0f) {
                CameraController_CameraCollision(gTerRenderer.iCurrentViewContext, pBounds);
            }
            bHide = 0;
            if ((uFlags2 & 0x80) && fDistanceSquared > gTerRenderer.fDistanceCullFrameYardsSquared) {
                bHide = 1;
            } else if ((Ter_GetMeshFlags(pLOD0, 3) & 4) || (Ter_GetMeshFlags(pLOD0, 3) & 0x10)
                       || (Ter_GetMeshFlags(pLOD0, 3) & 0x20)) {
                pBall = gPlayers[ViewController_GetActivePlayerNumber(
                        gTerRenderer.iCurrentViewContext)].vBall;
                pPin = &Ter_GetTGD()->pin[Game_CurrentPinSet()].x;
                LLMath_Subtract3(pBounds, pBall, v28);
                v28[1] = 0.0f;
                fBallToObject = (f32)Math_Sqrt(Vec3_LengthSqClamped(v28)) - pBounds[3];
                if (fBallToObject < 0.0f) {
                    fBallToObject = 0.0f;
                }
                LLMath_Subtract3(pPin, pBall, v18);
                v18[1] = 0.0f;
                fPinToBall = Math_Sqrt(Vec3_LengthSqClamped(v18));
                if (fPinToBall < 0.0f) {
                    fPinToBall = 0.0f;
                }
                LLMath_Subtract3(pBounds, pPin, v8);
                v8[1] = 0.0f;
                fObjectToPin = (f32)Math_Sqrt(Vec3_LengthSqClamped(v8)) - pBounds[3];
                if (fObjectToPin < 0.0f) {
                    fObjectToPin = 0.0f;
                }
                if (!CameraController_IsFlybyDone(
                        ViewController_GetCameraControl(gTerRenderer.iCurrentViewContext))) {
                    bHide = 1;
                } else if (fBallToObject > gTerRenderer.fCrowdHalfMaxDistanceFromGolfer) {
                    bHide = 1;
                } else if (fBallToObject > gTerRenderer.fCrowdFullMaxDistanceFromGolfer
                           && fObjectToPin > fPinToBall) {
                    bHide = 1;
                } else if (fBallToObject > gTerRenderer.fCrowdFullMaxDistanceFromGolfer
                           && fObjectToPin <= fPinToBall && (nLast - i) % 2 != 0) {
                    bHide = 1;
                } else if (((Ter_GetMeshFlags(pLOD0, 1) & 1)
                            && gSession.nTeeSet[ViewController_GetActivePlayerNumber(
                                    gTerRenderer.iCurrentViewContext)]
                                    != 0)
                           || ((Ter_GetMeshFlags(pLOD0, 1) & 2)
                               && gSession.nTeeSet[ViewController_GetActivePlayerNumber(
                                       gTerRenderer.iCurrentViewContext)] != 1)
                           || ((Ter_GetMeshFlags(pLOD0, 1) & 4)
                               && gSession.nTeeSet[ViewController_GetActivePlayerNumber(
                                       gTerRenderer.iCurrentViewContext)] != 2)) {
                    bHide = 1;
                } else if (((Ter_GetMeshFlags(pLOD0, 1) & 1) || (Ter_GetMeshFlags(pLOD0, 1) & 2)
                            || (Ter_GetMeshFlags(pLOD0, 1) & 4))
                           && gPlayers[ViewController_GetActivePlayerNumber(
                                   gTerRenderer.iCurrentViewContext)].ball.nLie
                                   != 0) {
                    bHide = 1;
                }
            }
            if (bHide) {
                eClipMethod = 3;
            } else if (pPatch->eClipMethod == 2) {
                eClipMethod = 2;
            } else {
                pView = ViewController_GetCameraControl(gTerRenderer.iCurrentViewContext);
                eClipMethod = fn_80007B2C(pLOD0, RC_spGetCurrentRenderCtx(), Math_Sqrt(fDistanceSquared),
                                          gTerRenderer.fCameraMinHalfFieldOfViewTan, pView->f54);
            }
            if (eClipMethod != 3) {
                pRef = &gTerRenderer.pObjectSortList[gTerRenderer.iTotalSortObjects];
                pRef->fDistanceSquared = fDistanceSquared;
                pRef->f14 = Vec3_LengthSqClamped(v48);
                LLMath_Subtract3(pBounds, gTerRenderer.xCameraReferencePos, v38);
                pRef->f18 = Vec3_Dot(gTerRenderer.xCameraLookVector, v38);
                iObject = nLast - i;
                pRef->eClipMethod = eClipMethod;
                pRef->pContainerPatch = pPatch;
                pRef->iGlobalObjectIndex = iObject;
                gTerRenderer.pObjectStateList[iObject].a20[0] = Ter_GetMeshFlags(pLOD0, 0);
                gTerRenderer.pObjectStateList[iObject].a20[1] = Ter_GetMeshFlags(pLOD0, 1);
                gTerRenderer.pObjectStateList[iObject].a20[2] = Ter_GetMeshFlags(pLOD0, 2);
                gTerRenderer.pObjectStateList[iObject].a20[3] = Ter_GetMeshFlags(pLOD0, 3);
                if ((uFlags2 & 0x40) || (Ter_GetMeshFlags(pLOD0, 3) & 0x10)
                    || (Ter_GetMeshFlags(pLOD0, 3) & 0x20)) {
                    pRef->nLODs = 1;
                    pRef->apObject[0] = pLOD0;
                } else {
                    pRef->nLODs = 3;
                    pRef->apObject[0] = pLOD0;
                    pRef->apObject[1] = pLOD1;
                    pRef->apObject[2] = pLOD2;
                }
                gTerRenderer.iTotalSortObjects++;
            }
        }
        pLOD0 = Ter_GetMeshNext(pLOD0);
        pLOD1 = Ter_GetMeshNext(pLOD1);
        pLOD2 = Ter_GetMeshNext(pLOD2);
    }
}

// Sorts pObjectSortList by distance, except when gSession.b11 is set.
void Ter_SortObjects(void) {
    if (gSession.b11 == 0) {
        qsort(gTerRenderer.pObjectSortList, gTerRenderer.iTotalSortObjects, sizeof(Ter_ObjectReference),
              Ter_CompareObjectDistance);
    }
}

// Ter_SortObjects's comparison: nearest first.
s32 Ter_CompareObjectDistance(const void* pA, const void* pB) {
    f32 fA = ((const Ter_ObjectReference*)pA)->fDistanceSquared;
    f32 fB = ((const Ter_ObjectReference*)pB)->fDistanceSquared;

    if (fA > fB) return 1;
    if (fA < fB) return -1;
    return 0;
}

// Sets this frame's LOD planes (Ter_SetLODPlanes with the LOD steps gTerLOD0Steps ..
// gTerLOD2OverlapSteps) and the object cull distance, (fFOVScale x fDistanceCullYardsBase) squared.
void Ter_UpdateLODPlanes(void) {
    Ter_SetLODPlanes(gTerRenderer.LODPlanes, gTerLODStepSize, gTerLOD0Steps, gTerLOD1Steps,
                     gTerLOD1OverlapSteps, gTerLOD2OverlapSteps);
    gTerRenderer.fDistanceCullFrameYardsSquared =
        gTerRenderer.fFOVScale * gTerRenderer.fDistanceCullYardsBase;
    gTerRenderer.fDistanceCullFrameYardsSquared =
        gTerRenderer.fDistanceCullFrameYardsSquared * gTerRenderer.fDistanceCullFrameYardsSquared;
}

// The three levels of detail's ranges, in steps of fStep: LOD 0 from 0 to (a x fFOVScale + 1)
// steps, LOD 1 from c steps before that end to b steps after, LOD 2 from d steps before that on.
void Ter_SetLODPlanes(Ter_LODPlane* pPlanes, f32 fStep, s32 a, s32 b, s32 c, s32 d) {
    s32 n = (f32)a * gTerRenderer.fFOVScale;

    pPlanes[0].fBegin = 0.0f;
    pPlanes[2].fEnd = 0.0f;
    pPlanes[0].fEnd = (f32)(n + 1) * fStep;
    pPlanes[1].fBegin = pPlanes[0].fEnd - (f32)c * fStep;
    pPlanes[1].fEnd = pPlanes[1].fBegin + (f32)b * fStep;
    pPlanes[2].fBegin = pPlanes[1].fEnd - (f32)d * fStep;
}

// Turns the 'tLOD' chunk's distances a and b into Ter_SetLODPlanes's step counts, with step s =
// gTerLODStepSize and overlaps c = gTerLOD1OverlapSteps, d = gTerLOD2OverlapSteps: *pA = ((a + c x
// s) / s - 1) / fFOVScale, *pB = c + (b + d x s - (a + c x s)) / s (integer divisions).
void Ter_LODStepsFromDistances(s32* pA, s32* pB, s32 a, s32 b) {
    // fake match: the (s32) and (int) casts of gTerLODStepSize are two conversions (the original
    // stores the one fctiwz result twice); the same cast everywhere shares one.
    s32 n = a + gTerLOD1OverlapSteps * (s32)gTerLODStepSize;

    *pA = (f32)(n / (int)gTerLODStepSize - 1) / gTerRenderer.fFOVScale;
    *pB = gTerLOD1OverlapSteps + (b + gTerLOD2OverlapSteps * (s32)gTerLODStepSize - n) / (int)gTerLODStepSize;
}

// fake match: stands in for a function the original linker stripped. The file's pool has 0.4,
// 0.2, 0.1 and 2.0 in that order right after Ter_RenderView's; its body is unknown, this one only
// reproduces the order.
static f32 GoTerrain_StrippedFn4(f32 x) {
    x += 0.4f;
    x += 0.2f;
    x += 0.1f;
    return x + 2.0f;
}

// Picks each sorted object's level of detail by its distance: the first LOD plane whose end it is
// inside. Once the 'tLOD' chunk is loaded, objects whose flags (bits 0x4, 0x10, 0x20 of the model's
// word 3) ask for it always get level 0, and the others never get level 0 when
// CameraController_IsFlybyDone is 0 for the view.
// Unless gSession.b11 is set, an object between two planes fades from one level into the next.
void Ter_SelectObjectLODs(void) {
    Ter_LODPlane* pPlanes = gTerRenderer.LODPlanes;
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
        for (i = gTerRenderer.iTotalSortObjects - 1; i >= 0; i--) {
            s32 iObject;
            iObject = gTerRenderer.pObjectSortList[i].iGlobalObjectIndex;
            fDistanceSquared = gTerRenderer.pObjectSortList[i].fDistanceSquared;
            nLast = gTerRenderer.pObjectSortList[i].nLODs - 1;
            if (((gTerRenderer.pObjectStateList[iObject].a20[3] & 4) ||
                 (gTerRenderer.pObjectStateList[iObject].a20[3] & 0x10) ||
                 (gTerRenderer.pObjectStateList[iObject].a20[3] & 0x20)) &&
                Ter_IsLODDataLoaded()) {
                nLOD = 0;
            } else {
                for (nLOD = 0; nLOD < nLast; nLOD++) {
                    if (fDistanceSquared <= pPlanes[nLOD].fEnd * pPlanes[nLOD].fEnd) break;
                }
                if (!CameraController_IsFlybyDone(
                        ViewController_GetCameraControl(gTerRenderer.iCurrentViewContext))) {
                    uFlags = gTerRenderer.pObjectStateList[iObject].a20[3];
                    if (!(uFlags & 4) && !(uFlags & 0x10) && !(uFlags & 0x20) && Ter_IsLODDataLoaded()
                        && nLOD == 0) {
                        nLOD = 1;
                    }
                }
            }
            gTerRenderer.pObjectSortList[i].fAlpha = fAlpha;
            gTerRenderer.pObjectSortList[i].iOpaqueLOD = nLOD;
            gTerRenderer.pObjectSortList[i].iTranslucentLOD = nLOD;
        }
        return;
    }
    for (i = gTerRenderer.iTotalSortObjects - 1; i >= 0; i--) {
        s32 iObject;
        fDistanceSquared = gTerRenderer.pObjectSortList[i].fDistanceSquared;
        iObject = gTerRenderer.pObjectSortList[i].iGlobalObjectIndex;
        nLast = gTerRenderer.pObjectSortList[i].nLODs - 1;
        for (nLOD = 0; nLOD < nLast; nLOD++) {
            if (fDistanceSquared <= pPlanes[nLOD].fEnd * pPlanes[nLOD].fEnd) break;
        }
        if (!CameraController_IsFlybyDone(
                ViewController_GetCameraControl(gTerRenderer.iCurrentViewContext))) {
            uFlags = gTerRenderer.pObjectStateList[iObject].a20[3];
            if (!(uFlags & 4) && !(uFlags & 0x10) && !(uFlags & 0x20) && Ter_IsLODDataLoaded() && nLOD == 0) {
                nLOD = 1;
            }
        }
        if (nLOD < nLast
            && fDistanceSquared > pPlanes[nLOD + 1].fBegin * pPlanes[nLOD + 1].fBegin) {
            // between this plane's end and the next one's start: fade over to the next level
            fT = (f32)Math_Sqrt(fDistanceSquared);
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
            uFlags = gTerRenderer.pObjectStateList[iObject].a20[3];
            if (((uFlags & 4) || (uFlags & 0x10) || (uFlags & 0x20)) && Ter_IsLODDataLoaded()) {
                fAlpha = 0.0f;
                nTranslucent = 0;
                nLOD = 0;
            }
        } else {
            fAlpha = 0.0f;
            uFlags = gTerRenderer.pObjectStateList[iObject].a20[3];
            if (((uFlags & 4) || (uFlags & 0x10) || (uFlags & 0x20)) && Ter_IsLODDataLoaded()) {
                nLOD = 0;
            }
            nTranslucent = nLOD;
        }
        gTerRenderer.pObjectSortList[i].iOpaqueLOD = nLOD;
        gTerRenderer.pObjectSortList[i].iTranslucentLOD = nTranslucent;
        gTerRenderer.pObjectSortList[i].fAlpha = fAlpha;
    }
}

// Whether the 'tLOD' chunk has been loaded.
u8 Ter_IsLODDataLoaded(void) {
    return gTerLODDataNear != -1;
}

// Sorts the objects (farthest first) into the draw lists: post-draw objects (bit 0x80 of the
// model's word 2) straight to pPostDrawItemsList; the others opaque when far or when they must
// stay solid, faded in over the near range (pNearbyObjectList), and their fading level into
// pTranslucentObjectList. Crowd objects (bit 0x20 of word 0, or 0x10 or 0x20 of word 3; not when
// CameraController_IsFlybyDone is 0 for the view) use the crowd's fade distances.
void Ter_BuildObjectDrawLists(void) {
    UObjMesh* pModel;
    s32 uFlags0;
    s32 i;
    s32 uFlags2;
    s32 uCrowd;
    f32 fNear;
    f32 fFar;
    f32 fFarSquared;
    f32 fRange;
    f32 fDistance;
    f32 fT;
    Ter_ObjectReference* pRef;
    s32 iObject;

    for (i = gTerRenderer.iTotalSortObjects - 1; i >= 0; i--) {
        pRef = &gTerRenderer.pObjectSortList[i];
        pModel = pRef->apObject[pRef->iOpaqueLOD];
        uFlags0 = Ter_GetMeshFlags(pModel, 0);
        uFlags2 = Ter_GetMeshFlags(pModel, 2);
        if ((Ter_GetMeshFlags(pModel, 3) & 0x10) || (Ter_GetMeshFlags(pModel, 3) & 0x20)) {
            uFlags0 |= 0x20;
            uFlags2 &= ~0x80;
            uFlags0 &= ~0x40;
        }
        if (!CameraController_IsFlybyDone(
                ViewController_GetCameraControl(gTerRenderer.iCurrentViewContext))) {
            uFlags0 &= ~0x20;
        }
        uCrowd = uFlags0 & 0x20;
        if (uCrowd == 0 && !(uFlags0 & 0x40)) {
            fNear = 0.1f * gTerRenderer.fFOVScale;
            fFar = 0.2f * gTerRenderer.fFOVScale;
            fFarSquared = fFar * fFar;
            fRange = fFar - fNear;
        } else if (uCrowd != 0 && !(uFlags0 & 0x40)) {
            fNear = gTerRenderer.fCrowdFadeDistanceMin * gTerRenderer.fFOVScale;
            fFar = gTerRenderer.fCrowdFadeDistanceMax * gTerRenderer.fFOVScale;
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
            pRef = &gTerRenderer.pObjectSortList[i];
            Ter_AddObjectDraw(&gTerRenderer.pPostDrawItemsList[gTerRenderer.iPostDrawItems],
                        &gTerRenderer.iPostDrawItems, 400, pRef->apObject[pRef->iOpaqueLOD], 1.0f,
                        gTerRenderer.fDefaultObjectMipmapBias[pRef->iOpaqueLOD], pRef->fDistanceSquared,
                        pRef->iGlobalObjectIndex, pRef->eClipMethod, pRef->fDistanceSquared > 0.0f, 0);
        } else if (gTerRenderer.pObjectSortList[i].fDistanceSquared < fFarSquared) {
            fDistance = Math_Sqrt(gTerRenderer.pObjectSortList[i].fDistanceSquared);
            if (fDistance > fFar || (uFlags0 & 0x40)
                || (uCrowd == 0
                    && ((gTerRenderer.pObjectSortList[i].f14 > gTerRenderer.fXZDistanceToClosestBallSquared
                         && gTerRenderer.pObjectSortList[i].f18 > 0.0f)
                        || Ter_IsBallStoppedInModel(gTerRenderer.pObjectSortList[i].apObject[0])))) {
                iObject = gTerRenderer.pObjectSortList[i].iGlobalObjectIndex;
                gTerRenderer.pObjectStateList[iObject].aView[gTerRenderer.iCurrentViewContext].n4 = 3;
                gTerRenderer.pObjectStateList[iObject].aView[gTerRenderer.iCurrentViewContext].f0 = 1.0f;
                pRef = &gTerRenderer.pObjectSortList[i];
                Ter_AddObjectDraw(&gTerRenderer.pOpaqueObjectList[gTerRenderer.iOpaqueObjects],
                            &gTerRenderer.iOpaqueObjects, 650, pRef->apObject[pRef->iOpaqueLOD], 1.0f,
                            gTerRenderer.fDefaultObjectMipmapBias[pRef->iOpaqueLOD], pRef->fDistanceSquared,
                            pRef->iGlobalObjectIndex, pRef->eClipMethod, pRef->fDistanceSquared > 0.0f, 0);
            } else {
                fT = (fDistance - fNear) / fRange;
                if (fT < 0.0f) {
                    fT = 0.0f;
                }
                if (fT != 0.0f) {
                    pRef = &gTerRenderer.pObjectSortList[i];
                    Ter_AddObjectDraw(&gTerRenderer.pNearbyObjectList[gTerRenderer.iNearbyObjects],
                                &gTerRenderer.iNearbyObjects, 70, pRef->apObject[pRef->iOpaqueLOD], fT,
                                gTerRenderer.fDefaultObjectMipmapBias[pRef->iOpaqueLOD],
                                pRef->fDistanceSquared, pRef->iGlobalObjectIndex, pRef->eClipMethod,
                                pRef->fDistanceSquared > 0.0f, 0);
                }
            }
        } else {
            pRef = &gTerRenderer.pObjectSortList[i];
            gTerRenderer.pObjectStateList[pRef->iGlobalObjectIndex]
                .aView[gTerRenderer.iCurrentViewContext].n4 = 3;
            pRef = &gTerRenderer.pObjectSortList[i];
            Ter_AddObjectDraw(&gTerRenderer.pOpaqueObjectList[gTerRenderer.iOpaqueObjects],
                        &gTerRenderer.iOpaqueObjects, 650, pRef->apObject[pRef->iOpaqueLOD], 1.0f,
                        gTerRenderer.fDefaultObjectMipmapBias[pRef->iOpaqueLOD], pRef->fDistanceSquared,
                        pRef->iGlobalObjectIndex, pRef->eClipMethod, pRef->fDistanceSquared > 0.0f, 0);
        }
        if (gSession.b11 == 0) {
            pRef = &gTerRenderer.pObjectSortList[i];
            if (pRef->fAlpha != 0.0f && !(uFlags0 & 0x40)) {
                Ter_AddObjectDraw(&gTerRenderer.pTranslucentObjectList[gTerRenderer.iTranslucentObjects],
                            &gTerRenderer.iTranslucentObjects, 200, pRef->apObject[pRef->iTranslucentLOD],
                            pRef->fAlpha, gTerRenderer.fDefaultObjectMipmapBias[pRef->iTranslucentLOD],
                            pRef->fDistanceSquared, pRef->iGlobalObjectIndex, pRef->eClipMethod,
                            pRef->fDistanceSquared > 0.0f, 0);
            }
        }
    }
}

// Whether a ball has settled inside pModel's bounding sphere: a ball that has left where its shot
// started, has hit something (nCollideCount) and moves slower than 10.
u8 Ter_IsBallStoppedInModel(UObjMesh* pModel) {
    f32* pSphere = Ter_GetMeshBoundingSphere(pModel);
    f32 fRadiusSq = pSphere[3] * pSphere[3];
    int i;

    for (i = 0; i < gNumPlayersSetUp; i++) {
        if (LLMath_SquareDistanceBetween3(gPlayers[i].ball.vPos, gPlayers[i].ball.vStart) > 0.0f
            && LLMath_SquareDistanceBetween3(pSphere, gPlayers[i].ball.vPos) < fRadiusSq
            && gPlayers[i].ball.nCollideCount != 0 && gPlayers[i].ball.fSpeed < 10.0f) {
            return 1;
        }
    }
    return 0;
}

// Adds a draw of pModel to a draw list: fills pDraw and counts it in *pCount. A model whose flags
// (bytes 0 and 3) ask for it is skipped in modes 6-8 (GM_IsSpeedGolfMode); one with bits 0 and 1 of byte 0
// is otherwise drawn as its node chosen by the object's state (n18).
void Ter_AddObjectDraw(Ter_ObjectDrawData* pDraw, s32* pCount, s32 nUnused, UObjMesh* pModel, f32 fAlpha,
                 f32 fMipmapBias, f32 fDistanceSquared, s32 iObject, s32 eClipMethod, u8 bUseFog,
                 u8 bSetsPrimField) {
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
    uFlags0 = Ter_GetMeshFlags(pModel, 0);
    uFlags3 = Ter_GetMeshFlags(pModel, 3);
    if (uFlags0 & 1) {
        if (uFlags0 & 2) {
            if (GM_IsSpeedGolfMode()) return;
            pDraw->pObject = Ter_GetMeshChild(pModel, gTerRenderer.pObjectStateList[iObject].n18);
        }
    } else if ((uFlags3 & 4) || (uFlags3 & 0x10) || (uFlags3 & 0x20)) {
        if (GM_IsSpeedGolfMode()) return;
    }
    if (pDraw->pObject->p18 != NULL) {
        pDraw->eShaderObjectType = pDraw->pObject->p18->n0;
    }
    (*pCount)++;
}

// Draws render pass nRenderPass's sorted patches, if it has any: each list (the pass a patch is
// drawn in, 0..2) one clip method at a time, then the deferred items with the terrain's filters.
// fake match: the (u32) casts on the clip index; with a signed index the compiler walks one pointer
// through the lists instead of keeping the list base and a byte offset apart as EA's code does.
void Ter_DrawPatchPass(int nRenderPass) {
    u8 bFirst = 1;
    u8 bAny;
    int nList;
    int nClip;
    Ter_PatchReference* pPatch;

    bAny = 0;
    for (nList = 0; nList < 3; nList++) {
        for (nClip = 0; nClip < 3; nClip++) {
            if (gTerRenderer.pSortedPatchList[nRenderPass][nList][(u32)nClip] != NULL) {
                bAny = 1;
                break;
            }
        }
    }
    if (bAny) {
        if (nRenderPass == 2) {
            Ter_SetZWrite(0);
        }
        RenderState_Flush();
        RenderState_SetDrawFlags(0x70);
        DS_vSetAlphaTestMode(0, 6, 1);
        for (nList = 0; nList <= 2; nList++) {
            if (nRenderPass != 2) {
                if (nList == 0) {
                    Ter_SetZWrite(1);
                } else {
                    Ter_SetZWrite(0);
                }
            }
            for (nClip = 0; nClip <= 2; nClip++) {
                if (gTerRenderer.pSortedPatchList[nRenderPass][nList][(u32)nClip] != NULL) {
                    switch (nClip) {
                    case 2:
                        RenderState_SetClipMode(1);
                        break;
                    case 1:
                        RenderState_SetClipMode(1);
                        break;
                    default:
                        RenderState_SetClipMode(0);
                        break;
                    }
                    RenderState_Flush();
                    for (pPatch =
                             gTerRenderer.pSortedPatchList[nRenderPass][nList][(u32)nClip];
                         pPatch != NULL; pPatch = pPatch->pNext[nList]) {
                        Ter_DrawPatchGround(pPatch->pGround, nClip, nList, pPatch->n1C, pPatch->n18,
                                            pPatch->n20,
                                    &bFirst, 0, 0, pPatch->fDistance,
                                    pPatch->fDistance + 2.0f * pPatch->fBoundingRadius);
                    }
                }
            }
        }
        Ter_DrawObjectList(gTerRenderer.pDeferredItemsList, gTerRenderer.iDeferredItems,
                    gTerRenderer.eTerrainFilterMin, gTerRenderer.eTerrainFilterMag);
        gTerRenderer.iDeferredItems = 0;
        DS_vSetAlphaTestMode(1, 6, 1);
        RenderState_Flush();
        Ter_SetZWrite(1);
        RenderState_Flush();
    }
}

// Turns z writes on or off, only while the terrain manages them (boManageZUpdate), and remembers
// the setting in gTerLastZWrite (Ter_SetObjectRenderState compares against it).
void Ter_SetZWrite(int n) {
    if (gTerRenderer.boManageZUpdate) {
        DS_vEnableZBufferUpdate(n);
        gTerLastZWrite = n;
    }
}

// Draws render pass 0's fourth patch lists (pSortedPatchList[0][3]), if it has any, without z
// writes; the lens's far clip distance (0xAC) is raised by 25 while the renderer is set up, then
// put back. In split screen, patches with bit 0x8 of n1C are left out.
// fake match: the (u32) casts on the clip index, as in Ter_DrawPatchPass.
void Ter_DrawFarClipPatches(void) {
    u8 bFirst = 1;
    u8 bAny;
    int nClip;
    Ter_PatchReference* pPatch;
    CamLens* pLens;
    f32 fAC;

    bAny = 0;
    for (nClip = 0; nClip < 3; nClip++) {
        if (gTerRenderer.pSortedPatchList[0][3][(u32)nClip] != NULL) {
            bAny = 1;
            break;
        }
    }
    if (bAny) {
        Ter_SetZWrite(0);
        pLens = ((Camera*)*gppCurrentRenderCtx)->unk10;
        fAC = CA_fGetCameraFarZ((u8*)pLens);
        Camera_SetLensFarClip((u8*)pLens, 25.0f + fAC);
        RC_UpdateCurrentScreenMatrices();
        RC_vUpdateCurrentRenderCtxTransformationMatrices();
        RenderState_SetCameraMatrices();
        RenderState_SetCameraMatrices();
        RenderState_SetCameraMatrices();
        Camera_SetLensFarClip((u8*)pLens, fAC);
        RenderState_Flush();
        RenderState_SetDrawFlags(0x70);
        for (nClip = 0; nClip <= 2; nClip++) {
            if (gTerRenderer.pSortedPatchList[0][3][(u32)nClip] != NULL) {
                switch (nClip) {
                case 2:
                    RenderState_SetClipMode(1);
                    break;
                case 1:
                    RenderState_SetClipMode(1);
                    break;
                default:
                    RenderState_SetClipMode(0);
                    break;
                }
                RenderState_Flush();
                for (pPatch =
                         gTerRenderer.pSortedPatchList[0][3][(u32)nClip];
                     pPatch != NULL; pPatch = pPatch->pNext[3]) {
                    if (!gSession.nSplitScreen || !(pPatch->n1C & 8)) {
                        Ter_DrawPatchGround(pPatch->pGround, nClip, 3, pPatch->n1C, pPatch->n18, pPatch->n20,
                                            &bFirst,
                                    0, 0, pPatch->fDistance,
                                    pPatch->fDistance + 2.0f * pPatch->fBoundingRadius);
                    }
                }
            }
        }
        DS_vSetAlphaTestMode(1, 6, 1);
        RenderState_Flush();
        Ter_SetZWrite(1);
        RenderState_Flush();
    }
}

// Draws the opaque object list, then the translucent one (objects fading between two levels of
// detail) if it has any, both with the object filters; alpha test back to reference 128.
void Ter_DrawObjects(void) {
    Ter_DrawObjectList(gTerRenderer.pOpaqueObjectList, gTerRenderer.iOpaqueObjects,
                       gTerRenderer.eObjectFilterMin,
                gTerRenderer.eObjectFilterMag);
    if (gTerRenderer.iTranslucentObjects != 0) {
        Ter_DrawObjectList(gTerRenderer.pTranslucentObjectList, gTerRenderer.iTranslucentObjects,
                    gTerRenderer.eObjectFilterMin, gTerRenderer.eObjectFilterMag);
    }
    DS_vSetAlphaTestMode(1, 6, 128);
    RenderState_Flush();
}

// Draws the post-draw terrain patches, each in every pass (bits 0..2 of n1C) it takes part in.
void Ter_DrawPostDrawPatches(void) {
    u8 bFirst = 1;
    int i;
    int nPass;
    int nPassBit;

    Ter_BeginRender();
    RenderState_SetDrawFlags(0x70);
    for (i = 0; i < gTerRenderer.iTotalPostDrawTerrainPatches; i++) {
        switch (gTerRenderer.pPostDrawTerrainList[i].eClipMethod) {
        case 2:
            RenderState_SetClipMode(1);
            break;
        case 1:
            RenderState_SetClipMode(1);
            break;
        default:
            RenderState_SetClipMode(0);
            break;
        }
        RenderState_Flush();
        for (nPass = 0, nPassBit = 1; nPass <= 2; nPass++, nPassBit <<= 1) {
            if (nPassBit & gTerRenderer.pPostDrawTerrainList[i].n1C) {
                Ter_DrawPatchGround(gTerRenderer.pPostDrawTerrainList[i].pGround,
                            gTerRenderer.pPostDrawTerrainList[i].eClipMethod, nPass,
                            gTerRenderer.pPostDrawTerrainList[i].n1C,
                            gTerRenderer.pPostDrawTerrainList[i].n18,
                            gTerRenderer.pPostDrawTerrainList[i].n20, &bFirst, 1, 0,
                            gTerRenderer.pPostDrawTerrainList[i].fDistance,
                            gTerRenderer.pPostDrawTerrainList[i].fDistance
                                + 2.0f * gTerRenderer.pPostDrawTerrainList[i].fBoundingRadius);
            }
        }
    }
    Ter_EndRender();
}

// Draws the post-draw objects, then the nearby ones (fading in as the camera nears) without z
// writes. gomainloop calls it after a view's scene, for views 0 and 1.
void Ter_DrawPostDrawObjects(void) {
    Ter_BeginRender();
    Ter_DrawObjectList(gTerRenderer.pPostDrawItemsList, gTerRenderer.iPostDrawItems,
                       gTerRenderer.eObjectFilterMin,
                gTerRenderer.eObjectFilterMag);
    Ter_SetZWrite(0);
    Ter_DrawObjectList(gTerRenderer.pNearbyObjectList, gTerRenderer.iNearbyObjects,
                       gTerRenderer.eObjectFilterMin,
                gTerRenderer.eObjectFilterMag);
    Ter_SetZWrite(1);
    RenderState_SetConstantAlphaOn(0);
    Ter_EndRender();
    DS_vSetAlphaTestMode(1, 6, 128);
    RenderState_Flush();
}

// Draws one patch's ground in render pass nPass: its mesh for the pass and the patch's bits (n1C
// bits 0-2 and 0x80, n18 bit 1), then the extra meshes its word 2 asks for: bit 0x8 drawn at once,
// bits 0x10 and 0x20 as deferred items, bit 0x40 (near enough, not in split screen) raised by
// 0.005 without z writes. Passes 1 and 2 leave out patches beyond 100 x fFOVScale unless the
// ground's bit 0x80 is set. A lake surface (n20 bit 0x80) uses fLakeSurfaceMipmapBias; when
// CameraController_IsFlybyDone is 0 for the view, one with ground bit 0x80 is left out. *pbFirst
// tracks a renderer state switched by fFar; b2 keeps it, the bit 0x10 deferred mesh and the raised
// mesh out. b1 is not read.
void Ter_DrawPatchGround(void* pGround, s32 eClipMethod, s32 nPass, s32 n1C, s32 n18, s32 n20, u8* pbFirst,
                         u8 b1,
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
    if (nPass >= 1 && nPass <= 2 && fNear > 100.0f * gTerRenderer.fFOVScale
        && !(Ter_GetMeshFlags(pGround, 0) & 0x80)) {
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
        if (!CameraController_IsFlybyDone(ViewController_GetCameraControl(gTerRenderer.iCurrentViewContext))
            && (Ter_GetMeshFlags(pGround, 0) & 0x80)) {
            return;
        }
        bLake = 1;
        fBias = gTerRenderer.fLakeSurfaceMipmapBias;
    }
    pGround = Ter_GetMeshChild(pGround, gTerLowBitCounts[nPass][nMesh]);
    bFirst = *pbFirst;
    if (bFirst == 0 && fFar > 0.0f && b2 == 0) {
        *pbFirst = 1;
        RenderState_SetDrawFlags(0x70);
        RenderState_Flush();
    } else if ((fFar <= 0.0f || b2 != 0) && bFirst != 0) {
        *pbFirst = 0;
        RenderState_SetDrawFlags(0x50);
        RenderState_Flush();
    }
    uFlags2 = Ter_GetMeshFlags(pGround, 2);
    pMesh = Ter_GetMeshChild(pGround, 0);
    if ((n20 & uPinBit) && !(n20 & uOtherPins)) {
        DS_vSetAlphaTestMode(1, 6, 1);
        RenderState_Flush();
        bPinSet = 1;
    }
    if (bLake) {
        RenderState_Flush();
    }
    if (uFlags2 & 8) {
        Ter_DrawMeshCurrentPart((u8*)pMesh);
        pMesh = Ter_GetMeshNext(pMesh);
    }
    if (bLake) {
        RenderState_Flush();
    }
    if ((uFlags2 & 0x10) && b2 == 0) {
        if (pMesh->n20 != 0) {
            Ter_AddObjectDraw(&gTerRenderer.pDeferredItemsList[gTerRenderer.iDeferredItems],
                        &gTerRenderer.iDeferredItems, 50, pMesh, 1.0f, bLake ? fBias : 0.0f, 0.0f, 0x289,
                        eClipMethod, fFar > 0.0f, 0);
        }
        pMesh = Ter_GetMeshNext(pMesh);
    }
    if (uFlags2 & 0x20) {
        if (pMesh->n20 != 0) {
            Ter_AddObjectDraw(&gTerRenderer.pDeferredItemsList[gTerRenderer.iDeferredItems],
                        &gTerRenderer.iDeferredItems, 50, pMesh, 1.0f, bLake ? fBias : 0.0f, 0.0f, 0x289,
                        eClipMethod, fFar > 0.0f, 0);
        }
        pMesh = Ter_GetMeshNext(pMesh);
    }
    if (gSession.nSplitScreen == 0 && (uFlags2 & 0x40) && b2 == 0) {
        if (fNear < 60.0f * gTerRenderer.fFOVScale && pMesh->n20 != 0) {
            LLMath_IdentifyMat(mRaise);
            mRaise[3][1] = 0.005f;
            RC_vSetCurrentRenderCtxTransformationMatrix(mRaise);
            RenderState_SetCameraMatrices();
            DS_vEnableZBufferUpdate(0);
            RenderState_Flush();
            Ter_DrawMeshCurrentPart((u8*)pMesh);
            DS_vEnableZBufferUpdate(1);
            RC_vSetCurrentRenderCtxTransformationMatrix(NULL);
            RenderState_SetCameraMatrices();
            RenderState_Flush();
        }
        Ter_GetMeshNext(pMesh);
    }
    if (bPinSet) {
        DS_vSetAlphaTestMode(0, 6, 1);
        RenderState_Flush();
    }
}

// fake match: EA passes the flags through an inline; CodeWarrior substitutes the argument expression at
// each use, so it is built twice and its bits are taken apart again.
static inline void Ter_FlagBits(u32 uFlags) {
    RenderState_SetDrawFlags(((uFlags & 0x10) ? 0x10 : 0) | ((uFlags & 0x20) ? 0x20 : 0) | 0x40);
}

// Draws nCount objects of a draw list, switching the renderer state only when it changes from one
// object to the next: the clip method, the mipmap bias, and the flags (0x40, fog 0x20, and 0x10 for
// shader types other than 1 and 3) unless the object sets its own. Objects whose state word 0 has
// bit 0x1 hand their f4 to row 2 or 3 of SD_SetShaderTypeParameters (by shader type); without bit
// 0x2 its swing about 0.5 is cut by up to fTreeDampingMaxForce up close, less with distance (not at
// all from the squared distance fTreeDampingDistance on). The filters are not read.
void Ter_DrawObjectList(Ter_ObjectDrawData* pList, s32 nCount, s32 eFilterMin, s32 eFilterMag) {
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
    f32 fDamp;

    Ter_ResetObjectRenderState();
    for (i = 0; i < nCount; i++) {
        pDraw = &pList[i];
        iObject = pDraw->iGlobalObjectIndex;
        if (eClipMethod != pDraw->eClipMethod) {
            eClipMethod = pDraw->eClipMethod;
            switch (eClipMethod) {
            case 2:
                RenderState_SetClipMode(1);
                break;
            case 1:
                RenderState_SetClipMode(1);
                break;
            default:
                RenderState_SetClipMode(0);
                break;
            }
            bDirty = 1;
        }
        if (fBias != pDraw->fMipmapBias) {
            fBias = pDraw->fMipmapBias;
            bDirty = 1;
        }
        bNewBit5 = (gTerRenderer.pObjectStateList[iObject].a20[1] >> 5) & 1;
        bNewShaded = pDraw->eShaderObjectType != 1 && pDraw->eShaderObjectType != 3;
        if (pDraw->bSetsPrimField == 0
            && (bUseFog != pDraw->bUseFog || bBit5 != bNewBit5 || bShaded != bNewShaded || bForce)) {
            bUseFog = pDraw->bUseFog;
            bBit5 = bNewBit5;
            bShaded = bNewShaded;
            Ter_FlagBits((bShaded ? 0x10 : 0) | 0x40 | (bUseFog ? 0x20 : 0));
            bForce = 0;
            bDirty = 1;
        }
        if (Ter_SetObjectRenderState(pDraw, 0)) {
            bDirty = 1;
        }
        if ((gTerRenderer.pObjectStateList[iObject].a20[0] & 1)
            && !(gTerRenderer.pObjectStateList[iObject].a20[0] & 2)) {
            fDamp = 1.0f;
            if (gTerRenderer.fTreeDampingMaxForce) {
                fDamp = gTerRenderer.fTreeDampingMaxForce
                        * (pDraw->fDistanceSquared / gTerRenderer.fTreeDampingDistance)
                      + (1.0f - gTerRenderer.fTreeDampingMaxForce);
                if (fDamp > 1.0f) {
                    fDamp = 1.0f;
                }
            }
            if (bDirty) {
                RenderState_Flush();
                bDirty = 0;
            }
            if (pDraw->eShaderObjectType == 2) {
                fWave2 = fDamp * (gTerRenderer.pObjectStateList[iObject].f4 - 0.5f) + 0.5f;
                SD_SetShaderTypeParameters(2, &fWave2);
            } else if (pDraw->eShaderObjectType == 3) {
                fWave3 = fDamp * (gTerRenderer.pObjectStateList[iObject].f4 - 0.5f) + 0.5f;
                SD_SetShaderTypeParameters(3, &fWave3);
            }
        } else if ((gTerRenderer.pObjectStateList[iObject].a20[0] & 1)
                   && (gTerRenderer.pObjectStateList[iObject].a20[0] & 2)) {
            if (bDirty) {
                RenderState_Flush();
                bDirty = 0;
            }
            if (pDraw->eShaderObjectType == 2) {
                fWave2 = gTerRenderer.pObjectStateList[iObject].f4;
                SD_SetShaderTypeParameters(2, &fWave2);
            } else if (pDraw->eShaderObjectType == 3) {
                fWave3 = gTerRenderer.pObjectStateList[iObject].f4;
                SD_SetShaderTypeParameters(3, &fWave3);
            }
        } else if (bDirty) {
            RenderState_Flush();
            bDirty = 0;
        }
        Ter_DrawMeshCurrentPart((u8*)pDraw->pObject);
        if (pDraw->bSetsPrimField) {
            bForce = 1;
        }
    }
}

// Forgets the last object's z write and alpha (gTerLastZWrite, gTerLastObjectAlpha), so that
// Ter_SetObjectRenderState sets the next object up in full.
void Ter_ResetObjectRenderState(void) {
    gTerLastZWrite = -1;
    gTerLastObjectAlpha = -1.0f;
}

// Sets the renderer up for an object's draw, unless its alpha and z writes are those of the last one
// (gTerLastObjectAlpha, gTerLastZWrite) and bForce is clear; returns whether it did. An opaque object gets
// the mesh's own blend flags and z writes, a faded one draws with its alpha.
u8 Ter_SetObjectRenderState(Ter_ObjectDrawData* pDraw, u8 bForce) {
    f32 fAlpha = pDraw->fAlpha;
    s32 nRef;
    u32 uFlags;
    u8 bZWrite;

    if (Ter_GetMeshDrawFlags(pDraw->pObject) & 2) {
        nRef = 1;
        uFlags = 0x40;
        bZWrite = 0;
    } else {
        nRef = 0x50;
        uFlags = 0;
        bZWrite = 1;
    }
    if (fAlpha != gTerLastObjectAlpha || (s8)bZWrite != gTerLastZWrite || bForce) {
        if (1.0f == fAlpha) {
            DS_vSetAlphaTestMode(1, 6, nRef);
            RenderState_ChangeDrawFlags(0x40, ((uFlags & 0x40) ? 0x40 : 0)
                                  | (((uFlags & 0x10) ? 0x10 : 0) | ((uFlags & 0x20) ? 0x20 : 0)));
            Ter_SetZWrite(bZWrite);
            RenderState_SetConstantAlphaOn(0);
            RenderState_SetBlendFactors(4, 5);
        } else {
            DS_vSetAlphaTestMode(1, 6, fAlpha * nRef);
            Ter_SetZWrite(1);
            RenderState_SetConstantAlphaOn(1);
            RenderState_SetConstantAlpha(255.0f * (0.5f * fAlpha));
        }
        RenderState_Flush();
        gTerLastObjectAlpha = fAlpha;
        return 1;
    }
    return 0;
}

// Starts the crowd's animation: after fDelay seconds when that is above 0 (Ter_AnimateObjects counts it
// down, then calls here again), otherwise now, for fDuration, each crowd object from a
// pseudo-random point of its cycle. EA passes the share of the crowd that starts, but sets it to 1.
void Ter_StartCrowdAnimation(f32 fPercentage, f32 fDuration, f32 fDelay) {
    f32 fRand = 0.0f;
    int i;

    fPercentage = 1.0f;
    if (fDelay > fRand) {
        gTerRenderer.fCrowdAnimationDelayedStartTimer = fDelay;
        gTerRenderer.fCrowdAnimationDelayedStartPercentage = fPercentage;
        gTerRenderer.fCrowdAnimationDelayedStartDuration = fDuration;
        return;
    }
    gTerRenderer.fCrowdAnimationCountdown = fDuration;
    gTerRenderer.fCrowdAnimationDelayedStartPercentage = fPercentage;
    for (i = 0; i < TER_NUM_OBJECTS; i++) {
        s32 nFlags = gTerRenderer.pObjectStateList[i].a20[0];
        s32 nFlags3 = gTerRenderer.pObjectStateList[i].a20[3];

        if (nFlags & 1) {
            if ((nFlags & 2) && !(nFlags3 & 0x40)) {
                if (fPercentage >= fRand) {
                    gTerRenderer.pObjectStateList[i].n1C = 3;
                    gTerRenderer.pObjectStateList[i].f14 = gTerRenderer.pObjectStateList[i].f10;
                }
                fRand *= 131.2934f;
                fRand += 82.459f;
                fRand -= Math_Floor(fRand);
            } else if ((nFlags & 2) && (nFlags3 & 0x40)) {
                if (fPercentage >= fRand) {
                    gTerRenderer.pObjectStateList[i].n1C = 1;
                    gTerRenderer.pObjectStateList[i].f14 = gTerRenderer.pObjectStateList[i].f10;
                }
                fRand *= 131.2934f;
                fRand += 82.459f;
                fRand -= Math_Floor(fRand);
            }
        }
    }
}

// Stops the crowd's animation and puts every object with bits 0 and 1 of a20[0] back: n1C to 0,
// and f14 to f10, or with bReset f14 and n18 to 0.
void Ter_StopCrowdAnimation(u8 bReset) {
    int i;

    gTerRenderer.fCrowdAnimationDelayedStartTimer = -1.0f;
    gTerRenderer.fCrowdAnimationCountdown = 0.0f;
    for (i = 0; i < TER_NUM_OBJECTS; i++) {
        s32 nFlags = gTerRenderer.pObjectStateList[i].a20[0];
        s32 nFlags3 = gTerRenderer.pObjectStateList[i].a20[3];

        if (nFlags & 1) {
            if ((nFlags & 2) && !(nFlags3 & 0x40)) {
                gTerRenderer.pObjectStateList[i].n1C = 0;
                if (bReset) {
                    gTerRenderer.pObjectStateList[i].f14 = 0.0f;
                    gTerRenderer.pObjectStateList[i].n18 = 0;
                } else {
                    gTerRenderer.pObjectStateList[i].f14 = gTerRenderer.pObjectStateList[i].f10;
                }
            } else if ((nFlags & 2) && (nFlags3 & 0x40)) {
                gTerRenderer.pObjectStateList[i].n1C = 0;
                if (bReset) {
                    gTerRenderer.pObjectStateList[i].f14 = 0.0f;
                    gTerRenderer.pObjectStateList[i].n18 = 0;
                } else {
                    gTerRenderer.pObjectStateList[i].f14 = gTerRenderer.pObjectStateList[i].f10;
                }
            }
        }
    }
}

f32 Ter_GetCrowdAnimationCountdown(void) {
    return gTerRenderer.fCrowdAnimationCountdown;
}

f32 Ter_GetCrowdAnimationDelayedStartPercentage(void) {
    return gTerRenderer.fCrowdAnimationDelayedStartPercentage;
}

// The ball hit object nObject of patch nPatch (event.c): its state's n1C becomes 1 when bit 0x1 of
// its word 3 is set (Ter_AnimateObjects then raises that object).
void Ter_SetObjectHit(u16 nPatch, u16 nObject) {
    Ter_ObjectState* pState =
        &gTerRenderer.pObjectStateList[gTerRenderer.iPatchFirstObjectInstanceIndex[nPatch] + nObject];

    if (pState->a20[3] & 1) {
        pState->n1C = 1;
    }
}

// The crowd's pose steps (Ter_AnimateObjects): for crowd objects without bit 0x40 of word 3, and
// (gTerCrowdPoseStepsFlag40) for those with it
TerPoseStep gTerCrowdPoseSteps[6] = {
    { 0, 3, 1, 0.5f, 0.0f },
    { 1, 3, 2, 1.0f, 0.0f },
    { 2, 3, 3, 1.0f, 0.5f },
    { 3, 0, 2, 0.5f, 1.0f },
    { 2, 0, 1, 0.0f, 1.0f },
    { 1, 0, 0, 0.0f, 0.5f },
};

TerPoseStep gTerCrowdPoseStepsFlag40[2] = {
    { 0, 1, 1, 1.0f, 0.5f },
    { 1, 0, 0, 0.5f, 1.0f },
};

// Animates the course objects once a frame (the frame time capped at 1/30 s): runs the crowd
// countdowns, fades each object's views in (state 2) or out (state 0), and moves each object with
// bit 0x1 of word 0: crowd members (bit 0x2) held in iCrowdPose, swaying in their pose or easing to
// the next one through gTerCrowdPoseSteps (gTerCrowdPoseStepsFlag40 with bit 0x40 of word 3); bit
// 0x1 of word 3 rises to 1 once n1C is 1; the trees sway by their period with noise.
// fake match: `3 == n18` in the two pose-step tests (register order; found by the permuter).
void Ter_AnimateObjects(void) {
    f32 fTime;
    s32 i;
    s32 v;
    s32 k;
    s32 uFlags0;
    s32 uFlags3;
    f32 fPeriod;
    f32 fStep;
    f32 fNoise;
    f32 fSum;
    f32 fScale;

    fTime = gSession.fFrameTime;
    if (fTime > 1.0f / 30.0f) {
        fTime = 1.0f / 30.0f;
    }
    if (fTime < 0.0f) {
        fTime = 0.0f;
    }
    if (gTerRenderer.fCrowdAnimationDelayedStartTimer > 0.0f) {
        gTerRenderer.fCrowdAnimationDelayedStartTimer -= fTime;
        if (gTerRenderer.fCrowdAnimationDelayedStartTimer <= 0.0f) {
            Ter_StartCrowdAnimation(gTerRenderer.fCrowdAnimationDelayedStartPercentage,
                        gTerRenderer.fCrowdAnimationDelayedStartDuration, 0.0f);
        }
    }
    if (gTerRenderer.fCrowdAnimationCountdown > 0.0f) {
        gTerRenderer.fCrowdAnimationCountdown -= fTime;
        if (gTerRenderer.fCrowdAnimationCountdown <= 0.0f) {
            Ter_StopCrowdAnimation(0);
        }
    }
    for (i = 0; i < TER_NUM_OBJECTS; i++) {
        for (v = 0; v < 2; v++) {
            if (gTerRenderer.pObjectStateList[i].aView[v].n4 == 2) {
                gTerRenderer.pObjectStateList[i].aView[v].f0 += 4.0f * gSession.fFrameTime;
                if (gTerRenderer.pObjectStateList[i].aView[v].f0 >= 1.0f) {
                    gTerRenderer.pObjectStateList[i].aView[v].f0 = 1.0f;
                    gTerRenderer.pObjectStateList[i].aView[v].n4 = 3;
                }
            }
            if (gTerRenderer.pObjectStateList[i].aView[v].n4 == 0) {
                gTerRenderer.pObjectStateList[i].aView[v].f0 -= 4.0f * gSession.fFrameTime;
                if (gTerRenderer.pObjectStateList[i].aView[v].f0 <= 0.0f) {
                    gTerRenderer.pObjectStateList[i].aView[v].f0 = 0.0f;
                    gTerRenderer.pObjectStateList[i].aView[v].n4 = 1;
                }
            }
        }
        uFlags0 = gTerRenderer.pObjectStateList[i].a20[0];
        uFlags3 = gTerRenderer.pObjectStateList[i].a20[3];
        if (!(uFlags0 & 1)) {
            continue;
        }
        if ((uFlags0 & 2) && gTerRenderer.iCrowdPose != 0) {
            if (gTerRenderer.iCrowdPose == 1) {
                gTerRenderer.pObjectStateList[i].n18 = 0;
                gTerRenderer.pObjectStateList[i].n1C = 0;
            } else {
                gTerRenderer.pObjectStateList[i].n18 = 3;
                gTerRenderer.pObjectStateList[i].n1C = 3;
            }
            gTerRenderer.pObjectStateList[i].f4 = gTerRenderer.fCrowdInterpValue;
        } else if ((uFlags0 & 2) && !(uFlags3 & 0x40)) {
            gTerRenderer.pObjectStateList[i].nC += (u32)(FRAME_RATE * fTime);
            if (gTerRenderer.pObjectStateList[i].n18 == gTerRenderer.pObjectStateList[i].n1C
                || gTerRenderer.pObjectStateList[i].f14 > 0.0f) {
                fPeriod = 4.0f * (gTerRenderer.pObjectStateList[i].f0 - gTerRenderer.fTreeMinPeriod) + 1.5f;
                if (gTerRenderer.pObjectStateList[i].n18 == 3) {
                    gTerRenderer.pObjectStateList[i].f4 =
                        0.5f * Math_Sin(10.0f
                                           * (6.2831855f
                                              * Ter_GetTimeInCycle(gTerRenderer.pObjectStateList[i].nC,
                                                            fPeriod / 10.0f)
                                              / fPeriod))
                        + 0.5f;
                } else {
                    gTerRenderer.pObjectStateList[i].f4 =
                        0.5f * Math_Sin(0.5f
                                           * (6.2831855f
                                              * Ter_GetTimeInCycle(gTerRenderer.pObjectStateList[i].nC,
                                                            fPeriod / 0.5f)
                                              / fPeriod))
                        + 0.5f;
                }
                gTerRenderer.pObjectStateList[i].f14 -= fTime;
            } else {
                for (k = 0; k < sizeof(gTerCrowdPoseSteps) / sizeof(gTerCrowdPoseSteps[0]); k++) {
                    if (gTerRenderer.pObjectStateList[i].n18 == gTerCrowdPoseSteps[k].n0
                        && gTerRenderer.pObjectStateList[i].n1C == gTerCrowdPoseSteps[k].n4) {
                        fStep = gTerCrowdPoseSteps[k].fC - gTerRenderer.pObjectStateList[i].f4;
                        if (gTerRenderer.pObjectStateList[i].n18 == 2
                            || 3 == gTerRenderer.pObjectStateList[i].n18) {
                            if (fStep > 6.0f * fTime) {
                                fStep = 6.0f * fTime;
                            }
                            if (fStep < -6.0f * fTime) {
                                fStep = -6.0f * fTime;
                            }
                        } else {
                            if (fStep > 4.0f * fTime) {
                                fStep = 4.0f * fTime;
                            }
                            if (fStep < -4.0f * fTime) {
                                fStep = -4.0f * fTime;
                            }
                        }
                        gTerRenderer.pObjectStateList[i].f4 += fStep;
                        if (fabsf(gTerRenderer.pObjectStateList[i].f4 - gTerCrowdPoseSteps[k].fC) < 0.01f) {
                            gTerRenderer.pObjectStateList[i].n18 = gTerCrowdPoseSteps[k].n8;
                            gTerRenderer.pObjectStateList[i].f4 = gTerCrowdPoseSteps[k].f10;
                            gTerRenderer.pObjectStateList[i].nC = 0;
                            break;
                        }
                    }
                }
            }
        } else if ((uFlags0 & 2) && (uFlags3 & 0x40)) {
            gTerRenderer.pObjectStateList[i].nC += (u32)(FRAME_RATE * fTime);
            if (gTerRenderer.pObjectStateList[i].n18 == gTerRenderer.pObjectStateList[i].n1C
                || gTerRenderer.pObjectStateList[i].f14 > 0.0f) {
                if (gTerRenderer.pObjectStateList[i].n18 == 0) {
                    gTerRenderer.pObjectStateList[i].f4 -= 5.0f * fTime;
                    if (gTerRenderer.pObjectStateList[i].f4 < 0.0f) {
                        gTerRenderer.pObjectStateList[i].f4 = 0.0f;
                    }
                    gTerRenderer.pObjectStateList[i].f14 -= fTime;
                } else {
                    fPeriod =
                        4.0f * (gTerRenderer.pObjectStateList[i].f0 - gTerRenderer.fTreeMinPeriod) + 1.5f;
                    if (gTerRenderer.pObjectStateList[i].n18 == 1) {
                        gTerRenderer.pObjectStateList[i].f4 =
                            0.5f * Math_Sin(10.0f
                                               * (6.2831855f
                                                  * Ter_GetTimeInCycle(gTerRenderer.pObjectStateList[i].nC,
                                                                fPeriod / 10.0f)
                                                  / fPeriod))
                            + 0.5f;
                    } else {
                        gTerRenderer.pObjectStateList[i].f4 =
                            0.5f * Math_Sin(0.5f
                                               * (6.2831855f
                                                  * Ter_GetTimeInCycle(gTerRenderer.pObjectStateList[i].nC,
                                                                fPeriod / 0.5f)
                                                  / fPeriod))
                            + 0.5f;
                    }
                    gTerRenderer.pObjectStateList[i].f14 -= fTime;
                }
            } else {
                for (k = 0; k < sizeof(gTerCrowdPoseStepsFlag40) / sizeof(gTerCrowdPoseStepsFlag40[0]); k++) {
                    if (gTerRenderer.pObjectStateList[i].n18 == gTerCrowdPoseStepsFlag40[k].n0
                        && gTerRenderer.pObjectStateList[i].n1C == gTerCrowdPoseStepsFlag40[k].n4) {
                        fStep = gTerCrowdPoseStepsFlag40[k].fC - gTerRenderer.pObjectStateList[i].f4;
                        if (gTerRenderer.pObjectStateList[i].n18 == 2
                            || 3 == gTerRenderer.pObjectStateList[i].n18) {
                            if (fStep > 6.0f * fTime) {
                                fStep = 6.0f * fTime;
                            }
                            if (fStep < -6.0f * fTime) {
                                fStep = -6.0f * fTime;
                            }
                        } else {
                            if (fStep > 4.0f * fTime) {
                                fStep = 4.0f * fTime;
                            }
                            if (fStep < -4.0f * fTime) {
                                fStep = -4.0f * fTime;
                            }
                        }
                        gTerRenderer.pObjectStateList[i].f4 += fStep;
                        if (fabsf(gTerRenderer.pObjectStateList[i].f4 - gTerCrowdPoseStepsFlag40[k].fC)
                            < 0.01f) {
                            gTerRenderer.pObjectStateList[i].n18 = gTerCrowdPoseStepsFlag40[k].n8;
                            gTerRenderer.pObjectStateList[i].f4 = gTerCrowdPoseStepsFlag40[k].f10;
                            gTerRenderer.pObjectStateList[i].nC = 0;
                            break;
                        }
                    }
                }
            }
        } else if (uFlags3 & 1) {
            if (gTerRenderer.pObjectStateList[i].n1C == 1) {
                gTerRenderer.pObjectStateList[i].f4 += fTime;
                if (gTerRenderer.pObjectStateList[i].f4 > 1.0f) {
                    gTerRenderer.pObjectStateList[i].f4 = 1.0f;
                }
            } else {
                gTerRenderer.pObjectStateList[i].f4 = 0.0f;
            }
        } else {
            fSum = 1.0f + gTerRenderer.fTreeNoiseAmplitudeScale;
            fScale = 1.0f / fSum;
            fNoise = gTerRenderer.fTreeNoiseAmplitudeScale;
            gTerRenderer.pObjectStateList[i].f4 =
                0.5f * fScale
                    * Math_Sin(6.2831855f
                                  * Ter_GetTimeInCycle(gSession.nFrameCount,
                                                       gTerRenderer.pObjectStateList[i].f0)
                                  / gTerRenderer.pObjectStateList[i].f0)
                + 0.5f * (fScale * fNoise)
                      * Math_Sin(6.2831855f
                                    * Ter_GetTimeInCycle(gSession.nFrameCount,
                                                  gTerRenderer.fTreeNoisePeriodScale
                                                      * gTerRenderer.pObjectStateList[i].f0)
                                    / (gTerRenderer.fTreeNoisePeriodScale
                                       * gTerRenderer.pObjectStateList[i].f0));
            gTerRenderer.pObjectStateList[i].f4 *= gTerRenderer.fTreeOverdrive;
            gTerRenderer.pObjectStateList[i].f4 += 0.5f;
        }
    }
}

// Draws object list nList of the hole data (0 before the patches, 2 after them) when its switch is
// on, as panorama items: each object that is not off screen, except (list 0) objects 0-1 or 2-3 by
// lbl_802811F0's flag 0x2, (list 2) the last gTerPanoramaList2HiddenCount, and in split screen
// those with flag 8.
void Ter_DrawPanoramaList(void* pHoleData, u32 nList) {
    UObjMesh* pRoot;
    UObjMesh* pList;
    UObjMesh* pMesh;
    View* pView;
    s32 nItems;
    s32 nCount;
    s32 uFlags;
    s32 nClip;
    int i;

    if ((nList != 0 || gTerDrawPanoramaList0) && (nList != 2 || gTerDrawPanoramaList2)) {
        pRoot = Ter_GetHoleModelRoot(pHoleData);
        nItems = 0;
        if (Ter_GetMeshChildCount(pRoot) >= (s32)(nList + 1)) {
            pList = Ter_GetMeshChild(pRoot, nList);
            nCount = Ter_GetMeshChildCount(pList);
            pMesh = Ter_GetMeshChild(pList, 0);
            for (i = 0; i < nCount; i++) {
                if (nList == 0) {
                    if (Weather_IsRaining()) {
                        if (i >= 0 && i <= 1) {
                            pMesh = Ter_GetMeshNext(pMesh);
                            continue;
                        }
                    } else if (i >= 2 && i <= 3) {
                        pMesh = Ter_GetMeshNext(pMesh);
                        continue;
                    }
                }
                if (nList == 2 && i >= nCount - gTerPanoramaList2HiddenCount) {
                    pMesh = Ter_GetMeshNext(pMesh);
                    continue;
                }
                uFlags = Ter_GetMeshFlags(pMesh, 2);
                if (!gSession.nSplitScreen || !(uFlags & 8)) {
                    pView = ViewController_GetCameraControl(gTerRenderer.iCurrentViewContext);
                    nClip = fn_80007B2C(pMesh, RC_spGetCurrentRenderCtx(), 0.0f,
                                        gTerRenderer.fCameraMinHalfFieldOfViewTan,
                                        pView->f54);
                    if (nClip != 3) {
                        // fake match: uFlags is reused for bUseFog (fog unless flag 0x20); a new local
                        // is computed after the call to Ter_GetMeshMipmapBiasScale, the original before it
                        uFlags = ((uFlags & 0x20) >> 5) ^ 1;
                        Ter_AddObjectDraw(&gTerRenderer.pPanoramaItemsList[nItems], &nItems, 300, pMesh, 1.0f,
                                    gTerRenderer.fDefaultObjectMipmapBias[0]
                                            * Ter_GetMeshMipmapBiasScale(pMesh), 0.0f,
                                    0x289 - i, nClip, uFlags, 0);
                    }
                }
                pMesh = Ter_GetMeshNext(pMesh);
            }
        }
        Ter_DrawObjectList(gTerRenderer.pPanoramaItemsList, nItems, gTerRenderer.eTerrainFilterMin,
                    gTerRenderer.eTerrainFilterMag);
        Ter_SetZWrite(1);
        RenderState_Flush();
    }
}

// The 'tLOD' chunk arrived: two values, made whole numbers and, when the second is the larger and
// both are at least gTerLOD1OverlapSteps x gTerLODStepSize, turned into gTerLOD0Steps and gTerLOD1Steps.
void Ter_LODLoadCallback(UStreamObject* pObject) {
    TerLODData* pData = (TerLODData*)pObject->pData;
    s32 nRem;

    gTerLODDataNear = pData->fC;
    gTerLODDataFar = pData->f10;
    StaticMem_Free(pObject);
    // A value that is not a multiple of gTerLODStepSize gets its remainder added (a multiple only
    // when the remainder was half of it); EA's rounding as it is.
    nRem = gTerLODDataNear % (s32)gTerLODStepSize;
    if (nRem != 0) {
        gTerLODDataNear += nRem;
    }
    nRem = gTerLODDataFar % (s32)gTerLODStepSize;
    if (nRem != 0) {
        gTerLODDataFar += nRem;
    }
    if (gTerLODDataFar > gTerLODDataNear) {
        f32 fLimit = gTerLOD1OverlapSteps * gTerLODStepSize;

        if (gTerLODDataNear < fLimit || gTerLODDataFar < fLimit) {
            return;
        }
        Ter_LODStepsFromDistances(&gTerLOD0Steps, &gTerLOD1Steps, gTerLODDataNear, gTerLODDataFar);
    }
}

// The 'ter ' chunk arrived: keeps it (pCurrentHoleDataStreamData) and its hole data
// (pCurrentHoleData, from fn_800073B4) for drawing.
void Ter_HoleDataLoadCallback(UStreamObject* pObject) {
    gTerRenderer.pCurrentHoleDataStreamData = pObject;
    gTerRenderer.pCurrentHoleData = fn_800073B4(pObject->pData, 0);
}

// The 'tgd ' (course) chunk arrived: clears the pin and tee positions (Ter_TeeLoadCallback and
// Ter_PinLoadCallback fill them), readies the collision data, fills light sets 2, 0 and 1 from the
// hole's lights and makes set 3 current, and hands the glows the course's values (p3C, or
// defaults). The light vector gSession.f5B3C..f5B48 is the hole's own (v60) when it has one, else
// the glows' vector; it is raised to at least its distance across the ground and made at least 2000
// long.
void Ter_CourseLoadCallback(UStreamObject* pObject) {
    f32 v18[4];
    f32 v8[4];
    int i;
    u32 nGlow;
    CourseGlowBlock* pGlow;
    f32 fLength;

    gTerRenderer.pCourseStreamData = pObject;
    gTerRenderer.pCourse = (CourseInfo*)pObject->pData;
    for (i = 0; i < 4; i++) {
        gTerRenderer.pCourse->tee[i].x = 0.0f;
        gTerRenderer.pCourse->pin[i].x = 0.0f;
        gTerRenderer.pCourse->tee[i].y = 0.0f;
        gTerRenderer.pCourse->pin[i].y = 0.0f;
        gTerRenderer.pCourse->tee[i].z = 0.0f;
        gTerRenderer.pCourse->pin[i].z = 0.0f;
        gTerRenderer.pCourse->tee[i].w = 0.0f;
        gTerRenderer.pCourse->pin[i].w = 0.0f;
    }
    Ter_InitTGD(gTerRenderer.pCourse);
    LF_vSetCurrentLightFogEnvironment(2);
    fn_800935CC(&gTerRenderer.pCourse->lights);
    fn_80093900(gTerRenderer.pCourse->p38);
    LF_vSetCurrentLightFogEnvironment(0);
    fn_800935CC(&gTerRenderer.pCourse->lights);
    fn_80093900(gTerRenderer.pCourse->p38);
    LF_vSetCurrentLightFogEnvironment(1);
    fn_800935CC(&gTerRenderer.pCourse->lights);
    fn_80093900(gTerRenderer.pCourse->p38);
    LF_vSetCurrentLightFogEnvironment(3);
    LF_SetCurrentDefaultLights();
    LF_ResetCurrentFogSettings();
    pGlow = gTerRenderer.pCourse->p3C;
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
    SF_vSetSunColor(v18);
    SF_vSetSunPosition(v8);
    SF_vSetFlareType(nGlow);
    if (gTerRenderer.pCourse->v60[0] || gTerRenderer.pCourse->v60[1] || gTerRenderer.pCourse->v60[2]) {
        gSession.f5B3C = gTerRenderer.pCourse->v60[0];
        gSession.f5B40 = gTerRenderer.pCourse->v60[1];
        gSession.f5B44 = gTerRenderer.pCourse->v60[2];
        gSession.f5B48 = 1.0f;
    } else {
        gSession.f5B3C = v8[0];
        gSession.f5B40 = v8[1];
        gSession.f5B44 = v8[2];
        gSession.f5B48 = v8[3];
    }
    fLength = Math_Sqrt(gSession.f5B3C * gSession.f5B3C + gSession.f5B44 * gSession.f5B44);
    if (gSession.f5B40 < fLength) {
        gSession.f5B40 = fLength;
    }
    // port: f5B3C..f5B48 are read as one vector
    fLength = Math_Sqrt(Vec3_LengthSqClamped(&gSession.f5B3C));
    if (fLength < 2000.0f && fLength > 0.0f) {
        Vec3_Scale(2000.0f / fLength, &gSession.f5B3C, &gSession.f5B3C);
    }
    i = Game_GetCourse();
    if (i >= 21) {
        i = 0;
    }
    Ter_SetCourseMipmapBias(i);
}

// Sets the three levels of detail's object mipmap biases (fDefaultObjectMipmapBias) from course n's
// row of gTerCourseMipmapBias.
void Ter_SetCourseMipmapBias(int n) {
    gTerRenderer.fDefaultObjectMipmapBias[0] = gTerCourseMipmapBias[n][0];
    gTerRenderer.fDefaultObjectMipmapBias[1] = gTerCourseMipmapBias[n][1];
    gTerRenderer.fDefaultObjectMipmapBias[2] = gTerCourseMipmapBias[n][2];
}

// Unloads the hole's terrain (the hole loader, Code8006F438.c): stops the crowd, frees the 'ter '
// and 'tgd ' chunks, and puts the LOD settings back to their defaults (no 'tLOD' chunk, steps 26
// and 16).
void Ter_UnloadHole(void) {
    Ter_StopCrowdAnimation(1);
    if (gTerRenderer.pCurrentHoleData != NULL) {
        fn_800075CC(gTerRenderer.pCurrentHoleData);
        gTerRenderer.pCurrentHoleData = NULL;
        StaticMem_Free(gTerRenderer.pCurrentHoleDataStreamData);
    }
    if (gTerRenderer.pCourse != NULL) {
        StaticMem_Free(gTerRenderer.pCourseStreamData);
        gTerRenderer.pCourse = NULL;
    }
    gTerRenderer.iLowLODListOffset = -1;
    gTerLODDataNear = -1;
    gTerLODDataFar = -1;
    gTerLOD0Steps = 26;
    gTerLOD1Steps = 16;
}

// A tee's position arrived: into its row of the course's tees, if the course is loaded.
void Ter_TeeLoadCallback(UStreamObject* pObject) {
    TerPosData* pTee = (TerPosData*)pObject->pData;

    if (gTerRenderer.pCourse != NULL) {
        gTerRenderer.pCourse->tee[pTee->nIndex].x = pTee->vPos[0];
        gTerRenderer.pCourse->tee[pTee->nIndex].y = pTee->vPos[1];
        gTerRenderer.pCourse->tee[pTee->nIndex].z = pTee->vPos[2];
        gTerRenderer.pCourse->tee[pTee->nIndex].w = 1.0f;
    }
    StaticMem_Free(pObject);
}

// A pin position arrived (UKernel.c hands it on). With GM_Currently_SkillZoneMode set it goes to
// GameModeSkillZoneBase_AddCup; otherwise a pin the course already has (w not 0) is copied into the
// chunk, and a missing one is taken from it.
u8 Ter_PinLoadCallback(UStreamObject* pObject) {
    TerPosData* pPin = (TerPosData*)pObject->pData;

    if (gTerRenderer.pCourse != NULL) {
        if (GM_Currently_SkillZoneMode()) {
            GameModeSkillZoneBase_AddCup(pPin->vPos[0], pPin->vPos[1], pPin->vPos[2]);
        } else if (0.0f != gTerRenderer.pCourse->pin[pPin->nIndex].w) {
            pPin->vPos[0] = gTerRenderer.pCourse->pin[pPin->nIndex].x;
            pPin->vPos[1] = gTerRenderer.pCourse->pin[pPin->nIndex].y;
            pPin->vPos[2] = gTerRenderer.pCourse->pin[pPin->nIndex].z;
        } else {
            gTerRenderer.pCourse->pin[pPin->nIndex].x = pPin->vPos[0];
            gTerRenderer.pCourse->pin[pPin->nIndex].y = pPin->vPos[1];
            gTerRenderer.pCourse->pin[pPin->nIndex].z = pPin->vPos[2];
            gTerRenderer.pCourse->pin[pPin->nIndex].w = 1.0f;
        }
    }
    StaticMem_Free(pObject);
    return 0;
}

// The flag follows the wind: it turns to face it and plays "flagcalm" below 5, "flagbrzy" below
// 13, else "flagwind".
void Ter_UpdateFlagForWind(void) {
    f32 vWind[3];
    f32 fSpeed = Wind_GetPhysicsWindVelocity(vWind);
    Character* pFlag = SkeletalObject_FindObject(100);

    if (pFlag != NULL) {
        const char* aClips[3] = {"flagcalm", "flagbrzy", "flagwind"};
        int n;

        Character_SetOrientationVec(pFlag, vWind, 0.0f);
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
        Character_PlayClip(pFlag, Char_SetClip(pFlag, 3, 0, aClips[n]), 1, 0.0f);
    }
}

// Draws the loaded hole's terrain in view n (gomainloop, once per view); nothing before a 'ter '
// chunk has arrived.
void Ter_DrawHoleView(int n) {
    if (gTerRenderer.pCurrentHoleData != NULL) {
        Ter_BeginRender();
        Ter_RenderView(gTerRenderer.pCurrentHoleData, n);
        Ter_EndRender();
    }
}

// The model of object list nObjList of patch nPatch: node 1 of the hole data's tree holds one node
// per patch, and a patch's node 1 holds, in its node 0, its object lists. NULL when out of range.
UObjMesh* Ter_GetObjectListModel(u16 nPatch, u16 nObjList) {
    UObjMesh* pModel = NULL;
    UObjMesh* pNode;

    pNode = Ter_GetMeshChild(Ter_GetHoleModelRoot(gTerRenderer.pCurrentHoleData), 1);
    if (nPatch < Ter_GetMeshChildCount(pNode)) {
        pNode = Ter_GetMeshChild(pNode, nPatch);
        if (Ter_GetMeshChildCount(pNode) >= 2) {
            pNode = Ter_GetMeshChild(Ter_GetMeshChild(pNode, 1), 0);
            if (nObjList < Ter_GetMeshChildCount(pNode)) {
                pModel = Ter_GetMeshChild(pNode, nObjList);
            }
        }
    }
    return pModel;
}

// Draws the grass patches (GoGrass.c calls it): sets the renderer up, takes the camera's position
// and look direction, the flat distance to the nearest ball and the smaller half field of view's
// tangent (view 0's), then builds the grass list (Ter_BuildGrassPatchList) and draws it
// (Ter_DrawGrassPatches, Ter_DrawGrassPatchesPass3).
void Ter_RenderGrass(void) {
    int i;
    void* pHoleData = gTerRenderer.pCurrentHoleData;
    CamLens* pLens = Camera_GetCurrentLens();
    f32 vDiff[4];
    f32 fDist;
    f32 fTan;
    f32 fWideTan;

    RC_vSetCurrentRenderCtxTransformationMatrix(NULL);
    RenderState_SetCameraMatrices();
    RenderState_SetCameraMatrices();
    RenderState_SetCameraMatrices();
    RenderState_SetBlendFactors(4, 5);
    DS_vSetAlphaTestMode(1, 6, 1);
    RenderState_SetDrawFlags(0x70);
    RenderState_Flush();
    gTerRenderer.xCameraReferencePos[0] = pLens->m4[3][0];
    gTerRenderer.xCameraReferencePos[1] = pLens->m4[3][1];
    gTerRenderer.xCameraReferencePos[2] = pLens->m4[3][2];
    gTerRenderer.xCameraReferencePos[3] = 1.0f;
    gTerRenderer.xCameraLookVector[0] = pLens->m4[2][0];
    gTerRenderer.xCameraLookVector[1] = pLens->m4[2][1];
    gTerRenderer.xCameraLookVector[2] = pLens->m4[2][2];
    gTerRenderer.xCameraLookVector[3] = 1.0f;
    gTerRenderer.fXZDistanceToClosestBallSquared = 1000000.0f;
    for (i = 0; i < gNumPlayersSetUp; i++) {
        LLMath_Subtract(gPlayers[i].ball.vPos, gTerRenderer.xCameraReferencePos, vDiff);
        vDiff[1] = 0.0f;
        fDist = Vec3_LengthSqClamped(vDiff);
        if (fDist < gTerRenderer.fXZDistanceToClosestBallSquared) {
            gTerRenderer.fXZDistanceToClosestBallSquared = fDist;
        }
    }
    fTan = Math_Tan(0.5f * pLens->fFov);
    fWideTan = Math_Tan(0.5f
                        * (0.75f * pLens->fFov
                           * VM_fGetViewportHeightOverWidth((u8*)RC_spGetCurrentRenderCtxViewport())));
    gTerRenderer.fCameraMinHalfFieldOfViewTan =
        (fTan <= fWideTan / ViewController_GetCameraControl(0)->f54) ? fTan : fWideTan
                / ViewController_GetCameraControl(0)->f54;
    RenderState_Flush();
    Ter_BuildGrassPatchList(pHoleData);
    Ter_DrawGrassPatches(0);
    Ter_DrawGrassPatchesPass3();
}

// Draws the grass list's patches of render pass nRenderPass that take part in the first pass (bit 0
// of n1C), from the end of the list, one clip method at a time (clip mode 0 for method 0, 1 for 1
// and 2); then alpha test back on and z writes on. Ter_RenderGrass calls it with 0.
// fake match: the one-pass loop over the passes (as in Ter_DrawPostDrawPatches) keeps the pass bit
// in a register for the `and.` test of bit 0.
void Ter_DrawGrassPatches(int nRenderPass) {
    u8 bFirst = 1;
    int i;
    Ter_PatchReference* pPatch;
    int nPass;
    int nPassBit;
    s32 nClip;

    RenderState_Flush();
    RenderState_SetDrawFlags(0x70);
    for (nPass = 0, nPassBit = 1; nPass < 1; nPass++, nPassBit <<= 1) {
        for (nClip = 0; nClip <= 2; nClip++) {
            switch (nClip) {
            case 2:
                RenderState_SetClipMode(1);
                break;
            case 1:
                RenderState_SetClipMode(1);
                break;
            default:
                RenderState_SetClipMode(0);
                break;
            }
            RenderState_Flush();
            for (i = gTerRenderer.iNumGrassPatches - 1; i >= 0; i--) {
                pPatch = &gTerRenderer.xpGrassPatchList[i];
                if (pPatch->eClipMethod == nClip && pPatch->iRenderPass == nRenderPass
                    && (pPatch->n1C & nPassBit)) {
                    Ter_DrawPatchGround(pPatch->pGround, nClip, nPass, pPatch->n1C, pPatch->n18, pPatch->n20,
                                        &bFirst,
                                0, 1, pPatch->fDistance, pPatch->fDistance + 2.0f * pPatch->fBoundingRadius);
                }
            }
        }
    }
    DS_vSetAlphaTestMode(1, 6, 1);
    RenderState_Flush();
    Ter_SetZWrite(1);
    RenderState_Flush();
}

// Draws the grass list's patches in list 0x80 of n1C as pass 3 with z writes off, from the end of
// the list, one clip method at a time; then alpha test and z writes back on. Ter_RenderGrass calls
// it after Ter_DrawGrassPatches.
void Ter_DrawGrassPatchesPass3(void) {
    u8 bFirst = 1;
    int i;
    s32 nClip;
    Ter_PatchReference* pPatch;

    Ter_SetZWrite(0);
    RenderState_Flush();
    RenderState_SetDrawFlags(0x70);
    for (nClip = 0; nClip <= 2; nClip++) {
        switch (nClip) {
        case 2:
            RenderState_SetClipMode(1);
            break;
        case 1:
            RenderState_SetClipMode(1);
            break;
        default:
            RenderState_SetClipMode(0);
            break;
        }
        RenderState_Flush();
        for (i = gTerRenderer.iNumGrassPatches - 1; i >= 0; i--) {
            pPatch = &gTerRenderer.xpGrassPatchList[i];
            if (pPatch->eClipMethod == nClip && (pPatch->n1C & 0x80)) {
                Ter_DrawPatchGround(pPatch->pGround, nClip, 3, pPatch->n1C, pPatch->n18, pPatch->n20,
                                    &bFirst, 0,
                            1, pPatch->fDistance, pPatch->fDistance + 2.0f * pPatch->fBoundingRadius);
            }
        }
    }
    DS_vSetAlphaTestMode(1, 6, 1);
    RenderState_Flush();
    Ter_SetZWrite(1);
    RenderState_Flush();
}

// Builds the grass list: every patch of this frame in list 0x80 of n1C, or whose ground's drawn
// mesh (Ter_GetGroundDrawMesh) has flag 8 in flag byte 3, and whose drawn mesh is not off screen
// (clip method 3), is copied to xpGrassPatchList with that clip method and its distance from the
// camera less its radius (at least 0).
void Ter_BuildGrassPatchList(void* pUnused) {
    Ter_PatchReference* pPatch;
    void* pCamera;
    Ter_PatchReference* pGrass;
    int i;
    View* pView;
    f32 fRadius;
    f32 fDist;
    s32 nClip;

    // pUnused: the one caller, Ter_RenderGrass, passes the hole data, which this function does not read
    pCamera = RC_spGetCurrentRenderCtx();
    gTerRenderer.iNumGrassPatches = 0;
    pGrass = gTerRenderer.xpGrassPatchList;
    for (i = 0; i < gTerRenderer.iTotalPatches; i++) {
        pPatch = &gTerRenderer.pPatchList[i];
        if ((pPatch->n1C & 0x80) || (Ter_GetMeshFlags(Ter_GetGroundDrawMesh(pPatch->pGround), 3) & 8)) {
            fRadius = Ter_GetMeshBoundingSphere(Ter_GetGroundDrawMesh(pPatch->pGround))[3];
            fDist = LLMath_DistanceBetween3(gTerRenderer.xCameraReferencePos,
                                             Ter_GetMeshBoundingSphere(
                                                     Ter_GetGroundDrawMesh(pPatch->pGround)))
                    - fRadius;
            if (fDist < 0.0f) {
                fDist = 0.0f;
            }
            pView = ViewController_GetCameraControl(gTerRenderer.iCurrentViewContext);
            nClip = fn_80007B2C(Ter_GetGroundDrawMesh(pPatch->pGround), pCamera, fDist,
                                gTerRenderer.fCameraMinHalfFieldOfViewTan, pView->f54);
            if (nClip != 3) {
                Mem_cpy(pGrass, pPatch, sizeof(Ter_PatchReference));
                pGrass->eClipMethod = nClip;
                pGrass->fDistance = fDist;
                pGrass++;
                gTerRenderer.iNumGrassPatches++;
            }
        }
    }
}

// Sets boManageZUpdate and returns what it was.
u8 Ter_SetManageZUpdate(u8 b) {
    u8 bOld = gTerRenderer.boManageZUpdate;

    gTerRenderer.boManageZUpdate = b;
    return bOld;
}

f32 Math_Floor(f32 x) {
    return floor(x);
}

// ---- the renderer's state ----

// Turns the constant alpha on (1) or off (0), applied with the next RenderState_Apply: on, the TEV
// stages take the draw's alpha from RenderState_SetConstantAlpha's value (times the texture's)
// instead of the vertex colour's.
void RenderState_SetConstantAlphaOn(u8 b) {
    gRenderState.bConstantAlpha = b;
    gRenderState.uChanged |= 0x80;
}

// Sets the fog's end distance, applied with the next RenderState_Apply.
void RenderState_SetFogEnd(f32 f) {
    gRenderState.fFogEnd = f;
    gRenderState.uChanged |= 0x8;
}

// Sets the fog's start distance, applied with the next RenderState_Apply.
void RenderState_SetFogStart(f32 f) {
    gRenderState.fFogStart = f;
    gRenderState.uChanged |= 0x8;
}

// Sets the fog colour (0..255 each; its alpha is always 0x80), applied with the next
// RenderState_Apply.
void RenderState_SetFogColour(u8 r, u8 g, u8 b) {
    gRenderState.c30.r = r;
    gRenderState.c30.g = g;
    gRenderState.c30.b = b;
    gRenderState.c30.a = 0x80;
    gRenderState.uChanged |= 0x8;
}

// Sets the blend source and destination factors, applied with the next RenderState_Apply.
void RenderState_SetBlendFactors(int a, int b) {
    gRenderState.nBlendSrc = a;
    gRenderState.nBlendDst = b;
    gRenderState.uChanged |= 0x10;
}

// Sets the GX clip mode, applied with the next RenderState_Apply.
void RenderState_SetClipMode(int a) {
    gRenderState.nClipMode = a;
    gRenderState.uChanged |= 0x400;
}

// The constant alpha, 0..255, used while RenderState_SetConstantAlphaOn is on.
void RenderState_SetConstantAlpha(u8 b) {
    gRenderState.nConstantAlpha = b;
    gRenderState.uChanged |= 0x80;
}

// Clears the draw flags uClear, then sets uSet (uDrawFlags: 0x10 textured, 0x20 fog, 0x40 blended),
// applied with the next RenderState_Apply.
void RenderState_ChangeDrawFlags(u32 uClear, u32 uSet) {
    gRenderState.uDrawFlags &= ~uClear;
    gRenderState.uDrawFlags |= uSet;
    gRenderState.uChanged |= 0x20;
}

// Hands pData to shader type nRow's SetParameters hook (ModuleHooks.pfn8 of lbl_80188E88; row 17 is
// the grass).
void SD_SetShaderTypeParameters(int nRow, void* pData) {
    lbl_80188E88[nRow].pfn8(pData);
}

// Where n frames falls in a cycle of fPeriod seconds, in seconds.
f32 Ter_GetTimeInCycle(u32 n, f32 fPeriod) {
    return FRAME_TIME * (f32)(n % (u32)(FRAME_RATE * (0.5f / FRAME_RATE + fPeriod)));
}

void RC_vUpdateRenderCtxScreen();
void LF_ApplyFogToRenderState(void);
void LF_UpdateFogColourForCamera();
void LF_SetFogSettings(TerSettings* pSettings);

// Gives the current render camera the model matrix pMtx (NULL: the identity).
void RC_vSetCurrentRenderCtxTransformationMatrix(f32 (*pMtx)[4]) {
    RC_vSetRenderCtxTransformationMatrix(*gppCurrentRenderCtx, pMtx);
}

// The current render context's viewport, its screen rectangle as fractions of the frame buffer
// (RC_spGetRenderCtxViewport).
f32* RC_spGetCurrentRenderCtxViewport(void) {
    return RC_spGetRenderCtxViewport(*(void**)gppCurrentRenderCtx);
}

// Works out the current render context's transformation matrices again
// (RC_vUpdateRenderCtxTransformationMatrices).
void RC_vUpdateCurrentRenderCtxTransformationMatrices(void) {
    RC_vUpdateRenderCtxTransformationMatrices(*(void**)gppCurrentRenderCtx);
}

// Works out the current render camera's screen values and projection again
// (RC_vUpdateRenderCtxScreenMatricesAndInfo).
void RC_UpdateCurrentScreenMatrices(void) {
    RC_vUpdateRenderCtxScreen(*(s32*)((u8*)gppCurrentRenderCtx));
}

// Updates the fog for this frame: blends the fog colour for the camera's heading
// (LF_UpdateFogColourForCamera), then hands the fog colour and distances to the renderer
// (LF_ApplyFogToRenderState).
void LF_UpdateFog(void) {
    LF_UpdateFogColourForCamera();
    LF_ApplyFogToRenderState();
}

// Takes the current light set's fog settings (colours round the compass, fog distance) as the ones
// the fog is made from (LF_SetFogSettings).
void LF_UseCurrentFogSettings(void) {
    LF_SetFogSettings(&LF_spGetCurrentLightFogEnvironment()->settings);
}

LightSet* LF_spGetCurrentLightFogEnvironment(void) {
    return lbl_80281380->pCur;
}

// Makes light set nSet of lbl_80281380 the current one.
void LF_vSetCurrentLightFogEnvironment(s32 nSet) {
    lbl_80281380->pCur = &lbl_80281380->aSet[nSet];
}

// Resets the current light set's fog settings to the defaults (fn_8006F334: one grey-blue all
// round, no turn).
void LF_ResetCurrentFogSettings(void) {
    fn_8006F334(&LF_spGetCurrentLightFogEnvironment()->settings);
}

// Gives the current light set's light group the default lights (fn_8006EDC0).
// Ter_CourseLoadCallback calls it for set 3.
void LF_SetCurrentDefaultLights(void) {
    fn_8006EDC0(&LF_spGetCurrentLightFogEnvironment()->group);
}

// Hands the renderer the fog made from the fog settings (lbl_802811E0): the colour f44..f4C, a
// start distance of 0.375 x f50 and an end of 4.15 x (f50 + 10).
void LF_ApplyFogToRenderState(void) {
    RenderState_SetFogColour(lbl_802811E0->f44, lbl_802811E0->f48, lbl_802811E0->f4C);
    RenderState_SetFogStart(0.375f * lbl_802811E0->f50);
    RenderState_SetFogEnd(4.15f * (10.0f + lbl_802811E0->f50));
}

void fn_8006F154();
void CharacterRender_SetCurrentBuffer(s32 iBuffer);
void CharacterRender_StartNewFrame(void);

// Blends the fog colour for the current camera's heading (fn_8006F154). The current render context
// it fetches first is not used.
void LF_UpdateFogColourForCamera(void) {
    RC_spGetCurrentRenderCtx();
    fn_8006F154();
}

// Takes a copy of the fog settings (colours round the compass, turn, fog distance) that the fog
// colour and distances are made from (lbl_802811E0).
void LF_SetFogSettings(TerSettings* pSettings) {
    Mem_cpy(lbl_802811E0, pSettings, sizeof(TerSettings));
}

// a - b into out, three floats; the same helper as Ball.c's Ball_Vec3Sub.
#ifdef __MWERKS__
asm void LLMath_Subtract3(register f32* pA, register f32* pB, register f32* pOut) {
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
void LLMath_Subtract3(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// a - b into out, four floats.
#ifdef __MWERKS__
asm void LLMath_Subtract(register f32* pA, register f32* pB, register f32* pOut) {
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
void LLMath_Subtract(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
    pOut[3] = pA[3] - pB[3];
}
#endif

// Sets the lens's far clip distance (fAC, which CA_fGetCameraFarZ reads). Ter_DrawFarClipPatches and
// GoGreenGrid.c push it out for a draw and put it back.
void Camera_SetLensFarClip(u8* p, f32 v) {
    *(f32*)(p + 0xAC) = v;
}

// The next mesh after pNode among its parent's children (p14).
UObjMesh* Ter_GetMeshNext(UObjMesh* pNode) {
    return pNode->p14;
}

// The node's bounding sphere: centre (v58), then radius (f64).
f32* Ter_GetMeshBoundingSphere(UObjMesh* pNode) {
    return pNode->pInfo->v58;
}

// A flag byte of the node (a24); this file reads bytes 0-3.
s32 Ter_GetMeshFlags(UObjMesh* pNode, s32 n) {
    return pNode->pInfo->aFlags[n];
}

UObjMesh* Ter_GetMeshChild(UObjMesh* pNode, s32 n) {
    return pNode->p8[n];
}

// How many nodes the node holds.
s32 Ter_GetMeshChildCount(UObjMesh* pNode) {
    return pNode->pInfo->n0;
}

// The root of the hole data's model tree (the pointer at offset 0xEC of the hole data, whose layout
// is not described): its p8[1] holds a mesh per patch.
UObjMesh* Ter_GetHoleModelRoot(u8* pHoleData) {
    return *(UObjMesh**)(pHoleData + 0xEC);
}

// A terrain object's bounds (pInfo->a68): its centre [0..2], a radius [3] and a height [7].
f32* Ter_GetMeshBounds(UObjMesh* pMesh) {
    return pMesh->pInfo->a68;
}

// Draws the mesh's current part: if part n28 is switched on (byte 0x1C + n28), hands its 0x2C-byte
// UObjMeshPart (p18[n28]) to LLObj_Gc.c's fn_800082CC, as Object_DrawMesh does. Raw offsets: the
// mesh comes in as bytes.
void Ter_DrawMeshCurrentPart(u8* pObject) {
    s32 n = *(s32*)(pObject + 0x28);

    if (pObject[n + 0x1C] != 0) {
        fn_800082CC((UObjMeshPart*)(*(u8**)(pObject + 0x18) + n * 0x2C));
    }
}

// A terrain object's draw flag byte (pInfo->b8B): bit 1, drawn without z writes
// (Ter_SetObjectRenderState).
s32 Ter_GetMeshDrawFlags(UObjMesh* pMesh) {
    return pMesh->pInfo->b8B;
}

// What a terrain object's mipmap bias is scaled by (pInfo->f54).
f32 Ter_GetMeshMipmapBiasScale(UObjMesh* pMesh) {
    return pMesh->pInfo->f54;
}

// The mesh drawn for a patch's ground (pC): Ter_BuildGrassPatchList tests its flags and bounds for
// the grass list.
UObjMesh* Ter_GetGroundDrawMesh(UObjMesh* pGround) {
    return pGround->pC;
}

// Whether the hole's weather has flag 0x2 (lbl_802811F0, rolled per course by fn_8006F650):
// GameAudio.c plays the rain sound on it, Replay.c records it as weather 3, the caddie's weather
// tip fires, and Ter_DrawPanoramaList leaves out panorama list 0's first two meshes.
u8 Weather_IsRaining(void) {
    return lbl_802811F0->uFlags & 2;
}

// Sets the sun flare's type (lbl_802813B8->n1930; SF_vInitModule sets 1, a course's glow data
// 0..3).
void SF_vSetFlareType(s32 v) {
    lbl_802813B8->n1930 = v;
}

// Sets the sun's position for the flare (lbl_802813B8->v4; SF_vInitModule sets (0, 150, -400)).
void SF_vSetSunPosition(f32* p0) {
    LLMath_CopyVec(p0, lbl_802813B8->v4);
}

// Sets the sun's colour for the flare (lbl_802813B8->v14; SF_vInitModule sets 0.8, 0.8, 0.4).
void SF_vSetSunColor(f32* p0) {
    LLMath_CopyVec(p0, lbl_802813B8->v14);
}

// Records which buffer this frame draws to for the characters (gCharRendCurrentBuffer):
// gomainloop.c passes the video field's parity (lbl_80281B88 & 1) each frame outside start-up; when
// the value equals that parity, which with that one caller is always, the stored value flips
// instead. Nothing reads it in this build.
void CharacterRender_SetCurrentBuffer(s32 iBuffer) {
    s32 iNew;

    iNew = iBuffer;
    if (iNew == (s32)(lbl_80281B88 & 1)) {
        iNew = gCharRendCurrentBuffer ^ 1;
    }
    gCharRendCurrentBuffer = iNew;
}

// Starts a frame of character drawing: empty in this build (and in TW07). Character_PreRenderAll
// and FEgolferanim.c call it first.
void CharacterRender_StartNewFrame(void) {
}

// Sets the renderer up for drawing the characters: the identity model matrix, the render context's
// matrices and the camera's, blend source alpha over inverse source alpha. Character_RenderAll and
// shadow.c call it first. EA's takes a flags word (TW07: uint32 flags); shadow.c passes 2, which
// this build ignores.
void CharacterRender_RenderSetup(void) {
    RC_vSetCurrentRenderCtxTransformationMatrix(NULL);
    RC_vUpdateCurrentRenderCtxTransformationMatrices();
    RenderState_SetCameraMatrices();
    RenderState_SetBlendFactors(4, 5);
    RenderState_Flush();
}
