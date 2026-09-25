// uiArc.c (our name): a menu UI element drawn as an arc or circle: nSegments pieces from fStart
// to fEnd degrees (0 to 360 by default), shaded from one colour to another (fn_80102AC8), and
// the message handler that sets it up (fn_80103684).

#include "game_types.h"
#include "llpict.h"
#include "frontend/fe.h"
#include "game/frontend.h"
#include "frontend/uisvec.h"
#include "unsorted/cull.h"
#include "camera.h"

// fe_movies.c
void fn_800913EC(s16 n2, s16 n0, s16 n8, s32 a, s32 b);
void fn_80091460(s16 n2, s16 n0, s16 n8, s32 a, s32 b);
void fn_800912F4(FEVertex* pVtx, f32* pPos, f32* pUV, f32* pColour, f32* pScale, f32* pAdd);
void fn_8009222C(f32* pOut, LLPict* pPict);
f32  fn_80092210(void);
f32* fn_80093268(void);                 // uiTransform.c
void fn_800760D8(LLPict* pPict);        // LLVideo.c

// The tint of the last draw (as fe_movies.c's lbl_80281F28/lbl_80281F2C); nothing here reads it.
f32* lbl_80282458;
f32* lbl_8028245C;

// Draw pArc: nSegments quads from its inner to its outer radius (v20 * v28 with flag 1, else the
// centre; v28), from fStart to fEnd degrees (flag 0x10: the whole circle), textured like fe_movies.c's
// quads, its colour aColorA (flag 0x20: shaded towards aColorB; with flag 4 and not 2, white with
// only the alpha of the colours). a and b are unused: fn_80103684 passes 0, 0.
void fn_80102AC8(UIArc* pArc, s32 a, s32 b) {
    FEVertex aVtx[4];
    Vec4 aPos[4];
    Vec4 aOut[4];
    f32 aUV[4][4];
    f32 aColour[4][4];
    f32 vScale[4];
    f32 vAdd[4];
    f32 vPictUV[4];     // fn_8009222C fills two
    GXColor colorA;
    GXColor colorB;
    // fake match: scalar declaration order controls CodeWarrior's saved-FPR allocation.
    f32 fInnerY;
    f32 fZ;
    f32 fStart;
    f32 fEnd;
    f32 fV;
    f64 fU0;            // f64: the original keeps these in 8-byte stack slots
    f64 fV0;
    f64 fU1;
    f64 fV1;
    f64 fU2;
    f64 fV2;
    f64 fU3;
    f64 fV3;
    f32 fProj;
    f32 fU;
    f32 fAngle;
    f32 fStep;
    f32 fOuterY;
    f32 fDist;
    f32 fOuterX;
    f32 fInnerX;
    f32 fSin;
    f32 fCos;
    f32 fS;
    f32 fT;
    f32 fR;
    f32 fG;
    f32 fB;
    f32 fA;
    UIFileEntry* pEntry;
    LLPict* pPict;
    char* pName;
    UITransform* pMtx;
    UITransform* pColour;
    UITransform* pAdd;
    TexBank* pBank;
    int nBank;
    u32 nHi;
    int nLo;
    int i;
    int j;

    fDist = fn_80093268()[2];
    fZ = fn_80092210();
    pMtx = fn_80093274();
    pColour = fn_80093274();
    pAdd = fn_80093274();
    lbl_80282458 = fn_8016C198()->a;
    lbl_8028245C = fn_8016C18C()->a;
    if (pArc->n2 != -1) {
        pEntry = lbl_80281F1C->pFile->p8->apTables[pArc->n2]->apEntries[pArc->n0];
        pName = pEntry->szC;    // fake match: EA takes the name's address before the flag tests
        if (pEntry->u0 & 1) {
            if (gSession.nGameType == 3) {
                pBank = lbl_801A26DC[pArc->n0 + pArc->u8];
                fn_8005CC64(pBank, fn_800922A0(pBank));
            } else {
                nBank = fn_8008FFF0(pName);
                fn_8005CC64(lbl_80281F1C->p8->ap4[nBank], pEntry->p4);
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
    fInnerX = fInnerY = 0.0f;
    vScale[0] = (1.0f / 511.0f) * pColour->w40.a[0];
    vScale[1] = (1.0f / 511.0f) * pColour->w40.a[1];
    vScale[2] = (1.0f / 511.0f) * pColour->w40.a[2];
    vScale[3] = (1.0f / 511.0f) * pColour->w40.a[3];
    vAdd[0] = pAdd->f50[0];
    vAdd[1] = pAdd->f50[1];
    vAdd[2] = pAdd->f50[2];
    vAdd[3] = pAdd->f5C;

    // the texture turned by nQuarterTurns: each corner's (u, v) offset
    nHi = (pArc->nQuarterTurns >> 1) & 1;
    nLo = (pArc->nQuarterTurns & 1) ^ nHi;
    fU = pArc->v18[0];
    fV = pArc->v18[1];
    fU0 = nHi + fU;
    fV0 = fV + !nLo;
    fU1 = fU + nLo;
    fV1 = nHi + fV;
    // fake match: EA computes corner 3 before corner 2.
    fU3 = fU + !nLo;
    fV3 = !nHi + fV;
    fU2 = !nHi + fU;
    fV2 = fV + nLo;

    fStart = PI * ((pArc->uFlags & 0x10) ? 0.0f : pArc->fStart) / 180.0f;
    fEnd = PI * ((pArc->uFlags & 0x10) ? 360.0f : pArc->fEnd) / 180.0f;
    colorA = pArc->colorA;
    colorB = pArc->colorB;
    if (fStart > fEnd) {
        fStart -= 2.0f * PI;
    }
    fOuterX = pArc->v28[0];
    fOuterY = pArc->v28[1];
    if (pArc->uFlags & 1) {
        fInnerX = pArc->v20[0] * fOuterX;
        fInnerY = pArc->v20[1] * fOuterY;
    }
    fStep = (fEnd - fStart) / pArc->nSegments;
    // fake match: white-colour branches assign B, G, R in EA's FPR order.
    for (i = 0; i < pArc->nSegments;) {
        if (i == 0) {
            if (pArc->uFlags & 0x20) {
                fS = (f32)(pArc->nSegments - i) / pArc->nSegments;
                fT = (f32)i / pArc->nSegments;
                // fake match: keep EA's multiply-then-add colour interpolation.
                fA = colorA.a * fS;
                fA += colorB.a * fT;
                if (!(pArc->uFlags & 4) || (pArc->uFlags & 2)) {
                    fR = colorA.r * fS;
                    fR += colorB.r * fT;
                    fG = colorA.g * fS;
                    fG += colorB.g * fT;
                    fB = colorA.b * fS;
                    fB += colorB.b * fT;
                } else {
                    fB = fG = fR = 255.0f;
                }
            } else {
                fA = colorA.a;
                if (!(pArc->uFlags & 4) || (pArc->uFlags & 2)) {
                    fR = colorA.r;
                    fG = colorA.g;
                    fB = colorA.b;
                } else {
                    fB = fG = fR = 255.0f;
                }
            }
        }
        if (i == 0) {
            fSin = fn_800095F0(fStart);
            fCos = fn_80009638(fStart);
        }
        i++;
        aVtx[0].f0 = fU0;
        aVtx[0].f4 = fV0;
        aVtx[0].f8 = fInnerX * fCos;
        aVtx[0].fC = fInnerY * fSin;
        aVtx[0].f10 = 0.0f;
        aVtx[0].au14[0] = fR;
        aVtx[0].au14[1] = fG;
        aVtx[0].au14[2] = fB;
        aVtx[0].au14[3] = fA;
        aVtx[1].f0 = fU1;
        aVtx[1].f4 = fV1;
        aVtx[1].f8 = fOuterX * fCos;
        aVtx[1].fC = fOuterY * fSin;
        aVtx[1].f10 = 0.0f;
        aVtx[1].au14[0] = fR;
        aVtx[1].au14[1] = fG;
        aVtx[1].au14[2] = fB;
        aVtx[1].au14[3] = fA;

        // the colour and angle at the segment's far edge (and the next segment's near edge)
        if (pArc->uFlags & 0x20) {
            fS = (f32)(pArc->nSegments - i) / pArc->nSegments;
            fT = (f32)i / pArc->nSegments;
            // fake match: keep EA's multiply-then-add colour interpolation.
            fA = colorA.a * fS;
            fA += colorB.a * fT;
            if (!(pArc->uFlags & 4) || (pArc->uFlags & 2)) {
                fR = colorA.r * fS;
                fR += colorB.r * fT;
                fG = colorA.g * fS;
                fG += colorB.g * fT;
                fB = colorA.b * fS;
                fB += colorB.b * fT;
            } else {
                fB = fG = fR = 255.0f;
            }
        } else {
            fA = colorA.a;
            if (!(pArc->uFlags & 4) || (pArc->uFlags & 2)) {
                fR = colorA.r;
                fG = colorA.g;
                fB = colorA.b;
            } else {
                fB = fG = fR = 255.0f;
            }
        }
        if (i == pArc->nSegments) {
            fAngle = fEnd;
        } else {
            fAngle = fStep * i + fStart;
        }
        fSin = fn_800095F0(fAngle);
        fCos = fn_80009638(fAngle);
        aVtx[2].f0 = fU2;
        aVtx[2].f4 = fV2;
        aVtx[2].f8 = fOuterX * fCos;
        aVtx[2].fC = fOuterY * fSin;
        aVtx[2].f10 = 0.0f;
        aVtx[2].au14[0] = fR;
        aVtx[2].au14[1] = fG;
        aVtx[2].au14[2] = fB;
        aVtx[2].au14[3] = fA;
        aVtx[3].f0 = fU3;
        aVtx[3].f4 = fV3;
        aVtx[3].f8 = fInnerX * fCos;
        aVtx[3].fC = fInnerY * fSin;
        aVtx[3].f10 = 0.0f;
        aVtx[3].au14[0] = fR;
        aVtx[3].au14[1] = fG;
        aVtx[3].au14[2] = fB;
        aVtx[3].au14[3] = fA;

        fn_800912F4(&aVtx[0], &aPos[0].x, aUV[0], aColour[0], vScale, vAdd);
        fn_800912F4(&aVtx[1], &aPos[1].x, aUV[1], aColour[1], vScale, vAdd);
        fn_800912F4(&aVtx[2], &aPos[2].x, aUV[2], aColour[2], vScale, vAdd);
        fn_800912F4(&aVtx[3], &aPos[3].x, aUV[3], aColour[3], vScale, vAdd);
        if (pArc->n2 != -1 && (pEntry->u0 & 2)) {
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
        for (j = 0; j < 4; j++) {
            fn_800BAD60(pMtx->m, &aPos[j], &aOut[j]);
            fProj = fDist / (fDist + aOut[j].z);
            aOut[j].x *= fProj;
            aOut[j].y *= fProj;
            aOut[j].z = fZ;
        }
        fn_80012F18(7);
        fn_80012EF8();
        if (aColour[0][3] != 0.0f || aColour[1][3] != 0.0f || aColour[2][3] != 0.0f
            || aColour[3][3] != 0.0f) {
            if (pArc->n2 == -1) {
                fn_8001644C(0xA0, &aOut[0].x, aColour[0], NULL, 4);
            } else {
                fn_8001644C(0xA0, &aOut[0].x, aColour[0], aUV[0], 4);
            }
        }
    }
}

// The element's messages: -1, -3 pass it on, -2 draws it, the others set its fields.
void fn_80103684(UIArc* pArc, int nMsg, s32 n, MsgArg* pArgs) {
    switch (nMsg) {
    case -1:
        fn_800913EC(pArc->n2, pArc->n0, pArc->u8, 0, 0);
        return;
    case -2:
        fn_80102AC8(pArc, 0, 0);
        return;
    case -3:
        fn_80091460(pArc->n2, pArc->n0, pArc->u8, 0, 0);
        return;
    case 1:
        pArc->v20[0] = pArgs[0].f;
        pArc->v20[1] = pArgs[1].f;
        return;
    case 2:
        pArc->v28[0] = pArgs[0].f;
        pArc->v28[1] = pArgs[1].f;
        return;
    case 3:
        pArc->fStart = pArgs[0].f;
        return;
    case 4:
        pArc->fEnd = pArgs[0].f;
        return;
    case 5:
        pArc->v18[0] = pArgs[0].f;
        pArc->v18[1] = pArgs[1].f;
        return;
    case 6:
        pArc->colorA.r = pArgs[0].i;
        pArc->colorA.g = pArgs[1].i;
        pArc->colorA.b = pArgs[2].i;
        pArc->colorA.a = pArgs[3].i;
        return;
    case 7:
        pArc->colorB.r = pArgs[0].i;
        pArc->colorB.g = pArgs[1].i;
        pArc->colorB.b = pArgs[2].i;
        pArc->colorB.a = pArgs[3].i;
        return;
    case 8:
        pArc->nSegments = pArgs[0].i;
        return;
    case 11:
        pArc->nQuarterTurns = (pArgs[0].i / 90) % 4;
        return;
    case 12:                    // taken, and ignored
    case 13:
    case 14:
        return;
    }
}
