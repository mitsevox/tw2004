// Skin.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled; the
// sweep code below is the matched small functions.

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

void  fn_80008380(void);
void  fn_80016978(f32 fLeft, f32 fTop, f32 fWidth, f32 fHeight);
void  fn_8006E7A4(LightGroup* pGroup);         // GoLighting.c: load the group's lights
void  fn_8006EADC(UObject* pObj);              // GoLighting.c: light the object
void  fn_8006ED70(void);                       // GoLighting.c
void  fn_801127A0(void* pDesc);                // hwsMaterial_Gc.c
void  fn_801127C4(void* pDesc);                // hwsMaterial_Gc.c
void  fn_800CEE88(u8 b);                       // SkinPart.c
void  fn_800CEF04(SkinDesc* pDesc);            // SkinPart.c: offsets to pointers
void  fn_800CD404(Skin* pSkin);                // SkinPart.c
s32   fn_800CD700(Skin* pSkin);                // SkinPart.c
void  fn_800CE164(void);                       // SkinPart.c
void  fn_800CDA68(Skin* pSkin);                // SkinPart.c
void  fn_8011C9B0(Skin* pSkin);                // SkinMorph.c
s32   fn_8011CDE8(Skin* pSkin);                // SkinMorph.c
void  fn_8011CE58(Skin* pSkin);                // SkinMorph.c
HwsMemBlock* fn_801128C8(SkinDesc* pDesc, s32 nSize);         // hwsOverride_Gc.c
HwsOverrideTable* fn_80112A10(SkinDesc* pDesc, s32 nMeshes);  // hwsOverride_Gc.c
void  fn_80112B18(HwsOverrideTable* pTable, int i, void* p);   // hwsOverride_Gc.c

u8    fn_8001EC48(Character* pChar);           // char.c
void  fn_80035810(Character* pChar);
void  fn_80035D10(Character* pChar, int nView);
void  fn_80035F40(void* pCamera);
void  fn_80036054(ShaderObject* pObj, int nRow, const void* pDesc);
void  fn_800360A0(ShaderObject* pObj);
void  fn_800360D4(ShaderObject* pObj);
void  fn_8003612C(LightGroup* pGroup);
void  fn_8003614C(Character* pChar, f32* pOut);
int   fn_80035A9C(void);
void  fn_80035FBC(void);
void  fn_80035FDC(UObject* pObj);
void  fn_80035FFC(void);
void  fn_80036024(f32 f);
void  fn_80093824(void);                                     // goballfx.c
f32   fn_8004B78C(CourseInfo* pCourse, f32* pPos);           // GoTerrainCollision.c: the ground's light
void  fn_8003519C(int nRow, void* pData);                    // GoTerrain.c: calls row nRow's pfn8
void  fn_8011CB5C(Skin* pSkin, int nView);                   // SkinMorph.c
void  fn_800CE16C(void);                                     // SkinPart.c
void  fn_8003662C(Skin* pSkin, CharModel* pCharModel, int nSkip, int nFirst, int nView);
void  fn_80036790(Skin* pSkin, int n);
void  fn_80037AB8(Skin* pSkin, CharModel* pCharModel, int nSkip, int nFirst);
void  fn_800090A0(f32* pA, f32* pB, f32* pOut);                // Quaternion.c
void  fn_800090E4(f32* pQuat, f32* pIn, f32* pOut);            // Quaternion.c: pIn turned by pQuat
void  fn_8000914C(f32* pQ, f32 (*pMtx)[4]);                   // Quaternion.c: to a matrix
void  fn_8000A798(f32 (*pSrc)[4], f32 (*pDst)[4]);            // UMemPool.c: inverts a matrix
void  fn_80029EF4(u32* pSrc, u32* pDst, u32 nBits);          // Skeleton.c
void  fn_80036278(SkinModel44* pEntries, s32 nEntries);
void  fn_80036344(SkinModel44* pEntries, s32 nEntries);
void  fn_800363B4(SkinModel54* pEntries, s32 nEntries);
void  fn_800364AC(SkinModel* pModel);
void  fn_8003682C(SkinModel* pModel);
void  fn_80036894(SkinDesc* pDesc);
void  fn_800368FC(SkinDesc* pDesc);
void  fn_80037574(BonePose* pBones, s32 nBones);
void  fn_80037D5C(SkinDesc* pDesc);

// ---- sweep code (tidied) ----

void fn_80035F1C(void) {
    fn_80035F40(fn_8001614C());
}

// Hands the camera's screen rectangle on to fn_80016978.
void fn_80035F40(void* pCamera) {
    f32* pRect;

    pRect = fn_80012EF0(pCamera);
    fn_80016978(fn_80012EE8(pRect), fn_80012EE0(pRect), fn_80012ED8(pRect), fn_80012ED0(pRect));
}

