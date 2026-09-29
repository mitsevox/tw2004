// LLPict_Gc.c (EA's name, from its asserts; the 2002 source tree has its Xbox twin,
// Legacy\LL\Xbox\LLPict_Xbox.c): the GameCube side of EA's pictures (LLPict, llpict.h: Y, U and V
// planes drawn as three I8 textures). It makes pictures from "MADk" files for the menus and
// loading screens, and shows a movie's decoded frames for LLVideo.c: each plane is copied into the
// GPU's 8 x 4 tile order.

#include "llpict.h"
#include "core/startup.h"

void Pict_TilePlane(u8* pSrc, u8* pDst, int nWidth, int nHeight);
void Pict_InitTextures(LLPict* pPict);
void Pict_TilePlaneInPlace(u8* pPlane, void* pWork, int nWidth, int nHeight);

void* gPictWorkBuffer;          // 2048 bytes from Pict_InitModule: a band of four rows being tiled
void** gpPictWorkBuffer = &gPictWorkBuffer;     // every use of the work buffer goes through it

void PictInt_InitModule(void);
void PictInt_CloseModule(void);

// At boot (gomainloop.c): starts LLPictInt.c (nothing to do) and allocates the 2048-byte work
// buffer Pict_CreateFromMemory tiles pictures through (gPictWorkBuffer).
void Pict_InitModule(void) {
    void* pBuffer;
    PictInt_InitModule();
    pBuffer = StaticMem_Alloc(2048, 2, 32, "LLPict_Gc.c", 68);
    *gpPictWorkBuffer = pBuffer;
}

// At shutdown: closes LLPictInt.c (nothing to do) and frees the work buffer Pict_InitModule
// allocated.
void Pict_CloseModule(void) {
    PictInt_CloseModule();
    StaticMem_Free(*gpPictWorkBuffer);
}

// Pict_TilePlane in place: rearranges an nWidth x nHeight plane of bytes (nHeight a multiple of 4)
// into GameCube I8 tile order. Each band of four rows (nWidth * 4 bytes: the 2048-byte work buffer
// holds rows up to 512 wide) is copied to pWork first and tiled back from there; the first 8 bytes
// are already where they belong.
void Pict_TilePlaneInPlace(u8* pPlane, void* pWork, int nWidth, int nHeight) {
    int i;
    int y;
    int nOff;

    for (y = 0; y < nHeight; y += 4) {
        // a band of four rows: nWidth words
        memcpy(pWork, pPlane, nWidth * sizeof(u32));
        for (i = 1; i < nWidth / 2; i++) {
            nOff = (i / 4) * 2 + (nWidth / 4) * (i % 4);
            ((u32*)pPlane)[i * 2] = ((u32*)pWork)[nOff];
            ((u32*)pPlane)[i * 2 + 1] = ((u32*)pWork)[nOff + 1];
        }
        pPlane += nWidth * sizeof(u32);
    }
}

// Copies a plane of nWidth x nHeight bytes into GameCube I8 tile order: tiles of 8 x 4 bytes, each
// row of a tile being 8 bytes of one source row.
void Pict_TilePlane(u8* pSrc, u8* pDst, int nWidth, int nHeight) {
    int i;
    int y;
    int nOff;

    for (y = 0; y < nHeight; y += 4) {
        for (i = 0; i < nWidth / 2; i++) {
            nOff = (i / 4) * 2 + (nWidth / 4) * (i % 4);
            ((u32*)pDst)[i * 2] = ((u32*)pSrc)[nOff];
            ((u32*)pDst)[i * 2 + 1] = ((u32*)pSrc)[nOff + 1];
        }
        pSrc += nWidth * 4;
        pDst += nWidth * 4;
    }
}

// Makes the picture's three planes into I8 textures (U and V at half the width and height).
void Pict_InitTextures(LLPict* pPict) {
    GXInitTexObj(&pPict->aTex[0], Pict_GetPlaneY(pPict), pPict->nWidth, pPict->nHeight, 1, 0, 0, 0);
    GXInitTexObjLOD(&pPict->aTex[0], 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXInitTexObj(&pPict->aTex[1], Pict_GetPlaneU(pPict), pPict->nWidth / 2, pPict->nHeight / 2, 1, 0, 0, 0);
    GXInitTexObjLOD(&pPict->aTex[1], 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
    GXInitTexObj(&pPict->aTex[2], Pict_GetPlaneV(pPict), pPict->nWidth / 2, pPict->nHeight / 2, 1, 0, 0, 0);
    GXInitTexObjLOD(&pPict->aTex[2], 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
}

// Decodes the "MADk" picture file at pData into a new picture ready to draw: PictInt_Decode, then
// each plane tiled in place for the GPU (Pict_TilePlaneInPlace) and its textures made
// (Pict_InitTextures). NULL when pData is NULL or the file does not decode (not MADk, or no memory
// for the picture). uSize is not used. Used for the menus' pictures and the loading screens
// (uiProcessPolygon.c, Code80090940.c); Pict_Free frees the picture.
LLPict* Pict_CreateFromMemory(u8* pData, u32 uSize) {
    LLPict* pPict;

    if (pData == NULL) {
        return NULL;
    }
    pPict = PictInt_Decode((PictFile*)pData);
    if (pPict == NULL) {
        return NULL;
    }
    DCFlushRange(Pict_GetPlaneY(pPict), 1.5f * (pPict->nWidth * pPict->nHeight));
    Pict_TilePlaneInPlace(Pict_GetPlaneY(pPict), *gpPictWorkBuffer, pPict->nWidth, pPict->nHeight);
    Pict_TilePlaneInPlace(Pict_GetPlaneU(pPict), *gpPictWorkBuffer, pPict->nWidth / 2, pPict->nHeight / 2);
    Pict_TilePlaneInPlace(Pict_GetPlaneV(pPict), *gpPictWorkBuffer, pPict->nWidth / 2, pPict->nHeight / 2);
    DCFlushRange(Pict_GetPlaneY(pPict), 1.5f * (pPict->nWidth * pPict->nHeight));
    Pict_InitTextures(pPict);
    return pPict;
}

// Frees a picture Pict_CreateFromMemory made, and its pixels; NULL does nothing.
void Pict_Free(LLPict* pPict) {
    if (pPict != NULL) {
        StaticMem_Free(pPict->pPixels);
        StaticMem_Free(pPict);
    }
}

// Empty in this build. Called right after a picture is freed (uiProcessPolygon.c) and once a frame
// after the menus free their marked pictures (gomainloop.c).
void Pict_AfterFree(void) {
}

// Sets a movie's picture and decoder up (LLVideo.c LLVideo_Create): no pixels or frame yet, a new
// MAD decoder (0x50 bytes), and pfnRead(pArg) as the function the decoder reads the movie's MAD
// files from. The read function is one global of the decoder code (MAD_SetReadCallback), so the
// last movie opened reads for all.
void Pict_OpenMovie(LLPict* pPict, PictStream* pStream, PictFile* (*pfnRead)(void* pArg),
                    void* pArg) {
    pPict->pPixels = NULL;
    pStream->pDecoder = StaticMem_Alloc(80, 1, 32, "LLPict_Gc.c", 278);
    pStream->pFrame = NULL;
    MAD_SetReadCallback(pfnRead, pArg);
    MAD_InitDecoder(pStream->pDecoder);
}

// Undoes Pict_OpenMovie (LLVideo.c LLVideo_Destroy): frees the picture's pixels, gives the held
// frame back, closes the decoder (frees its frames) and frees it. The LLPict itself belongs to the
// Video and is not freed.
void Pict_CloseMovie(LLPict* pPict, PictStream* pStream) {
    if (pPict->pPixels != NULL) {
        StaticMem_Free(pPict->pPixels);
    }
    if (pStream->pFrame != NULL) {
        MAD_ReleaseFrame(pStream->pDecoder, pStream->pFrame);
    }
    MAD_CloseDecoder(pStream->pDecoder);
    StaticMem_Free(pStream->pDecoder);
}

// Empty in this build. LLVideo_Start calls it when a movie starts playing.
void Pict_StartMovie(LLPict* pPict, PictStream* pStream) {
}

// On a movie's first frame (LLVideo_UpdateAll): sizes the picture to the decoder's frame, allocates
// its three planes (1.5 bytes a pixel) and draws the whole texture (f6C, f70 = 1).
void Pict_SizeToMovie(LLPict* pPict, PictStream* pStream) {
    pPict->nWidth = pStream->pFrame->nWidth;
    pPict->nHeight = pStream->pFrame->nHeight;
    pPict->pPixels =
        StaticMem_Alloc(pPict->nWidth * pPict->nHeight * 3 / 2, 1, 32, "LLPict_Gc.c", 346);
    pPict->fMaxU = 1.0f;
    pPict->fMaxV = 1.0f;
}

// The decoder has reached the movie's end (MAD_IsAtEnd). Always 0 in this game: MAD_ReadNextFile
// never sets the end count (its EA bug), so LLVideo.c ends a movie only when it is starved.
u8 Pict_IsMovieAtEnd(LLPict* pPict, PictStream* pStream) {
    return MAD_IsAtEnd(pStream->pDecoder);
}

// Gives back the frame held and takes the decoder's next one (MAD_GetNextFrame, which reads the
// next MAD file through the read function); 1 when there is one, 0 when the read found nothing or
// the frame did not decode.
u8 Pict_NextMovieFrame(LLPict* pPict, PictStream* pStream) {
    if (pStream->pFrame != NULL) {
        MAD_ReleaseFrame(pStream->pDecoder, pStream->pFrame);
    }
    pStream->pFrame = MAD_GetNextFrame(pStream->pDecoder, 0);
    return pStream->pFrame != NULL;
}

// Empty in this build. LLVideo_UpdateAll calls it with n2 = 0 on a movie's first frame, right after
// Pict_SizeToMovie.
void Pict_OnFirstMovieFrame(LLPict* pPict, PictStream* pStream, int n2) {
}

// Shows the decoder's current frame: copies its three planes into the picture in GameCube I8 tile
// order (Pict_TilePlane; U and V at half the width and height) and makes the picture's textures
// (Pict_InitTextures). LLVideo_UpdateAll calls it for each new frame.
void Pict_CopyMovieFrame(LLPict* pPict, PictStream* pStream) {
    DCFlushRange(PictFrame_GetPlaneY(pStream->pFrame), pPict->nWidth * pPict->nHeight * 3 / 2);
    Pict_TilePlane(PictFrame_GetPlaneY(pStream->pFrame), Pict_GetPlaneY(pPict), pPict->nWidth,
                   pPict->nHeight);
    Pict_TilePlane(PictFrame_GetPlaneU(pStream->pFrame), Pict_GetPlaneU(pPict), pPict->nWidth / 2,
                   pPict->nHeight / 2);
    Pict_TilePlane(PictFrame_GetPlaneV(pStream->pFrame), Pict_GetPlaneV(pPict), pPict->nWidth / 2,
                   pPict->nHeight / 2);
    DCFlushRange(Pict_GetPlaneY(pPict), pPict->nWidth * pPict->nHeight * 3 / 2);
    Pict_InitTextures(pPict);
}

// The picture's V (Cr) plane: after Y and U, a quarter of Y's size.
u8* Pict_GetPlaneV(LLPict* pPict) {
    return pPict->pPixels + pPict->nWidth * pPict->nHeight * 5 / 4;
}

// The picture's U (Cb) plane: right after Y, a quarter of its size.
u8* Pict_GetPlaneU(LLPict* pPict) {
    return pPict->pPixels + pPict->nWidth * pPict->nHeight;
}

u8* Pict_GetPlaneY(LLPict* pPict) {
    return pPict->pPixels;
}

// A decoded frame's V (Cr) plane: after Y and U, a quarter of Y's size.
u8* PictFrame_GetPlaneV(PictFrame* pFrame) {
    return pFrame->pPixels + ((u32)(pFrame->nWidth * pFrame->nHeight * 5) >> 2);
}

// A decoded frame's U (Cb) plane: right after Y, a quarter of its size.
u8* PictFrame_GetPlaneU(PictFrame* pFrame) {
    return pFrame->pPixels + pFrame->nWidth * pFrame->nHeight;
}

u8* PictFrame_GetPlaneY(PictFrame* pFrame) {
    return pFrame->pPixels;
}
