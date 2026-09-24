// GoShaderObjectCommon_ShaderObjectsData_Gc.c (EA's name, from its asserts): builds the display
// lists that draw the shader objects through GX texture and colour stages.

#include "engine.h"
#include "gx.h"
#include "core/startup.h"
#include "shaderdata.h"

// The morph the list being built records its vertices for (a command 6 sets it), and which
// vertex arrays the list has: texture coordinates, normals, colours.
MorphAnim* lbl_80281EA0;
// fn_800738DC's state: the texture map a palettized texture loads into (always 0), the texture
// last set up, and which stage setup is loaded (EB2 two-texture or palettized, EB1 one texture,
// EB0 with it the one for formats 0 and 1).
int lbl_80281EA4;
TexEntry* lbl_80281EA8;
TexBank* lbl_80281EAC;
u8 lbl_80281EB0;
u8 lbl_80281EB1;
u8 lbl_80281EB2;
u8 lbl_80281EB3;
u8 lbl_80281EB4;
u8 lbl_80281EB5;

// An int's absolute value as EA wrote it here: (sign + v) ^ sign (srawi, add, xor). The
// compiler's own abs() and IABS give xor, subf; the sign in a local puts it first in the add.
static inline int AbsAddXor(int v) {
    int nSign = v >> 31;

    return (nSign + v) ^ nSign;
}

int fn_80073878(TexBank* pBank, TexEntry* pTex);
void fn_800738DC(TexBank* pBank, TexEntry* pTex, u8 bFirst);
void fn_800740F4(TexBank* pBank, TexEntry* pTex, TexBank* pBank2, TexEntry* pTex2);
u16* fn_80074628(u16* p, u16* pEnd);
u16* fn_80074A24(u16* p, u16* pEnd, u8 bSecond, u8* pbFirst);
u16* fn_80074BE0(u16* p, u16* pEnd);
u16* fn_80074CEC(u16* p, u16* pEnd);
u16* fn_80074CF4(u16* p, u16* pEnd);
void fn_80075250(u16 nIndex);
void fn_8007525C(u16 nIndex);
void fn_80075268(u16 nIndex);
void fn_80075274(u16 nIndex);
u16 fn_80076D68(f32 fU, f32 fV, f32 fScale, f32 fInv);    // GoShaderObjectCommon_TexAnimManager_Gc.c
void fn_80076BF0(void);                                 // GoShaderObjectCommon_TexAnimManager_Gc.c
void fn_80012520(u32 ePrim, u32 eFormat, u16 nVerts);   // LLFont.c: GXBegin
void fn_800124A8(void);                                 // LLFont.c: end the primitive

// Another texture in pTex's bank with the same u0 as pTex: its index, or -1.
int fn_80073878(TexBank* pBank, TexEntry* pTex) {
    int i;
    for (i = 0; i < pBank->n2; i++) {
        if (pBank->p8[i].u0 == pTex->u0 && pTex != &pBank->p8[i]) return i;
    }
    return -1;
}

