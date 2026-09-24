// rcmp_mad_codec.c (EA's name, from its asserts; also in EA's 2002 source tree): the decoder for
// the 'MAD' movie format (TW06's rcmp folder splits it into maddec.c and madidct.c): the block
// decoder and inverse DCT, the movie decoder that hands out frames (MadDecoder, llpict.h), and
// the 'TEO '/'BALF' stream handlers after it.
//
// A MAD file is a picture: 'MADk' a key frame, 'MADm' and 'MADe' frames coded against the last
// key or 'MADm' frame. The coded data is read 16 bits at a time, little-endian, into a 32-bit
// bit buffer.

#include "engine.h"
#include "dynobj.h"
#include "llpict.h"

s32 lbl_802821A8;               // the decoder's tables are built
s32 lbl_802821AC;               // 0: a key frame, 1: coded against a reference
s32 lbl_802821B0;               // bits left in lbl_802821B4
u32 lbl_802821B4;               // the bit buffer, next bit at the top
u8* lbl_802821B8;               // the next coded byte
void* lbl_802821C0;             // what the read function is given
PictFile* (*lbl_802821C4)(void* pArg);   // reads the next MAD file

// MPEG-1's default intra quantizer matrix
const s32 lbl_80184A68[64] = {
    8,  16, 19, 22, 26, 27, 29, 34,
    16, 16, 22, 24, 27, 29, 34, 37,
    19, 22, 26, 27, 29, 34, 34, 38,
    22, 22, 26, 27, 29, 34, 37, 40,
    22, 26, 27, 29, 32, 35, 40, 48,
    26, 27, 29, 32, 35, 40, 48, 58,
    26, 27, 29, 34, 38, 46, 56, 69,
    27, 29, 35, 38, 46, 56, 69, 83,
};

// the scaled IDCT's factors for each coefficient (0x2000 = 1.0)
s32 lbl_80190FE0[64] = {
    0x2000, 0x1712, 0x187E, 0x1B37, 0x2000, 0x28BA, 0x3B21, 0x73FC,
    0x1712, 0x10A2, 0x11A8, 0x139F, 0x1712, 0x1D5D, 0x2AA1, 0x539F,
    0x187E, 0x11A8, 0x12BF, 0x14D4, 0x187E, 0x1F2C, 0x2D41, 0x58C5,
    0x1B37, 0x139F, 0x14D4, 0x1725, 0x1B37, 0x22A3, 0x3249, 0x62A3,
    0x2000, 0x1712, 0x187E, 0x1B37, 0x2000, 0x28BA, 0x3B21, 0x73FC,
    0x28BA, 0x1D5D, 0x1F2C, 0x22A3, 0x28BA, 0x33D6, 0x4B42, 0x939F,
    0x3B21, 0x2AA1, 0x2D41, 0x3249, 0x3B21, 0x4B42, 0x6D41, 0xD650,
    0x73FC, 0x539F, 0x58C5, 0x62A3, 0x73FC, 0x939F, 0xD650, 0x1A463,
};
u8 lbl_801F6858[512];           // a pixel value's clamp to 0..255, by its low 9 bits
s32 lbl_801F7058[64];           // the quantizer for this picture
u32 lbl_801F7158[64];           // looked up by the buffer's top 6 bits
s32 lbl_801F8358[64];

void fn_800B769C(void);
u32 fn_800B8984(u8* pData, int nBytes);
s32 fn_800B8A04(s32 a, s32 b);
void fn_800B8618(u8* pRefY, u8* pRefU, u8* pRefV, u8* pY, u8* pU, u8* pV);
u32 fn_800B94B0(PictFile* pFile);
void fn_800B95FC(PictFile* pFile);
void fn_800B9624(PictFrame** apList, PictFrame* pFrame);
PictFile* fn_800B965C(MadDecoder* p);
PictFrame* fn_800B9700(MadDecoder* p);
PictFrame* fn_800B9760(PictFrame** apList);
PictFrame* fn_800B97A8(MadDecoder* p);
void fn_800B9808(MadDecoder* p, PictFrame* pFrame);
void fn_800B9864(MadDecoder* p, PictFrame* pFrame);

// Drop nBits bits from the buffer, refilling 16 at a time.
void fn_800B7D80(int nBits) {
    lbl_802821B0 -= nBits;
    lbl_802821B4 <<= nBits;
    if (lbl_802821B0 < 16) {
        lbl_802821B4 |= fn_800B8984(lbl_802821B8, 2) << (16 - lbl_802821B0);
        lbl_802821B0 += 16;
        lbl_802821B8 += 2;
    }
}

// The code at the top of the buffer: its entry's low byte is its length in bits.
s32 fn_800B7DF4(void) {
    s32 nCode = lbl_801F7158[lbl_802821B4 >> 26];
    fn_800B7D80(nCode & 0xFF);
    return nCode >> 22;
}

