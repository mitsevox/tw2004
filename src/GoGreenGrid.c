// GoGreenGrid.c (EA's name, from its asserts; TW06): the grid drawn over the putting green. It is
// laid out from the target back towards the ball, the ground height under each point is sampled a
// few points a frame, and the lines are built into a mesh per view.

#include "game_types.h"
#include "game.h"
#include "golfer.h"
#include "ball.h"
#include "camera.h"
#include "physics.h"
#include "terrain.h"
#include "unsorted/cull.h"
#include "greengrid.h"

// Skin.c
void fn_80036054(void* pMesh, int n, s32* pDesc);
void fn_800360A0(void* pMesh);
void fn_800360D4(u8* pMesh);
void fn_80035294(void);
void fn_800352BC(void);
void fn_800354B4(u8* p, f32 v);         // sets the lens's f32 at 0xAC (fn_80014268 reads it)
f32  fn_80014268(u8* p);

void fn_8009CB78(f32* pA, f32* pB, f32* pOut);
void GR_BuildGridRenderData(int nView);
u8   fn_8009BD24(int nPlayer);
u8   fn_8009BD94(int nPlayer);

GreenGrid lbl_801E3068;
GreenGrid* lbl_802813C0 = &lbl_801E3068;
TexBank*  lbl_80281FAC;         // the "gridpt" texture's bank
TexEntry* lbl_80281FA8;         // and the texture

void GR_vInit(void) {
    s32 desc[2];
    int i;
    int nViews;
    int nRows;
    if (gSession.nSplitScreen) {
        nViews = 2;
        desc[0] = 0x80;
        nRows = 8;
    } else {
        nViews = 1;
        desc[0] = 0x100;
        nRows = 16;
    }
    desc[1] = 1;
    for (i = 0; i < nViews; i++) {
        fn_80036054(lbl_802813C0->aMesh[i], 0x13, desc);
        lbl_802813C0->apVert[i] = fn_80009B34(desc[0] * 12, 2, 16, "GoGreenGrid.c", 94);
        lbl_802813C0->apUV[i] = fn_80009B34(desc[0] * 8, 2, 16, "GoGreenGrid.c", 99);
        lbl_802813C0->apColor[i] = fn_80009B34(desc[0] * 4, 2, 16, "GoGreenGrid.c", 104);
        lbl_802813C0->apIndex[i] = fn_80009B34(desc[0] * 2, 2, 16, "GoGreenGrid.c", 109);
        lbl_802813C0->apHeight[i] = fn_80009B34(nRows * 16, 2, 16, "GoGreenGrid.c", 114);
        lbl_802813C0->aTarget[i][0] = 0.0f;
        lbl_802813C0->aTarget[i][1] = 0.0f;
        lbl_802813C0->aTarget[i][2] = 0.0f;
        lbl_802813C0->aTarget[i][3] = 1.0f;
        lbl_802813C0->anRows[i] = nRows;
    }
    lbl_802813C0->fCellW = 1.0f;
    lbl_802813C0->fCellD = 1.0f;
    lbl_802813C0->fFC = 1.0f;
    lbl_802813C0->f100 = 0.125f;
    lbl_802813C0->b104 = 1;
    lbl_802813C0->anColor[0] = 0x80;
    lbl_802813C0->anColor[1] = 0x80;
    lbl_802813C0->anColor[2] = 0x40;
    lbl_802813C0->anColor[3] = 0x41;
    lbl_802813C0->nCols = 4;
    fn_800102DC(fn_8000BEE4("gridpt"), &lbl_80281FAC, &lbl_80281FA8);
}

// ---- sweep code (not yet cleaned up) ----

void fn_8009B894(void);

void fn_8009B894(void) {
}

// ---- end of sweep code ----

void fn_8009B898(void) {
    int nViews = gSession.nSplitScreen ? 2 : 1;
    int i;
    for (i = 0; i < nViews; i++) {
        fn_800360A0(lbl_802813C0->aMesh[i]);
        fn_80009E70(lbl_802813C0->apVert[i]);
        fn_80009E70(lbl_802813C0->apUV[i]);
        fn_80009E70(lbl_802813C0->apColor[i]);
        fn_80009E70(lbl_802813C0->apIndex[i]);
        fn_80009E70(lbl_802813C0->apHeight[i]);
    }
}