// Picks the "shadow" part of the character's skin and of its club's skin, for n17B4.
void fn_80035640(Character* pChar) {
    u64 uShadow;
    s32 nParts;
    int i;
    Skin* pSkin;

    fn_800CB700(&uShadow, "shadow");
    fn_800CE02C(pChar->pSkin, pChar->n17B4);
    nParts = fn_800CCA40(pChar->pSkin);
    for (i = 0; i < nParts; i++) {
        if (fn_800CCDDC(pChar->pSkin, i) == uShadow) {
            // port: EA passes an argument fn_800CE0B0 ignores
            ((void (*)(Skin*, int, int))fn_800CE0B0)(pChar->pSkin, i, pChar->n17B4);
        }
    }
    fn_800CE128(pChar->pSkin);
    if (pChar->p16D8 != NULL) {
        pSkin = pChar->p16D8->apSkins[pChar->nClubClass];
        if (pSkin != NULL) {
            fn_800CE02C(pSkin, pChar->n17B4);
            // port: EA passes an argument fn_800CE0B0 ignores
            ((void (*)(Skin*, int, int))fn_800CE0B0)(pSkin, fn_800CDAFC(pSkin, uShadow),
                                                     pChar->n17B4);
            fn_800CE128(pSkin);
        }
    }
}

// Picks every part but "shadow" of the character's skin, then of its club's skin (fn_80035810).
void fn_80035754(Character* pChar) {
    u64 uShadow;
    s32 nParts;
    int i;

    fn_800CB700(&uShadow, "shadow");
    fn_800CE02C(pChar->pSkin, pChar->n17B4);
    nParts = fn_800CCA40(pChar->pSkin);
    for (i = 0; i < nParts; i++) {
        if (fn_800CCDDC(pChar->pSkin, i) != uShadow) {
            // port: EA passes an argument fn_800CE0B0 ignores
            ((void (*)(Skin*, int, int))fn_800CE0B0)(pChar->pSkin, i, pChar->n17B4);
        }
    }
    fn_800CE128(pChar->pSkin);
    fn_80035810(pChar);
}

// Picks every part but "shadow" of the character's club skin.
void fn_80035810(Character* pChar) {
    u64 uShadow;
    Skin* pSkin;
    s32 nShadow;
    int i;

    fn_800CB700(&uShadow, "shadow");
    if (pChar->p16D8 != NULL) {
        pSkin = pChar->p16D8->apSkins[pChar->nClubClass];
        if (pSkin != NULL) {
            fn_800CE02C(pSkin, pChar->n17B4);
            nShadow = fn_800CDB70(pSkin, "shadow");
            for (i = 0; i < fn_800CCA40(pSkin); i++) {
                if (i != nShadow) {
                    // port: EA passes an argument fn_800CE0B0 ignores
                    ((void (*)(Skin*, int, int))fn_800CE0B0)(pSkin, i, pChar->n17B4);
                }
            }
            fn_800CE128(pSkin);
        }
    }
}