// Set up the texture stages for one texture (NULL pBank: none), skipped when it is the texture
// already set up. bFirst forgets what was set up. A texture with a pair (b47 bit 0) loads both,
// in the order its b40 says; b40 9 is palettized (its palette loaded, alpha-tested and blended),
// 0 and 1 and the others each have their own stage setup. A texture outside its bank decodes its
// name (for a message the retail build left out) and draws untextured.
void fn_800738DC(TexBank* pBank, TexEntry* pTex, u8 bFirst) {
    char szName[20];            // the size is unknown: the frame leaves 20 bytes for it
    TexGXObj* pObj;
    int nPair;

    if (bFirst) {
        lbl_80281EB2 = lbl_80281EB1 = lbl_80281EB0 = 0;
        // port: an address no bank or texture has, so the test below fails; only compared
        lbl_80281EAC = (TexBank*)&bFirst;
        lbl_80281EA8 = (TexEntry*)&bFirst;
        lbl_80281EA4 = 0;
    }
    if (pBank != lbl_80281EAC || pTex != lbl_80281EA8) {
        lbl_80281EAC = pBank;
        lbl_80281EA8 = pTex;
        if (pBank != NULL) {
            if (pTex->n3E >= pBank->n2) {
                fn_800CB8F0(&pTex->u0, szName);
            } else {
                pObj = &pBank->p10[pTex->n3E];
                if (pTex->b47 & 1) {
                    nPair = fn_80073878(pBank, pTex);
                    if (nPair != -1) {
                        if (pTex->b40 == 0) {
                            GXLoadTexObj((GXTexObj*)pObj, 1);
                            GXLoadTexObj((GXTexObj*)&pBank->p10[nPair], 0);
                        } else {
                            GXLoadTexObj((GXTexObj*)pObj, 0);
                            GXLoadTexObj((GXTexObj*)&pBank->p10[nPair], 1);
                        }
                        if (!lbl_80281EB2) {
                            lbl_80281EB2 = 1;
                            lbl_80281EB1 = 0;
                            lbl_80281EB0 = 0;
                            GXSetNumTexGens(1);
                            GXSetNumTevStages(2);
                            GXSetTevOrder(0, 0, 0, 4);
                            GXSetTevOrder(1, 0, 1, 4);
                            if (*lbl_80280DC8) {
                                GXSetTevColorIn(0, 0xF, 8, 0xA, 0xF);
                                GXSetTevColorOp(0, 0, 0, 1, 1, 0);
                                GXSetTevAlphaIn(0, 7, 6, 6, 7);
                                GXSetTevAlphaOp(0, 0, 0, 1, 1, 0);
                                GXSetTevColorIn(1, 0xF, 0, 0xC, 0xF);
                                GXSetTevColorOp(1, 0, 0, 0, 1, 0);
                                GXSetTevAlphaIn(1, 4, 7, 6, 7);
                                GXSetTevAlphaOp(1, 0xE, 0, 1, 1, 0);
                            } else {
                                GXSetTevColorIn(0, 0xF, 8, 0xA, 0xF);
                                GXSetTevColorOp(0, 0, 0, 1, 1, 0);
                                GXSetTevAlphaIn(0, 7, 6, 6, 7);
                                GXSetTevAlphaOp(0, 0, 0, 1, 1, 0);
                                GXSetTevColorIn(1, 0xF, 0, 0xC, 0xF);
                                GXSetTevColorOp(1, 0, 0, 0, 1, 0);
                                GXSetTevAlphaIn(1, 7, 4, 5, 7);
                                GXSetTevAlphaOp(1, 0, 0, 1, 1, 0);
                            }
                        }
                    }
                } else {
                    if (pTex->b40 == 0 || pTex->b40 == 1) {
                        GXLoadTexObj((GXTexObj*)pObj, 0);
                        if (!lbl_80281EB1 || !lbl_80281EB0) {
                            lbl_80281EB2 = 0;
                            lbl_80281EB1 = 1;
                            lbl_80281EB0 = 1;
                            GXSetNumTexGens(1);
                            GXSetTevOrder(0, 0, 0, 4);
                            GXSetNumTevStages(1);
                            if (*lbl_80280DC8) {
                                GXSetTevColorIn(0, 0xF, 0xA, 0xC, 0xF);
                                GXSetTevColorOp(0, 0, 0, 1, 1, 0);
                                GXSetTevAlphaIn(0, 7, 4, 6, 7);
                                GXSetTevAlphaOp(0, 0, 0, 1, 1, 0);
                            } else {
                                GXSetTevColorIn(0, 0xF, 0xA, 0xC, 0xF);
                                GXSetTevColorOp(0, 0, 0, 1, 1, 0);
                                GXSetTevAlphaIn(0, 7, 4, 5, 7);
                                GXSetTevAlphaOp(0, 0, 0, 1, 1, 0);
                            }
                        }
                    } else if (pTex->b40 == 9) {
                        if (pTex->nPalette != -1) {
                            GXLoadTlut((GXTlutObj*)&pBank->p14[pTex->n3E], 0);
                        }
                        GXLoadTexObj((GXTexObj*)pObj, lbl_80281EA4);
                        if (!lbl_80281EB2) {
                            lbl_80281EB2 = 1;
                            lbl_80281EB1 = 0;
                            lbl_80281EB0 = 0;
                            GXSetNumTexGens(1);
                            GXSetNumTevStages(1);
                            GXSetTevOrder(0, 0, 0, 4);
                            GXSetTevColorIn(1, 0xF, 0xF, 0xF, 8);
                            GXSetTevColorOp(1, 0, 0, 0, 1, 0);
                            GXSetTevAlphaIn(1, 7, 7, 7, 4);
                            GXSetTevAlphaOp(1, 0, 0, 1, 1, 0);
                            GXSetBlendMode(1, 4, 5, 0);
                            GXSetAlphaCompare(7, 0, 0, 7, 0x7F);
                            GXSetZCompLoc(1);
                        }
                    } else {
                        GXLoadTexObj((GXTexObj*)pObj, 0);
                        if (!lbl_80281EB1 || lbl_80281EB0) {
                            lbl_80281EB2 = 0;
                            lbl_80281EB1 = 1;
                            lbl_80281EB0 = 0;
                            GXSetNumTexGens(1);
                            GXSetTevOrder(0, 0, 0, 4);
                            GXSetNumTevStages(1);
                            if (*lbl_80280DC8) {
                                GXSetTevColorIn(0, 0xF, 8, 0xA, 0xF);
                                GXSetTevColorOp(0, 0, 0, 1, 1, 0);
                                GXSetTevAlphaIn(0, 7, 4, 6, 7);
                                GXSetTevAlphaOp(0, 0, 0, 1, 1, 0);
                            } else {
                                GXSetTevColorIn(0, 0xF, 8, 0xA, 0xF);
                                GXSetTevColorOp(0, 0, 0, 1, 1, 0);
                                GXSetTevAlphaIn(0, 7, 4, 5, 7);
                                GXSetTevAlphaOp(0, 0, 0, 1, 1, 0);
                            }
                        }
                    }
                }
                return;
            }
        }
            GXSetNumTexGens(0);
            GXSetNumTevStages(1);
            GXSetTevOrder(0, 0xFF, 0xFF, 4);
            if (*lbl_80280DC8) {
                GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xA);
                GXSetTevColorOp(0, 0, 0, 0, 1, 0);
                GXSetTevAlphaIn(0, 7, 7, 7, 6);
                GXSetTevAlphaOp(0, 0, 0, 1, 1, 0);
            } else {
                GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xA);
                GXSetTevColorOp(0, 0, 0, 0, 1, 0);
                GXSetTevAlphaIn(0, 7, 7, 7, 5);
                GXSetTevAlphaOp(0, 0, 0, 1, 1, 0);
            }
            lbl_80281EB2 = 0;
            lbl_80281EB1 = 0;
            lbl_80281EB0 = 0;
    }
}

