// Code800B90F4.c (our name): own unit, its .sbss starts on the 8-aligned address after
// rcmp_mad_codec.c's padding at 0x802821BC..0x802821C0 and ends padded at 0x802821E4..0x802821E8

#include "engine.h"
#include "dynobj.h"
#include "character.h"
#include "camera.h"
#include "terrain.h"
#include "llpict.h"

// Defined here, last address first (CodeWarrior lays out .sbss in reverse).
UObject* gpCrAPBallTeo10000;          // } and 10000
UObject* gpCrAPBallTeo10030;          // } 10030
UObject* gpCrAPBallTeo10040;          // } made from the 'TEO ' objects 10040,
TexBank* gpCrAPBallLogoBank;          // from the 'BALF' object
void* gpCrAPBallUnusedMem;
f32 gfCrAPBallOffsetY;               // }
f32 gfCrAPBallOffsetX;               // } the held ball's offset in its bone (x, y); never set, so 0
PictFile* (*gpfnMadRead)(void* pArg);   // reads the next MAD file
void* gpMadReadArg;             // what the read function is given

void MAD_initdecode(u8* src, int motion, int quality);
void MAD_decodemacroblock(u8* src_y, u8* src_cb, u8* src_cr, u8* dest_y, u8* dest_cb, u8* dest_cr, int width);
u32 MAD_GetFileKind(PictFile* pFile);
void MAD_FreeFile(PictFile* pFile);
void MAD_AddFrameToList(PictFrame** apList, PictFrame* pFrame);
PictFile* MAD_ReadNextFile(MadDecoder* p);
PictFrame* MAD_TakeReferenceFrame(MadDecoder* p);
PictFrame* MAD_TakeFrameFromList(PictFrame** apList);
PictFrame* MAD_TakeOutputFrame(MadDecoder* p);
void MAD_ReleaseFrame(MadDecoder* p, PictFrame* pFrame);
void MAD_RemoveFrameFromLists(MadDecoder* p, PictFrame* pFrame);

// Sets the function (and the argument it is given) that MAD_ReadNextFile takes the movie's MAD
// files from; LLPict_Gc.c's movie set-up passes it on (fn_8002FEB0).
void MAD_SetReadCallback(PictFile* (*pfnRead)(void* pArg), void* pArg) {
    gpfnMadRead = pfnRead;
    gpMadReadArg = pArg;
}

// Allocates frame pFrame's pixels for an nWidth x nHeight picture: the Y plane and the quarter-size
// U and V planes after it (3/2 bytes a pixel), with no references yet.
void MAD_AllocFrame(PictFrame* pFrame, int nWidth, int nHeight) {
    pFrame->nRefs = 0;
    pFrame->pPixels = StaticMem_Alloc((u32)(nHeight * nWidth * 3) >> 1, 1, 32, "rcmp_mad_codec.c", 79);
    pFrame->nWidth = nWidth;
    pFrame->nHeight = nHeight;
}

// Frees frame pFrame's pixels, when it has any (the frame itself belongs to the decoder's block of
// six).
void MAD_FreeFrame(PictFrame* pFrame) {
    if (pFrame->pPixels != NULL) {
        StaticMem_Free(pFrame->pPixels);
        pFrame->pPixels = NULL;
    }
}

// Resets decoder p for a new movie: the frames are allocated with the first file (bFirst), no files
// read, no reference frame, both frame lists empty. Always returns 1.
int MAD_InitDecoder(MadDecoder* p) {
    int i;

    p->bFirst = 1;
    p->nFiles = 0;
    p->pLast = NULL;
    p->pFrames = NULL;
    p->nEnd = 0;
    for (i = 0; i < 6; i++) {
        p->apUsed[i] = NULL;
        p->apFree[i] = NULL;
    }
    return 1;
}

