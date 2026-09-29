// rcmp_mad_codec.c (EA's name: MAD_AllocFrame and MAD_GetNextFrame tag their allocations with EA's
// file name "rcmp_mad_codec.c", the only copy of that string in the game; TW2005 keeps
// Golf/Rcmp/rcmp_mad_codec.c, and EA's rcmp package, as NFSMW links it, has rcmp_mad_codec.cpp
// apart from maddec, maddeca and madidct): the MAD movie decoder's frame handling (MadDecoder,
// llpict.h): reading the movie's files, six frames handed out with references, and decoding each
// file into one (the block decoder, MAD_decodemacroblock, is in maddec.c).
// Its data: the string (.data), the read function and its argument (.sbss 0x802821C0..0x802821C8,
// on the 8-aligned address after maddec.c's padding) and MAD_GetNextFrame's constants (.sdata2
// 0x802841C8..0x802841D8). The Create-A-Player ball's code after it is a unit of its own
// (Code800B9944.c).

#include "engine.h"
#include "dynobj.h"
#include "character.h"
#include "camera.h"
#include "terrain.h"
#include "llpict.h"

// Defined here, last address first (CodeWarrior lays out .sbss in reverse).
PictFile* (*gpfnMadRead)(void* pArg);   // reads the movie's next MAD file (MAD_SetReadCallback)
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
void MAD_RemoveFrameFromLists(MadDecoder* p, PictFrame* pFrame);

// Sets the function (and the argument it is given) that MAD_ReadNextFile takes the movie's MAD
// files from; LLPict_Gc.c's movie set-up passes it on (Pict_OpenMovie).
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
        pRefY = pRefU = pRefV = PictFrame_GetPlaneY(pFrame);
    } else {
        if (p->pLast != NULL) {
            pRefY = PictFrame_GetPlaneY(p->pLast);
            pRefU = PictFrame_GetPlaneU(p->pLast);
            pRefV = PictFrame_GetPlaneV(p->pLast);
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
    pY = PictFrame_GetPlaneY(pFrame);
    pU = PictFrame_GetPlaneU(pFrame);
    pV = PictFrame_GetPlaneV(pFrame);
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
    // EA bug: this test repeats the one above, so the end count is never set: nEnd stays 0,
    // MAD_IsAtEnd never answers 1, and LLVideo.c stops a movie only once it is starved (bStarved)
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
