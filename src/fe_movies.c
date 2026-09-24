// fe_movies.c (TW06's file name, a guess from the filemap): the front end's movies. Not yet
// decompiled; the sweep code below is the matched small functions.

#include "game_types.h"
#include "llpict.h"
#include "frontend/fe.h"
#include "game/frontend.h"
#include "llvideo.h"
#include "core/startup.h"
#include "frontend/uisvec.h"
#include "unsorted/cull.h"

void fn_80008380(void);
void fn_80092250(f32* pA, f32* pB, f32* pOut);
void fn_80092080(LLPict* pPict, f32 fAlpha);    // draws the picture at that alpha
void fn_80091FC0(LLPict* pPict, int nFrames, f32 fStep);
s32  fn_800171B0(void);                 // ViewController.c
void fn_800760D8(LLPict* pPict);        // LLVideo.c
void fn_800760F4(f32* pUV, LLPict* pPict);  // LLVideo.c
void fn_80016978(f32 x0, f32 y0, f32 x1, f32 y1);
f32  fn_8006E118(u64 tEnd, u64 tStart);    // GameManager.c: seconds between two time stamps
void fn_8000AE48(f32* pA, f32* pB, f32* pOut);     // pOut = pA * pB, element by element
void fn_80090D28(FEQuad* pQuad);
void fn_800913EC(s16 nTable, s16 nEntry);
void fn_80091460(s16 nTable, s16 nEntry);
f32* fn_80093268(void);             // uiTransform.c
void fn_8009222C(f32* pOut, LLPict* pPict);
void fn_80090940(int nEntry);      // makes the picture of entry nEntry (lbl_801D87C0's table)
void fn_800909B4(int nEntry);       // sets flag 0x10 on entry nEntry (lbl_801D87C0's table)

// ---- sweep code (not yet cleaned up) ----

void fn_80091708(void);
void fn_80006EDC();
void fn_80006FE8();
void fn_80007254();
void fn_800083A0();
void fn_80091BDC(int nPoint);
void fn_80091B98(s32 p0);
void fn_80012EF8();
void fn_8005CC64();
void fn_80091DB8(int nFrames);
void fn_80091D84(void);
void fn_80091EE4(void);
void fn_8009220C(void);
f32 fn_80092210(void);
void fn_80013E30();
void fn_80092274(s32 p0);

void fn_80091454(void) {
    lbl_80281370 = 0;
}

void fn_80091708(void) {
}

void fn_80091B98(s32 p0) {
    fn_80006EDC();
    fn_80091BDC(p0);
    fn_80006FE8();
    fn_80007254();
    fn_800083A0();
    fn_800A4BDC();
}

void fn_80091D84(void) {
    fn_80091DB8(30);
    fn_8005CC64(0, 0);
    fn_80012EF8();
}

void fn_80091EE4(void) {
}

void fn_8009220C(void) {
}

f32 fn_80092210(void) {
    if (lbl_80281F1C != NULL) {
        return lbl_80281F1C->f18;
    }
    return 0.0f;
}

void fn_80092274(s32 p0) {
    fn_80013E30(*(s32*)((u8*)lbl_80280DF0), p0, lbl_80280DF0);
}

TexEntry* fn_800922A0(TexBank* pBank) {
    return pBank->p8;
}

// ---- end of sweep code ----

// Eight points across the screen, an eighth apart, all at height 0.839 (fn_80091BDC draws at them).
void fn_8009170C(void) {
    lbl_801D8818[0][0] = 0.0f;
    lbl_801D8818[0][1] = 0.839f;
    lbl_801D8818[1][0] = 0.125f;
    lbl_801D8818[1][1] = 0.839f;
    lbl_801D8818[2][0] = 0.25f;
    lbl_801D8818[2][1] = 0.839f;
    lbl_801D8818[3][0] = 0.375f;
    lbl_801D8818[3][1] = 0.839f;
    lbl_801D8818[4][0] = 0.5f;
    lbl_801D8818[4][1] = 0.839f;
    lbl_801D8818[5][0] = 0.625f;
    lbl_801D8818[5][1] = 0.839f;
    lbl_801D8818[6][0] = 0.75f;
    lbl_801D8818[6][1] = 0.839f;
    lbl_801D8818[7][0] = 0.875f;
    lbl_801D8818[7][1] = 0.839f;
}

