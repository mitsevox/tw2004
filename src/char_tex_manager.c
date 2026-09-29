// char_tex_manager.c (TW06's golf/animation/char_tex_manager.c; sGetUserTextureIdx is TW06's
// name): puts the user logos on a golfer's model. A texture named "_usrtextr<n>" in the model is
// replaced by the profile's logo n (0..4) and the logo palette (FE_LogoDesign_GetClut, the same for all).

#include "frontend/fe.h"
#include "lldyntex.h"

// The user logo a texture stands for (the n of "_usrtextr<n>"), or -1.
int sGetUserTextureIdx(u64 uHash) {
    char szName[16];

    SKA_UnpackSwappedName(&uHash, szName);
    if (strncmp(szName, "_usrtextr", strlen("_usrtextr")) != 0) {
        return -1;
    }
    return atoi(szName + strlen("_usrtextr"));
}

// The pixels of the created golfer's logo n (0..4 of pChoices) for a texture named "_usrtextr<n>",
// laid out as a texture by FE_LogoDesign_GetLogoAsTexture (64x64 for a square logo, else 128x32, in
// one shared buffer); NULL for any other texture.
u8* sGetUserLogoTexturePtr(u64 uHash, SkinChoices* pChoices) {
    int nLogo;
    int nWidth;
    int nHeight;

    nLogo = sGetUserTextureIdx(uHash);
    if (nLogo < 0 || nLogo >= 5) return NULL;
    if (pChoices->aLogo[nLogo].nShape == LOGO_SQUARE) {
        nWidth = 64;
        nHeight = 64;
    } else {
        nWidth = 128;
        nHeight = 32;
    }
    return FE_LogoDesign_GetLogoAsTexture(pChoices->aLogo[nLogo].aPixels, nWidth, nHeight);
}

// The palette for a texture named "_usrtextr<n>" (n 0..4): the logo palette every user logo shares
// (FE_LogoDesign_GetClut); NULL for any other texture.
s16* CharacterTex_GetUserLogoPalette(u64 uHash, SkinChoices* pChoices) {
    int nLogo;

    nLogo = sGetUserTextureIdx(uHash);
    if (nLogo < 0 || nLogo >= 5) return NULL;
    return FE_LogoDesign_GetClut();
}

// Puts the created golfer's logos into the model's dynamic textures: every texture named
// "_usrtextr<n>" gets logo n's pixels and the logo palette. Does nothing without pChoices or
// pModel.
void sApplyUserLogos(void* pChar, void* pModel, SkinChoices* pChoices) {
    int nNumTex;
    int i;
    u64 uHash;
    u8* pPixels;
    s16* pPalette;

    if (pChoices != NULL && pModel != NULL) {
        nNumTex = fn_8010AD10(pModel);
        for (i = 0; i < nNumTex; i++) {
            uHash = fn_8010AD18(pModel, i);
            pPixels = sGetUserLogoTexturePtr(uHash, pChoices);
            if (pPixels != NULL) {
                fn_8010B1D4(pModel, i, pPixels, NULL, -1);
                pPalette = CharacterTex_GetUserLogoPalette(uHash, pChoices);
                if (pPalette != NULL) {
                    // port: EA passes two arguments fn_8010B2A8 ignores
                    ((void (*)(DynTex*, int, s16*, void*, s32))fn_8010B2A8)(pModel, i, pPalette, NULL, -1);
                }
            }
        }
    }
}
