// fe_movies.c (TW06's file name, a guess from the filemap): the front end's movies, the loading
// screen's tiles and the quads the front end draws.

#include "game_types.h"
#include "llpict.h"
#include "frontend/fe.h"
#include "game/frontend.h"
#include "llvideo.h"
#include "core/startup.h"
#include "frontend/uisvec.h"
#include "unsorted/cull.h"

u32 gUITxf2BankMarkOnExit[FE_NUM_801D8890];
FE801D8890 gUITxf2BankState[FE_NUM_801D8890];
FE801D8858 gUILoadingScreen;
f32 gUILoadingBarTilePos[8][2];

f32* gpUIPolyColourAdd;
f32* gpUIPolyColourMul;
struct TexEntry* gpUILoadingBarTexture;
struct TexBank*  gpUILoadingBarBank;

void fn_80008380(void);
void LLMath_Add(f32* pA, f32* pB, f32* pOut);
void UI_DrawFullScreenPicture(LLPict* pPict, f32 fAlpha);    // draws the picture at that alpha
void UI_ShowPictureFadingIn(LLPict* pPict, int nFrames, f32 fStep);
s32  RC_GetCurrentFrameBuffer(void);                 // ViewController.c
void fn_800760D8(LLPict* pPict);        // LLVideo.c
void fn_800760F4(f32* pUV, LLPict* pPict);  // LLVideo.c
void fn_80016978(f32 x0, f32 y0, f32 x1, f32 y1);
void LLMath_MultiplyVec(f32* pA, f32* pB, f32* pOut);     // pOut = pA * pB, element by element
void UIPoly_Draw(FEQuad* pQuad);
void UIPoly_UnpackVertex(FEVertex* pVtx, f32* pPos, f32* pUV, f32* pColour, f32* pScale, f32* pAdd);
void UI_LoadEntryPicture(s16 nTable, s16 nEntry);
void UI_ReleaseEntryPicture(s16 nTable, s16 nEntry);
f32* UITransform_GetViewParams(void);             // uiTransform.c
void UI_GetPictureUVScale(f32* pOut, LLPict* pPict);
void UI_InitForHole(void);
void fn_80006EDC();
void fn_80006FE8();
void fn_80007254();
void fn_800083A0();
void UI_DrawLoadingBarTile(int nPoint);
void UI_ShowLoadingBarTile(s32 p0);
void UI_FadeInLoadingScreen(int nFrames);
void UI_ShowLoadingScreen(void);
void UI_OnFrontEndStart(void);
void UI_RestoreAfterMovie(void);
f32 UI_GetDrawDepth(void);
void fn_80013E30();
void UI_SetCurrentRenderCtxFrameBuffer(s32 p0);

u8 gbUIFirstMenuDraw = 1;
f32 gFELockedGolferShade = 0.25f;
int gUILoadingBarBankSlot = -1;

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x80283B90), before the unsigned conversion constant UIPoly_TintVertex uses first; its body is
// unknown.
static f32 fe_movies_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Copy pSrc to pDst with its colour tinted to gpUIPolyColourMul * (colour + gpUIPolyColourAdd);
// without bTint the colour is white and only the alpha is tinted.
void UIPoly_TintVertex(FEVertex* pSrc, FEVertex* pDst, u8 bTint) {
    memcpy(pDst, pSrc, sizeof(FEVertex));
    if (bTint) {
        pDst->au14[0] = gpUIPolyColourMul[0] * (pSrc->au14[0] + gpUIPolyColourAdd[0]);
        pDst->au14[1] = gpUIPolyColourMul[1] * (pSrc->au14[1] + gpUIPolyColourAdd[1]);
        pDst->au14[2] = gpUIPolyColourMul[2] * (pSrc->au14[2] + gpUIPolyColourAdd[2]);
        pDst->au14[3] = gpUIPolyColourMul[3] * (pSrc->au14[3] + gpUIPolyColourAdd[3]);
    } else {
        pDst->au14[0] = 0xFF;
        pDst->au14[1] = 0xFF;
        pDst->au14[2] = 0xFF;
        pDst->au14[3] = gpUIPolyColourMul[3] * (pSrc->au14[3] + gpUIPolyColourAdd[3]);
    }
}

