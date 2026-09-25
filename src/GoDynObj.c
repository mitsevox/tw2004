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
#include "unsorted/cull.h"

char lbl_80187B98[27][13] = {
    "LogoBENHapex",
    "LogoCALLhxb",
    "LogoCALLhxr",
    "LogoCALLctr",
    "LogoEApiece",
    "LogoEAspiner",
    "LogoEApboo",
    "LogoEAspow",
    "LogoEAwtc",
    "LogoMAXFnod",
    "LogoMAXFa3r",
    "LogoMAXFm3r",
    "LogoMAXFm3b",
    "LogoNIKEsoft",
    "LogoNIKEfar",
    "LogoNIKEfly",
    "LogoNIKEspn",
    "LogoNIKElng",
    "LogoNIKEtw",
    "LogoPRECed",
    "LogoPRECtp",
    "LogoTITLprov",
    "LogoSTRAace",
    "LogoWILSwils",
    "LogoNIKEone",
    "LogoNIKEoneT",
    "LogoCYBM",
};
char lbl_80187CF8[4][13] = {"logoea", "logonike", "logotitl", "logostrt"};

f32 lbl_80281128 = 1.0f;

GoDynObjMgr* lbl_80281DA0;

void fn_80045FC8(UStreamObject* pObject);   // the 'BALL' stream handler
void fn_80046FDC(s32 nView);
void fn_800470B0(s32 nView);
void fn_80047208(u8* aState);
int  fn_800F1960(void);                 // GameModeReplay.c: how many targets the target games have
void fn_800F196C(int i, f32* pOut);     // GameModeReplay.c: target i's position
void fn_80093DB8(Ball* pBall, int nPlayer);    // GoObjShadow.c
void BFX_vRender(Ball* pBall, int nPlayer);    // GoObjShadow.c
void fn_80048584(UObject* pObj, s8 nLod);
void fn_8000ADC0(f32 (*pMtx)[4]);                   // identity
int  fn_800636EC(void);
void fn_8004858C(f32* pOut, f32 fTurn, f32 fTilt);
void fn_8000C5A4(f32 (*pMtx)[4]);
void fn_8000A194(f32 (*pMtx)[4], f32 a, f32 b, f32 c);  // a rotation matrix from three angles
void fn_8000A0E8(f32 (*pSrc)[4], f32 (*pDst)[4]);
void fn_800BADF8(f32 (*pMtx)[4], f32 (*pSrc)[4], f32 (*pDst)[4], int nRows);
void fn_8000A144(f32 (*pSrc)[4], f32 (*pDst)[4]);        // copies three rows
void fn_80048680(f32* pA, f32* pB, f32* pOut);
void fn_800486A4(f32* pA, f32* pB, f32* pOut);
void fn_800486C8(f32* pA, f32* pB, f32* pOut);
void fn_80047290(void);
void fn_8004731C(u8* pState);
f32  fn_8004787C(int nPlayer);
void Quat_QuatToMatrix(f32* pQ, f32 (*pMtx)[4]);         // Quaternion.c: to a matrix
void fn_80047C24(int nPlayer);
void fn_80048184(int nPlayer);
int  fn_8001005C(TexBank* pBank, u64 uHash);       // LLTex.c: the texture's index, or 0x80000000

// ---- sweep code (not yet cleaned up) ----

void fn_800460F8(UStreamObject* arg0);
void fn_80045F74(UStreamObject* arg0);

void fn_80045F74(UStreamObject* arg0) {
    if (fn_8000B508(arg0) == 0) {
        (*(UObjModel**)((u8*)(arg0) + 4)) = fn_80045D80(arg0->pData);
        (*(void (**)(UStreamObject*))((u8*)(arg0) + 8)) = fn_800460F8;
        fn_8000B4B8(arg0);
    }
}

// ---- end of sweep code ----

// The 'BALL' stream handler: a bank of ball logos. Each player's chosen logo (its profile's byte
// 0x38, from lbl_80187B98's names) is copied over the pixels of that player's logo texture
// ("logoea", "logonike", ...), then the bank is freed.
void fn_80045FC8(UStreamObject* pObject) {
    u64 uLogo;
    u64 uSlot;
    TexBank* pSlotBank;
    TexEntry* pSlot;
    TexBank* pBank;
    TexEntry* pLogo;
    int i;
    int nLogo;

    pBank = fn_8000FB88(pObject, NULL, -2);
    for (i = 0; i < gSession.nNumPlayers; i++) {
        if ((s8)gSession.aProfile[i].nOutfit >= 0) {
            fn_800CB700(&uLogo, lbl_80187B98[(s8)gSession.aProfile[i].nOutfit]);
            fn_800CB700(&uSlot, lbl_80187CF8[i]);
            fn_800102DC(uSlot, &pSlotBank, &pSlot);
            nLogo = fn_8001005C(pBank, uLogo);
            pLogo = &pBank->p8[nLogo];
            // EA bug: this loop counts with the player loop's i
            while (i < pLogo->n41) {
                Mem_cpy(pSlotBank->p18 + pSlot->aMips[0].uPixels, pBank->p18 + pLogo->aMips[0].uPixels,
                        pLogo->aMips[0].nC * 16);
                i++;
            }
        }
    }
    fn_80009E70(pBank);
    fn_80009E70(pObject);
}

