// FEgolferanim.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the golfer
// animated on the menu screens (the create-a-player screen's golfer). Its state is CrAPState
// (camera.h); a small state machine (lbl_801D8708) streams the next golfer in, sets him up and
// shows him, and the render code draws him into the menu's texture.

#include "game.h"
#include "camera.h"
#include "character.h"
#include "frontend/fe.h"
#include "game/frontend.h"
#include "gx.h"
#include "ustream.h"

// The golfers the menus show in turn when none is picked: four rows of five golfer ids, the row
// picked at random.
s32 lbl_801899E0[4][5] = {
    { 0, 5, 11, 14, 16 },
    { 1, 2, 20, 17, 10 },
    { 0, 8, 21, 24, 26 },
    { 1, 27, 4, 15, 25 },
};

// Per screen kind: two vectors copied into CrAPState.v120 and v130 (all three are the same).
f32 lbl_80189A40[2][4] = { { 0.5f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } };
f32 lbl_80189A60[2][4] = { { 0.5f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } };
f32 lbl_80189A80[2][4] = { { 0.5f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f, 1.0f } };

void fn_8008B204(void);
void fn_8008B20C(void);
void fn_8008B208(void);
void fn_8008B3C8(void);
void fn_8008B3CC(void);
void fn_8008B450(void);
void fn_8008B430(void);
void fn_8008B4A0(void);
void fn_8008B4C0(void);
void fn_8008B574(void);
void fn_8008B570(void);
void fn_8008B5FC(void);
void fn_8008B61C(void);
void fn_8008B694(void);
void fn_8008B674(void);
void fn_8008B6E4(void);

// The loader's states: 1 wait for a golfer to load, 2 stream him in, 3 set him up, 4 set up the
// golfer already loaded.
FEGolferState lbl_80189AA0[FE_NUM_GOLFER_STATES] = {
    { NULL, NULL, NULL, NULL, 0 },
    { fn_8008B204, fn_8008B20C, fn_8008B208, fn_8008B3C8, 2 },
    { fn_8008B3CC, fn_8008B450, fn_8008B430, fn_8008B4A0, 3 },
    { fn_8008B4C0, fn_8008B574, fn_8008B570, fn_8008B5FC, 1 },
    { fn_8008B61C, fn_8008B694, fn_8008B674, fn_8008B6E4, 1 },
};

FEGolferMachine lbl_801D8708;

s32 lbl_80281330 = 1;           // draw the golfer into the menu's texture (fn_8008E358)
u8  lbl_8028133C = 1;
s32 lbl_80281340 = -1;          // } the golfer and profile slot last drawn (fn_8008CE88)
s32 lbl_80281344 = -1;          // }

CrAPState* lbl_80281EE0;
Character* lbl_80281EE8[CRAP_NUM_GOLFERS];

void fn_8008B00C(void);
void fn_8008B704(void);
void fn_8008B754(int nNext);
void fn_8008B790(void);
void fn_8008B7D0(int nState);
void fn_8008B820(void);
void fn_8008B850(void);
void fn_8008B864(void);
void fn_8008C938(void);
void fn_8008C93C(void);
void fn_8008CA88(void);
void fn_8008CC30(void);
void fn_8008CE2C(void);
void fn_8008CE88(u8 b);
void fn_8008D8F4(void);
void fn_8008DBE8(void);
void fn_8008DC10(void);
u8   fn_8008DCF0(int nGolfer, CrAPGolfer* pGolfer);
void fn_8008DD50(u8 bNoBlend);
Clip* fn_8008E02C(void);
void fn_8008E254(u8 b);
void fn_8008EA44(u8 b);
void fn_8008EBB4(void);
void fn_8008EBE4(void);
void fn_8008AD80(void);

void fn_80007254(void);
void fn_80008380(void);
void fn_8000ADC0(f32 (*m)[4]);          // identity matrix
void fn_80014DFC(s32 a, s32 b);
void fn_80016E90(int nView);
void fn_80018484(Character* pChar, CharModel* pModel);
void fn_8001BE88(Character* pChar, Clip* pClip, int bNoBlend, f32 f);
void fn_8001C0E0(Character* pChar);
void fn_8001C5B4(Character* pChar, int n);
void fn_8001D238(void);
void fn_8001D4A4(Character* pChar, int nSlot);
void fn_8001EE98(Character* pChar, u8 b);
void Session_SetupProfiles(void);
void fn_80079974(void);
void fn_800B9CF0(int n);
void fn_800CEE88(u8 b);
void fn_8010B098(void* p);
void fn_8010B9BC(void);
void fn_8010BEC4(void);
u8   fn_8010BFE0(void);
void UStream_Stop(void);

