// GoBreakLine.c (EA's name, from its asserts): the putt's break line, a line on the green from the
// ball that shows how the putt will break (BreakLine, breakline.h).

#include "golfer.h"
#include "game.h"
#include "camera.h"
#include "breakline.h"

BreakLine* lbl_80282228;
u8 lbl_8028222C;

void fn_80036054(void* pMesh, int n, s32* pDesc);    // Skin.c: sets up a mesh object
void fn_800360A0(void* pMesh);         // Skin.c: frees a mesh object
void fn_800360D4(u8* pMesh);           // Skin.c
void fn_800C9310(f32* pA, f32* pB, f32* pOut);
void fn_800C9334(f32* pA, f32* pB, f32* pOut);
void fn_800C9358(f32* pA, f32* pB, f32* pOut);
void fn_8003519C(int nRow, void* pData);   // GoTerrain.c: calls row nRow's function with pData

void BreakLine_InitModule(void) {
    lbl_80282228 = fn_80009B34(sizeof(BreakLine), 2, 16, "GoBreakLine.c", 93);
    lbl_80282228->fAB30 = 27.0f;
    lbl_80282228->fAB34 = 0.0f;
    lbl_80282228->fAB38 = 20.0f;
    lbl_80282228->fAB3C = 0.009f;
}

void fn_800C8108(void) {
    fn_80009E70(lbl_80282228);
    lbl_80282228 = NULL;
}

// Sets the line up for a hole: its settings, a mesh per view, the "brkline" texture and the pin.
void fn_800C8134(void) {
    CourseInfo* pCourse = fn_8000C594();
    int nPin = Game_CurrentPinSet();
    s32 desc[2];
    u64 uHash;

    lbl_80282228->fAAE0 = 0.02f;
    lbl_80282228->fAAE4 = 4.0f;
    lbl_80282228->nAAEC = 5;
    lbl_80282228->fAB0C = 0.01f;
    lbl_80282228->fAB10 = -800.0f;
    lbl_80282228->fAB14 = 0.0f;
    lbl_80282228->fAB18 = 100.0f;
    lbl_80282228->fAB1C = 0.024f;
    lbl_80282228->anColor[0] = 0x80;
    lbl_80282228->anColor[1] = 0x80;
    lbl_80282228->anColor[2] = 0x80;
    lbl_80282228->anColor[3] = 0x33;
    desc[0] = gSession.nSplitScreen ? 450 : 900;
    desc[1] = 1;
    fn_80036054(lbl_80282228->aMesh[0], 5, desc);
    if (gSession.nSplitScreen) {
        fn_80036054(lbl_80282228->aMesh[1], 5, desc);
    }
    uHash = fn_8000BEE4("brkline");
    fn_800102DC(uHash, &lbl_80282228->pBank, &lbl_80282228->pTex);
    Vec_Copy(&pCourse->pin[nPin].x, lbl_80282228->vPin);
    lbl_80282228->abSkip[0] = 1;
    lbl_80282228->anVerts[0] = 0;
    if (gSession.nSplitScreen) {
        lbl_80282228->abSkip[1] = 1;
        lbl_80282228->anVerts[1] = 0;
    }
    lbl_8028222C = 0;
}

void fn_800C830C(void) {
    fn_800360A0(lbl_80282228->aMesh[0]);
    if (gSession.nSplitScreen) {
        fn_800360A0(lbl_80282228->aMesh[1]);
    }
    lbl_8028222C = 0;
}

// Steps view nView's line (BreakLine_Render) while the line is on (lbl_8028222C) and its player, a
// human, stands over a putt within 75 of the hole with the target (vTarget) within an inch of the
// pin (EA's test; distances in yards). A view's first call after the line is set up only clears
// its abSkip.
void BreakLine_Update(int nView) {
    int nPlayer = fn_8001707C(nView);
    f32 fDist;

    fDist = Vec_Distance(lbl_80282228->vPin, gPlayers[nPlayer].vTarget);
    fDist *= 36.0f;                     // yards to inches
    if (fDist <= 1.0f &&
        gPlayers[nPlayer].nShotKind == 0 && gPlayers[nPlayer].swing.nState == 0 &&
        gPlayers[nPlayer].fDistance < 75.0f && !Player_IsCPU(nPlayer) && lbl_8028222C) {
        if (!lbl_80282228->abSkip[nView]) {
            BreakLine_Render(nView);
            return;
        }
        lbl_80282228->abSkip[nView] = 0;
    }
}