// Draws the character's skin: without flag 2, lit by the current course's light set (flag 4: set 3
// instead, without the character's own light settings) and the ground's light under it; with flag
// 2, only its "shadow" parts, through row 10 of lbl_80188E88.
void fn_800358E0(Character* pChar, u32 uFlags) {
    static f32 aRow10[4] = { 128.0f, 128.0f, 128.0f, 128.0f };
    f32 aRoot[3];
    f32 aData[4];
    CharEntry44* pEntry;
    u32 uSet3;
    int nMode;
    u32 uShadow;

    if (pChar->p44 != NULL) {
        pEntry = &pChar->p44[fn_80035A9C()];
    } else {
        pEntry = NULL;
    }
    uShadow = uFlags & 2;
    if (!uShadow) {
        uSet3 = uFlags & 4;
        if (uSet3) {
            fn_80035338(3);
        } else {
            fn_80035338(0);
            if (pEntry != NULL) {
                // port: Character.p44's entries are what lighting.h calls LightParams
                fn_80093854((LightParams*)pEntry);
            }
        }
        if (!uSet3) {
            fn_80093824();
        }
        fn_8003614C(pChar, aRoot);
        fn_80036024(0.5f * fn_8004B78C(fn_8000C594(), aRoot) + 0.5f);
        fn_80035FFC();
        fn_80035308();
    }
    fn_800352E4();
    fn_80035FDC(NULL);
    if (uShadow) {
        nMode = 2;
    } else if (pChar->n1654 != 1) {
        nMode = 1;
    } else {
        nMode = 2;
    }
    if (uShadow) {
        Vec_Copy(aRow10, aData);
        switch (nMode) {
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
        fn_8003519C(10, aData);
        fn_80035640(pChar);
    } else {
        switch (nMode) {
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
        fn_80035754(pChar);
    }
    fn_80035FBC();
}

// The current course, except on course 7, where fn_80015464's value picks another.
int fn_80035A9C(void) {
    int nCourse;

    nCourse = Game_GetCourse();
    if (nCourse == 7) {
        switch (fn_80015464()) {
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

// Poses the character's skin and its club's skin on its model for view n17B4 (the club's from bone
// 0x52 on). n: every caller passes 0; unused.
void fn_80035B40(Character* pChar, int n) {
    Skin* pClub;

    if (gSession.nSplitScreen == 0 || gSession.nGameType == 3) {
        fn_8011CB5C(pChar->pSkin, pChar->n17B4);
    }
    fn_8003662C(pChar->pSkin, pChar->pModel, 0, 0, pChar->n17B4);
    fn_80036790(pChar->pSkin, pChar->n17B4);
    // port: EA passes arguments fn_800CE16C ignores
    ((void (*)(Skin*, int))fn_800CE16C)(pChar->pSkin, pChar->n17B4);
    if (pChar->p16D8 != NULL) {
        pClub = pChar->p16D8->apSkins[pChar->nClubClass];
        if (pClub != NULL) {
            fn_8003662C(pClub, pChar->pModel, fn_8001EED8(pChar->pModel, 0x52) - 0x52, 0x52,
                        pChar->n17B4);
            fn_80036790(pClub, pChar->n17B4);
        }
    }
    pChar->n1698 = 1;
    if (pChar->nPlayer == 1000) {
        pChar->n1698 = 0;
    }
}

// Sets up the triangles' mesh objects, one per view.
void fn_80035C58(void) {
    DynRenderSize size;
    int i;

    size.nMaxVerts = 3;
    size.nMaxDraws = 1;
    for (i = 0; i < 2; i++) {
        fn_80036054(&lbl_801D4E78.aMesh[i], 0x13, &size);
    }
}

// And frees them.
void fn_80035CC0(void) {
    int i;

    for (i = 0; i < 2; i++) {
        fn_800360A0(&lbl_801D4E78.aMesh[i]);
    }
}

// Draws one grey triangle for view nView through the positions of the character's bones 1 and 7
// (bone 7 twice), when its model has them.
void fn_80035D10(Character* pChar, int nView) {
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
        lbl_801D4E78.aPos[nView][i][0] = apPos[i][0];
        lbl_801D4E78.aPos[nView][i][1] = apPos[i][1];
        lbl_801D4E78.aPos[nView][i][2] = apPos[i][2];
        lbl_801D4E78.aUV[nView][i][0] = 0.0f;
        lbl_801D4E78.aUV[nView][i][1] = 0.0f;
        lbl_801D4E78.aColor[nView][i][0] = 0x80;
        lbl_801D4E78.aColor[nView][i][1] = 0x80;
        lbl_801D4E78.aColor[nView][i][2] = 0x80;
        lbl_801D4E78.aColor[nView][i][3] = 0x80;
        lbl_801D4E78.aIndex[nView][i] = i;
    }
    fill.nCount = 3;
    fill.nVerts = 3;
    fill.pDraws = NULL;
    fill.pIndices = lbl_801D4E78.aIndex[nView];
    fill.pPos = lbl_801D4E78.aPos[nView];
    fill.pColour = lbl_801D4E78.aColor[nView];
    fill.pTexCoord = lbl_801D4E78.aUV[nView];
    fn_80014118(0);
    fn_80035138(0);
    fn_80012EF8();
    fn_80036100(&lbl_801D4E78.aMesh[nView], &fill, 1);
    fn_800360D4(&lbl_801D4E78.aMesh[nView]);
}

// Runs fn_80035D10 for view nView on every character made so far, except those fn_8001EC48 picks
// and those with flag 0x40 or 1.
void fn_80035E98(int nView) {
    int i;

    for (i = 0; i < lbl_80281CA8; i++) {
        if (!fn_8001EC48(lbl_801B9624[i]) && !(lbl_801B9624[i]->u10 & 0x41)) {
            fn_80035D10(lbl_801B9624[i], nView);
        }
    }
}

void fn_80035FBC(void) {
    fn_8006ED70();
}

void fn_80035FDC(UObject* pObj) {
    fn_8006EADC(pObj);
}

// Loads the current light set's lights.
void fn_80035FFC(void) {
    fn_8003612C(&fn_8003532C()->group);
}

void fn_80036024(f32 f) {
    fn_8003532C()->group.v18[0] = f;
}

// Sets up a shader object for row nRow of lbl_80188E88, handing its hooks pDesc.
void fn_80036054(ShaderObject* pObj, int nRow, const void* pDesc) {
    pObj->nRow = nRow;
    pObj->pHooks = &lbl_80188E88[nRow].shader;
    pObj->pHooks->pfnInit(pObj, pDesc);
}

// Frees it, if its row has a free hook.
void fn_800360A0(ShaderObject* pObj) {
    if (pObj->pHooks->pfnFree != NULL) {
        pObj->pHooks->pfnFree(pObj);
    }
}

// Draws it.
void fn_800360D4(ShaderObject* pObj) {
    pObj->pHooks->pfnDraw(pObj);
}

// Hands it a frame's data.
void fn_80036100(ShaderObject* pObj, const void* pData, int n) {
    pObj->pHooks->pfnFill(pObj, pData, n);
}

void fn_8003612C(LightGroup* pGroup) {
    fn_8006E7A4(pGroup);
}

// Copies the character's root bone position to pOut.
void fn_8003614C(Character* pChar, f32* pOut) {
    if (pChar != NULL) {
        Vec_Copy(pChar->pModel->pBones[0].v1C, pOut);
    }
}

// Blends the morph weights of two format 1 poses into pOut, fWeight of the way from pA to pB, for
// each morph either sets; the morphs pOut gets are those of both, and pA's and pB's are cleared.
void fn_80036180(SkelPose1* pA, SkelPose1* pB, SkelPose1* pOut, f32 fWeight) {
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
        fn_80029EF4(aBits, pBlockOut->aBits, 20);
        for (j = 0; j < 20; j++) {
            if (fn_8001E9CC(aBits, j)) {
                pBlockOut->af8[j] = fWeight * (pBlockB->af8[j] - pBlockA->af8[j]) + pBlockA->af8[j];
            }
        }
        fn_8001E938(pBlockA->aBits, 20);
        fn_8001E938(pBlockB->aBits, 20);
    }
}

// Byte-swaps nEntries SkinModel44 entries.
void fn_80036278(SkinModel44* pEntries, s32 nEntries) {
    SwapField aFormat[5] = { { 4, 4 }, { 4, 4 }, { 2, 2 }, { 2, 2 }, { 4, 2 } };
    void* pSrc;
    void* pDst;
    int i;
    SkinModel44* pEntry = pEntries;

    for (i = 0; i < nEntries; i++) {
        pSrc = pDst = pEntry;
        fn_8001F08C(&pSrc, &pDst, aFormat, 5, 1);
        pEntry++;
    }
}

// Byte-swaps each SkinModel44 entry's p4 array.
void fn_80036344(SkinModel44* pEntries, s32 nEntries) {
    u8* pData;
    u8* pSrc;
    int i;

    for (i = 0; i < nEntries; i++) {
        pData = (u8*)pEntries->p4;
        pSrc = pData;
        fn_80076158(&pSrc, pData, pEntries->n8 * 4, 4);
        pEntries++;
    }
}

// Byte-swaps nEntries SkinModel54 entries.
void fn_800363B4(SkinModel54* pEntries, s32 nEntries) {
    SwapField aFormat[3] = { { 2, 2 }, { 6, 2 }, { 12, 4 } };
    void* pSrc;
    void* pDst;
    int i;
    SkinModel54* pEntry = pEntries;

    for (i = 0; i < nEntries; i++) {
        pSrc = pDst = pEntry;
        fn_8001F08C(&pSrc, &pDst, aFormat, 3, 1);
        pEntry++;
    }
}

void fn_80036460(int n) {
}

void fn_80036464(void) {
    if (lbl_80281D70 != NULL) {
        fn_80009E70(lbl_80281D70);
    }
    lbl_80281D70 = NULL;
    lbl_80281D78 = 0;
    lbl_80281D74 = 0;
}

void fn_800364A0(void) {
    lbl_80281D74 = 0;
}

// Turns a model's offsets into pointers and byte-swaps its tables, once (bit 31 of u30). A model
// that is not version 4 is cleared to an empty one.
void fn_800364AC(SkinModel* pModel) {
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
    fn_80036278(pModel->p44, pModel->n40);
    fn_800363B4(pModel->p54, pModel->n50);
    for (i = 0; i < pModel->n40; i++) {
        if (pModel->p44[i].p4 != NULL) {
            pModel->p44[i].p4 = (s32*)((u8*)pModel + (uptr)pModel->p44[i].p4);
        }
        // EA bug: swaps every entry's array once per entry, and reaches entries whose p4 is
        // still an offset; right only for a single entry.
        fn_80036344(pModel->p44, pModel->n40);
    }
    pModel->u30 = pModel->u30 | 0x80000000;
}

// Builds the skin's matrices (p108C): those of the model's bones come from the character model's
// p768 (from bone nFirst on, nSkip further along there); each one after that whose bit is set in
// p10CC is a weighted sum of up to three of them (SkinModel.p54). nView: both callers pass the
// character's n17B4; unused.
void fn_8003662C(Skin* pSkin, CharModel* pCharModel, int nSkip, int nFirst, int nView) {
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
    if (!(pSkin->u10D4 & 2)) {
        return;
    }
    fn_800CDA68(pSkin);
    if (pSkin->p108C != pCharModel->p768) {
        Mem_cpy(pSkin->p108C[nFirst], pCharModel->p768[nFirst + nSkip],
                (pSkin->pModel->n14 - nFirst) * sizeof(*pSkin->p108C));
    }
    aMtx = pSkin->p108C;
    for (i = pSkin->pModel->n14; i < nMatrices; i++) {
        if (fn_8001E9CC(pSkin->p10CC, i)) {
            pEntry = &pSkin->pModel->p54[i];
            nBones = pEntry->nBones;
            memset(aMtx[i], 0, sizeof(aMtx[i]));
            for (j = 0; j < nBones; j++) {
                pDst = aMtx[i];
                fWeight = pEntry->afWeights[j];
                for (k = 0, pSrc = aMtx[pEntry->aBones[j]]; k < 4; k++) {
                    fn_8000AE6C(pDst[k], pSrc[k], fWeight, pDst[k]);
                }
            }
        }
    }
}

// Hands table n (Skin.a10A0) every p44 entry whose bit is set in p10D0, with the matrices.
void fn_80036790(Skin* pSkin, int n) {
    int i;
    s32 nEntries;
    HwsOverrideTable* pTable;

    if (pSkin->a10A0[n] != NULL && (pSkin->u10D4 & 2)) {
        pTable = pSkin->a10A0[n];
        nEntries = pSkin->pModel->n40;
        for (i = 0; i < nEntries; i++) {
            if (fn_8001E9CC(pSkin->p10D0, i)) {
                fn_80112B18(pTable, pSkin->pModel->p44[i].n0, pSkin->p108C);
            }
        }
    }
}

// Byte-swaps a model's header.
void fn_8003682C(SkinModel* pModel) {
    SwapField aFormat[26] = { { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 2, 2 },
                              { 2, 2 }, { 2, 2 }, { 2, 2 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 }, { 4, 4 },
                              { 4, 4 }, { 4, 4 }, { 4, 4 }, { 8, 1 }, { 224, 4 } };
    void* pSrc;
    void* pDst;

    pDst = pSrc = pModel;
    fn_8001F08C(&pSrc, &pDst, aFormat, 26, 1);
}

// Byte-swaps a skin description's header.
void fn_80036894(SkinDesc* pDesc) {
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
    fn_8001F08C(&pSrc, &pDst, aFormat, 48, 1);
}

// Byte-swaps a skin description read from its little-endian file, in place, and turns its mesh
// bit offsets into pointers. Version 8 descriptions with n04 == 0 have the old p14 layout
// (SkinDesc14Old) and are converted to the new one.
void fn_800368FC(SkinDesc* pDesc) {
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
    u8* p;
    void* pSrc;                 // port: fn_80076158 takes it as a u8** too (one stack slot in EA's code)
    void* pDst;
    s32 n;
    int i;

    if (pDesc->nVersion > 8 || (pDesc->nVersion == 8 && pDesc->n04 == 1)) {
        p = (u8*)pDesc->p14;
        for (i = 0; i < pDesc->n10; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aDesc14, 10, 1);
            p += sizeof(SkinDesc14);
        }
    } else if (pDesc->nVersion == 8 && pDesc->n04 == 0) {
        p = (u8*)pDesc->p14;
        if (p != NULL) {
            for (i = 0; i < pDesc->n10; i++) {
                pSrc = pDst = p;
                fn_8001F08C(&pSrc, &pDst, aOld14, 9, 1);
                p += sizeof(SkinDesc14);
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

    p = (u8*)pDesc->p18;
    if (p != NULL) {
        for (i = 0; i < pDesc->n10; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aDesc18, 3, 1);
            p += sizeof(SkinDesc18);
        }
    }

    p = (u8*)pDesc->p20;
    if (p != NULL && pDesc->n1C != 0) {
        pSrc = pDst = p;
        fn_80076158((u8**)&pSrc, p, pDesc->n1C * 4, 4);
    }

    p = (u8*)pDesc->p28;
    if (p != NULL && pDesc->n24 != 0) {
        for (i = 0; i < pDesc->n24; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aDesc28, 5, 1);
            p += sizeof(SkinDesc28);
        }
        for (i = 0; i < pDesc->n24; i++) {
            if (pDesc->p28[i].p10 != NULL) {
                pDesc->p28[i].p10 = (u8*)pDesc + (uptr)pDesc->p28[i].p10;
            }
        }
    }

    p = (u8*)pDesc->p34;
    if (p != NULL && pDesc->n2C != 0) {
        for (i = 0; i < pDesc->n2C; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aMesh, 4, 1);
            p += sizeof(SkinMesh);
        }
    }

    // Each mesh's bit data: its layout depends on the mesh's flags.
    for (i = 0; i < pDesc->n2C; i++) {
        if (pDesc->p34[i].pBits != NULL) {
            pDesc->p34[i].pBits = (SkinMeshBit*)((u8*)pDesc + (uptr)pDesc->p34[i].pBits);
        }
        pMesh = &pDesc->p34[i];
        pData = (u8*)pMesh->pBits;
        n = pMesh->n8;
        if (pData == NULL) {
            pMesh->pBits = NULL;
        } else if (pMesh->uFlags & 0x30) {
            pSrc = pDst = pData;
            fn_80076158((u8**)&pSrc, pData, n * 8, 2);
            pData += n * 8;
            pSrc = pData;
            fn_80076158((u8**)&pSrc, pData, n * 4, 1);
            pData += n * 4;
            if (pMesh->uFlags & 0x1000) {
                pSrc = pData;
                fn_80076158((u8**)&pSrc, pData, n * 2, 2);
            } else if (pMesh->uFlags & 0x40) {
                pSrc = pData;
                fn_80076158((u8**)&pSrc, pData, n * 4, 2);
            }
        } else if (pMesh->uFlags & 0x40) {
            pSrc = pDst = pData;
            fn_80076158((u8**)&pSrc, pData, n * 4, 2);
        } else if (pMesh->uFlags & 0x1808) {
            pSrc = pData;
            fn_80076158((u8**)&pSrc, pData, n * 2, 2);
        } else if (pMesh->uFlags & 0x300004) {
            pSrc = pData;
            fn_80076158((u8**)&pSrc, pData, n * 64, 4);
        } else if (pMesh->uFlags & 2) {
            pSrc = pData;
            fn_80076158((u8**)&pSrc, pData, n * 2, 2);
        }
    }

    p = (u8*)pDesc->p3C;
    if (p != NULL && pDesc->n38 != 0) {
        pSrc = pDst = p;
        fn_80076158((u8**)&pSrc, p, pDesc->n38 * 4, 4);
    }

    p = (u8*)pDesc->p44;
    for (i = 0; i < pDesc->n40; i++) {
        pSrc = pDst = p;
        fn_8001F08C(&pSrc, &pDst, aDesc44, 12, 1);
        p += sizeof(SkinDesc44);
    }

    p = (u8*)pDesc->pParts;
    if (p != NULL && pDesc->nParts != 0) {
        for (i = 0; i < pDesc->nParts; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aPart, 3, 1);
            p += sizeof(SkinPartDef);
        }
    }

    p = (u8*)pDesc->pVariants;
    if (p != NULL && pDesc->nVariants != 0) {
        for (i = 0; i < pDesc->nVariants; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aVariant, 5, 1);
            p += sizeof(SkinVariant);
        }
    }

    p = (u8*)pDesc->p5C;
    if (p != NULL) {
        if (pDesc->n58 != 0 && p != NULL && pDesc->n58 != 0) {
            pSrc = pDst = p;
            fn_80076158((u8**)&pSrc, p, pDesc->n58 * 8, 4);
        }
    }

    p = (u8*)pDesc->pLinks;
    if (p != NULL && pDesc->nLinks != 0) {
        for (i = 0; i < pDesc->nLinks; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aLink, 2, 1);
            p += sizeof(SkinLink);
        }
    }

    p = (u8*)pDesc->p6C;
    if (p != NULL && pDesc->n68 != 0) {
        pSrc = pDst = p;
        fn_80076158((u8**)&pSrc, p, pDesc->n68 * 4, 4);
    }

    p = (u8*)pDesc->p74;
    if (p != NULL && pDesc->n70 != 0) {
        for (i = 0; i < pDesc->n70; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aDesc74, 5, 1);
            p += sizeof(SkinDesc74);
        }
    }

    p = (u8*)pDesc->p7C;
    if (p != NULL && pDesc->n78 != 0) {
        for (i = 0; i < pDesc->n78; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aDesc7C, 4, 1);
            p += sizeof(SkinDesc7C);
        }
    }

    p = (u8*)pDesc->p8C;
    if (p != NULL && pDesc->n88 != 0) {
        for (i = 0; i < pDesc->n88; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aDesc8C, 5, 1);
            p += sizeof(SkinDesc8C);
        }
    }

    p = pDesc->p94;
    if (p != NULL && pDesc->n90 != 0) {
        for (i = 0; i < pDesc->n90; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aDesc94, 3, 1);
            p += 0x50;
        }
    }

    p = (u8*)pDesc->pA4;
    if (p != NULL && pDesc->nA0 != 0) {
        pSrc = pDst = p;
        fn_80076158((u8**)&pSrc, p, pDesc->nA0 * 2, 2);
    }

    p = (u8*)pDesc->pAC;
    if (p != NULL && pDesc->nA8 != 0) {
        pSrc = pDst = p;
        fn_80076158((u8**)&pSrc, p, pDesc->nA8 * 2, 2);
    }

    p = (u8*)pDesc->pB8;
    if (p != NULL && pDesc->nB4 != 0) {
        for (i = 0; i < pDesc->nB4; i++) {
            pSrc = pDst = p;
            fn_8001F08C(&pSrc, &pDst, aDescB8, 2, 1);
            p += sizeof(SkinDescB8);
        }
    }
}

