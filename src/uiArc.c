// uiArc.c (our name): a menu UI element drawn as an arc or circle: nSegments pieces from fStart
// to fEnd degrees (0 to 360 by default), shaded from one colour to another (UIArc_Draw), and
// the message handler that sets it up (UIArc_ProcessMessage).

#include "game_types.h"
#include "llpict.h"
#include "frontend/fe.h"
#include "game/frontend.h"
#include "frontend/uisvec.h"
#include "unsorted/cull.h"
#include "camera.h"

// fe_movies.c
void UI_LoadEntryPicture(s16 n2, s16 n0, s16 n8, s32 a, s32 b);
void UI_ReleaseEntryPicture(s16 n2, s16 n0, s16 n8, s32 a, s32 b);
void UIPoly_UnpackVertex(FEVertex* pVtx, f32* pPos, f32* pUV, f32* pColour, f32* pScale, f32* pAdd);
void UI_GetPictureUVScale(f32* pOut, LLPict* pPict);
f32  UI_GetDrawDepth(void);
f32* UITransform_GetViewParams(void);                 // uiTransform.c
void fn_800760D8(LLPict* pPict);        // LLVideo.c

// The tint of the last draw (as fe_movies.c's gpUIPolyColourMul/gpUIPolyColourAdd); nothing here reads it.
// .sbss: defined in reverse address order (CodeWarrior lays it out last-defined-first).
f32* gpUIArcColourAdd;
f32* gpUIArcColourMul;

// fake match: these two stand in for code the original linker stripped. The file's pool starts
// with the u32 conversion's constant and then 1/511, before UIArc_Draw uses 0.0f first; their
// bodies are unknown, these only reproduce the order (a conversion's constant is pooled after the
// function's literal constants, hence two functions).
static f32 uiArc_StrippedFn(u32 n) {
    return n;
}

static f32 uiArc_StrippedFn2(f32 x) {
    return (1.0f / 511.0f) * x;
}