// Draws view nView's line, first growing it by one step: while the ball copy rolls, step it, lay
// the next pair of vertices across its path, fade the line's alpha from fAB30 at the ball to fAB34
// at the end, and trigger events 0x28 / 0x29 when it stops or starts to move away from the pin.
void BreakLine_Render(int nView) {
    f32 vAxis[4] = {0.0f, 0.0f, 1.0f, 0.0f};
    int nPlayer = fn_8001707C(nView);
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
        Vec_Distance(lbl_80282228->vPin, gPlayers[nPlayer].vTarget);
        fn_80016B9C();
        fn_80035118(4, 5);
        fn_80012F50(0, 6, 0x80);
        fn_80012F34(0);
        fn_8005CC64(lbl_80282228->pBank, lbl_80282228->pTex);
        fn_80014118(0x70);
        fn_80035138(0);
        fn_80012EF8();
        fDist = Vec_Distance(lbl_80282228->aBall[nView].vPos, lbl_80282228->vPin);
        if (lbl_80282228->abA91C[nView]) {
            if ((lbl_80282228->aBall[nView].nState == 2 || lbl_80282228->aBall[nView].nState == 3 ||
                 lbl_80282228->aBall[nView].nState == 4) && fDist > 0.001f) {
                fn_80050D24_SetSimulating(1);
                fn_8005585C_SimForTime(&lbl_80282228->aBall[nView], lbl_80282228->fAAE0, lbl_80282228->fAAE4);
                fn_80050D24_SetSimulating(0);
                fDist = fn_800BB028(lbl_80282228->aBall[nView].vPos,
                                    &lbl_80282228->aBall[nView].pCourse->pin[Game_CurrentPinSet()].x);
                if (fDist < lbl_80282228->afAAD4[nView]) {
                    lbl_80282228->afAAD4[nView] = fDist;
                    Vec_Copy(lbl_80282228->aBall[nView].vPos, lbl_80282228->aViewPoint[nView]);
                } else if (!lbl_80282228->abAADC[nView]) {
                    EVENT_Trigger(nPlayer, 0x29, lbl_80282228->aBall[nView].vPrev, 0);
                    lbl_80282228->abAADC[nView] = 1;
                }
                // The ball's heading on the ground, as an angle about y from the z axis.
                fn_800C9334(lbl_80282228->aBall[nView].vPos, lbl_80282228->aBall[nView].vPrev, vDir);
                vDir[1] = 0.0f;
                // EA bug: tests y, just cleared, where z was surely meant
                if (vDir[0] != 0.0f || vDir[1] != 0.0f) {
                    fn_800BAF04(vDir, vDir);
                }
                fAngle = fn_80009614(fn_8000C5FC(vDir, vAxis));
                vec4flt_CrossProduct(vDir, vAxis, vCross);
                fAngle = fAngle * (vCross[1] < 0.0f ? -1.0f : 1.0f);
                fSin = fn_800095F0(fAngle);
                fCos = fn_80009638(fAngle);
                // Turn the new pair of vertices to the heading and move them to the ball.
                for (i = lbl_80282228->anVerts[nView]; i <= lbl_80282228->anVerts[nView] + 1; i++) {
                    fX = lbl_80282228->aVert[nView][i][0];
                    fZ = lbl_80282228->aVert[nView][i][2];
                    lbl_80282228->aVert[nView][i][0] = fX * -fSin + fZ * fCos;
                    lbl_80282228->aVert[nView][i][2] = fX * fCos + fZ * fSin;
                }
                fn_800C9310(lbl_80282228->aVert[nView][lbl_80282228->anVerts[nView]],
                            lbl_80282228->aBall[nView].vPos,
                            lbl_80282228->aVert[nView][lbl_80282228->anVerts[nView]]);
                fn_800C9310(lbl_80282228->aVert[nView][lbl_80282228->anVerts[nView] + 1],
                            lbl_80282228->aBall[nView].vPos,
                            lbl_80282228->aVert[nView][lbl_80282228->anVerts[nView] + 1]);
                fAlpha = lbl_80282228->fAB30;
                if (fAlpha < 0.0f) {
                    fAlpha = 0.0f;
                }
                if (lbl_80282228->anVerts[nView] != 0) {
                    for (i = 0; i <= lbl_80282228->anVerts[nView]; i += 2) {
                        lbl_80282228->aColor[nView][i][3] = fAlpha - (f32)i *
                            ((fAlpha - lbl_80282228->fAB34) / (f32)lbl_80282228->anVerts[nView]);
                        lbl_80282228->aColor[nView][i + 1][3] = fAlpha - (f32)i *
                            ((fAlpha - lbl_80282228->fAB34) / (f32)lbl_80282228->anVerts[nView]);
                    }
                }
                lbl_80282228->anVerts[nView] += 2;
                lbl_80282228->anAAF0[nView]++;
                if (lbl_80282228->anAAF0[nView] == lbl_80282228->nAAEC + 1) {
                    Vec3Copy(lbl_80282228->aVert[nView][lbl_80282228->anVerts[nView] - 2],
                             lbl_80282228->aVert[nView][lbl_80282228->anVerts[nView]]);
                    Vec3Copy(lbl_80282228->aVert[nView][lbl_80282228->anVerts[nView] - 1],
                             lbl_80282228->aVert[nView][lbl_80282228->anVerts[nView] + 1]);
                    lbl_80282228->anVerts[nView] += 2;
                }
                if (lbl_80282228->anAAF0[nView] > lbl_80282228->nAAEC) {
                    lbl_80282228->anAAF0[nView] = 1;
                }
                // EA bug: a misplaced parenthesis compares the split-screen test's truth value with
                // 450, so in split screen the line is never stopped at its length
                if ((!gSession.nSplitScreen && lbl_80282228->anVerts[nView] + 2 >= 900) ||
                    (gSession.nSplitScreen && lbl_80282228->anVerts[nView] + 2) >= 450) {
                    lbl_80282228->abA91C[nView] = 0;
                }
            } else {
                EVENT_Trigger(nPlayer, 0x28, lbl_80282228->aBall[nView].vPos, 0);
                lbl_80282228->abA91C[nView] = 0;
                if (!lbl_80282228->abAADC[nView]) {
                    EVENT_Trigger(nPlayer, 0x29, lbl_80282228->aBall[nView].vPos, 0);
                    lbl_80282228->abAADC[nView] = 1;
                }
            }
        }
        if (lbl_80282228->anVerts[nView] > 2) {
            for (i = 0; i < lbl_80282228->anVerts[nView]; i++) {
                aIndex[i] = i;
            }
            desc.desc.n0 = lbl_80282228->anVerts[nView];
            desc.desc.nVerts = lbl_80282228->anVerts[nView];
            desc.desc.pDraw = NULL;
            desc.desc.pIndices = aIndex;
            desc.desc.pPos = lbl_80282228->aVert[nView][0];
            desc.desc.pColour = lbl_80282228->aColor[nView][0];
            desc.desc.pUV = lbl_80282228->aUV[nView][0];
            desc.af18[0] = lbl_80282228->fAB10;
            desc.af18[1] = lbl_80282228->fAB14;
            desc.af18[2] = lbl_80282228->fAB18;
            desc.af18[3] = 1.0f / lbl_80282228->fAB18;
            nFrame = gSession.nFrameCount;
            fn_8003519C(5, &nFrame);
            fn_80036100((ShaderObject*)lbl_80282228->aMesh[nView], &desc, 1);
            fn_800360D4(lbl_80282228->aMesh[nView]);
        }
        fn_80012F50(1, 6, 0x80);
        fn_80012F34(1);
        fn_80012EF8();
    }
}

