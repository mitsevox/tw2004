// Skin.c (EA's name, from its asserts; also in EA's 2002 source tree): not yet decompiled; the
// sweep code below is the matched small functions.

#include "game_types.h"
#include "engine.h"
#include "camera.h"
#include "lighting.h"
#include "dynobj.h"
#include "charstate.h"

void  fn_80008380(void);
void  fn_80016978(f32 fLeft, f32 fTop, f32 fWidth, f32 fHeight);
void  fn_8006E7A4(LightGroup* pGroup);         // GoLighting.c: load the group's lights
void  fn_8006EADC(UObject* pObj);              // GoLighting.c: light the object
void  fn_8006ED70(void);                       // GoLighting.c
void  fn_801127C4(void* pDesc);                // hwsMaterial_Gc.c

void  fn_80035F40(void* pCamera);
void  fn_8003612C(LightGroup* pGroup);
void  fn_80037D5C(SkinDesc* pDesc);

// ---- sweep code (tidied) ----

void fn_80035F1C(void) {
    fn_80035F40(fn_8001614C());
}

// Hands the camera's screen rectangle on to fn_80016978.
void fn_80035F40(void* pCamera) {
    f32* pRect;

    pRect = fn_80012EF0(pCamera);
    fn_80016978(fn_80012EE8(pRect), fn_80012EE0(pRect), fn_80012ED8(pRect), fn_80012ED0(pRect));
}

void fn_80035FBC(void) {
    fn_8006ED70();
}

void fn_80035FDC(UObject* pObj) {
    fn_8006EADC(pObj);
}

// Loads the current light set's lights.
void fn_80035FFC(void) {
    fn_8003612C(&fn_8003532C()->group);
}

void fn_80036024(f32 f) {
    fn_8003532C()->group.v18[0] = f;
}

void fn_800360A0(void* arg0) {
    s32 (*temp_r12)();

    temp_r12 = (*(s32 (**)())((u8*)((*(void**)((u8*)(arg0) + 0x24))) + 4));
    if (temp_r12 != NULL) {
        temp_r12(arg0);
    }
}

void fn_8003612C(LightGroup* pGroup) {
    fn_8006E7A4(pGroup);
}

// pObj's type is not known yet: the one fn_800358E0 is handed.
void fn_8003614C(void* pObj, f32* pOut) {
    if (pObj != NULL) {
        Vec_Copy((f32*)(*(u8**)(*(u8**)((u8*)pObj + 0x38) + 4) + 0x1C), pOut);
    }
}

void fn_80036460(int n) {
}

void fn_80036464(void) {
    if (lbl_80281D70 != NULL) {
        fn_80009E70(lbl_80281D70);
    }
    lbl_80281D70 = NULL;
    lbl_80281D78 = 0;
    lbl_80281D74 = 0;
}

void fn_800364A0(void) {
    lbl_80281D74 = 0;
}

// Frees what a loaded skin allocated (bit 2 of u10D4); 0 when it was not loaded.
s32 fn_80037708(Skin* pSkin) {
    SkinModel* pModel;

    if (!(pSkin->u10D4 & 2)) {
        return 0;
    }
    fn_80008380();
    pModel = pSkin->pModel;
    if (pModel != NULL && pModel->pDesc != NULL) {
        fn_80112910(pSkin->p1090);
        pSkin->p1090 = NULL;
        fn_80112910(pSkin->a1098[0]);
        pSkin->a1098[0] = NULL;
        fn_80112A58(pSkin->a10A0[0]);
        pSkin->a10A0[0] = NULL;
        fn_80037D5C(pSkin->pModel->pDesc);
    }
    ((void (*)(Skin*))fn_800CE168)(pSkin);  // port: EA passes an argument fn_800CE168 ignores
    if (pSkin->p108C != NULL) {
        fn_80009E70(pSkin->p108C);
    }
    pSkin->p108C = NULL;
    if (pSkin->p10CC != NULL) {
        fn_80009E70(pSkin->p10CC);
    }
    pSkin->p10CC = NULL;
    if (pSkin->p10D0 != NULL) {
        fn_80009E70(pSkin->p10D0);
    }
    pSkin->p10D0 = NULL;
    pSkin->u10D4 = pSkin->u10D4 & ~2;
    return 1;
}

// ---- end of sweep code ----

// ---- sweep code (tidied) ----

void fn_80037CD8(Skin* pSkin) {
    SkinModel* pModel;

    fn_80037708(pSkin);
    fn_8011CD84(pSkin);
    fn_800CD56C(pSkin);
    pModel = pSkin->pModel;
    if (pModel != NULL) {
        if (pModel->pDesc != NULL) {
            fn_801127C4(pModel->pDesc);
            fn_80009E70(pSkin->pModel->pDesc);
        }
        fn_80009E70(pSkin->pModel);
    }
    if (pSkin->p1088 != NULL) {
        fn_80009E70(pSkin->p1088);
    }
    fn_80009E70(pSkin);
}

// ---- end of sweep code ----