// Lays the view's grid out for its player's target: with the putter it runs from beyond the pin
// back past the ball (at most 8 or 16 rows), otherwise it is a square of nCols x nCols points.
void fn_8009B970(int nView) {
    // fake match: an s32 (long) copy of nView, kept in its own register, for the fn_8001707C calls
    s32 nViewCopy;
    f32 vPin[4];
    f32 fNegZ;
    f32 fDirX;
    f32 fAcross;
    f32 fAlong;
    f32 fDist;
    f32 fLen;
    nViewCopy = nView;
    if (!fn_8009BD24(fn_8001707C(nViewCopy))) {
        return;
    }
    Vec_Copy(PLAYER(fn_8001707C(nViewCopy))->vTarget, lbl_802813C0->aTarget[nView]);
    lbl_802813C0->anDone[nView] = 0;
    fn_8009CB78(PLAYER(fn_8001707C(nViewCopy))->vTarget, PLAYER(fn_8001707C(nViewCopy))->ball.vPos,
                lbl_802813C0->aDir[nView]);
    lbl_802813C0->aDir[nView][1] = 0.0f;
    fDist = (f32)fn_80009680(fn_80009744(lbl_802813C0->aDir[nView]));
    fn_800BAF04(lbl_802813C0->aDir[nView], lbl_802813C0->aDir[nView]);
    if (PLAYER(fn_8001707C(nViewCopy))->nClub == CLUB_PUTTER_e) {
        fn_8009CB78(&fn_8000C594()->pin[Game_CurrentPinSet()].x, PLAYER(fn_8001707C(nViewCopy))->ball.vPos,
                    vPin);
        vPin[1] = 0.0f;
        fLen = 2.0f + (f32)fn_80009680(fn_80009744(vPin));
        lbl_802813C0->anRows[nView] = 2.0f * fLen;
        if (gSession.nSplitScreen && lbl_802813C0->anRows[nView] > 8) {
            lbl_802813C0->anRows[nView] = 8;
        } else if (!gSession.nSplitScreen && lbl_802813C0->anRows[nView] > 16) {
            lbl_802813C0->anRows[nView] = 16;
        }
        lbl_802813C0->fCellD = fLen / (lbl_802813C0->anRows[nView] - 1);
        fAcross = (lbl_802813C0->nCols / 2) * lbl_802813C0->fCellW - lbl_802813C0->fCellW / 2.0f;
        fAlong = lbl_802813C0->fCellD / 2.0f + fDist;
    } else {
        lbl_802813C0->anRows[nView] = lbl_802813C0->nCols;
        lbl_802813C0->fCellD = lbl_802813C0->fCellW;
        fAcross = (lbl_802813C0->nCols / 2) * lbl_802813C0->fCellW - lbl_802813C0->fCellW / 2.0f;
        fAlong = (lbl_802813C0->anRows[nView] / 2) * lbl_802813C0->fCellD - lbl_802813C0->fCellD / 2.0f;
    }
    fDirX = lbl_802813C0->aDir[nView][0];
    fNegZ = -lbl_802813C0->aDir[nView][2];
    lbl_802813C0->aCorner[nView][0] = fAcross * fNegZ
        + (PLAYER(fn_8001707C(nViewCopy))->vTarget[0] - fAlong * lbl_802813C0->aDir[nView][0]);
    lbl_802813C0->aCorner[nView][2] = fAcross * fDirX
        + (PLAYER(fn_8001707C(nViewCopy))->vTarget[2] - fAlong * lbl_802813C0->aDir[nView][2]);
}