// Make the golfer display's state and start the loader.
void fn_8008AD80(void) {
    int i;
    int nPrev;
    int nNext;

    lbl_80281EE0 = fn_80009B34(sizeof(CrAPState), 2, 0, "FEgolferanim.c", 337);
    lbl_80281EE0->n0 = 0;
    lbl_80281EE0->n4 = 0;
    lbl_80281EE0->n1B8 = 0;
    lbl_80281EE0->n1BC = -1;
    lbl_80281EE0->n1B4 = 0;
    lbl_80281EE0->sz10[0] = '\0';
    lbl_80281EE0->b85 = 1;
    lbl_80281EE0->b86 = 0;
    lbl_80281EE0->b91 = 1;
    lbl_80281EE0->b88 = 0;
    lbl_80281EE0->b89 = 0;
    lbl_80281EE0->b84 = 1;
    lbl_80281EE0->b82 = 0;
    lbl_80281EE0->b83 = 0;
    lbl_80281EE0->f14C = 0.5f;
    lbl_80281EE0->nBC = 0;
    lbl_80281EE0->b87 = 0;
    lbl_80281EE0->b8A = 0;
    lbl_80281EE0->n8C = -1;
    lbl_80281EE0->n194 = 0;
    lbl_80281EE0->n198 = Rand_Next(0) & 3;
    lbl_80281EE0->f19C = 0.0f;
    lbl_80281EE0->f1A0 = 0.0f;
    lbl_80281EE0->b1B0 = 0;
    lbl_80281EE0->b90 = 0;
    lbl_80281EE0->b1D1 = 1;
    lbl_80281EE0->b1D2 = 0;
    lbl_80281EE0->b1DC = 0;
    lbl_80281EE0->aGolfer[0].nC = -1;
    for (i = 0; i < CRAP_NUM_GOLFERS; i++) {
        nPrev = i - 1;
        if (nPrev < 0) {
            nPrev = CRAP_NUM_GOLFERS - 1;
        }
        nNext = i + 1;
        if (nNext >= CRAP_NUM_GOLFERS) {
            nNext = 0;
        }
        lbl_80281EE8[i] = NULL;
        lbl_80281EE0->aGolfer[i].pChar = lbl_80281EE8[i];
        lbl_80281EE0->aGolfer[i].n10 = i;
        lbl_80281EE0->aGolfer[i].n14 = -1;
        lbl_80281EE0->aGolfer[i].n1C = -1;
        lbl_80281EE0->aGolfer[i].b19 = 0;
        lbl_80281EE0->aGolfer[i].pPrev = &lbl_80281EE0->aGolfer[nPrev];
        lbl_80281EE0->aGolfer[i].pNext = &lbl_80281EE0->aGolfer[nNext];
        lbl_80281EE0->aGolfer[i].b18 = 0;
        lbl_80281EE0->aGolfer[i].nC = lbl_801899E0[lbl_80281EE0->n198][lbl_80281EE0->n194];
        lbl_80281EE0->n194++;
        if (lbl_80281EE0->n194 >= 5) {
            lbl_80281EE0->n194 = 0;
            lbl_80281EE0->n198++;
            if (lbl_80281EE0->n198 >= 4) {
                lbl_80281EE0->n198 = 0;
            }
        }
    }
    lbl_80281EE0->pB4 = &lbl_80281EE0->aGolfer[0];
    gPlayers[0].pChar = lbl_80281EE0->pB4->pChar;
    lbl_80281EE0->pB8 = &lbl_80281EE0->aGolfer[0];
    lbl_80281EE0->n190 = 0;
    fn_8008B790();
    lbl_80281EE0->n74 = 0;
}

// Stop the loader and free the state.
void fn_8008B00C(void) {
    fn_8008B820();
    fn_8008DBE8();
    fn_8008DC10();
    fn_80009E70(lbl_80281EE0);
    lbl_80281EE0 = NULL;
}

// Show golfer nGolfer: the one shown, the next or the previous one if one of them is him, else
// load him into the shown slot. Golfers 7 and 29 (with a or b) set b90.
void fn_8008B044(int nGolfer, int a, int b) {
    CrAPGolfer* pGolfer;

    fn_8008B704();
    fn_8008B754(1);
    if (nGolfer == 7 || nGolfer == 29) {
        a = -1;
        b = -1;
        lbl_80281EE0->b90 = 1;
    }
    if (b == 7 || b == 29) {
        lbl_80281EE0->b90 = 1;
    }
    if (a == 7 || a == 29) {
        lbl_80281EE0->b90 = 1;
    }
    pGolfer = lbl_80281EE0->pB4;
    if (pGolfer->nC != nGolfer) {
        if (pGolfer->pNext->nC == nGolfer) {
            lbl_80281EE0->pB4 = pGolfer->pNext;
            gPlayers[0].pChar = lbl_80281EE0->pB4->pChar;
        } else if (pGolfer->pPrev->nC == nGolfer) {
            lbl_80281EE0->pB4 = pGolfer->pPrev;
            gPlayers[0].pChar = lbl_80281EE0->pB4->pChar;
        } else {
            pGolfer->nC = nGolfer;
            lbl_80281EE0->pB4->b18 = 0;
            lbl_80281EE0->pB4->n1C = -1;
        }
    }
    lbl_80281EE0->b87 = 1;
    fn_8008DC10();
    if (lbl_80281EE0->n0 == 3) {
        fn_8008DD50(0);
    }
}

