// Code800B9944.c (our name: no EA name is known): the ball the Create-A-Player menu golfer holds:
// its 'TEO ' models and 'BALF' logo bank as they stream in, the logo put on it, and its drawing in
// his hand (FEgolferanim.c calls it). GoDynObj.c does the same for the ball in play.
// A unit of its own, not part of rcmp_mad_codec.c before it: EA's rcmp_mad_codec.c is the MAD
// codec's frame code (its file name string; NFSMW's rcmp_mad_codec.cpp), and the two share no data.
// This file's .sbss (0x802821C8..0x802821E8, padded after gpCrAPBallTeo10000), .sdata and .sdata2
// (1.0f at 0x802841D8) each come after the MAD code's, in link order.

#include "engine.h"
#include "dynobj.h"
#include "character.h"
#include "camera.h"
#include "terrain.h"
#include "llpict.h"

// Defined here, last address first (CodeWarrior lays out .sbss in reverse).
UObject* gpCrAPBallTeo10000;    // the menu ball's objects, made from the 'TEO ' objects 10000
UObject* gpCrAPBallTeo10030;    // (always drawn), 10030 and 10040 (drawn only while a logo is on
UObject* gpCrAPBallTeo10040;    // the ball) by FE_CrAPBall_MakeObjects
TexBank* gpCrAPBallLogoBank;    // the 'BALF' texture bank of ball logos (FE_CrAPBall_LoadBALF)
void* gpCrAPBallUnusedMem;      // freed by FE_CrAPBall_Free when set; nothing here sets it
f32 gfCrAPBallOffsetY;          // } the held ball's offset in bone 0x54 (x, y); never set, so 0
f32 gfCrAPBallOffsetX;          // }

// ---- the 'TEO ' and 'BALF' stream handlers ----

u8 gbCrAPBallLogoShown = 1;     // a logo is on the ball: objects 10030 and 10040 are drawn

f32 gfCrAPBallOffsetZ = -2.0f;  // the held ball's offset in bone 0x54 (z; x and y above)
f32 gfCrAPBallScaleX = 1.0f;    // }
f32 gfCrAPBallScaleY = 1.0f;    // } the ball's scale on each axis
f32 gfCrAPBallScaleZ = 1.0f;    // }
char gszCrAPBallLogoTex[] = "logoea";   // the texture FE_CrAPBall_SetLogo copies a ball logo over

void FE_CrAPBall_LoadBALF(UStreamObject* pObject);
void FE_CrAPBall_LoadTEO(UStreamObject* pObject);
void LLMath_CopyMat44(f32 (*pSrc)[4], f32 (*pDst)[4]);
void LLMath_IdentifyMat(f32 (*pMtx)[4]);                   // identity
void UObject_ComposeRotation(f32 (*pMtx)[4]);
void LLMath_mat44fltMultiplyList(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows);
void LLMath_mat44fltMultiplyList33(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows);
void Character_GetBonePos(Character* pChar, int nBone, f32* pPos);
void RenderState_SetRenderSurface(int a, int nWidth, int nHeight, int nField, int b, int c);
int  fn_8001005C(TexBank* pBank, u64 uHash);       // LLTex.c: the texture's index, or 0x80000000

// Registers the stream handlers for the ball the Create-A-Player menu golfer holds: 'TEO ' objects
// (its models, FE_CrAPBall_LoadTEO) and the 'BALF' texture bank (its logos, FE_CrAPBall_LoadBALF).
void FE_CrAPBall_RegisterStreamClients(void) {
    Stream_RegisterLoadChunkCallback('TEO ', FE_CrAPBall_LoadTEO);
    Stream_RegisterLoadChunkCallback('BALF', FE_CrAPBall_LoadBALF);
}

void FE_CrAPBall_UnRegisterStreamClients(void) {
    Stream_UnregisterLoadChunkCallback('TEO ');
    Stream_UnregisterLoadChunkCallback('BALF');
}

// The 'BALF' load handler: the object is a texture bank of ball logos, kept for
// FE_CrAPBall_SetLogo; the stream object itself is freed.
void FE_CrAPBall_LoadBALF(UStreamObject* pObject) {
    gpCrAPBallLogoBank = fn_8000FB88(pObject, NULL, 0);
    StaticMem_Free(pObject);
}

// ---- sweep code (not yet cleaned up) ----

void FE_CrAPBall_FreeTEO(void* arg0);

