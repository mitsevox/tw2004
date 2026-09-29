// GoBreakLine.c (EA's name, from its asserts; TW07 golf/hi-rendering/GoBreakLine.c): the putt's
// break line, a line on the green from the ball that shows how the putt will break (BreakLine,
// breakline.h). When the player stands over a putt, a copy of the ball is launched with the
// putt's power for the distance and stepped a little each frame, laying a textured strip behind
// it; the point where it came nearest the cup is kept for GameAnalysis_GetPuttBreakAngle. Also the
// caddie's putt read in feet (BreakLine_GetCaddyTipInfo).

#include "golfer.h"
#include "game.h"
#include "camera.h"
#include "breakline.h"

u8 gbBreakLineOn;            // a line was started (BreakLine_Reset); BreakLine_Update steps it
BreakLine* gpBreakLine;      // the line's state, allocated by BreakLine_InitModule

void SD_InitShaderObject(void* pMesh, int n, s32* pDesc);    // Skin.c: sets up a mesh object
void SD_FreeShaderObject(void* pMesh);         // Skin.c: frees a mesh object
void SD_DrawShaderObject(u8* pMesh);           // Skin.c
void BreakLine_Vec3Add(f32* pA, f32* pB, f32* pOut);
void BreakLine_Vec4Sub(f32* pA, f32* pB, f32* pOut);
void BreakLine_Vec3Sub(f32* pA, f32* pB, f32* pOut);
void SD_SetShaderTypeParameters(int nRow, void* pData);   // GoTerrain.c: calls row nRow's function with pData

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f (0x802843F8), before the 27.0f BreakLine_InitModule uses first; its body is unknown.
static f32 GoBreakLine_StrippedFn(f32 x) {
    return x + 1.0f;
}

// Allocates the break line's state (gpBreakLine) at start-up and sets its fade from alpha 27 at
// the ball (fAB30) to 0 at the end (fAB34), with fAB38 20 and fAB3C 0.009, which nothing in this
// file reads.
void BreakLine_InitModule(void) {
    gpBreakLine = StaticMem_Alloc(sizeof(BreakLine), 2, 16, "GoBreakLine.c", 93);
    gpBreakLine->fAB30 = 27.0f;
    gpBreakLine->fAB34 = 0.0f;
    gpBreakLine->fAB38 = 20.0f;
    gpBreakLine->fAB3C = 0.009f;
}

// Frees the break line's state (BreakLine_InitModule's allocation).
void BreakLine_CloseModule(void) {
    StaticMem_Free(gpBreakLine);
    gpBreakLine = NULL;
}

// Sets the line up for a hole: its settings, a mesh per view, the "brkline" texture and the pin.
void BreakLine_InitForHole(void) {
    CourseInfo* pCourse = Ter_GetTGD();
    int nPin = Game_CurrentPinSet();
    s32 desc[2];
    u64 uHash;

    gpBreakLine->fAAE0 = 0.02f;
    gpBreakLine->fAAE4 = 4.0f;
    gpBreakLine->nAAEC = 5;
    gpBreakLine->fAB0C = 0.01f;
    gpBreakLine->fAB10 = -800.0f;
    gpBreakLine->fAB14 = 0.0f;
    gpBreakLine->fAB18 = 100.0f;
    gpBreakLine->fAB1C = 0.024f;
    gpBreakLine->anColor[0] = 0x80;
    gpBreakLine->anColor[1] = 0x80;
    gpBreakLine->anColor[2] = 0x80;
    gpBreakLine->anColor[3] = 0x33;
    desc[0] = gSession.nSplitScreen ? 450 : 900;
    desc[1] = 1;
    SD_InitShaderObject(gpBreakLine->aMesh[0], 5, desc);
    if (gSession.nSplitScreen) {
        SD_InitShaderObject(gpBreakLine->aMesh[1], 5, desc);
    }
    uHash = fn_8000BEE4("brkline");
    fn_800102DC(uHash, &gpBreakLine->pBank, &gpBreakLine->pTex);
    LLMath_CopyVec(&pCourse->pin[nPin].x, gpBreakLine->vPin);
    gpBreakLine->abSkip[0] = 1;
    gpBreakLine->anVerts[0] = 0;
    if (gSession.nSplitScreen) {
        gpBreakLine->abSkip[1] = 1;
        gpBreakLine->anVerts[1] = 0;
    }
    gbBreakLineOn = 0;
}

// Frees the meshes BreakLine_InitForHole made (one per view) and turns the line off (gbBreakLineOn).
void BreakLine_CloseAfterHole(void) {
    SD_FreeShaderObject(gpBreakLine->aMesh[0]);
    if (gSession.nSplitScreen) {
        SD_FreeShaderObject(gpBreakLine->aMesh[1]);
    }
    gbBreakLineOn = 0;
}

// Steps view nView's line (BreakLine_Render) while the line is on (gbBreakLineOn) and its player, a
// human, stands over a putt within 75 of the hole with the target (vTarget) within an inch of the
// pin (EA's test; distances in yards). A view's first call after the line is set up only clears
// its abSkip.
void BreakLine_Update(int nView) {
    int nPlayer = ViewController_GetActivePlayerNumber(nView);
    f32 fDist;

    fDist = LLMath_DistanceBetween3(gpBreakLine->vPin, gPlayers[nPlayer].vTarget);
    fDist *= 36.0f;                     // yards to inches
    if (fDist <= 1.0f &&
        gPlayers[nPlayer].nShotKind == 0 && gPlayers[nPlayer].swing.nState == 0 &&
        gPlayers[nPlayer].fDistance < 75.0f && !Player_IsCPU(nPlayer) && gbBreakLineOn) {
        if (!gpBreakLine->abSkip[nView]) {
            BreakLine_Render(nView);
            return;
        }
        gpBreakLine->abSkip[nView] = 0;
    }
}