// With b90 set, wait until no golfer slot holds a character.
u8 fn_8008B1AC(void) {
    u8 bReady;

    if (lbl_80281EE0->b90) {
        bReady = 1;
        if (lbl_80281EE0->aGolfer[0].pChar != NULL) {
            lbl_80281EE0->aGolfer[0].b19 = 1;
            bReady = 0;
        }
        if (bReady) {
            lbl_80281EE0->b90 = 0;
            return 1;
        }
        return 0;
    }
    return 1;
}

void fn_8008B204(void) {
}

void fn_8008B208(void) {
}

// State 1: pick the next golfer to load, the shown one first, then its neighbours.
void fn_8008B20C(void) {
    CrAPGolfer* pGolfer;

    if (lbl_801D8708.bAbort) {
        fn_8008B850();
        return;
    }
    if (lbl_80281EE0->b8A == 1 || lbl_80281EE0->aGolfer[0].b19 == 1) return;
    pGolfer = lbl_80281EE0->pB4;
    if (pGolfer->b18 == 0 && pGolfer->nC != -1) {
        if (fn_8008DCF0(pGolfer->nC, pGolfer)) {
            fn_8008DAEC();
            return;
        }
        lbl_80281EE0->pB8 = lbl_80281EE0->pB4;
        lbl_80281EE0->nBC = 0;
        if (lbl_80281EE0->pB8->pChar != NULL) {
            lbl_80281EE0->pB8->b19 = 1;
            return;
        }
        if (fn_8008B1AC()) {
            fn_8008B850();
        }
    } else if (pGolfer->pNext->b18 == 0 && pGolfer->pNext->nC != -1) {
        if (fn_8008DCF0(pGolfer->pNext->nC, pGolfer->pNext)) return;
        lbl_80281EE0->pB8 = lbl_80281EE0->pB4->pNext;
        lbl_80281EE0->nBC = 1;
        if (lbl_80281EE0->pB8->pChar != NULL) {
            lbl_80281EE0->pB8->b19 = 1;
            return;
        }
        if (fn_8008B1AC()) {
            fn_8008B850();
        }
    } else if (pGolfer->pPrev->b18 == 0 && pGolfer->pPrev->nC != -1) {
        if (fn_8008DCF0(pGolfer->pPrev->nC, pGolfer->pPrev)) return;
        lbl_80281EE0->pB8 = lbl_80281EE0->pB4->pPrev;
        lbl_80281EE0->nBC = 2;
        if (lbl_80281EE0->pB8->pChar != NULL) {
            lbl_80281EE0->pB8->b19 = 1;
            return;
        }
        if (fn_8008B1AC()) {
            fn_8008B850();
        }
    }
}

void fn_8008B3C8(void) {
}

// State 2: open the golfer's stream.
void fn_8008B3CC(void) {
    fn_80014DFC(lbl_80281EE0->pB8->nC, lbl_80281EE0->pB8->n10);
    fn_8008EBB4();
    lbl_80281EE0->pB8->n14 = lbl_80281EE0->pB8->nC;
    lbl_80281EE0->n8C = lbl_80281EE0->pB8->nC;
    lbl_80281EE0->n190++;
}

void fn_8008B430(void) {
    fn_8008EBE4();
}

void fn_8008B450(void) {
    if (!UStream_Update()) {
        if (lbl_801D8708.bAbort) {
            lbl_80281EE0->pB8->b18 = 0;
        }
        fn_8008B850();
    }
}

void fn_8008B4A0(void) {
    UStream_Stop();
}

// State 3: set up the golfer that came in.
void fn_8008B4C0(void) {
    int nGolfer;

    Session_SetupProfiles();
    fn_800CEE88(1);
    nGolfer = lbl_80281EE0->pB8->pChar->nC;
    if (nGolfer == 7 || nGolfer == 29) {
        fn_80079974();
    }
    fn_8001D4A4(lbl_80281EE0->pB8->pChar, lbl_80281ED4->nSlot);
    fn_800CEE88(0);
    fn_8010B098(lbl_80281EE0->pB8->pChar->a64[lbl_80281EE0->pB8->pChar->n74]);
    fn_80019D64(lbl_80281EE0->pB8->pChar, fn_80019DE8, fn_80019E80);
}

void fn_8008B570(void) {
}

void fn_8008B574(void) {
    if (!fn_8010BFE0()) {
        if (lbl_801D8708.bAbort) {
            lbl_80281EE0->pB8->b18 = 0;
        } else {
            lbl_80281EE0->pB8->b18 = 1;
            lbl_80281344 = -1;
            lbl_80281340 = -1;
            lbl_80281EE0->pB8->n1C = -1;
        }
        lbl_80281EE0->n8C = -1;
        fn_8008B850();
    }
}

void fn_8008B5FC(void) {
    fn_8010B9BC();
}

// State 4: set up the golfer shown.
void fn_8008B61C(void) {
    fn_8001D4A4(lbl_80281EE0->pB4->pChar, lbl_80281ED4->nSlot);
    fn_80019D64(lbl_80281EE0->pB4->pChar, fn_80019EF4, fn_8001A0FC);
}

void fn_8008B674(void) {
    fn_8010BEC4();
}