// Frees what a loaded skin allocated (bit 2 of u10D4); 0 when it was not loaded.
s32 fn_80037708(Skin* pSkin) {
    SkinModel* pModel;

    if (!(pSkin->u10D4 & 2)) {
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
    ((void (*)(Skin*))fn_800CE168)(pSkin);  // port: EA passes an argument fn_800CE168 ignores
    if (pSkin->p108C != NULL) {
        fn_80009E70(pSkin->p108C);
    }
    pSkin->p108C = NULL;
    if (pSkin->p10CC != NULL) {
        fn_80009E70(pSkin->p10CC);
    }
    pSkin->p10CC = NULL;
    if (pSkin->p10D0 != NULL) {
        fn_80009E70(pSkin->p10D0);
    }
    pSkin->p10D0 = NULL;
    pSkin->u10D4 = pSkin->u10D4 & ~2;
    return 1;
}

// ---- end of sweep code ----

// Byte-swaps the model's bone poses.
void fn_80037574(BonePose* pBones, s32 nBones) {
    u8* pSrc;

    pSrc = (u8*)pBones;
    fn_80076158(&pSrc, (u8*)pBones, nBones * sizeof(BonePose), 4);
}

// Allocates what a skin needs once loaded (bit 2 of u10D4; 0 if it already was): the matrices,
// the two bit arrays and, with a description, its morph memory. b: also calls fn_800CE164.
s32 fn_800375AC(Skin* pSkin, u8 b) {
    SkinModel* pModel;
    SkinDesc* pDesc;
    s32 nSize;

    if (pSkin->u10D4 & 2) {
        return 0;
    }
    pModel = pSkin->pModel;
    pDesc = pModel->pDesc;
    pSkin->p108C = fn_80009B34(pModel->n50 * sizeof(*pSkin->p108C), 2, 0x80, "Skin.c", 0x500);
    pSkin->p10CC = fn_80009B34((pModel->n50 + 31) / 32 * 4, 2, 0, "Skin.c", 0x50C);
    pSkin->p10D0 = fn_80009B34((pModel->n40 + 31) / 32 * 4, 2, 0, "Skin.c", 0x50D);
    fn_8001E938(pSkin->p10CC, pModel->n50);
    fn_8001E938(pSkin->p10D0, pModel->n40);
    if (pDesc != NULL) {
        fn_800CD700(pSkin);
        nSize = fn_8011CDE8(pSkin);
        if (nSize != 0) {
            pSkin->a1098[0] = fn_801128C8(pModel->pDesc, nSize);
        }
        pSkin->a10A0[0] = fn_80112A10(pModel->pDesc, 0);
        pSkin->u10D4 = 1;
        if (b) {
            ((void (*)(Skin*))fn_800CE164)(pSkin);  // port: EA passes an argument fn_800CE164 ignores
        }
    }
    fn_8011CE58(pSkin);
    pSkin->u10D4 = pSkin->u10D4 | 2;
    return 1;
}

// Makes a skin from its file: copies the model (and its description) into memory of its own,
// byte-swapping the file first if that has not been done, and starts the pose at the model's.
// b picks the allocation mode (1 instead of 2).
Skin* fn_800377FC(u8* pData, u8 b) {
    SkinModel* pModel;
    Skin* pSkin;
    SkinDesc* pDesc;
    BonePose* pBones;
    s32 nDescSize;
    s32 nSize;
    u8 bSwap;
    int i;
    u8 bOld;

    bOld = fn_800CEE90();
    fn_800CEE88(1);
    pSkin = fn_80009B34(sizeof(Skin), 2, 0x80, "Skin.c", 0x5C4);
    memset(pSkin, 0, sizeof(Skin));
    pModel = (SkinModel*)pData;     // the file's model, then the copy
    if ((pModel->u30 & 0x40000002) != 0x40000002) {
        fn_8003682C((SkinModel*)pData);
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
            fn_80036894(pDesc);
        }
        nDescSize = (pDesc != NULL) ? pDesc->n08 : 0;
        nSize = pModel->n08 - nDescSize;
    } else {
        nSize = sizeof(SkinModel);
        nDescSize = 0;
    }
    if (b) {
        pSkin->pModel = fn_80009B34(nSize, 1, 0x80, "Skin.c", 0x5EF);
    } else {
        pSkin->pModel = fn_80009B34(nSize, 2, 0x80, "Skin.c", 0x5F3);
    }
    memcpy(pSkin->pModel, pModel, nSize);
    pModel = pSkin->pModel;
    fn_800364AC(pModel);
    pModel->n08 = nSize;
    if (nDescSize != 0) {
        if (b) {
            pModel->pDesc = fn_80009B34(nDescSize, 1, 0x80, "Skin.c", 0x604);
        } else {
            pModel->pDesc = fn_80009B34(nDescSize, 2, 0x80, "Skin.c", 0x608);
        }
        memcpy(pModel->pDesc, pDesc, nDescSize);
        fn_800CEF04(pModel->pDesc);
        fn_800368FC(pModel->pDesc);
    } else {
        pModel->pDesc = NULL;
    }
    pDesc = pModel->pDesc;
    pSkin->b1044 = 0;
    pBones = pModel->p34;
    fn_80037574(pBones, pModel->n14);
    fn_8001E8A4(pSkin->pose.a0, 0x80);
    fn_8001E8A4(pSkin->pose.a10, 0x80);
    fn_8001E8A4(pSkin->pose.a20, 0x80);
    fn_8001E8A4(pSkin->pose.a30, 0x80);
    for (i = 0; i < pModel->n14; i++) {
        fn_8001E85C(pBones[i].q0, pSkin->pose.aBones[i].q0);
        fn_8001E85C(pBones[i].v10, pSkin->pose.aBones[i].v10);
    }
    pSkin->p1088 = fn_80009B34(pModel->n14 * sizeof(*pSkin->p1088), 2, 0x80, "Skin.c", 0x62A);
    if (pDesc != NULL) {
        fn_801127A0(pDesc);
    }
    fn_800CD404(pSkin);
    fn_8011C9B0(pSkin);
    fn_800CEE88(bOld);
    fn_800375AC(pSkin, 0);
    return pSkin;
}

