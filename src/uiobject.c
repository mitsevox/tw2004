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
void fn_8007644C(CamLens* pLens);    // GoCamera.c: free a lens
void fn_800AE338(void);
void fn_800AE380(void);
void fn_800AF0A8(s32 v);

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

void fn_800AF0A8(s32 v) {
    fn_80013E38((u8*)*lbl_80280DF0, v);
}