void fn_8008B694(void) {
    if (!fn_8010BFE0()) {
        if (lbl_801D8708.bAbort) {
            lbl_80281EE0->pB4->b18 = 0;
        }
        fn_8008B850();
    }
}

void fn_8008B6E4(void) {
    fn_8010B9BC();
}

// Abort the running state.
void fn_8008B704(void) {
    lbl_801D8708.bAbort = 1;
    lbl_80189AA0[lbl_801D8708.nState].pfnAbort();
}

void fn_8008B754(int nNext) {
    lbl_801D8708.nNext = nNext;
}

void fn_8008B760(void) {
    fn_8008B754(1);
    fn_8008B704();
    fn_8008B7D0(1);
}

// Start in state 1.
void fn_8008B790(void) {
    lbl_801D8708.nState = 1;
    lbl_801D8708.nNext = lbl_80189AA0[1].nNext;
    lbl_801D8708.bDone = 0;
    lbl_801D8708.bEnter = 1;
    lbl_801D8708.bAbort = 0;
    lbl_801D8708.bPaused = 0;
}

// Run the machine until it reaches nState.
void fn_8008B7D0(int nState) {
    while (lbl_801D8708.nState != nState) {
        fn_8008B864();
        fn_80007254();
    }
}

void fn_8008B820(void) {
    fn_8008B754(1);
    fn_8008B704();
    fn_8008B7D0(1);
}

// The running state is finished.
void fn_8008B850(void) {
    lbl_801D8708.bDone = 1;
}

// Run the state machine once: enter the state, update it, and when it is finished leave it for
// the next one.
void fn_8008B864(void) {
    if (lbl_801D8708.bPaused == 0 && lbl_801D8708.nState != 0) {
        if (lbl_801D8708.bEnter) {
            lbl_80189AA0[lbl_801D8708.nState].pfnEnter();
            lbl_801D8708.bEnter = 0;
        }
        lbl_80189AA0[lbl_801D8708.nState].pfnUpdate();
        if (lbl_801D8708.bDone) {
            lbl_80189AA0[lbl_801D8708.nState].pfnExit();
            lbl_801D8708.bDone = 0;
            lbl_801D8708.nState = lbl_801D8708.nNext;
            lbl_801D8708.bEnter = 1;
            lbl_801D8708.bAbort = 0;
            lbl_801D8708.nNext = lbl_80189AA0[lbl_801D8708.nNext].nNext;
        }
    }
}

// Pause the machine (or not); returns the old setting.
u8 fn_8008B978(u8 bPaused) {
    u8 bOld = lbl_801D8708.bPaused;

    lbl_801D8708.bPaused = bPaused;
    return bOld;
}

int fn_8008B990(void) {
    return lbl_801D8708.nState;
}

void fn_8008B9A0(void) {
    View* pView;

    pView = fn_80017028(fn_80016D10());
    fn_80016E90(fn_80016D10());
    fn_8000ADC0(lbl_80281EE0->mC0);
    Vec_Copy(pView->v0, lbl_80281EE0->v100);
    Vec_Copy(pView->v10, lbl_80281EE0->v110);
    switch (lbl_80281EE0->n0) {
    case 0:
        Vec_Copy(lbl_80189A40[0], lbl_80281EE0->v120);
        Vec_Copy(lbl_80189A40[1], lbl_80281EE0->v130);
        break;
    case 1:
    case 3:
    case 4:
        Vec_Copy(lbl_80189A60[0], lbl_80281EE0->v120);
        Vec_Copy(lbl_80189A60[1], lbl_80281EE0->v130);
        break;
    case 2:
        Vec_Copy(lbl_80189A80[0], lbl_80281EE0->v120);
        Vec_Copy(lbl_80189A80[1], lbl_80281EE0->v130);
        break;
    }
}

// Draw the golfer into the menu's texture, when he is shown and lbl_80281330 allows it.
void fn_8008C844(void) {
    if (lbl_80281EE0->pB4->b18 && lbl_80281EE0->b86 == 0 && lbl_80281EE0->b88 == 0 && lbl_80281330 != 0) {
        lbl_80281EE0->b18C = 1;
        fn_8008C938();
        fn_8008C93C();
        fn_8008CE88(0);
        fn_800B9CF0(0);
        fn_8008CC30();
        fn_8008CE2C();
        fn_8008C93C();
    }
}

void fn_8008C8C4(void) {
    if (lbl_80281EE0->pB4->b18 && lbl_80281EE0->b86 == 0 && lbl_80281EE0->b88 == 0) {
        if (lbl_80281330 != 0) {
            fn_8008CA88();
            return;
        }
        lbl_80281EE0->b18C = 0;
        fn_8008CE88(0);
        fn_800B9CF0(0);
    }
}

void fn_8008C938(void) {
}

// Copy the frame buffer's golfer (from (128, 0), 384 x 448) into the screen-copy texture.
void fn_8008CE2C(void) {
    GXPixModeSync();
    GXSetTexCopySrc(128, 0, 384, 448);
    GXSetTexCopyDst(384, 448, 6, 0);           // RGBA8, no mipmap
    GXCopyTex(fn_8002A624(), 0);
    GXPixModeSync();
    GXInvalidateTexAll();
}

