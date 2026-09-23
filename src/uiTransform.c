// uiTransform.c (EA's name, from its asserts; also in EA's 2002 source tree): the menu UI's
// transform stack (lbl_80281F38). Pushing a UI element multiplies its move, rotation and scale
// into a copy of the current level; the current level is also kept in lbl_801D91FC.

#include "golfer.h"
#include "game/frontend.h"

f32  fn_80014280(f32 x);                // tan

// Multiply a translation, a scale or a rotation (radians, one per axis) into a level's matrix.
void fn_80092CE8(UITransform* p, f32 x, f32 y, f32 z);
void fn_80092D68(UITransform* p, f32 x, f32 y, f32 z);
void fn_80092DE8(UITransform* p, f32 fAngle);
void fn_80092E74(UITransform* p, f32 fAngle);
void fn_80092F00(UITransform* p, f32 fAngle);

void         fn_80092F8C(UITransformDesc* p);
f32*         fn_80093268(void);
UITransform* fn_80093274(void);
void         fn_80093280(int nOp, UITransformDesc* p);

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f and then 0.0f (0x80283C20), before the constants the functions below use first; its body is
// unknown.
static void uiTransform_StrippedFn(f32* p) {
    p[0] = 1.0f;
    p[1] = 0.0f;
}

// Apply an element's transform to the current level: move it, then rotate and scale it about its
// pivot, keeping the running totals.
void fn_80092F8C(UITransformDesc* p) {
    fn_80092CE8(&UI_TRANSFORM_TOP, p->vMove[0], p->vMove[1], p->vMove[2]);
    fn_80092CE8(&UI_TRANSFORM_TOP, p->vPivot[0], p->vPivot[1], p->vPivot[2]);
    UI_TRANSFORM_TOP.f74 += p->vMove[0];
    UI_TRANSFORM_TOP.f78 += p->vMove[1];
    UI_TRANSFORM_TOP.f64 += p->vPivot[0];
    UI_TRANSFORM_TOP.f68 += p->vPivot[1];
    fn_80092DE8(&UI_TRANSFORM_TOP, PI * p->vRotate[0] / 180.0f);
    fn_80092E74(&UI_TRANSFORM_TOP, PI * p->vRotate[1] / 180.0f);
    UI_TRANSFORM_TOP.f60 += PI * p->vRotate[2] / 180.0f;
    fn_80092F00(&UI_TRANSFORM_TOP, PI * p->vRotate[2] / 180.0f);
    UI_TRANSFORM_TOP.f6C *= p->vScale[0];
    UI_TRANSFORM_TOP.f70 *= p->vScale[1];
    fn_80092D68(&UI_TRANSFORM_TOP, p->vScale[0], p->vScale[1], p->vScale[2]);
    fn_80092CE8(&UI_TRANSFORM_TOP, -p->vPivot[0], -p->vPivot[1], -p->vPivot[2]);
    UI_TRANSFORM_TOP.w40 = p->w34;
    UI_TRANSFORM_TOP.f50[0] = p->f44[0] / 511.0f;
    UI_TRANSFORM_TOP.f50[1] = p->f44[1] / 511.0f;
    UI_TRANSFORM_TOP.f50[2] = p->f44[2] / 511.0f;
    UI_TRANSFORM_TOP.f5C += p->f44[3] / 511.0f;
}

f32* fn_80093268(void) {
    return lbl_801D91F0;
}

UITransform* fn_80093274(void) {
    return &lbl_801D91FC;
}

// The front end's transform callback. 0: reset the current level's totals and set up the view
// (a 50-degree field of view on a 512-wide screen); 1: push an element's transform; 2: pop;
// 3: clear the element's n30.
void fn_80093280(int nOp, UITransformDesc* p) {
    switch (nOp) {
    case 0:
        UI_TRANSFORM_TOP.f60 = 0.0f;
        UI_TRANSFORM_TOP.f5C = 0.0f;
        UI_TRANSFORM_TOP.f64 = 0.0f;
        UI_TRANSFORM_TOP.f68 = 0.0f;
        UI_TRANSFORM_TOP.f74 = 0.0f;
        UI_TRANSFORM_TOP.f78 = 0.0f;
        UI_TRANSFORM_TOP.f6C = 1.0f;
        UI_TRANSFORM_TOP.f70 = 1.0f;
        lbl_801D91F0[0] = 0.87266463f;
        lbl_801D91F0[1] = fn_80014280(0.5f * lbl_801D91F0[0]);
        lbl_801D91F0[2] = 256.0f * (1.0f / lbl_801D91F0[1]);
        break;
    case 1:
        lbl_80281F38->aLevel[lbl_80281F38->nTop + 1] = UI_TRANSFORM_TOP;
        lbl_80281F38->nTop++;
        fn_80092F8C(p);
        lbl_801D91FC = UI_TRANSFORM_TOP;
        break;
    case 2:
        lbl_80281F38->nTop--;
        lbl_801D91FC = UI_TRANSFORM_TOP;
        break;
    case 3:
        p->n30 = 0;
        break;
    }
}