// Samples the ground height under up to four more of the view's grid points (a frame's share),
// first laying the grid out again if the player's target has moved. Points on ground of class 12
// are lifted by a ninth.
void fn_8009BE08(int nView) {
    int n;
    CourseInfo* pCourse = fn_8000C594();
    // fake match: an s32 (long) copy of nView, kept in its own register, for the fn_8001707C calls
    s32 nViewCopy;
    f32 vPoint[4];
    f32 vNormal[4];
    SurfaceType* pSurface;
    int nSteps = 0;
    f32 fAcross;
    f32 fAlong;
    f32 fDirX;
    f32 fDirZ;
    nViewCopy = nView;
    if (!fn_8009BD24(fn_8001707C(nViewCopy))) {
        return;
    }
    if (lbl_802813C0->aTarget[nView][0] != PLAYER(fn_8001707C(nViewCopy))->vTarget[0]
        || lbl_802813C0->aTarget[nView][1] != PLAYER(fn_8001707C(nViewCopy))->vTarget[1]
        || lbl_802813C0->aTarget[nView][2] != PLAYER(fn_8001707C(nViewCopy))->vTarget[2]) {
        fn_8009B970(nView);
    }
    fDirX = lbl_802813C0->aDir[nView][0];
    fDirZ = lbl_802813C0->aDir[nView][2];
    while ((n = lbl_802813C0->anDone[nView]) < lbl_802813C0->nCols * lbl_802813C0->anRows[nView]
           && nSteps < 4) {
        fAcross = (n % lbl_802813C0->nCols) * lbl_802813C0->fCellW;
        fAlong = (n / lbl_802813C0->nCols) * lbl_802813C0->fCellD;
        vPoint[0] = fAcross * fDirZ + (fAlong * fDirX + lbl_802813C0->aCorner[nView][0]);
        vPoint[1] = 2.0f + PLAYER(fn_8001707C(nViewCopy))->vTarget[1];
        vPoint[2] = (fAlong * fDirZ + lbl_802813C0->aCorner[nView][2]) - fAcross * fDirX;
        vPoint[3] = 1.0f;
        lbl_802813C0->apHeight[nView][n] =
            Ter_GetSupportingGroundData(pCourse, vPoint, &pSurface, vNormal);
        if (pSurface != NULL && pSurface->nClass == 12) {
            lbl_802813C0->apHeight[nView][n] += 1.0f / 9.0f;
        }
        nSteps++;
        lbl_802813C0->anDone[nView]++;
    }
}