void fn_8008D8CC(void) {
    fn_8001D238();
    fn_8008AD80();
    fn_8008D8F4();
}

// Stop loading, and show the next golfer of lbl_801899E0 (screen 0) or none (screen 4).
void fn_8008DAEC(void) {
    lbl_80281EE0->b8A = 1;
    fn_8008B704();
    fn_8008B754(1);
    lbl_80281EE0->n190 = 0;
    lbl_80281EE0->pB4->b18 = 0;
    switch (lbl_80281EE0->n0) {
    case 0:
        lbl_80281EE0->aGolfer[0].nC = lbl_801899E0[lbl_80281EE0->n198][lbl_80281EE0->n194 % 5];
        lbl_80281EE0->n194++;
        if (lbl_80281EE0->n194 >= 5) {
            lbl_80281EE0->n194 = 0;
            lbl_80281EE0->n198++;
            if (lbl_80281EE0->n198 >= 4) {
                lbl_80281EE0->n198 = 0;
            }
        }
        break;
    case 4:
        lbl_80281EE0->aGolfer[0].nC = -1;
        break;
    }
}

void fn_8008DBE8(void) {
    lbl_80281EE0->aGolfer[0].b19 = 1;
    if (lbl_80281EE0->pB4 != NULL) {
        lbl_80281EE0->pB4->b18 = 0;
    }
}

// Once the loader is idle: finish a stop (b8A), or free a golfer slot's character (b19).
void fn_8008DC10(void) {
    if (fn_8008B990() == 1) {
        if (lbl_80281EE0->b8A && fn_8008B990() == 1) {
            fn_8008DBE8();
            lbl_80281EE0->b8A = 0;
            return;
        }
        if (lbl_80281EE0->aGolfer[0].b19
            && (lbl_80281EE0->pB4->b18 == 0 || &lbl_80281EE0->aGolfer[0] != lbl_80281EE0->pB4)
            && (fn_8008B990() == 1 || &lbl_80281EE0->aGolfer[0] != lbl_80281EE0->pB8)) {
            if (lbl_80281EE0->aGolfer[0].pChar != NULL) {
                fn_80008380();
                fn_8001C0E0(lbl_80281EE0->aGolfer[0].pChar);
            }
            lbl_80281EE0->aGolfer[0].b18 = 0;
            lbl_80281EE0->aGolfer[0].b19 = 0;
            lbl_80281EE0->aGolfer[0].pChar = NULL;
        }
    }
}

// Another golfer slot than pGolfer has golfer nGolfer, shown or streamed.
u8 fn_8008DCF0(int nGolfer, CrAPGolfer* pGolfer) {
    if (&lbl_80281EE0->aGolfer[0] != pGolfer
        && ((lbl_80281EE0->aGolfer[0].nC == nGolfer && lbl_80281EE0->aGolfer[0].b18)
            || lbl_80281EE0->aGolfer[0].n14 == nGolfer)) {
        return 1;
    }
    return 0;
}

void fn_8008DD34(int nProfile, s8 n) {
    gpSaveData[nProfile].choices.n113 = n;
}

// Start the golfer's idle animation (or the one n8 asks for) and point the camera at him.
void fn_8008DD50(u8 bNoBlend) {
    View* pView;
    Clip* pClip;

    pView = fn_80017028(fn_80016D10());
    if (lbl_80281EE0->pB4 == NULL) return;
    if (lbl_80281EE0->pB4->pChar == NULL) return;
    if (lbl_80281EE0->n8 != 0) {
        if (lbl_80281EE0->n8 == 1) {
            fn_8001C5B4(lbl_80281EE0->pB4->pChar, 0);
        } else {
            fn_8001C5B4(lbl_80281EE0->pB4->pChar, 2);
        }
        pClip = Char_SetClip(lbl_80281EE0->pB4->pChar, 8, 0, NULL);
        lbl_80281EE0->sz10[0] = '\0';
        fn_8001BE88(lbl_80281EE0->pB4->pChar, pClip, !bNoBlend, 0.5f);
        fn_8001C5B4(lbl_80281EE0->pB4->pChar, lbl_80281EE0->n1B8);
        if (lbl_80281EE0->n8 == 1) {
            GolfCamera_SwitchCrAPCamera(pView, NULL, 3, bNoBlend, 0, 0);
            return;
        }
        GolfCamera_SwitchCrAPCamera(pView, NULL, 4, bNoBlend, 0, 0);
        return;
    }
    if (lbl_80281EE0->n1B4 == 5 && lbl_80281EE0->b1DC == 0) {
        fn_8001C5B4(lbl_80281EE0->pB4->pChar, 2);
        lbl_80281EE0->n1B4 = -1;
        if (lbl_80281EE0->n4 == 0 || lbl_80281EE0->n4 == 2) {
            lbl_80281EE0->b1C8 = 1;
            fn_8008E2F8(1, 0.0f);
        }
        fn_8008E254(0);
    } else {
        fn_8001C5B4(lbl_80281EE0->pB4->pChar, 0);
        lbl_80281EE0->n1B4++;
        lbl_80281EE0->b1C8 = 0;
    }
    if (lbl_80281EE0->n50 > 0 && (pClip = lbl_80281EE0->pB4->pChar->pCurClip) != NULL) {
        lbl_80281EE0->n50--;
    } else {
        pClip = fn_8008E02C();
    }
    if (pClip->uD8 != 0) {
        if (!fn_8001EDF4(lbl_80281EE0->pB4->pChar) && fn_80077ACC()->choices.n113 != 0) {
            fn_8008E2F8(0, PI);
        }
        fn_8008EA44(fn_80077ACC()->choices.n113);
    } else {
        if (fn_8001EDF4(lbl_80281EE0->pB4->pChar)) {
            fn_8008E2F8(0, 0.0f);
        }
        fn_8008EA44(0);
    }
    lbl_80281EE0->sz10[0] = '\0';
    fn_8001BE88(lbl_80281EE0->pB4->pChar, pClip, !bNoBlend, 0.5f);
    fn_8001C5B4(lbl_80281EE0->pB4->pChar, lbl_80281EE0->n1B8);
    GolfCamera_SwitchCrAPCamera(pView, NULL, lbl_80281EE0->n4, bNoBlend, lbl_80281EE0->b1DC, 0);
}