// Once per skin model (flag 0x8000): turns its bone poses from relative to their parent (the
// character model's bone parents, from bone nFirst on nSkip further along) into model space, then
// stores each bone's inverse matrix in p1088.
void fn_80037AB8(Skin* pSkin, CharModel* pCharModel, int nSkip, int nFirst) {
    f32 aQuat[4];
    f32 aTurned[4];
    f32 aMtx[4][4];
    BonePose* pParent;
    int nBone;
    int i;

    if (pSkin->pModel != NULL && !(pSkin->pModel->u30 & 0x8000)) {
        pSkin->pModel->u30 |= 0x8000;
        for (i = 1; i < pSkin->pModel->n14; i++) {
            nBone = i;
            if (i >= nFirst) {
                nBone = i + nSkip;
            }
            pParent = &pSkin->pModel->p34[pCharModel->pBones[nBone].nParent];
            fn_800090E4(pParent->q0, pSkin->pModel->p34[i].v10, aTurned);
            fn_800090A0(pParent->v10, aTurned, pSkin->pModel->p34[i].v10);
            pSkin->pModel->p34[i].v10[3] = 0.0f;
            fn_80008FCC(pSkin->pModel->p34[i].q0, pParent->q0, aQuat);
            fn_8001E85C(aQuat, pSkin->pModel->p34[i].q0);
        }
        for (i = 0; i < pSkin->pModel->n14; i++) {
            fn_8000914C(pSkin->pModel->p34[i].q0, aMtx);
            fn_8001E880(pSkin->pModel->p34[i].v10, aMtx[3]);
            fn_8000A798(aMtx, pSkin->p1088[i]);
        }
    }
}

