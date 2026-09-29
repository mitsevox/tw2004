// GoGreenGrid.c (EA's name, from its asserts; TW06; TW07 golf/hi-rendering/GoGreenGrid.c): the grid
// drawn over the putting green (and around a target on the green or fringe). It is laid out from
// the target back towards the ball (GR_ResetGreenGrid), the ground height under each point is
// sampled a few points a frame (GR_UpdateGreenGrid), and the lines are built into a mesh per view
// and drawn (GR_DrawGreenGrid).

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
void SD_InitShaderObject(void* pMesh, int n, s32* pDesc);
void SD_FreeShaderObject(void* pMesh);
void SD_DrawShaderObject(u8* pMesh);
void RC_vUpdateCurrentRenderCtxTransformationMatrices(void);
void RC_UpdateCurrentScreenMatrices(void);
void Camera_SetLensFarClip(u8* p, f32 v); // sets the lens's far clip distance, fAC (CA_fGetCameraFarZ reads it)
f32  CA_fGetCameraFarZ(u8* p);

void GR_Vec4Sub(f32* pA, f32* pB, f32* pOut);
void GR_BuildGridRenderData(s32 nView);
u8   GR_ShouldDrawGrid(int nPlayer);
u8   GR_PlayerIsTargetingGreen(int nPlayer);

GreenGrid gGreenGrid;                   // the grid's state
GreenGrid* gpGreenGrid = &gGreenGrid;   // every GR_ function reads the grid through it
TexBank*  gpGreenGridTexBank;   // the "gridpt" texture's bank
TexEntry* gpGreenGridTex;       // and the texture

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x80283EC8), before the 0.0f GR_vInit uses first; its body is unknown.
static f32 GoGreenGrid_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Sets the grid up at start-up (GO_vInitIG): for each view (two in split screen) a mesh and buffers
// for 256 vertices (128) and 16 rows (8) of heights; cells of 1 x 1, four columns, its colour and
// the "gridpt" texture.
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
        SD_InitShaderObject(gpGreenGrid->aMesh[i], 0x13, desc);
        gpGreenGrid->apVert[i] = StaticMem_Alloc(desc[0] * 12, 2, 16, "GoGreenGrid.c", 94);
        gpGreenGrid->apUV[i] = StaticMem_Alloc(desc[0] * 8, 2, 16, "GoGreenGrid.c", 99);
        gpGreenGrid->apColor[i] = StaticMem_Alloc(desc[0] * 4, 2, 16, "GoGreenGrid.c", 104);
        gpGreenGrid->apIndex[i] = StaticMem_Alloc(desc[0] * 2, 2, 16, "GoGreenGrid.c", 109);
        gpGreenGrid->apHeight[i] = StaticMem_Alloc(nRows * 16, 2, 16, "GoGreenGrid.c", 114);
        gpGreenGrid->aTarget[i][0] = 0.0f;
        gpGreenGrid->aTarget[i][1] = 0.0f;
        gpGreenGrid->aTarget[i][2] = 0.0f;
        gpGreenGrid->aTarget[i][3] = 1.0f;
        gpGreenGrid->anRows[i] = nRows;
    }
    gpGreenGrid->fCellW = 1.0f;
    gpGreenGrid->fCellD = 1.0f;
    gpGreenGrid->fFC = 1.0f;
    gpGreenGrid->f100 = 0.125f;
    gpGreenGrid->b104 = 1;
    gpGreenGrid->anColor[0] = 0x80;
    gpGreenGrid->anColor[1] = 0x80;
    gpGreenGrid->anColor[2] = 0x40;
    gpGreenGrid->anColor[3] = 0x41;
    gpGreenGrid->nCols = 4;
    fn_800102DC(fn_8000BEE4("gridpt"), &gpGreenGridTexBank, &gpGreenGridTex);
}

// The grid's per-hole set-up, called in the per-hole list just before BreakLine_InitForHole; empty
// in this build.
void GR_vInitForHole(void) {
}

// Frees each view's grid mesh and buffers (GR_vInit's), in gomainloop's shut-down.
void GR_vClose(void) {
    int nViews = gSession.nSplitScreen ? 2 : 1;
    int i;
    for (i = 0; i < nViews; i++) {
        SD_FreeShaderObject(gpGreenGrid->aMesh[i]);
        StaticMem_Free(gpGreenGrid->apVert[i]);
        StaticMem_Free(gpGreenGrid->apUV[i]);
        StaticMem_Free(gpGreenGrid->apColor[i]);
        StaticMem_Free(gpGreenGrid->apIndex[i]);
        StaticMem_Free(gpGreenGrid->apHeight[i]);
    }
}

// Lays the view's grid out for its player's target and starts its height sampling over: with the
// putter it runs from beyond the pin back past the ball (at most 8 or 16 rows), otherwise it is a
// square of nCols x nCols points.
void GR_ResetGreenGrid(int nView) {
    // fake match: an s32 (long) copy of nView, kept in its own register, for the
    // ViewController_GetActivePlayerNumber calls
    s32 nViewCopy;
    f32 vPin[4];
    f32 fNegZ;
    f32 fDirX;
    f32 fAcross;
    f32 fAlong;
    f32 fDist;
    f32 fLen;
    nViewCopy = nView;
    if (!GR_ShouldDrawGrid(ViewController_GetActivePlayerNumber(nViewCopy))) {
        return;
    }
    LLMath_CopyVec(PLAYER(ViewController_GetActivePlayerNumber(nViewCopy))->vTarget,
                   gpGreenGrid->aTarget[nView]);
    gpGreenGrid->anDone[nView] = 0;
    GR_Vec4Sub(PLAYER(ViewController_GetActivePlayerNumber(nViewCopy))->vTarget,
                PLAYER(ViewController_GetActivePlayerNumber(nViewCopy))->ball.vPos,
                gpGreenGrid->aDir[nView]);
    gpGreenGrid->aDir[nView][1] = 0.0f;
    fDist = (f32)Math_Sqrt(Vec3_LengthSqClamped(gpGreenGrid->aDir[nView]));
    LLMath_Normalize3(gpGreenGrid->aDir[nView], gpGreenGrid->aDir[nView]);
    if (PLAYER(ViewController_GetActivePlayerNumber(nViewCopy))->nClub == CLUB_PUTTER_e) {
        GR_Vec4Sub(&Ter_GetTGD()->pin[Game_CurrentPinSet()].x,
                    PLAYER(ViewController_GetActivePlayerNumber(nViewCopy))->ball.vPos,
                    vPin);
        vPin[1] = 0.0f;
        fLen = 2.0f + (f32)Math_Sqrt(Vec3_LengthSqClamped(vPin));
        gpGreenGrid->anRows[nView] = 2.0f * fLen;
        if (gSession.nSplitScreen && gpGreenGrid->anRows[nView] > 8) {
            gpGreenGrid->anRows[nView] = 8;
        } else if (!gSession.nSplitScreen && gpGreenGrid->anRows[nView] > 16) {
            gpGreenGrid->anRows[nView] = 16;
        }
        gpGreenGrid->fCellD = fLen / (gpGreenGrid->anRows[nView] - 1);
        fAcross = (gpGreenGrid->nCols / 2) * gpGreenGrid->fCellW - gpGreenGrid->fCellW / 2.0f;
        fAlong = gpGreenGrid->fCellD / 2.0f + fDist;
    } else {
        gpGreenGrid->anRows[nView] = gpGreenGrid->nCols;
        gpGreenGrid->fCellD = gpGreenGrid->fCellW;
        fAcross = (gpGreenGrid->nCols / 2) * gpGreenGrid->fCellW - gpGreenGrid->fCellW / 2.0f;
        fAlong = (gpGreenGrid->anRows[nView] / 2) * gpGreenGrid->fCellD - gpGreenGrid->fCellD / 2.0f;
    }
    fDirX = gpGreenGrid->aDir[nView][0];
    fNegZ = -gpGreenGrid->aDir[nView][2];
    gpGreenGrid->aCorner[nView][0] = fAcross * fNegZ
        + (PLAYER(ViewController_GetActivePlayerNumber(nViewCopy))->vTarget[0] - fAlong
           * gpGreenGrid->aDir[nView][0]);
    gpGreenGrid->aCorner[nView][2] = fAcross * fDirX
        + (PLAYER(ViewController_GetActivePlayerNumber(nViewCopy))->vTarget[2] - fAlong
           * gpGreenGrid->aDir[nView][2]);
}

// Whether the grid shows for the player: never with GM_Currently_SkillZoneMode; with the putter when
// options.bPuttingGrid is set; otherwise when the player's ground (nSurface) is of a class that
// GR_PlayerIsTargetingGreen lists.
u8 GR_ShouldDrawGrid(int nPlayer) {
    if (GM_Currently_SkillZoneMode()) {
        return 0;
    }
    if (gPlayers[nPlayer].nClub == CLUB_PUTTER_e) {
        return gSession.options.bPuttingGrid;
    }
    return GR_PlayerIsTargetingGreen(nPlayer);
}

// Whether the ground under the player's target (Player.nSurface) is of class 3 (green), 4 (fringe),
// 12 (the cup) or 18 (green).
u8 GR_PlayerIsTargetingGreen(int nPlayer) {
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

// Samples the ground height under up to four more of the view's grid points (a frame's share),
// first laying the grid out again if the player's target has moved. Points on ground of class 12
// are lifted by a ninth.
void GR_UpdateGreenGrid(int nView) {
    int n;
    int nRow;
    int nCol;
    CourseInfo* pCourse = Ter_GetTGD();
    // fake match: an s32 (long) copy of nView, kept in its own register, for the
    // ViewController_GetActivePlayerNumber calls
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
    if (!GR_ShouldDrawGrid(ViewController_GetActivePlayerNumber(nViewCopy))) {
        return;
    }
    if (gpGreenGrid->aTarget[nView][0] != PLAYER(ViewController_GetActivePlayerNumber(nViewCopy))->vTarget[0]
        || gpGreenGrid->aTarget[nView][1]
                != PLAYER(ViewController_GetActivePlayerNumber(nViewCopy))->vTarget[1]
        || gpGreenGrid->aTarget[nView][2]
                != PLAYER(ViewController_GetActivePlayerNumber(nViewCopy))->vTarget[2]) {
        GR_ResetGreenGrid(nView);
    }
    fDirX = gpGreenGrid->aDir[nView][0];
    fDirZ = gpGreenGrid->aDir[nView][2];
    while ((n = gpGreenGrid->anDone[nView]) < gpGreenGrid->nCols * gpGreenGrid->anRows[nView]
           && nSteps < 4) {
        nRow = n / gpGreenGrid->nCols;
        nCol = n % gpGreenGrid->nCols;
        fAcross = nCol * gpGreenGrid->fCellW;
        fAlong = nRow * gpGreenGrid->fCellD;
        vPoint[0] = fAcross * fDirZ + (fAlong * fDirX + gpGreenGrid->aCorner[nView][0]);
        vPoint[1] = 2.0f + PLAYER(ViewController_GetActivePlayerNumber(nViewCopy))->vTarget[1];
        vPoint[2] = (fAlong * fDirZ + gpGreenGrid->aCorner[nView][2]) - fAcross * fDirX;
        vPoint[3] = 1.0f;
        gpGreenGrid->apHeight[nView][n] =
            Ter_GetSupportingGroundData(pCourse, vPoint, &pSurface, vNormal);
        if (pSurface != NULL && pSurface->nClass == 12) {
            gpGreenGrid->apHeight[nView][n] += 1.0f / 9.0f;
        }
        nSteps++;
        gpGreenGrid->anDone[nView]++;
    }
}

// Builds the view's grid lines from the sampled heights: first the rows across, then the lines
// along. Each point gives two vertices (a line strip drawn doubled back); a point with no ground
// (the -65536.125 marker) breaks the line, and the end of each line fades out. The texture scrolls
// with the frame count.
void GR_BuildGridRenderData(s32 nView) {
    // fake match: this declaration order (found by search) sets the register allocation
    f32 fAlong;
    int n;
    int i;
    f32 fV;
    f32 fHeight;
    f32 fGap;
    f32 fU;
    f32 fAcross;
    int nCol;
    int nRow;
    int nEdge;
    int k;
    f32 fDirX;
    f32 fDirZ;
    int bInGap;
    int nGapEdge;
    f32 fHole;
    f32 fCornerX;
    f32 fCornerZ;
    f32 fPeriod;
    f32 fPrev;
    // fake match: &gSession through a local, so its base (not &gSession.nFrameCount) is kept
    Session* pSession;
    gpGreenGrid->nVerts = 0;
    fPrev = 0.0f;
    gpGreenGrid->nIndices = 0;
    bInGap = 0;
    nGapEdge = -1;
    fHole = -65536.125f;
    fDirX = gpGreenGrid->aDir[nView][0];
    fDirZ = gpGreenGrid->aDir[nView][2];
    fCornerX = gpGreenGrid->aCorner[nView][0];
    fCornerZ = gpGreenGrid->aCorner[nView][2];
    // fake match: pSession is set in the loop test (always run at least once, so the second loop
    // sees it set too); the compiler then hoists &gSession to the end of the preamble, as EA's
    // code schedules it
    for (n = 0; pSession = &gSession, n < gpGreenGrid->nCols * gpGreenGrid->anRows[nView]; n++) {
        nRow = n / gpGreenGrid->nCols;
        nCol = n % gpGreenGrid->nCols;
        fAcross = nCol * gpGreenGrid->fCellW;
        fAlong = nRow * gpGreenGrid->fCellD;
        fHeight = gpGreenGrid->apHeight[nView][nCol + nRow * gpGreenGrid->nCols];
        if (nCol == 0) {
            nEdge = 0;
        } else if (nCol == gpGreenGrid->nCols - 1) {
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
            do {
                gpGreenGrid->apVert[nView][gpGreenGrid->nVerts * 3 + 0] =
                    fAcross * fDirZ + (fAlong * fDirX + fCornerX);
                gpGreenGrid->apVert[nView][gpGreenGrid->nVerts * 3 + 1] = 0.01f + fHeight;
                gpGreenGrid->apVert[nView][gpGreenGrid->nVerts * 3 + 2] =
                    (fAlong * fDirZ + fCornerZ) - fAcross * fDirX;
                gpGreenGrid->apColor[nView][gpGreenGrid->nVerts * 4 + 0] = gpGreenGrid->anColor[0];
                gpGreenGrid->apColor[nView][gpGreenGrid->nVerts * 4 + 1] = gpGreenGrid->anColor[1];
                gpGreenGrid->apColor[nView][gpGreenGrid->nVerts * 4 + 2] = gpGreenGrid->anColor[2];
                gpGreenGrid->apColor[nView][gpGreenGrid->nVerts * 4 + 3] =
                    (k == nEdge) ? 0 : (u8)gpGreenGrid->anColor[3];
                if (k == 0 && gpGreenGrid->nVerts > 0) {
                    fU = (u32)pSession->nFrameCount * gpGreenGrid->f100 * (fPrev - fHeight);
                    fU = fU - Math_Floor(fU);
                    gpGreenGrid->apUV[nView][gpGreenGrid->nVerts * 2 + 0] = fU;
                    gpGreenGrid->apUV[nView][gpGreenGrid->nVerts * 2 + 1] = 0.75f;
                    gpGreenGrid->apUV[nView][gpGreenGrid->nVerts * 2 - 2] = fU + gpGreenGrid->fFC;
                    gpGreenGrid->apUV[nView][gpGreenGrid->nVerts * 2 - 1] = 0.75f;
                }
                k++;
                gpGreenGrid->apIndex[nView][gpGreenGrid->nIndices] = gpGreenGrid->nVerts;
                gpGreenGrid->nIndices++;
                gpGreenGrid->nVerts++;
            } while (k < 2);
            fPrev = fHeight;
        }
    }
    bInGap = 0;
    fHole = -65536.125f;
    nGapEdge = -1;
    for (n = 0; n < gpGreenGrid->nCols * gpGreenGrid->anRows[nView]; n++) {
        nRow = n % gpGreenGrid->anRows[nView];
        nCol = n / gpGreenGrid->anRows[nView];
        fAcross = nCol * gpGreenGrid->fCellW;
        fAlong = nRow * gpGreenGrid->fCellD;
        fHeight = gpGreenGrid->apHeight[nView][nCol + nRow * gpGreenGrid->nCols];
        if (nRow == 0) {
            nEdge = 0;
        } else if (nRow == gpGreenGrid->anRows[nView] - 1) {
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
            do {
                gpGreenGrid->apVert[nView][gpGreenGrid->nVerts * 3 + 0] =
                    fAcross * fDirZ + (fAlong * fDirX + fCornerX);
                gpGreenGrid->apVert[nView][gpGreenGrid->nVerts * 3 + 1] = 0.01f + fHeight;
                gpGreenGrid->apVert[nView][gpGreenGrid->nVerts * 3 + 2] =
                    (fAlong * fDirZ + fCornerZ) - fAcross * fDirX;
                gpGreenGrid->apColor[nView][gpGreenGrid->nVerts * 4 + 0] = gpGreenGrid->anColor[0];
                gpGreenGrid->apColor[nView][gpGreenGrid->nVerts * 4 + 1] = gpGreenGrid->anColor[1];
                gpGreenGrid->apColor[nView][gpGreenGrid->nVerts * 4 + 2] = gpGreenGrid->anColor[2];
                gpGreenGrid->apColor[nView][gpGreenGrid->nVerts * 4 + 3] =
                    (k == nEdge) ? 0 : (u8)gpGreenGrid->anColor[3];
                if (k == 0 && gpGreenGrid->nVerts > 0) {
                    fU = (u32)pSession->nFrameCount * gpGreenGrid->f100 * (fPrev - fHeight);
                    if ((s8)GOLFERSTATE_GetCurrentState(ViewController_GetActivePlayerNumber(nView))
                        == GS_ZOOM) {
                        fV = 0.75f;
                        fGap = 0.0625f;
                    } else {
                        fV = 0.25f;
                        fGap = 0.1875f;
                    }
                    if (gpGreenGrid->fCellW > gpGreenGrid->fCellD) {
                        fPeriod = gpGreenGrid->fFC * (gpGreenGrid->fCellD / gpGreenGrid->fCellW) + fGap;
                        fU = fU - fPeriod * (int)(fU / fPeriod);
                        if (fU < 0.0f) {
                            fU += fPeriod;
                        }
                        fU = fU + ((1.0f + fGap) - fPeriod);
                    } else {
                        fU = fU - Math_Floor(fU);
                    }
                    gpGreenGrid->apUV[nView][gpGreenGrid->nVerts * 2 + 0] = fU;
                    gpGreenGrid->apUV[nView][gpGreenGrid->nVerts * 2 + 1] = fV;
                    gpGreenGrid->apUV[nView][gpGreenGrid->nVerts * 2 - 2] =
                        gpGreenGrid->fFC * (gpGreenGrid->fCellD / gpGreenGrid->fCellW) + fU;
                    gpGreenGrid->apUV[nView][gpGreenGrid->nVerts * 2 - 1] = fV;
                }
                k++;
                gpGreenGrid->apIndex[nView][gpGreenGrid->nIndices] = gpGreenGrid->nVerts;
                gpGreenGrid->nIndices++;
                gpGreenGrid->nVerts++;
            } while (k < 2);
            fPrev = fHeight;
        }
    }
    // the strip's first and last vertices are clear, and so is each pair with a clear half
    gpGreenGrid->apColor[nView][3] = 0;
    gpGreenGrid->apColor[nView][gpGreenGrid->nVerts * 4 - 1] = 0;
    for (i = 1; i < gpGreenGrid->nVerts - 1; i += 2) {
        if (gpGreenGrid->apColor[nView][i * 4 + 3] == 0 || gpGreenGrid->apColor[nView][i * 4 + 7] == 0) {
            gpGreenGrid->apColor[nView][i * 4 + 3] = 0;
            gpGreenGrid->apColor[nView][i * 4 + 7] = 0;
        }
    }
}

// Draws the view's grid once every point has been sampled: only for a human player standing over
// the ball (set-up, aiming and green cameras, or the swing before it starts).
void GR_DrawGreenGrid(int nView) {
    // fake match: an s32 (long) copy of nView, kept in its own register, for the
    // ViewController_GetActivePlayerNumber calls
    s32 nViewCopy;
    TrailDraw draw;
    TrailMeshDesc desc;
    CamLens* pLens;
    f32 fAC;
    int nPlayer;
    int nState;
    nViewCopy = nView;
    if (!GR_ShouldDrawGrid(ViewController_GetActivePlayerNumber(nView))) {
        return;
    }
    nPlayer = ViewController_GetActivePlayerNumber(nViewCopy);
    nState = GOLFERSTATE_GetCurrentState(nPlayer);
    if ((s8)nState == GS_WAIT) {
        return;
    }
    if (Player_IsCPU(nPlayer)) {
        return;
    }
    if (GUI_IsPauseMenuOpen()) {
        return;
    }
    if (GUI_IsCaddieTipWindowOpen()) {
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
    if (gpGreenGrid->anDone[nView] != gpGreenGrid->nCols * gpGreenGrid->anRows[nView]) {
        return;
    }
    GR_BuildGridRenderData(nView);
    RenderState_SetCameraMatrices();
    RenderState_SetBlendFactors(4, 5);
    DS_vSetAlphaTestMode(0, 6, 0x80);
    DS_vEnableZBufferUpdate(0);
    RenderState_SetBankTexture(gpGreenGridTexBank, gpGreenGridTex);
    if (gpGreenGrid->b104) {
        RenderState_SetDrawFlags(0x70);
    } else {
        RenderState_SetDrawFlags(0x60);
    }
    RenderState_SetClipMode(0);
    pLens = ((Camera*)*gppCurrentRenderCtx)->unk10;
    fAC = CA_fGetCameraFarZ((u8*)pLens);
    Camera_SetLensFarClip((u8*)pLens, 500.0f + fAC);
    RC_UpdateCurrentScreenMatrices();
    RC_vUpdateCurrentRenderCtxTransformationMatrices();
    RenderState_SetCameraMatrices();
    RenderState_SetCameraMatrices();
    RenderState_SetCameraMatrices();
    Camera_SetLensFarClip((u8*)pLens, fAC);
    RenderState_Flush();
    draw.nPrims = 3;
    draw.nFirst = 0;
    draw.nCount = gpGreenGrid->nIndices;
    desc.n0 = 1;
    desc.nVerts = gpGreenGrid->nVerts;
    desc.pDraw = &draw;
    desc.pIndices = gpGreenGrid->apIndex[nView];
    desc.pPos = gpGreenGrid->apVert[nView];
    desc.pColour = gpGreenGrid->apColor[nView];
    desc.pUV = gpGreenGrid->apUV[nView];
    SD_FillShaderObject((ShaderObject*)gpGreenGrid->aMesh[nView], &desc, 1);
    SD_DrawShaderObject(gpGreenGrid->aMesh[nView]);
    DS_vSetAlphaTestMode(1, 6, 0x80);
    DS_vEnableZBufferUpdate(1);
    RenderState_Flush();
}

// a - b into out (four floats)
#ifdef __MWERKS__
asm void GR_Vec4Sub(register f32* pA, register f32* pB, register f32* pOut) {
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
void GR_Vec4Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
    pOut[3] = pA[3] - pB[3];
}
#endif
