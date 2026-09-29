// FE_LogoDesign.c (EA's name, from its allocations; TW07 keeps the file and these functions in the
// same order): the Create-A-Player logo editor, where the player draws the five user logos of a
// save profile. A logo is a 64 x 64 or 128 x 32 grid of colour indexes into a 256-colour palette
// (the CLUT, copied from the texture "__LogoSquare"); the menus set and read its pixels through
// FE_CrAPMessages.c, and FE_LogoDesign_UploadCustomLogo copies it into the texture "__LogoSquare"
// or "__LogoRect" to be drawn. Last come the copy between a logo's rows and the texture layout and
// the logo laid out as a texture for the golfer's clothes (char_tex_manager.c); these were the
// units LogoTexture.c and unsorted/sweep_8010FF5C.c until 2026-09-29 (their code follows on with
// no gap, and TW07's FE_LogoDesign.c also ends with pixel helpers).

#include "engine.h"
#include "gx.h"
#include "core/startup.h"
#include "frontend/fe.h"

void FE_LogoDesign_OpenOnce(void);
void FE_LogoDesign_CloseOnce(void);
void FE_LogoDesign_InitModule(void);
void FE_LogoDesign_CloseModule(void);
int  FE_LogoDesign_GetPixelIndex(int nX, int nY);
void FE_LogoDesign_UploadCustomLogo(void);
void FE_LogoDesign_LoadClut(void);

// Last address first: CodeWarrior lays out uninitialised globals in reverse order of definition.
u8 gbLogoClutLoaded;            // the palette has been copied from "__LogoSquare"
s16* gpLogoClut;                // the palette: 256 entries, 5-5-5 RGB with the top bit as alpha
LogoEdit* gpLogoEdit;           // the editor's state while the front end runs

// Allocates the logo editor's palette (256 colours, cleared) once at start-up (gomainloop's set-up
// beside GOLFERSTATE_OpenONCE); FE_LogoDesign_LoadClut fills it later.
void FE_LogoDesign_OpenOnce(void) {
    gpLogoClut = StaticMem_Alloc(256 * sizeof(s16), 0, 0, "FE_LogoDesign.c", 47);
    Mem_set(gpLogoClut, 0, 256 * sizeof(s16));
    gbLogoClutLoaded = 0;
}

// Frees the palette FE_LogoDesign_OpenOnce allocated and marks it not loaded.
void FE_LogoDesign_CloseOnce(void) {
    StaticMem_Free(gpLogoClut);
    gpLogoClut = NULL;
    gbLogoClutLoaded = 0;
}

// Starts the logo editor when the front end starts (GO_vInitFE): its state (LogoEdit) allocated and
// cleared (logo 0, square, unchanged), and the palette copied from "__LogoSquare" if that has not
// been done yet (FE_LogoDesign_LoadClut).
void FE_LogoDesign_InitModule(void) {
    gpLogoEdit = StaticMem_Alloc(sizeof(LogoEdit), 2, 0, "FE_LogoDesign.c", 67);
    Mem_set(gpLogoEdit, 0, sizeof(LogoEdit));
    FE_LogoDesign_LoadClut();
}

// Frees the logo editor's state (FE_LogoDesign_InitModule); the palette stays.
void FE_LogoDesign_CloseModule(void) {
    StaticMem_Free(gpLogoEdit);
    gpLogoEdit = NULL;
}

// The logo editor works on the profile's user logo n (0..4) from now on; it is marked changed so
// its texture is redrawn (GM_vSelectLogo).
void FE_LogoDesign_SetCurrentLogoNumber(s32 n) {
    gpLogoEdit->nLogo = n;
    gpLogoEdit->bDirty = 1;
}

s32 FE_LogoDesign_GetCurrentLogoNumber(void) {
    return gpLogoEdit->nLogo;
}

// Sets the logo editor's shape (LOGO_SQUARE: 64 x 64, LOGO_RECT: 128 x 32) and marks the logo
// changed. Only the editor's; the logo record keeps its own nShape.
void FE_LogoDesign_SetCurrentLogoMode(s32 nShape) {
    gpLogoEdit->nShape = nShape;
    gpLogoEdit->bDirty = 1;
}