// ---- sweep code (not yet cleaned up) ----

void fn_800460F8(UStreamObject* arg0) {
    void* temp_r31;

    temp_r31 = (*(void**)((u8*)(arg0) + 4));
    fn_800075CC(*(UObjModelRoot**)((u8*)(temp_r31) + 0x10));
    fn_80009E70(temp_r31);
}

// ---- end of sweep code ----

void fn_80046130(void) {
    Stream_RegisterLoadChunkCallback('TEO ', fn_80045F74);
    Stream_RegisterLoadChunkCallback('BALL', fn_80045FC8);
}

void fn_80046174(void) {
    Stream_UnregisterLoadChunkCallback('TEO ');
    Stream_UnregisterLoadChunkCallback('BALL');
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

// Defined after fn_800461A8 so that its "GoDynObj.c" comes first in .data, as in the original.
s32 lbl_80187D38[4] = {1, 0, 3, 2};

void fn_80046264(void) {
    fn_80009E70(lbl_80281DA0);
}

// Makes the models of the 'TEO ' objects that are loaded, puts the flag (character 100) at the
// pin, and resets the tuning values and the per-player objects.
void DynObj_InitForHole(void) {
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
u8 DynObj_bAltShotDrawBall(int nPlayer) {
    if (Game_GetMode() == 21 && (s8)GOLFERSTATE_GetCurrentState(nPlayer) == 19 &&
        (s8)GOLFERSTATE_GetCurrentState(lbl_80187D38[nPlayer]) != 19) {
        return 0;
    }
    return 1;
}

// fake match: stands in for a function the original linker stripped. Its constants (10.0, 0.5,
// 1.0, 0.0) are still in this file's pool ahead of fn_80046928's; its body is unknown.
static f32 GoDynObj_StrippedFn(f32 x) {
    x += 10.0f;
    x += 0.5f;
    x += 1.0f;
    if (x > 0.0f) {
        return 0.0f;
    }
    return x;
}

u8 fn_80046928(int nPlayer) {
    int nLie = gPlayers[nPlayer].ball.nLie;
    u32 nClass;

    if (nLie == 16) {
        if (gPlayers[nPlayer].ball.nSurface < 0 || gPlayers[nPlayer].ball.nSurface >= NUM_SURFACE_TYPES) {
            return 0;
        }
        nClass = gSurfaceTypes[gPlayers[nPlayer].ball.nSurface].nClass;
        if (nClass == 7 || nClass == 15 || nClass == 16) {
            return 0;
        }
        if (gPlayers[nPlayer].ball.fHeight > 0.5f) {
            return 0;
        }
    }
    if ((nLie == 0 || nLie == 12 || nLie == 9) && (s8)GOLFERSTATE_GetCurrentState(nPlayer) == 19) {
        return 0;
    }
    if (!DynObj_bAltShotDrawBall(nPlayer)) {
        return 0;
    }
    if ((s8)GOLFERSTATE_GetCurrentState(nPlayer) == 18 && nPlayer == fn_800636EC()) {
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
    if (!DynObj_bAltShotDrawBall(nPlayer)) {
        return 0;
    }
    return 1;
}

u8 fn_80046B1C(int nPlayer) {
    if (gPlayers[nPlayer].ball.nLie == 9 &&(s8)GOLFERSTATE_GetCurrentState(nPlayer) == 19 &&
        DynObj_bAltShotDrawBall(nPlayer)) {
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
    fn_80035240(NULL);
    if (gSession.nSplitScreen == 0) {
        fn_80047290();
        if ((s8)GOLFERSTATE_GetCurrentState(fn_8001707C(nView)) != 9) {
            fn_80047208(aState);
        }
    }
}

// A ball moving at 10 or more just above ground of class 3 leaves a 'TEO ' 10005 object at pPos:
// ten are used in turn.
void fn_80046C34(f32* pPos, int nPlayer) {
    DynObjDef def;
    DynObjSetup setup;
    DynObjModel model;
    f32 vPos[4];
    f32 vNormal[4];
    SurfaceType* pSurface;
    f32 fGround;
    s32 nId;

    vPos[0] = pPos[0];
    vPos[1] = pPos[1];
    vPos[2] = pPos[2];
    vPos[3] = 1.0f;
    fGround = Ter_GetSupportingGroundData(fn_8000C594(), vPos, &pSurface, vNormal);
    if (TER_NO_GROUND != fGround && pSurface->nClass == 3 && fGround - pPos[1] < 0.02f &&
        (f32)fn_80009680(fn_80009744(gPlayers[nPlayer].ball.vVel)) >= 10.0f) {
        if (lbl_80281DA0->apRing[lbl_80281DA0->nRing] == NULL) {
            // EA bug: def.n1A is never set, and type 0's setup copies it to DynObj.n14E
            def.n0 = 0;
            def.n4 = 0;
            def.aPos[0] = pPos[0];
            def.aPos[1] = pPos[1];
            def.aPos[2] = pPos[2];
            def.u14 = 0x200;
            def.n18 = 0;
            setup.pDef = &def;
            setup.pModel = &model;
            model.aEntries[0].uType = 'TEO ';
            setup.pModel->aEntries[0].u.pRef = (DynObjModelRef*)fn_8000B70C('TEO ', 10005);
            setup.pfnHandler = fn_800499B0(setup.pDef->n4);
            setup.pC = NULL;
            nId = fn_800490B8(&setup);
            if (nId != -2) {
                lbl_80281DA0->apRing[lbl_80281DA0->nRing] = fn_80048E4C(nId);
            }
        } else {
            Vec_Copy(vPos, lbl_80281DA0->apRing[lbl_80281DA0->nRing]->obj.m80[3]);
            fn_8000C5A4(lbl_80281DA0->apRing[lbl_80281DA0->nRing]->obj.m0);
        }
        lbl_80281DA0->nRing = lbl_80281DA0->nRing + 1;
        lbl_80281DA0->nRing = lbl_80281DA0->nRing % 10;
    }
}

// Puts the player's 'TEO ' 10001 object on the ground at pPos, turned to the player's aim; the
// first time it is made (a type 0 object, flags 0xC00).
void fn_80046E1C(f32* pPos, int nPlayer) {
    f32 mTurn[4][4];
    f32 mObj[4][4];
    DynObjDef def;
    DynObjSetup setup;
    DynObjModel model;
    f32 vPos[4];
    f32 vNormal[4];
    SurfaceType* pSurface;
    f32 fGround;
    s32 nId;

    vPos[0] = pPos[0];
    vPos[1] = pPos[1];
    vPos[2] = pPos[2];
    vPos[3] = 1.0f;
    fGround = Ter_GetSupportingGroundData(fn_8000C594(), vPos, &pSurface, vNormal);
    if (lbl_80281DA0->apPlayer[nPlayer] == NULL) {
        // EA bug: def.n1A is never set, and type 0's setup copies it to DynObj.n14E
        vPos[1] = 0.01f + fGround;
        def.n0 = 0;
        def.n4 = 0;
        def.aPos[0] = vPos[0];
        def.aPos[1] = vPos[1];
        def.aPos[2] = vPos[2];
        def.u14 = 0xC00;
        def.n18 = 0;
        setup.pDef = &def;
        setup.pModel = &model;
        model.aEntries[0].uType = 'TEO ';
        setup.pModel->aEntries[0].u.pRef = (DynObjModelRef*)fn_8000B70C('TEO ', 10001);
        setup.pfnHandler = fn_800499B0(setup.pDef->n4);
        setup.pC = NULL;
        nId = fn_800490B8(&setup);
        if (nId != -2) {
            lbl_80281DA0->apPlayer[nPlayer] = fn_80048E4C(nId);
        }
    }
    fn_8000ADC0(mTurn);
    fn_8000ADC0(lbl_80281DA0->apPlayer[nPlayer]->obj.m0);
    fn_8000A194(mTurn, -gPlayers[nPlayer].fAim, 0.0f, 0.0f);
    fn_800BADF8(lbl_80281DA0->apPlayer[nPlayer]->obj.m0, mTurn, mObj, 4);
    fn_8000A0E8(mObj, lbl_80281DA0->apPlayer[nPlayer]->obj.m0);
    Vec_Copy(vPos, lbl_80281DA0->apPlayer[nPlayer]->obj.m80[3]);
    fn_8000C5A4(lbl_80281DA0->apPlayer[nPlayer]->obj.m0);
}

// Draws a 'TEO ' model (10006 + the target's kind) at each target of the target games.
void fn_80046FDC(s32 nView) {
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
void fn_800470B0(s32 nView) {
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
            BFX_vRender(&PLAYER(i)->ball, i);
        }
    }
}

// Places and draws each shown player's ball: the 'TEO ' 10000 model, plus its logo models 10030+i
// and 10040+i when the player has a logo. A flying or rolling ball turns by its spin; the level of
// detail follows the camera's distance (over 50: 2, over 10: 1); the ball is flattened to the
// view's aspect and sunk into the ground by the surface's lie, and grown when it looks small on
// screen. pState[i] is set when fn_80046A54 picks player i.
void fn_8004731C(u8* pState) {
    f32 aSpin[4];
    f32 aTurn[4];
    f32 aRot[4];
    s32 i;
    UObject* pBall;
    UObject* pLogoA;
    UObject* pLogoB;
    f32 fDist;
    f32 fSink;
    f32 fSize;
    f32 fGrow;
    f32 fTurn;
    f32 fT;

    pBall = lbl_80281DA0->pTeo10000;
    for (i = 0; i < gSession.nNumPlayers; i++) {
        pState[i] = 0;
        if (!fn_80046928(i)) {
            continue;
        }
        if ((s8)gSession.aProfile[i].nOutfit >= 0) {
            pLogoA = lbl_80281DA0->apTeo10030[i];
            pLogoB = lbl_80281DA0->apTeo10040[i];
        } else {
            pLogoA = NULL;
            pLogoB = NULL;
        }
        if (gPlayers[i].ball.nState != 0 && gPlayers[i].ball.nState != 1) {
            // port: NTSC rate; the frame's spin turn, scaled by the ball's radius squared
            fT = 59.94f / 60.0f;
            fT *= 59.94f * gSession.fFrameTime;    // port: NTSC rate
            fT *= lbl_80283304;
            fTurn = 60.0f;
            fTurn *= fT / 36.0f;
            fn_8001EF34(fTurn, gPlayers[i].ball.vSpin, aSpin);
            Quat_BuildFromVector(aSpin, aTurn);
            Quat_Multiply(gPlayers[i].vOrient, aTurn, aRot);
            fn_8001E85C(aRot, gPlayers[i].vOrient);
            Quat_QuatToMatrix(aRot, pBall->m0);
            if (pLogoA != NULL) {
                Quat_QuatToMatrix(aRot, pLogoA->m0);
            }
            if (pLogoB != NULL) {
                Quat_QuatToMatrix(aRot, pLogoB->m0);
            }
        } else {
            Quat_QuatToMatrix(gPlayers[i].vOrient, pBall->m0);
            if (pLogoA != NULL) {
                Quat_QuatToMatrix(gPlayers[i].vOrient, pLogoA->m0);
            }
            if (pLogoB != NULL) {
                Quat_QuatToMatrix(gPlayers[i].vOrient, pLogoB->m0);
            }
        }
        fDist = fn_800BB028(fn_8001F004()->m4[3], gPlayers[i].ball.vPos);
        if (fDist > 2500.0f) {
            fn_80048584(pBall, 2);
            if (pLogoA != NULL) {
                fn_80048584(pLogoA, 2);
            }
            if (pLogoB != NULL) {
                fn_80048584(pLogoB, 2);
            }
        } else if (fDist > 100.0f) {
            fn_80048584(pBall, 1);
            if (pLogoA != NULL) {
                fn_80048584(pLogoA, 1);
            }
            if (pLogoB != NULL) {
                fn_80048584(pLogoB, 1);
            }
        } else {
            fn_80048584(pBall, 0);
            if (pLogoA != NULL) {
                fn_80048584(pLogoA, 0);
            }
            if (pLogoB != NULL) {
                fn_80048584(pLogoB, 0);
            }
        }
        fn_8000ADC0(pBall->m40);
        if (pLogoA != NULL) {
            fn_8000ADC0(pLogoA->m40);
        }
        if (pLogoB != NULL) {
            fn_8000ADC0(pLogoB->m40);
        }
        pBall->m40[0][0] = lbl_80281128;
        pBall->m40[1][1] = lbl_80281128 / fn_80017028(gPlayers[i].nView[0])->f54;
        pBall->m40[2][2] = lbl_80281128;
        if (pLogoA != NULL) {
            pLogoA->m40[0][0] = lbl_80281128;
            pLogoA->m40[1][1] = lbl_80281128 / fn_80017028(gPlayers[i].nView[0])->f54;
            pLogoA->m40[2][2] = lbl_80281128;
        }
        if (pLogoB != NULL) {
            pLogoB->m40[0][0] = lbl_80281128;
            pLogoB->m40[1][1] = lbl_80281128 / fn_80017028(gPlayers[i].nView[0])->f54;
            pLogoB->m40[2][2] = lbl_80281128;
        }
        Vec_Copy(gPlayers[i].ball.vPos, pBall->m80[3]);
        pBall->m80[3][3] = 1.0f;
        if (pLogoA != NULL) {
            Vec_Copy(gPlayers[i].ball.vPos, pLogoA->m80[3]);
            pLogoA->m80[3][3] = 1.0f;
        }
        if (pLogoB != NULL) {
            Vec_Copy(gPlayers[i].ball.vPos, pLogoB->m80[3]);
            pLogoB->m80[3][3] = 1.0f;
        }
        if (gPlayers[i].ball.nState != 2 && gPlayers[i].ball.nSurface >= 0) {
            fSink = 1.0f - (gPlayers[i].ball.f70 + gSurfaceTypes[gPlayers[i].ball.nSurface].f00);
            pBall->m80[3][1] -= 2.0f * (fSink * gRealBallRadiusIn / 36.0f);
            if (pLogoA != NULL) {
                pLogoA->m80[3][1] -= 2.0f * (fSink * gRealBallRadiusIn / 36.0f);
            }
            if (pLogoB != NULL) {
                pLogoB->m80[3][1] -= 2.0f * (fSink * gRealBallRadiusIn / 36.0f);
            }
        }
        fSize = fn_8004787C(i);
        if (fSize < 0.01f) {
            fSize = 0.01f;
        }
        if (fSize < 1.5f) {
            fGrow = 1.0f / (fSize / 1.5f);
            fGrow = 0.7f * (fGrow - 1.0f) + 1.0f;
            fn_8000AE28(fGrow, pBall->m40[0], pBall->m40[0]);
            fn_8000AE28(fGrow, pBall->m40[1], pBall->m40[1]);
            fn_8000AE28(fGrow, pBall->m40[2], pBall->m40[2]);
            if (pLogoA != NULL) {
                fn_8000AE28(fGrow, pLogoA->m40[0], pLogoA->m40[0]);
            }
            if (pLogoA != NULL) {
                fn_8000AE28(fGrow, pLogoA->m40[1], pLogoA->m40[1]);
            }
            if (pLogoA != NULL) {
                fn_8000AE28(fGrow, pLogoA->m40[2], pLogoA->m40[2]);
            }
            if (pLogoB != NULL) {
                fn_8000AE28(fGrow, pLogoB->m40[0], pLogoB->m40[0]);
            }
            if (pLogoB != NULL) {
                fn_8000AE28(fGrow, pLogoB->m40[1], pLogoB->m40[1]);
            }
            if (pLogoB != NULL) {
                fn_8000AE28(fGrow, pLogoB->m40[2], pLogoB->m40[2]);
            }
        }
        fn_8000C5A4(pBall->m0);
        if (pLogoA != NULL) {
            fn_8000C5A4(pLogoA->m0);
        }
        if (pLogoB != NULL) {
            fn_8000C5A4(pLogoB->m0);
        }
        if (fn_80046A54(i)) {
            pState[i] = 1;
        }
        fn_80048894(pBall);
        if (pLogoA != NULL) {
            fn_80048894(pLogoA);
        }
        if (pLogoB != NULL) {
            fn_80048894(pLogoB);
        }
    }
}

// How big nPlayer's ball looks on screen: the distance between the points its radius above and
// below its centre land on, on a 512 x 448 screen.
f32 fn_8004787C(int nPlayer) {
    Vec4 vPos;
    f32 vDiff[4];
    Vec4 vAbove;
    Vec4 vBelow;
    Vec4 vTop;
    Vec4 vBottom;
    Camera* pCamera;
    CamLens* pLens;
    f32 fRadius;

    pCamera = fn_8001614C();
    pLens = fn_80008370(pCamera);
    fRadius = lbl_80281DA0->pTeo10000->pModel->apLod[0]->pInfo->f64 * lbl_80281128;
    Vec_Copy(gPlayers[nPlayer].ball.vPos, &vPos.x);
    vPos.w = 1.0f;
    fn_800BAD60(pLens->m44, &vPos, &vAbove);
    Vec_Copy(&vAbove.x, &vBelow.x);
    vAbove.y += fRadius;
    vBelow.y -= fRadius;
    fn_800BAD60(pCamera->m5C, &vAbove, &vTop);
    fn_800BAD60(pCamera->m5C, &vBelow, &vBottom);
    if (0.0f != vTop.w) {
        fn_8000AE28(1.0f / vTop.w, &vTop.x, &vTop.x);
    }
    if (0.0f != vBottom.w) {
        fn_8000AE28(1.0f / vBottom.w, &vBottom.x, &vBottom.x);
    }
    vTop.z = 0.0f;
    vTop.x = 256.0f * (1.0f + vTop.x);
    vTop.y = 224.0f * (1.0f + vTop.y);
    vBottom.z = 0.0f;
    vBottom.x = 256.0f * (1.0f + vBottom.x);
    vBottom.y = 224.0f * (1.0f + vBottom.y);
    fn_80048680(&vTop.x, &vBottom.x, vDiff);
    return (f32)fn_80009680(fn_80009744(vDiff));
}

// Puts the player's 'TEO ' 10002 object at pPos, facing against the aim; the first time it is made
// (as a type 0 object, flag 0x200).
void fn_80047A24(f32* pPos, int nPlayer) {
    DynObjDef def;
    DynObjSetup setup;
    DynObjModel model;
    GoDynObjPlayerB* pB = &lbl_80281DA0->aB[nPlayer];
    s32 nId;

    pB->v20[0] = pPos[0];
    pB->v20[1] = pPos[1];
    pB->v20[2] = pPos[2];
    pB->v20[3] = 1.0f;
    pB->fC = -gPlayers[nPlayer].fAim;
    pB->bF5 = 1;
    pB->b0 = 1;
    if (!pB->bF4) {
        // EA bug: def.n1A is never set, and type 0's setup copies it to DynObj.n14E
        def.n0 = 0;
        def.n4 = 0;
        def.aPos[0] = pB->v20[0];
        def.aPos[1] = pB->v20[1];
        def.aPos[2] = pB->v20[2];
        def.u14 = 0x200;
        def.n18 = 0;
        setup.pDef = &def;
        setup.pModel = &model;
        model.aEntries[0].uType = 'TEO ';
        setup.pModel->aEntries[0].u.pRef = (DynObjModelRef*)fn_8000B70C('TEO ', 10002);
        setup.pfnHandler = fn_800499B0(setup.pDef->n4);
        setup.pC = NULL;
        nId = fn_800490B8(&setup);
        if (nId != -2) {
            pB->pF0 = fn_80048E4C(nId);
        }
        pB->bF4 = 1;
    } else {
        fn_8000ADC0(pB->pF0->obj.m0);
        Vec_Copy(pB->v20, pB->pF0->obj.m80[3]);
        fn_8000C5A4(pB->pF0->obj.m0);
    }
}

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

// Flies the player's 'TEO ' 10002 object (fn_80047A24) off its spot: launched once with a random
// speed and spin, then carried by its velocity, the wind and gravity until it lands.
void fn_80047C24(int nPlayer) {
    f32 mTmp[4][4];
    f32 vMove[4];
    f32 vWind[4];
    GoDynObjPlayerB* pB = &lbl_80281DA0->aB[nPlayer];
    f32 fRange;
    f32 fGround;

    if (!pB->b0 || 0.0f == gSession.fFrameTime) return;
    if (!pB->bF4) return;
    pB->b0 = 0;
    if (pB->bF5) {
        fn_8000ADC0(pB->mB0);
        fn_8000A194(pB->mB0, pB->fC, 0.0f, 0.0f);
        fn_800BADF8(pB->pF0->obj.m0, pB->mB0, mTmp, 4);
        fn_8000A0E8(mTmp, pB->pF0->obj.m0);
        fn_8000C5A4(pB->pF0->obj.m0);
        Vec_Copy(pB->v20, pB->v30);
        fn_8004858C(pB->v40, pB->fC, lbl_80281DA0->fAA0);
        fn_8000AE28(lbl_80281DA0->fA98 * (0.5f * Misc_RandFuncf(1) + 0.5f), pB->v40, pB->v40);
        pB->v40[3] = pB->v40[1];
        pB->bF5 = 0;
        pB->b0 = 1;
        pB->f10 = 0.0f;
        fRange = lbl_80281DA0->fAA8;
        pB->v60[0] = fRange * Misc_RandFuncf(1) - 0.5f * fRange;
        fRange = lbl_80281DA0->fAA8;
        pB->v60[1] = fRange * Misc_RandFuncf(1) - 0.5f * fRange;
        fRange = lbl_80281DA0->fAA8;
        pB->v60[2] = fRange * Misc_RandFuncf(1) - 0.5f * fRange;
        pB->v50[0] = 0.0f;
        pB->v50[1] = 0.0f;
        pB->v50[2] = 0.0f;
        return;
    }
    pB->f10 += gSession.fFrameTime;
    Wind_Get(vWind);
    fn_8000AE28(0.48888f * 0.3f, vWind, vWind);
    fn_800486C8(vWind, pB->v40, vMove);
    fn_8001EF34(pB->f10, vMove, vMove);
    vMove[1] = vMove[1] + -4.9f * pB->f10 * pB->f10;
    fn_800486C8(vMove, pB->v20, pB->v30);
    pB->b0 = 1;
    fGround = fn_8004D5F0(fn_8000C594(), pB->v30);
    if (pB->v30[1] < fGround) {
        pB->v30[1] = 0.01f + fGround;
        pB->b0 = 0;
    }
    fn_800486A4(pB->v60, pB->v50, pB->v50);
    fn_8000ADC0(pB->mB0);
    fn_8000A194(pB->mB0, pB->fC + pB->v50[0], pB->v50[1], pB->v50[2]);
    fn_8000A144(pB->mB0, pB->pF0->obj.m0);
    fn_8000C5A4(pB->pF0->obj.m0);
    Vec_Copy(pB->v30, pB->pF0->obj.m80[3]);
    pB->pF0->obj.m80[3][3] = 1.0f;
}


// Puts the player's 'TEO ' 10004 object at pPos (raised by fAAC), the first time making it (a type
// 0 object with no flags); after that its turn angles wind back to 0 (at once with bReset).
void DynObj_TeeAdd(f32* pPos, int nPlayer, u8 bReset) {
    DynObjDef def;
    DynObjSetup setup;
    DynObjModel model;
    GoDynObjPlayerA* pA = &lbl_80281DA0->aA[nPlayer];
    s32 nId;
    int i;

    pA->v20[0] = pPos[0];
    pA->v20[1] = pPos[1] + lbl_80281DA0->fAAC;
    pA->v20[2] = pPos[2];
    pA->v20[3] = 1.0f;
    pA->bF9 = 1;
    pA->b0 = 1;
    pA->b70 = 0;
    if (!pA->bF8) {
        // EA bug: def.n1A is never set, and type 0's setup copies it to DynObj.n14E
        def.n0 = 0;
        def.n4 = 0;
        def.aPos[0] = pA->v20[0];
        def.aPos[1] = pA->v20[1];
        def.aPos[2] = pA->v20[2];
        def.u14 = 0;
        def.n18 = 0;
        setup.pDef = &def;
        setup.pModel = &model;
        model.aEntries[0].uType = 'TEO ';
        setup.pModel->aEntries[0].u.pRef = (DynObjModelRef*)fn_8000B70C('TEO ', 10004);
        setup.pfnHandler = fn_800499B0(setup.pDef->n4);
        setup.pC = NULL;
        nId = fn_800490B8(&setup);
        if (nId != -2) {
            pA->pF4 = fn_80048E4C(nId);
        }
        pA->bF8 = 1;
        fn_8000ADC0(pA->mB4);
        Vec_Copy(pA->v20, pA->v30);
        Vec_Copy(pA->v20, pA->pF4->obj.m80[3]);
        fn_8000C5A4(pA->pF4->obj.m0);
        return;
    }
    if (bReset) {
        pA->v50[0] = 0.0f;
        pA->v50[1] = 0.0f;
        pA->v50[2] = 0.0f;
    } else {
        for (i = 0; i < 3; i++) {
            if (fabs(pA->v50[i]) > PI / 4.0f) {
                pA->v50[i] = 0.0f;
            }
            if (pA->v50[i] > 0.0f) {
                pA->v50[i] -= PI / 2.0f * gSession.fFrameTime;
                if (pA->v50[i] < 0.0f) {
                    pA->v50[i] = 0.0f;
                }
            }
            if (pA->v50[i] < 0.0f) {
                pA->v50[i] += PI / 2.0f * gSession.fFrameTime;
                if (pA->v50[i] > 0.0f) {
                    pA->v50[i] = 0.0f;
                }
            }
        }
    }
    fn_8000ADC0(pA->mB4);
    fn_8000A194(pA->mB4, pA->v50[0], pA->v50[1], pA->v50[2]);
    fn_8000A144(pA->mB4, pA->pF4->obj.m0);
    Vec_Copy(pA->v20, pA->v30);
    Vec_Copy(pA->v20, pA->pF4->obj.m80[3]);
    fn_8000C5A4(pA->pF4->obj.m0);
}

void fn_8004816C(int nPlayer) {
    lbl_80281DA0->aA[nPlayer].b70 = 1;
}

// Flies the player's 'TEO ' 10004 object (DynObj_TeeAdd) once fn_8004816C launched it: a random
// heading, speed and spin, then its velocity, the wind and gravity until it lands. Until then it
// stays where it was put.
void fn_80048184(int nPlayer) {
    f32 mTmp[4][4];
    f32 vMove[4];
    f32 vWind[4];
    GoDynObjPlayerA* pA = &lbl_80281DA0->aA[nPlayer];
    f32 fRange;
    f32 fGround;

    if (0.0f == gSession.fFrameTime) return;
    if (!pA->bF8) return;
    if (pA->b70 && pA->b0) {
        pA->b0 = 0;
        if (pA->bF9) {
            pA->fC = -gPlayers[nPlayer].fAim + Misc_RandFuncf(1) - 0.5f;
            fn_8000ADC0(pA->mB4);
            fn_8000A194(pA->mB4, pA->fC, 0.0f, 0.0f);
            fn_800BADF8(pA->pF4->obj.m0, pA->mB4, mTmp, 4);
            fn_8000A0E8(mTmp, pA->pF4->obj.m0);
            fn_8000C5A4(pA->pF4->obj.m0);
            Vec_Copy(pA->v20, pA->v30);
            fn_8004858C(pA->v40, pA->fC, lbl_80281DA0->fA9C);
            fn_8000AE28(lbl_80281DA0->fA94 * Misc_RandFuncf(1), pA->v40, pA->v40);
            pA->v40[3] = pA->v40[1];
            pA->bF9 = 0;
            pA->b0 = 1;
            pA->f10 = 0.0f;
            fRange = lbl_80281DA0->fAA4;
            pA->v60[0] = fRange * Misc_RandFuncf(1) - 0.5f * fRange;
            fRange = lbl_80281DA0->fAA4;
            pA->v60[1] = fRange * Misc_RandFuncf(1) - 0.5f * fRange;
            fRange = lbl_80281DA0->fAA4;
            pA->v60[2] = fRange * Misc_RandFuncf(1) - 0.5f * fRange;
            pA->v50[0] = 0.0f;
            pA->v50[1] = 0.0f;
            pA->v50[2] = 0.0f;
            return;
        }
        pA->f10 += gSession.fFrameTime;
        Wind_Get(vWind);
        fn_8000AE28(0.48888f, vWind, vWind);
        fn_800486C8(vWind, pA->v40, vMove);
        fn_8001EF34(pA->f10, vMove, vMove);
        vMove[1] = vMove[1] + -4.9f * pA->f10 * pA->f10;
        fn_800486C8(vMove, pA->v20, pA->v30);
        pA->b0 = 1;
        fGround = fn_8004D5F0(fn_8000C594(), pA->v30);
        if (pA->v30[1] < fGround) {
            pA->v30[1] = 0.01f + fGround;
            pA->b0 = 0;
        }
        fn_800486A4(pA->v60, pA->v50, pA->v50);
        fn_8000ADC0(pA->mB4);
        fn_8000A194(pA->mB4, pA->fC + pA->v50[0], pA->v50[1], pA->v50[2]);
        fn_8000A144(pA->mB4, pA->pF4->obj.m0);
        fn_8000C5A4(pA->pF4->obj.m0);
        Vec_Copy(pA->v30, pA->pF4->obj.m80[3]);
        pA->pF4->obj.m80[3][3] = 1.0f;
    } else {
        fn_8000ADC0(pA->mB4);
        fn_8000A194(pA->mB4, pA->v50[0], pA->v50[1], pA->v50[2]);
        fn_8000A144(pA->mB4, pA->pF4->obj.m0);
        fn_8000C5A4(pA->pF4->obj.m0);
        Vec_Copy(pA->v30, pA->pF4->obj.m80[3]);
        pA->pF4->obj.m80[3][3] = 1.0f;
    }
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

// A direction (x, y, z, 0): straight up tilted by fTilt, towards the heading fTurn.
void fn_8004858C(f32* pOut, f32 fTurn, f32 fTilt) {
    f32 fSinTilt;
    f32 fCosTilt;
    f32 fSinTurn;
    f32 fCosTurn;

    if (0.0f == fTilt) {
        pOut[1] = 1.0f;
        pOut[2] = 0.0f;
        pOut[0] = 0.0f;
    } else {
        fSinTilt = fn_800095F0(fTilt);
        fCosTilt = fn_80009638(fTilt);
        if (0.0f == fTurn) {
            pOut[0] = 0.0f;
            pOut[1] = fCosTilt;
            pOut[2] = fSinTilt;
        } else {
            fSinTurn = fn_800095F0(fTurn);
            fCosTurn = fn_80009638(fTurn);
            pOut[1] = fCosTilt;
            pOut[0] = fSinTilt * fSinTurn;
            pOut[2] = fSinTilt * fCosTurn;
        }
    }
    pOut[3] = 0.0f;
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