// Load the texture bank from LoadData.c's 'txf2' copy, unless the session has flag 4.
void fn_80091778(void) {
    if (gSession.uFlags & 4) return;
    lbl_80281378 = fn_800107C0(lbl_80281C0C, NULL, 0);
    lbl_80281F20 = fn_800106C4(lbl_80281378);
    lbl_80281F24 = fn_800922A0(lbl_80281F20);
}

// Decodes the picture in the 'load' object, once.
void fn_800917C8(void) {
    if (lbl_801D8858.p30 == NULL) {
        lbl_801D8858.p30 = fn_8002FD00(lbl_80281C04, lbl_801A25F0.uSize);
    }
}

// Free the picture fn_800917C8 decoded, unless the session has flag 4.
void fn_80091818(void) {
    if (!(gSession.uFlags & 4) && lbl_801D8858.p30 != NULL) {
        fn_8002FE70(lbl_801D8858.p30);
        fn_8002FEAC();
        lbl_801D8858.p30 = NULL;
    }
}

// Free the bank fn_80091778 loaded.
void fn_80091870(void) {
    if (gSession.uFlags & 4) return;
    fn_80010544(lbl_80281378);
}

// Set lbl_801D8858 up once, unless the session has flag 4: the clock, the number of players in
// game type 4 (else 0) and values from it, then the 'load' object's picture (fn_800917C8).
void fn_800918A4(void) {
    if (!(gSession.uFlags & 4) && !lbl_801D8858.b18) {
        lbl_801D8858.b18 = 1;
        lbl_801D8858.n1C = -1;
        lbl_801D8858.f10 = 0.0f;
        lbl_801D8858.u20 = fn_80095368();
        lbl_801D8858.n0 = 0;
        if (gSession.nGameType == 4) {
            lbl_801D8858.n14 = gSession.nNumPlayers;
        } else {
            lbl_801D8858.n14 = 0;
        }
        lbl_801D8858.fC = lbl_801D8858.f4 = (4.83f * lbl_801D8858.n14 + 3.1f) / 8.0f;
        lbl_801D8858.f8 = 0.0f;
        fn_800917C8();
    }
}

// Copy pSrc to pDst with its colour tinted by lbl_80281F28/lbl_80281F2C; without bTint the colour
// is white and only the alpha is tinted.
void fn_80090B80(FEVertex* pSrc, FEVertex* pDst, u8 bTint) {
    memcpy(pDst, pSrc, sizeof(FEVertex));
    if (bTint) {
        pDst->au14[0] = lbl_80281F28[0] * (pSrc->au14[0] + lbl_80281F2C[0]);
        pDst->au14[1] = lbl_80281F28[1] * (pSrc->au14[1] + lbl_80281F2C[1]);
        pDst->au14[2] = lbl_80281F28[2] * (pSrc->au14[2] + lbl_80281F2C[2]);
        pDst->au14[3] = lbl_80281F28[3] * (pSrc->au14[3] + lbl_80281F2C[3]);
    } else {
        pDst->au14[0] = 0xFF;
        pDst->au14[1] = 0xFF;
        pDst->au14[2] = 0xFF;
        pDst->au14[3] = lbl_80281F28[3] * (pSrc->au14[3] + lbl_80281F2C[3]);
    }
}

