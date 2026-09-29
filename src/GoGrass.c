// GoGrass.c (EA's name, from its asserts; TW06): not yet decompiled; the sweep code below is the
// matched small functions.

#include "game_types.h"
#include "platform.h"
#include "grassshader.h"
#include "golfer.h"
#include "endian.h"
#include "gx.h"

// .bss, reverse address order.
// fake match: EA's .bss has 4 zero bytes before lbl_802603B0 and 0x10 after lbl_802602C0's 0x90
// (lbl_80260360 on a 32-byte boundary), which these types alone do not make; the aligned
// attributes stand in for them (a larger EA type, or objects no code uses).
GrassManager lbl_802603B0 __attribute__((aligned(8)));
GXTexObj lbl_8026038C;
UObjMesh lbl_80260360 __attribute__((aligned(32)));
UObjMeshInfo lbl_802602C0;

GrassManager* lbl_80281900 = &lbl_802603B0;

// This file's .sbss (grassshader.h), in reverse address order as the compiler lays it out.
s32   lbl_80282514;
void* lbl_80282510;

// ---- sweep code (not yet cleaned up) ----

void Grass_InitModule(void);
void Grass_UpdateView(void);
void Grass_RenderTopTexture(void);
void Grass_BuildVisibleList(void);
void Grass_CloseModule(void);
u8 fn_80112B80();
void Grass_UnRegisterStreamClients(void);
void Grass_DrawTopTextureDebug(void);
void Grass_BeginRender(void);
void Grass_EndRender(void);
void Grass_DrawBuffers(void);
void Grass_PlaceCell(int nX, int nZ, int nCull, f32 f);
f32 Grass_Fmod(f32 fX, f32 fM);
void Grass_Vec3Add(f32* pA, f32* pB, f32* pOut);
Sphere* Grass_GetObjBoundingSphere(RenderObj* pObj);
int fn_80007CE8(RenderObj* pObj, Camera* pCamera, int nMode, f32 fScale);   // LLObj_Gc.c
void SD_SetShaderTypeParameters(int nRow, void* pData);   // GoTerrain.c: calls row nRow's function with pData
void Grass_Render(void);
void Grass_ReleaseTopCamera(void);
void Grass_FreeTopTexture(void);
void Grass_CopyTopTexture(void);
void RC_vSetCurrentRenderCtx(void* pCamera);   // makes it the current render camera
void RenderState_SetRenderSurface(int a, int nWidth, int nHeight, int nField, int b, int c);
void fn_80016B54(int nWidth, int nHeight, f32 fX, f32 fY);
void RC_ApplyCurrentViewport(void);
void fn_80016948(void);
s32  Ter_SetManageZUpdate(s32 n);  // sets a value, returns the old one
void Ter_RenderGrass(void);
void RenderState_SetClipMode();
void RC_UpdateCurrentScreenMatrices();
void GrassRender_vBuildAndUploadOneTimeData(void);
void fn_800738DC(TexBank* pBank, TexEntry* pTex, u8 bFirst);   // GoShaderObjectCommon
void Grass_QueueRelease(GrassBuffer* pBuffer);
void Grass_DeInitForHole(void);
void fn_80008380(void);
void Grass_RegisterStreamClients(void);
void Grass_LoadNetworkData(GrassChunk* pChunk);
void Grass_CreateTopTexture(void);
void Grass_CreateTopCamera(void);
void Grass_AimTopCamera(void);
s32 Grass_AllocBuffers(void);
void Grass_LoadStreamFile(UStreamObject* pObject);
int  Grass_CompareFarthestFirst(f32** ppA, f32** ppB);
void Grass_AddFreeBuffer(GrassBuffer* pBuffer);
GrassBuffer* Grass_TakeFreeBuffer(s32 nSize);
void Grass_ReleaseQueued(void);
void Grass_FreeBuffers(void);

// The grass's draw data.
char lbl_80194598[4][8] = {"akgras1", "akgras2", "akgras3", "akgras4"}; // textures, by n3A4
f32 lbl_801945B8[4] = {0.21f, 0.31f, 0.1f, 1.0f};                          // a colour (Grass_RenderTopTexture)
f32 lbl_801945C8[8] = {0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};  // the unit square's corners
f32 lbl_801945E8[4] = {0.5f, 0.5f, 0.5f, 0.5f};                            // a colour (Grass_DrawTopTextureDebug)
f32 lbl_801945F8[8] = {0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.5f, 1.0f, 1.0f};  // (Grass_DrawTopTextureDebug)
f32 lbl_80194618[8] = {0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};  // (Grass_DrawTopTextureDebug)

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x80284A30), before the 4.9f Grass_InitModule uses first; its body is unknown.
static f32 GoGrass_StrippedFn(f32 x) {
    return x + 1.0f;
}