// Starts view nView's line when its player stands over a putt within 75 of the hole: lays out the
// line's vertices, colours and texture coordinates, and launches a copy of the ball with the
// putt's power for the distance, with sounds and effects off.
void BreakLine_Reset(int nView) {
    Player* pPlayer;
    f32 fPower;
    int i;
    int nTex = 0;

    pPlayer = &gPlayers[fn_8001707C(nView)];
    if (pPlayer->nShotKind == 0 && pPlayer->fDistance < 75.0f && pPlayer->fDistance > 0.0f) {
        lbl_80282228->abA91C[nView] = 1;
        lbl_80282228->abSkip[nView] = 1;
        lbl_80282228->anVerts[nView] = 0;
        lbl_8028222C = 1;
        Mem_cpy(&lbl_80282228->aBall[nView], &pPlayer->ball, sizeof(Ball));
        Vec_Copy(lbl_80282228->aBall[nView].vPos, lbl_80282228->aViewPoint[nView]);
        lbl_80282228->afAAD4[nView] = 1000000.0f;
        lbl_80282228->abAADC[nView] = 0;
        fPower = fn_80050D34(pPlayer->fDistance);
        lbl_80282228->anAAF0[nView] = 0;
        for (i = 0; i < BREAKLINE_POINTS; i++) {
            lbl_80282228->aVert[nView][i * 2][0] = 0.0f;
            lbl_80282228->aVert[nView][i * 2][1] = 0.0f;
            lbl_80282228->aVert[nView][i * 2][2] = lbl_80282228->fAB1C;
            lbl_80282228->aVert[nView][i * 2 + 1][0] = 0.0f;
            lbl_80282228->aVert[nView][i * 2 + 1][1] = 0.0f;
            lbl_80282228->aVert[nView][i * 2 + 1][2] = -lbl_80282228->fAB1C;
            lbl_80282228->aColor[nView][i * 2][0] = lbl_80282228->anColor[0];
            lbl_80282228->aColor[nView][i * 2][1] = lbl_80282228->anColor[1];
            lbl_80282228->aColor[nView][i * 2][2] = lbl_80282228->anColor[2];
            lbl_80282228->aColor[nView][i * 2][3] = lbl_80282228->anColor[3];
            lbl_80282228->aColor[nView][i * 2 + 1][0] = lbl_80282228->anColor[0];
            lbl_80282228->aColor[nView][i * 2 + 1][1] = lbl_80282228->anColor[1];
            lbl_80282228->aColor[nView][i * 2 + 1][2] = lbl_80282228->anColor[2];
            lbl_80282228->aColor[nView][i * 2 + 1][3] = lbl_80282228->anColor[3];
            lbl_80282228->aUV[nView][i * 2][0] = (f32)nTex / (f32)lbl_80282228->nAAEC;
            lbl_80282228->aUV[nView][i * 2][1] = 0.0f;
            lbl_80282228->aUV[nView][i * 2 + 1][0] = (f32)nTex / (f32)lbl_80282228->nAAEC;
            lbl_80282228->aUV[nView][i * 2 + 1][1] = 1.0f;
            nTex++;
            if (nTex > lbl_80282228->nAAEC) {
                nTex = 0;
            }
        }
        fn_80050D24_SetSimulating(1);
        lbl_80282228->aBall[nView].nState = 0;
        Physics_ShotImpact(&lbl_80282228->aBall[nView], pPlayer->nClub, pPlayer->nShotKind, fPower,
                    pPlayer->fAim, 1, pPlayer->vLaunchA, pPlayer->vLaunchB);
        fn_80050D24_SetSimulating(0);
    }
}