// pQuad's message handler: -3/-1 act on its UI file entry, -2 draws it, 0 and 1 set a corner's
// position and texture coordinates, 2 and 3 its colour and alpha (corner -1: all four), 5 sets the
// index pair (by bSplit, from one packed number or two).
void fn_800914DC(FEQuad* pQuad, int nMsg, u32 bSplit, FEMsgArg* pArgs) {
    s16 nOld;

    switch (nMsg) {
    case -1:
        // port: EA passes arguments fn_800913EC ignores
        ((void (*)(s16, s16, s16, int, int))fn_800913EC)(pQuad->n2, pQuad->n0, pQuad->nA, 0, 0);
        break;
    case -2:
        // port: EA passes arguments fn_80090D28 ignores
        ((void (*)(FEQuad*, int, int))fn_80090D28)(pQuad, 0, 0);
        break;
    case -3:
        // port: EA passes arguments fn_80091460 ignores
        ((void (*)(s16, s16, s16, int, int))fn_80091460)(pQuad->n2, pQuad->n0, pQuad->nA, 0, 0);
        break;
    case 0:
        pQuad->aVtx[pArgs[0].n].f8 = pArgs[1].f;
        pQuad->aVtx[pArgs[0].n].fC = pArgs[2].f;
        pQuad->aVtx[pArgs[0].n].f10 = pArgs[3].f;
        break;
    case 1:
        pQuad->aVtx[pArgs[0].n].f0 = pArgs[1].f;
        pQuad->aVtx[pArgs[0].n].f4 = pArgs[2].f;
        break;
    case 3:
        if (pArgs[0].n == -1) {
            pQuad->aVtx[0].au14[3] = pArgs[1].n;
            pQuad->aVtx[1].au14[3] = pArgs[1].n;
            pQuad->aVtx[2].au14[3] = pArgs[1].n;
            pQuad->aVtx[3].au14[3] = pArgs[1].n;
        } else {
            pQuad->aVtx[pArgs[0].n].au14[3] = pArgs[1].n;
        }
        break;
    case 2:
        if (pArgs[0].n == -1) {
            pQuad->aVtx[0].au14[0] = pArgs[1].n;
            pQuad->aVtx[0].au14[1] = pArgs[2].n;
            pQuad->aVtx[0].au14[2] = pArgs[3].n;
            pQuad->aVtx[1].au14[0] = pArgs[1].n;
            pQuad->aVtx[1].au14[1] = pArgs[2].n;
            pQuad->aVtx[1].au14[2] = pArgs[3].n;
            pQuad->aVtx[2].au14[0] = pArgs[1].n;
            pQuad->aVtx[2].au14[1] = pArgs[2].n;
            pQuad->aVtx[2].au14[2] = pArgs[3].n;
            pQuad->aVtx[3].au14[0] = pArgs[1].n;
            pQuad->aVtx[3].au14[1] = pArgs[2].n;
            pQuad->aVtx[3].au14[2] = pArgs[3].n;
        } else {
            pQuad->aVtx[pArgs[0].n].au14[0] = pArgs[1].n;
            pQuad->aVtx[pArgs[0].n].au14[1] = pArgs[2].n;
            pQuad->aVtx[pArgs[0].n].au14[2] = pArgs[3].n;
        }
        break;
    case 5:
        if (pArgs[0].n != -1) {
            nOld = pQuad->n0;
            if (bSplit == 1) {
                pQuad->n0 = pArgs[0].n;
                pQuad->n2 = (u32)pArgs[0].n >> 16;
            } else {
                pQuad->n0 = pArgs[1].n;
                pQuad->n2 = pArgs[0].n;
            }
            // fake match: a no-op; the original compares the old n0 with the new one here
            if (nOld == pQuad->n0) {
                return;
            }
        }
        break;
    case 6:
        break;
    }
}

// Unpack pVtx into four-float arrays: its position (w 1), its texture coordinates (0, 1) and its
// colour, which is then scaled by pScale and offset by pAdd.
void fn_800912F4(FEVertex* pVtx, f32* pPos, f32* pUV, f32* pColour, f32* pScale, f32* pAdd) {
    pPos[0] = pVtx->f8;
    pPos[1] = pVtx->fC;
    pPos[2] = pVtx->f10;
    pPos[3] = 1.0f;
    pUV[0] = pVtx->f0;
    pUV[1] = pVtx->f4;
    pUV[2] = 0.0f;
    pUV[3] = 1.0f;
    pColour[0] = pVtx->au14[0];
    pColour[1] = pVtx->au14[1];
    pColour[2] = pVtx->au14[2];
    pColour[3] = pVtx->au14[3];
    fn_8000AE48(pColour, pScale, pColour);
    fn_80092250(pColour, pAdd, pColour);
}