// Palette colour nColor as 0-255 components: the entry is 5-5-5 RGB (red in bits 10-14, green 5-9,
// blue 0-4, each scaled by 8) with the top bit as alpha, given as 255 when set and 0 when clear.
void FE_LogoDesign_GetClutEntry(int nColor, u32* pR, u32* pG, u32* pB, u32* pA) {
    s16* pPalette = FE_LogoDesign_GetClut();
    *pR = (pPalette[nColor] >> 7) & 0xF8;
    *pB = (pPalette[nColor] << 3) & 0xF8;
    *pG = (pPalette[nColor] >> 2) & 0xF8;
    *pA = (pPalette[nColor] >> 15) & 1;
    if (*pA) {
        *pA = 0xFF;
    }
}

// Marks the logo being edited changed, so FE_LogoDesign_UploadCustomLogo copies it into its texture
// on the next frame.
void FE_LogoDesign_RefreshLogo(void) {
    gpLogoEdit->bDirty = 1;
}

// Fills the logo being edited with the pixels of the texture named pName (fn_8000BD80), taken out
// of the texture layout at the editor's shape's size (FE_LogoDesign_CopyLogoTexturePixels). Nothing
// when there is no such texture. It does not mark the logo changed (the menus do,
// GM_vMarkLogoChanged).
void FE_LogoDesign_SetLogoToPremadeTexture(char* pName) {
    u8* pLogo = FE_LogoDesign_GetCurrentLogo()->aPixels;
    u8* pPixels;
    int nWidth;
    int nHeight;
    if (fn_8000BD80(pName, &pPixels)) {
        if (gpLogoEdit->nShape == LOGO_SQUARE) {
            nWidth = 64;
            nHeight = 64;
        } else {
            nWidth = 128;
            nHeight = 32;
        }
        FE_LogoDesign_CopyLogoTexturePixels(pLogo, pPixels, 0, nWidth, nHeight);
    }
}

// Sets pixel (nX, nY) of the logo being edited to palette colour nColor and marks the logo changed.
// EA bug: a pixel off the logo (FE_LogoDesign_GetPixelIndex returns -1) writes the byte before it.
void FE_LogoDesign_SetPixel(int nX, int nY, int nColor) {
    u8* pLogo;
    int n;
    pLogo = FE_LogoDesign_GetCurrentLogo()->aPixels;
    n = FE_LogoDesign_GetPixelIndex(nX, nY);
    pLogo[n] = nColor;
    gpLogoEdit->bDirty = 1;
}

// Pixel (nX, nY)'s index in the logo's rows (LogoRecord.aPixels): nX + nY * 64 for a square logo,
// nX + nY * 128 for a rectangular one; -1 when it is off the logo or the editor's shape is neither.
int FE_LogoDesign_GetPixelIndex(int nX, int nY) {
    s32 nShape = gpLogoEdit->nShape;
    if (nShape == LOGO_SQUARE) {
        if (nX < 0 || nX >= 64 || nY < 0 || nY >= 64) {
            return -1;
        }
        return nX + (nY << 6);
    }
    if (nShape == LOGO_RECT) {
        if (nX < 0 || nX >= 128 || nY < 0 || nY >= 32) {
            return -1;
        }
        return nX + (nY << 7);
    }
    return -1;
}

// Called once a frame (gomainloop): when the logo being edited has changed, it is copied into the
// first level of the texture "__LogoSquare" or "__LogoRect" (by the editor's shape; found by its
// hash, else by name) in the texture layout (FE_LogoDesign_CopyLogoTexturePixels), and that texture
// is set for drawing (RenderState_SetBankTexture). Nothing when the texture is missing.
void FE_LogoDesign_UploadCustomLogo(void) {
    u8* pLogo = FE_LogoDesign_GetCurrentLogo()->aPixels;
    char* pName;
    TexBank* pBank;
    TexEntry* pTex;
    u8* pPixels;
    int nWidth;
    int nHeight;
    if (gpLogoEdit->bDirty) {
        gpLogoEdit->bDirty = 0;
        if (gpLogoEdit->nShape == LOGO_SQUARE) {
            pName = "__LogoSquare";
        } else {
            pName = "__LogoRect";
        }
        fn_800102DC(fn_8000BEE4(pName), &pBank, &pTex);
        if (pTex == NULL) {
            fn_8000BDF8(pName, &pBank, &pTex);
        }
        if (pTex != NULL) {
            pPixels = pBank->p18 + pTex->aMips[0].uPixels;
            if (gpLogoEdit->nShape == LOGO_SQUARE) {
                nWidth = 64;
                nHeight = 64;
            } else {
                nWidth = 128;
                nHeight = 32;
            }
            FE_LogoDesign_CopyLogoTexturePixels(pPixels, pLogo, 1, nWidth, nHeight);
            RenderState_SetBankTexture(pBank, pTex);
        }
    }
}