// Frees decoder p's frames: the pixels of every frame in its free and used lists, then the block of
// six frames (the decoder itself is freed by its owner, LLPict_Gc.c).
void MAD_CloseDecoder(MadDecoder* p) {
    int i;

    for (i = 0; i < 6; i++) {
        if (p->apFree[i] != NULL) {
            MAD_FreeFrame(p->apFree[i]);
        }
        if (p->apUsed[i] != NULL) {
            MAD_FreeFrame(p->apUsed[i]);
        }
    }
    if (p->pFrames != NULL) {
        StaticMem_Free(p->pFrames);
    }
}

// Decodes MAD file pFile into a frame taken from the free list. A 'MADk' key frame drops the old
// reference frame and is coded on its own; 'MADm' and 'MADe' frames are coded against the reference
// (pLast). A 'MADk' or 'MADm' frame becomes the new reference (the old one is released); a 'MADe'
// frame is only handed out. NULL when there is no free frame, no reference for a coded frame, or
// the kind is unknown.
PictFrame* MAD_DecodeFrame(MadDecoder* p, PictFile* pFile) {
    PictFrame* pFrame;
    u8* pRefY;
    u8* pRefU;
    u8* pRefV;
    u8* pY;
    u8* pU;
    u8* pV;
    int xc;
    int y;
    int x;

    if (MAD_GetFileKind(pFile) == 'MADk') {
        if (p->pLast != NULL) {
            MAD_ReleaseFrame(p, p->pLast);
            p->pLast = NULL;
        }
        pFrame = MAD_TakeReferenceFrame(p);
        if (pFrame == NULL) {
            return NULL;
        }
        MAD_initdecode(pFile->aData, 0, pFile->n15);
        // a key frame has no reference: it gets its own Y plane for all three
        pRefY = pRefU = pRefV = fn_8003024C(pFrame);
    } else {
        if (p->pLast != NULL) {
            pRefY = fn_8003024C(p->pLast);
            pRefU = fn_80030234(p->pLast);
            pRefV = fn_80030214(p->pLast);
        } else {
            return NULL;
        }
        if (MAD_GetFileKind(pFile) == 'MADm') {
            pFrame = MAD_TakeReferenceFrame(p);
        } else if (MAD_GetFileKind(pFile) == 'MADe') {
            pFrame = MAD_TakeOutputFrame(p);
        } else {
            return NULL;
        }
        if (pFrame == NULL) {
            return NULL;
        }
        MAD_initdecode(pFile->aData, 1, pFile->n15);
    }
    pY = fn_8003024C(pFrame);
    pU = fn_80030234(pFrame);
    pV = fn_80030214(pFrame);
    for (y = 0; y < p->nHeight; y += 16) {
        // a block is 16x16 Y pixels and 8x8 U and V ones
        for (x = 0, xc = 0; x < p->nWidth; xc += 8, x += 16) {
            MAD_decodemacroblock(&pRefY[x + y * p->nWidth], &pRefU[xc + y * p->nWidth / 4],
                                 &pRefV[xc + y * p->nWidth / 4], &pY[x + y * p->nWidth],
                                 &pU[xc + y * p->nWidth / 4], &pV[xc + y * p->nWidth / 4], p->nWidth);
        }
    }
    if (MAD_GetFileKind(pFile) == 'MADm') {
        if (p->pLast != NULL) {
            MAD_ReleaseFrame(p, p->pLast);
        }
        p->pLast = pFrame;
    } else if (MAD_GetFileKind(pFile) == 'MADk') {
        p->pLast = pFrame;
    }
    return pFrame;
}

// The file's kind ('MADk', 'MADm' or 'MADe'); no file counts as a key frame.
u32 MAD_GetFileKind(PictFile* pFile) {
    if (pFile != NULL) {
        return pFile->uMagic;
    }
    return 'MADk';
}

