// char_tex_manager.c (TW06's golf/animation/char_tex_manager.c; sGetUserTextureIdx is TW06's
// name): puts the user logos on a golfer's model. A texture named "_usrtextr<n>" in the model is
// replaced by the profile's logo n (0..4) and its palette.

#include "frontend/fe.h"

int  fn_8010AD10(void* pModel);                     // how many textures the model has
u64  fn_8010AD18(void* pModel, int nTex);           // a texture's name hash
void fn_8010B1D4(void* pModel, int nTex, u8* pPixels, int a, int b);
void fn_8010B2A8(void* pModel, int nTex, s16* pPalette, int a, int b);

// The user logo a texture stands for (the n of "_usrtextr<n>"), or -1.
int sGetUserTextureIdx(u64 uHash) {
    char szName[16];

    fn_800CB8F0(&uHash, szName);
    if (strncmp(szName, "_usrtextr", strlen("_usrtextr")) != 0) {
        return -1;
    }
    return atoi(szName + strlen("_usrtextr"));
}

// The pixels of the user logo a texture stands for, or NULL.
u8* fn_80017398(u64 uHash, ProfileLogos* pLogos) {
    int nLogo;
    int nWidth;
    int nHeight;

    nLogo = sGetUserTextureIdx(uHash);
    if (nLogo < 0 || nLogo >= 5) return NULL;
    if (pLogos->aLogo[nLogo].nShape == LOGO_SQUARE) {
        nWidth = 64;
        nHeight = 64;
    } else {
        nWidth = 128;
        nHeight = 32;
    }
    return fn_8010FF5C(pLogos->aLogo[nLogo].aPixels, nWidth, nHeight);
}

// The palette for a user logo texture, or NULL.
s16* fn_80017410(u64 uHash, ProfileLogos* pLogos) {
    int nLogo;

    nLogo = sGetUserTextureIdx(uHash);
    if (nLogo < 0 || nLogo >= 5) return NULL;
    return fn_8010FBC4();
}

// Puts the profile's logos into every user logo texture of the model.
void fn_8001744C(void* pChar, void* pModel, ProfileLogos* pLogos) {
    int nNumTex;
    int i;
    u64 uHash;
    u8* pPixels;
    s16* pPalette;

    if (pLogos != NULL && pModel != NULL) {
        nNumTex = fn_8010AD10(pModel);
        for (i = 0; i < nNumTex; i++) {
            uHash = fn_8010AD18(pModel, i);
            pPixels = fn_80017398(uHash, pLogos);
            if (pPixels != NULL) {
                fn_8010B1D4(pModel, i, pPixels, 0, -1);
                pPalette = fn_80017410(uHash, pLogos);
                if (pPalette != NULL) {
                    fn_8010B2A8(pModel, i, pPalette, 0, -1);
                }
            }
        }
    }
}
