// PsMgr.c (EA's name, from its asserts; also in EA's 2002 source tree): the manager of the weather
// particle effects (include/psmgr.h). Only kind 0, the rain, is made on the GameCube: fn_800A2A80
// creates it and dims the hole's lights to match, fn_800A2BBC and fn_800A2C08 run and draw it.

#include "psmgr.h"
#include "golfer.h"

void fn_800A27FC(f32 fLight1, f32 fLight2);
void fn_800A29B4(UStreamObject* pObject);

// GoShaderObject_Rain_Gc.c
void SD_vShaderObject_Rain_Dynamic_Init(void* pRain, f32* pStrength);
void fn_800B4F24(void* pRain);
void fn_800B4FA4(void* pRain);
void fn_800B52D4(void* pRain, f32* pFrameTime, s32 n);

void fn_800A2934(void) {
    int i;
    lbl_802813F8->n4 = 0;
    for (i = 0; i < PS_NUM_KINDS; i++) {
        lbl_801F16F4[i] = NULL;
    }
}

void fn_800A2958(void) {
}

void fn_800A295C(void) {
    UStream_RegisterHandler('sfxd', fn_800A29B4);
}

void fn_800A298C(void) {
    UStream_UnregisterHandler('sfxd');
}

s32 fn_800A2A80(s32 nKind, f32* pArg, s32 n3) {
    f32 fStrength;
    switch (nKind) {
    case 0:
        lbl_801F16F4[0] = fn_80009B34(0x28, 2, 32, "PsMgr.c", 530);    // the rain object
        if (pArg != NULL) {
            fStrength = *pArg;
        } else {
            fStrength = 0.9f;
        }
        SD_vShaderObject_Rain_Dynamic_Init(lbl_801F16F4[0], &fStrength);
        fn_800A27FC(1.0f + 0.55f * fStrength, 0.45f + 0.55f * fStrength);
        break;
    case 1:
        break;
    }
    return 0;
}

void fn_800A2B34(s32 nKind) {
    switch (nKind) {
    case 0:
        if (lbl_801F16F4[nKind] != NULL) {
            fn_800B4F24(lbl_801F16F4[nKind]);
            fn_80009E70(lbl_801F16F4[nKind]);
            lbl_801F16F4[nKind] = NULL;
        }
        break;
    case 1:
        break;
    }
}

void fn_800A2BA8(void) {
    lbl_802813F8->n4 = 1 - lbl_802813F8->n4;
}

void fn_800A2BBC(int nView) {
    f32 fFrameTime;
    if (lbl_801F16F4[0] != NULL) {
        fFrameTime = gSession.fFrameTime;
        fn_800B52D4(lbl_801F16F4[0], &fFrameTime, 0);
    }
}

void fn_800A2C08(int nView) {
    if (lbl_801F16F4[0] != NULL) {
        fn_800B4FA4(lbl_801F16F4[0]);
    }
}