// The movie's next frame: decoded from pFile, or from the next file read when pFile is NULL (NULL
// when there is none). The first call takes the frame rate (16.16 frames a second; fFrameTime =
// 1000 / (rate / 65535) milliseconds) and the picture size from the file and allocates the six
// frames. The file is freed after. The frame comes with its references (MAD_ReleaseFrame gives one
// back).
PictFrame* MAD_GetNextFrame(MadDecoder* p, PictFile* pFile) {
    PictFrame* pFrame;
    PictFrame* pOut;
    int i;

    if (pFile == NULL) {
        pFile = MAD_ReadNextFile(p);
        if (pFile == NULL) {
            return NULL;
        }
    }
    if (p->bFirst) {
        p->nRate = pFile->uC;
        p->fFrameTime = 1000.0f / (p->nRate / 65535.0f);
        p->nWidth = pFile->nWidth;
        p->nHeight = pFile->nHeight;
        pFrame = StaticMem_Alloc(6 * sizeof(PictFrame), 1, 32, "rcmp_mad_codec.c", 473);
        p->pFrames = pFrame;
        for (i = 0; i < 6; i++) {
            MAD_AllocFrame(pFrame, p->nWidth, p->nHeight);
            MAD_AddFrameToList(p->apFree, pFrame);
            pFrame++;
        }
        p->bFirst = 0;
    }
    pOut = MAD_DecodeFrame(p, pFile);
    MAD_FreeFile(pFile);
    return pOut;
}

void MAD_FreeFile(PictFile* pFile) {
    if (pFile != NULL) {
        StaticMem_Free(pFile);
    }
}

// Puts pFrame in the first empty slot of a six-slot frame list (the decoder's apFree or apUsed);
// nothing when the list is full.
void MAD_AddFrameToList(PictFrame** apList, PictFrame* pFrame) {
    int i;

    for (i = 0; i < 6; i++) {
        if (apList[i] == NULL) {
            apList[i] = pFrame;
            return;
        }
    }
}

// The next MAD file from the read function (MAD_SetReadCallback), NULL when there is none; its rate
// (uC), width and height are swapped to big-endian and the decoder's file count goes up. The end
// count (nEnd) is never set: see the EA bug inside.
// port: the swaps assume a big-endian machine; a little-endian port reads the header as it is.
PictFile* MAD_ReadNextFile(MadDecoder* p) {
    PictFile* pFile = gpfnMadRead(gpMadReadArg);

    if (pFile == NULL) {
        return NULL;
    }
    p->nFiles++;
    // EA's test repeats the one above, so this is never reached
    if (pFile == NULL) {
        if (p->nEnd == 0) {
            p->nEnd = 1;
        } else {
            p->nEnd = 2;
        }
    }
    __stwbrx(pFile->uC, &pFile->uC, 0);
    pFile->nWidth = ((u16)pFile->nWidth >> 8) | (((u16)pFile->nWidth & 0xFF) << 8);
    // fake match: the height's redundant & 0xFF (the width without it gives other code)
    pFile->nHeight = (((u16)pFile->nHeight >> 8) & 0xFF) | (((u16)pFile->nHeight & 0xFF) << 8);
    return pFile;
}

// A free frame moved to the used list with 2 references: one for whoever the frame is handed to,
// one for its time as the reference frame (MAD_DecodeFrame, for 'MADk' and 'MADm'). NULL when none
// is free.
PictFrame* MAD_TakeReferenceFrame(MadDecoder* p) {
    PictFrame* pFrame = MAD_TakeFrameFromList(p->apFree);

    if (pFrame == NULL) {
        return NULL;
    }
    MAD_AddFrameToList(p->apUsed, pFrame);
    pFrame->nRefs = 2;
    return pFrame;
}

// Takes the first frame out of a six-slot frame list; NULL when the list is empty.
PictFrame* MAD_TakeFrameFromList(PictFrame** apList) {
    PictFrame* pFrame;
    int i;

    for (i = 0; i < 6; i++) {
        if (apList[i] != NULL) {
            pFrame = apList[i];
            apList[i] = NULL;
            return pFrame;
        }
    }
    return NULL;
}