// Fill an 8x8 block (rows nStride words apart) with lbl_801F8358[0].
void fn_800B7E38(s32* pOut, int nStride) {
    int i;

    for (i = 0; i < 8; i++) {
        pOut[0] = lbl_801F8358[0];
        pOut[1] = lbl_801F8358[0];
        pOut[2] = lbl_801F8358[0];
        pOut[3] = lbl_801F8358[0];
        pOut[4] = lbl_801F8358[0];
        pOut[5] = lbl_801F8358[0];
        pOut[6] = lbl_801F8358[0];
        pOut[7] = lbl_801F8358[0];
        pOut += nStride;
    }
}

// An 8x8 block of pixels into a 16-wide block of 16.16 values, nAdd added to each.
void fn_800B8064(const u8* pSrc, int nStride, s32* pOut, int nAdd) {
    int i;

    for (i = 0; i < 8; i++) {
        pOut[0] = (pSrc[0] + nAdd) << 16;
        pOut[1] = (pSrc[1] + nAdd) << 16;
        pOut[2] = (pSrc[2] + nAdd) << 16;
        pOut[3] = (pSrc[3] + nAdd) << 16;
        pOut[4] = (pSrc[4] + nAdd) << 16;
        pOut[5] = (pSrc[5] + nAdd) << 16;
        pOut[6] = (pSrc[6] + nAdd) << 16;
        pOut[7] = (pSrc[7] + nAdd) << 16;
        pSrc += nStride;
        pOut += 16;
    }
}

// The same into an 8-wide block.
void fn_800B8180(const u8* pSrc, int nStride, s32* pOut, int nAdd) {
    int i;

    for (i = 0; i < 8; i++) {
        pOut[0] = (pSrc[0] + nAdd) << 16;
        pOut[1] = (pSrc[1] + nAdd) << 16;
        pOut[2] = (pSrc[2] + nAdd) << 16;
        pOut[3] = (pSrc[3] + nAdd) << 16;
        pOut[4] = (pSrc[4] + nAdd) << 16;
        pOut[5] = (pSrc[5] + nAdd) << 16;
        pOut[6] = (pSrc[6] + nAdd) << 16;
        pOut[7] = (pSrc[7] + nAdd) << 16;
        pSrc += nStride;
        pOut += 8;
    }
}

// A 16x16 block of 16.16 values back to pixels, clamped through lbl_801F6858.
void fn_800B829C(const s32* pIn, u8* pOut, int nStride) {
    int i;

    for (i = 0; i < 16; i++) {
        pOut[0] = lbl_801F6858[(pIn[0] >> 16) & 0x1FF];
        pOut[1] = lbl_801F6858[(pIn[1] >> 16) & 0x1FF];
        pOut[2] = lbl_801F6858[(pIn[2] >> 16) & 0x1FF];
        pOut[3] = lbl_801F6858[(pIn[3] >> 16) & 0x1FF];
        pOut[4] = lbl_801F6858[(pIn[4] >> 16) & 0x1FF];
        pOut[5] = lbl_801F6858[(pIn[5] >> 16) & 0x1FF];
        pOut[6] = lbl_801F6858[(pIn[6] >> 16) & 0x1FF];
        pOut[7] = lbl_801F6858[(pIn[7] >> 16) & 0x1FF];
        pOut[8] = lbl_801F6858[(pIn[8] >> 16) & 0x1FF];
        pOut[9] = lbl_801F6858[(pIn[9] >> 16) & 0x1FF];
        pOut[10] = lbl_801F6858[(pIn[10] >> 16) & 0x1FF];
        pOut[11] = lbl_801F6858[(pIn[11] >> 16) & 0x1FF];
        pOut[12] = lbl_801F6858[(pIn[12] >> 16) & 0x1FF];
        pOut[13] = lbl_801F6858[(pIn[13] >> 16) & 0x1FF];
        pOut[14] = lbl_801F6858[(pIn[14] >> 16) & 0x1FF];
        pOut[15] = lbl_801F6858[(pIn[15] >> 16) & 0x1FF];
        pIn += 16;
        pOut += nStride;
    }
}

// An 8x8 block of 16.16 values back to pixels.
void fn_800B83DC(const s32* pIn, u8* pOut, int nStride) {
    int i;

    for (i = 0; i < 8; i++) {
        pOut[0] = lbl_801F6858[(pIn[0] >> 16) & 0x1FF];
        pOut[1] = lbl_801F6858[(pIn[1] >> 16) & 0x1FF];
        pOut[2] = lbl_801F6858[(pIn[2] >> 16) & 0x1FF];
        pOut[3] = lbl_801F6858[(pIn[3] >> 16) & 0x1FF];
        pOut[4] = lbl_801F6858[(pIn[4] >> 16) & 0x1FF];
        pOut[5] = lbl_801F6858[(pIn[5] >> 16) & 0x1FF];
        pOut[6] = lbl_801F6858[(pIn[6] >> 16) & 0x1FF];
        pOut[7] = lbl_801F6858[(pIn[7] >> 16) & 0x1FF];
        pIn += 8;
        pOut += nStride;
    }
}

