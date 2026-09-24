// char_tex_manager.c (TW06's golf/animation/char_tex_manager.c; sGetUserTextureIdx is TW06's
// name): puts the user logos on a golfer's model. A texture named "_usrtextr<n>" in the model is
// replaced by the profile's logo n (0..4) and the logo palette (fn_8010FBC4, the same for all).

#include "frontend/fe.h"
#include "lldyntex.h"

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
u8* fn_80017398(u64 uHash, SkinChoices* pChoices) {
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
    return fn_8010FF5C(pChoices->aLogo[nLogo].aPixels, nWidth, nHeight);
}

// The palette for a user logo texture, or NULL.
s16* fn_80017410(u64 uHash, SkinChoices* pChoices) {
    int nLogo;

    nLogo = sGetUserTextureIdx(uHash);
    if (nLogo < 0 || nLogo >= 5) return NULL;
    return fn_8010FBC4();
}

// Puts the profile's logos into every user logo texture of the model.
void fn_8001744C(void* pChar, void* pModel, SkinChoices* pChoices) {
    int nNumTex;
    int i;
    u64 uHash;
    u8* pPixels;
    s16* pPalette;

    if (pChoices != NULL && pModel != NULL) {
        nNumTex = fn_8010AD10(pModel);
        for (i = 0; i < nNumTex; i++) {
            uHash = fn_8010AD18(pModel, i);
            pPixels = fn_80017398(uHash, pChoices);
            if (pPixels != NULL) {
                fn_8010B1D4(pModel, i, pPixels, NULL, -1);
                pPalette = fn_80017410(uHash, pChoices);
                if (pPalette != NULL) {
                    // port: EA passes two arguments fn_8010B2A8 ignores
                    ((void (*)(DynTex*, int, s16*, void*, s32))fn_8010B2A8)(pModel, i, pPalette, NULL, -1);
                }
            }
        }
    }
}