// The grass's start, once per round (gomainloop's round set-up): Grass_LoadNetworkData is
// registered for network chunk 6, the manager's settings get their defaults (sway, fade, grid, the
// eight row offsets in af168), its 256 x 256 texture is set up, and gbGrassFrameSkipped is cleared.
void Grass_InitModule(void) {
    lbl_80281900->p370 = NULL;
    lbl_80281900->n1C = 0;
    // port: Grass_LoadNetworkData takes the chunk as a GrassChunk*
    Network_RegisterLoadNetworkCallback(6, (void (*)(u8*))Grass_LoadNetworkData);
    lbl_80281900->n3DC = 1;
    lbl_80281900->n3E0 = 1;
    lbl_80281900->n3F0 = 1;
    lbl_80281900->f3E8 = 4.9f;
    lbl_80281900->f3D0 = 128.0f;
    lbl_80281900->f3BC = 128.0f;
    lbl_80281900->f3C0 = 128.0f;
    lbl_80281900->f3C4 = 1.5f;
    lbl_80281900->f3C8 = 2.6f;
    lbl_80281900->nC0 = 10;
    lbl_80281900->f3D4 = 2.5914f;
    lbl_80281900->f3D8 = 2.5914f;
    lbl_80281900->f3A8 = 2.5f;
    lbl_80281900->f3AC = 2.5f;
    lbl_80281900->f3B0 = 500.0f;
    lbl_80281900->f3B4 = 0.6f;
    lbl_80281900->f3B8 = 0.13f;
    lbl_80281900->n3CC = 0;
    lbl_80281900->n10 = 0;
    lbl_80281900->n104 = 0;
    lbl_80281900->n100 = 0;
    lbl_80281900->f3EC = 0.0f;
    lbl_80281900->f3FC = 0.0f;
    lbl_80281900->f400 = -0.36f;
    lbl_80281900->n3E4 = 1;
    lbl_80281900->f40C = 0.78f;
    lbl_80281900->f410 = 0.16f;
    lbl_80281900->f414 = 0.0f;
    lbl_80281900->f418 = 0.025f;
    lbl_80281900->f41C = 0.025f;
    lbl_80281900->n3A4 = 0;
    lbl_80281900->n14 = -500;
    lbl_80281900->n16 = -500;
    lbl_80281900->n18 = 1000;
    lbl_80281900->n1A = 1000;
    lbl_80281900->af168[0] = 1.19f;
    lbl_80281900->af168[1] = 0.168f;
    lbl_80281900->af168[2] = 0.49f;
    lbl_80281900->af168[3] = 0.308f;
    lbl_80281900->af168[4] = 0.981f;
    lbl_80281900->af168[5] = 0.06f;
    lbl_80281900->af168[6] = 0.685f;
    lbl_80281900->af168[7] = 0.272f;
    fn_8002A528(&lbl_80281900->tex80, 256, 256, fn_8002A624(), NULL, 6, 0, 0, 0);
    lbl_80282514 = 0;
}

// Empty in this build: the round's shutdown (fn_8006CDC4) calls it, the partner of
// Grass_InitModule.
void Grass_CloseModule(void) {
}

// The hole's grass goes (hole unload, beside Ter_UnloadHole): when a grass file is loaded, the
// buffers in use are queued and released, the file's stream memory, the buffers and the top camera
// are freed and gbGrassFrameSkipped cleared. The network chunk count is reset either way.
void Grass_DeInitForHole(void) {
    int i;
    if (lbl_80281900->p370 != NULL) {
        fn_80008380();
        for (i = 0; i < lbl_80281900->anF8[lbl_80281900->n100]; i++) {
            Grass_QueueRelease(lbl_80281900->apF0[lbl_80281900->n100][i]);
        }
        Grass_ReleaseQueued();
        StaticMem_Free(lbl_80281900->p370);
        lbl_80281900->p370 = NULL;
        Grass_FreeBuffers();
        Grass_ReleaseTopCamera();
        lbl_80282514 = 0;
    }
    lbl_80281900->n1C = 0;
}

// The 'gras' stream chunk gets its loader (Grass_LoadStreamFile), only when fn_80112B80 says the
// grass is on (one view, at most three players).
void Grass_RegisterStreamClients(void) {
    if (fn_80112B80() != 0) {
        Stream_RegisterLoadChunkCallback('gras', Grass_LoadStreamFile);
    }
}

// The partner of Grass_RegisterStreamClients: the 'gras' loader is removed, under the same
// fn_80112B80 test.
void Grass_UnRegisterStreamClients(void) {
    if (fn_80112B80() != 0) {
        Stream_UnregisterLoadChunkCallback(0x67726173);
    }
}

// The network chunk 6 handler (registered by Grass_InitModule): the chunk and its data (after its
// n2 0x30-byte entries) are kept in the next of the a48/a20 slots and the data byte-swapped in
// place. The first chunk's data gives f3B8 (the blade height) and n3A4 (which akgras texture).
void Grass_LoadNetworkData(GrassChunk* pChunk) {
    u8* pData;
    lbl_80281900->a48[lbl_80281900->n1C] = pChunk;
    lbl_80281900->a20[lbl_80281900->n1C] = (GrassChunkData*)((u8*)(pChunk + 1) + pChunk->n2 * 0x30);
    pData = (u8*)lbl_80281900->a20[lbl_80281900->n1C];
    BYTESWAP_SWAPDATA(&pData, pData, 8, 4);
    lbl_80281900->f3B8 = lbl_80281900->a20[0]->f4;
    lbl_80281900->n3A4 = lbl_80281900->a20[0]->n0;
    lbl_80281900->n1C = lbl_80281900->n1C + 1;
}

// The 'gras' stream handler: the hole's grass file is loaded. Its header gives the grid (defaults
// when its version is not 100); the records' two offsets become addresses.
void Grass_LoadStreamFile(UStreamObject* pObject) {
    u8* pCur;
    u8* pBase;
    s32 nSkip;
    s32 n;
    int i;

    lbl_80281900->p370 = pObject;
    pCur = pObject->pData;
    Grass_AllocBuffers();
    Grass_CreateTopCamera();
    nSkip = *(s32*)pCur;
    pCur += 0x10;
    pBase = pCur;
    pCur += nSkip;
    if (((GrassFileHeader*)pCur)->uVersion == 100) {
        lbl_80281900->n14 = ((GrassFileHeader*)pCur)->n6;
        lbl_80281900->n16 = ((GrassFileHeader*)pCur)->n8;
        lbl_80281900->n18 = ((GrassFileHeader*)pCur)->nA;
    } else {
        lbl_80281900->n14 = -500;
        lbl_80281900->n16 = -500;
        lbl_80281900->n18 = 400;
    }
    n = ((GrassFileHeader*)pCur)->n0;
    pCur += sizeof(GrassFileHeader);
    lbl_80281900->n1A = n / lbl_80281900->n18;
    lbl_80281900->p8 = (s16*)pCur;
    pCur += n * 2;
    lbl_80281900->n10 = *(s32*)pCur;
    lbl_80281900->pC = (GrassTile*)(pCur + 0x10);
    // port: the file's offsets are made into 32-bit addresses in place
    for (i = 0; i < lbl_80281900->n10; i++) {
        lbl_80281900->pC[i].au10[0] += (u32)pBase;
        lbl_80281900->pC[i].au10[1] += (u32)pBase;
    }
}