// Builds the view's grid lines from the sampled heights: first the rows across, then the lines
// along. Each point gives two vertices (a line strip drawn doubled back); a point with no ground
// (the -65536.125 marker) breaks the line, and the end of each line fades out. The texture scrolls
// with the frame count.
void GR_BuildGridRenderData(int nView) {
    f32 fPrev;
    f32 fHole;
    f32 fDirX;
    f32 fDirZ;
    f32 fCornerX;
    f32 fCornerZ;
    f32 fAlong;
    f32 fAcross;
    f32 fHeight;
    f32 fX;
    f32 fY;
    f32 fZ;
    f32 fU;
    f32 fV;
    f32 fGap;
    f32 fPeriod;
    int n;
    int nRow;
    int nCol;
    int nEdge;
    int nGapEdge;
    int bInGap;
    int k;
    int i;
    // fake match: an s32 (long) copy of nView, kept in its own register, for the fn_8001707C call
    s32 nViewCopy;
    nViewCopy = nView;
    lbl_802813C0->nVerts = 0;
    fPrev = 0.0f;
    lbl_802813C0->nIndices = 0;
    bInGap = 0;
    nGapEdge = -1;
    fHole = -65536.125f;
    fDirX = lbl_802813C0->aDir[nView][0];
    fDirZ = lbl_802813C0->aDir[nView][2];
    fCornerX = lbl_802813C0->aCorner[nView][0];
    fCornerZ = lbl_802813C0->aCorner[nView][2];
    for (n = 0; n < lbl_802813C0->nCols * lbl_802813C0->anRows[nView]; n++) {
        nRow = n / lbl_802813C0->nCols;
        nCol = n % lbl_802813C0->nCols;
        fAcross = nCol * lbl_802813C0->fCellW;
        fAlong = nRow * lbl_802813C0->fCellD;
        fHeight = lbl_802813C0->apHeight[nView][nCol + nRow * lbl_802813C0->nCols];
        if (nCol == 0) {
            nEdge = 0;
        } else if (nCol == lbl_802813C0->nCols - 1) {
            nEdge = 1;
        } else {
            nEdge = -1;
        }
        if (fHole == fHeight) {
            if (!bInGap) {
                nGapEdge = nEdge;
                bInGap = 1;
            }
        } else {
            if (bInGap) {
                nEdge = nGapEdge;
                bInGap = 0;
            }
            fY = 0.01f + fHeight;
            fX = fAcross * fDirZ + (fAlong * fDirX + fCornerX);
            fZ = (fAlong * fDirZ + fCornerZ) - fAcross * fDirX;
            k = 0;
            do {
                lbl_802813C0->apVert[nView][lbl_802813C0->nVerts * 3 + 0] = fX;
                lbl_802813C0->apVert[nView][lbl_802813C0->nVerts * 3 + 1] = fY;
                lbl_802813C0->apVert[nView][lbl_802813C0->nVerts * 3 + 2] = fZ;
                lbl_802813C0->apColor[nView][lbl_802813C0->nVerts * 4 + 0] = lbl_802813C0->anColor[0];
                lbl_802813C0->apColor[nView][lbl_802813C0->nVerts * 4 + 1] = lbl_802813C0->anColor[1];
                lbl_802813C0->apColor[nView][lbl_802813C0->nVerts * 4 + 2] = lbl_802813C0->anColor[2];
                lbl_802813C0->apColor[nView][lbl_802813C0->nVerts * 4 + 3] =
                    (k == nEdge) ? 0 : (u8)lbl_802813C0->anColor[3];
                if (k == 0 && lbl_802813C0->nVerts > 0) {
                    fU = (u32)gSession.nFrameCount * lbl_802813C0->f100 * (fPrev - fHeight);
                    fU = fU - fn_80035074(fU);
                    lbl_802813C0->apUV[nView][lbl_802813C0->nVerts * 2 + 0] = fU;
                    lbl_802813C0->apUV[nView][lbl_802813C0->nVerts * 2 + 1] = 0.75f;
                    lbl_802813C0->apUV[nView][lbl_802813C0->nVerts * 2 - 2] = fU + lbl_802813C0->fFC;
                    lbl_802813C0->apUV[nView][lbl_802813C0->nVerts * 2 - 1] = 0.75f;
                }
                k++;
                lbl_802813C0->apIndex[nView][lbl_802813C0->nIndices] = lbl_802813C0->nVerts;
                lbl_802813C0->nIndices++;
                lbl_802813C0->nVerts++;
            } while (k < 2);
            fPrev = fHeight;
        }
    }
    bInGap = 0;
    fHole = -65536.125f;
    nGapEdge = -1;
    for (n = 0; n < lbl_802813C0->nCols * lbl_802813C0->anRows[nView]; n++) {
        nCol = n / lbl_802813C0->anRows[nView];
        nRow = n % lbl_802813C0->anRows[nView];
        fAcross = nCol * lbl_802813C0->fCellW;
        fAlong = nRow * lbl_802813C0->fCellD;
        fHeight = lbl_802813C0->apHeight[nView][nCol + nRow * lbl_802813C0->nCols];
        if (nRow == 0) {
            nEdge = 0;
        } else if (nRow == lbl_802813C0->anRows[nView] - 1) {
            nEdge = 1;
        } else {
            nEdge = -1;
        }
        if (fHole == fHeight) {
            if (!bInGap) {
                nGapEdge = nEdge;
                bInGap = 1;
            }
        } else {
            if (bInGap) {
                nEdge = nGapEdge;
                bInGap = 0;
            }
            k = 0;
            fY = 0.01f + fHeight;
            fX = fAcross * fDirZ + (fAlong * fDirX + fCornerX);
            fZ = (fAlong * fDirZ + fCornerZ) - fAcross * fDirX;
            do {
                lbl_802813C0->apVert[nView][lbl_802813C0->nVerts * 3 + 0] = fX;
                lbl_802813C0->apVert[nView][lbl_802813C0->nVerts * 3 + 1] = fY;
                lbl_802813C0->apVert[nView][lbl_802813C0->nVerts * 3 + 2] = fZ;
                lbl_802813C0->apColor[nView][lbl_802813C0->nVerts * 4 + 0] = lbl_802813C0->anColor[0];
                lbl_802813C0->apColor[nView][lbl_802813C0->nVerts * 4 + 1] = lbl_802813C0->anColor[1];
                lbl_802813C0->apColor[nView][lbl_802813C0->nVerts * 4 + 2] = lbl_802813C0->anColor[2];
                lbl_802813C0->apColor[nView][lbl_802813C0->nVerts * 4 + 3] =
                    (k == nEdge) ? 0 : (u8)lbl_802813C0->anColor[3];
                if (k == 0 && lbl_802813C0->nVerts > 0) {
                    fU = (u32)gSession.nFrameCount * lbl_802813C0->f100 * (fPrev - fHeight);
                    if ((s8)GOLFERSTATE_GetCurrentState(fn_8001707C(nViewCopy)) == GS_ZOOM) {
                        fV = 0.75f;
                        fGap = 0.0625f;
                    } else {
                        fV = 0.25f;
                        fGap = 0.1875f;
                    }
                    if (lbl_802813C0->fCellW > lbl_802813C0->fCellD) {
                        fPeriod = lbl_802813C0->fFC * (lbl_802813C0->fCellD / lbl_802813C0->fCellW) + fGap;
                        fU = fU - fPeriod * (int)(fU / fPeriod);
                        if (fU < 0.0f) {
                            fU += fPeriod;
                        }
                        fU = fU + ((1.0f + fGap) - fPeriod);
                    } else {
                        fU = fU - fn_80035074(fU);
                    }
                    lbl_802813C0->apUV[nView][lbl_802813C0->nVerts * 2 + 0] = fU;
                    lbl_802813C0->apUV[nView][lbl_802813C0->nVerts * 2 + 1] = fV;
                    lbl_802813C0->apUV[nView][lbl_802813C0->nVerts * 2 - 2] =
                        lbl_802813C0->fFC * (lbl_802813C0->fCellD / lbl_802813C0->fCellW) + fU;
                    lbl_802813C0->apUV[nView][lbl_802813C0->nVerts * 2 - 1] = fV;
                }
                k++;
                lbl_802813C0->apIndex[nView][lbl_802813C0->nIndices] = lbl_802813C0->nVerts;
                lbl_802813C0->nIndices++;
                lbl_802813C0->nVerts++;
            } while (k < 2);
            fPrev = fHeight;
        }
    }
    // the strip's first and last vertices are clear, and so is each pair with a clear half
    lbl_802813C0->apColor[nView][3] = 0;
    lbl_802813C0->apColor[nView][lbl_802813C0->nVerts * 4 - 1] = 0;
    for (i = 1; i < lbl_802813C0->nVerts - 1; i += 2) {
        if (lbl_802813C0->apColor[nView][i * 4 + 3] == 0 || lbl_802813C0->apColor[nView][i * 4 + 7] == 0) {
            lbl_802813C0->apColor[nView][i * 4 + 3] = 0;
            lbl_802813C0->apColor[nView][i * 4 + 7] = 0;
        }
    }
}