// Draw pArc, an arc element of the menu UI: nSegments quads from its inner radii (v20 times the
// outer radii with flag 1, else 0: a pie) to its outer radii v28 (across, down), from fStart to
// fEnd degrees (flag 0x10: the whole circle), textured like UIPoly_Draw's polygons (in the menus,
// game type 3, 'txf2' bank n0 + u8), the texture offset by v18 and turned by nQuarterTurns. Its
// colour is colorA (flag 0x20: shaded from colorA to colorB along the arc; with flag 4 and not 2,
// white with only the colours' alpha). Drawing stops at the first segment whose four corners are
// all transparent. a and b are unused: UIArc_ProcessMessage passes 0, 0.
void UIArc_Draw(UIArc* pArc, s32 a, s32 b) {
    FEVertex aVtx[4];
    Vec4 aPos[4];
    Vec4 aOut[4];
    f32 aUV[4][4];
    f32 aColour[4][4];
    f32 vScale[4];
    f32 vAdd[4];
    f32 vPictUV[4];     // UI_GetPictureUVScale fills two
    GXColor colorA;
    GXColor colorB;
    // fake match: scalar declaration order controls CodeWarrior's saved-FPR allocation.
    // fake match: the texture corners, colour, and far edge's sine and cosine are one-element
    // arrays: CodeWarrior turns each into a compiler temporary (numbered in order of first use),
    // which puts them after the other locals in the register allocator's order, as in the
    // original. port: plain locals.
    f32 fCos2[1];
    f64 fU0[1];         // f64: the original keeps these in 8-byte stack slots
    f32 fStep;
    f32 fStart;
    f32 fEnd;
    f64 fV0[1];
    f32 fInnerX;
    f32 fOuterX;
    f64 fU1[1];
    f32 fCos;
    f64 fV1[1];
    f32 fU;
    f32 fInnerY;
    f64 fU3[1];
    f64 fV3[1];
    f32 fOuterY;
    f32 fDist;
    f32 fSin2[1];
    f32 fProj;
    f64 fU2[1];
    f64 fV2[1];
    f32 fR[1];
    f32 fAngle;
    f32 fZ;
    f32 fG[1];
    f32 fV;
    f32 fB[1];
    f32 fA[1];
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

    fDist = UITransform_GetViewParams()[2];
    fZ = UI_GetDrawDepth();
    pMtx = UITransform_GetCurrent();
    pColour = UITransform_GetCurrent();
    pAdd = UITransform_GetCurrent();
    gpUIArcColourMul = &UISGetColorMultipler()->r;
    gpUIArcColourAdd = &UISGetColorAdditive()->r;
    if (pArc->n2 != -1) {
        pEntry = gpFrontEnd->pFile->p8->apTables[pArc->n2]->apEntries[pArc->n0];
        pName = pEntry->szC;    // fake match: EA takes the name's address before the flag tests
        if (pEntry->u0 & 1) {
            if (gSession.nGameType == 3) {
                pBank = lbl_801A26DC[pArc->n0 + pArc->u8];
                RenderState_SetBankTexture(pBank, UI_GetTexBankFirstTexture(pBank));
            } else {
                nBank = UI_GetTextureBankIndex(pName);
                RenderState_SetBankTexture(gpFrontEnd->p8->ap4[nBank], pEntry->p4);
            }
        } else if (pEntry->u0 & 2) {
            pPict = (LLPict*)pEntry->p8;
            fn_800760D8(pPict);
        }
        RenderState_SetDrawFlags(0x50);
    } else {
        RenderState_SetDrawFlags(0x40);
    }
    RenderState_SetViewport(RC_spGetCurrentRenderCtx());
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
    fU0[0] = nHi + fU;
    fV0[0] = fV + !nLo;
    fU1[0] = fU + nLo;
    fV1[0] = nHi + fV;
    // fake match: EA computes corner 3 before corner 2.
    fU3[0] = fU + !nLo;
    fV3[0] = !nHi + fV;
    fU2[0] = !nHi + fU;
    fV2[0] = fV + nLo;

    // fake match: uFlags through a dead 64-bit round trip (the low word is uFlags itself): its
    // high word's srawi takes an issue slot in the first scheduling pass, which moves fV0's add
    // after the other corners' conversions (EA's float registers); it is deleted after
    // allocation. port: plain pArc->uFlags.
    fStart = PI * (((s32)(s64)(s32)pArc->uFlags & 0x10) ? 0.0f : pArc->fStart) / 180.0f;
    fEnd = PI * (((s32)(s64)(s32)pArc->uFlags & 0x10) ? 360.0f : pArc->fEnd) / 180.0f;
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
            // fake match: dead stores, number R, G and B before A (see the declarations)
            fR[0] = 0.0f;
            fG[0] = 0.0f;
            fB[0] = 0.0f;
            if (pArc->uFlags & 0x20) {
                // fake match: keep EA's multiply-then-add colour interpolation.
                fA[0] = colorA.a * ((f32)(pArc->nSegments - i) / pArc->nSegments);
                fA[0] += colorB.a * ((f32)i / pArc->nSegments);
                if (!(pArc->uFlags & 4) || (pArc->uFlags & 2)) {
                    fR[0] = colorA.r * ((f32)(pArc->nSegments - i) / pArc->nSegments);
                    fR[0] += colorB.r * ((f32)i / pArc->nSegments);
                    fG[0] = colorA.g * ((f32)(pArc->nSegments - i) / pArc->nSegments);
                    fG[0] += colorB.g * ((f32)i / pArc->nSegments);
                    fB[0] = colorA.b * ((f32)(pArc->nSegments - i) / pArc->nSegments);
                    fB[0] += colorB.b * ((f32)i / pArc->nSegments);
                } else {
                    fB[0] = fG[0] = fR[0] = 255.0f;
                }
            } else {
                fA[0] = colorA.a;
                if (!(pArc->uFlags & 4) || (pArc->uFlags & 2)) {
                    fR[0] = colorA.r;
                    fG[0] = colorA.g;
                    fB[0] = colorA.b;
                } else {
                    fB[0] = fG[0] = fR[0] = 255.0f;
                }
            }
        }
        if (i == 0) {
            fCos2[0] = 0.0f;    // fake match: a dead store, numbers fCos2 before fSin2
            fSin2[0] = Math_Sin(fStart);
            fCos = Math_Cos(fStart);
        } else {
            fCos = fCos2[0];
        }
        i++;
        aVtx[0].f0 = fU0[0];
        aVtx[0].f4 = fV0[0];
        aVtx[0].f8 = fInnerX * fCos;
        aVtx[0].fC = fInnerY * fSin2[0];
        aVtx[0].f10 = 0.0f;
        aVtx[0].au14[0] = fR[0];
        aVtx[0].au14[1] = fG[0];
        aVtx[0].au14[2] = fB[0];
        aVtx[0].au14[3] = fA[0];
        aVtx[1].f0 = fU1[0];
        aVtx[1].f4 = fV1[0];
        aVtx[1].f8 = fOuterX * fCos;
        aVtx[1].fC = fOuterY * fSin2[0];
        aVtx[1].f10 = 0.0f;
        aVtx[1].au14[0] = fR[0];
        aVtx[1].au14[1] = fG[0];
        aVtx[1].au14[2] = fB[0];
        aVtx[1].au14[3] = fA[0];

        // the colour and angle at the segment's far edge (and the next segment's near edge)
        if (pArc->uFlags & 0x20) {
            // fake match: keep EA's multiply-then-add colour interpolation.
            fA[0] = colorA.a * ((f32)(pArc->nSegments - i) / pArc->nSegments);
            fA[0] += colorB.a * ((f32)i / pArc->nSegments);
            if (!(pArc->uFlags & 4) || (pArc->uFlags & 2)) {
                fR[0] = colorA.r * ((f32)(pArc->nSegments - i) / pArc->nSegments);
                fR[0] += colorB.r * ((f32)i / pArc->nSegments);
                fG[0] = colorA.g * ((f32)(pArc->nSegments - i) / pArc->nSegments);
                fG[0] += colorB.g * ((f32)i / pArc->nSegments);
                fB[0] = colorA.b * ((f32)(pArc->nSegments - i) / pArc->nSegments);
                fB[0] += colorB.b * ((f32)i / pArc->nSegments);
            } else {
                fB[0] = fG[0] = fR[0] = 255.0f;
            }
        } else {
            fA[0] = colorA.a;
            if (!(pArc->uFlags & 4) || (pArc->uFlags & 2)) {
                fR[0] = colorA.r;
                fG[0] = colorA.g;
                fB[0] = colorA.b;
            } else {
                fB[0] = fG[0] = fR[0] = 255.0f;
            }
        }
        if (i == pArc->nSegments) {
            fAngle = fEnd;
        } else {
            fAngle = fStep * i + fStart;
        }
        fSin2[0] = Math_Sin(fAngle);
        fCos2[0] = Math_Cos(fAngle);
        aVtx[2].f0 = fU2[0];
        aVtx[2].f4 = fV2[0];
        aVtx[2].f8 = fOuterX * fCos2[0];
        aVtx[2].fC = fOuterY * fSin2[0];
        aVtx[2].f10 = 0.0f;
        aVtx[2].au14[0] = fR[0];
        aVtx[2].au14[1] = fG[0];
        aVtx[2].au14[2] = fB[0];
        aVtx[2].au14[3] = fA[0];
        aVtx[3].f0 = fU3[0];
        aVtx[3].f4 = fV3[0];
        aVtx[3].f8 = fInnerX * fCos2[0];
        aVtx[3].fC = fInnerY * fSin2[0];
        aVtx[3].f10 = 0.0f;
        aVtx[3].au14[0] = fR[0];
        aVtx[3].au14[1] = fG[0];
        aVtx[3].au14[2] = fB[0];
        aVtx[3].au14[3] = fA[0];

        UIPoly_UnpackVertex(&aVtx[0], &aPos[0].x, aUV[0], aColour[0], vScale, vAdd);
        UIPoly_UnpackVertex(&aVtx[1], &aPos[1].x, aUV[1], aColour[1], vScale, vAdd);
        UIPoly_UnpackVertex(&aVtx[2], &aPos[2].x, aUV[2], aColour[2], vScale, vAdd);
        UIPoly_UnpackVertex(&aVtx[3], &aPos[3].x, aUV[3], aColour[3], vScale, vAdd);
        if (pArc->n2 != -1 && (pEntry->u0 & 2)) {
            // a movie's picture fills only part of its texture
            UI_GetPictureUVScale(vPictUV, pPict);
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
            LLMath_mat44fltMultiply(pMtx->m, &aPos[j], &aOut[j]);
            fProj = fDist / (fDist + aOut[j].z);
            aOut[j].x *= fProj;
            aOut[j].y *= fProj;
            aOut[j].z = fZ;
        }
        DS_vSetZBufferMode(7);
        RenderState_Flush();
        if (aColour[0][3] != 0.0f || aColour[1][3] != 0.0f || aColour[2][3] != 0.0f
            || aColour[3][3] != 0.0f) {
            if (pArc->n2 == -1) {
                RenderView_DrawPrimitive(0xA0, &aOut[0].x, aColour[0], NULL, 4);
            } else {
                RenderView_DrawPrimitive(0xA0, &aOut[0].x, aColour[0], aUV[0], 4);
            }
        } else {
            break;
        }
    }
}

// The arc element's messages (the UI studio's plugin 8): -1 (its screen loads) decodes its picture,
// -2 draws it, -3 (its screen unloads) marks the picture to be freed; 1 sets its inner radii (as
// fractions of the outer), 2 its outer radii, 3 the start and 4 the end angle (degrees), 5 the
// texture offset, 6 and 7 the two colours (red, green, blue, alpha), 8 the number of segments, 11
// the texture's turn (pArgs[0] degrees, in quarter turns); 12 to 14 are taken and ignored.
void UIArc_ProcessMessage(UIArc* pArc, int nMsg, s32 n, MsgArg* pArgs) {
    switch (nMsg) {
    case -1:
        UI_LoadEntryPicture(pArc->n2, pArc->n0, pArc->u8, 0, 0);
        return;
    case -2:
        UIArc_Draw(pArc, 0, 0);
        return;
    case -3:
        UI_ReleaseEntryPicture(pArc->n2, pArc->n0, pArc->u8, 0, 0);
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