// The qsort order of Grass_BuildVisibleList: by each GrassBuffer's f8 (its squared distance from
// the camera), the farthest first, so the grass draws back to front. Equal gives -1.
int Grass_CompareFarthestFirst(f32** ppA, f32** ppB) {
    f32 fA = (*ppA)[2];
    f32 fB = (*ppB)[2];
    if (fA < fB) {
        return 1;
    }
    if (fA >= fB) {
        return -1;
    }
    return 0;
}

// The grass's per-view update, before the hole is drawn (gomainloop, one full-screen view only;
// never while the three-screen camera is on, and not on the first frame after load): new random row
// offsets when f3D4 changed, the queued buffers released, the visible list rebuilt
// (Grass_BuildVisibleList), the top camera aimed and, when n3E4 is set, the top texture rendered;
// then the 16 sway offsets around the circle and the sway phase f414 moved on by f418 per 60th of a
// second.
void Grass_UpdateView(void) {
    f32 vPoint[4];
    int i;
    f32 fSin;
    f32 fCos;

    if (lbl_80281900->p370 == NULL) {
        return;
    }
    if (lbl_80281900->n3DC == 0) {
        return;
    }
    if (lbl_80282514 == 0) {
        lbl_80282514 = 1;
        return;
    }
    // port: as in Grass_Render, EA's GoGrass.c saw GolfCamera_bIs3ScreenCamOn as returning int
    if (((int (*)(void))GolfCamera_bIs3ScreenCamOn)() != 0) {
        return;
    }
    if (lbl_80281900->f3D4 != lbl_80281900->f3D8) {
        for (i = 0; i < 8; i++) {
            lbl_80281900->af168[i] = lbl_80281900->f3D4 * Misc_RandFuncf(1);
        }
        lbl_80281900->f3D8 = lbl_80281900->f3D4;
    }
    Grass_ReleaseQueued();
    Grass_BuildVisibleList();
    Grass_AimTopCamera();
    if (lbl_80281900->n3E4 != 0) {
        Grass_RenderTopTexture();
    }
    fSin = Math_Sin(lbl_80281900->f40C);
    fCos = Math_Cos(lbl_80281900->f40C);
    for (i = 0; i < 16; i++) {
        vPoint[0] = fCos * lbl_80281900->f41C *
                    Math_Sin(lbl_80281900->f414 + 2.0f * PI * ((f32)i / 16.0f));
        vPoint[1] = 0.0f;
        vPoint[2] = fSin * lbl_80281900->f41C *
                    Math_Sin(lbl_80281900->f414 + 2.0f * PI * ((f32)i / 16.0f));
        vPoint[3] = 0.0f;
        LLMath_CopyVec(vPoint, lbl_80281900->av230[i]);
    }
    lbl_80281900->f228 = 16.0f * lbl_80281900->f410 * fCos;
    lbl_80281900->f22C = 16.0f * lbl_80281900->f410 * fSin;
    lbl_80281900->f414 = lbl_80281900->f414 + 60.0f * (lbl_80281900->f418 * gSession.fFrameTime);
    if (lbl_80281900->f414 > 2.0f * PI) {
        lbl_80281900->f414 = lbl_80281900->f414 - 2.0f * PI;
    }
}

// Draws the grass after the hole (gomainloop, one full-screen view, not with the three-screen
// camera): render states and shader parameters (Grass_BeginRender), the buffers
// (Grass_DrawBuffers), states back (Grass_EndRender), and the top texture's debug view when n3CC is
// set (never, in this build).
void Grass_Render(void) {
    // port: EA's GoGrass.c saw GolfCamera_bIs3ScreenCamOn as returning int (its result is not masked here); it
    //       returns u8
    if (lbl_80281900->p370 != NULL && lbl_80281900->n3E0 != 0 && ((int (*)(void))GolfCamera_bIs3ScreenCamOn)() == 0) {
        Grass_BeginRender();
        Grass_DrawBuffers();
        Grass_EndRender();
        if (lbl_80281900->n3CC != 0) {
            Grass_DrawTopTextureDebug();
        }
    }
}

// The grass's overhead camera (at load, Grass_LoadStreamFile): a flat 20 x 20 lens drawing into a
// 256 x 256 frame buffer, for the top texture.
void Grass_CreateTopCamera(void) {
    lbl_80281900->pLens = CA_spCreateCamera();
    lbl_80281900->pFrameBuf = FB_spCreateFrameBuffer();
    lbl_80281900->pRect = VM_spCreateViewport();
    FB_vSetFrameBuffer(lbl_80281900->pFrameBuf, 0.0f, 0.0f, 256.0f, 256.0f, 1.0f, 1.0f);
    VM_vSetViewportRect(lbl_80281900->pRect, 0.0f, 0.0f, 1.0f, 1.0f);
    fn_800B3438(lbl_80281900->pRect, 1.0f, 1.0f);
    CA_vInitCamera(lbl_80281900->pLens);
    fn_80076A0C_SetType(lbl_80281900->pLens, 1);
    fn_80076948(lbl_80281900->pLens, 20.0f, 20.0f);
    lbl_80281900->pCamera =
        RC_spCreateRenderCtx(lbl_80281900->pLens, lbl_80281900->pFrameBuf, lbl_80281900->pRect);
}

