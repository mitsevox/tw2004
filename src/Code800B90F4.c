// Code800B90F4.c (our name): own unit, its .sbss starts on the 8-aligned address after
// rcmp_mad_codec.c's padding at 0x802821BC..0x802821C0 and ends padded at 0x802821E4..0x802821E8

#include "engine.h"
#include "dynobj.h"
#include "character.h"
#include "camera.h"
#include "terrain.h"
#include "llpict.h"

// Defined here, last address first (CodeWarrior lays out .sbss in reverse).
UObject* lbl_802821E0;          // } and 10000
UObject* lbl_802821DC;          // } 10030
UObject* lbl_802821D8;          // } made from the 'TEO ' objects 10040,
TexBank* lbl_802821D4;          // from the 'BALF' object
void* lbl_802821D0;
f32 lbl_802821CC;               // }
f32 lbl_802821C8;               // } the held ball's offset in its bone (x, y); never set, so 0
PictFile* (*lbl_802821C4)(void* pArg);   // reads the next MAD file
void* lbl_802821C0;             // what the read function is given

void fn_800B8528(u8* pData, int nMode, int nQuant);
void fn_800B8618(u8* pRefY, u8* pRefU, u8* pRefV, u8* pY, u8* pU, u8* pV, int nStride);
u32 fn_800B94B0(PictFile* pFile);
void fn_800B95FC(PictFile* pFile);
void fn_800B9624(PictFrame** apList, PictFrame* pFrame);
PictFile* fn_800B965C(MadDecoder* p);
PictFrame* fn_800B9700(MadDecoder* p);
PictFrame* fn_800B9760(PictFrame** apList);
PictFrame* fn_800B97A8(MadDecoder* p);
void fn_800B9808(MadDecoder* p, PictFrame* pFrame);
void fn_800B9864(MadDecoder* p, PictFrame* pFrame);

// The read function the decoder takes its MAD files from.
void fn_800B90F4(PictFile* (*pfnRead)(void* pArg), void* pArg) {
    lbl_802821C4 = pfnRead;
    lbl_802821C0 = pArg;
}

void fn_800B9100(PictFrame* pFrame, int nWidth, int nHeight) {
    pFrame->nRefs = 0;
    pFrame->pPixels = fn_80009B34((u32)(nHeight * nWidth * 3) >> 1, 1, 32, "rcmp_mad_codec.c", 79);
    pFrame->nWidth = nWidth;
    pFrame->nHeight = nHeight;
}

void fn_800B9178(PictFrame* pFrame) {
    if (pFrame->pPixels != NULL) {
        fn_80009E70(pFrame->pPixels);
        pFrame->pPixels = NULL;
    }
}

