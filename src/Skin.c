// Skin.c (EA's name, from its asserts; Golf\Animation\Skin.c in EA's 2002 source tree, and
// golf/animation/Skin.c in TW07, whose SKN_ prefix the names here take): a character's skinned
// model. SKN_Create makes a skin from its file (headers byte-swapped once in the file, the model
// and description copied, byte-swapped and their offsets made pointers); SKN_AllocRenderData and
// SKN_FreeRenderData give and take back what posing and drawing it needs (single view keeps only
// the viewed golfer's body skin loaded). Each frame SKN_PoseCharacter builds the skin's matrices
// from the character model's and SKN_DrawCharacter lights and draws its parts (all but "shadow",
// or with flag 2 only those, for the shadow pass). Also here: small render wrappers other modules
// use (RC_ viewport, LI_ / LF_ lights, SD_ shader objects) and the animation blend of morph
// weights (SKN_BlendMorphWeights).

#include "game_types.h"
#include "engine.h"
#include "camera.h"
#include "lighting.h"
#include "dynobj.h"
#include "charstate.h"
#include "endian.h"
#include "platform.h"
#include "ball.h"
#include "terrain.h"
#include "game.h"

void  fn_80008380(void);                                  // wait for the GPU (fn_800070DC)
void  fn_80016978(f32 fLeft, f32 fTop, f32 fWidth, f32 fHeight);   // Code80016198.c: the viewport
void  fn_8006E7A4(LightGroup* pGroup);                   // GoLighting.c: load the group's lights
void  fn_8006EADC(UObject* pObj);                        // GoLighting.c: light the object
void  fn_8006ED70(void);                                 // GoLighting.c: lighting off
void  fn_80093824(void);                                 // GoLightFogEnv.c
f32   fn_8004B78C(CourseInfo* pCourse, f32* pPos);       // GoTerrainCollision.c: the ground's light
void  SD_SetShaderTypeParameters(int nRow, void* pData); // GoTerrain.c: calls row nRow's pfn8
void  fn_801127A0(void* pDesc);                          // hwsMaterial_Gc.c
HwsMemBlock* fn_801128C8(SkinDesc* pDesc, s32 nSize);         // hwsOverride_Gc.c
HwsOverrideTable* fn_80112A10(SkinDesc* pDesc, s32 nMeshes);  // hwsOverride_Gc.c
void  fn_80112B18(HwsOverrideTable* pTable, int i, void* p);  // hwsOverride_Gc.c
void  SkinPart_SetChangeAllCopies(u8 b);                 // SkinPart.c
void  SkinPart_FixupDesc(SkinDesc* pDesc);               // SkinPart.c: offsets to pointers
void  SkinPart_AllocChoices(Skin* pSkin);                // SkinPart.c
s32   SkinPart_GetMaxOptionsSize(Skin* pSkin);           // SkinPart.c
void  SkinPart_UpdateMarks(Skin* pSkin);                 // SkinPart.c
void  SkinPart_InitSkin(void);                           // SkinPart.c: empty
void  SkinPart_UpdateSkin(void);                         // SkinPart.c: empty
void  fn_8011C9B0(Skin* pSkin);                          // SkinMorph.c
void  fn_8011CB5C(Skin* pSkin, int nView);               // SkinMorph.c
s32   fn_8011CDE8(Skin* pSkin);                          // SkinMorph.c
void  fn_8011CE58(Skin* pSkin);                          // SkinMorph.c
void  BitArray_CopyArray(u32* pSrc, u32* pDst, u32 nBits);      // Skeleton.c
u8    Character_IsGolfer(Character* pChar);              // char.c
void  fn_80037D5C(SkinDesc* pDesc);                      // Code80037AB8.c

void  SKN_DrawClubParts(Character* pChar);
void  SKN_DrawBoneTri(Character* pChar, int nView);
void  RC_ApplyViewport(void* pCamera);
void  SD_InitShaderObject(ShaderObject* pObj, int nRow, const void* pDesc);
void  SD_FreeShaderObject(ShaderObject* pObj);
void  SD_DrawShaderObject(ShaderObject* pObj);
void  LI_LoadLightGroup(LightGroup* pGroup);
void  SKN_GetCharPosition(Character* pChar, f32* pOut);
int   SKN_GetLightCourse(void);
void  LI_ResetLights(void);
void  LI_SetObjectLights(UObject* pObj);
void  LF_LoadCurrentLights(void);
void  LF_SetCurrentBrightness(f32 f);
void  SKN_BuildMatrices(Skin* pSkin, CharModel* pCharModel, int nSkip, int nFirst, int nView);
void  SKN_SetMeshMatrices(Skin* pSkin, int n);
void  SKN_SwapMeshEntries(SkinModel44* pEntries, s32 nEntries);
void  SKN_SwapMeshEntryLists(SkinModel44* pEntries, s32 nEntries);
void  SKN_SwapMatrixBlends(SkinModel54* pEntries, s32 nEntries);
void  SKN_FixupModel(SkinModel* pModel);
void  SKN_SwapModelHeader(SkinModel* pModel);
void  SKN_SwapDescHeader(SkinDesc* pDesc);
void  SKN_SwapDesc(SkinDesc* pDesc);
void  SKN_SwapBonePoses(BonePose* pBones, s32 nBones);

// .bss and .sbss, each in reverse address order (CodeWarrior lays them out last-defined-first)
SkinTris gSkinBoneTris;         // SKN_DrawBoneTri's triangle and mesh object, per view
// A buffer SKN_CloseModule frees; nothing in this build allocates it (SKN_InitModule is empty).
s32 gSkinFrameBufSize;          // only ever cleared (by SKN_CloseModule)
s32 gSkinFrameBufUsed;          // cleared every frame before the characters are posed
void* gSkinFrameBuf;

