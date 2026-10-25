// char_tex_manager.c: puts the user logos on a golfer's model. Each of its dynamic textures named
// "_usrtextr<n>" gets the pixels of user logo n (0..4) of the golfer's look (SkinChoices.aLogo)
// and the logo palette. char.c compiles it as part of itself (#include).

#include "frontend/fe.h"
#include "character.h"
#include "lldyntex.h"

// The user logo a texture stands for: the number after "_usrtextr" in its name, or -1 for a name
// that does not start with "_usrtextr".
static int sGetUserTextureIdx(u64 TexID) {
    char name[16];

    SKA_UnpackSwappedName(&TexID, name);
    if (strncmp(name, "_usrtextr", strlen("_usrtextr")) != 0) {
        return -1;
    }
    return atoi(name + strlen("_usrtextr"));
}

// For a texture named "_usrtextr<n>" (n 0..4), logo n of pCrapInfo laid out as a texture by
// FE_LogoDesign_GetLogoAsTexture (64 x 64 for a square logo, else 128 x 32), in the one buffer every
// call shares; NULL for any other texture.
static u8* sGetUserLogoTexturePtr(u64 TexID, SkinChoices* pCrapInfo) {
    int iUserTextureIdx;
    int nWidth;
    int nHeight;

    iUserTextureIdx = sGetUserTextureIdx(TexID);
    if (iUserTextureIdx < 0 || iUserTextureIdx >= SAVE_NUM_LOGOS) return NULL;
    if (pCrapInfo->aLogo[iUserTextureIdx].nShape == LOGO_SQUARE) {
        nWidth = 64;
        nHeight = 64;
    } else {
        nWidth = 128;
        nHeight = 32;
    }
    return FE_LogoDesign_GetLogoAsTexture(pCrapInfo->aLogo[iUserTextureIdx].aPixels, nWidth, nHeight);
}

// For a texture named "_usrtextr<n>" (n 0..4), the logo palette (FE_LogoDesign_GetClut), the same for
// every user logo; NULL for any other texture. pCrapInfo is not read.
static s16* sGetUserLogoPalettePtr(u64 TexID, SkinChoices* pCrapInfo) {
    int iUserTextureIdx;

    iUserTextureIdx = sGetUserTextureIdx(TexID);
    if (iUserTextureIdx < 0 || iUserTextureIdx >= SAVE_NUM_LOGOS) return NULL;
    return FE_LogoDesign_GetClut();
}

// Puts the logos of pCrapInfo into pDynTex, the character's dynamic textures: each texture named
// "_usrtextr<n>" gets logo n's pixels, then the logo palette. Nothing happens when either is NULL;
// pCharacter is not read.
static void sApplyUserLogos(Character* pCharacter, DynTex* pDynTex, SkinChoices* pCrapInfo) {
    int nTextures;
    int iTexture;
    u64 TexID;
    u8* pTexturePtr;
    s16* pPalettePtr;

    if (pCrapInfo != NULL && pDynTex != NULL) {
        nTextures = DynTex_GetNumTextures(pDynTex);
        for (iTexture = 0; iTexture < nTextures; iTexture++) {
            TexID = DynTex_GetTexIDByIndex(pDynTex, iTexture);
            pTexturePtr = sGetUserLogoTexturePtr(TexID, pCrapInfo);
            if (pTexturePtr != NULL) {
                DynTex_UpdateTexturePixels(pDynTex, iTexture, pTexturePtr, NULL, -1);
                pPalettePtr = sGetUserLogoPalettePtr(TexID, pCrapInfo);
                if (pPalettePtr != NULL) {
                    DynTex_UpdateTexturePalette(pDynTex, iTexture, pPalettePtr, NULL, -1);
                }
            }
        }
    }
}