// Set up the texture stages for two textures: three stages, or four when pTex2 has a second GX
// object (b47 bit 0), loaded as texture map 2. With no textures, or a first texture that has a
// second object, one untextured stage draws the vertex colour, its alpha from the vertex colour
// or, while *lbl_80280DC8 is set, from the constant colour.
void fn_800740F4(TexBank* pBank, TexEntry* pTex, TexBank* pBank2, TexEntry* pTex2) {
    TexGXObj* pObj;
    TexGXObj* pObj2;

    if (pBank != NULL && pBank2 != NULL && !(pTex->b47 & 1)) {
        pObj = &pBank->p10[pTex->n3E];
        pObj2 = &pBank2->p10[pTex2->n3E];
        if (!(pTex2->b47 & 1)) {
            GXLoadTexObj((GXTexObj*)pObj, 0);
            GXLoadTexObj((GXTexObj*)pObj2, 1);
            GXSetNumTexGens(2);
            GXSetNumTevStages(3);
            GXSetTevOrder(0, 0, 0, 4);
            GXSetTevOrder(1, 1, 1, 4);
            GXSetTevOrder(2, 0xFF, 0xFF, 4);
            GXSetTevColorIn(0, 0xF, 8, 0xA, 0xF);
            GXSetTevColorOp(0, 0, 0, 1, 1, 1);
            GXSetTevAlphaIn(0, 7, 4, 5, 7);
            GXSetTevAlphaOp(0, 0, 0, 1, 1, 1);
            GXSetTevColorIn(1, 0xF, 8, 0xA, 0xF);
            GXSetTevColorOp(1, 0, 0, 1, 1, 2);
            GXSetTevAlphaIn(1, 7, 4, 5, 7);
            GXSetTevAlphaOp(1, 0, 0, 1, 1, 2);
            GXSetTevColorIn(2, 2, 4, 5, 0xF);
            GXSetTevColorOp(2, 0, 0, 0, 1, 0);
            GXSetTevAlphaIn(2, 7, 6, 1, 7);
            GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
        } else {
            GXLoadTexObj((GXTexObj*)pObj, 0);
            GXLoadTexObj((GXTexObj*)pObj2, 1);
            GXLoadTexObj((GXTexObj*)(pObj2 + 1), 2);
            GXSetNumTexGens(2);
            GXSetNumTevStages(4);
            GXSetTevOrder(0, 1, 2, 4);
            GXSetTevOrder(1, 1, 1, 4);
            GXSetTevOrder(2, 0, 0, 4);
            GXSetTevOrder(3, 0xFF, 0xFF, 4);
            GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xF);
            GXSetTevColorOp(0, 0, 0, 0, 1, 1);
            GXSetTevAlphaIn(0, 7, 4, 5, 7);
            GXSetTevAlphaOp(0, 0, 0, 1, 1, 1);
            GXSetTevColorIn(1, 0xF, 8, 0xA, 0xF);
            GXSetTevColorOp(1, 0, 0, 1, 1, 2);
            GXSetTevAlphaIn(1, 7, 6, 6, 7);
            GXSetTevAlphaOp(1, 0, 0, 0, 1, 2);
            GXSetTevColorIn(2, 0xF, 8, 0xA, 0xF);
            GXSetTevColorOp(2, 0, 0, 1, 1, 3);
            GXSetTevAlphaIn(2, 7, 4, 5, 7);
            GXSetTevAlphaOp(2, 0, 0, 1, 1, 3);
            GXSetTevColorIn(3, 6, 4, 3, 0xF);
            GXSetTevColorOp(3, 0, 0, 0, 1, 0);
            GXSetTevAlphaIn(3, 7, 6, 3, 7);
            GXSetTevAlphaOp(3, 0, 0, 0, 1, 0);
        }
    } else {
        GXSetNumTexGens(0);
        GXSetNumTevStages(1);
        GXSetTevOrder(0, 0xFF, 0xFF, 4);
        if (*lbl_80280DC8) {
            GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xA);
            GXSetTevColorOp(0, 0, 0, 0, 1, 0);
            GXSetTevAlphaIn(0, 7, 7, 7, 6);
            GXSetTevAlphaOp(0, 0, 0, 1, 1, 0);
        } else {
            GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xA);
            GXSetTevColorOp(0, 0, 0, 0, 1, 0);
            GXSetTevAlphaIn(0, 7, 7, 7, 5);
            GXSetTevAlphaOp(0, 0, 0, 1, 1, 0);
        }
    }
}