// The partner of Grass_CreateTopCamera: its frame buffer, lens, viewport and render context are
// released.
void Grass_ReleaseTopCamera(void) {
    FB_vReleaseFrameBuffer(lbl_80281900->pFrameBuf);
    CA_vReleaseCamera(lbl_80281900->pLens);
    VM_vReleaseViewport(lbl_80281900->pRect);
    RC_vReleaseRenderCtx(lbl_80281900->pCamera);
}

// Points the overhead lens straight down from height f3B0 (500) at the visible grass: over the
// list's minimum x, z corner plus half the lens's view size.
void Grass_AimTopCamera(void) {
    f32 aEye[4];
    f32 aAt[4];
    f32 fX;
    f32 fZ;

    Camera_GetCurrentLens();
    fX = 0.5f * lbl_80281900->pLens->fFlatWidth + lbl_80281900->fMinX;
    fZ = 0.5f * lbl_80281900->pLens->fFlatHeight + lbl_80281900->fMinZ;
    aEye[0] = fX;
    aEye[1] = lbl_80281900->f3B0;
    aEye[2] = fZ;
    aEye[3] = 1.0f;
    aAt[0] = fX;
    aAt[1] = lbl_80281900->f3B0 - 1.0f;
    aAt[2] = fZ;
    aAt[3] = 1.0f;
    CA_vSetLookAt(lbl_80281900->pLens, aEye, aAt);
}

// The grass's 256 x 256 top texture: its buffer (allocated here, GoGrass.c line 1311) and texture
// object. fn_80112D20 calls it when the grass is on.
void Grass_CreateTopTexture(void) {
    lbl_80282510 = StaticMem_Alloc(GXGetTexBufferSize(256, 256, 4, 0, 0), 2, 32, "GoGrass.c", 1311);
    GXInitTexObj(&lbl_8026038C, lbl_80282510, 256, 256, 4, 0, 0, 0);
}

// The partner of Grass_CreateTopTexture (fn_80112DA0): the buffer is freed if there is one.
void Grass_FreeTopTexture(void) {
    if (lbl_80282510 != NULL) {
        StaticMem_Free(lbl_80282510);
    }
    lbl_80282510 = NULL;
}

// The embedded frame buffer's 256 x 256 corner is copied into the top texture and the texture cache
// invalidated.
void Grass_CopyTopTexture(void) {
    GXSetTexCopySrc(0, 0, 256, 256);
    GXSetTexCopyDst(256, 256, 4, 0);
    GXCopyTex(lbl_80282510, 0);
    GXPixModeSync();
    GXInvalidateTexAll();
}

// Renders the top texture: with the overhead camera current, a 256 x 256 viewport is filled with
// gGrassTopClearColor, the terrain's grass drawn from above (Ter_RenderGrass) and copied out
// (Grass_CopyTopTexture); then the previous camera and the full 512 x 448 screen come back.
// Grass_BeginRender loads the texture as the blades' ground colour.
void Grass_RenderTopTexture(void) {
    s32 nOld;
    void* pCamera;

    pCamera = RC_spGetCurrentRenderCtx();
    RC_vSetCurrentRenderCtx(lbl_80281900->pCamera);
    DS_vEnableZBufferUpdate(0);
    DS_vSetZBufferMode(7);
    DS_vSetAlphaTestMode(0, 6, 128);
    RenderState_SetRenderSurface(1, 256, 256, 0, 1, 1);
    RenderState_SetScissor(0, 0, 256, 256);
    fn_80016B54(256, 256, 1.0f, 1.0f);
    RC_ApplyCurrentViewport();
    fn_80016948();
    RC_UpdateCurrentScreenMatrices();
    RC_vUpdateRenderCtxTransformationMatrices(RC_spGetCurrentRenderCtx());
    RenderState_SetViewport(RC_spGetCurrentRenderCtx());
    RenderState_SetCameraMatrices();
    RenderState_Flush();
    RenderView_SetUseCurrentMatrices(0);
    RenderState_SetDrawFlags(0);
    RenderView_SetColor(lbl_801945B8);
    DS_vEnableZBufferUpdate(0);
    DS_vSetAlphaTestMode(0, 6, 128);
    RenderState_Flush();
    RenderView_DrawPrimitive(161, lbl_801945C8, 0, lbl_801945C8, 2);
    nOld = Ter_SetManageZUpdate(0);
    Ter_RenderGrass();
    Ter_SetManageZUpdate(nOld);
    Grass_CopyTopTexture();
    RC_vSetCurrentRenderCtx(pCamera);
    RenderState_SetViewport(RC_spGetCurrentRenderCtx());
    RC_UpdateCurrentScreenMatrices();
    RC_vUpdateRenderCtxTransformationMatrices(RC_spGetCurrentRenderCtx());
    RenderState_SetCameraMatrices();
    RenderState_SetRenderSurface(0, 512, 448, lbl_80281B88 & 1, 8, 1);
    RenderState_SetScissor(0, 0, 512, 448);
    fn_80016B54(512, 448, 1.0f, 1.0f);
    RC_ApplyCurrentViewport();
    DS_vSetAlphaTestMode(1, 6, 128);
    DS_vEnableZBufferUpdate(1);
    DS_vSetZBufferMode(3);
    RenderState_Flush();
}

// A debug view: the top texture drawn on a screen rectangle at half brightness. Grass_Render calls
// it only when n3CC is set, which nothing in this build does.
void Grass_DrawTopTextureDebug(void) {
    RenderView_SetUseCurrentMatrices(0);
    RenderState_SetDrawFlags(16);
    RenderView_SetColor(lbl_801945E8);
    DS_vEnableZBufferUpdate(0);
    DS_vSetAlphaTestMode(0, 6, 128);
    GXLoadTexObj(&lbl_8026038C, 0);
    GXSetNumTexGens(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetNumTevStages(1);
    GXSetTevColorIn(0, 15, 8, 10, 15);
    GXSetTevColorOp(0, 0, 0, 1, 1, 0);
    GXSetTevAlphaIn(0, 7, 4, 5, 7);
    GXSetTevAlphaOp(0, 0, 0, 1, 1, 0);
    DS_vSetZBufferMode(7);
    RenderState_Flush();
    RenderView_DrawPrimitive(161, lbl_801945F8, 0, lbl_80194618, 2);
    DS_vSetZBufferMode(3);
    DS_vEnableZBufferUpdate(1);
    DS_vSetAlphaTestMode(1, 6, 128);
    RenderState_Flush();
}

// The grass's render states: the camera's matrices, no clipping, no alpha test, no z writes, draw
// flags 80; then its texture stages and shader parameters (GrassRender_vBuildAndUploadOneTimeData).
void Grass_BeginRender(void) {
    RC_UpdateCurrentScreenMatrices();
    RC_vUpdateRenderCtxTransformationMatrices(RC_spGetCurrentRenderCtx());
    RenderState_SetClipMode(0);
    RenderState_SetCameraMatrices();
    DS_vSetAlphaTestMode(0, 6, 128);
    DS_vEnableZBufferUpdate(0);
    RenderState_SetDrawFlags(80);
    RenderState_Flush();
    GrassRender_vBuildAndUploadOneTimeData();
}

// Sets up the grass's three texture stages (its blade texture by n3A4, akgras1..4, and the top
// texture) and works out the shader parameters from the overhead lens and the settings: the top
// texture's coordinates from x and z (af108), the fade with distance (af148: 1 + f3C8 - distance /
// f3C4), the colours, then hands them to SD_vSetGrassParamsOnce. Called every time the grass is
// drawn (Grass_BeginRender), despite its name.
void GrassRender_vBuildAndUploadOneTimeData(void) {
    u64 uHash = fn_8000BEE4(lbl_80194598[lbl_80281900->n3A4]);
    f32 fX;
    f32 fZ;

    fn_800102DC(uHash, &lbl_80281900->pBank, &lbl_80281900->pTex);
    fn_800738DC(lbl_80281900->pBank, lbl_80281900->pTex, 1);
    GXLoadTexObj(&lbl_8026038C, 2);
    GXSetNumTexGens(2);
    GXSetNumTevStages(3);
    GXSetTevOrder(0, 1, 2, 4);
    GXSetTevOrder(1, 0, 0, 4);
    GXSetTevOrder(2, 0, 1, 4);
    GXSetTevColorIn(0, 15, 8, 12, 15);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(0, 7, 6, 6, 7);
    GXSetTevAlphaOp(0, 0, 0, 1, 1, 0);
    GXSetTevColorIn(1, 15, 8, 12, 0);
    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(1, 7, 6, 6, 7);
    GXSetTevAlphaOp(1, 0, 0, 1, 1, 0);
    GXSetTevColorIn(2, 15, 0, 12, 15);
    GXSetTevColorOp(2, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(2, 7, 4, 5, 7);
    GXSetTevAlphaOp(2, 0, 0, 1, 1, 0);
    fX = lbl_80281900->pLens->m4[3][0] - 0.5f * lbl_80281900->pLens->fFlatWidth;
    fZ = lbl_80281900->pLens->m4[3][2] - 0.5f * lbl_80281900->pLens->fFlatHeight;
    lbl_80281900->af108[0][0] = 1.0f + fX / lbl_80281900->pLens->fFlatWidth;
    lbl_80281900->af108[0][1] = 1.0f + fZ / lbl_80281900->pLens->fFlatHeight;
    lbl_80281900->af108[0][2] = 1.0f;
    lbl_80281900->af108[0][3] = 1.0f;
    lbl_80281900->af108[1][0] = -1.0f / lbl_80281900->pLens->fFlatWidth;
    lbl_80281900->af108[1][1] = -1.0f / lbl_80281900->pLens->fFlatHeight;
    lbl_80281900->af108[1][2] = 0.0f;
    lbl_80281900->af108[1][3] = 0.0f;
    lbl_80281900->af148[0][0] = 0.0f;
    lbl_80281900->af148[0][1] = 0.0f;
    lbl_80281900->af148[0][2] = 0.0f;
    lbl_80281900->af148[0][3] = 1.0f + lbl_80281900->f3C8;
    lbl_80281900->af148[1][0] = 1.0f;
    lbl_80281900->af148[1][1] = 1.0f;
    lbl_80281900->af148[1][2] = 1.0f;
    lbl_80281900->af148[1][3] = -1.0f / lbl_80281900->f3C4;
    lbl_80281900->af218[0] = lbl_80281900->f3C0 * lbl_80281900->f3F8 / 255.0f;
    lbl_80281900->af218[1] = lbl_80281900->f3C0 * lbl_80281900->f3F8 / 255.0f;
    lbl_80281900->af218[2] = lbl_80281900->f3C0 * lbl_80281900->f3F8 / 255.0f;
    lbl_80281900->af218[3] = 0.5f;
    lbl_80281900->af208[0] = lbl_80281900->f3BC / 255.0f;
    lbl_80281900->af208[1] = lbl_80281900->f3BC / 255.0f;
    lbl_80281900->af208[2] = lbl_80281900->f3BC / 255.0f;
    lbl_80281900->af208[3] = 0.5f;
    SD_vSetGrassParamsOnce(lbl_80281900->af208, lbl_80281900->af218, lbl_80281900->af108,
                           lbl_80281900->af168, lbl_80281900->af148, lbl_80281900->av230,
                           lbl_80281900->f228, lbl_80281900->f22C);
    RenderState_Flush();
}

void Grass_EndRender(void) {
    DS_vSetAlphaTestMode(1, 6, 128);
    DS_vEnableZBufferUpdate(1);
    RenderState_Flush();
}

// Draws the visible buffers in two passes (GrassParams n24: which shell set): the camera's
// direction, flattened and normalised, picks each set's vertex run (a18) and opacity (a10); each
// buffer's x, z goes to the grass shader (SD_SetShaderTypeParameters row 17, the GrassParams at
// f348), then the buffer is drawn.
void Grass_DrawBuffers(void) {
    f32 vDir[4];
    s32 nPass;
    int i;
    GrassBuffer* pBuffer;
    s32 nBuffers;
    CamLens* pLens = Camera_GetCurrentLens();

    nBuffers = lbl_80281900->anF8[lbl_80281900->n100];
    LLMath_CopyVec(pLens->m4[2], vDir);
    vDir[1] = 0.0f;
    if (vDir[0] != 0.0f || vDir[1] != 0.0f || vDir[2] != 0.0f) {
        LLMath_Normalize3(vDir, vDir);
    }
    if (vDir[2] < 0.0f) {
        lbl_80281900->n360 = 0;
    } else {
        lbl_80281900->n360 = 1;
    }
    if (vDir[0] < 0.0f) {
        lbl_80281900->n364 = 0;
    } else {
        lbl_80281900->n364 = 1;
    }
    lbl_80281900->f358 = fabs(vDir[2]);
    lbl_80281900->f35C = fabs(vDir[0]);
    lbl_80281900->f354 = lbl_80281900->f3B4;
    lbl_80281900->f348 = lbl_80281900->f3D0;
    lbl_80281900->f368 = lbl_80281900->f3B8;
    for (nPass = 0; nPass < 2; nPass++) {
        for (i = 0; i < nBuffers; i++) {
            pBuffer = lbl_80281900->apF0[lbl_80281900->n100][i];
            lbl_80281900->f34C = pBuffer->f0;
            lbl_80281900->f350 = pBuffer->f4;
            lbl_80281900->n36C = nPass;
            SD_SetShaderTypeParameters(17, &lbl_80281900->f348);
            fn_800082CC((UObjMeshPart*)pBuffer->a14);
        }
    }
}

// Places the grass of grid cell (nX, nZ) in the list being built, fDist its squared camera
// distance: the buffer already made for that spot in the other list is reused, else a free one is
// taken (Grass_TakeFreeBuffer) and built from the cell's file record (shader object type 17,
// SD_vShaderObject_Grass_Static_Init). The list's bounds grow to take it in. nCull (the cell's
// fn_80007CE8 result) is not used.
void Grass_PlaceCell(int nX, int nZ, int nCull, f32 f) {
    GrassBufferDesc desc;
    int i;
    u8 bFound;
    GrassBuffer* pOld;
    f32 fX;
    f32 fZ;
    GrassBuffer* pBuffer;
    s32 nCur;
    int nTile;

    nTile = lbl_80281900->p8[nX + nZ * lbl_80281900->n18];
    fX = 2.5f * (f32)nX + (f32)lbl_80281900->n14;
    fZ = 2.5f * (f32)nZ + (f32)lbl_80281900->n16;
    nCur = lbl_80281900->n100;
    bFound = 0;
    for (i = 0; i < lbl_80281900->anF8[1 - nCur] && !bFound; i++) {
        pOld = lbl_80281900->apF0[1 - nCur][i];
        if (fX == pOld->f0 && fZ == pOld->f4) {
            pBuffer = pOld;
            bFound = 1;
        }
    }
    if (!bFound) {
        pBuffer = Grass_TakeFreeBuffer((lbl_80281900->pC[nTile].n0 * 32 + 0x580) / 16);
        if (pBuffer == NULL) {
            return;
        }
        pBuffer->f0 = fX;
        pBuffer->f4 = fZ;
        pBuffer->p48 = &lbl_80281900->pC[nTile];
        desc.pTile = pBuffer->p48;
        desc.pVerts = pBuffer->p40;
        desc.nVerts = pBuffer->n44;
        desc.fX = pBuffer->f0;
        desc.fZ = pBuffer->f4;
        desc.f14 = lbl_80281900->f3B8;
        fn_8000827C((UObjMeshPart*)pBuffer->a14, NULL, 17, &desc);
    }
    pBuffer->b10 = 1;
    pBuffer->f8 = f;
    pBuffer->nC = 1;
    if (pBuffer->nC == 1) {
        lbl_80281900->n404++;
    } else {
        lbl_80281900->n408++;
    }
    lbl_80281900->apF0[nCur][lbl_80281900->anF8[nCur]] = pBuffer;
    lbl_80281900->anF8[nCur]++;
    if (pBuffer->f0 < lbl_80281900->fMinX) {
        lbl_80281900->fMinX = pBuffer->f0;
    }
    if (pBuffer->f0 > lbl_80281900->fMaxX) {
        lbl_80281900->fMaxX = pBuffer->f0;
    }
    if (pBuffer->f4 < lbl_80281900->fMinZ) {
        lbl_80281900->fMinZ = pBuffer->f4;
    }
    if (pBuffer->f4 > lbl_80281900->fMaxZ) {
        lbl_80281900->fMaxZ = pBuffer->f4;
    }
}

// Builds the other list of grass buffers for this frame: every grid cell within f3E8 of the point
// f3EC ahead of the camera (snapped to the 2.5 grid) whose bounding sphere is in view gets placed
// (Grass_PlaceCell), the list is sorted farthest first, and the old list's unplaced buffers are
// queued for release. Also sets f3F8, the camera's downward tilt between f400 and f3FC as 0..1.
void Grass_BuildVisibleList(void) {
    f32 vFlat[4];
    f32 vLook[4];
    f32 vAhead[4];
    f32 vPos[4];
    f32 vCentre[4];
    f32 vSphere[4];
    int nX;
    int nZ;
    Sphere* pSphere;
    int nCull;
    CamLens* pLens;
    GrassTile* pTile;
    s32 nOther;
    int nRadius;
    int nCellX;
    int nCellZ;
    int i;
    f32 fX;
    f32 fZ;
    f32 fCellX;

    pLens = Camera_GetCurrentLens();
    lbl_80281900->n100 = 1 - lbl_80281900->n100;
    lbl_80281900->anF8[lbl_80281900->n100] = 0;
    nOther = 1 - lbl_80281900->n100;
    lbl_80281900->fMinX = 10000.0f;
    lbl_80281900->fMaxX = -10000.0f;
    lbl_80281900->fMinZ = 10000.0f;
    lbl_80281900->fMaxZ = -10000.0f;
    for (i = 0; i < lbl_80281900->anF8[nOther]; i++) {
        lbl_80281900->apF0[nOther][i]->b10 = 0;
    }
    lbl_80281900->n404 = 0;
    lbl_80281900->n408 = 0;
    nRadius = 1.0f + lbl_80281900->f3E8 / 2.5f;
    Vec3Copy(pLens->m4[2], vLook);
    Vec3Copy(vLook, vFlat);
    if (vLook[0] != 0.0f || vLook[1] != 0.0f || vLook[2] != 0.0f) {
        LLMath_Normalize3(vLook, vLook);
    }
    lbl_80281900->f3F8 =
        (vLook[1] - lbl_80281900->f400) / (lbl_80281900->f3FC - lbl_80281900->f400);
    if (lbl_80281900->f3F8 < 0.0f) {
        lbl_80281900->f3F8 = 0.0f;
    }
    if (lbl_80281900->f3F8 > 1.0f) {
        lbl_80281900->f3F8 = 1.0f;
    }
    vFlat[1] = 0.0f;
    if (vFlat[0] != 0.0f || vFlat[1] != 0.0f || vFlat[2] != 0.0f) {
        LLMath_Normalize3(vFlat, vFlat);
    }
    LLMath_CopyVec(pLens->m4[3], vPos);
    LLMath_Scale(lbl_80281900->f3EC, vFlat, vAhead);
    Grass_Vec3Add(vAhead, vPos, vCentre);
    if (vCentre[0] < 0.0f) {
        fX = vCentre[0] - (2.5f - (f32)fabs(Grass_Fmod(vCentre[0], 2.5f)));
    } else {
        fX = vCentre[0] - (f32)fabs(Grass_Fmod(vCentre[0], 2.5f));
    }
    if (vCentre[2] < 0.0f) {
        fZ = vCentre[2] - (2.5f - (f32)fabs(Grass_Fmod(vCentre[2], 2.5f)));
    } else {
        fZ = vCentre[2] - (f32)fabs(Grass_Fmod(vCentre[2], 2.5f));
    }
    nCellX = (fX - (f32)lbl_80281900->n14) / 2.5f;
    nCellZ = (fZ - (f32)lbl_80281900->n16) / 2.5f;
    lbl_80260360.pInfo = &lbl_802602C0;
    pSphere = Grass_GetObjBoundingSphere((RenderObj*)&lbl_80260360);
    for (nX = nCellX - nRadius; nX <= nCellX + nRadius; nX++) {
        fCellX = 2.5f * (f32)nX;
        for (nZ = nCellZ - nRadius; nZ <= nCellZ + nRadius; nZ++) {
            if (nX < 0 || nX >= lbl_80281900->n18 || nZ < 0 || nZ >= lbl_80281900->n1A) {
                continue;
            }
            if (lbl_80281900->p8[nX + nZ * lbl_80281900->n18] == -1) {
                continue;
            }
            pTile = &lbl_80281900->pC[lbl_80281900->p8[nX + nZ * lbl_80281900->n18]];
            // the cell's sphere: its centre, and the radius over half its height and the
            // 1.25 x 1.25 half cell
            pSphere->radius = Math_Sqrt(
                3.125f + (0.5f * (pTile->f8 + lbl_80281900->f3B8 - pTile->f4)) *
                             (0.5f * (pTile->f8 + lbl_80281900->f3B8 - pTile->f4)));
            pSphere->x = 1.25f + ((f32)lbl_80281900->n14 + fCellX);
            pSphere->y = 0.5f * (lbl_80281900->f3B8 + (pTile->f4 + pTile->f8));
            pSphere->z = 1.25f + (2.5f * (f32)nZ + (f32)lbl_80281900->n16);
            nCull = fn_80007CE8((RenderObj*)&lbl_80260360, RC_spGetCurrentRenderCtx(),
                                0, ViewController_GetCameraControl(gTerRenderer.iCurrentViewContext)->f54);
            if (nCull == 2) {
                continue;
            }
            Vec3Copy(&pSphere->x, vSphere);
            Grass_PlaceCell(nX, nZ, nCull, LLMath_SquareDistanceBetween3(vSphere, vPos));
        }
    }
    // port: Grass_CompareFarthestFirst compares two GrassBuffer pointers' f8 (the larger first)
    qsort(lbl_80281900->apF0[lbl_80281900->n100], lbl_80281900->anF8[lbl_80281900->n100], 4,
          (s32 (*)(const void*, const void*))Grass_CompareFarthestFirst);
    for (i = 0; i < lbl_80281900->anF8[nOther]; i++) {
        if (lbl_80281900->apF0[nOther][i]->b10 == 0) {
            Grass_QueueRelease(lbl_80281900->apF0[nOther][i]);
        }
    }
}

// Puts pBuffer in the first empty one of the 16 free slots (apDC).
void Grass_AddFreeBuffer(GrassBuffer* pBuffer) {
    int i;
    for (i = 0; i < 16; i++) {
        if (lbl_80281900->apDC[i] == NULL) {
            lbl_80281900->apDC[i] = pBuffer;
            lbl_80281900->nE8 = lbl_80281900->nE8 + 1;
            return;
        }
    }
}

// Pushes pBuffer on the apD8 stack.
void Grass_QueueRelease(GrassBuffer* pBuffer) {
    lbl_80281900->apD8[lbl_80281900->nE4] = pBuffer;
    lbl_80281900->nE4 = lbl_80281900->nE4 + 1;
}

// Takes the smallest free buffer holding at least nSize vertices out of the free slots (NULL if
// there is none).
GrassBuffer* Grass_TakeFreeBuffer(s32 nSize) {
    int i;
    int nBest = -1;
    GrassBuffer* pBest = NULL;
    GrassBuffer* pBuffer;

    if (lbl_80281900->nE8 == 0) {
        return NULL;
    }
    for (i = 0; i < 16; i++) {
        pBuffer = lbl_80281900->apDC[i];
        if (pBuffer != NULL && pBuffer->n44 >= nSize && (pBest == NULL || pBuffer->n44 < pBest->n44)) {
            nBest = i;
            pBest = pBuffer;
        }
    }
    if (pBest != NULL) {
        lbl_80281900->nE8--;
        lbl_80281900->apDC[nBest] = NULL;
        return pBest;
    }
    return NULL;
}

// Empties the release queue: each buffer's shader object is closed (fn_80008248) and the buffer
// goes back to the free slots.
void Grass_ReleaseQueued(void) {
    while (lbl_80281900->nE4 != 0) {
        fn_80008248((UObjMeshPart*)lbl_80281900->apD8[lbl_80281900->nE4 - 1]->a14);
        Grass_AddFreeBuffer(lbl_80281900->apD8[lbl_80281900->nE4 - 1]);
        lbl_80281900->nE4 = lbl_80281900->nE4 - 1;
    }
}

// ---- end of sweep code ----

// Makes the 16 grass buffers and their lists, all free: the first four hold 600 vertices, the rest
// 450. Returns the bytes allocated.
s32 Grass_AllocBuffers(void) {
    s32 nBytes;
    s32 nVerts;
    s32 nSize;
    int i;

    lbl_80281900->nE4 = 0;
    lbl_80281900->nE8 = 16;
    lbl_80281900->nE0 = 16;
    lbl_80281900->anF8[0] = 0;
    lbl_80281900->anF8[1] = 0;
    lbl_80281900->pEC = StaticMem_Alloc(16 * sizeof(GrassBuffer), 2, 16, "GoGrass.c", 3707);
    lbl_80281900->apF0[0] = StaticMem_Alloc(16 * sizeof(GrassBuffer*), 2, 16, "GoGrass.c", 3709);
    lbl_80281900->apF0[1] = StaticMem_Alloc(16 * sizeof(GrassBuffer*), 2, 16, "GoGrass.c", 3711);
    lbl_80281900->apD8 = StaticMem_Alloc(16 * sizeof(GrassBuffer*), 2, 16, "GoGrass.c", 3714);
    lbl_80281900->apDC = StaticMem_Alloc(16 * sizeof(GrassBuffer*), 2, 16, "GoGrass.c", 3716);
    nBytes = 16 * sizeof(GrassBuffer) + 4 * (16 * sizeof(GrassBuffer*));
    for (i = 0; i < lbl_80281900->nE0; i++) {
        if (i < 4) {
            nVerts = 600;
        } else {
            nVerts = 450;
        }
        nSize = nVerts * 16;
        lbl_80281900->pEC[i].p40 = StaticMem_Alloc(nSize, 2, 16, "GoGrass.c", 3729);
        nBytes += nSize;
        lbl_80281900->pEC[i].n44 = nVerts;
    }
    for (i = 0; i < lbl_80281900->nE0; i++) {
        lbl_80281900->apDC[i] = &lbl_80281900->pEC[i];
    }
    return nBytes;
}

// ---- sweep code (not yet cleaned up) ----

// The partner of Grass_AllocBuffers: each buffer's vertices, then the lists and the buffers.
void Grass_FreeBuffers(void) {
    int i;
    for (i = 0; i < lbl_80281900->nE0; i++) {
        StaticMem_Free(lbl_80281900->pEC[i].p40);
    }
    StaticMem_Free(lbl_80281900->apF0[0]);
    StaticMem_Free(lbl_80281900->apF0[1]);
    StaticMem_Free(lbl_80281900->pEC);
    StaticMem_Free(lbl_80281900->apDC);
    StaticMem_Free(lbl_80281900->apD8);
}

s32 Grass_IsLoaded(void);

// Whether the hole has its grass file loaded (GoGolfCam asks, for the lie-based camera height).
s32 Grass_IsLoaded(void) {
    return lbl_80281900->p370 != NULL;
}

// fmod for floats: the remainder of fX / fM.
f32 Grass_Fmod(f32 fX, f32 fM) {
    return fmod(fX, fM);
}

// pOut = pB + pA, three floats (paired singles).
#ifdef __MWERKS__
asm void Grass_Vec3Add(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_add f2, f2, f0
    ps_add f3, f3, f1
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void Grass_Vec3Add(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

// The object's bounding sphere.
Sphere* Grass_GetObjBoundingSphere(RenderObj* pObj) {
    return &pObj->data->bounds;
}

// ---- end of sweep code ----