// Draw pQuad, a polygon element of the menu UI. With a UI file entry (n2 its table, n0 its entry;
// n2 -1: none) it is textured: a texture entry (flag 1) binds its texture (in the menus, game type
// 3, the first texture of 'txf2' bank n0; else the entry's texture in the bank its name picks,
// fn_8008FFF0) and keeps its corner colours only with n8 bit 0 (else they are white, alpha kept); a
// picture entry (flag 2) binds its decoded picture, and the texture coordinates are scaled to the
// part of the texture the picture fills. Colour-table colour n4 (not -1) replaces the corners'
// colours first. The colours are tinted by the UI studio's multiply and add, scaled by the
// transform's colour level and offset; the corners go through the current UI transform, a
// perspective divide by the view distance, and are put at the front end's draw depth. Nothing is
// drawn when all four corners are transparent.
void UIPoly_Draw(FEQuad* pQuad) {
    FEVertex aVtx[4];
    Vec4 aPos[4];
    Vec4 aOut[4];
    f32 aUV[4][4];
    f32 aColour[4][4];
    f32 vScale[4];
    f32 vAdd[4];
    f32 vPictUV[4];     // UI_GetPictureUVScale fills all four, two are used; the original's buffer is 16 bytes
    f32 fDist;
    f32 fZ;
    f32 fProj;
    UIFileEntry* pEntry;
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

    fDist = UITransform_GetViewParams()[2];
    fZ = UI_GetDrawDepth();
    pMtx = UITransform_GetCurrent();
    pColour = UITransform_GetCurrent();
    pAdd = UITransform_GetCurrent();
    gpUIPolyColourMul = &UISGetColorMultipler()->r;
    gpUIPolyColourAdd = &UISGetColorAdditive()->r;
    bTint = 1;
    if (pQuad->n2 != -1) {
        pEntry = gpFrontEnd->pFile->p8->apTables[pQuad->n2]->apEntries[pQuad->n0];
        szName = pEntry->szC;
        if (pEntry->u0 & 1) {
            if (gSession.nGameType == 3) {
                pBank = lbl_801A26DC[pQuad->n0];
                pTex = UI_GetTexBankFirstTexture(pBank);
                RenderState_SetBankTexture(pBank, pTex);
            } else {
                // the texture bank by the entry's name (UI_GetTextureBankIndex: -1, 0 or 1)
                nBank = UI_GetTextureBankIndex(szName);
                RenderState_SetBankTexture(gpFrontEnd->p8->ap4[nBank], pEntry->p4);
            }
            if (!(pQuad->n8 & 1)) {
                bTint = 0;
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
    vScale[0] = (1.0f / 511.0f) * pColour->w40.a[0];
    vScale[1] = (1.0f / 511.0f) * pColour->w40.a[1];
    vScale[2] = (1.0f / 511.0f) * pColour->w40.a[2];
    vScale[3] = (1.0f / 511.0f) * pColour->w40.a[3];
    vAdd[0] = pAdd->f50[0];
    vAdd[1] = pAdd->f50[1];
    vAdd[2] = pAdd->f50[2];
    vAdd[3] = pAdd->f5C;
    nColour = pQuad->n4;
    if (gpFrontEnd->p14 != NULL && nColour < (s16)gpFrontEnd->p14->nCount && nColour != -1) {
        // the table's colours are alpha, blue, green, red
        pQuad->aVtx[0].au14[3] = gpFrontEnd->p14->apEntries[nColour]->p8[0];
        pQuad->aVtx[1].au14[3] = gpFrontEnd->p14->apEntries[nColour]->p8[0];
        pQuad->aVtx[2].au14[3] = gpFrontEnd->p14->apEntries[nColour]->p8[0];
        pQuad->aVtx[3].au14[3] = gpFrontEnd->p14->apEntries[nColour]->p8[0];
        pQuad->aVtx[0].au14[2] = gpFrontEnd->p14->apEntries[nColour]->p8[1];
        pQuad->aVtx[1].au14[2] = gpFrontEnd->p14->apEntries[nColour]->p8[1];
        pQuad->aVtx[2].au14[2] = gpFrontEnd->p14->apEntries[nColour]->p8[1];
        pQuad->aVtx[3].au14[2] = gpFrontEnd->p14->apEntries[nColour]->p8[1];
        pQuad->aVtx[0].au14[1] = gpFrontEnd->p14->apEntries[nColour]->p8[2];
        pQuad->aVtx[1].au14[1] = gpFrontEnd->p14->apEntries[nColour]->p8[2];
        pQuad->aVtx[2].au14[1] = gpFrontEnd->p14->apEntries[nColour]->p8[2];
        pQuad->aVtx[3].au14[1] = gpFrontEnd->p14->apEntries[nColour]->p8[2];
        pQuad->aVtx[0].au14[0] = gpFrontEnd->p14->apEntries[nColour]->p8[3];
        pQuad->aVtx[1].au14[0] = gpFrontEnd->p14->apEntries[nColour]->p8[3];
        pQuad->aVtx[2].au14[0] = gpFrontEnd->p14->apEntries[nColour]->p8[3];
        pQuad->aVtx[3].au14[0] = gpFrontEnd->p14->apEntries[nColour]->p8[3];
    }
    UIPoly_TintVertex(&pQuad->aVtx[0], &aVtx[0], bTint);
    UIPoly_TintVertex(&pQuad->aVtx[1], &aVtx[1], bTint);
    UIPoly_TintVertex(&pQuad->aVtx[2], &aVtx[2], bTint);
    UIPoly_TintVertex(&pQuad->aVtx[3], &aVtx[3], bTint);
    UIPoly_UnpackVertex(&aVtx[0], &aPos[0].x, aUV[0], aColour[0], vScale, vAdd);
    UIPoly_UnpackVertex(&aVtx[1], &aPos[1].x, aUV[1], aColour[1], vScale, vAdd);
    UIPoly_UnpackVertex(&aVtx[2], &aPos[2].x, aUV[2], aColour[2], vScale, vAdd);
    UIPoly_UnpackVertex(&aVtx[3], &aPos[3].x, aUV[3], aColour[3], vScale, vAdd);
    if (pQuad->n2 != -1 && (pEntry->u0 & 2)) {
        // a picture fills only part of its texture
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
    for (i = 0; i < 4; i++) {
        LLMath_mat44fltMultiply(pMtx->m, &aPos[i], &aOut[i]);
        fProj = fDist / (fDist + aOut[i].z);
        aOut[i].x *= fProj;
        aOut[i].y *= fProj;
        aOut[i].z = fZ;
    }
    DS_vEnableZBufferUpdate(0);
    DS_vSetZBufferMode(7);
    RenderState_Flush();
    if (aColour[0][3] != 0.0f || aColour[1][3] != 0.0f || aColour[2][3] != 0.0f
        || aColour[3][3] != 0.0f) {
        if (pQuad->n2 == -1) {
            RenderView_DrawPrimitive(0xA0, &aOut[0].x, aColour[0], NULL, 4);
        } else {
            RenderView_DrawPrimitive(0xA0, &aOut[0].x, aColour[0], aUV[0], 4);
        }
    }
}

// Unpack pVtx into four-float arrays: its position (w 1), its texture coordinates (then 0, 1) and
// its colour, which is then scaled by pScale and offset by pAdd.
void UIPoly_UnpackVertex(FEVertex* pVtx, f32* pPos, f32* pUV, f32* pColour, f32* pScale, f32* pAdd) {
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
    LLMath_MultiplyVec(pColour, pScale, pColour);
    LLMath_Add(pColour, pAdd, pColour);
}

// When a screen loads (message -1 of the polygon and arc elements): if UI file entry (nTable,
// nEntry) is a picture entry (flag 2 set, flag 1 clear), decode its picture (UI_DecodeEntryPicture:
// entry nEntry of the picture table). nTable -1: no entry, nothing to do.
void UI_LoadEntryPicture(s16 nTable, s16 nEntry) {
    u32 uFlags;

    if (nTable == -1) return;
    uFlags = gpFrontEnd->pFile->p8->apTables[nTable]->apEntries[nEntry]->u0;
    if (!(uFlags & 1) && (uFlags & 2)) {
        UI_DecodeEntryPicture(nEntry);
    }
}

// Clear gbUIFirstMenuDraw (uiProcessInterface.c calls it on the first menu draw and at the menus'
// shutdown; nothing else happens there in this build).
void UI_ClearFirstMenuDraw(void) {
    gbUIFirstMenuDraw = 0;
}

// When a screen unloads (message -3 of the polygon and arc elements): if UI file entry (nTable,
// nEntry) is a picture entry (flag 2 set, flag 1 clear), wait for the GPU (fn_80008380) and mark
// its picture to be freed (UI_MarkEntryPictureForFree; UI_FreeMarkedEntryPictures frees it on the
// next frame).
void UI_ReleaseEntryPicture(s16 nTable, s16 nEntry) {
    u32 uFlags;

    if (nTable == -1) return;
    uFlags = gpFrontEnd->pFile->p8->apTables[nTable]->apEntries[nEntry]->u0;
    if (!(uFlags & 1) && (uFlags & 2)) {
        fn_80008380();
        UI_MarkEntryPictureForFree(nEntry);
    }
}

// The polygon element's messages (the UI studio's plugin 0): -1 (its screen loads) decodes its
// picture, -2 draws it, -3 (its screen unloads) marks the picture to be freed; 0 sets corner
// pArgs[0]'s position (pArgs[1..3]), 1 its texture coordinates (pArgs[1..2]); 2 sets the colour
// (pArgs[1..3], red, green, blue) and 3 the alpha (pArgs[1]) of corner pArgs[0], or of all four for
// -1; 5 sets the UI file entry, unless pArgs[0] is -1: with bSplit 1 from one number (low 16 bits
// the entry, high 16 the table), else table pArgs[0], entry pArgs[1]; 6 is taken and ignored.
void UIPoly_ProcessMessage(FEQuad* pQuad, int nMsg, u32 bSplit, FEMsgArg* pArgs) {
    s16 nOld;

    switch (nMsg) {
    case -1:
        // port: EA passes arguments UI_LoadEntryPicture ignores
        ((void (*)(s16, s16, s16, int, int))UI_LoadEntryPicture)(pQuad->n2, pQuad->n0, pQuad->nA, 0, 0);
        break;
    case -2:
        // port: EA passes arguments UIPoly_Draw ignores
        ((void (*)(FEQuad*, int, int))UIPoly_Draw)(pQuad, 0, 0);
        break;
    case -3:
        // port: EA passes arguments UI_ReleaseEntryPicture ignores
        ((void (*)(s16, s16, s16, int, int))UI_ReleaseEntryPicture)(pQuad->n2, pQuad->n0, pQuad->nA, 0, 0);
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

// Run with the other set-ups before each hole (Code8006F438.c fn_8006F518); empty in this build.
void UI_InitForHole(void) {
}

// fake match: stands in for a function the original linker stripped. The pool has 512.0f and
// 448.0f (0x80283BA8, the screen size UI_DrawFullScreenPicture uses) before UI_InitLoadingBarTilePos's
// constants; its body is unknown.
static f32 fe_movies_StrippedFn2(f32 x) {
    return x + 448.0f + 512.0f;
}

// The loading bar's eight tile positions (gUILoadingBarTilePos): across the screen an eighth apart,
// all at height 0.839 (UI_DrawLoadingBarTile draws at them).
void UI_InitLoadingBarTilePos(void) {
    gUILoadingBarTilePos[0][0] = 0.0f;
    gUILoadingBarTilePos[0][1] = 0.839f;
    gUILoadingBarTilePos[1][0] = 0.125f;
    gUILoadingBarTilePos[1][1] = 0.839f;
    gUILoadingBarTilePos[2][0] = 0.25f;
    gUILoadingBarTilePos[2][1] = 0.839f;
    gUILoadingBarTilePos[3][0] = 0.375f;
    gUILoadingBarTilePos[3][1] = 0.839f;
    gUILoadingBarTilePos[4][0] = 0.5f;
    gUILoadingBarTilePos[4][1] = 0.839f;
    gUILoadingBarTilePos[5][0] = 0.625f;
    gUILoadingBarTilePos[5][1] = 0.839f;
    gUILoadingBarTilePos[6][0] = 0.75f;
    gUILoadingBarTilePos[6][1] = 0.839f;
    gUILoadingBarTilePos[7][0] = 0.875f;
    gUILoadingBarTilePos[7][1] = 0.839f;
}

// Load the loading bar's texture bank from LoadData.c's copy of the 'txf2' object with id 10000 and
// take its first texture, unless the session has flag 4.
void UI_LoadLoadingBarTexture(void) {
    if (gSession.uFlags & 4) return;
    gUILoadingBarBankSlot = fn_800107C0(lbl_80281C0C, NULL, 0);
    gpUILoadingBarBank = fn_800106C4(gUILoadingBarBankSlot);
    gpUILoadingBarTexture = UI_GetTexBankFirstTexture(gpUILoadingBarBank);
}

// Decode the loading screen's picture from the 'load' object's data into gUILoadingScreen.p30,
// unless one is there.
void UI_DecodeLoadingPicture(void) {
    if (gUILoadingScreen.p30 == NULL) {
        gUILoadingScreen.p30 = fn_8002FD00(lbl_80281C04, lbl_801A25F0.uSize);
    }
}

// Free the loading screen's picture UI_DecodeLoadingPicture decoded, unless the session has flag 4.
void UI_FreeLoadingPicture(void) {
    if (!(gSession.uFlags & 4) && gUILoadingScreen.p30 != NULL) {
        fn_8002FE70(gUILoadingScreen.p30);
        fn_8002FEAC();
        gUILoadingScreen.p30 = NULL;
    }
}

// Free the loading bar's texture bank UI_LoadLoadingBarTexture loaded, unless the session has flag
// 4.
void UI_FreeLoadingBarTexture(void) {
    if (gSession.uFlags & 4) return;
    fn_80010544(gUILoadingBarBankSlot);
}

// Set the loading screen up (gUILoadingScreen), unless the session has flag 4 or it is running
// already (b18): the clock now (TI_sRead), no bar tile shown yet (n1C -1), no time passed; n14 the
// number of players when a round is being set up (game type 4), else 0; the seconds per bar tile
// (fC, and the first tile's time f4) (4.83 * n14 + 3.1) / 8. Then decode the loading picture
// (UI_DecodeLoadingPicture).
void UI_InitLoadingBar(void) {
    if (!(gSession.uFlags & 4) && !gUILoadingScreen.b18) {
        gUILoadingScreen.b18 = 1;
        gUILoadingScreen.n1C = -1;
        gUILoadingScreen.f10 = 0.0f;
        gUILoadingScreen.u20 = TI_sRead();
        gUILoadingScreen.n0 = 0;
        if (gSession.nGameType == 4) {
            gUILoadingScreen.n14 = gSession.nNumPlayers;
        } else {
            gUILoadingScreen.n14 = 0;
        }
        gUILoadingScreen.fC = gUILoadingScreen.f4 = (4.83f * gUILoadingScreen.n14 + 3.1f) / 8.0f;
        gUILoadingScreen.f8 = 0.0f;
        UI_DecodeLoadingPicture();
    }
}

// One step of the loading screen while something streams in (streammanagerhole.c calls it between
// stream updates), unless the session has flag 4. The seconds since the last step are added to f10.
// Until f10 passes f4 the screen is redrawn only every 2 seconds (f8); a redraw draws the loading
// picture over the whole screen and the bar tiles shown so far (0 to n1C, at most 8) in one frame.
// When f10 passes f4 (or no tile is shown yet) the next tile is shown (UI_ShowLoadingBarTile) and
// f4 moves on by fC. nMode 1 ends it: every tile not yet shown is shown, a frame each, and b18 is
// cleared.
void UI_DrawLoadingScreenAndProgressBar(int nMode) {
    f32 fSecs;
    int i;

    if (gSession.uFlags & 4) return;
    if (nMode == 1) {
        gUILoadingScreen.b18 = 0;
    }
    gUILoadingScreen.u28 = TI_sRead();
    fSecs = fn_8006E118(gUILoadingScreen.u28, gUILoadingScreen.u20);
    gUILoadingScreen.u20 = gUILoadingScreen.u28;
    gUILoadingScreen.f10 += fabsf(fSecs);
    if (nMode != 1 && gUILoadingScreen.f10 <= gUILoadingScreen.f4) {
        if (gUILoadingScreen.f10 > gUILoadingScreen.f8) {
            gUILoadingScreen.f8 += 2.0f;
        } else {
            return;
        }
    }
    fn_80006EDC();
    UI_DrawFullScreenPicture(gUILoadingScreen.p30, 1.0f);
    if (gUILoadingScreen.n1C >= 0) {
        for (i = 0; i <= gUILoadingScreen.n1C; i++) {
            if (i >= 8) break;
            UI_DrawLoadingBarTile(i);
        }
    }
    fn_80006FE8();
    fn_800083A0();
    fn_80007254();
    Gaud_Cycle();
    fn_80008380();
    if (gUILoadingScreen.f10 > gUILoadingScreen.f4 || gUILoadingScreen.n1C == -1) {
        if (gUILoadingScreen.f10 > gUILoadingScreen.f4) {
            gUILoadingScreen.f4 += gUILoadingScreen.fC;
        }
        if (++gUILoadingScreen.n1C >= 8) return;
        UI_ShowLoadingBarTile(gUILoadingScreen.n1C);
    }
    if (nMode == 1) {
        if (gUILoadingScreen.n1C < 0) {
            gUILoadingScreen.n1C = 0;
        }
        if (gUILoadingScreen.n1C > 7) {
            gUILoadingScreen.n1C = 7;
        }
        for (i = gUILoadingScreen.n1C + 1; i < 8; i++) {
            UI_ShowLoadingBarTile(i);
        }
        gUILoadingScreen.b18 = 0;
    }
}

// Draw loading-bar tile nTile in a frame of its own (start the frame, draw the tile, end the frame,
// step the audio).
void UI_ShowLoadingBarTile(s32 p0) {
    fn_80006EDC();
    UI_DrawLoadingBarTile(p0);
    fn_80006FE8();
    fn_80007254();
    fn_800083A0();
    Gaud_Cycle();
}

// Draw tile nPoint of the loading bar at point nPoint of gUILoadingBarTilePos, an eighth of the
// screen wide and 0.142 high, from the loading bar's texture: tiles 0-3 come from its top half, 4-7
// from its bottom half (their u runs past 1 and wraps).
void UI_DrawLoadingBarTile(int nPoint) {
    f32 afColour[4];
    f32 afXY[8];
    f32 afUV[8];

    fn_80016978(0.0f, 0.0f, 1.0f, 1.0f);
    RenderState_SetBlendFactors(4, 5);
    DS_vSetAlphaTestMode(0, 6, 0x80);
    DS_vSetZBufferMode(7);
    RenderView_SetUseCurrentMatrices(0);
    DS_vEnableZBufferUpdate(0);
    RenderState_SetBankTexture(gpUILoadingBarBank, gpUILoadingBarTexture);
    RenderState_SetDrawFlags(0x50);
    RenderState_Flush();
    afColour[0] = 0.5f;
    afColour[3] = 0.25f;        // EA code: overwritten at once
    afColour[1] = 0.5f;
    afColour[2] = 0.5f;
    afColour[3] = 0.5f;
    RenderView_SetColor(afColour);
    RenderView_MakeQuad(afXY, NULL, gUILoadingBarTilePos[nPoint][0], gUILoadingBarTilePos[nPoint][1],
                gUILoadingBarTilePos[nPoint][0] + 0.125f, gUILoadingBarTilePos[nPoint][1] + 0.142f);
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
    RenderView_DrawPrimitive(0xA1, afXY, 0, afUV, 2);
}

// Show the loading screen's picture for 30 frames, fading in (UI_FadeInLoadingScreen), then unbind
// the texture. GoEntry.c calls it as the menus hand over to a round and after each hole (not in the
// demo).
void UI_ShowLoadingScreen(void) {
    UI_FadeInLoadingScreen(30);
    RenderState_SetBankTexture(0, 0);
    RenderState_Flush();
}

// Decode the loading screen's picture from the 'load' object's data, show it for nFrames frames
// fading in over 30 (UI_ShowPictureFadingIn), wait for the GPU (fn_80008380), then free it.
void UI_FadeInLoadingScreen(int nFrames) {
    LLPict* pPict;

    pPict = fn_8002FD00(lbl_80281C04, lbl_801A25F0.uSize);
    UI_ShowPictureFadingIn(pPict, nFrames, 1.0f / 30.0f);
    fn_80008380();
    fn_8002FE70(pPict);
    fn_8002FEAC();
}

// The demo's screen between holes (GoEntry.c calls it in the demo where it otherwise calls
// UI_ShowLoadingScreen): the picture of entry 0 of the UI file's picture table, shown for 30 frames
// fading in, then freed. The 600 calls to fn_80007254 after it do nothing (it is empty in this
// build). Then the texture is unbound.
void UI_ShowDemoLoadingScreen(void) {
    int i = 0;
    LLPict* pPict;
    UIFileEntry* pEntry;
    UIMovieData* pData;

    pEntry = gpFrontEnd->pFile->p8->apTables[gUIState.n3C]->apEntries[0];
    pData = pEntry->p4;
    pEntry->p8 = (u8*)fn_8002FD00(pData->aData, pData->uSize);
    pPict = (LLPict*)pEntry->p8;
    fn_800760D8(pPict);
    UI_ShowPictureFadingIn(pPict, 30, 1.0f / 30.0f);
    fn_80008380();
    fn_8002FE70((LLPict*)pEntry->p8);
    pEntry->p8 = NULL;
    fn_8002FEAC();
    for (; i < 600; i++) {
        fn_80007254();
    }
    RenderState_SetBankTexture(0, 0);
    RenderState_Flush();
}

// Called by GoEntry.c each time the menus start (game type 3), before the intro movie; empty in
// this build.
void UI_OnFrontEndStart(void) {
}

// The start-up movies, as the start-up UI (game type 1, nC 0) shuts down (uiProcessInterface.c
// fn_80090400): "eas", then, unless the session has flag 0x4000, one of the two cameo movies
// "tigcam01" / "tigcam02" at random (any button skips it); then the first 'LEGL' picture startUp.c
// kept, shown for 180 frames (fading in over 30) and freed.
void UI_PlayStartUpMovies(void) {
    char szPath[0x40];          // the size is unknown: the frame leaves 0x40 bytes for it
    char szName[0x40];          // the size is unknown: the frame leaves 0x40 bytes for it
    LLPict* pPict;

    FE_MakeMoviePath("eas", szPath);
    LLVideo_PlayFile(szPath, NULL, 0, 0);
    if (!(gSession.uFlags & 0x4000)) {
        sprintf(szName, "tigcam%02d", (s16)((Misc_RandFunc(0) & 1) + 1));
        FE_MakeCameoMoviePath(szName, szPath);
        LLVideo_PlayFile(szPath, FE_IsMovieSkipPressed, 0, 0);
    }
    pPict = fn_8002FD00(lbl_80282134, lbl_8028212C);
    UI_ShowPictureFadingIn(pPict, 180, 1.0f / 30.0f);
    fn_80008380();
    fn_8002FE70(pPict);
    StaticMem_Free(lbl_80282134);
    lbl_80282134 = NULL;
}

// Show pPict over the whole screen for nFrames frames, a frame each (with the audio stepped and the
// disc checked, fn_800B7490), its alpha rising by fStep a frame from 0 up to 1.
void UI_ShowPictureFadingIn(LLPict* pPict, int nFrames, f32 fStep) {
    f32 fAlpha = 0.0f;
    int i;

    for (i = 0; i < nFrames; i++) {
        fn_80006EDC();
        fAlpha += fStep;
        if (fAlpha > 1.0f) {
            fAlpha = 1.0f;
        }
        UI_DrawFullScreenPicture(pPict, fAlpha);
        fn_80006FE8();
        fn_80007254();
        fn_800083A0();
        Gaud_Cycle();
        fn_800B7490();
    }
}

// Draw pPict over the whole 512 x 448 screen at alpha fAlpha (colour 0.5, the renderer's full
// strength, alpha 0.5 * fAlpha), through a frame buffer of its own, then put the previous frame
// buffer back.
void UI_DrawFullScreenPicture(LLPict* pPict, f32 fAlpha) {
    f32 afColour[4];
    f32 afXY[8];
    f32 afUV[8];
    GoFrameBuf frameBuf;
    s32 nOld;

    fn_800760D8(pPict);
    RenderState_SetBlendFactors(4, 5);
    RenderState_SetDrawFlags(0x50);
    DS_vSetAlphaTestMode(0, 6, 0x80);
    DS_vSetZBufferMode(7);
    nOld = RC_GetCurrentFrameBuffer();
    FB_vSetFrameBuffer(&frameBuf, 0.0f, 0.0f, 512.0f, 448.0f, 1.0f, 1.0f);
    // port: the render slot is typed s32 but holds a pointer
    UI_SetCurrentRenderCtxFrameBuffer((s32)&frameBuf);
    RenderState_SetViewport(RC_spGetCurrentRenderCtx());
    RenderState_Flush();
    afColour[0] = 0.5f;
    afColour[1] = 0.5f;
    afColour[2] = 0.5f;
    afColour[3] = 0.5f * fAlpha;
    RenderView_MakeQuad(NULL, afXY, 0.0f, 0.0f, 1.0f, 1.0f);
    fn_800760F4(afUV, pPict);
    RenderView_SetColor(afColour);
    RenderView_DrawPrimitive(0xA1, afXY, 0, afUV, 2);
    UI_SetCurrentRenderCtxFrameBuffer(nOld);
}

// Before a movie (FE_Manager.c fn_800772E0): free the pixel data (fn_8000FFAC) of each 'txf2'
// texture bank whose gUITxf2BankState n4 is positive, waiting for the GPU first. Nothing in this
// build makes n4 positive (FE_Manager.c only clears it), so nothing is freed.
void UI_FreeTxf2BankPixels(void) {
    int i;

    for (i = 0; i < FE_NUM_801D8890; i++) {
        if (gUITxf2BankState[i].n4 > 0) {
            fn_80008380();
            fn_8000FFAC(lbl_801A26DC[i]);
        }
    }
}

// Called after a movie (FE_Manager.c fn_8007731C); empty in this build.
void UI_RestoreAfterMovie(void) {
}

// The depth the menu UI's polygons and arcs are drawn at: the front end's f18 (set to 1 when a
// round starts), 0 without a front end.
f32 UI_GetDrawDepth(void) {
    if (gpFrontEnd != NULL) {
        return gpFrontEnd->f18;
    }
    return 0.0f;
}

// The texture coordinates of pPict's far corner (f6C, f70: how much of its texture the picture
// fills), then 0 and 1.
void UI_GetPictureUVScale(f32* pOut, LLPict* pPict) {
    pOut[0] = pPict->f6C;
    pOut[1] = pPict->f70;
    pOut[2] = 0.0f;
    pOut[3] = 1.0f;
}

// b + a into out (four floats)
#ifdef __MWERKS__
asm void LLMath_Add(register f32* pA, register f32* pB, register f32* pOut) {
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
void LLMath_Add(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
    pOut[3] = pB[3] + pA[3];
}
#endif

// Make p0 (a GoFrameBuf) the current render context's frame buffer (GoRenderCtx_Gc.c fn_80013E30).
// port: the frame buffer travels as an s32, and fn_80013E30 is declared here without parameters: EA
//       passes the render-context slot as a third argument it ignores.
void UI_SetCurrentRenderCtxFrameBuffer(s32 p0) {
    fn_80013E30(*(s32*)((u8*)lbl_80280DF0), p0, lbl_80280DF0);
}

TexEntry* UI_GetTexBankFirstTexture(TexBank* pBank) {
    return pBank->p8;
}