// Draw pQuad: with a UI file entry, textured by it (flag 1: a texture, flag 2: a movie's current
// picture); its colours tinted, recoloured from the colour table (n4) and scaled by the transform
// level's colour; its corners transformed and projected; nothing when every corner is transparent.
void fn_80090D28(FEQuad* pQuad) {
    FEVertex aVtx[4];
    Vec4 aPos[4];
    Vec4 aOut[4];
    f32 aUV[4][4];
    f32 aColour[4][4];
    f32 vScale[4];
    f32 vAdd[4];
    f32 vPictUV[4];     // fn_8009222C fills two; the original's buffer is 16 bytes
    f32 fDist;
    f32 fZ;
    f32 fProj;
    UIColorEntry* pEntry;
    LLPict* pPict;
    UITransform* pMtx;
    UITransform* pColour;
    UITransform* pAdd;
    TexBank* pBank;
    TexEntry* pTex;
    char* szName;
    int nBank;
    s16 nColour;
    u8 bTint;
    int i;

    fDist = fn_80093268()[2];
    fZ = fn_80092210();
    pMtx = fn_80093274();
    pColour = fn_80093274();
    pAdd = fn_80093274();
    lbl_80281F28 = fn_8016C198()->a;
    lbl_80281F2C = fn_8016C18C()->a;
    bTint = 1;
    if (pQuad->n2 != -1) {
        pEntry = lbl_80281F1C->pFile->p8->apTables[pQuad->n2]->apEntries[pQuad->n0];
        szName = pEntry->szC;
        if (pEntry->u0 & 1) {
            if (gSession.nGameType == 3) {
                pBank = lbl_801A26DC[pQuad->n0];
                pTex = fn_800922A0(pBank);
                fn_8005CC64(pBank, pTex);
            } else {
                // the texture bank by the entry's name (fn_8008FFF0: -1, 0 or 1)
                nBank = fn_8008FFF0(szName);
                fn_8005CC64(lbl_80281F1C->p8->ap4[nBank], pEntry->p4);
            }
            if (!(pQuad->n8 & 1)) {
                bTint = 0;
            }
        } else if (pEntry->u0 & 2) {
            pPict = (LLPict*)pEntry->p8;
            fn_800760D8(pPict);
        }
        fn_80014118(0x50);
    } else {
        fn_80014118(0x40);
    }
    fn_80013EEC(fn_8001614C());
    vScale[0] = (1.0f / 511.0f) * pColour->w40.a[0];
    vScale[1] = (1.0f / 511.0f) * pColour->w40.a[1];
    vScale[2] = (1.0f / 511.0f) * pColour->w40.a[2];
    vScale[3] = (1.0f / 511.0f) * pColour->w40.a[3];
    vAdd[0] = pAdd->f50[0];
    vAdd[1] = pAdd->f50[1];
    vAdd[2] = pAdd->f50[2];
    vAdd[3] = pAdd->f5C;
    nColour = pQuad->n4;
    if (lbl_80281F1C->p14 != NULL && nColour < (s16)lbl_80281F1C->p14->nCount && nColour != -1) {
        // the table's colours are alpha, blue, green, red
        pQuad->aVtx[0].au14[3] = lbl_80281F1C->p14->apEntries[nColour]->p8[0];
        pQuad->aVtx[1].au14[3] = lbl_80281F1C->p14->apEntries[nColour]->p8[0];
        pQuad->aVtx[2].au14[3] = lbl_80281F1C->p14->apEntries[nColour]->p8[0];
        pQuad->aVtx[3].au14[3] = lbl_80281F1C->p14->apEntries[nColour]->p8[0];
        pQuad->aVtx[0].au14[2] = lbl_80281F1C->p14->apEntries[nColour]->p8[1];
        pQuad->aVtx[1].au14[2] = lbl_80281F1C->p14->apEntries[nColour]->p8[1];
        pQuad->aVtx[2].au14[2] = lbl_80281F1C->p14->apEntries[nColour]->p8[1];
        pQuad->aVtx[3].au14[2] = lbl_80281F1C->p14->apEntries[nColour]->p8[1];
        pQuad->aVtx[0].au14[1] = lbl_80281F1C->p14->apEntries[nColour]->p8[2];
        pQuad->aVtx[1].au14[1] = lbl_80281F1C->p14->apEntries[nColour]->p8[2];
        pQuad->aVtx[2].au14[1] = lbl_80281F1C->p14->apEntries[nColour]->p8[2];
        pQuad->aVtx[3].au14[1] = lbl_80281F1C->p14->apEntries[nColour]->p8[2];
        pQuad->aVtx[0].au14[0] = lbl_80281F1C->p14->apEntries[nColour]->p8[3];
        pQuad->aVtx[1].au14[0] = lbl_80281F1C->p14->apEntries[nColour]->p8[3];
        pQuad->aVtx[2].au14[0] = lbl_80281F1C->p14->apEntries[nColour]->p8[3];
        pQuad->aVtx[3].au14[0] = lbl_80281F1C->p14->apEntries[nColour]->p8[3];
    }
    fn_80090B80(&pQuad->aVtx[0], &aVtx[0], bTint);
    fn_80090B80(&pQuad->aVtx[1], &aVtx[1], bTint);
    fn_80090B80(&pQuad->aVtx[2], &aVtx[2], bTint);
    fn_80090B80(&pQuad->aVtx[3], &aVtx[3], bTint);
    fn_800912F4(&aVtx[0], &aPos[0].x, aUV[0], aColour[0], vScale, vAdd);
    fn_800912F4(&aVtx[1], &aPos[1].x, aUV[1], aColour[1], vScale, vAdd);
    fn_800912F4(&aVtx[2], &aPos[2].x, aUV[2], aColour[2], vScale, vAdd);
    fn_800912F4(&aVtx[3], &aPos[3].x, aUV[3], aColour[3], vScale, vAdd);
    if (pQuad->n2 != -1 && (pEntry->u0 & 2)) {
        // a movie's picture fills only part of its texture
        fn_8009222C(vPictUV, pPict);
        aUV[0][0] *= vPictUV[0];
        aUV[0][1] *= vPictUV[1];
        aUV[1][0] *= vPictUV[0];
        aUV[1][1] *= vPictUV[1];
        aUV[2][0] *= vPictUV[0];
        aUV[2][1] *= vPictUV[1];
        aUV[3][0] *= vPictUV[0];
        aUV[3][1] *= vPictUV[1];
    }
    for (i = 0; i < 4; i++) {
        fn_800BAD60(pMtx->m, &aPos[i], &aOut[i]);
        fProj = fDist / (fDist + aOut[i].z);
        aOut[i].x *= fProj;
        aOut[i].y *= fProj;
        aOut[i].z = fZ;
    }
    fn_80012F34(0);
    fn_80012F18(7);
    fn_80012EF8();
    if (aColour[0][3] != 0.0f || aColour[1][3] != 0.0f || aColour[2][3] != 0.0f
        || aColour[3][3] != 0.0f) {
        if (pQuad->n2 == -1) {
            fn_8001644C(0xA0, &aOut[0].x, aColour[0], NULL, 4);
        } else {
            fn_8001644C(0xA0, &aOut[0].x, aColour[0], aUV[0], 4);
        }
    }
}