// A command of the display list: a primitive (0 points, 1 a line strip, 2 triangles, 3 a triangle
// strip), its vertex layout and its vertex count, then the vertices as u16 array indexes. An array
// the list has but a layout leaves out gets index 0. Layouts 1 and 2 carry a morph offset (three
// s8s packed in two u16s), recorded for the vertex (layout 2 only when one of them is above 2).
// Every command handler takes the commands' end (pEnd), which this one does not use.
u16* fn_80074628(u16* p, u16* pEnd) {
    u16 auDelta[2];
    u16 ePrim;
    u16 eLayout;
    u16 nVerts;
    u16 nPos;
    u16 nTex;
    u16 nColour;
    u16 nNormal;
    u16 nTex2;

    ePrim = p[0];
    eLayout = p[1];
    nVerts = p[2];
    p += 3;
    switch (ePrim) {
    case 0:
        fn_80012520(0xB8, 0, nVerts);
        break;
    case 1:
        fn_80012520(0xB0, 0, nVerts);
        break;
    case 2:
        fn_80012520(0x90, 0, nVerts);
        break;
    case 3:
        fn_80012520(0x98, 0, nVerts);
        break;
    }
    switch (eLayout) {
    case 4:
        while (nVerts != 0) {
            nPos = p[0];
            nColour = p[1];
            p += 2;
            fn_80075274(nPos);
            if (lbl_80281EB4) {
                fn_80075268(0);
            }
            fn_8007525C(nColour);
            if (lbl_80281EB3) {
                fn_80075250(0);
            }
            nVerts--;
        }
        break;
    case 0:
    case 3:
        while (nVerts != 0) {
            nPos = p[0];
            nTex = p[1];
            nColour = p[2];
            p += 3;
            fn_80075274(nPos);
            if (lbl_80281EB4) {
                fn_80075268(0);
            }
            fn_8007525C(nColour);
            fn_80075250(nTex);
            nVerts--;
        }
        break;
    case 1:
        while (nVerts != 0) {
            nPos = p[0];
            auDelta[0] = p[1];
            auDelta[1] = p[2];
            nTex = p[3];
            nColour = p[4];
            p += 5;
            fn_80075274(nPos);
            if (lbl_80281EB4) {
                fn_80075268(0);
            }
            fn_8007525C(nColour);
            fn_80075250(nTex);
            // port: the offset's bytes are read in the data's (big-endian) order
            fn_80097330(lbl_80281EA0, nPos, (s8*)auDelta);
            nVerts--;
        }
        break;
    case 2:
        while (nVerts != 0) {
            nPos = p[0];
            auDelta[0] = p[1];
            auDelta[1] = p[2];
            nColour = p[3];
            p += 4;
            fn_80075274(nPos);
            if (lbl_80281EB4) {
                fn_80075268(0);
            }
            fn_8007525C(nColour);
            if (lbl_80281EB3) {
                fn_80075250(0);
            }
            if (AbsAddXor(((s8*)auDelta)[0]) > 2 || AbsAddXor(((s8*)auDelta)[1]) > 2 ||
                AbsAddXor(((s8*)auDelta)[2]) > 2) {
                fn_80097330(lbl_80281EA0, nPos, (s8*)auDelta);
            }
            nVerts--;
        }
        break;
    case 5:
        while (nVerts != 0) {
            nNormal = p[0];
            nPos = p[1];
            nTex = p[2];
            p += 3;
            fn_80075274(nPos);
            fn_80075268(nNormal);
            if (lbl_80281EB5) {
                fn_8007525C(0);
            }
            fn_80075250(nTex);
            nVerts--;
        }
        break;
    case 6:
        while (nVerts != 0) {
            nPos = p[0];
            nNormal = p[1];
            p += 2;
            fn_80075274(nPos);
            fn_80075268(nNormal);
            if (lbl_80281EB5) {
                fn_8007525C(0);
            }
            if (lbl_80281EB3) {
                fn_80075250(0);
            }
            nVerts--;
        }
        break;
    case 7:
        while (nVerts != 0) {
            nPos = *p;
            p += 1;
            fn_80075274(nPos);
            if (lbl_80281EB4) {
                fn_80075268(0);
            }
            if (lbl_80281EB5) {
                fn_8007525C(0);
            }
            if (lbl_80281EB3) {
                fn_80075250(0);
            }
            nVerts--;
        }
        break;
    case 11:
        while (nVerts != 0) {
            nPos = p[0];
            nTex = p[1];
            nColour = p[2];
            nTex2 = p[3];
            p += 4;
            fn_80075274(nPos);
            if (lbl_80281EB4) {
                fn_80075268(0);
            }
            fn_8007525C(nColour);
            fn_80075250(nTex);
            fn_80075250(nTex2);
            nVerts--;
        }
        break;
    }
    fn_800124A8();
    return p;
}

