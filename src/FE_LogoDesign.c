// FE_LogoDesign.c (EA's name, from its asserts): the logo editor. A logo is a 64 x 64 or 128 x 32
// grid of colour indexes into a 256-colour palette; the palette comes from the texture
// "__LogoSquare", and the logo is copied into the texture "__LogoSquare" or "__LogoRect" to be drawn.

#include "engine.h"
#include "frontend/fe.h"


void fn_8010F6C8(void);
void fn_8010F718(void);
void fn_8010F748(void);
void fn_8010F794(void);
int  fn_8010F978(int nX, int nY);
void fn_8010FA00(void);
void fn_8010FAF4(void);

// Last address first: CodeWarrior lays out uninitialised globals in reverse order of definition.
u8 lbl_802824C0;
s16* lbl_802824BC;
LogoEdit* lbl_802824B8;

// Allocate the palette, empty. (The allocator records EA's file name and line.)
void fn_8010F6C8(void) {
    lbl_802824BC = fn_80009B34(256 * sizeof(s16), 0, 0, "FE_LogoDesign.c", 47);
    fn_80005AE8(lbl_802824BC, 0, 256 * sizeof(s16));
    lbl_802824C0 = 0;
}

void fn_8010F718(void) {
    fn_80009E70(lbl_802824BC);
    lbl_802824BC = NULL;
    lbl_802824C0 = 0;
}

// Start editing: a cleared logo, and the palette if it is not loaded yet.
void fn_8010F748(void) {
    lbl_802824B8 = fn_80009B34(sizeof(LogoEdit), 2, 0, "FE_LogoDesign.c", 67);
    fn_80005AE8(lbl_802824B8, 0, sizeof(LogoEdit));
    fn_8010FAF4();
}

void fn_8010F794(void) {
    fn_80009E70(lbl_802824B8);
    lbl_802824B8 = NULL;
}

void fn_8010F7C0(s32 n) {
    lbl_802824B8->n0 = n;
    lbl_802824B8->bDirty = 1;
}

s32 fn_8010F7D8(void) {
    return lbl_802824B8->n0;
}

void fn_8010F7E4(s32 nShape) {
    lbl_802824B8->nShape = nShape;
    lbl_802824B8->bDirty = 1;
}

// A palette colour as 0-255 components; alpha is 0 or 255.
void fn_8010F7FC(int nColor, u32* pR, u32* pG, u32* pB, u32* pA) {
    s16* pPalette = fn_8010FBC4();
    *pR = (pPalette[nColor] >> 7) & 0xF8;
    *pB = (pPalette[nColor] << 3) & 0xF8;
    *pG = (pPalette[nColor] >> 2) & 0xF8;
    *pA = (pPalette[nColor] >> 15) & 1;
    if (*pA) {
        *pA = 0xFF;
    }
}

void fn_8010F880(void) {
    lbl_802824B8->bDirty = 1;
}

// Load the logo from a texture.
void fn_8010F890(char* pName) {
    u8* pLogo = fn_8010FB70()->aPixels;
    u8* pPixels;
    int nWidth;
    int nHeight;
    if (fn_8000BD80(pName, &pPixels)) {
        if (lbl_802824B8->nShape == LOGO_SQUARE) {
            nWidth = 64;
            nHeight = 64;
        } else {
            nWidth = 128;
            nHeight = 32;
        }
        fn_8010FC3C(pLogo, pPixels, 0, nWidth, nHeight);
    }
}

// EA bug: a pixel off the logo (fn_8010F978 returns -1) writes the byte before it.
void fn_8010F90C(int nX, int nY, int nColor) {
    u8* pLogo;
    int n;
    pLogo = fn_8010FB70()->aPixels;
    n = fn_8010F978(nX, nY);
    pLogo[n] = nColor;
    lbl_802824B8->bDirty = 1;
}

// A pixel's index in the logo, or -1 if it is off the logo.
int fn_8010F978(int nX, int nY) {
    s32 nShape = lbl_802824B8->nShape;
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

// Once a frame: if the logo changed, copy it into its texture and draw with that.
void fn_8010FA00(void) {
    u8* pLogo = fn_8010FB70()->aPixels;
    char* pName;
    TexBank* pBank;
    TexEntry* pTex;
    u8* pPixels;
    int nWidth;
    int nHeight;
    if (lbl_802824B8->bDirty) {
        lbl_802824B8->bDirty = 0;
        if (lbl_802824B8->nShape == LOGO_SQUARE) {
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
            if (lbl_802824B8->nShape == LOGO_SQUARE) {
                nWidth = 64;
                nHeight = 64;
            } else {
                nWidth = 128;
                nHeight = 32;
            }
            fn_8010FC3C(pPixels, pLogo, 1, nWidth, nHeight);
            fn_8005CC64(pBank, pTex);
        }
    }
}

// Copy the palette of "__LogoSquare", once.
void fn_8010FAF4(void) {
    TexBank* pBank;
    TexEntry* pTex;
    s16* pPalette;
    if (lbl_802824C0 == 0) {
        fn_8000BDF8("__LogoSquare", &pBank, &pTex);
        if (pTex != NULL) {
            pPalette = fn_8010FBC4();
            Mem_cpy(pPalette, pBank->p20 + pBank->pC[pTex->nPalette].uColors, pBank->u24);
            lbl_802824C0 = 1;
        }
    }
}

// The logo being edited: the menus' own copy while b10640 is set, else the profile's user logo
// that LogoEdit.n0 names.
LogoRecord* fn_8010FB70(void) {
    if (lbl_80281ED4->b10640) {
        return &lbl_80281ED4->logo106E0;
    }
    return &fn_80077ACC()->choices.aLogo[lbl_802824B8->n0];
}

// The palette.
s16* fn_8010FBC4(void) {
    return lbl_802824BC;
}

// A pixel's colour index, and its colour as fn_8010F7FC gives it.
int fn_8010FBCC(int nX, int nY, u32* pR, u32* pG, u32* pB, u32* pA) {
    u8* pLogo = fn_8010FB70()->aPixels;
    int nColor = pLogo[fn_8010F978(nX, nY)];
    fn_8010F7FC(nColor, pR, pG, pB, pA);
    return nColor;
}
