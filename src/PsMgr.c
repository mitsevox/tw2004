// PsMgr.c (EA's name, from its asserts; also in EA's 2002 source tree): the manager of the weather
// particle effects (include/psmgr.h). Only kind 0, the rain, is made on the GameCube: fn_800A2A80
// creates it and rescales the hole's light colours to match (directional x 1 + 0.55 s, point
// x 0.45 + 0.55 s: brighter at the default 0.9), fn_800A2BBC and fn_800A2C08 run and draw it.

#include "psmgr.h"
#include "golfer.h"
#include "lighting.h"

void fn_800A27FC(f32 fLight1, f32 fLight2);
void fn_800A29B4(UStreamObject* pObject);

// GoShaderObject_Rain_Gc.c
void SD_vShaderObject_Rain_Dynamic_Init(void* pRain, f32* pStrength);
void fn_800B4F24(void* pRain);
void fn_800B4FA4(void* pRain);
void fn_800B52D4(void* pRain, f32* pFrameTime, s32 n);

// Scales the colour of the hole's directional lights by fDir and of its point lights by fPoint,
// then hands the lights to the light sets again.
void fn_800A27FC(f32 fDir, f32 fPoint) {
    int i;

    for (i = 0; i < fn_8000C594()->lights.nLights; i++) {
        if (fn_8000C594()->lights.aLight[i].nType == 1) {
            fn_8000AE28(fn_8000C594()->lights.aLight[i].vColor, fDir,
                        fn_8000C594()->lights.aLight[i].vColor);
        } else if (fn_8000C594()->lights.aLight[i].nType == 2) {
            fn_8000AE28(fn_8000C594()->lights.aLight[i].vColor, fPoint,
                        fn_8000C594()->lights.aLight[i].vColor);
        }
    }
    fn_80035338(0);
    fn_800935CC(&fn_8000C594()->lights);
    fn_80093900(fn_8000C594()->p38);
    if (fn_8000C594()->p44 != NULL) {
        fn_80035338(2);
        fn_80093900(fn_8000C594()->p44);
        fn_80035308();
        fn_800352E4();
    }
}

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
    Stream_RegisterLoadChunkCallback('sfxd', fn_800A29B4);
}

void fn_800A298C(void) {
    Stream_UnregisterLoadChunkCallback('sfxd');
}

// The 'sfxd' stream handler: each word (sscanf %s) after an '=' before the first '}' of its text
// goes into lbl_801F1640 in turn.
void fn_800A29B4(UStreamObject* pObject) {
    char* pEnd;
    char* p;
    int i;
    char szName[16];

    p = (char*)pObject->pData;
    pEnd = strchr(p, '}');
    p = strchr(p + 1, '=');
    i = 0;
    while (p != NULL && p < pEnd) {
        p++;
        sscanf(p, "%s", szName);
        p += strlen(szName);
        p = strchr(p, '=');
        strcpy(lbl_801F1640[i], szName);
        i++;
    }
    fn_80009E70(pObject);
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
    case 1:
        return;
    case 0:
        if (lbl_801F16F4[nKind] != NULL) {
            fn_800B4F24(lbl_801F16F4[nKind]);
            fn_80009E70(lbl_801F16F4[nKind]);
            lbl_801F16F4[nKind] = NULL;
        }
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
