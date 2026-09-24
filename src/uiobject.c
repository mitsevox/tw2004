// uiobject.c (our name, after TW06's golf/ui core/istudio runtime/uiobject.c; UI_Obj_InitModule is
// a TW06 pair): the 3D objects the in-game UI draws (TW06: the confidence-boost rings and the
// "tappa spinna" UI). Not yet decompiled; the code below is the sweep's matched small functions.

#include "game_types.h"
#include "engine.h"
#include "camera.h"
#include "dynobj.h"
#include "lighting.h"
#include "uiobject.h"

void fn_80013E38(u8* p, s32 v);  // GoRenderCtx_Gc.c
void UI_Obj_InitModule(void);
void fn_800AE338(void);
void fn_800AE380(void);
void fn_800AE3C4(void);
void fn_800AF0A8(s32 v);
void fn_800AEFE4(void);
void fn_800AF0D4(UObjMesh* pMesh);
void fn_800082CC(UObjMeshPart* pPart);
void fn_8003612C(LightGroup* pGroup);   // Skin.c: load the group's lights (fn_8006E7A4)
void fn_8006EADC(UObject* pObj);        // GoLighting.c: light the object
void fn_8006ED70(void);                 // GoLighting.c
void fn_80035098(u8 b);                 // GoTerrain.c: renderer state
void fn_80035154(u8 b);                 // GoTerrain.c: renderer state
void fn_80035294(void);                 // GoTerrain.c

// Set up the objects: their settings, lens, textures and one directional light.
void UI_Obj_InitModule(void) {
    u64 uName;

    lbl_802820D0 = NULL;
    lbl_801F5B98.a28[0] = 1.35f;
    lbl_801F5B98.a28[1] = 0.74f;
    lbl_801F5B98.a28[2] = 0.03f;
    lbl_801F5B98.a28[3] = 0.0f;
    fn_800AE380();
    lbl_802820CC = fn_80076400();
    fn_800768E0(lbl_802820CC);
    lbl_801F5B98.a0[0] = -0.345f;
    lbl_801F5B98.a0[1] = -0.23f;
    lbl_801F5B98.a0[2] = -3.13f;
    lbl_801F5B98.a0[3] = 0.0f;
    lbl_801F5B98.a0[4] = 0.0f;
    lbl_801F5B98.a0[5] = 0.0f;
    lbl_801F5B98.a0[6] = 0.00168f;
    lbl_801F5B98.a0[7] = 0.02f;
    lbl_801F5B98.a0[8] = 0.02f;
    lbl_801F5B98.a0[9] = 0.0f;
    lbl_801F5B98.a38[0] = -0.345f;
    lbl_801F5B98.a38[1] = -0.23f;
    lbl_801F5B98.a38[2] = -3.13f;
    lbl_801F5B98.a38[3] = 0.0f;
    lbl_801F5B98.a38[4] = 0.0f;
    lbl_801F5B98.a38[5] = 0.0f;
    lbl_801F5B98.a38[6] = 0.00168f;
    lbl_801F5B98.a38[7] = 0.02f;
    lbl_801F5B98.a38[8] = 0.02f;
    lbl_801F5B98.a38[9] = 0.0f;
    uName = fn_8000BEE4("toball");
    fn_800102DC(uName, &lbl_802820BC, &lbl_802820C8);
    uName = fn_8000BEE4("toball");
    fn_800102DC(uName, &lbl_802820BC, &lbl_802820C4);
    uName = fn_8000BEE4("ring");
    fn_800102DC(uName, &lbl_802820BC, &lbl_802820C0);
    fn_8006E5A8(&lbl_801F5B40, 1);
    lbl_801F5B40.apLight[0]->nType = 1;
    lbl_801F5B40.apLight[0]->u.dir.f10 = 1.0f;
    lbl_801F5B40.apLight[0]->u.dir.fC = 1.0f;
}

// Free the object, the lens and the lights.
void fn_800AE338(void) {
    if (lbl_802820D0 != NULL) {
        fn_80048860(lbl_802820D0);
    }
    lbl_802820D0 = NULL;
    fn_8007644C(lbl_802820CC);
    fn_8006E62C(&lbl_801F5B40);
}

// Make the object from its 'TEO ' model once that has streamed in.
// port: a 'TEO ' object's UStreamObject.uUnk4 holds its model (see rcmp_mad_codec.c fn_800B9B48).
void fn_800AE380(void) {
    if (lbl_802820D0 == NULL) {
        lbl_802820D0 = fn_80048808((UObjModel*)fn_8000B70C('TEO ', 10003)->uUnk4);
    }
}

void fn_800AE3C4(void) {
    lbl_801F5B78[0] = lbl_801F5B98.a28[3];
    lbl_801F5B78[1] = lbl_801F5B98.a28[3];
    lbl_801F5B78[2] = lbl_801F5B98.a28[3];
    lbl_801F5B78[3] = lbl_801F5B98.a28[3];
    lbl_801F5B78[4] = lbl_801F5B98.a28[3];
    lbl_801F5B78[5] = lbl_801F5B98.a28[3];
    lbl_801F5B78[6] = lbl_801F5B98.a28[3];
    lbl_801F5B78[7] = lbl_801F5B98.a28[3];
}

// Draw the object: its light's colour, the renderer state, the view, then the model.
void fn_800AEFE4(void) {
    lbl_801F5B40.apLight[0]->u.dir.vColor[0] = lbl_802820B8;
    lbl_801F5B40.apLight[0]->u.dir.vColor[1] = lbl_80281470;
    lbl_801F5B40.apLight[0]->u.dir.vColor[2] = lbl_80281474;
    lbl_801F5B40.apLight[0]->u.dir.vColor[3] = 0.0f;
    fn_8003612C(&lbl_801F5B40);
    fn_8006EADC(lbl_802820D0);
    fn_80035118(4, 5);
    fn_80035098(1);
    fn_80035154(255.0f * lbl_80281478);
    fn_80012EF8();
    fn_80035240(lbl_802820D0->m80);
    fn_80035294();
    fn_80016B9C();
    fn_80035138(1);
    fn_80012EF8();
    fn_800AF0D4(lbl_802820D0->pModel->apLod[0]);
    fn_8006ED70();
}

void fn_800AF0A8(s32 v) {
    fn_80013E38((u8*)*lbl_80280DF0, v);
}

// Draw the mesh's current part, if it is used (UObject.c's fn_80048A84 again).
void fn_800AF0D4(UObjMesh* pMesh) {
    if (pMesh->a1C[pMesh->n28] != 0) {
        fn_800082CC(&pMesh->p18[pMesh->n28]);
    }
}