// Hands the skin the morph weights a format 1 pose buffer changed (bits 5..19 of its first block),
// then clears the block's bits.
void fn_80037C48(Skin* pSkin, SkelPose* pPose) {
    SkelPoseBlock* pBlock;
    int i;

    if (pSkin != NULL) {
        pBlock = &((SkelPose1*)pPose)->aBlocks[0];
        for (i = 5; i < 20; i++) {
            if (fn_8001E9CC(pBlock->aBits, i)) {
                fn_8011CADC(pSkin, i - 5, pBlock->af8[i]);
            }
        }
        fn_8001E938(pBlock->aBits, 20);
    }
}

// ---- sweep code (tidied) ----

void fn_80037CD8(Skin* pSkin) {
    SkinModel* pModel;

    fn_80037708(pSkin);
    fn_8011CD84(pSkin);
    fn_800CD56C(pSkin);
    pModel = pSkin->pModel;
    if (pModel != NULL) {
        if (pModel->pDesc != NULL) {
            fn_801127C4(pModel->pDesc);
            fn_80009E70(pSkin->pModel->pDesc);
        }
        fn_80009E70(pSkin->pModel);
    }
    if (pSkin->p1088 != NULL) {
        fn_80009E70(pSkin->p1088);
    }
    fn_80009E70(pSkin);
}

// ---- end of sweep code ----

// Frees the mesh bit data a description allocated for itself (flag 0x400000).
void fn_80037D5C(SkinDesc* pDesc) {
    int i;

    for (i = 0; i < pDesc->n2C; i++) {
        if (pDesc->p34[i].pBits != NULL && (pDesc->p34[i].uFlags & 0x400000)) {
            fn_80009E70(pDesc->p34[i].pBits);
        }
    }
}