// The 'TEO ' load handler: unless an object of the same type and id is listed already, the object's
// model is built from its data (fn_80045D80) and kept in the object (word 4), with
// FE_CrAPBall_FreeTEO as its free function (word 8), and the object is listed (fn_8000B4B8) for
// FE_CrAPBall_MakeObjects to find.
void FE_CrAPBall_LoadTEO(UStreamObject* pObject) {
    if (fn_8000B508(pObject) == 0) {
        (*(UObjModel**)((u8*)(pObject) + 4)) = fn_80045D80(pObject->pData);
        (*(void (**)(void*))((u8*)(pObject) + 8)) = FE_CrAPBall_FreeTEO;
        fn_8000B4B8(pObject);
    }
}

// Frees the model FE_CrAPBall_LoadTEO built for a 'TEO ' object (pObject: the object): its root
// (fn_800075CC), then the model.
void FE_CrAPBall_FreeTEO(void* pObject) {
    void* pModel;

    pModel = (*(void**)((u8*)(pObject) + 4));
    fn_800075CC(*(UObjModelRoot**)((u8*)(pModel) + 0x10));
    StaticMem_Free(pModel);
}

// ---- end of sweep code ----

// Clears the menu ball's state when the front end starts (GO_vInitFE): no objects, no logo bank,
// the logo layers shown.
void FE_CrAPBall_Init(void) {
    gpCrAPBallTeo10000 = NULL;
    gpCrAPBallTeo10030 = NULL;
    gpCrAPBallTeo10040 = NULL;
    gpCrAPBallLogoBank = NULL;
    gpCrAPBallUnusedMem = NULL;
    gbCrAPBallLogoShown = 1;
}

// Frees the menu ball's three objects, its logo bank (its pixel and palette data, then the bank)
// and gpCrAPBallUnusedMem when set (nothing here sets it), and clears them. Called when the front end
// closes.
void FE_CrAPBall_Free(void) {
    if (gpCrAPBallTeo10000 != NULL) {
        fn_80048860(gpCrAPBallTeo10000);
    }
    gpCrAPBallTeo10000 = NULL;
    if (gpCrAPBallTeo10030 != NULL) {
        fn_80048860(gpCrAPBallTeo10030);
    }
    gpCrAPBallTeo10030 = NULL;
    if (gpCrAPBallTeo10040 != NULL) {
        fn_80048860(gpCrAPBallTeo10040);
    }
    gpCrAPBallTeo10040 = NULL;
    if (gpCrAPBallLogoBank != NULL) {
        fn_8000FFAC(gpCrAPBallLogoBank);
        StaticMem_Free(gpCrAPBallLogoBank);
        gpCrAPBallLogoBank = NULL;
    }
    if (gpCrAPBallUnusedMem != NULL) {
        StaticMem_Free(gpCrAPBallUnusedMem);
        gpCrAPBallUnusedMem = NULL;
    }
}

// The three objects, made from their 'TEO ' models once those have streamed in.
// port: a 'TEO ' object's UStreamObject.uUnk4 holds its model (FE_CrAPBall_LoadTEO stores it there).
void FE_CrAPBall_MakeObjects(void) {
    UStreamObject* pObject;

    if (gpCrAPBallTeo10000 == NULL) {
        pObject = fn_8000B70C('TEO ', 10000);
        if (pObject != NULL) {
            gpCrAPBallTeo10000 = fn_80048808((UObjModel*)pObject->uUnk4);
        }
    }
    if (gpCrAPBallTeo10030 == NULL) {
        pObject = fn_8000B70C('TEO ', 10030);
        if (pObject != NULL) {
            gpCrAPBallTeo10030 = fn_80048808((UObjModel*)pObject->uUnk4);
        }
    }
    if (gpCrAPBallTeo10040 == NULL) {
        pObject = fn_8000B70C('TEO ', 10040);
        if (pObject != NULL) {
            gpCrAPBallTeo10040 = fn_80048808((UObjModel*)pObject->uUnk4);
        }
    }
}

// Draw pObj turned by mBone and scaled by mScale, at pPos in the create-a-player view; its
// matrices are put back after.
void FE_CrAPBall_DrawObject(UObject* pObj, f32 (*mBone)[4], f32 (*mScale)[4], f32* pPos) {
    f32 m0[4][4];
    f32 m40[4][4];
    f32 m80[4][4];

    LLMath_CopyMat44(pObj->m0, m0);
    LLMath_CopyMat44(pObj->m40, m40);
    LLMath_CopyMat44(pObj->m80, m80);
    LLMath_mat44fltMultiplyList33(mBone, pObj->m0, pObj->m0, 3);
    LLMath_mat44fltMultiplyList33(mScale, pObj->m40, pObj->m40, 3);
    UObject_ComposeRotation(pObj->m0);
    LLMath_CopyVec(pPos, pObj->m80[3]);
    pObj->m80[3][3] = 1.0f;
    LLMath_mat44fltMultiplyList(gpCrAPState->mC0, pObj->m80, pObj->m80, 4);
    LLMath_IdentifyMat(pObj->m0);
    fn_80048894(pObj);
    LLMath_CopyMat44(m0, pObj->m0);
    LLMath_CopyMat44(m40, pObj->m40);
    LLMath_CopyMat44(m80, pObj->m80);
}