// Draws the view's grid once every point has been sampled: only for a human player standing over
// the ball (set-up, aiming and green cameras, or the swing before it starts).
void fn_8009C914(int nView) {
    // fake match: an s32 (long) copy of nView, kept in its own register, for the fn_8001707C calls
    s32 nViewCopy;
    TrailDraw draw;
    TrailMeshDesc desc;
    CamLens* pLens;
    f32 fAC;
    int nPlayer;
    int nState;
    nViewCopy = nView;
    if (!fn_8009BD24(fn_8001707C(nView))) {
        return;
    }
    nPlayer = fn_8001707C(nViewCopy);
    nState = GOLFERSTATE_GetCurrentState(nPlayer);
    if ((s8)nState == GS_WAIT) {
        return;
    }
    if (Player_IsCPU(nPlayer)) {
        return;
    }
    if (fn_800E415C()) {
        return;
    }
    if (fn_800E5098()) {
        return;
    }
    // fake match: the state ranges written as EA's compiled range checks.
    if (!((u8)(nState - GS_SHOT_SETUP) <= GS_ELEVATOR - GS_SHOT_SETUP || (s8)nState == GS_SWING
          || (u8)(nState - GS_GREEN_REVERSE_PUTT) <= GS_KNEE_CAM - GS_GREEN_REVERSE_PUTT
          || (s8)nState == GS_GREEN_MORPH)) {
        return;
    }
    switch (gPlayers[nPlayer].swing.nState) {
    case 0:
        break;
    default:
        return;
    }
    if (lbl_802813C0->anDone[nView] != lbl_802813C0->nCols * lbl_802813C0->anRows[nView]) {
        return;
    }
    GR_BuildGridRenderData(nView);
    fn_80016B9C();
    fn_80035118(4, 5);
    fn_80012F50(0, 6, 0x80);
    fn_80012F34(0);
    fn_8005CC64(lbl_80281FAC, lbl_80281FA8);
    if (lbl_802813C0->b104) {
        fn_80014118(0x70);
    } else {
        fn_80014118(0x60);
    }
    fn_80035138(0);
    pLens = ((Camera*)*lbl_80280DF0)->unk10;
    fAC = fn_80014268((u8*)pLens);
    fn_800354B4((u8*)pLens, 500.0f + fAC);
    fn_800352BC();
    fn_80035294();
    fn_80016B9C();
    fn_80016B9C();
    fn_80016B9C();
    fn_800354B4((u8*)pLens, fAC);
    fn_80012EF8();
    draw.nPrims = 3;
    draw.nFirst = 0;
    draw.nCount = lbl_802813C0->nIndices;
    desc.n0 = 1;
    desc.nVerts = lbl_802813C0->nVerts;
    desc.pDraw = &draw;
    desc.pIndices = lbl_802813C0->apIndex[nView];
    desc.pPos = lbl_802813C0->apVert[nView];
    desc.pColour = lbl_802813C0->apColor[nView];
    desc.pUV = lbl_802813C0->apUV[nView];
    fn_80036100((ShaderObject*)lbl_802813C0->aMesh[nView], &desc, 1);
    fn_800360D4(lbl_802813C0->aMesh[nView]);
    fn_80012F50(1, 6, 0x80);
    fn_80012F34(1);
    fn_80012EF8();
}

// Whether the grid shows for the player: never with fn_800E39F0; with the putter when
// options.b84 is set; otherwise when the player's ground (nSurface) is of a class that
// fn_8009BD94 lists.
u8 fn_8009BD24(int nPlayer) {
    if (fn_800E39F0()) {
        return 0;
    }
    if (gPlayers[nPlayer].nClub == CLUB_PUTTER_e) {
        return gSession.options.b84;
    }
    return fn_8009BD94(nPlayer);
}

u8 fn_8009BD94(int nPlayer) {
    int bOn = 0;
    int nSurface;
    nSurface = gPlayers[nPlayer].nSurface;
    // EA bug: only the first test checks for no surface (-1); the others read the row before
    // gSurfaceTypes.
    if ((nSurface >= 0 && gSurfaceTypes[nSurface].nClass == 3) || gSurfaceTypes[nSurface].nClass == 4
        || gSurfaceTypes[nSurface].nClass == 12 || gSurfaceTypes[nSurface].nClass == 18) {
        bOn = 1;
    }
    return bOn;
}

// a - b into out (four floats)
#ifdef __MWERKS__
asm void fn_8009CB78(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 0, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 0, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_8009CB78(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
    pOut[3] = pA[3] - pB[3];
}
#endif