// The idle clip for the camera kind.
Clip* fn_8008E02C(void) {
    if (lbl_80281EE0->n4 == 1) {
        return Char_SetClip(lbl_80281EE0->pB4->pChar, 11, 0, NULL);
    }
    if (lbl_80281EE0->n4 == 2) {
        return Char_SetClip(lbl_80281EE0->pB4->pChar, 7, 0, NULL);
    }
    return Char_SetClip(lbl_80281EE0->pB4->pChar, 1, 0, NULL);
}

// Turn the golfer by fTurn radians: f1A0 is where he should face, f19C where he faces; f19C
// follows it 0.05 a frame, the short way round.
void fn_8008E0B0(f32 fTurn) {
    f32 fDiff;

    lbl_80281EE0->f19C += fTurn;
    lbl_80281EE0->f1A0 += fTurn;
    if (lbl_80281EE0->f19C < 0.0f) {
        lbl_80281EE0->f19C = 2.0f * PI + lbl_80281EE0->f19C;
    } else if (lbl_80281EE0->f19C > 2.0f * PI) {
        lbl_80281EE0->f19C = lbl_80281EE0->f19C - 2.0f * PI;
    }
    if (lbl_80281EE0->f1A0 < 0.0f) {
        lbl_80281EE0->f1A0 = 2.0f * PI + lbl_80281EE0->f1A0;
    } else if (lbl_80281EE0->f1A0 > 2.0f * PI) {
        lbl_80281EE0->f1A0 = lbl_80281EE0->f1A0 - 2.0f * PI;
    }
    fDiff = fabsf(lbl_80281EE0->f1A0 - lbl_80281EE0->f19C);
    if (fDiff < 2.0f * PI - fDiff) {
        if (lbl_80281EE0->f19C < lbl_80281EE0->f1A0 - 0.05f) {
            lbl_80281EE0->f19C = lbl_80281EE0->f19C + 0.05f;
        } else if (lbl_80281EE0->f19C > 0.05f + lbl_80281EE0->f1A0) {
            lbl_80281EE0->f19C = lbl_80281EE0->f19C - 0.05f;
        }
    } else if (lbl_80281EE0->f19C < lbl_80281EE0->f1A0) {
        lbl_80281EE0->f19C = lbl_80281EE0->f19C - 0.05f;
        if (lbl_80281EE0->f19C < 0.0f) {
            lbl_80281EE0->f19C = 2.0f * PI + lbl_80281EE0->f19C;
        }
    } else if (lbl_80281EE0->f19C > lbl_80281EE0->f1A0) {
        lbl_80281EE0->f19C = lbl_80281EE0->f19C + 0.05f;
        if (lbl_80281EE0->f19C > 2.0f * PI) {
            lbl_80281EE0->f19C = lbl_80281EE0->f19C - 2.0f * PI;
        }
    }
    if (lbl_80281EE0->pB4->pChar != NULL) {
        fn_800192D4(lbl_80281EE0->pB4->pChar, lbl_80281EE0->f19C);
    }
}

void fn_8008E244(void) {
    lbl_80281EE0->b1DC = 0;
}

void fn_8008E254(u8 b) {
    View* pView = fn_80017028(fn_80016D10());

    if (b) {
        if (lbl_80281EE0->b1DC != 1) {
            lbl_80281EE0->b1DC = 1;
            GolfCamera_SwitchCrAPCamera(pView, NULL, lbl_80281EE0->n4, 1, lbl_80281EE0->b1DC, 1);
        }
    } else if (lbl_80281EE0->b1DC != 0) {
        lbl_80281EE0->b1DC = 0;
        GolfCamera_SwitchCrAPCamera(pView, NULL, lbl_80281EE0->n4, 1, lbl_80281EE0->b1DC, 1);
    }
}

