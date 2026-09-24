// CharSliders.c (EA's name, from its asserts; TW06): the character's body sliders, from
// fn_8010D454 on (their blending, fn_8010DC94..fn_8010E4DC, sits in FE_PGATourMessages.c's unit
// for now). The functions before fn_8010D454 are a game mode's (the one Game_GetMode() gives 26
// for; fn_8010C4A0 sets its callbacks) and belong to another file not split out yet.

#include "engine.h"
#include "character.h"
#include "charstate.h"
#include "golfer.h"
#include "ball.h"

u8 lbl_80282491;                        // set when the session is split screen (fn_8010D3B8)

// ---- sweep code (not yet cleaned up) ----

void fn_8010C714(void);
extern s32 lbl_80281764;
void fn_8010C718(void);
void fn_8010C73C(void);
void fn_8010C740(void);
void fn_8010D330(void);
void fn_8010C8B8(void);
s32 fn_8010C8D8(void);
s32 fn_8010C8E0(void);
void fn_8010CA2C();
u32 fn_8010D33C(s32* arg0);
void fn_8010C934(void);
void fn_8010C958(void);
void fn_8010D278();
void fn_8010D230(void);
void fn_8010D250(void);
extern s32 lbl_80281760;
void fn_8010D32C(void);
void fn_8010D334(s32 v);
s32 fn_8010D364(void);
extern s32 lbl_8028176C;
s32 fn_8010D390(void);
void fn_8010D428(s32 p0, s32 p1);

void fn_8010C714(void) {
}

void fn_8010C718(void) {
    gSession.nTeeSet[0] = 0;
    gSession.nTeeSet[1] = 0;
    gSession.options.n20 = 0;
    lbl_80281764 = 5;
}

void fn_8010C73C(void) {
}

void fn_8010C740(void) {
    EASBio_SetCurrentGameWon(1);
}

void fn_8010C8B8(void) {
    fn_8010D330();
}

s32 fn_8010C8D8(void) {
    return 0;
}

s32 fn_8010C8E0(void) {
    return 5;
}

void fn_8010C934(void) {
    fn_8010D33C(0);
}

void fn_8010C958(void) {
    fn_8010CA2C();
}

void fn_8010D230(void) {
    fn_8010D278();
}

void fn_8010D250(void) {
    fn_8010D278();
    lbl_80281764 = 5;
}

void fn_8010D32C(void) {
}

void fn_8010D330(void) {
}

void fn_8010D334(s32 v) {
    lbl_80281760 = v;
}

u32 fn_8010D33C(s32* arg0) {
    if (arg0 != NULL) {
        *arg0 = lbl_80281764;
    }
    return (u32) ((5 - lbl_80281764) | (lbl_80281764 - 5)) >> 0x1FU;
}

s32 fn_8010D364(void) {
    s32 t0;
    t0 = Game_GetMode();
    return (((u32)__cntlzw((26 - t0)) >> 5) & 0xFF);
}

s32 fn_8010D390(void) {
    if (((s32) lbl_80281764 != 5) && ((s32) lbl_8028176C > 0)) {
        return 1;
    }
    return 0;
}

void fn_8010D428(s32 p0, s32 p1) {
    fn_800A7664(8, p0, p1);
}

// ---- end of sweep code ----

// A fresh ball for the player on the tee set the session gives it.
void fn_8010C978(int nPlayer) {
    fn_80055AA8(&gPlayers[nPlayer].ball,
                &gPlayers[nPlayer].ball.pCourse->tee[gSession.nTeeSet[nPlayer]].x, nPlayer);
}

void fn_8010D3B8(void) {
    if (gSession.nSplitScreen) {
        lbl_80282491 = 1;
    }
}

// The ball came to rest on surface 155.
void fn_8010D3D8(int nPlayer) {
    Player* pPlayer = &gPlayers[nPlayer];

    if (pPlayer->ball.nSurface == 155) {
        // port: EA passes two arguments fn_800A746C ignores
        ((void (*)(s32, int, int, int, int))fn_800A746C)(1, 0, 0, 0, 0);
    }
}

// Free slider definitions made by CharSlider_CreateDefinitionsFromMem.
void fn_8010D454(CharSliderDefs* pDefs) {
    int i;
    int j;

    if (pDefs != NULL) {
        if (pDefs->pDefs != NULL) {
            for (i = 0; i < pDefs->nSliders; i++) {
                if (pDefs->pDefs[i].pLinks != NULL) {
                    fn_80009E70(pDefs->pDefs[i].pLinks);
                }
                if (pDefs->pDefs[i].pLimits != NULL) {
                    fn_80009E70(pDefs->pDefs[i].pLimits);
                }
                if (pDefs->pDefs[i].pBoneRanges != NULL) {
                    for (j = 0; j < pDefs->pDefs[i].nBoneRanges; j++) {
                        fn_80009E70(pDefs->pDefs[i].pBoneRanges[j].items.pBones);
                    }
                    fn_80009E70(pDefs->pDefs[i].pBoneRanges);
                }
                if (pDefs->pDefs[i].pMorphRanges != NULL) {
                    for (j = 0; j < pDefs->pDefs[i].nMorphRanges; j++) {
                        fn_80009E70(pDefs->pDefs[i].pMorphRanges[j].items.pMorphs);
                    }
                    fn_80009E70(pDefs->pDefs[i].pMorphRanges);
                }
            }
            fn_80009E70(pDefs->pDefs);
        }
        if (pDefs->pValues != NULL) {
            fn_80009E70(pDefs->pValues);
        }
        if (pDefs->aMorphIds != NULL) {
            fn_80009E70(pDefs->aMorphIds);
        }
        fn_80009E70(pDefs);
    }
}