// Draws the ball in the Create-A-Player menu golfer's hand (bone 0x54) when he holds it, into the
// 384 x 528 target when bTarget, else to the screen (512 x 448). Object 10000 is always drawn;
// 10030 and 10040 only while a logo is on the ball (FE_CrAPBall_SetLogo). The ball sits at the
// bone's offset (0, 0, -2) with scale 1 on each axis (gfCrAPBallOffsetX, gfCrAPBallOffsetY,
// gfCrAPBallOffsetZ; gfCrAPBallScaleX..gfCrAPBallScaleZ).
void FE_CrAPBall_Render(u8 bTarget) {
    f32 vPos[4];
    f32 mScale[4][4];
    f32 (*mBone)[4];

    if (Character_IsHoldingBall(gpCrAPState->pB4->pChar)) {
        vPos[0] = gfCrAPBallOffsetX;
        vPos[1] = gfCrAPBallOffsetY;
        vPos[2] = gfCrAPBallOffsetZ;
        vPos[3] = 1.0f;
        Character_GetBonePos(gpCrAPState->pB4->pChar, 0x54, vPos);
        mBone = Character_GetBoneMatrix(gpCrAPState->pB4->pChar, 0x54);
        LLMath_IdentifyMat(mScale);
        mScale[0][0] = gfCrAPBallScaleX;
        mScale[1][1] = gfCrAPBallScaleY;
        mScale[2][2] = gfCrAPBallScaleZ;
        if (bTarget) {
            RenderState_SetRenderSurface(1, 0x180, 0x210, 0, 1, 1);
        } else {
            RenderState_SetRenderSurface(0, 0x200, 0x1C0, lbl_80281B88 & 1, 1, 1);
        }
        RenderState_SetViewport(RC_spGetCurrentRenderCtx());
        RenderState_SetBlendFactors(4, 5);
        DS_vSetAlphaTestMode(0, 6, 0x80);
        DS_vEnableZBufferUpdate(1);
        RenderState_Flush();
        if (gpCrAPBallTeo10000 != NULL) {
            FE_CrAPBall_DrawObject(gpCrAPBallTeo10000, mBone, mScale, vPos);
        }
        if (gbCrAPBallLogoShown) {
            if (gpCrAPBallTeo10030 != NULL) {
                FE_CrAPBall_DrawObject(gpCrAPBallTeo10030, mBone, mScale, vPos);
            }
            if (gpCrAPBallTeo10040 != NULL) {
                FE_CrAPBall_DrawObject(gpCrAPBallTeo10040, mBone, mScale, vPos);
            }
        }
        RC_vSetCurrentRenderCtxTransformationMatrix(NULL);
        RenderState_SetRenderSurface(0, 0x200, 0x1C0, lbl_80281B88 & 1, 8, 1);
        RenderState_SetViewport(RC_spGetCurrentRenderCtx());
        RenderState_Flush();
    }
}

// Puts ball logo szBall on the menu golfer's ball: that texture of the 'BALF' bank has each of its
// levels copied over the texture "logoea" and the logo layers are shown. NULL hides the logo layers
// instead. Nothing without the bank, without a "logoea" texture, or when the bank has no such logo.
void FE_CrAPBall_SetLogo(char* szBall) {
    u64       uLogo;
    u64       uSlot;
    TexBank*  pSlotBank;
    TexEntry* pSlot;
    TexEntry* pLogo;
    int       nLogo;
    int       i;

    if (gpCrAPBallLogoBank == NULL) {
        return;
    }
    if (szBall == NULL) {
        gbCrAPBallLogoShown = 0;
        return;
    }
    SKA_PackName(&uLogo, szBall);
    SKA_PackName(&uSlot, gszCrAPBallLogoTex);
    fn_800102DC(uSlot, &pSlotBank, &pSlot);
    if (pSlotBank == NULL || pSlot == NULL) {
        return;
    }
    nLogo = fn_8001005C(gpCrAPBallLogoBank, uLogo);
    if (nLogo == (int)0x80000000) {
        return;
    }
    gbCrAPBallLogoShown = 1;
    pLogo = &gpCrAPBallLogoBank->p8[nLogo];
    for (i = 0; i < pLogo->n41; i++) {
        Mem_cpy(pSlotBank->p18 + pSlot->aMips[i].uPixels, gpCrAPBallLogoBank->p18 + pLogo->aMips[i].uPixels,
                pLogo->aMips[i].nC * 16);
    }
}
