// GoShaderObject_Rain_Gc.c (EA's name, from its asserts; also in EA's 2002 source tree; TW06): the
// rain shader object. Partly decompiled.

#include "game_types.h"
#include "engine.h"
#include "rain.h"

void fn_800B4B5C(void);
void fn_800B4BB0(void);
void fn_800B4BB4(void);
void fn_800B4BB8(void);
void fn_800B4BD8(void);
void fn_800B4BFC(void);
void fn_800B4C00(RainList* pList, int nDrops);
void SD_vShaderObject_Rain_Dynamic_Init(RainObject* pRain, f32* pStrength);
void fn_800B4F24(RainObject* pRain);

void fn_800B4B5C(void) {
    lbl_802814B8->n0 = 0;
    lbl_802814B8->n4 = 0;
    lbl_802814B8->f8 = fn_80029B64(3075.0f);
    lbl_802814B8->pBank = NULL;
    lbl_802814B8->pTex = NULL;
}

void fn_800B4BB0(void) {
}

void fn_800B4BB4(void) {
}

void fn_800B4BB8(void) {
    lbl_802814B8->n0 = 0;
    lbl_802814B8->n4 = 1 - lbl_802814B8->n4;
}

void fn_800B4BD8(void) {
    lbl_802814B8->n0 = (lbl_802814B8->n0 + 1) % 2;
}

void fn_800B4BFC(void) {
}

// Set up a rain object: the drops' display list and its cleared buffers, and find the "splash"
// texture. pStrength is unused (PsMgr.c, the only caller, passes it).
void SD_vShaderObject_Rain_Dynamic_Init(RainObject* pRain, f32* pStrength) {
    RainData* pData = &pRain->data;
    u64 uSplash;
    int i;

    fn_800B4C00(&pData->list, 900);
    for (i = 0; i < 4; i++) {
        pData->apA[i] = fn_80009B34(RAIN_BUF_A_SIZE, 2, 0x20, "GoShaderObject_Rain_Gc.c", 280);
        memset(pData->apA[i], 0, RAIN_BUF_A_SIZE);
    }
    for (i = 0; i < 2; i++) {
        pData->apB[i] = fn_80009B34(RAIN_BUF_B_SIZE, 2, 0x20, "GoShaderObject_Rain_Gc.c", 292);
        memset(pData->apB[i], 0, RAIN_BUF_B_SIZE);
    }
    uSplash = fn_8000BEE4("splash");
    fn_800102DC(uSplash, &lbl_802814B8->pBank, &lbl_802814B8->pTex);
}

// Free a rain object's list and buffers.
void fn_800B4F24(RainObject* pRain) {
    RainData* pData = &pRain->data;
    int i;

    fn_80009E70(pData->list.pList);
    for (i = 0; i < 4; i++) {
        fn_80009E70(pData->apA[i]);
    }
    for (i = 0; i < 2; i++) {
        fn_80009E70(pData->apB[i]);
    }
}

// ---- sweep code (not yet cleaned up) ----

void fn_800B58B4(s32 p0);
void fn_800B58C0(f32 farg0, f32 farg1, f32 farg2);
void fn_800B58D4(f32 farg0, f32 farg1);
void fn_800B58E4(s32 p0, s32 p1, s32 p2, s32 p3);

void fn_800B58B4(s32 p0) {
    *(volatile u8*)0xCC008000 = p0;
}

void fn_800B58C0(f32 farg0, f32 farg1, f32 farg2) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
    *(f32* )0xCC008000 = farg2;
}

void fn_800B58D4(f32 farg0, f32 farg1) {
    *(f32* )0xCC008000 = farg0;
    *(f32* )0xCC008000 = farg1;
}

void fn_800B58E4(s32 p0, s32 p1, s32 p2, s32 p3) {
    *(volatile u8*)0xCC008000 = p0;
    *(volatile u8*)0xCC008000 = p1;
    *(volatile u8*)0xCC008000 = p2;
    *(volatile u8*)0xCC008000 = p3;
}

// ---- end of sweep code ----