int fn_800B91B8(MadDecoder* p) {
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

void fn_800B920C(MadDecoder* p) {
    int i;

    for (i = 0; i < 6; i++) {
        if (p->apFree[i] != NULL) {
            fn_800B9178(p->apFree[i]);
        }
        if (p->apUsed[i] != NULL) {
            fn_800B9178(p->apUsed[i]);
        }
    }
    if (p->pFrames != NULL) {
        fn_80009E70(p->pFrames);
    }
}

// Decode pFile into a new frame. A 'MADk' or 'MADm' frame becomes the next reference; the frame is
// NULL when there is no free one, no reference or an unknown kind.
PictFrame* fn_800B928C(MadDecoder* p, PictFile* pFile) {
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

    if (fn_800B94B0(pFile) == 'MADk') {
        if (p->pLast != NULL) {
            fn_800B9808(p, p->pLast);
            p->pLast = NULL;
        }
        pFrame = fn_800B9700(p);
        if (pFrame == NULL) {
            return NULL;
        }
        fn_800B8528(pFile->aData, 0, pFile->n15);
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
        if (fn_800B94B0(pFile) == 'MADm') {
            pFrame = fn_800B9700(p);
        } else if (fn_800B94B0(pFile) == 'MADe') {
            pFrame = fn_800B97A8(p);
        } else {
            return NULL;
        }
        if (pFrame == NULL) {
            return NULL;
        }
        fn_800B8528(pFile->aData, 1, pFile->n15);
    }
    pY = fn_8003024C(pFrame);
    pU = fn_80030234(pFrame);
    pV = fn_80030214(pFrame);
    for (y = 0; y < p->nHeight; y += 16) {
        // a block is 16x16 Y pixels and 8x8 U and V ones
        for (x = 0, xc = 0; x < p->nWidth; xc += 8, x += 16) {
            fn_800B8618(&pRefY[x + y * p->nWidth], &pRefU[xc + y * p->nWidth / 4],
                        &pRefV[xc + y * p->nWidth / 4], &pY[x + y * p->nWidth],
                        &pU[xc + y * p->nWidth / 4], &pV[xc + y * p->nWidth / 4], p->nWidth);
        }
    }
    if (fn_800B94B0(pFile) == 'MADm') {
        if (p->pLast != NULL) {
            fn_800B9808(p, p->pLast);
        }
        p->pLast = pFrame;
    } else if (fn_800B94B0(pFile) == 'MADk') {
        p->pLast = pFrame;
    }
    return pFrame;
}

// The file's kind ('MADk', 'MADm' or 'MADe'); no file counts as a key frame.
u32 fn_800B94B0(PictFile* pFile) {
    if (pFile != NULL) {
        return pFile->uMagic;
    }
    return 'MADk';
}

// The next frame: decoded from pFile, or from the next file read when pFile is NULL. The first
// call allocates the six frames.
PictFrame* fn_800B94CC(MadDecoder* p, PictFile* pFile) {
    PictFrame* pFrame;
    PictFrame* pOut;
    int i;

    if (pFile == NULL) {
        pFile = fn_800B965C(p);
        if (pFile == NULL) {
            return NULL;
        }
    }
    if (p->bFirst) {
        p->nRate = pFile->uC;
        p->fFrameTime = 1000.0f / (p->nRate / 65535.0f);
        p->nWidth = pFile->nWidth;
        p->nHeight = pFile->nHeight;
        pFrame = fn_80009B34(6 * sizeof(PictFrame), 1, 32, "rcmp_mad_codec.c", 473);
        p->pFrames = pFrame;
        for (i = 0; i < 6; i++) {
            fn_800B9100(pFrame, p->nWidth, p->nHeight);
            fn_800B9624(p->apFree, pFrame);
            pFrame++;
        }
        p->bFirst = 0;
    }
    pOut = fn_800B928C(p, pFile);
    fn_800B95FC(pFile);
    return pOut;
}

void fn_800B95FC(PictFile* pFile) {
    if (pFile != NULL) {
        fn_80009E70(pFile);
    }
}

// Put pFrame in the first empty slot of a list.
void fn_800B9624(PictFrame** apList, PictFrame* pFrame) {
    int i;

    for (i = 0; i < 6; i++) {
        if (apList[i] == NULL) {
            apList[i] = pFrame;
            return;
        }
    }
}

// The next MAD file from the read function, its header swapped to big-endian.
// port: the swaps assume a big-endian machine; a little-endian port reads the header as it is.
PictFile* fn_800B965C(MadDecoder* p) {
    PictFile* pFile = lbl_802821C4(lbl_802821C0);

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

// A free frame, handed out with 2 references.
PictFrame* fn_800B9700(MadDecoder* p) {
    PictFrame* pFrame = fn_800B9760(p->apFree);

    if (pFrame == NULL) {
        return NULL;
    }
    fn_800B9624(p->apUsed, pFrame);
    pFrame->nRefs = 2;
    return pFrame;
}

// Take the first frame out of a list.
PictFrame* fn_800B9760(PictFrame** apList) {
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

// A free frame, handed out with 1 reference.
PictFrame* fn_800B97A8(MadDecoder* p) {
    PictFrame* pFrame = fn_800B9760(p->apFree);

    if (pFrame == NULL) {
        return NULL;
    }
    fn_800B9624(p->apUsed, pFrame);
    pFrame->nRefs = 1;
    return pFrame;
}

// Drop a reference to pFrame; at none it goes back to the free list.
void fn_800B9808(MadDecoder* p, PictFrame* pFrame) {
    pFrame->nRefs--;
    if (pFrame->nRefs == 0) {
        fn_800B9864(p, pFrame);
        fn_800B9624(p->apFree, pFrame);
    }
}

// Take pFrame out of both lists.
void fn_800B9864(MadDecoder* p, PictFrame* pFrame) {
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

u8 fn_800B9930(MadDecoder* p) {
    return p->nEnd == 2;
}

// ---- the 'TEO ' and 'BALF' stream handlers ----

u8 lbl_802814E8 = 1;

f32 lbl_802814EC = -2.0f;       // }  and z
f32 lbl_802814F0 = 1.0f;        // } the ball's scale on each axis
f32 lbl_802814F4 = 1.0f;        // }
f32 lbl_802814F8 = 1.0f;        // }
char lbl_802814FC[] = "logoea";

void fn_800B99BC(UStreamObject* pObject);
void fn_800B99FC(UStreamObject* arg0);
void fn_8000A0E8(f32 (*pSrc)[4], f32 (*pDst)[4]);
void fn_8000ADC0(f32 (*pMtx)[4]);                   // identity
void fn_8000C5A4(f32 (*pMtx)[4]);
void fn_800BADF8(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows);
void fn_800BAE5C(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows);
void fn_8001EB8C(Character* pChar, int nBone, f32* pPos);
void fn_800140E8(int a, int nWidth, int nHeight, int nField, int b, int c);
int  fn_8001005C(TexBank* pBank, u64 uHash);       // LLTex.c: the texture's index, or 0x80000000

void fn_800B9944(void) {
    Stream_RegisterLoadChunkCallback('TEO ', fn_800B99FC);
    Stream_RegisterLoadChunkCallback('BALF', fn_800B99BC);
}

void fn_800B9988(void) {
    Stream_UnregisterLoadChunkCallback('TEO ');
    Stream_UnregisterLoadChunkCallback('BALF');
}

// A 'BALF' object is a texture bank.
void fn_800B99BC(UStreamObject* pObject) {
    lbl_802821D4 = fn_8000FB88(pObject, NULL, 0);
    fn_80009E70(pObject);
}

// ---- sweep code (not yet cleaned up) ----

void fn_800B9A50(void* arg0);

void fn_800B99FC(UStreamObject* arg0) {
    if (fn_8000B508(arg0) == 0) {
        (*(UObjModel**)((u8*)(arg0) + 4)) = fn_80045D80(arg0->pData);
        (*(void (**)(void*))((u8*)(arg0) + 8)) = fn_800B9A50;
        fn_8000B4B8(arg0);
    }
}

void fn_800B9A50(void* arg0) {
    void* temp_r31;

    temp_r31 = (*(void**)((u8*)(arg0) + 4));
    fn_800075CC(*(UObjModelRoot**)((u8*)(temp_r31) + 0x10));
    fn_80009E70(temp_r31);
}

// ---- end of sweep code ----

void fn_800B9A88(void) {
    lbl_802821E0 = NULL;
    lbl_802821DC = NULL;
    lbl_802821D8 = NULL;
    lbl_802821D4 = NULL;
    lbl_802821D0 = NULL;
    lbl_802814E8 = 1;
}

void fn_800B9AAC(void) {
    if (lbl_802821E0 != NULL) {
        fn_80048860(lbl_802821E0);
    }
    lbl_802821E0 = NULL;
    if (lbl_802821DC != NULL) {
        fn_80048860(lbl_802821DC);
    }
    lbl_802821DC = NULL;
    if (lbl_802821D8 != NULL) {
        fn_80048860(lbl_802821D8);
    }
    lbl_802821D8 = NULL;
    if (lbl_802821D4 != NULL) {
        fn_8000FFAC(lbl_802821D4);
        fn_80009E70(lbl_802821D4);
        lbl_802821D4 = NULL;
    }
    if (lbl_802821D0 != NULL) {
        fn_80009E70(lbl_802821D0);
        lbl_802821D0 = NULL;
    }
}

// The three objects, made from their 'TEO ' models once those have streamed in.
// port: a 'TEO ' object's UStreamObject.uUnk4 holds its model (fn_800B99FC stores it there).
void fn_800B9B48(void) {
    UStreamObject* pObject;

    if (lbl_802821E0 == NULL) {
        pObject = fn_8000B70C('TEO ', 10000);
        if (pObject != NULL) {
            lbl_802821E0 = fn_80048808((UObjModel*)pObject->uUnk4);
        }
    }
    if (lbl_802821DC == NULL) {
        pObject = fn_8000B70C('TEO ', 10030);
        if (pObject != NULL) {
            lbl_802821DC = fn_80048808((UObjModel*)pObject->uUnk4);
        }
    }
    if (lbl_802821D8 == NULL) {
        pObject = fn_8000B70C('TEO ', 10040);
        if (pObject != NULL) {
            lbl_802821D8 = fn_80048808((UObjModel*)pObject->uUnk4);
        }
    }
}

// Draw pObj turned by mBone and scaled by mScale, at pPos in the create-a-player view; its
// matrices are put back after.
void fn_800B9BF4(UObject* pObj, f32 (*mBone)[4], f32 (*mScale)[4], f32* pPos) {
    f32 m0[4][4];
    f32 m40[4][4];
    f32 m80[4][4];

    fn_8000A0E8(pObj->m0, m0);
    fn_8000A0E8(pObj->m40, m40);
    fn_8000A0E8(pObj->m80, m80);
    fn_800BAE5C(mBone, pObj->m0, pObj->m0, 3);
    fn_800BAE5C(mScale, pObj->m40, pObj->m40, 3);
    fn_8000C5A4(pObj->m0);
    Vec_Copy(pPos, pObj->m80[3]);
    pObj->m80[3][3] = 1.0f;
    fn_800BADF8(lbl_80281EE0->mC0, pObj->m80, pObj->m80, 4);
    fn_8000ADC0(pObj->m0);
    fn_80048894(pObj);
    fn_8000A0E8(m0, pObj->m0);
    fn_8000A0E8(m40, pObj->m40);
    fn_8000A0E8(m80, pObj->m80);
}

// Draw the ball in the create-a-player golfer's hand (bone 0x54), when he holds it: to the
// 384x528 target when bTarget, else to the screen.
void fn_800B9CF0(u8 bTarget) {
    f32 vPos[4];
    f32 mScale[4][4];
    f32 (*mBone)[4];

    if (fn_8001DBF4(lbl_80281EE0->pB4->pChar)) {
        vPos[0] = lbl_802821C8;
        vPos[1] = lbl_802821CC;
        vPos[2] = lbl_802814EC;
        vPos[3] = 1.0f;
        fn_8001EB8C(lbl_80281EE0->pB4->pChar, 0x54, vPos);
        mBone = fn_8001ED08(lbl_80281EE0->pB4->pChar, 0x54);
        fn_8000ADC0(mScale);
        mScale[0][0] = lbl_802814F0;
        mScale[1][1] = lbl_802814F4;
        mScale[2][2] = lbl_802814F8;
        if (bTarget) {
            fn_800140E8(1, 0x180, 0x210, 0, 1, 1);
        } else {
            fn_800140E8(0, 0x200, 0x1C0, lbl_80281B88 & 1, 1, 1);
        }
        fn_80013EEC(fn_8001614C());
        fn_80035118(4, 5);
        fn_80012F50(0, 6, 0x80);
        fn_80012F34(1);
        fn_80012EF8();
        if (lbl_802821E0 != NULL) {
            fn_800B9BF4(lbl_802821E0, mBone, mScale, vPos);
        }
        if (lbl_802814E8) {
            if (lbl_802821DC != NULL) {
                fn_800B9BF4(lbl_802821DC, mBone, mScale, vPos);
            }
            if (lbl_802821D8 != NULL) {
                fn_800B9BF4(lbl_802821D8, mBone, mScale, vPos);
            }
        }
        fn_80035240(NULL);
        fn_800140E8(0, 0x200, 0x1C0, lbl_80281B88 & 1, 8, 1);
        fn_80013EEC(fn_8001614C());
        fn_80012EF8();
    }
}

// Put the logo szBall on the held ball: its texture's levels are copied over the "logoea"
// texture. NULL hides the logo layers instead.
void fn_800B9EB8(char* szBall) {
    u64       uLogo;
    u64       uSlot;
    TexBank*  pSlotBank;
    TexEntry* pSlot;
    TexEntry* pLogo;
    int       nLogo;
    int       i;

    if (lbl_802821D4 == NULL) {
        return;
    }
    if (szBall == NULL) {
        lbl_802814E8 = 0;
        return;
    }
    fn_800CB700(&uLogo, szBall);
    fn_800CB700(&uSlot, lbl_802814FC);
    fn_800102DC(uSlot, &pSlotBank, &pSlot);
    if (pSlotBank == NULL || pSlot == NULL) {
        return;
    }
    nLogo = fn_8001005C(lbl_802821D4, uLogo);
    if (nLogo == (int)0x80000000) {
        return;
    }
    lbl_802814E8 = 1;
    pLogo = &lbl_802821D4->p8[nLogo];
    for (i = 0; i < pLogo->n41; i++) {
        Mem_cpy(pSlotBank->p18 + pSlot->aMips[i].uPixels, lbl_802821D4->p18 + pLogo->aMips[i].uPixels,
                pLogo->aMips[i].nC * 16);
    }
}