// Start a picture: pData is its coded data, nMode 0 for a key frame and 1 for one coded against
// a reference, nQuant its quality.
void fn_800B8528(u8* pData, int nMode, int nQuant) {
    int i;

    if (lbl_802821A8 == 0) {
        fn_800B769C();
    }
    lbl_802821B4 = (fn_800B8984(pData, 2) << 16) | fn_800B8984(pData + 2, 2);
    lbl_802821B0 = 32;
    lbl_802821B8 = pData + 4;
    lbl_802821AC = nMode;
    lbl_801F7058[0] = fn_800B8A04(lbl_80184A68[0] << 16, lbl_80190FE0[0]);
    for (i = 1; i < 64; i++) {
        lbl_801F7058[i] = fn_800B8A04((nQuant * lbl_80184A68[i]) << 13, lbl_80190FE0[i]);
    }
}

// nBytes bytes at pData, little-endian.
u32 fn_800B8984(u8* pData, int nBytes) {
    if (nBytes == 1) {
        return pData[0];
    }
    if (nBytes == 2) {
        return pData[0] | (pData[1] << 8);
    }
    if (nBytes == 3) {
        return pData[0] | ((pData[1] << 8) | (pData[2] << 16));
    }
    if (nBytes == 4) {
        return pData[0] | ((pData[1] << 8) | ((pData[2] << 16) | (pData[3] << 24)));
    }
    return 0;
}

// a * b in 16.16 fixed point, rounded.
s32 fn_800B8A04(s32 a, s32 b) {
    return ((s64)a * b + 0x8000) >> 16;
}

// The same as fn_800B7D80.
void fn_800B8A2C(int nBits) {
    lbl_802821B0 -= nBits;
    lbl_802821B4 <<= nBits;
    if (lbl_802821B0 < 16) {
        lbl_802821B4 |= fn_800B8984(lbl_802821B8, 2) << (16 - lbl_802821B0);
        lbl_802821B0 += 16;
        lbl_802821B8 += 2;
    }
}

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

// Decode pFile into a new frame. A 'MADm' frame becomes the next reference; the frame is NULL
// when there is no free one, no reference or an unknown kind.
PictFrame* fn_800B928C(MadDecoder* p, PictFile* pFile) {
    PictFrame* pFrame;
    u8* pRefY;
    u8* pRefU;
    u8* pRefV;
    u8* pY;
    u8* pU;
    u8* pV;
    int y;
    int x;
    int xc;

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
                        &pU[xc + y * p->nWidth / 4], &pV[xc + y * p->nWidth / 4]);
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
    pFile->nWidth = (pFile->nWidth << 8) | ((pFile->nWidth >> 8) & 0xFF);
    pFile->nHeight = (pFile->nHeight << 8) | ((pFile->nHeight >> 8) & 0xFF);
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

void* lbl_802821D0;
TexBank* lbl_802821D4;          // from the 'BALF' object
UObject* lbl_802821D8;          // } made from the 'TEO ' objects 10040,
UObject* lbl_802821DC;          // } 10030
UObject* lbl_802821E0;          // } and 10000
u8 lbl_802814E8 = 1;

void fn_800B99BC(UStreamObject* pObject);
void fn_800B99FC(UStreamObject* arg0);

void fn_800B9944(void) {
    UStream_RegisterHandler('TEO ', fn_800B99FC);
    UStream_RegisterHandler('BALF', fn_800B99BC);
}

void fn_800B9988(void) {
    UStream_UnregisterHandler('TEO ');
    UStream_UnregisterHandler('BALF');
}

// A 'BALF' object is a texture bank.
void fn_800B99BC(UStreamObject* pObject) {
    lbl_802821D4 = fn_8000FB88(pObject, NULL, 0);
    fn_80009E70(pObject);
}

// ---- sweep code (not yet cleaned up) ----

s32 fn_80045D80(s32);
void fn_800B9A50(void* arg0);
s32 fn_800075CC(s32);

void fn_800B99FC(UStreamObject* arg0) {
    if (fn_8000B508(arg0) == 0) {
        (*(s32*)((u8*)(arg0) + 4)) = fn_80045D80((*(s32*)((u8*)(arg0) + 0)));
        (*(void (**)(void*))((u8*)(arg0) + 8)) = fn_800B9A50;
        fn_8000B4B8(arg0);
    }
}

void fn_800B9A50(void* arg0) {
    void* temp_r31;

    temp_r31 = (*(void**)((u8*)(arg0) + 4));
    fn_800075CC((*(s32*)((u8*)(temp_r31) + 0x10)));
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