// A command of the display list: a texture by its 64-bit id (four u16s, high first), with
// bSecond a second one; both found (fn_800102DC) set up the texture stages (fn_800740F4 for
// two, fn_800738DC for one, which also gets *pbFirst). A NULL p sets up no texture.
u16* fn_80074A24(u16* p, u16* pEnd, u8 bSecond, u8* pbFirst) {
    u64 uId;
    u64 uId2;
    TexBank* pBank;
    TexEntry* pTex;
    TexBank* pBank2;
    TexEntry* pTex2;
    int i;

    uId = 0;
    uId2 = 1;
    if (p != NULL) {
        for (i = 0; i < 4; i++) {
            uId <<= 16;
            uId |= p[i];
        }
        p += 4;
    }
    if (bSecond) {
        uId2 = 0;
        if (p != NULL) {
            for (i = 0; i < 4; i++) {
                uId2 <<= 16;
                uId2 |= p[i];
            }
            p += 4;
        }
    }
    if (uId != 0 && uId2 != 0) {
        if (bSecond) {
            fn_800102DC(uId, &pBank, &pTex);
            fn_800102DC(uId2, &pBank2, &pTex2);
            fn_800740F4(pBank, pTex, pBank2, pTex2);
        } else {
            fn_800102DC(uId, &pBank, &pTex);
            fn_800738DC(pBank, pTex, *pbFirst);
        }
    } else {
        fn_800738DC(NULL, NULL, *pbFirst);
    }
    *pbFirst = 0;
    return p;
}

// A command of the display list: three s16s make a texture matrix (fn_80076D68), loaded as
// texture matrix 0x39 (the first two: 0 for none, else 0.5 + 10000 / n; the third: a scale,
// made positive, and its inverse).
u16* fn_80074BE0(u16* p, u16* pEnd) {
    f32 fU;
    f32 fV;
    f32 fScale;
    f32 fInv;
    u16* pNext;

    fInv = fV = fU = 0.0f;
    if (p[0] != 0) {
        fU = 0.5f + 10000.0f / (s16)p[0];
    }
    if (p[1] != 0) {
        fV = 0.5f + 10000.0f / (s16)p[1];
    }
    fScale = (s16)p[2];
    pNext = p + 3;
    if (fScale) {
        fInv = 1.0f / fScale;
    }
    if (fScale < 0.0f) {
        fScale *= -1.0f;
    }
    GXLoadTexMtxIndx(fn_80076D68(fU, fV, fScale, fInv), 0x39, 1);
    return pNext;
}

// A command of the display list with one u16 argument, which the list skips.
u16* fn_80074CEC(u16* p, u16* pEnd) {
    return p + 1;
}

// A command of the display list: the blend mode (0 none, 1 alpha blend, 2 source times alpha
// replacing what is drawn there, 3 subtract).
u16* fn_80074CF4(u16* p, u16* pEnd) {
    switch (*p++) {
    case 0:
        GXSetBlendMode(0, 4, 5, 0);
        break;
    case 1:
        GXSetBlendMode(1, 4, 5, 0);
        break;
    case 2:
        GXSetBlendMode(1, 4, 0, 0);
        break;
    case 3:
        GXSetBlendMode(3, 1, 1, 0);
        break;
    }
    return p;
}