// Face the golfer fAngle radians round: at once, or (bTarget) over time (fn_8008E0B0).
void fn_8008E2F8(u8 bTarget, f32 fAngle) {
    if (bTarget) {
        lbl_80281EE0->f1A0 = fAngle;
        return;
    }
    lbl_80281EE0->f19C = fAngle;
    lbl_80281EE0->f1A0 = fAngle;
    if (lbl_80281EE0->pB4->pChar != NULL) {
        fn_800192D4(lbl_80281EE0->pB4->pChar, lbl_80281EE0->f19C);
    }
}

void fn_8008E354(void) {
}

void fn_8008E358(u8 b) {
    lbl_80281330 = b;
}

void fn_8008E364(int n) {
    Clip* pClip;

    if (n != lbl_80281EE0->n4) {
        pClip = fn_8008E02C();
        if (pClip != NULL) {
            if (lbl_80281EE0->n1C0 != 1 && lbl_80281EE0->n1C0 != 0) {
                fn_8008E724(pClip->name, NULL, 0, 0);
                fn_8008E944(0, 0.0f);
                lbl_80281EE0->n1C4 = 2;
            } else {
                strncpy(lbl_80281EE0->sz20, pClip->name, sizeof(lbl_80281EE0->sz20));
                lbl_80281EE0->n1C4 = 2;
            }
        }
        lbl_80281EE0->sz30[0] = '\0';
    }
    lbl_80281EE0->n4 = n;
}

int fn_8008E420(void) {
    if (lbl_80281EE0->pB4 != NULL && lbl_80281EE0->pB4->pChar != NULL) {
        return 1;
    }
    return 0;
}

int fn_8008E44C(void) {
    return lbl_80281EE0->pB4->b18 != 0;
}

// Play the animation szAnim on the golfer (for golfer 29, with an 'f' in front if it has no
// such animation) and point the camera with szShot.
u8 fn_8008E468(char* szAnim, char* szShot, u8 bNoBlend) {
    char szName[0x20];
    View* pView;
    Clip* pClip;

    pView = fn_80017028(fn_80016D10());
    if (!fn_80103B80()) {
        return 0;
    }
    if (lbl_80281EE0->n8 == 1) {
        GolfCamera_SwitchCrAPCamera(pView, szShot, 3, bNoBlend, 0, 0);
    } else if (lbl_80281EE0->n8 == 2) {
        GolfCamera_SwitchCrAPCamera(pView, szShot, 4, bNoBlend, 0, 0);
    } else {
        GolfCamera_SwitchCrAPCamera(pView, szShot, lbl_80281EE0->n4, bNoBlend, lbl_80281EE0->b1DC, 0);
    }
    if (lbl_80281EE0->pB4 != NULL && lbl_80281EE0->pB4->pChar != NULL && szAnim != NULL) {
        strncpy(szName, szAnim, sizeof(szName));
        szName[sizeof(szName) - 1] = '\0';
        pClip = AnimLib_FindByName(lbl_80281EE0->pB4->pChar->pLib, szName);
        if (pClip == NULL && lbl_80281EE0->pB4->pChar->nC == 29 && szName != NULL && szName[0] != '\0') {
            szName[0] = 'f';
            pClip = AnimLib_FindByName(lbl_80281EE0->pB4->pChar->pLib, szName);
        }
        if (pClip == NULL) {
            return 0;
        }
        if (pClip->uD8 != 0) {
            if (!fn_8001EDF4(lbl_80281EE0->pB4->pChar) && fn_80077ACC()->choices.n113 != 0) {
                fn_8008E2F8(0, PI);
            }
            fn_8008EA44(fn_80077ACC()->choices.n113);
        } else {
            if (fn_8001EDF4(lbl_80281EE0->pB4->pChar)) {
                fn_8008E2F8(0, 0.0f);
            }
            fn_8008EA44(0);
        }
        fn_8001BE88(lbl_80281EE0->pB4->pChar, pClip, !bNoBlend, 0.5f);
        strcpy(lbl_80281EE0->sz10, szAnim);
        lbl_80281EE0->n1B4 = 0;
        return 1;
    }
    return 0;
}

// The animation fn_8008E468 started, or NULL.
char* fn_8008E6BC(void) {
    if (lbl_80281EE0->sz10[0] == '\0') {
        return NULL;
    }
    return lbl_80281EE0->sz10;
}

void fn_8008E6D4(int n) {
    lbl_80281EE0->n1B8 = n;
    lbl_80281EE0->n1BC = -1;
    fn_8001C5B4(lbl_80281EE0->pB4->pChar, lbl_80281EE0->n1B8);
}

void fn_8008E718(int n) {
    lbl_80281EE0->n1BC = n;
}

void fn_8008E724(char* szAnim, char* szShot, s8 n, u8 bLoop) {
    if (szAnim == NULL) {
        lbl_80281EE0->sz20[0] = '\0';
        lbl_80281EE0->n1C0 = 4;
    } else {
        strncpy(lbl_80281EE0->sz20, szAnim, sizeof(lbl_80281EE0->sz20));
        lbl_80281EE0->f1CC = 0.5f;
        if (bLoop) {
            lbl_80281EE0->n1C0 = 0;
        } else {
            lbl_80281EE0->n1C0 = 1;
        }
        lbl_80281EE0->b1C8 = 1;
        lbl_80281EE0->n1C4 = 3;
    }
    if (szShot == NULL) {
        lbl_80281EE0->sz30[0] = '\0';
    } else {
        strncpy(lbl_80281EE0->sz30, szShot, sizeof(lbl_80281EE0->sz30));
    }
    lbl_80281EE0->n1D0 = n;
}

void fn_8008E818(int n) {
    lbl_80281EE0->n50 = n;
}

void fn_8008E824(void) {
    fn_8008DD50(0);
    lbl_80281EE0->n50 = 0;
    lbl_80281EE0->n1C0 = 4;
}

void fn_8008E860(int n) {
    int nOld;

    if (fn_80103B80()) {
        nOld = lbl_80281EE0->n8;
        lbl_80281EE0->n8 = n;
        lbl_80281EE0->nC = 0;
        lbl_80281EE0->b80 = 0;
        if (nOld != lbl_80281EE0->n8) {
            fn_8008E244();
            fn_8008DD50(0);
        }
    }
}

void fn_8008E8D0(int n) {
    if (fn_80103B80()) {
        lbl_80281EE0->nC = n;
        lbl_80281EE0->b80 = 1;
    }
}

void fn_8008E918(int n) {
    lbl_80281EE0->n74 = n;
}

u8 fn_8008E924(void) {
    return lbl_80281EE0->n74 == 0;
}

u8 fn_8008E938(void) {
    return lbl_80281EE0->b78;
}

u8 fn_8008E944(u8 b, f32 f) {
    u8 bOld = lbl_80281EE0->b78;

    lbl_80281EE0->b78 = b;
    lbl_80281EE0->f7C = f;
    return bOld;
}

void fn_8008E960(char* sz) {
    if (sz == NULL) {
        lbl_80281EE0->sz54[0] = '\0';
        return;
    }
    strncpy(lbl_80281EE0->sz54, sz, sizeof(lbl_80281EE0->sz54));
}

int fn_8008E9A8(void) {
    return lbl_80281EE0->n8;
}

void fn_8008E9B4(void) {
    Clip* pClip;

    fn_8001C5B4(lbl_80281EE0->pB4->pChar, 5);
    pClip = Char_SetClip(lbl_80281EE0->pB4->pChar, 8, 0, NULL);
    if (pClip != NULL) {
        fn_8001BE88(lbl_80281EE0->pB4->pChar, pClip, 0, 0.5f);
    }
    fn_8001C5B4(lbl_80281EE0->pB4->pChar, lbl_80281EE0->n1B8);
}

void fn_8008EA38(u8 b) {
    lbl_80281EE0->b81 = b;
}

// Golfers 7 and 29 only: pass b on to the character and rebuild its model.
void fn_8008EA44(u8 b) {
    if (lbl_80281EE0->pB4 != NULL && lbl_80281EE0->pB4->pChar != NULL) {
        if (lbl_80281EE0->pB4->pChar->nC != 7 && lbl_80281EE0->pB4->pChar->nC != 29) return;
        fn_8001EE98(lbl_80281EE0->pB4->pChar, b);
        fn_80018484(lbl_80281EE0->pB4->pChar, lbl_80281EE0->pB4->pChar->pModel);
    }
}

u8 fn_8008EAB0(void) {
    return lbl_80281EE0->b1D1;
}

void fn_8008EABC(u8 b) {
    lbl_80281EE0->b1D1 = b;
}

void fn_8008EAC8(u8 b) {
    lbl_80281EE0->b1D2 = b;
}

u8 fn_8008EAD4(void) {
    return lbl_80281EE0->b1D2;
}

void fn_8008EAE0(int n) {
    lbl_80281EE0->n1D4 = n;
}

int fn_8008EAEC(void) {
    return lbl_80281EE0->n1D4;
}

void fn_8008EAF8(int n) {
    lbl_80281EE0->n1D8 = n;
}

int fn_8008EB04(void) {
    return lbl_80281EE0->n1D8;
}

// The golfer is shown and nothing covers him.
u8 fn_8008EB10(void) {
    u8 bResult = 0;

    if (lbl_80281EE0->pB4->b18 && lbl_80281EE0->b86 == 0 && lbl_80281EE0->b88 == 0 && lbl_80281F19 == 0
        && lbl_801D87C0.b0 == 0 && lbl_801D87C0.b49 == 0) {
        bResult = 1;
    }
    return bResult;
}

void fn_8008EB70(void) {
    fn_8008E724(NULL, NULL, 0, 0);
    fn_8008DD50(0);
    fn_8008E944(0, 0.0f);
}

// ---- sweep code (not yet cleaned up) ----

void UStream_Close();
s32 UStream_Open();

void fn_8008EBB4(void) {
    s32 t0;
    t0 = UStream_Open(&lbl_80280DF8->aParams[3]);
    lbl_80280DF8->nStream = t0;
}

void fn_8008EBE4(void) {
    UStream_Close(lbl_80280DF8->nStream);
}

// ---- end of sweep code ----
