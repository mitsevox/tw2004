// GoComicCam.c (EA's name, from its asserts; also in EA's 2002 source tree): the comic-book camera,
// which shows the shot in panels on screen, one after another (the state at lbl_80282178,
// comiccam.h). Layout 0 is three panels side by side, layout 1 a 3x3 grid filled in a random order.

#include "golfer.h"
#include "comiccam.h"

void fn_800352BC(void);
void fn_80038624(f32* pColour);
void fn_800A6AC8(u8 nPlayer, u8 n);     // GameAudio.c

void fn_800B39B8(ComicPanel* pPanel, f32* pRect, int nPlayer, f32 fFrameTime);
u8   fn_800B3C64(ComicPanel* pPanel, int nPlayer);
void fn_800B3D64(ComicPanel* pPanel, View* pView, f32 fFrameTime);
void fn_800B3D68(ComicPanel* pPanel, f32* pRect, f32 fFrameTime);
void fn_800B3F4C(f32* pRect, f32 fTop, f32 fLeft, f32 fWidth, f32 fHeight);
void fn_800B3F9C(void);
void fn_800B4108(void);
u8   fn_800B4818(f32* pRect, int nPlayer);
u8   fn_800B4908(void);
void fn_800B4914(View* pView, int nPlayer);

void fn_800B34F0(void) {
    lbl_80282178 = fn_80009B34(sizeof(ComicCam), 2, 0, "GoComicCam.c", 91);
}

void fn_800B352C(void) {
    fn_80009E70(lbl_80282178);
}

// Start the comic camera for nPlayer's view with layout nKind.
void fn_800B3550(int nKind, View* pView, int nPlayer) {
    f32* pCam = fn_8001731C(pView);
    f32* pSub = fn_80017314(pView);
    int i;
    int nShot;
    CamShot* pShot;

    for (i = 0; i < 10; i++) {
        lbl_80282178->a24[i] = lbl_80282178->a4C[i] = -1.0f;
    }
    // EA bug: nShot is left unset for any other nKind (GoGolfCam.c only passes 0 and 1).
    switch (nKind) {
    case 0:
        fn_800B3F9C();
        nShot = 0x28;
        break;
    case 1:
        fn_800B4108();
        nShot = 0x2B;
        break;
    }
    lbl_80282178->bNext = 0;
    lbl_80282178->n8 = 0;
    lbl_80282178->nKind = nKind;
    lbl_80282178->bDone = 0;
    lbl_80282178->nPlayer = nPlayer;
    pShot = DynamicCam_ChooseScript(nPlayer, nShot, pView->script.pShot);
    if (pShot != NULL && !CameraScript_WillGolferBeOccludedInThisView(nPlayer, pShot, &pView->script)) {
        CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f, 0x19,
                                       0.0f);
    }
}