// Draws view nView's line, first growing it by one step: while the ball copy rolls, step it, lay
// the next pair of vertices across its path, fade the line's alpha from fAB30 at the ball to fAB34
// at the end, and trigger events 0x28 / 0x29 when it stops or starts to move away from the pin.
void BreakLine_Render(int nView) {
    f32 vAxis[4] = {0.0f, 0.0f, 1.0f, 0.0f};
    int nPlayer = ViewController_GetActivePlayerNumber(nView);
    TrailMeshDescEx desc;
    s16 aIndex[BREAKLINE_VERTS];        // the frame fits at least this many; the size is not known
    s32 nFrame;
    f32 vCross[4];
    f32 vDir[4];
    f32 fDist;
    f32 fAngle;
    f32 fSin;
    f32 fCos;
    f32 fAlpha;
    f32 fZ;
    f32 fX;
    int i;

    if (gSession.options.a24[2]) {
        LLMath_DistanceBetween3(gpBreakLine->vPin, gPlayers[nPlayer].vTarget);
        RenderState_SetCameraMatrices();
        RenderState_SetBlendFactors(4, 5);
        DS_vSetAlphaTestMode(0, 6, 0x80);
        DS_vEnableZBufferUpdate(0);
        RenderState_SetBankTexture(gpBreakLine->pBank, gpBreakLine->pTex);
        RenderState_SetDrawFlags(0x70);
        RenderState_SetClipMode(0);
        RenderState_Flush();
        fDist = LLMath_DistanceBetween3(gpBreakLine->aBall[nView].vPos, gpBreakLine->vPin);
        if (gpBreakLine->abA91C[nView]) {
            if ((gpBreakLine->aBall[nView].nState == 2 || gpBreakLine->aBall[nView].nState == 3 ||
                 gpBreakLine->aBall[nView].nState == 4) && fDist > 0.001f) {
                Physics_SetSimulating(1);
                Physics_TimedSimulation(&gpBreakLine->aBall[nView], gpBreakLine->fAAE0,
                                        gpBreakLine->fAAE4);
                Physics_SetSimulating(0);
                fDist = LLMath_SquareDistanceBetween3(gpBreakLine->aBall[nView].vPos,
                                    &gpBreakLine->aBall[nView].pCourse->pin[Game_CurrentPinSet()].x);
                if (fDist < gpBreakLine->afAAD4[nView]) {
                    gpBreakLine->afAAD4[nView] = fDist;
                    LLMath_CopyVec(gpBreakLine->aBall[nView].vPos, gpBreakLine->aViewPoint[nView]);
                } else if (!gpBreakLine->abAADC[nView]) {
                    EVENT_Trigger(nPlayer, 0x29, gpBreakLine->aBall[nView].vPrev, 0);
                    gpBreakLine->abAADC[nView] = 1;
                }
                // The ball's heading on the ground, as an angle about y from the z axis.
                BreakLine_Vec4Sub(gpBreakLine->aBall[nView].vPos, gpBreakLine->aBall[nView].vPrev, vDir);
                vDir[1] = 0.0f;
                // EA bug: tests y, just cleared, where z was surely meant
                if (vDir[0] != 0.0f || vDir[1] != 0.0f) {
                    LLMath_Normalize3(vDir, vDir);
                }
                fAngle = Math_Acos(Vec3_Dot(vDir, vAxis));
                vec4flt_CrossProduct(vDir, vAxis, vCross);
                fAngle = fAngle * (vCross[1] < 0.0f ? -1.0f : 1.0f);
                fSin = Math_Sin(fAngle);
                fCos = Math_Cos(fAngle);
                // Turn the new pair of vertices to the heading and move them to the ball.
                for (i = gpBreakLine->anVerts[nView]; i <= gpBreakLine->anVerts[nView] + 1; i++) {
                    fX = gpBreakLine->aVert[nView][i][0];
                    fZ = fX * fCos + gpBreakLine->aVert[nView][i][2] * fSin;
                    gpBreakLine->aVert[nView][i][0] = fX * -fSin + gpBreakLine->aVert[nView][i][2] * fCos;
                    gpBreakLine->aVert[nView][i][2] = fZ;
                }
                BreakLine_Vec3Add(gpBreakLine->aVert[nView][gpBreakLine->anVerts[nView]],
                            gpBreakLine->aBall[nView].vPos,
                            gpBreakLine->aVert[nView][gpBreakLine->anVerts[nView]]);
                BreakLine_Vec3Add(gpBreakLine->aVert[nView][gpBreakLine->anVerts[nView] + 1],
                            gpBreakLine->aBall[nView].vPos,
                            gpBreakLine->aVert[nView][gpBreakLine->anVerts[nView] + 1]);
                fAlpha = gpBreakLine->fAB30;
                if (fAlpha < 0.0f) {
                    fAlpha = 0.0f;
                }
                if (gpBreakLine->anVerts[nView] != 0) {
                    for (i = 0; i <= gpBreakLine->anVerts[nView]; i += 2) {
                        gpBreakLine->aColor[nView][i][3] = fAlpha - (f32)i *
                            ((fAlpha - gpBreakLine->fAB34) / (f32)gpBreakLine->anVerts[nView]);
                        gpBreakLine->aColor[nView][i + 1][3] = fAlpha - (f32)i *
                            ((fAlpha - gpBreakLine->fAB34) / (f32)gpBreakLine->anVerts[nView]);
                    }
                }
                gpBreakLine->anVerts[nView] += 2;
                gpBreakLine->anAAF0[nView]++;
                if (gpBreakLine->anAAF0[nView] == gpBreakLine->nAAEC + 1) {
                    Vec3Copy(gpBreakLine->aVert[nView][gpBreakLine->anVerts[nView] - 2],
                             gpBreakLine->aVert[nView][gpBreakLine->anVerts[nView]]);
                    Vec3Copy(gpBreakLine->aVert[nView][gpBreakLine->anVerts[nView] - 1],
                             gpBreakLine->aVert[nView][gpBreakLine->anVerts[nView] + 1]);
                    gpBreakLine->anVerts[nView] += 2;
                }
                if (gpBreakLine->anAAF0[nView] > gpBreakLine->nAAEC) {
                    gpBreakLine->anAAF0[nView] = 1;
                }
                // EA bug: a misplaced parenthesis compares the split-screen test's truth value with
                // 450, so in split screen the line is never stopped at its length
                if ((!gSession.nSplitScreen && gpBreakLine->anVerts[nView] + 2 >= 900) ||
                    (gSession.nSplitScreen && gpBreakLine->anVerts[nView] + 2) >= 450) {
                    gpBreakLine->abA91C[nView] = 0;
                }
            } else {
                EVENT_Trigger(nPlayer, 0x28, gpBreakLine->aBall[nView].vPos, 0);
                gpBreakLine->abA91C[nView] = 0;
                if (!gpBreakLine->abAADC[nView]) {
                    EVENT_Trigger(nPlayer, 0x29, gpBreakLine->aBall[nView].vPos, 0);
                    gpBreakLine->abAADC[nView] = 1;
                }
            }
        }
        if (gpBreakLine->anVerts[nView] > 2) {
            for (i = 0; i < gpBreakLine->anVerts[nView]; i++) {
                aIndex[i] = i;
            }
            desc.desc.n0 = gpBreakLine->anVerts[nView];
            desc.desc.nVerts = gpBreakLine->anVerts[nView];
            desc.desc.pDraw = NULL;
            desc.desc.pIndices = aIndex;
            desc.desc.pPos = gpBreakLine->aVert[nView][0];
            desc.desc.pColour = gpBreakLine->aColor[nView][0];
            desc.desc.pUV = gpBreakLine->aUV[nView][0];
            desc.af18[0] = gpBreakLine->fAB10;
            desc.af18[1] = gpBreakLine->fAB14;
            desc.af18[2] = gpBreakLine->fAB18;
            desc.af18[3] = 1.0f / gpBreakLine->fAB18;
            nFrame = gSession.nFrameCount;
            SD_SetShaderTypeParameters(5, &nFrame);
            SD_FillShaderObject((ShaderObject*)gpBreakLine->aMesh[nView], &desc, 1);
            SD_DrawShaderObject(gpBreakLine->aMesh[nView]);
        }
        DS_vSetAlphaTestMode(1, 6, 0x80);
        DS_vEnableZBufferUpdate(1);
        RenderState_Flush();
    }
}

// Copies view nView's point of the line closest to the cup into pOut: where the rolling ball copy
// came nearest the pin (BreakLine_Render keeps it), the ball's start until the line moves.
void BreakLine_GetClosestPointToCupPos(int nView, f32* pOut) {
    LLMath_CopyVec(gpBreakLine->aViewPoint[nView], pOut);
}

// fake match: EA reads the player through an inline; written in place, pPlayer is allocated r29, not r31
static inline Player* fn_800C8C70_Read(int nView) {
    return &gPlayers[ViewController_GetActivePlayerNumber(nView)];
}

// Starts view nView's line when its player stands over a putt within 75 of the hole: lays out the
// line's vertices, colours and texture coordinates, and launches a copy of the ball with the
// putt's power for the distance, with sounds and effects off.
void BreakLine_Reset(int nView) {
    Player* pPlayer;
    f32 fPower;
    int i;
    int nTex = 0;

    pPlayer = fn_800C8C70_Read(nView);
    if (pPlayer->nShotKind == 0 && pPlayer->fDistance < 75.0f && pPlayer->fDistance > 0.0f) {
        gpBreakLine->abA91C[nView] = 1;
        gpBreakLine->abSkip[nView] = 1;
        gpBreakLine->anVerts[nView] = 0;
        gbBreakLineOn = 1;
        Mem_cpy(&gpBreakLine->aBall[nView], &pPlayer->ball, sizeof(Ball));
        LLMath_CopyVec(gpBreakLine->aBall[nView].vPos, gpBreakLine->aViewPoint[nView]);
        gpBreakLine->afAAD4[nView] = 1000000.0f;
        gpBreakLine->abAADC[nView] = 0;
        fPower = Physics_EstimatePuttPower(pPlayer->fDistance);
        gpBreakLine->anAAF0[nView] = 0;
        for (i = 0; i < BREAKLINE_POINTS; i++) {
            gpBreakLine->aVert[nView][i * 2][0] = 0.0f;
            gpBreakLine->aVert[nView][i * 2][1] = 0.0f;
            gpBreakLine->aVert[nView][i * 2][2] = gpBreakLine->fAB1C;
            gpBreakLine->aVert[nView][i * 2 + 1][0] = 0.0f;
            gpBreakLine->aVert[nView][i * 2 + 1][1] = 0.0f;
            gpBreakLine->aVert[nView][i * 2 + 1][2] = -gpBreakLine->fAB1C;
            gpBreakLine->aColor[nView][i * 2][0] = gpBreakLine->anColor[0];
            gpBreakLine->aColor[nView][i * 2][1] = gpBreakLine->anColor[1];
            gpBreakLine->aColor[nView][i * 2][2] = gpBreakLine->anColor[2];
            gpBreakLine->aColor[nView][i * 2][3] = gpBreakLine->anColor[3];
            gpBreakLine->aColor[nView][i * 2 + 1][0] = gpBreakLine->anColor[0];
            gpBreakLine->aColor[nView][i * 2 + 1][1] = gpBreakLine->anColor[1];
            gpBreakLine->aColor[nView][i * 2 + 1][2] = gpBreakLine->anColor[2];
            gpBreakLine->aColor[nView][i * 2 + 1][3] = gpBreakLine->anColor[3];
            gpBreakLine->aUV[nView][i * 2][0] = (f32)nTex / (f32)gpBreakLine->nAAEC;
            gpBreakLine->aUV[nView][i * 2][1] = 0.0f;
            gpBreakLine->aUV[nView][i * 2 + 1][0] = (f32)nTex / (f32)gpBreakLine->nAAEC;
            gpBreakLine->aUV[nView][i * 2 + 1][1] = 1.0f;
            nTex++;
            if (nTex > gpBreakLine->nAAEC) {
                nTex = 0;
            }
        }
        Physics_SetSimulating(1);
        gpBreakLine->aBall[nView].nState = 0;
        Physics_ShotImpact(&gpBreakLine->aBall[nView], pPlayer->nClub, pPlayer->nShotKind, fPower,
                    pPlayer->fAim, 1, pPlayer->vLaunchA, pPlayer->vLaunchB);
        Physics_SetSimulating(0);
    }
}

// The caddie's putt read for view nView, in feet: how far past (+) or short of the hole the aim
// point lies along the line from the ball, and how far to the side of that line (the sign gives
// the side). 0, 0 in split screen or when the points coincide; -999 when there is no tip, 999
// when the caddie gave up.
void BreakLine_GetCaddyTipInfo(int nView, f32* pLong, f32* pSide) {
    int nPlayer = ViewController_GetActivePlayerNumber(nView);
    int nPin = Game_CurrentPinSet();
    CourseInfo* pCourse = Ter_GetTGD();
    s8 nTip;
    f32 vTip[4];
    f32 vPin[4];
    f32 vHole[4];
    f32 vBall[4];
    f32 vAim[4];
    f32 vToHole[4];
    f32 vToAim[4];
    f32 vCross[4];
    f32 fHoleDist;
    f32 fAimDist;
    f32 fDot;
    f32 fAlong;
    f32 fSide;
    f32 fSign;

    if (gSession.nSplitScreen) {
        *pLong = 0.0f;
        *pSide = 0.0f;
        return;
    }
    nTip = Caddie_GetTip(nPlayer, vTip);
    if (nTip == 0) {
        *pLong = -999.0f;
        *pSide = -999.0f;
        return;
    }
    if (nTip == 2) {
        *pLong = 999.0f;
        *pSide = 999.0f;
        return;
    }
    // Flatten the three points onto the ground (y = 0).
    LLMath_CopyVec(&pCourse->pin[nPin].x, vPin);
    LLMath_CopyVec(vPin, vHole);
    vHole[1] = 0.0f;
    LLMath_CopyVec(gPlayers[nPlayer].vBall, vBall);
    vBall[1] = 0.0f;
    LLMath_CopyVec(vTip, vAim);
    vAim[1] = 0.0f;
    if (vAim[0] == vHole[0] && vAim[2] == vHole[2]) {
        *pLong = 0.0f;
        *pSide = 0.0f;
        return;
    }
    fHoleDist = LLMath_DistanceBetween3(vHole, vBall);
    if (fabs(fHoleDist) < 0.001f) {
        *pLong = 0.0f;
        *pSide = 0.0f;
        return;
    }
    if (vAim[0] == vBall[0] && vAim[2] == vBall[2]) {
        *pLong = 0.0f;
        *pSide = 0.0f;
        return;
    }
    fAimDist = LLMath_DistanceBetween3(vAim, vBall);
    if (fabs(fAimDist) < 0.001f) {
        *pLong = 0.0f;
        *pSide = 0.0f;
        return;
    }
    if (vBall[0] == vHole[0] && vBall[2] == vHole[2]) {
        *pLong = 0.0f;
        *pSide = 0.0f;
        return;
    }
    BreakLine_Vec3Sub(vHole, vBall, vToHole);
    LLMath_Normalize3(vToHole, vToHole);
    BreakLine_Vec3Sub(vAim, vBall, vToAim);
    LLMath_Normalize3(vToAim, vToAim);
    fDot = Vec3_Dot(vToHole, vToAim);
    if (0.0f == fDot) {
        *pLong = 0.0f;
        *pSide = 0.0f;
        return;
    }
    fAlong = fDot * fAimDist;
    fSide = fAimDist * Math_Sin(Math_Acos(fDot));
    vec4flt_CrossProduct(vToHole, vToAim, vCross);
    fAlong = fAlong - fHoleDist;
    if (vCross[1] < 0.0f) {
        fSign = 1.0f;
    } else {
        fSign = -1.0f;
    }
    fSide = fSide * fSign;
    *pLong = 3.0f * fAlong;
    *pSide = 3.0f * fSide;
}

// a + b into out (three floats)
#ifdef __MWERKS__
asm void BreakLine_Vec3Add(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_add f2, f2, f0
    ps_add f3, f3, f1
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void BreakLine_Vec3Add(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

// a - b into out (four floats)
#ifdef __MWERKS__
asm void BreakLine_Vec4Sub(register f32* pA, register f32* pB, register f32* pOut) {
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
void BreakLine_Vec4Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
    pOut[3] = pA[3] - pB[3];
}
#endif

// a - b into out (three floats)
#ifdef __MWERKS__
asm void BreakLine_Vec3Sub(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void BreakLine_Vec3Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif
