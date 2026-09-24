// Code800BA940.c (our name): glows around each view's ball. fn_800BA940 fills the settings when
// the game objects are initialised (the block lbl_80281518 points at), fn_800BAB80 moves and
// grows the glows each frame and fn_800BAA50 queues them for drawing. Its own constants are
// 0x80284248..0x80284278.

#include "game_types.h"
#include "engine.h"
#include "camera.h"
#include "unsorted/cull.h"
#include "golfer.h"
#include "glows.h"

void fn_800BA940(void);
void fn_800BAA4C(void);
void fn_800BAA50(int nPlayer);
void fn_800BAB80(int nPlayer);
void fn_8009AA18(Player* pPlayer);
void fn_8006DEA8(void);                 // gomainloop.c
void fn_80098938(void);                 // GoShaderObject_Glows_Gc.c: draw the queued glows
void fn_8006DF28(void);                 // gomainloop.c

// The size fT of the way from the start size to the largest.
static inline f32 GlowSize(f32 fT) {
    return fT * (lbl_80281518->fMaxSize - lbl_80281518->fStartSize) + lbl_80281518->fStartSize;
}

void fn_800BA940(void) {
    lbl_80281518->f88 = 0.1f;
    lbl_80281518->f8C = 0.2f;
    lbl_80281518->f90 = 0.1f;
    lbl_80281518->fMaxSize = 300.0f;
    lbl_80281518->fFadeSize = 200.0f;
    lbl_80281518->fGrowth = 4.0f;
    lbl_80281518->fStartSize = 25.0f;
    lbl_80281518->fDistScale = 0.05f;
    // the two glows start half and all the way grown, so they pulse in turn
    lbl_80281518->aSize[0][0] = GlowSize(0.5f);
    lbl_80281518->aSize[0][1] = GlowSize(1.0f);
    lbl_80281518->aSize[1][0] = GlowSize(0.5f);
    lbl_80281518->aSize[1][1] = GlowSize(1.0f);
    lbl_80281518->aColour[0][0] = 1.0f;
    lbl_80281518->aColour[0][1] = 1.0f;
    lbl_80281518->aColour[0][2] = 0.0f;
    lbl_80281518->aColour[0][3] = 0.5f;
    lbl_80281518->aColour[1][0] = 0.0f;
    lbl_80281518->aColour[1][1] = 1.0f;
    lbl_80281518->aColour[1][2] = 1.0f;
    lbl_80281518->aColour[1][3] = 0.5f;
}

void fn_800BAA4C(void) {
}

// Queue the player's two glows, unless the game is paused.
void fn_800BAA50(int nPlayer) {
    Player* pPlayer;
    u32 uColour;
    f32 fScale;
    f32 fSize;
    int nView;
    int i;

    fn_80008370(fn_8001614C());
    if (gSession.nPaused == 0 && lbl_80281F80 != NULL) {
        pPlayer = &gPlayers[nPlayer];
        nView = pPlayer->nView[0];
        fn_8009AA18(pPlayer);
        for (i = 0; i < 2; i++) {
            fScale = lbl_80281518->aGlow[nView][i].fScale;
            fSize = 0.005f * lbl_80281518->aSize[nView][i] * fScale;
            uColour = (lbl_80281518->aRGBA[nView][i][3] << 24) |
                      (lbl_80281518->aRGBA[nView][i][0] << 16) |
                      (lbl_80281518->aRGBA[nView][i][1] << 8) | lbl_80281518->aRGBA[nView][i][2];
            fn_8009B260(lbl_80281518->aGlow[nView][i].vPos, uColour, uColour, 3, 0, 0,
                        0.1f * fScale + fSize, fSize, 0.0f);
        }
        fn_8006DEA8();
        fn_80098938();
        fn_8006DF28();
    }
}

// Move the player's glows to the ball, set their colour and fade, and grow them.
void fn_800BAB80(int nPlayer) {
    CamLens* pLens;
    Ball* pBall;
    f32 fScale;
    f32 fAlpha;
    int nView;
    int i;

    pLens = fn_80008370(fn_8001614C());
    pBall = &gPlayers[nPlayer].ball;
    nView = gPlayers[nPlayer].nView[0];
    fScale = lbl_80281518->fDistScale * Vec_Distance(pLens->m4[3], pBall->vPos);
    for (i = 0; i < 2; i++) {
        Vec3Copy(pBall->vPos, lbl_80281518->aGlow[nView][i].vPos);
        lbl_80281518->aGlow[nView][i].fScale = fScale;
        if (lbl_80281518->aSize[nView][i] < lbl_80281518->fFadeSize) {
            fAlpha = 128.0f;
        } else {
            fAlpha = 128.0f * (1.0f - (lbl_80281518->aSize[nView][i] - lbl_80281518->fFadeSize) /
                                          (lbl_80281518->fMaxSize - lbl_80281518->fFadeSize));
        }
        lbl_80281518->aRGBA[nView][i][0] = 255.0f * lbl_80281518->aColour[nView][0];
        lbl_80281518->aRGBA[nView][i][1] = 255.0f * lbl_80281518->aColour[nView][1];
        lbl_80281518->aRGBA[nView][i][2] = 255.0f * lbl_80281518->aColour[nView][2];
        lbl_80281518->aRGBA[nView][i][3] = fAlpha;
        lbl_80281518->aSize[nView][i] = lbl_80281518->aSize[nView][i] + lbl_80281518->fGrowth;
        if (lbl_80281518->aSize[nView][i] > lbl_80281518->fMaxSize) {
            lbl_80281518->aSize[nView][i] = lbl_80281518->fStartSize;
        }
    }
}