// A frame of the comic camera for nPlayer's view: draw the panels, and when the current one has run
// its course move on to the next. Returns 1 when the camera is finished.
u8 fn_800B36F4(View* pView, int nPlayer, f32 fFrameTime) {
    f32 aColour[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
    f32* pRect;
    ComicPanel* pPanel = &lbl_80282178->aPanel[lbl_80282178->nPanel];

    pRect = fn_80012EF0(fn_80017004(gPlayers[nPlayer].nView[0]));
    if (gSession.nPaused != 0) {
        lbl_80282178->n8 = -2;
    }
    if (lbl_80282178->n8 < 2) {
        fn_800B3F4C(pRect, 0.0f, 0.0f, 1.0f, 1.0f);
        fn_80038624(aColour);
        if (gSession.nPaused == 0) {
            fn_800B3F4C(pRect, pPanel->fTop, pPanel->fLeft, pPanel->fWidth, pPanel->fHeight);
        }
        lbl_80282178->n8 = lbl_80282178->n8 + 1;
    }
    if (lbl_80282178->bNext && gSession.nPaused == 0) {
        lbl_80282178->n10++;
        fn_800B3D68(pPanel, pRect, fFrameTime);
        if (lbl_80282178->n10 > 2) {
            lbl_80282178->bNext = 0;
            fn_800B3F4C(pRect, pPanel->fTop, pPanel->fLeft, pPanel->fWidth, pPanel->fHeight);
            fn_800B4914(pView, nPlayer);
        }
    } else if (lbl_80282178->bNext && gSession.nPaused != 0) {
        lbl_80282178->n10 = -2;
    }
    if (!lbl_80282178->bNext) {
        fn_800B39B8(pPanel, pRect, nPlayer, fFrameTime);
        fn_800B3D64(pPanel, pView, fFrameTime);
        fn_800B3D68(pPanel, pRect, fFrameTime);
        lbl_80282178->fTime = lbl_80282178->fTime + fFrameTime;
        if (fn_800B3C64(pPanel, nPlayer)) {
            lbl_80282178->a4C[lbl_80282178->nPanel] = pPanel->fTime;
            lbl_80282178->a24[lbl_80282178->nPanel] = pPanel->fTime;
            if (pPanel->nNext >= 0) {
                lbl_80282178->bNext = 1;
                lbl_80282178->nPanel = pPanel->nNext;
                lbl_80282178->fTime = 0.0f;
                lbl_80282178->n10 = 0;
                lbl_80282178->nShown = lbl_80282178->nShown + 1;
            } else {
                lbl_80282178->bDone = 1;
            }
        }
        if (fn_800B4818(pRect, nPlayer)) {
            return 1;
        }
    }
    return 0;
}

// Layout 1's ending: once the last panel is reached, grow it a little each frame until it fills the
// screen: its shorter side (both when square), from the centre when centred, else away from the
// nearer screen edge. nPlayer is not used.
void fn_800B39B8(ComicPanel* pPanel, f32* pRect, int nPlayer, f32 fFrameTime) {
    f32 fX;
    f32 fY;
    f32 fHeight;
    f32 fWidth;
    f32 fFrames;
    f32 fRight;
    f32 fBottom;
    u8 bGrowX;
    u8 bGrowY;

    fFrames = FRAME_RATE * fFrameTime;
    switch (lbl_80282178->nKind) {
    case 0:
        break;
    case 1:
        if ((pPanel->nNext == -1 ||
             (lbl_80282178->nShown >= lbl_80282178->nPanels - 1 && 0.0f == lbl_80282178->fTime)) &&
            fFrameTime > 0.0f) {
            pPanel->nNext = -1;
            fX = pRect[0];
            fWidth = pRect[2];
            fHeight = pRect[3];
            fY = pRect[1];
            fRight = 1.0f - (fX + fWidth);
            fBottom = 1.0f - (fY + fHeight);
            if (fabsf(fWidth - fHeight) < 0.001f) {
                bGrowY = 1;
                bGrowX = 1;
            } else if (fWidth < fHeight) {
                bGrowX = 1;
                bGrowY = 0;
            } else {
                bGrowX = 0;
                bGrowY = 1;
            }
            if (bGrowX) {
                if (fabsf(fX - fRight) < 0.001f) {
                    fWidth += 0.1f * fFrames;
                    fX -= 0.05f * fFrames;
                } else if (fX < fRight) {
                    fWidth += 0.05f * fFrames;
                } else {
                    fWidth += 0.05f * fFrames;
                    fX -= 0.05f * fFrames;
                }
            }
            if (bGrowY) {
                if (fabsf(fY - fBottom) < 0.001f) {
                    fHeight += 0.1f * fFrames;
                    fY -= 0.05f * fFrames;
                } else if (fY < fBottom) {
                    fHeight += 0.05f * fFrames;
                } else {
                    fY -= 0.05f * fFrames;
                    fHeight += 0.05f * fFrames;
                }
            }
            fX = fX < 0.0f ? 0.0f : (fX > 1.0f ? 1.0f : fX);
            fY = fY < 0.0f ? 0.0f : (fY > 1.0f ? 1.0f : fY);
            fHeight = fHeight < 0.0f ? 0.0f : (fHeight > 1.0f - fY ? 1.0f - fY : fHeight);
            fWidth = fWidth < 0.0f ? 0.0f : (fWidth > 1.0f - fX ? 1.0f - fX : fWidth);
            fn_800B3F4C(pRect, fY, fX, fWidth, fHeight);
        }
        break;
    }
}

// Has the panel run its course? Kinds 0 and 1 wait for the golfer's animation events 1 and 2, kind
// 2 for the panel's time.
u8 fn_800B3C64(ComicPanel* pPanel, int nPlayer) {
    u8 bBefore;

    switch (pPanel->n0) {
    case 0:
        bBefore = !(fn_8005CB78(gPlayers[nPlayer].pChar, 1) < gPlayers[nPlayer].pChar->fAnimTime);
        if (!bBefore) {
            return 1;
        }
        return 0;
    case 1:
        bBefore = !(fn_8005CB78(gPlayers[nPlayer].pChar, 2) < gPlayers[nPlayer].pChar->fAnimTime);
        if (!bBefore) {
            return 1;
        }
        return 0;
    case 2:
        return lbl_80282178->fTime >= pPanel->f4;
    default:
        return 0;
    }
}

// Does nothing; its only caller, fn_800B36F4, passes these.
void fn_800B3D64(ComicPanel* pPanel, View* pView, f32 fFrameTime) {
}

// Draw the panels still showing, each darkened as its time runs out, then put the render camera's
// rectangle pRect back. pPanel is not used.
void fn_800B3D68(ComicPanel* pPanel, f32* pRect, f32 fFrameTime) {
    f32 aColour[4] = { 0.0f, 0.0f, 0.0f, 0.5f };
    f32 fY;
    f32 fX;
    f32 fHeight;
    f32 fWidth;
    int i;

    fX = pRect[0];
    fY = pRect[1];
    fHeight = pRect[3];
    fWidth = pRect[2];
    for (i = 0; i < 10; i++) {
        if (0.0f < lbl_80282178->a24[i]) {
            fn_800B3F4C(pRect, lbl_80282178->aPanel[i].fTop, lbl_80282178->aPanel[i].fLeft,
                        lbl_80282178->aPanel[i].fWidth, lbl_80282178->aPanel[i].fHeight);
            aColour[3] = 0.5f * (1.0f - lbl_80282178->a24[i] / lbl_80282178->a4C[i]);
            aColour[3] = aColour[3] < 0.0f ? 0.0f : (aColour[3] > 0.5f ? 0.5f : aColour[3]);
            lbl_80282178->a24[i] = lbl_80282178->a24[i] - fFrameTime;
            fn_80038624(aColour);
            if (lbl_80282178->bNext && lbl_80282178->a24[i] <= 0.0f) {
                lbl_80282178->a24[i] = lbl_80282178->a24[i] + fFrameTime;
            }
        }
    }
    fn_800B3F4C(pRect, fY, fX, fWidth, fHeight);
}

// Set the render camera's screen rectangle pRect and bring the camera up to date.
void fn_800B3F4C(f32* pRect, f32 fTop, f32 fLeft, f32 fWidth, f32 fHeight) {
    fn_800171D8(pRect, fLeft, fTop, fWidth, fHeight);
    fn_800352BC();
    fn_80035240(NULL);
    fn_80013CCC(fn_8001614C());
    fn_80013EEC(fn_8001614C());
    fn_80016B9C();
    fn_80012EF8();
}

// Layout 0: three panels side by side.
void fn_800B3F9C(void) {
    lbl_80282178->aPanel[0].fTop = 0.15f;
    lbl_80282178->aPanel[0].fLeft = 0.03f;
    lbl_80282178->aPanel[0].fWidth = 0.28f;
    lbl_80282178->aPanel[0].fHeight = 0.7f;
    lbl_80282178->aPanel[0].n0 = 0;
    lbl_80282178->aPanel[0].f1C = 0.5f;
    lbl_80282178->aPanel[0].n24 = 0;
    lbl_80282178->aPanel[0].nNext = 1;
    lbl_80282178->aPanel[0].fTime = -1.0f;
    lbl_80282178->aPanel[1].fTop = 0.15f;
    lbl_80282178->aPanel[1].fLeft = 0.36f;
    lbl_80282178->aPanel[1].fWidth = 0.28f;
    lbl_80282178->aPanel[1].fHeight = 0.7f;
    lbl_80282178->aPanel[1].f1C = 0.1f;
    lbl_80282178->aPanel[1].n0 = 1;
    lbl_80282178->aPanel[1].n24 = 0;
    lbl_80282178->aPanel[1].nNext = 2;
    lbl_80282178->aPanel[1].fTime = -1.0f;
    lbl_80282178->aPanel[2].fTop = 0.15f;
    lbl_80282178->aPanel[2].fLeft = 0.69f;
    lbl_80282178->aPanel[2].fWidth = 0.28f;
    lbl_80282178->aPanel[2].fHeight = 0.7f;
    lbl_80282178->aPanel[2].n0 = 2;
    lbl_80282178->aPanel[2].f4 = 1.0f;
    lbl_80282178->aPanel[2].f1C = 0.2f;
    lbl_80282178->aPanel[2].n24 = 0;
    lbl_80282178->aPanel[2].nNext = -1;
    lbl_80282178->aPanel[2].fTime = -1.0f;
    lbl_80282178->nPanel = 0;
    lbl_80282178->fTime = 0.0f;
    lbl_80282178->nShown = 0;
    lbl_80282178->nPanels = 3;
    fn_800A6AC8(0, 0);
}

// Layout 1: a 3x3 grid of small panels, shown in one of four orders picked at random; each panel
// stays 3 frames. Half the time the grid is run through twice (18 panels).
void fn_800B4108(void) {
    s32 aOrder[4][9] = {
        { 3, 2, 6, 0, 8, 7, 4, 1, 5 },
        { 7, 3, 1, 0, 8, 2, 6, 5, 4 },
        { 1, 0, 4, 6, 8, 5, 3, 2, 7 },
        { 0, 1, 2, 7, 8, 3, 6, 5, 4 },
    };
    int nOrder = Misc_RandFunc(1) & 3;

    lbl_80282178->aPanel[aOrder[nOrder][0]].fTop = 0.03f;
    lbl_80282178->aPanel[aOrder[nOrder][0]].fLeft = 0.03f;
    lbl_80282178->aPanel[aOrder[nOrder][0]].fWidth = 0.28f;
    lbl_80282178->aPanel[aOrder[nOrder][0]].fHeight = 0.28f;
    lbl_80282178->aPanel[aOrder[nOrder][0]].n0 = 2;
    lbl_80282178->aPanel[aOrder[nOrder][0]].f4 = 3.0f / FRAME_RATE;
    lbl_80282178->aPanel[aOrder[nOrder][0]].n24 = 1;
    lbl_80282178->aPanel[0].nNext = 1;
    lbl_80282178->aPanel[aOrder[nOrder][0]].fTime = 2.0f;
    lbl_80282178->aPanel[aOrder[nOrder][1]].fTop = 0.03f;
    lbl_80282178->aPanel[aOrder[nOrder][1]].fLeft = 0.36f;
    lbl_80282178->aPanel[aOrder[nOrder][1]].fWidth = 0.28f;
    lbl_80282178->aPanel[aOrder[nOrder][1]].fHeight = 0.28f;
    lbl_80282178->aPanel[aOrder[nOrder][1]].n0 = 2;
    lbl_80282178->aPanel[aOrder[nOrder][1]].f4 = 3.0f / FRAME_RATE;
    lbl_80282178->aPanel[aOrder[nOrder][1]].n24 = 1;
    lbl_80282178->aPanel[1].nNext = 2;
    lbl_80282178->aPanel[aOrder[nOrder][1]].fTime = 2.0f;
    lbl_80282178->aPanel[aOrder[nOrder][2]].fTop = 0.03f;
    lbl_80282178->aPanel[aOrder[nOrder][2]].fLeft = 0.69f;
    lbl_80282178->aPanel[aOrder[nOrder][2]].fWidth = 0.28f;
    lbl_80282178->aPanel[aOrder[nOrder][2]].fHeight = 0.28f;
    lbl_80282178->aPanel[aOrder[nOrder][2]].n0 = 2;
    lbl_80282178->aPanel[aOrder[nOrder][2]].f4 = 3.0f / FRAME_RATE;
    lbl_80282178->aPanel[aOrder[nOrder][2]].n24 = 1;
    lbl_80282178->aPanel[2].nNext = 3;
    lbl_80282178->aPanel[aOrder[nOrder][2]].fTime = 2.0f;
    lbl_80282178->aPanel[aOrder[nOrder][3]].fTop = 0.36f;
    lbl_80282178->aPanel[aOrder[nOrder][3]].fLeft = 0.03f;
    lbl_80282178->aPanel[aOrder[nOrder][3]].fWidth = 0.28f;
    lbl_80282178->aPanel[aOrder[nOrder][3]].fHeight = 0.28f;
    lbl_80282178->aPanel[aOrder[nOrder][3]].n0 = 2;
    lbl_80282178->aPanel[aOrder[nOrder][3]].f4 = 3.0f / FRAME_RATE;
    lbl_80282178->aPanel[aOrder[nOrder][3]].n24 = 1;
    lbl_80282178->aPanel[3].nNext = 4;
    lbl_80282178->aPanel[aOrder[nOrder][3]].fTime = 2.0f;
    lbl_80282178->aPanel[aOrder[nOrder][4]].fTop = 0.36f;
    lbl_80282178->aPanel[aOrder[nOrder][4]].fLeft = 0.36f;
    lbl_80282178->aPanel[aOrder[nOrder][4]].fWidth = 0.28f;
    lbl_80282178->aPanel[aOrder[nOrder][4]].fHeight = 0.28f;
    lbl_80282178->aPanel[aOrder[nOrder][4]].n0 = 2;
    lbl_80282178->aPanel[aOrder[nOrder][4]].f4 = 3.0f / FRAME_RATE;
    lbl_80282178->aPanel[aOrder[nOrder][4]].n24 = 1;
    lbl_80282178->aPanel[4].nNext = 5;
    lbl_80282178->aPanel[aOrder[nOrder][4]].fTime = 2.0f;
    lbl_80282178->aPanel[aOrder[nOrder][5]].fTop = 0.36f;
    lbl_80282178->aPanel[aOrder[nOrder][5]].fLeft = 0.69f;
    lbl_80282178->aPanel[aOrder[nOrder][5]].fWidth = 0.28f;
    lbl_80282178->aPanel[aOrder[nOrder][5]].fHeight = 0.28f;
    lbl_80282178->aPanel[aOrder[nOrder][5]].n0 = 2;
    lbl_80282178->aPanel[aOrder[nOrder][5]].f4 = 3.0f / FRAME_RATE;
    lbl_80282178->aPanel[aOrder[nOrder][5]].n24 = 1;
    lbl_80282178->aPanel[5].nNext = 6;
    lbl_80282178->aPanel[aOrder[nOrder][5]].fTime = 2.0f;
    lbl_80282178->aPanel[aOrder[nOrder][6]].fTop = 0.69f;
    lbl_80282178->aPanel[aOrder[nOrder][6]].fLeft = 0.03f;
    lbl_80282178->aPanel[aOrder[nOrder][6]].fWidth = 0.28f;
    lbl_80282178->aPanel[aOrder[nOrder][6]].fHeight = 0.28f;
    lbl_80282178->aPanel[aOrder[nOrder][6]].n0 = 2;
    lbl_80282178->aPanel[aOrder[nOrder][6]].f4 = 3.0f / FRAME_RATE;
    lbl_80282178->aPanel[aOrder[nOrder][6]].n24 = 1;
    lbl_80282178->aPanel[6].nNext = 7;
    lbl_80282178->aPanel[aOrder[nOrder][6]].fTime = 2.0f;
    lbl_80282178->aPanel[aOrder[nOrder][7]].fTop = 0.69f;
    lbl_80282178->aPanel[aOrder[nOrder][7]].fLeft = 0.36f;
    lbl_80282178->aPanel[aOrder[nOrder][7]].fWidth = 0.28f;
    lbl_80282178->aPanel[aOrder[nOrder][7]].fHeight = 0.28f;
    lbl_80282178->aPanel[aOrder[nOrder][7]].n0 = 2;
    lbl_80282178->aPanel[aOrder[nOrder][7]].f4 = 3.0f / FRAME_RATE;
    lbl_80282178->aPanel[aOrder[nOrder][7]].n24 = 1;
    lbl_80282178->aPanel[7].nNext = 8;
    lbl_80282178->aPanel[aOrder[nOrder][7]].fTime = 2.0f;
    lbl_80282178->aPanel[aOrder[nOrder][8]].fTop = 0.69f;
    lbl_80282178->aPanel[aOrder[nOrder][8]].fLeft = 0.69f;
    lbl_80282178->aPanel[aOrder[nOrder][8]].fWidth = 0.28f;
    lbl_80282178->aPanel[aOrder[nOrder][8]].fHeight = 0.28f;
    lbl_80282178->aPanel[aOrder[nOrder][8]].n0 = 2;
    lbl_80282178->aPanel[aOrder[nOrder][8]].f4 = 3.0f / FRAME_RATE;
    lbl_80282178->aPanel[aOrder[nOrder][8]].n24 = 1;
    lbl_80282178->aPanel[8].nNext = 0;
    lbl_80282178->aPanel[aOrder[nOrder][8]].fTime = 2.0f;
    lbl_80282178->nPanel = 0;
    lbl_80282178->fTime = 0.0f;
    lbl_80282178->nShown = 0;
    if (!(Misc_RandFunc(1) & 1)) {
        lbl_80282178->nPanels = 18;
    } else {
        lbl_80282178->nPanels = 9;
    }
    fn_800A6AC8(0, (int)(lbl_80282178->aPanel[0].fLeft * 3.0f));
}

// Is the comic camera finished? Layout 1: once the golfer is half a second past animation event 2
// and the current panel fills the screen; layout 0: once the last panel has run its course (bDone).
u8 fn_800B4818(f32* pRect, int nPlayer) {
    switch (lbl_80282178->nKind) {
    case 0:
        return lbl_80282178->bDone != 0;
    case 1:
        if (0.5f + fn_8005CB78(gPlayers[nPlayer].pChar, 2) < gPlayers[nPlayer].pChar->fAnimTime) {
            if (0.0f == pRect[0] && 0.0f == pRect[1] && 1.0f == pRect[2] && 1.0f == pRect[3]) {
                return 1;
            }
            return 0;
        }
        return 0;
    default:
        return 0;
    }
}

u8 fn_800B4908(void) {
    return lbl_80282178->bNext;
}

// Move the camera on for the current panel: layout 0 to shot 0x28 + the panel, layout 1 to the next
// of shots 0x2B..0x2D every 9 panels shown. Then hand fn_800A6AC8 (GameAudio.c) the panel's column
// (3 for layout 1's last panel).
void fn_800B4914(View* pView, int nPlayer) {
    f32* pCam = fn_8001731C(pView);
    f32* pSub = fn_80017314(pView);
    int nPanel = lbl_80282178->nPanel;
    ComicPanel* pPanel = &lbl_80282178->aPanel[nPanel];
    u8 nColumn = (int)(pPanel->fLeft * 3.0f);
    CamShot* pShot;
    int nShot;

    switch (lbl_80282178->nKind) {
    case 0:
        pShot = DynamicCam_ChooseScript(nPlayer, nPanel + 0x28, pView->script.pShot);
        if (pShot != NULL && !CameraScript_WillGolferBeOccludedInThisView(nPlayer, pShot, &pView->script)) {
            CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f, 100.0f,
                                           0x19, 0.0f);
        }
        fn_800A6AC8(nPlayer, nColumn);
        return;
    case 1:
        if (lbl_80282178->nShown % 9 == 0) {
            pShot = pView->script.pShot;
            if (pShot != NULL) {
                nShot = pShot->bAD + 1;
                if (nShot > 0x2D) {
                    nShot = 0x2B;
                }
                pShot = DynamicCam_ChooseScript(nPlayer, nShot, pShot);
                if (pShot != NULL &&
                    !CameraScript_WillGolferBeOccludedInThisView(nPlayer, pShot, &pView->script)) {
                    CameraScript_InterpToNewScript(&pView->script, pShot, nPlayer, pCam, pSub, 5, 0.0f,
                                                   100.0f, 0x19, 0.0f);
                }
            }
        }
        if (lbl_80282178->nShown >= lbl_80282178->nPanels - 1 && 0.0f == lbl_80282178->fTime) {
            nColumn = 3;
        }
        fn_800A6AC8(nPlayer, nColumn);
        return;
    }
}

// Is the golfer past animation event 2? (98%: the original returns the flag without a u8 mask, but
// GameEffects.c's call masks it as a u8; an int return is exact here but breaks that caller.)
u8 fn_800B4AE0(void) {
    u8 bBefore = !(fn_8005CB78(gPlayers[lbl_80282178->nPlayer].pChar, 2) <
                   gPlayers[lbl_80282178->nPlayer].pChar->fAnimTime);

    if (!bBefore) {
        return 1;
    }
    return 0;
}