// Draws the "shadow" part of the character's skin, then that of its club's skin (Character.pClubSet,
// its current club class), with the skins' override table nView.
void SKN_DrawShadowParts(Character* pChar) {
    u64 uShadow;
    s32 nParts;
    int i;
    Skin* pSkin;

    SKA_PackName(&uShadow, "shadow");
    SkinPart_BeginDraw(pChar->pSkin, pChar->nView);
    nParts = SkinPart_GetNumParts(pChar->pSkin);
    for (i = 0; i < nParts; i++) {
        if (SkinPart_GetPartId(pChar->pSkin, i) == uShadow) {
            // port: EA passes an argument SkinPart_DrawPart ignores
            ((void (*)(Skin*, int, int))SkinPart_DrawPart)(pChar->pSkin, i, pChar->nView);
        }
    }
    SkinPart_EndDraw(pChar->pSkin);
    if (pChar->pClubSet != NULL) {
        pSkin = pChar->pClubSet->apSkins[pChar->nClubClass];
        if (pSkin != NULL) {
            SkinPart_BeginDraw(pSkin, pChar->nView);
            // port: EA passes an argument SkinPart_DrawPart ignores
            ((void (*)(Skin*, int, int))SkinPart_DrawPart)(pSkin, SkinPart_FindPart(pSkin, uShadow),
                                                     pChar->nView);
            SkinPart_EndDraw(pSkin);
        }
    }
}

// Draws every part but "shadow" of the character's skin (override table nView), then its club's
// (SKN_DrawClubParts).
void SKN_DrawCharacterParts(Character* pChar) {
    u64 uShadow;
    s32 nParts;
    int i;

    SKA_PackName(&uShadow, "shadow");
    SkinPart_BeginDraw(pChar->pSkin, pChar->nView);
    nParts = SkinPart_GetNumParts(pChar->pSkin);
    for (i = 0; i < nParts; i++) {
        if (SkinPart_GetPartId(pChar->pSkin, i) != uShadow) {
            // port: EA passes an argument SkinPart_DrawPart ignores
            ((void (*)(Skin*, int, int))SkinPart_DrawPart)(pChar->pSkin, i, pChar->nView);
        }
    }
    SkinPart_EndDraw(pChar->pSkin);
    SKN_DrawClubParts(pChar);
}

// Draws every part but "shadow" of the skin of the character's current club (Character.pClubSet, per
// club class), with override table nView; nothing without one.
void SKN_DrawClubParts(Character* pChar) {
    u64 uShadow;
    Skin* pSkin;
    s32 nShadow;
    int i;

    SKA_PackName(&uShadow, "shadow");
    if (pChar->pClubSet != NULL) {
        pSkin = pChar->pClubSet->apSkins[pChar->nClubClass];
        if (pSkin != NULL) {
            SkinPart_BeginDraw(pSkin, pChar->nView);
            nShadow = SkinPart_FindPartByName(pSkin, "shadow");
            for (i = 0; i < SkinPart_GetNumParts(pSkin); i++) {
                if (i != nShadow) {
                    // port: EA passes an argument SkinPart_DrawPart ignores
                    ((void (*)(Skin*, int, int))SkinPart_DrawPart)(pSkin, i, pChar->nView);
                }
            }
            SkinPart_EndDraw(pSkin);
        }
    }
}

// fake match: stands in for a function the original linker stripped. The file's pool has 0.0f
// (0x80283018) before the 0.5f SKN_DrawCharacter uses first; its body is unknown.
static f32 Skin_StrippedFn(f32 x) {
    return (x < 0.0f) ? -x : x;
}

// Draws the character's skin and its club's. uFlags bit 2: only their "shadow" parts, with shader
// type 10's parameters set to 128 grey (shadow.c). Otherwise every other part, lit: bit 4 picks
// light set 3; without it light set 0 with the character's lighting entry for this course
// (Character.p44, SKN_GetLightCourse) and fn_80093824. The lights' brightness is 0.5 plus half the
// ground's light under the character (fn_8004B78C). Clip mode 1 either way.
void SKN_DrawCharacter(Character* pChar, u32 uFlags) {
    static f32 aShadowParams[4] = { 128.0f, 128.0f, 128.0f, 128.0f };
    f32 vPos[3];
    f32 aParams[4];
    CharEntry44* pEntry;
    u32 uSet3;
    int nMode;
    u32 uShadow;

    if (pChar->p44 != NULL) {
        pEntry = &pChar->p44[SKN_GetLightCourse()];
    } else {
        pEntry = NULL;
    }
    uShadow = uFlags & 2;
    if (!uShadow) {
        uSet3 = uFlags & 4;
        if (uSet3) {
            LF_vSetCurrentLightFogEnvironment(3);
        } else {
            LF_vSetCurrentLightFogEnvironment(0);
            if (pEntry != NULL) {
                // port: Character.p44's entries are what lighting.h calls LightParams
                fn_80093854((LightParams*)pEntry);
            }
        }
        if (!uSet3) {
            fn_80093824();
        }
        SKN_GetCharPosition(pChar, vPos);
        LF_SetCurrentBrightness(0.5f * fn_8004B78C(Ter_GetTGD(), vPos) + 0.5f);
        LF_LoadCurrentLights();
        fn_80035308();
    }
    fn_800352E4();
    LI_SetObjectLights(NULL);
    if (uShadow) {
        nMode = 2;
    } else if (pChar->n1654 != 1) {
        nMode = 1;
    } else {
        nMode = 2;
    }
    if (uShadow) {
        LLMath_CopyVec(aShadowParams, aParams);
        switch (nMode) {
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
        SD_SetShaderTypeParameters(10, aParams);
        SKN_DrawShadowParts(pChar);
    } else {
        switch (nMode) {
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
        SKN_DrawCharacterParts(pChar);
    }
    LI_ResetLights();
}

// The course whose lighting entry (Character.p44) SKN_DrawCharacter lights a character with: the
// current course, except on course 7 (the target games), where the hole (from 0) picks: 0 itself,
// 1-7 courses 22, 23, 13, 9, 16, 20 and 8, 15-17 course 24, any other 13.
int SKN_GetLightCourse(void) {
    int nCourse;

    nCourse = Game_GetCourse();
    if (nCourse == 7) {
        switch (Game_GetCurHoleNum()) {
        case 0:
            return nCourse;
        case 1:
            return 0x16;
        case 2:
            return 0x17;
        case 3:
            return 0xD;
        case 4:
            return 9;
        case 5:
            return 0x10;
        case 6:
            return 0x14;
        case 7:
            return 8;
        case 15:
        case 16:
        case 17:
            return 0x18;
        default:
            return 0xD;
        }
    }
    return nCourse;
}

// Poses the character's skin for this frame: blends its changed morph targets (single view or game
// type 3 only), builds its matrices from the model's and points its drawn meshes at them (override
// table nView); then the same for its current club's skin, from the grip bone (0x52) on. Sets n1698
// to 1 (posed), except for player 1000. n: every caller passes 0; unused.
void SKN_PoseCharacter(Character* pChar, int n) {
    Skin* pClub;

    if (gSession.nSplitScreen == 0 || gSession.nGameType == 3) {
        fn_8011CB5C(pChar->pSkin, pChar->nView);
    }
    SKN_BuildMatrices(pChar->pSkin, pChar->pModel, 0, 0, pChar->nView);
    SKN_SetMeshMatrices(pChar->pSkin, pChar->nView);
    // port: EA passes arguments SkinPart_UpdateSkin ignores
    ((void (*)(Skin*, int))SkinPart_UpdateSkin)(pChar->pSkin, pChar->nView);
    if (pChar->pClubSet != NULL) {
        pClub = pChar->pClubSet->apSkins[pChar->nClubClass];
        if (pClub != NULL) {
            SKN_BuildMatrices(pClub, pChar->pModel, CharModel_GetBoneIndex(pChar->pModel, 0x52) - 0x52, 0x52,
                        pChar->nView);
            SKN_SetMeshMatrices(pClub, pChar->nView);
        }
    }
    pChar->n1698 = 1;
    if (pChar->nPlayer == 1000) {
        pChar->n1698 = 0;
    }
}

// Sets up SKN_DrawBoneTri's mesh objects, one per view: shader row 0x13, room for 3 vertices and 1
// draw.
void SKN_InitTris(void) {
    DynRenderSize size;
    int i;

    size.nMaxVerts = 3;
    size.nMaxDraws = 1;
    for (i = 0; i < 2; i++) {
        SD_InitShaderObject(&gSkinBoneTris.aMesh[i], 0x13, &size);
    }
}

// Frees SKN_DrawBoneTri's mesh objects.
void SKN_FreeTris(void) {
    int i;

    for (i = 0; i < 2; i++) {
        SD_FreeShaderObject(&gSkinBoneTris.aMesh[i]);
    }
}

// Draws a grey triangle for view nView (0 or 1) through the positions of bones 1 and 7 of the
// character's model, bone 7 twice (so it covers no area); nothing when the model has fewer than 8
// bones.
void SKN_DrawBoneTri(Character* pChar, int nView) {
    f32* apPos[3];
    DynRenderFill fill;
    CharModel* pModel;
    int i;

    if (nView < 0 || nView >= 2) {
        return;
    }
    pModel = pChar->pModel;
    if (pModel->nBones <= 1 || pModel->nBones <= 7) {
        return;
    }
    apPos[0] = pModel->pMatrices[1][3];
    apPos[1] = pModel->pMatrices[7][3];
    apPos[2] = pModel->pMatrices[7][3];
    for (i = 0; i < 3; i++) {
        gSkinBoneTris.aPos[nView][i][0] = apPos[i][0];
        gSkinBoneTris.aPos[nView][i][1] = apPos[i][1];
        gSkinBoneTris.aPos[nView][i][2] = apPos[i][2];
        gSkinBoneTris.aUV[nView][i][0] = 0.0f;
        gSkinBoneTris.aUV[nView][i][1] = 0.0f;
        gSkinBoneTris.aColor[nView][i][0] = 0x80;
        gSkinBoneTris.aColor[nView][i][1] = 0x80;
        gSkinBoneTris.aColor[nView][i][2] = 0x80;
        gSkinBoneTris.aColor[nView][i][3] = 0x80;
        gSkinBoneTris.aIndex[nView][i] = i;
    }
    fill.nCount = 3;
    fill.nVerts = 3;
    fill.pDraws = NULL;
    fill.pIndices = gSkinBoneTris.aIndex[nView];
    fill.pPos = gSkinBoneTris.aPos[nView];
    fill.pColour = gSkinBoneTris.aColor[nView];
    fill.pTexCoord = gSkinBoneTris.aUV[nView];
    RenderState_SetDrawFlags(0);
    RenderState_SetClipMode(0);
    RenderState_Flush();
    SD_FillShaderObject(&gSkinBoneTris.aMesh[nView], &fill, 1);
    SD_DrawShaderObject(&gSkinBoneTris.aMesh[nView]);
}

// SKN_DrawBoneTri for view nView on every character made so far that is not a golfer
// (Character_IsGolfer) and has neither bit 0x40 nor bit 1 of u10 set.
void SKN_DrawBoneTris(int nView) {
    int i;

    for (i = 0; i < gNumCharacters; i++) {
        if (!Character_IsGolfer(gCharacters[i]) && !(gCharacters[i]->u10 & 0x41)) {
            SKN_DrawBoneTri(gCharacters[i], nView);
        }
    }
}

// ---- sweep code (tidied) ----

void RC_ApplyCurrentViewport(void) {
    RC_ApplyViewport(RC_spGetCurrentRenderCtx());
}

// Hands the render context's viewport (left, top, width, height) to fn_80016978, which makes it the
// current one.
void RC_ApplyViewport(void* pCamera) {
    f32* pRect;

    pRect = RC_spGetRenderCtxViewport(pCamera);
    fn_80016978(VM_fGetViewportLeft(pRect), VM_fGetViewportTop(pRect), VM_fGetViewportWidth(pRect),
                VM_fGetViewportHeight(pRect));
}

// Lighting off after a lit draw: colour channel 4 unlit with a grey ambient colour (fn_8006ED70).
void LI_ResetLights(void) {
    fn_8006ED70();
}

// Loads the lights for drawing pObj, in its space (NULL: in world space; fn_8006EADC).
void LI_SetObjectLights(UObject* pObj) {
    fn_8006EADC(pObj);
}

// Loads the current light set's lights.
void LF_LoadCurrentLights(void) {
    LI_LoadLightGroup(&LF_spGetCurrentLightFogEnvironment()->group);
}

// Sets the current light set's brightness: the factor its lights' colours are scaled by
// (LightGroup.v18[0], 1 by default).
void LF_SetCurrentBrightness(f32 f) {
    LF_spGetCurrentLightFogEnvironment()->group.v18[0] = f;
}

// Sets up a shader object of row nRow of the shader table (lbl_80188E88): takes the row's hooks and
// calls its init hook with pDesc (for row 0x13, a DynRenderSize).
void SD_InitShaderObject(ShaderObject* pObj, int nRow, const void* pDesc) {
    pObj->nRow = nRow;
    pObj->pHooks = &lbl_80188E88[nRow].shader;
    pObj->pHooks->pfnInit(pObj, pDesc);
}

// Calls the object's free hook, when its row has one.
void SD_FreeShaderObject(ShaderObject* pObj) {
    if (pObj->pHooks->pfnFree != NULL) {
        pObj->pHooks->pfnFree(pObj);
    }
}

// Calls the object's draw hook.
void SD_DrawShaderObject(ShaderObject* pObj) {
    pObj->pHooks->pfnDraw(pObj);
}

// Hands the object what to draw through its row's fill hook; what pData and n are depends on the
// row (row 0x13: a DynRenderFill and 1).
void SD_FillShaderObject(ShaderObject* pObj, const void* pData, int n) {
    pObj->pHooks->pfnFill(pObj, pData, n);
}

// Loads the group's lights (fn_8006E7A4): the directional light becomes the ambient colour, the
// point lights fill the point slots.
void LI_LoadLightGroup(LightGroup* pGroup) {
    fn_8006E7A4(pGroup);
}

// Copies the character's position (its root bone's, Bone.v1C) to pOut; nothing without a character.
void SKN_GetCharPosition(Character* pChar, f32* pOut) {
    if (pChar != NULL) {
        LLMath_CopyVec(pChar->pModel->pBones[0].v1C, pOut);
    }
}

// Blends two format 1 poses' morph weights into pOut, fWeight of the way from pA to pB (an
// animation blend node): in each of the three blocks, each of the 20 morphs set in either pose is
// blended and set in pOut (one set in only one pose blends with the other's value as it stands);
// then both inputs' bits are cleared.
void SKN_BlendMorphWeights(SkelPose1* pA, SkelPose1* pB, SkelPose1* pOut, f32 fWeight) {
    u32 aBits[4];   // only 20 bits are used; the size is not known
    int i;
    int j;
    SkelPoseBlock* pBlockA;
    SkelPoseBlock* pBlockB;
    SkelPoseBlock* pBlockOut;

    for (i = 0; i < 3; i++) {
        pBlockA = &pA->aBlocks[i];
        pBlockB = &pB->aBlocks[i];
        pBlockOut = &pOut->aBlocks[i];
        fn_80021980(pBlockA->aBits, pBlockB->aBits, aBits, 20);
        BitArray_CopyArray(aBits, pBlockOut->aBits, 20);
        for (j = 0; j < 20; j++) {
            if (BitArray_TestBit(aBits, j)) {
                pBlockOut->af8[j] = fWeight * (pBlockB->af8[j] - pBlockA->af8[j]) + pBlockA->af8[j];
            }
        }
        BitArray_ClearArray(pBlockA->aBits, 20);
        BitArray_ClearArray(pBlockB->aBits, 20);
    }
}

// Byte-swaps nEntries SkinModel44 entries (the meshes the model draws), in place.
void SKN_SwapMeshEntries(SkinModel44* pEntries, s32 nEntries) {
    SwapField aFormat[5] = { { 4, 4 }, { 4, 4 }, { 2, 2 }, { 2, 2 }, { 4, 2 } };
    void* pSrc;
    void* pDst;
    int i;
    SkinModel44* pEntry = pEntries;

    for (i = 0; i < nEntries; i++) {
        pSrc = pDst = pEntry;
        ByteSwap_Records(&pSrc, &pDst, aFormat, 5, 1);
        pEntry++;
    }
}

// Byte-swaps each of nEntries SkinModel44 entries' p4 array (n8 words), in place; p4 must already
// be a pointer.
void SKN_SwapMeshEntryLists(SkinModel44* pEntries, s32 nEntries) {
    u8* pData;
    u8* pSrc;
    int i;

    for (i = 0; i < nEntries; i++) {
        pData = (u8*)pEntries->p4;
        pSrc = pData;
        BYTESWAP_SWAPDATA(&pSrc, pData, pEntries->n8 * 4, 4);
        pEntries++;
    }
}

// Byte-swaps nEntries SkinModel54 entries (the blended matrices' recipes), in place.
void SKN_SwapMatrixBlends(SkinModel54* pEntries, s32 nEntries) {
    SwapField aFormat[3] = { { 2, 2 }, { 6, 2 }, { 12, 4 } };
    void* pSrc;
    void* pDst;
    int i;
    SkinModel54* pEntry = pEntries;

    for (i = 0; i < nEntries; i++) {
        pSrc = pDst = pEntry;
        ByteSwap_Records(&pSrc, &pDst, aFormat, 3, 1);
        pEntry++;
    }
}

// Empty in this build: gSkinFrameBuf is never allocated. n: 1800, or 3600 in split screen.
void SKN_InitModule(int n) {
}

// Frees gSkinFrameBuf, when set, and clears it and its two counts.
void SKN_CloseModule(void) {
    if (gSkinFrameBuf != NULL) {
        StaticMem_Free(gSkinFrameBuf);
    }
    gSkinFrameBuf = NULL;
    gSkinFrameBufSize = 0;
    gSkinFrameBufUsed = 0;
}

// Clears gSkinFrameBufUsed; called each frame before the characters are posed.
void SKN_BeginFrame(void) {
    gSkinFrameBufUsed = 0;
}

// Makes a model copied from its file usable, once (bit 31 of u30 marks it done): its offsets become
// pointers, its mesh entries and matrix blends are byte-swapped. A model that is not version 4
// (n00) is cleared to an empty version 4 one instead.
void SKN_FixupModel(SkinModel* pModel) {
    int i;

    if (pModel->u30 & 0x80000000) {
        return;
    }
    if (pModel->n00 != 4) {
        memset(pModel, 0, sizeof(SkinModel));
        pModel->n00 = 4;
        pModel->n04 = 0;
        pModel->n18 = pModel->n1A = pModel->n1C = pModel->n1E = -1;
        pModel->n20 = pModel->n24 = -1;
        pModel->n28 = pModel->n2C = -1;
        pModel->u30 = 0x80000000;
        return;
    }
    if (pModel->p34 != NULL) {
        pModel->p34 = (BonePose*)((u8*)pModel + (uptr)pModel->p34);
    }
    if (pModel->p38 != NULL) {
        pModel->p38 = (u8*)pModel + (uptr)pModel->p38;
    }
    if (pModel->p3C != NULL) {
        pModel->p3C = (u8*)pModel + (uptr)pModel->p3C;
    }
    if (pModel->p44 != NULL) {
        pModel->p44 = (SkinModel44*)((u8*)pModel + (uptr)pModel->p44);
    }
    if (pModel->pDesc != NULL) {
        pModel->pDesc = (SkinDesc*)((u8*)pModel + (uptr)pModel->pDesc);
    }
    if (pModel->p54 != NULL) {
        pModel->p54 = (SkinModel54*)((u8*)pModel + (uptr)pModel->p54);
    }
    SKN_SwapMeshEntries(pModel->p44, pModel->n40);
    SKN_SwapMatrixBlends(pModel->p54, pModel->n50);
    for (i = 0; i < pModel->n40; i++) {
        if (pModel->p44[i].p4 != NULL) {
            pModel->p44[i].p4 = (s32*)((u8*)pModel + (uptr)pModel->p44[i].p4);
        }
        // EA bug: swaps every entry's array once per entry, and reaches entries whose p4 is
        // still an offset; right only for a single entry.
        SKN_SwapMeshEntryLists(pModel->p44, pModel->n40);
    }
    pModel->u30 = pModel->u30 | 0x80000000;
}

// Builds the skin's matrices (p108C) for this frame; nothing without its render data (bit 2 of
// uFlags). The first SkinModel.n14, one per bone, are the character model's skinning matrices
// (p768), from nFirst on, each taken nSkip further along there (not copied when p108C is p768).
// Each later one whose bit is set in aMtxBits (SkinPart_UpdateMarks: those the chosen meshes use) is a
// weighted sum of up to three others (SkinModel.p54). nView: both callers pass the character's
// nView; unused.
void SKN_BuildMatrices(Skin* pSkin, CharModel* pCharModel, int nSkip, int nFirst, int nView) {
    int j;
    SkinModel54* pEntry;
    s32 nMatrices;
    f32 (*pDst)[4];
    f32 (*pSrc)[4];
    f32 fWeight;
    int nBones;
    int i;
    int k;
    f32 (*aMtx)[4][4];

    if (pSkin->pModel == NULL) {
        return;
    }
    nMatrices = pSkin->pModel->n50;
    if (!(pSkin->uFlags & 2)) {
        return;
    }
    SkinPart_UpdateMarks(pSkin);
    if (pSkin->p108C != pCharModel->p768) {
        Mem_cpy(pSkin->p108C[nFirst], pCharModel->p768[nFirst + nSkip],
                (pSkin->pModel->n14 - nFirst) * sizeof(*pSkin->p108C));
    }
    aMtx = pSkin->p108C;
    for (i = pSkin->pModel->n14; i < nMatrices; i++) {
        if (BitArray_TestBit(pSkin->aMtxBits, i)) {
            pEntry = &pSkin->pModel->p54[i];
            nBones = pEntry->nBones;
            memset(aMtx[i], 0, sizeof(aMtx[i]));
            for (j = 0; j < nBones; j++) {
                pDst = aMtx[i];
                fWeight = pEntry->afWeights[j];
                for (k = 0, pSrc = aMtx[pEntry->aBones[j]]; k < 4; k++) {
                    LLMath_AddScale(pDst[k], pSrc[k], fWeight, pDst[k]);
                }
            }
        }
    }
}

// Points the override (in table n, Skin.a10A0[n]) of each mesh the skin draws (bit set in aMeshBits;
// the mesh is SkinModel44.n0) at the skin's matrices, p108C; nothing without the table or render
// data.
void SKN_SetMeshMatrices(Skin* pSkin, int n) {
    int i;
    s32 nEntries;
    HwsOverrideTable* pTable;

    if (pSkin->a10A0[n] != NULL && (pSkin->uFlags & 2)) {
        pTable = pSkin->a10A0[n];
        nEntries = pSkin->pModel->n40;
        for (i = 0; i < nEntries; i++) {
            if (BitArray_TestBit(pSkin->aMeshBits, i)) {
                fn_80112B18(pTable, pSkin->pModel->p44[i].n0, pSkin->p108C);
            }
        }
    }
}

// Byte-swaps a model file's header (the 0x140-byte SkinModel), in place.
void SKN_SwapModelHeader(SkinModel* pModel) {
    SwapField aFormat[26] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 2, 2 },
                              { 2, 2 }, { 2, 2 }, { 2, 2 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 8, 1 }, { 224, 4 } };
    void* pSrc;
    void* pDst;

    pDst = pSrc = pModel;
    ByteSwap_Records(&pSrc, &pDst, aFormat, 26, 1);
}

// Byte-swaps a skin description's header (the 0x120-byte SkinDesc), in place.
void SKN_SwapDescHeader(SkinDesc* pDesc) {
    SwapField aFormat[48] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 100, 4 } };
    void* pSrc;
    void* pDst;

    pDst = pSrc = pDesc;
    ByteSwap_Records(&pSrc, &pDst, aFormat, 48, 1);
}

// fake match: EA takes the mesh through an inline; &pDesc->p34[j] in place allocates pMesh r19, not r18
static inline SkinMesh* fn_800368FC_Read(SkinDesc* pDesc, int j) {
    return &pDesc->p34[j];
}

// Byte-swaps a skin description's arrays (its header is SKN_SwapDescHeader's), in place, and turns
// the offsets of its p28 entries' data and its meshes' bit data into pointers. Version 8
// descriptions with n04 0 have the old p14 layout (SkinDesc14Old) and are converted to the new one
// (n04 becomes 1).
void SKN_SwapDesc(SkinDesc* pDesc) {
    SwapField aDesc14[10] = { { 8, -8 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 2, 2 }, { 2, 2 }, { 4, 4 },
                              { 4, 4 }, { 8, 4 }, { 8, 4 } };
    SwapField aOld14[9] = { { 8, -8 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                            { 8, 4 }, { 8, 4 } };
    SwapField aDesc18[3] = { { 4, 4 }, { 4, 4 }, { 16, 4 } };
    SwapField aDesc28[5] = { { 4, 4 }, { 4, 4 }, { 8, 1 }, { 4, 4 }, { 4, 4 } };
    SwapField aMesh[4] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    SwapField aDesc44[12] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    SwapField aPart[3] = { { 8, -8 }, { 4, 4 }, { 4, 4 } };
    SwapField aVariant[5] = { { 8, -8 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    SwapField aDesc74[5] = { { 8, -8 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    SwapField aDesc7C[4] = { { 8, -8 }, { 4, 4 }, { 4, 4 }, { 4, 4 } };
    SwapField aDesc8C[5] = { { 8, -8 }, { 12, 4 }, { 12, 4 }, { 12, 4 }, { 4, 4 } };
    SwapField aLink[2] = { { 8, -8 }, { 4, 4 } };
    SwapField aDesc94[3] = { { 64, 4 }, { 12, 4 }, { 4, 4 } };
    SwapField aDescB8[2] = { { 8, 4 }, { 8, 4 } };
    SkinDesc14Old old;
    SkinDesc14* pEntry;
    SkinMesh* pMesh;
    u8* pData;
    int i;
    int j;
    u8* p;
    void* pSrc;                 // port: BYTESWAP_SWAPDATA takes it as a u8** too (one stack slot in EA's code)
    void* pDst;
    s32 n;

    if (pDesc->nVersion > 8 || (pDesc->nVersion == 8 && pDesc->n04 == 1)) {
        // fake match: this walk alone uses p and j, every other one pData and i. With pData and i here
        // its registers swap; the mesh-bits loop needs a counter set before it (i) for EA's `mr` start
        p = (u8*)pDesc->p14;
        for (j = 0; j < pDesc->n10; j++) {
            pSrc = pDst = p;
            ByteSwap_Records(&pSrc, &pDst, aDesc14, 10, 1);
            p += sizeof(SkinDesc14);
        }
    } else if (pDesc->nVersion == 8 && pDesc->n04 == 0) {
        pData = (u8*)pDesc->p14;
        if (pData != NULL) {
            for (i = 0; i < pDesc->n10; i++) {
                pSrc = pDst = pData;
                ByteSwap_Records(&pSrc, &pDst, aOld14, 9, 1);
                pData += sizeof(SkinDesc14);
            }
            for (i = 0; i < pDesc->n10; i++) {
                pEntry = &pDesc->p14[i];
                memcpy(&old, pEntry, sizeof(SkinDesc14Old));
                pEntry->uId = old.uId;
                pEntry->u08 = old.u08;
                pEntry->u0C = old.u0C;
                pEntry->u10 = old.u14;
                pEntry->n14 = old.n10;
                pEntry->n16 = old.n18;
                pEntry->n18 = -1;
                pEntry->n1C = old.n1C;
                pEntry->a20 = old.a20;
            }
            pDesc->n04 = 1;
        }
    }

    pData = (u8*)pDesc->p18;
    if (pData != NULL) {
        for (i = 0; i < pDesc->n10; i++) {
            pSrc = pDst = pData;
            ByteSwap_Records(&pSrc, &pDst, aDesc18, 3, 1);
            pData += sizeof(SkinDesc18);
        }
    }

    pData = (u8*)pDesc->p20;
    if (pData != NULL && pDesc->n1C != 0) {
        pSrc = pDst = pData;
        BYTESWAP_SWAPDATA((u8**)&pSrc, pData, pDesc->n1C * 4, 4);
    }

    pData = (u8*)pDesc->p28;
    if (pData != NULL && pDesc->n24 != 0) {
        for (i = 0; i < pDesc->n24; i++) {
            pSrc = pDst = pData;
            ByteSwap_Records(&pSrc, &pDst, aDesc28, 5, 1);
            pData += sizeof(SkinDesc28);
        }
        for (i = 0; i < pDesc->n24; i++) {
            if (pDesc->p28[i].p10 != NULL) {
                pDesc->p28[i].p10 = (u8*)pDesc + (uptr)pDesc->p28[i].p10;
            }
        }
    }

    pData = (u8*)pDesc->p34;
    if (pData != NULL && pDesc->n2C != 0) {
        for (i = 0; i < pDesc->n2C; i++) {
            pSrc = pDst = pData;
            ByteSwap_Records(&pSrc, &pDst, aMesh, 4, 1);
            pData += sizeof(SkinMesh);
        }
    }

    // Each mesh's bit data: its layout depends on the mesh's flags.
    for (i = 0; i < pDesc->n2C; i++) {
        if (pDesc->p34[i].pBits != NULL) {
            pDesc->p34[i].pBits = (SkinMeshBit*)((u8*)pDesc + (uptr)pDesc->p34[i].pBits);
        }
        pMesh = fn_800368FC_Read(pDesc, i);
        pData = (u8*)pMesh->pBits;
        n = pMesh->n8;
        if (pData == NULL) {
            pMesh->pBits = NULL;
        } else if (pMesh->uFlags & 0x30) {
            pSrc = pDst = pData;
            // fake match: n * 8 as (n * 4) << 1 computes n * 4 once, before the first swap, as EA does
            BYTESWAP_SWAPDATA((u8**)&pSrc, pData, (n * 4) << 1, 2);
            pData += (n * 4) << 1;
            pSrc = pData;
            BYTESWAP_SWAPDATA((u8**)&pSrc, pData, n * 4, 1);
            pData += n * 4;
            if (pMesh->uFlags & 0x1000) {
                pSrc = pData;
                BYTESWAP_SWAPDATA((u8**)&pSrc, pData, n * 2, 2);
            } else if (pMesh->uFlags & 0x40) {
                pSrc = pData;
                // fake match: written n * sizeof(u32), the reused n * 4 is passed after &pSrc, as EA does
                BYTESWAP_SWAPDATA((u8**)&pSrc, pData, n * sizeof(u32), 2);
            }
        } else if (pMesh->uFlags & 0x40) {
            pSrc = pDst = pData;
            BYTESWAP_SWAPDATA((u8**)&pSrc, pData, n * 4, 2);
        } else if (pMesh->uFlags & 0x1808) {
            pSrc = pData;
            BYTESWAP_SWAPDATA((u8**)&pSrc, pData, n * 2, 2);
        } else if (pMesh->uFlags & 0x300004) {
            pSrc = pData;
            BYTESWAP_SWAPDATA((u8**)&pSrc, pData, n * 64, 4);
        } else if (pMesh->uFlags & 2) {
            pSrc = pData;
            BYTESWAP_SWAPDATA((u8**)&pSrc, pData, n * 2, 2);
        }
    }

    pData = (u8*)pDesc->p3C;
    if (pData != NULL && pDesc->n38 != 0) {
        pSrc = pDst = pData;
        BYTESWAP_SWAPDATA((u8**)&pSrc, pData, pDesc->n38 * 4, 4);
    }

    pData = (u8*)pDesc->p44;
    for (i = 0; i < pDesc->n40; i++) {
        pSrc = pDst = pData;
        ByteSwap_Records(&pSrc, &pDst, aDesc44, 12, 1);
        pData += sizeof(SkinDesc44);
    }

    pData = (u8*)pDesc->pParts;
    if (pData != NULL && pDesc->nParts != 0) {
        for (i = 0; i < pDesc->nParts; i++) {
            pSrc = pDst = pData;
            ByteSwap_Records(&pSrc, &pDst, aPart, 3, 1);
            pData += sizeof(SkinPartDef);
        }
    }

    pData = (u8*)pDesc->pVariants;
    if (pData != NULL && pDesc->nVariants != 0) {
        for (i = 0; i < pDesc->nVariants; i++) {
            pSrc = pDst = pData;
            ByteSwap_Records(&pSrc, &pDst, aVariant, 5, 1);
            pData += sizeof(SkinVariant);
        }
    }

    pData = (u8*)pDesc->p5C;
    if (pData != NULL) {
        if (pDesc->n58 != 0 && pData != NULL && pDesc->n58 != 0) {
            pSrc = pDst = pData;
            BYTESWAP_SWAPDATA((u8**)&pSrc, pData, pDesc->n58 * 8, 4);
        }
    }

    pData = (u8*)pDesc->pLinks;
    if (pData != NULL && pDesc->nLinks != 0) {
        for (i = 0; i < pDesc->nLinks; i++) {
            pSrc = pDst = pData;
            ByteSwap_Records(&pSrc, &pDst, aLink, 2, 1);
            pData += sizeof(SkinLink);
        }
    }

    pData = (u8*)pDesc->p6C;
    if (pData != NULL && pDesc->n68 != 0) {
        pSrc = pDst = pData;
        BYTESWAP_SWAPDATA((u8**)&pSrc, pData, pDesc->n68 * 4, 4);
    }

    pData = (u8*)pDesc->p74;
    if (pData != NULL && pDesc->n70 != 0) {
        for (i = 0; i < pDesc->n70; i++) {
            pSrc = pDst = pData;
            ByteSwap_Records(&pSrc, &pDst, aDesc74, 5, 1);
            pData += sizeof(SkinDesc74);
        }
    }

    pData = (u8*)pDesc->p7C;
    if (pData != NULL && pDesc->n78 != 0) {
        for (i = 0; i < pDesc->n78; i++) {
            pSrc = pDst = pData;
            ByteSwap_Records(&pSrc, &pDst, aDesc7C, 4, 1);
            pData += sizeof(SkinDesc7C);
        }
    }

    pData = (u8*)pDesc->p8C;
    if (pData != NULL && pDesc->n88 != 0) {
        for (i = 0; i < pDesc->n88; i++) {
            pSrc = pDst = pData;
            ByteSwap_Records(&pSrc, &pDst, aDesc8C, 5, 1);
            pData += sizeof(SkinDesc8C);
        }
    }

    pData = pDesc->p94;
    if (pData != NULL && pDesc->n90 != 0) {
        for (i = 0; i < pDesc->n90; i++) {
            pSrc = pDst = pData;
            ByteSwap_Records(&pSrc, &pDst, aDesc94, 3, 1);
            pData += 0x50;
        }
    }

    pData = (u8*)pDesc->pA4;
    if (pData != NULL && pDesc->nA0 != 0) {
        pSrc = pDst = pData;
        BYTESWAP_SWAPDATA((u8**)&pSrc, pData, pDesc->nA0 * 2, 2);
    }

    pData = (u8*)pDesc->pAC;
    if (pData != NULL && pDesc->nA8 != 0) {
        pSrc = pDst = pData;
        BYTESWAP_SWAPDATA((u8**)&pSrc, pData, pDesc->nA8 * 2, 2);
    }

    pData = (u8*)pDesc->pB8;
    if (pData != NULL && pDesc->nB4 != 0) {
        for (i = 0; i < pDesc->nB4; i++) {
            pSrc = pDst = pData;
            ByteSwap_Records(&pSrc, &pDst, aDescB8, 2, 1);
            pData += sizeof(SkinDescB8);
        }
    }
}

// Byte-swaps the model's bone poses.
void SKN_SwapBonePoses(BonePose* pBones, s32 nBones) {
    u8* pSrc;

    pSrc = (u8*)pBones;
    BYTESWAP_SWAPDATA(&pSrc, (u8*)pBones, nBones * sizeof(BonePose), 4);
}

// Gives a skin what posing and drawing it needs (bit 2 of uFlags marks a skin that has it); 0 when
// it already had it, else 1: its matrices (p108C, SkinModel.n50 of them) and the aMtxBits and aMeshBits
// bit arrays, cleared; with a description, its morph memory (a1098[0]) and mesh override table
// (a10A0[0]), and uFlags bit 1 (choices changed). Every morph target is marked changed. b: also
// calls SkinPart_InitSkin (empty in this build).
s32 SKN_AllocRenderData(Skin* pSkin, u8 b) {
    SkinModel* pModel;
    SkinDesc* pDesc;
    s32 nSize;

    if (pSkin->uFlags & 2) {
        return 0;
    }
    pModel = pSkin->pModel;
    pDesc = pModel->pDesc;
    pSkin->p108C = StaticMem_Alloc(pModel->n50 * sizeof(*pSkin->p108C), 2, 0x80, "Skin.c", 0x500);
    pSkin->aMtxBits = StaticMem_Alloc((pModel->n50 + 31) / 32 * 4, 2, 0, "Skin.c", 0x50C);
    pSkin->aMeshBits = StaticMem_Alloc((pModel->n40 + 31) / 32 * 4, 2, 0, "Skin.c", 0x50D);
    BitArray_ClearArray(pSkin->aMtxBits, pModel->n50);
    BitArray_ClearArray(pSkin->aMeshBits, pModel->n40);
    if (pDesc != NULL) {
        SkinPart_GetMaxOptionsSize(pSkin);
        nSize = fn_8011CDE8(pSkin);
        if (nSize != 0) {
            pSkin->a1098[0] = fn_801128C8(pModel->pDesc, nSize);
        }
        pSkin->a10A0[0] = fn_80112A10(pModel->pDesc, 0);
        pSkin->uFlags = 1;
        if (b) {
            // port: EA passes an argument SkinPart_InitSkin ignores
            ((void (*)(Skin*))SkinPart_InitSkin)(pSkin);
        }
    }
    fn_8011CE58(pSkin);
    pSkin->uFlags = pSkin->uFlags | 2;
    return 1;
}

// Frees what SKN_AllocRenderData gave the skin, after waiting for the GPU to finish drawing; 0 when
// it had none, else 1. With a description, also p1090 and the description's own mesh bit data
// (fn_80037D5C).
s32 SKN_FreeRenderData(Skin* pSkin) {
    SkinModel* pModel;

    if (!(pSkin->uFlags & 2)) {
        return 0;
    }
    fn_80008380();
    pModel = pSkin->pModel;
    if (pModel != NULL && pModel->pDesc != NULL) {
        fn_80112910(pSkin->p1090);
        pSkin->p1090 = NULL;
        fn_80112910(pSkin->a1098[0]);
        pSkin->a1098[0] = NULL;
        fn_80112A58(pSkin->a10A0[0]);
        pSkin->a10A0[0] = NULL;
        fn_80037D5C(pSkin->pModel->pDesc);
    }
    // port: EA passes an argument SkinPart_ShutdownSkin ignores
    ((void (*)(Skin*))SkinPart_ShutdownSkin)(pSkin);
    if (pSkin->p108C != NULL) {
        StaticMem_Free(pSkin->p108C);
    }
    pSkin->p108C = NULL;
    if (pSkin->aMtxBits != NULL) {
        StaticMem_Free(pSkin->aMtxBits);
    }
    pSkin->aMtxBits = NULL;
    if (pSkin->aMeshBits != NULL) {
        StaticMem_Free(pSkin->aMeshBits);
    }
    pSkin->aMeshBits = NULL;
    pSkin->uFlags = pSkin->uFlags & ~2;
    return 1;
}

// ---- end of sweep code ----

// Makes a skin from its file (pData: a SkinModel; version 4 carries a SkinDesc at pDesc's offset).
// The file's headers are byte-swapped in place the first time (u30 flags 0x40000002 mark it). The
// model and its description are copied into memory of their own (b: at the high end of the heap,
// allocation mode 1 instead of 2) and made usable; the pose starts at the model's bone poses; the
// skin gets its part choices, morph state and render data (SKN_AllocRenderData). SkinPart's
// change-all-copies is on meanwhile.
Skin* SKN_Create(u8* pData, u8 b) {
    SkinModel* pModel;
    Skin* pSkin;
    SkinDesc* pDesc;
    BonePose* pBones;
    s32 nDescSize;
    s32 nSize;
    u8 bSwap;
    int i;
    u8 bOld;

    bOld = SkinPart_GetChangeAllCopies();
    SkinPart_SetChangeAllCopies(1);
    pSkin = StaticMem_Alloc(sizeof(Skin), 2, 0x80, "Skin.c", 0x5C4);
    memset(pSkin, 0, sizeof(Skin));
    pModel = (SkinModel*)pData;     // the file's model, then the copy
    if ((pModel->u30 & 0x40000002) != 0x40000002) {
        SKN_SwapModelHeader((SkinModel*)pData);
        bSwap = 1;
        ((SkinModel*)pData)->u30 = ((SkinModel*)pData)->u30 | 0x40000000 | 2;
    } else {
        bSwap = 0;
    }
    if (pModel->n00 == 4) {
        pDesc = pModel->pDesc;
        if (pDesc != NULL) {
            pDesc = (SkinDesc*)((u8*)pModel + (uptr)pDesc);
        }
        if (bSwap) {
            SKN_SwapDescHeader(pDesc);
        }
        nDescSize = (pDesc != NULL) ? pDesc->n08 : 0;
        nSize = pModel->n08 - nDescSize;
    } else {
        nSize = sizeof(SkinModel);
        nDescSize = 0;
    }
    if (b) {
        pSkin->pModel = StaticMem_Alloc(nSize, 1, 0x80, "Skin.c", 0x5EF);
    } else {
        pSkin->pModel = StaticMem_Alloc(nSize, 2, 0x80, "Skin.c", 0x5F3);
    }
    memcpy(pSkin->pModel, pModel, nSize);
    pModel = pSkin->pModel;
    SKN_FixupModel(pModel);
    pModel->n08 = nSize;
    if (nDescSize != 0) {
        if (b) {
            pModel->pDesc = StaticMem_Alloc(nDescSize, 1, 0x80, "Skin.c", 0x604);
        } else {
            pModel->pDesc = StaticMem_Alloc(nDescSize, 2, 0x80, "Skin.c", 0x608);
        }
        memcpy(pModel->pDesc, pDesc, nDescSize);
        SkinPart_FixupDesc(pModel->pDesc);
        SKN_SwapDesc(pModel->pDesc);
    } else {
        pModel->pDesc = NULL;
    }
    pDesc = pModel->pDesc;
    pSkin->bFootPoints = 0;
    pBones = pModel->p34;
    SKN_SwapBonePoses(pBones, pModel->n14);
    BitArray_FillArray(pSkin->pose.a0, 0x80);
    BitArray_FillArray(pSkin->pose.a10, 0x80);
    BitArray_FillArray(pSkin->pose.a20, 0x80);
    BitArray_FillArray(pSkin->pose.a30, 0x80);
    for (i = 0; i < pModel->n14; i++) {
        Quat_Copy(pBones[i].q0, pSkin->pose.aBones[i].q0);
        Quat_Copy(pBones[i].v10, pSkin->pose.aBones[i].v10);
    }
    pSkin->p1088 = StaticMem_Alloc(pModel->n14 * sizeof(*pSkin->p1088), 2, 0x80, "Skin.c", 0x62A);
    if (pDesc != NULL) {
        fn_801127A0(pDesc);
    }
    SkinPart_AllocChoices(pSkin);
    fn_8011C9B0(pSkin);
    SkinPart_SetChangeAllCopies(bOld);
    SKN_AllocRenderData(pSkin, 0);
    return pSkin;
}