// A free frame moved to the used list with 1 reference, for a 'MADe' frame that is handed out and
// never becomes the reference (MAD_DecodeFrame). NULL when none is free.
PictFrame* MAD_TakeOutputFrame(MadDecoder* p) {
    PictFrame* pFrame = MAD_TakeFrameFromList(p->apFree);

    if (pFrame == NULL) {
        return NULL;
    }
    MAD_AddFrameToList(p->apUsed, pFrame);
    pFrame->nRefs = 1;
    return pFrame;
}

// Gives back one reference to pFrame; at none left it goes back to the free list. The picture code
// gives back the frame it showed (LLPict_Gc.c), MAD_DecodeFrame the old reference.
void MAD_ReleaseFrame(MadDecoder* p, PictFrame* pFrame) {
    pFrame->nRefs--;
    if (pFrame->nRefs == 0) {
        MAD_RemoveFrameFromLists(p, pFrame);
        MAD_AddFrameToList(p->apFree, pFrame);
    }
}

// Take pFrame out of both lists.
void MAD_RemoveFrameFromLists(MadDecoder* p, PictFrame* pFrame) {
    int i;

    for (i = 0; i < 6; i++) {
        if (p->apUsed[i] == pFrame) {
            p->apUsed[i] = NULL;
        }
        if (p->apFree[i] == pFrame) {
            p->apFree[i] = NULL;
        }
    }
}

// 1 once the decoder's end count (nEnd) reaches 2. It never does: nEnd is never set
// (MAD_ReadNextFile's EA bug), so this always answers 0.
u8 MAD_IsAtEnd(MadDecoder* p) {
    return p->nEnd == 2;
}

// ---- the 'TEO ' and 'BALF' stream handlers ----

u8 gbCrAPBallLogoShown = 1;

f32 gfCrAPBallOffsetZ = -2.0f;       // }  and z
f32 gfCrAPBallScaleX = 1.0f;        // } the ball's scale on each axis
f32 gfCrAPBallScaleY = 1.0f;        // }
f32 gfCrAPBallScaleZ = 1.0f;        // }
char gszCrAPBallLogoTex[] = "logoea";

void FE_CrAPBall_LoadBALF(UStreamObject* pObject);
void FE_CrAPBall_LoadTEO(UStreamObject* arg0);
void LLMath_CopyMat44(f32 (*pSrc)[4], f32 (*pDst)[4]);
void LLMath_IdentifyMat(f32 (*pMtx)[4]);                   // identity
void fn_8000C5A4(f32 (*pMtx)[4]);
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
void FE_CrAPBall_LoadTEO(UStreamObject* arg0) {
    if (fn_8000B508(arg0) == 0) {
        (*(UObjModel**)((u8*)(arg0) + 4)) = fn_80045D80(arg0->pData);
        (*(void (**)(void*))((u8*)(arg0) + 8)) = FE_CrAPBall_FreeTEO;
        fn_8000B4B8(arg0);
    }
}

// Frees the model FE_CrAPBall_LoadTEO built for a 'TEO ' object (arg0: the object): its root
// (fn_800075CC), then the model.
void FE_CrAPBall_FreeTEO(void* arg0) {
    void* temp_r31;

    temp_r31 = (*(void**)((u8*)(arg0) + 4));
    fn_800075CC(*(UObjModelRoot**)((u8*)(temp_r31) + 0x10));
    StaticMem_Free(temp_r31);
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
    fn_8000C5A4(pObj->m0);
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
// bone's offset (0, 0, -2) with scale 1 on each axis (gfCrAPBallOffsetX, gfCrAPBallOffsetY, gfCrAPBallOffsetZ;
// gfCrAPBallScaleX..gfCrAPBallScaleZ).
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