// Make the picture of UI file entry (nTable, nEntry) when its flags have 2 set and 1 clear.
void fn_800913EC(s16 nTable, s16 nEntry) {
    u32 uFlags;

    if (nTable == -1) return;
    uFlags = lbl_80281F1C->pFile->p8->apTables[nTable]->apEntries[nEntry]->u0;
    if (!(uFlags & 1) && (uFlags & 2)) {
        fn_80090940(nEntry);
    }
}

// For UI file entry (nTable, nEntry) with flags 2 set and 1 clear: fn_80008380, then flag 0x10.
void fn_80091460(s16 nTable, s16 nEntry) {
    u32 uFlags;

    if (nTable == -1) return;
    uFlags = lbl_80281F1C->pFile->p8->apTables[nTable]->apEntries[nEntry]->u0;
    if (!(uFlags & 1) && (uFlags & 2)) {
        fn_80008380();
        fn_800909B4(nEntry);
    }
}

// Update the loading screen set up by fn_800918A4, unless the session has flag 4: add the time since
// the last update to f10, and once it passes f4 (or f8, which steps by 2) redraw the picture and the
// tiles shown so far, showing one more each time f10 passes f4. nMode 1 draws every tile left and
// ends it (b18 cleared).
void fn_8009198C(int nMode) {
    f32 fSecs;
    int i;

    if (gSession.uFlags & 4) return;
    if (nMode == 1) {
        lbl_801D8858.b18 = 0;
    }
    lbl_801D8858.u28 = fn_80095368();
    fSecs = fn_8006E118(lbl_801D8858.u28, lbl_801D8858.u20);
    lbl_801D8858.u20 = lbl_801D8858.u28;
    lbl_801D8858.f10 += fabsf(fSecs);
    if (nMode != 1 && lbl_801D8858.f10 <= lbl_801D8858.f4) {
        if (lbl_801D8858.f10 > lbl_801D8858.f8) {
            lbl_801D8858.f8 += 2.0f;
        } else {
            return;
        }
    }
    fn_80006EDC();
    fn_80092080(lbl_801D8858.p30, 1.0f);
    if (lbl_801D8858.n1C >= 0) {
        for (i = 0; i <= lbl_801D8858.n1C; i++) {
            if (i >= 8) break;
            fn_80091BDC(i);
        }
    }
    fn_80006FE8();
    fn_800083A0();
    fn_80007254();
    fn_800A4BDC();
    fn_80008380();
    if (lbl_801D8858.f10 > lbl_801D8858.f4 || lbl_801D8858.n1C == -1) {
        if (lbl_801D8858.f10 > lbl_801D8858.f4) {
            lbl_801D8858.f4 += lbl_801D8858.fC;
        }
        if (++lbl_801D8858.n1C >= 8) return;
        fn_80091B98(lbl_801D8858.n1C);
    }
    if (nMode == 1) {
        if (lbl_801D8858.n1C < 0) {
            lbl_801D8858.n1C = 0;
        }
        if (lbl_801D8858.n1C > 7) {
            lbl_801D8858.n1C = 7;
        }
        for (i = lbl_801D8858.n1C + 1; i < 8; i++) {
            fn_80091B98(i);
        }
        lbl_801D8858.b18 = 0;
    }
}

// Draw tile nPoint of the texture bank fn_80091778 loaded at point nPoint of lbl_801D8818, an
// eighth of the screen wide: tiles 0-3 come from the top half of the texture, 4-7 from the bottom
// (their u runs past 1 and wraps).
void fn_80091BDC(int nPoint) {
    f32 afColour[4];
    f32 afXY[8];
    f32 afUV[8];

    fn_80016978(0.0f, 0.0f, 1.0f, 1.0f);
    fn_80035118(4, 5);
    fn_80012F50(0, 6, 0x80);
    fn_80012F18(7);
    fn_8001425C(0);
    fn_80012F34(0);
    fn_8005CC64(lbl_80281F20, lbl_80281F24);
    fn_80014118(0x50);
    fn_80012EF8();
    afColour[3] = 0.25f;        // EA code: overwritten at once
    afColour[0] = 0.5f;
    afColour[1] = 0.5f;
    afColour[2] = 0.5f;
    afColour[3] = 0.5f;
    fn_80014194(afColour);
    fn_800141F8(afXY, NULL, lbl_801D8818[nPoint][0], lbl_801D8818[nPoint][1],
                lbl_801D8818[nPoint][0] + 0.125f, lbl_801D8818[nPoint][1] + 0.142f);
    if (nPoint < 4) {
        afUV[0] = 0.25f * nPoint;
        afUV[1] = 0.0f;
        afUV[2] = 0.0f;
        afUV[3] = 0.0f;
        afUV[4] = 0.25f + 0.25f * nPoint;
        afUV[5] = 0.5f;
        afUV[6] = 0.0f;
        afUV[7] = 0.0f;
    } else {
        afUV[0] = 0.25f * nPoint;
        afUV[1] = 0.5f;
        afUV[2] = 0.0f;
        afUV[3] = 0.0f;
        afUV[4] = 0.25f + 0.25f * nPoint;
        afUV[5] = 1.0f;
        afUV[6] = 0.0f;
        afUV[7] = 0.0f;
    }
    fn_8001644C(0xA1, afXY, 0, afUV, 2);
}

// Decode the picture in the 'load' object, show it for nFrames frames (fading in over 30), then
// free it.
void fn_80091DB8(int nFrames) {
    LLPict* pPict;

    pPict = fn_8002FD00(lbl_80281C04, lbl_801A25F0.uSize);
    fn_80091FC0(pPict, nFrames, 1.0f / 30.0f);
    fn_80008380();
    fn_8002FE70(pPict);
    fn_8002FEAC();
}

// The movies after a round in game type 1 (uiProcessInterface.c fn_80090400): "eas", then, unless
// the session has flag 0x4000, one of the two cameo movies "tigcam01"/"tigcam02" at random
// (skippable with any button); then the first 'LEGL' picture startUp.c kept, shown for 180
// frames and freed.
void fn_80091EE8(void) {
    char szPath[0x40];          // the size is unknown: the frame leaves 0x40 bytes for it
    char szName[0x40];          // the size is unknown: the frame leaves 0x40 bytes for it
    LLPict* pPict;

    FE_MakeMoviePath("eas", szPath);
    fn_80075FB8(szPath, NULL, 0, 0);
    if (!(gSession.uFlags & 0x4000)) {
        sprintf(szName, "tigcam%02d", (s16)((Rand_Next(0) & 1) + 1));
        FE_MakeCameoMoviePath(szName, szPath);
        fn_80075FB8(szPath, fn_80076FDC, 0, 0);
    }
    pPict = fn_8002FD00(lbl_80282134, lbl_8028212C);
    fn_80091FC0(pPict, 180, 1.0f / 30.0f);
    fn_80008380();
    fn_8002FE70(pPict);
    fn_80009E70(lbl_80282134);
    lbl_80282134 = NULL;
}

// Show pPict for nFrames frames, fading it in by fStep a frame (up to 1).
void fn_80091FC0(LLPict* pPict, int nFrames, f32 fStep) {
    f32 fAlpha = 0.0f;
    int i;

    for (i = 0; i < nFrames; i++) {
        fn_80006EDC();
        fAlpha += fStep;
        if (fAlpha > 1.0f) {
            fAlpha = 1.0f;
        }
        fn_80092080(pPict, fAlpha);
        fn_80006FE8();
        fn_80007254();
        fn_800083A0();
        fn_800A4BDC();
        fn_800B7490();
    }
}

// Draw pPict over the whole 512x448 screen at fAlpha, through a frame buffer of its own, then put
// the previous render slot back.
void fn_80092080(LLPict* pPict, f32 fAlpha) {
    f32 afColour[4];
    f32 afXY[8];
    f32 afUV[8];
    GoFrameBuf frameBuf;
    s32 nOld;

    fn_800760D8(pPict);
    fn_80035118(4, 5);
    fn_80014118(0x50);
    fn_80012F50(0, 6, 0x80);
    fn_80012F18(7);
    nOld = fn_800171B0();
    fn_8006E26C(&frameBuf, 0.0f, 0.0f, 512.0f, 448.0f, 1.0f, 1.0f);
    // port: the render slot is typed s32 but holds a pointer
    fn_80092274((s32)&frameBuf);
    fn_80013EEC(fn_8001614C());
    fn_80012EF8();
    afColour[0] = 0.5f;
    afColour[1] = 0.5f;
    afColour[2] = 0.5f;
    afColour[3] = 0.5f * fAlpha;
    fn_800141F8(NULL, afXY, 0.0f, 0.0f, 1.0f, 1.0f);
    fn_800760F4(afUV, pPict);
    fn_80014194(afColour);
    fn_8001644C(0xA1, afXY, 0, afUV, 2);
    fn_80092274(nOld);
}

// Free the pixel data of every bank whose entry in lbl_801D8890 has a positive n4.
void fn_80092198(void) {
    int i;

    for (i = 0; i < FE_NUM_801D8890; i++) {
        if (lbl_801D8890[i].n4 > 0) {
            fn_80008380();
            fn_8000FFAC(lbl_801A26DC[i]);
        }
    }
}

// The picture's f6C and f70, then 0 and 1.
void fn_8009222C(f32* pOut, LLPict* pPict) {
    pOut[0] = pPict->f6C;
    pOut[1] = pPict->f70;
    pOut[2] = 0.0f;
    pOut[3] = 1.0f;
}

// b + a into out (four floats)
#ifdef __MWERKS__
asm void fn_80092250(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 0, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 0, 0
    ps_add f2, f2, f0
    ps_add f3, f3, f1
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_80092250(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
    pOut[3] = pB[3] + pA[3];
}
#endif