// Build pAnim's display list from pCmds's commands (1 a primitive, 2 and 7 one or two textures,
// 3 skipped, 4 the blend mode, 5 a texture matrix, 6 the morph and its f32), in a buffer then
// copied over the commands. Types 1, 3 and 10 draw untextured, 2 and 3 morph, 4 and 5 use a
// texture matrix; 10 has no colours or normals either.
void fn_80074DA8(ShaderVtxArrays* pArrays, int eType, MorphAnim* pAnim, ShaderCmds* pCmds) {
    void* pList;
    u8 bFirst;
    u16* p;
    u16* pEnd;
    u32 nWords;
    u8 bArrays;
    u32 nSize;
    u8 bTexMtx;

    bTexMtx = 0;
    lbl_80281EA0 = NULL;
    lbl_80281EB5 = lbl_80281EB4 = lbl_80281EB3 = 0;
    bFirst = 1;
    if (pCmds->nFrames > 1) {
        pAnim->b8 = 1;
    } else {
        pAnim->b8 = 0;
    }
    nWords = pCmds->nWords;
    p = pCmds->pCmds;
    nSize = (((nWords << 1) + 0x1F) & ~0x1F) + 0x800;   // the commands' bytes, rounded up, + 2 KB
    pList = fn_80009B34(nSize, 1, 32, "GoShaderObjectCommon_ShaderObjectsData_Gc.c", 1039);
    DCInvalidateRange(pList, nSize);
    GXBeginDisplayList(pList, nSize);
    GXResetWriteGatherPipe();
    bArrays = 0;
    GXClearVtxDesc();
    if (eType != 1 && eType != 3 && eType != 10 && pArrays->p14 != NULL) {
        GXSetVtxDesc(13, 3);
        GXSetArray(13, pArrays->p14, 4);
        bArrays = 1;
        lbl_80281EB3 = 1;
    }
    if (pArrays->apPos[pCmds->nFrame] != NULL) {
        GXSetVtxDesc(9, 3);
        if (pAnim->b8 == 0) {
            GXSetArray(9, pArrays->apPos[pCmds->nFrame], 12);
        }
        bArrays = 1;
    }
    if (eType != 10 && pArrays->p18 != NULL) {
        GXSetVtxDesc(11, 3);
        GXSetArray(11, pArrays->p18, 4);
        bArrays = 1;
        lbl_80281EB5 = 1;
    }
    if (eType != 10 && pArrays->p1C != NULL) {
        GXSetVtxDesc(10, 3);
        GXSetArray(10, pArrays->p1C, 12);
        bArrays = 1;
        lbl_80281EB4 = 1;
    }
    if (eType == 5 || eType == 4) {
        GXSetTexCoordGen2(0, 1, 4, 0x39, 0, 0x7D);
        fn_80076BF0();
        bArrays = 1;
        bTexMtx = 1;
    } else if (eType != 10) {
        GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    }
    if (bArrays) {
        GXInvalidateVtxCache();
    }
    if (pCmds->uFlags & 1) {
        fn_8001618C(1);
    }
    if (eType == 1 || eType == 3) {
        fn_80074A24(NULL, NULL, 0, &bFirst);
    }
    if (eType == 2 || eType == 3) {
        fn_8009727C(pAnim);
    }
    pEnd = p + nWords;
    while (p < pEnd) {
        switch (*p++) {
        case 1:
            p = fn_80074628(p, pEnd);
            break;
        case 2:
            if (eType != 1 && eType != 3 && eType != 10) {
                p = fn_80074A24(p, pEnd, 0, &bFirst);
            } else {
                p += 4;
            }
            break;
        case 7:
            if (eType != 1 && eType != 3 && eType != 10) {
                if (bFirst) {
                    GXSetVtxDesc(14, 3);
                    GXSetTexCoordGen2(1, 1, 5, 0x3C, 0, 0x7D);
                    GXSetArray(14, pArrays->p14, 4);
                    GXInvalidateVtxCache();
                }
                p = fn_80074A24(p, pEnd, 1, &bFirst);
            } else {
                p += 8;
            }
            break;
        case 3:
            p = fn_80074CEC(p, pEnd);
            break;
        case 4:
            p = fn_80074CF4(p, pEnd);
            break;
        case 5:
            if (eType == 5 || eType == 4) {
                p = fn_80074BE0(p, pEnd);
            } else {
                p += 3;
            }
            break;
        case 6:
            if (lbl_80281EA0 == NULL) {
                lbl_80281EA0 = pAnim;
            }
            // port: an f32 stored in two of the u16 commands, in the data's (big-endian) order
            lbl_80281EA0->fC = *(f32*)p;
            p += 2;
            lbl_80281EA0->p1C = pArrays->apPos[pCmds->nFrame];
            break;
        }
    }
    if (bTexMtx) {
        GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    }
    pAnim->n0 = GXEndDisplayList();
    // the commands' address rounded up to 32 bytes
    pAnim->p4 = (void*)(((uptr)pCmds->pCmds + 0x1F) & ~(uptr)0x1F);
    Mem_cpy(pAnim->p4, pList, pAnim->n0);
    DCFlushRange(pAnim->p4, pAnim->n0);
    fn_80009E70(pList);
    if (eType == 2 || eType == 3) {
        fn_80097474(pAnim);
    }
    fn_8001618C(0);
}

void fn_8007524C(void) {
}

// Write a vertex's texture coordinate, colour, normal and position index to GX's FIFO (the GX
// index writes, not inlined).
void fn_80075250(u16 nIndex) {
    *(volatile u16*)0xCC008000 = nIndex;
}

void fn_8007525C(u16 nIndex) {
    *(volatile u16*)0xCC008000 = nIndex;
}

void fn_80075268(u16 nIndex) {
    *(volatile u16*)0xCC008000 = nIndex;
}

void fn_80075274(u16 nIndex) {
    *(volatile u16*)0xCC008000 = nIndex;
}