// View nView's point.
void fn_800C8C3C(int nView, f32* pOut) {
    Vec_Copy(lbl_80282228->aViewPoint[nView], pOut);
}

// The caddie's putt read for view nView, in feet: how far past (+) or short of the hole the aim
// point lies along the line from the ball, and how far to the side of that line (the sign gives
// the side). 0, 0 in split screen or when the points coincide; -999 when there is no tip, 999
// when the caddie gave up.
void fn_800C9038(int nView, f32* pLong, f32* pSide) {
    int nPlayer = fn_8001707C(nView);
    int nPin = Game_CurrentPinSet();
    CourseInfo* pCourse = fn_8000C594();
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
    Vec_Copy(&pCourse->pin[nPin].x, vPin);
    Vec_Copy(vPin, vHole);
    vHole[1] = 0.0f;
    Vec_Copy(gPlayers[nPlayer].vBall, vBall);
    vBall[1] = 0.0f;
    Vec_Copy(vTip, vAim);
    vAim[1] = 0.0f;
    if (vAim[0] == vHole[0] && vAim[2] == vHole[2]) {
        *pLong = 0.0f;
        *pSide = 0.0f;
        return;
    }
    fHoleDist = Vec_Distance(vHole, vBall);
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
    fAimDist = Vec_Distance(vAim, vBall);
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
    fn_800C9358(vHole, vBall, vToHole);
    fn_800BAF04(vToHole, vToHole);
    fn_800C9358(vAim, vBall, vToAim);
    fn_800BAF04(vToAim, vToAim);
    fDot = fn_8000C5FC(vToHole, vToAim);
    if (0.0f == fDot) {
        *pLong = 0.0f;
        *pSide = 0.0f;
        return;
    }
    fAlong = fDot * fAimDist;
    fSide = fAimDist * fn_800095F0(fn_80009614(fDot));
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
asm void fn_800C9310(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_800C9310(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

// a - b into out (four floats)
#ifdef __MWERKS__
asm void fn_800C9334(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_800C9334(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
    pOut[3] = pA[3] - pB[3];
}
#endif

// a - b into out (three floats)
#ifdef __MWERKS__
asm void fn_800C9358(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_800C9358(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif
