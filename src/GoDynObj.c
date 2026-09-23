// GoDynObj.c (EA's name, from its asserts; also in EA's 2002 source tree): the course's own objects
// around the dynamic objects (dynobj.h): the 'TEO ' and 'BALL' stream handlers, the models of the
// 'TEO ' objects, per-player objects, and when they are drawn (GoDynObjMgr, lbl_80281DA0). Partly
// decompiled.

#include "dynobj.h"
#include "character.h"
#include "lighting.h"
#include "terrain.h"
#include "camera.h"
#include "game.h"

void fn_80045FC8(UStreamObject* pObject);   // the 'BALL' stream handler
void fn_80046FDC(int nView);
void fn_800470B0(int nView);
void fn_80047208(u8* aState);
int  fn_800F1960(void);                 // GameModeReplay.c: how many targets the target games have
void fn_800F196C(int i, f32* pOut);     // GameModeReplay.c: target i's position
void fn_80093DB8(Ball* pBall, int nPlayer);    // GoObjShadow.c
void fn_80093AE0(Ball* pBall, int nPlayer);    // GoObjShadow.c
void fn_80048584(UObject* pObj, s8 nLod);
void fn_8000ADC0(f32 (*pMtx)[4]);                   // identity
void fn_80047290(void);
void fn_8004731C(u8* pState);
void fn_80047C24(int nPlayer);
void fn_80048184(int nPlayer);
void fn_80035240(int n);
u8   fn_8000B54C(u32 uType, u32 uId);     // a stream object of this type and id is loaded
UStreamObject* fn_8000B70C(u32 uType, u32 uId);

// ---- sweep code (not yet cleaned up) ----

s32 fn_8000B4B8(void*);
u8 fn_8000B508();
s32 fn_80045D80(s32);
void fn_800460F8(UStreamObject* arg0);
void fn_80045F74(UStreamObject* arg0);
s32 fn_800075CC(s32);

void fn_80045F74(UStreamObject* arg0) {
    if (fn_8000B508() == 0) {
        (*(s32*)((u8*)(arg0) + 4)) = fn_80045D80((*(s32*)((u8*)(arg0) + 0)));
        (*(void (**)(UStreamObject*))((u8*)(arg0) + 8)) = fn_800460F8;
        fn_8000B4B8(arg0);
    }
}

void fn_800460F8(UStreamObject* arg0) {
    void* temp_r31;

    temp_r31 = (*(void**)((u8*)(arg0) + 4));
    fn_800075CC((*(s32*)((u8*)(temp_r31) + 0x10)));
    fn_80009E70(temp_r31);
}

// ---- end of sweep code ----

void fn_80046130(void) {
    UStream_RegisterHandler('TEO ', fn_80045F74);
    UStream_RegisterHandler('BALL', fn_80045FC8);
}

void fn_80046174(void) {
    UStream_UnregisterHandler('TEO ');
    UStream_UnregisterHandler('BALL');
}

// Allocates the state; no 'TEO ' models yet.
void fn_800461A8(void) {
    lbl_80281DA0 = fn_80009B34(sizeof(GoDynObjMgr), 2, 16, "GoDynObj.c", 283);
    lbl_80281DA0->pTeo10000 = NULL;
    lbl_80281DA0->apTeo10030[0] = NULL;
    lbl_80281DA0->apTeo10030[1] = NULL;
    lbl_80281DA0->apTeo10030[2] = NULL;
    lbl_80281DA0->apTeo10030[3] = NULL;
    lbl_80281DA0->apTeo10040[0] = NULL;
    lbl_80281DA0->apTeo10040[1] = NULL;
    lbl_80281DA0->apTeo10040[2] = NULL;
    lbl_80281DA0->apTeo10040[3] = NULL;
    lbl_80281DA0->apTeo10020[0] = NULL;
    lbl_80281DA0->apTeo10020[1] = NULL;
    lbl_80281DA0->apTeo10020[2] = NULL;
    lbl_80281DA0->apTeo10006[0] = NULL;
    lbl_80281DA0->apTeo10006[1] = NULL;
    lbl_80281DA0->apTeo10006[2] = NULL;
    lbl_80281DA0->apTeo10006[3] = NULL;
}

void fn_80046264(void) {
    fn_80009E70(lbl_80281DA0);
}

// Makes the models of the 'TEO ' objects that are loaded, puts the flag (character 100) at the
// pin, and resets the tuning values and the per-player objects.
void fn_80046288(void) {
    int i;
    int nId;
    Character* pFlag;

    if (lbl_80281DA0->pTeo10000 == NULL) {
        lbl_80281DA0->pTeo10000 = fn_80048808(((DynObjModelRef*)fn_8000B70C('TEO ', 10000))->p4);
        lbl_80281DA0->pTeo10000->uFlags |= 1;
        lbl_80281DA0->pTeo10000->uFlags |= 0x10;
    }
    for (i = 0; i < 4; i++) {
        if (lbl_80281DA0->apTeo10030[i] == NULL) {
            nId = i + 10030;
            if (fn_8000B54C('TEO ', nId)) {
                lbl_80281DA0->apTeo10030[i] = fn_80048808(((DynObjModelRef*)fn_8000B70C('TEO ', nId))->p4);
            }
        }
    }
    for (i = 0; i < 4; i++) {
        if (lbl_80281DA0->apTeo10040[i] == NULL) {
            nId = i + 10040;
            if (fn_8000B54C('TEO ', nId)) {
                lbl_80281DA0->apTeo10040[i] = fn_80048808(((DynObjModelRef*)fn_8000B70C('TEO ', nId))->p4);
            }
        }
    }
    if (fn_800E39F0()) {
        for (i = 0; i < 4; i++) {
            if (lbl_80281DA0->apTeo10006[i] == NULL) {
                nId = i + 10006;
                if (fn_8000B54C('TEO ', nId)) {
                    lbl_80281DA0->apTeo10006[i] =
                        fn_80048808(((DynObjModelRef*)fn_8000B70C('TEO ', nId))->p4);
                }
            }
        }
        for (i = 0; i < 3; i++) {
            if (lbl_80281DA0->apTeo10020[i] == NULL) {
                nId = i + 10020;
                if (fn_8000B54C('TEO ', nId)) {
                    lbl_80281DA0->apTeo10020[i] =
                        fn_80048808(((DynObjModelRef*)fn_8000B70C('TEO ', nId))->p4);
                }
            }
        }
    }
    pFlag = fn_8001D324(100);
    if (pFlag != NULL) {
        Character_SetPosition(pFlag, &fn_8000C594()->pin[Game_CurrentPinSet()].x, 1);
        if (Game_GetMode() != 6 && Game_GetMode() != 7 && Game_GetMode() != 8) {
            pFlag->u10 |= 2;
        }
        if (fn_800E39F0()) {
            pFlag->u10 |= 0x40;
        }
    }
    lbl_80281DA0->fA94 = 7.0f;
    lbl_80281DA0->fA98 = 10.5f;
    lbl_80281DA0->fA9C = 0.3f;
    lbl_80281DA0->fAA0 = 0.7f;
    lbl_80281DA0->fAA4 = 1.5f;
    lbl_80281DA0->fAA8 = 0.6f;
    lbl_80281DA0->fAAC = -0.05169f;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        lbl_80281DA0->aB[i].bF4 = 0;
        lbl_80281DA0->aA[i].bF8 = 0;
        lbl_80281DA0->apPlayer[i] = NULL;
    }
    lbl_80281DA0->apRing[0] = NULL;
    lbl_80281DA0->apRing[1] = NULL;
    lbl_80281DA0->apRing[2] = NULL;
    lbl_80281DA0->apRing[3] = NULL;
    lbl_80281DA0->apRing[4] = NULL;
    lbl_80281DA0->apRing[5] = NULL;
    lbl_80281DA0->apRing[6] = NULL;
    lbl_80281DA0->apRing[7] = NULL;
    lbl_80281DA0->apRing[8] = NULL;
    lbl_80281DA0->apRing[9] = NULL;
    lbl_80281DA0->nRing = 0;
}

// Frees the 'TEO ' models.
void fn_80046664(void) {
    int i;

    if (lbl_80281DA0->pTeo10000 != NULL) {
        fn_80048860(lbl_80281DA0->pTeo10000);
    }
    lbl_80281DA0->pTeo10000 = NULL;
    for (i = 0; i < 4; i++) {
        if (lbl_80281DA0->apTeo10030[i] != NULL) {
            fn_80048860(lbl_80281DA0->apTeo10030[i]);
        }
        lbl_80281DA0->apTeo10030[i] = NULL;
    }
    for (i = 0; i < 4; i++) {
        if (lbl_80281DA0->apTeo10040[i] != NULL) {
            fn_80048860(lbl_80281DA0->apTeo10040[i]);
        }
        lbl_80281DA0->apTeo10040[i] = NULL;
    }
    for (i = 0; i < 3; i++) {
        if (lbl_80281DA0->apTeo10020[i] != NULL) {
            fn_80048860(lbl_80281DA0->apTeo10020[i]);
        }
        lbl_80281DA0->apTeo10020[i] = NULL;
    }
    for (i = 0; i < 4; i++) {
        if (lbl_80281DA0->apTeo10006[i] != NULL) {
            fn_80048860(lbl_80281DA0->apTeo10006[i]);
        }
        lbl_80281DA0->apTeo10006[i] = NULL;
    }
}

// The per-frame update: message 6 to every object, with the frame time, then each player's.
void fn_800467B4(void) {
    int i;

    // port: the frame time goes through the message's pointer argument as its bits
    fn_80048F68(6, *(void**)&gSession.fFrameTime, NULL);
    for (i = 0; i < gSession.nNumPlayers; i++) {
        fn_80047C24(i);
        fn_80048184(i);
    }
}

// Draws every object (message 3), and with fn_800E39F0 the targets' 'TEO ' models.
void fn_80046828(int nView) {
    fn_80035118(4, 5);
    fn_80012F50(1, 6, 0x80);
    fn_80012F18(3);
    fn_80035338(1);
    fn_80035308();
    fn_800352E4();
    fn_80012EF8();
    fn_80048F68(3, NULL, NULL);
    if (fn_800E39F0()) {
        fn_80046FDC(nView);
        fn_800470B0(nView);
    }
}

// 0 in game mode 21 while the player is in state 19 and the partner is not.
u8 fn_800468B4(int nPlayer) {
    if (Game_GetMode() == 21 && (s8)GOLFERSTATE_GetCurrentState(nPlayer) == 19 &&
        (s8)GOLFERSTATE_GetCurrentState(lbl_80187D38[nPlayer]) != 19) {
        return 0;
    }
    return 1;
}

u8 fn_80046A54(int nPlayer) {
    if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) == 1 && fn_80048574(gPlayers[nPlayer].pChar, 3)) {
        return 0;
    }
    if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) == 18 && fn_80048574(gPlayers[nPlayer].pChar, 4)) {
        return 0;
    }
    if (!fn_800468B4(nPlayer)) {
        return 0;
    }
    return 1;
}

u8 fn_80046B1C(int nPlayer) {
    if (gPlayers[nPlayer].ball.nLie == 9 &&(s8)GOLFERSTATE_GetCurrentState(nPlayer) == 19 &&
        fn_800468B4(nPlayer)) {
        return 1;
    }
    return 0;
}

void fn_80046B8C(int nView) {
    u8 aState[8];

    fn_80035118(4, 5);
    fn_80012F50(1, 6, 0x80);
    fn_80012F18(3);
    fn_80035338(1);
    fn_80035308();
    fn_800352E4();
    fn_80012EF8();
    fn_8004731C(aState);
    fn_80035240(0);
    if (gSession.nSplitScreen == 0) {
        fn_80047290();
        if ((s8)GOLFERSTATE_GetCurrentState(fn_8001707C(nView)) != 9) {
            fn_80047208(aState);
        }
    }
}

// ---- 0x80046C34..0x80046FDC: not yet decompiled ----

// Draws a 'TEO ' model (10006 + the target's kind) at each target of the target games.
void fn_80046FDC(int nView) {
    f32 vPos[4];
    int i;
    int nKind;

    for (i = 0; i < fn_800F1960(); i++) {
        fn_800F196C(i, vPos);
        nKind = gpGame->pfn26C(fn_8001707C(nView), i);
        if (lbl_80281DA0->apTeo10006[nKind] != NULL) {
            fn_80048584(lbl_80281DA0->apTeo10006[nKind], 0);
            fn_8000ADC0(lbl_80281DA0->apTeo10006[nKind]->m80);
            Vec_Copy(vPos, lbl_80281DA0->apTeo10006[nKind]->m80[3]);
            fn_80048894(lbl_80281DA0->apTeo10006[nKind]);
        }
    }
}

// The same with the 'TEO ' 10020..10022 models, for the target kinds 0, 2 and 3.
void fn_800470B0(int nView) {
    f32 vPos[4];
    int i;
    int nModel;

    fn_80012F50(0, 6, 0x80);
    fn_80012F34(0);
    fn_80012EF8();
    for (i = 0; i < fn_800F1960(); i++) {
        fn_800F196C(i, vPos);
        switch (gpGame->pfn26C(fn_8001707C(nView), i)) {
        case 0:
            nModel = 0;
            break;
        case 2:
            nModel = 1;
            break;
        case 3:
            nModel = 2;
            break;
        default:
            nModel = -1;
            break;
        }
        if (nModel != -1 && lbl_80281DA0->apTeo10020[nModel] != NULL) {
            fn_80048584(lbl_80281DA0->apTeo10020[nModel], 0);
            fn_8000ADC0(lbl_80281DA0->apTeo10020[nModel]->m80);
            Vec_Copy(vPos, lbl_80281DA0->apTeo10020[nModel]->m80[3]);
            fn_80048894(lbl_80281DA0->apTeo10020[nModel]);
        }
    }
    fn_80012F50(1, 6, 0x80);
    fn_80012F34(1);
    fn_80012EF8();
}

void fn_80047208(u8* aState) {
    int i;

    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (aState[i]) {
            fn_80093DB8(&gPlayers[i].ball, i);
        }
    }
}

void fn_80047290(void) {
    int i;

    for (i = 0; i < gSession.nNumPlayers; i++) {
        if (fn_80046B1C(i)) {
            fn_80093AE0(&PLAYER(i)->ball, i);
        }
    }
}

// ---- 0x8004731C..0x80047B6C: not yet decompiled ----

// Gives up the player's object.
void fn_80047B6C(Ball* pBall, int nPlayer) {
    if (lbl_80281DA0->apPlayer[nPlayer] != NULL) {
        fn_800491C4(lbl_80281DA0->apPlayer[nPlayer]);
        fn_800490EC();
        lbl_80281DA0->apPlayer[nPlayer] = NULL;
    }
}

void fn_80047BC0(Ball* pBall, int nPlayer) {
    if (lbl_80281DA0->aB[nPlayer].bF4) {
        fn_800491C4(lbl_80281DA0->aB[nPlayer].pF0);
        fn_800490EC();
        lbl_80281DA0->aB[nPlayer].pF0 = NULL;
        lbl_80281DA0->aB[nPlayer].bF4 = 0;
    }
}

void fn_8004816C(int nPlayer) {
    lbl_80281DA0->aA[nPlayer].b70 = 1;
}

char* fn_800484E0(int i) {
    return lbl_80187B98[i];
}

// The index of the name in lbl_80187B98 (case ignored), or -1.
int fn_800484F4(const char* szName) {
    int i;

    if (szName == NULL) {
        return -1;
    }
    for (i = 0; i < 27; i++) {
        if (stricmp(szName, lbl_80187B98[i]) == 0) {
            return i;
        }
    }
    return -1;
}

int fn_80048574(Character* pChar, u64 uEvent) {
    return pChar->events[uEvent].bSet;
}

// Sets the level of detail the object is drawn with.
void fn_80048584(UObject* pObj, s8 nLod) {
    pObj->n104 = nLod;
}

// a - b into out (three floats)
#ifdef __MWERKS__
asm void fn_80048680(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_80048680(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// a + b into out (four floats)
#ifdef __MWERKS__
asm void fn_800486A4(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_800486A4(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
    pOut[3] = pB[3] + pA[3];
}
#endif

// a + b into out (three floats)
#ifdef __MWERKS__
asm void fn_800486C8(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_800486C8(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

void fn_800486EC(void) {
}

void fn_800486F0(void) {
}