// Copies the palette of the texture "__LogoSquare" (its bank's n24 bytes) into the logo editor's
// palette, once: nothing when it is already loaded, and nothing (to be tried again at the next
// FE_LogoDesign_InitModule) when the texture is not there.
void FE_LogoDesign_LoadClut(void) {
    TexBank* pBank;
    TexEntry* pTex;
    s16* pPalette;
    if (gbLogoClutLoaded == 0) {
        fn_8000BDF8("__LogoSquare", &pBank, &pTex);
        if (pTex != NULL) {
            pPalette = FE_LogoDesign_GetClut();
            Mem_cpy(pPalette, pBank->p20 + pBank->pC[pTex->nPalette].uColors, pBank->n24);
            gbLogoClutLoaded = 1;
        }
    }
}

// The logo being edited: the menus' own copy while bEditingCopy is set, else the profile's user
// logo that LogoEdit.nLogo names.
LogoRecord* FE_LogoDesign_GetCurrentLogo(void) {
    if (gpFEProfile->bEditingCopy) {
        return &gpFEProfile->logoCopy;
    }
    return &FE_GetCurrentProfile()->choices.aLogo[gpLogoEdit->nLogo];
}

// The logo palette (CLUT): 256 colours of 16 bits (see FE_LogoDesign_GetClutEntry).
// char_tex_manager.c gives it to the golfer's five user logo textures
// (CharacterTex_GetUserLogoPalette).
s16* FE_LogoDesign_GetClut(void) {
    return gpLogoClut;
}

// Pixel (nX, nY) of the logo being edited: returns its palette colour index and gives that colour's
// components as FE_LogoDesign_GetClutEntry does. A pixel off the logo reads the byte before it
// (FE_LogoDesign_GetPixelIndex returns -1), as FE_LogoDesign_SetPixel writes it.
int FE_LogoDesign_GetPixelColor(int nX, int nY, u32* pR, u32* pG, u32* pB, u32* pA) {
    u8* pLogo = FE_LogoDesign_GetCurrentLogo()->aPixels;
    int nColor = pLogo[FE_LogoDesign_GetPixelIndex(nX, nY)];
    FE_LogoDesign_GetClutEntry(nColor, pR, pG, pB, pA);
    return nColor;
}

// Copy a logo between its plain layout (rows of nWidth colour indexes) and the texture layout
// (tiles of 8 x 4 pixels, 32 bytes each, a row of tiles after another): bToTexture 1 from the
// logo in pSrc to the texture in pDst, 0 the other way. Then flush pDst for the GPU.
// One loop with the direction test inside: the compiler unswitches it into the two unrolled copies.
void FE_LogoDesign_CopyLogoTexturePixels(u8* pDst, u8* pSrc, int bToTexture, int nWidth, int nHeight) {
    int nTileRow;
    int i;
    int nRow;
    int y;
    int x;
    int nPos;

    nTileRow = (nWidth == 64) ? 64 * 4 : 128 * 4;
    for (y = 0, nRow = 0; y < nHeight; y++, nRow += nWidth) {
        i = nRow;
        for (x = 0; x < nWidth; x++) {
            nPos = (x / 8) * 32 + ((y % 4) * 8 + (y / 4) * nTileRow) + x % 8;
            if (bToTexture) {
                pDst[nPos] = pSrc[i];
            } else {
                pDst[i] = pSrc[nPos];
            }
            i++;
        }
    }
    DCFlushRange(pDst, 64 * 64);
    GXInvalidateTexAll();
}

u8 gLogoTexturePixels[64 * 64];

// Logo pLogo (nWidth x nHeight colour indexes) laid out as a texture in gLogoTexturePixels
// (FE_LogoDesign_CopyLogoTexturePixels), which it returns: one buffer shared by every call, so each
// call overwrites the last (char_tex_manager.c, for a created golfer's user logo).
u8* FE_LogoDesign_GetLogoAsTexture(u8* pLogo, int nWidth, int nHeight) {
    FE_LogoDesign_CopyLogoTexturePixels(gLogoTexturePixels, pLogo, 1, nWidth, nHeight);
    return gLogoTexturePixels;
}
