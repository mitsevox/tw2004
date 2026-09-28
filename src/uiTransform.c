// uiTransform.c (EA's name, from its asserts; also in EA's 2002 source tree): the menu UI's
// transform stack (gpUITransformStack). Pushing a UI element multiplies its move, rotation and scale
// into a copy of the current level; the current level is also kept in gUICurTransform.

#include "golfer.h"
#include "game/frontend.h"

// The file's globals, defined last-address-first (the compiler lays .bss out in reverse).
UITransform       gUICurTransform;         // a copy of the current level
f32               gUIViewParams[3];      // the UI view: field of view, tan of half of it, distance
UITransformStack* gpUITransformStack;         // the stack (UITransform_Init)

f32  Math_Tan(f32 x);                // tan

// 4x4 matrix helpers (the engine's; declared here until their own files are written).
void LLMath_IdentifyMat(f32 m[4][4]);                                          // identity
void LLMath_mat44fltMultiplyList(f32 a[4][4], f32 b[4][4], f32 out[4][4], int nRows); // out = a x b, 4 rows
void LLMath_mat44fltMultiplyList33(f32 a[4][4], f32 b[4][4], f32 out[4][4], int nRows); // out = a x b, 3 rows
void LLMath_CopyMat44(f32 src[4][4], f32 dst[4][4]);                         // copy 4 rows
void LLMath_CopyMat34(f32 src[4][4], f32 dst[4][4]);                         // copy 3 rows

// Multiply a translation, a scale or a rotation (radians, about x, y or z) into a level's matrix.
void UITransform_Translate(UITransform* p, f32 x, f32 y, f32 z);
void UITransform_Scale(UITransform* p, f32 x, f32 y, f32 z);
void UITransform_RotateX(UITransform* p, f32 fAngle);
void UITransform_RotateY(UITransform* p, f32 fAngle);
void UITransform_RotateZ(UITransform* p, f32 fAngle);

void         UITransform_Apply(UITransformDesc* p);
f32*         UITransform_GetViewParams(void);
void         UITransform_Shutdown(void);

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f and then 0.0f (0x80283C20), before the constants the functions below use first; its body is
// unknown.
static void uiTransform_StrippedFn(f32* p) {
    p[0] = 1.0f;
    p[1] = 0.0f;
}

// Multiply a translation by (x, y, z) into level p's matrix: p->m becomes p->m times a matrix whose
// row 3 is the move.
void UITransform_Translate(UITransform* p, f32 x, f32 y, f32 z) {
    f32 mMove[4][4];
    f32 mOut[4][4];
    LLMath_IdentifyMat(mMove);
    mMove[3][0] = x;
    mMove[3][1] = y;
    mMove[3][2] = z;
    LLMath_mat44fltMultiplyList(p->m, mMove, mOut, 4);
    LLMath_CopyMat44(mOut, p->m);
}

// Multiply a scale by (x, y, z) into level p's matrix; only its first three rows are multiplied and
// written back, so the translation row is kept.
void UITransform_Scale(UITransform* p, f32 x, f32 y, f32 z) {
    f32 mScale[4][4];
    f32 mOut[4][4];
    LLMath_IdentifyMat(mScale);
    mScale[0][0] = x;
    mScale[1][1] = y;
    mScale[2][2] = z;
    LLMath_mat44fltMultiplyList33(p->m, mScale, mOut, 3);
    LLMath_CopyMat34(mOut, p->m);
}

// Multiply a rotation of fAngle radians about the x axis into level p's matrix (the first three
// rows only, as UITransform_Scale).
void UITransform_RotateX(UITransform* p, f32 fAngle) {
    f32 mRot[4][4];
    f32 mOut[4][4];
    f32 fSin;
    f32 fCos;
    LLMath_IdentifyMat(mRot);
    fSin = Math_Sin(fAngle);
    fCos = Math_Cos(fAngle);
    mRot[1][1] = fCos;
    mRot[1][2] = fSin;
    mRot[2][1] = -fSin;
    mRot[2][2] = fCos;
    LLMath_mat44fltMultiplyList33(p->m, mRot, mOut, 3);
    LLMath_CopyMat34(mOut, p->m);
}

// Multiply a rotation of fAngle radians about the y axis into level p's matrix (the first three
// rows only, as UITransform_Scale).
void UITransform_RotateY(UITransform* p, f32 fAngle) {
    f32 mRot[4][4];
    f32 mOut[4][4];
    f32 fSin;
    f32 fCos;
    LLMath_IdentifyMat(mRot);
    fSin = Math_Sin(fAngle);
    fCos = Math_Cos(fAngle);
    mRot[0][0] = fCos;
    mRot[0][2] = -fSin;
    mRot[2][0] = fSin;
    mRot[2][2] = fCos;
    LLMath_mat44fltMultiplyList33(p->m, mRot, mOut, 3);
    LLMath_CopyMat34(mOut, p->m);
}

// Multiply a rotation of fAngle radians about the z axis into level p's matrix (the first three
// rows only, as UITransform_Scale).
void UITransform_RotateZ(UITransform* p, f32 fAngle) {
    f32 mRot[4][4];
    f32 mOut[4][4];
    f32 fSin;
    f32 fCos;
    LLMath_IdentifyMat(mRot);
    fSin = Math_Sin(fAngle);
    fCos = Math_Cos(fAngle);
    mRot[0][0] = fCos;
    mRot[0][1] = fSin;
    mRot[1][0] = -fSin;
    mRot[1][1] = fCos;
    LLMath_mat44fltMultiplyList33(p->m, mRot, mOut, 3);
    LLMath_CopyMat34(mOut, p->m);
}

// Apply UI element p's transform to the stack's current level: move by vMove, then rotate (vRotate,
// in degrees: about x, then y, then z) and scale (vScale) about the pivot vPivot. The level's
// running totals add the move and the pivot (x and y) and the z rotation, and multiply the scale (x
// and y). The level also takes the element's w34 (as w40) and its f44 over 511 (f50[0..2]; f44[3] /
// 511 is added to f5C).
void UITransform_Apply(UITransformDesc* p) {
    UITransform_Translate(&gpUITransformStack->aLevel[gpUITransformStack->nTop], p->vMove[0], p->vMove[1],
                          p->vMove[2]);
    UITransform_Translate(&gpUITransformStack->aLevel[gpUITransformStack->nTop], p->vPivot[0], p->vPivot[1],
                          p->vPivot[2]);
    gpUITransformStack->aLevel[gpUITransformStack->nTop].f74 += p->vMove[0];
    gpUITransformStack->aLevel[gpUITransformStack->nTop].f78 += p->vMove[1];
    gpUITransformStack->aLevel[gpUITransformStack->nTop].f64 += p->vPivot[0];
    gpUITransformStack->aLevel[gpUITransformStack->nTop].f68 += p->vPivot[1];
    UITransform_RotateX(&gpUITransformStack->aLevel[gpUITransformStack->nTop], PI * p->vRotate[0] / 180.0f);
    UITransform_RotateY(&gpUITransformStack->aLevel[gpUITransformStack->nTop], PI * p->vRotate[1] / 180.0f);
    gpUITransformStack->aLevel[gpUITransformStack->nTop].f60 += PI * p->vRotate[2] / 180.0f;
    UITransform_RotateZ(&gpUITransformStack->aLevel[gpUITransformStack->nTop], PI * p->vRotate[2] / 180.0f);
    gpUITransformStack->aLevel[gpUITransformStack->nTop].f6C *= p->vScale[0];
    gpUITransformStack->aLevel[gpUITransformStack->nTop].f70 *= p->vScale[1];
    UITransform_Scale(&gpUITransformStack->aLevel[gpUITransformStack->nTop], p->vScale[0], p->vScale[1],
                      p->vScale[2]);
    UITransform_Translate(&gpUITransformStack->aLevel[gpUITransformStack->nTop], -p->vPivot[0],
                          -p->vPivot[1], -p->vPivot[2]);
    gpUITransformStack->aLevel[gpUITransformStack->nTop].w40 = p->w34;
    gpUITransformStack->aLevel[gpUITransformStack->nTop].f50[0] = p->f44[0] / 511.0f;
    gpUITransformStack->aLevel[gpUITransformStack->nTop].f50[1] = p->f44[1] / 511.0f;
    gpUITransformStack->aLevel[gpUITransformStack->nTop].f50[2] = p->f44[2] / 511.0f;
    gpUITransformStack->aLevel[gpUITransformStack->nTop].f5C += p->f44[3] / 511.0f;
}

// The UI view (gUIViewParams, set by op 0 of UITransform_HandleOp): the field of view in radians,
// the tan of half of it, and the distance at which the view is 512 units wide.
f32* UITransform_GetViewParams(void) {
    return gUIViewParams;
}

// A copy of the transform stack's current level (gUICurTransform), which the UI's own drawing uses
// (fe_movies.c, uiText.c, uiArc.c).
UITransform* UITransform_GetCurrent(void) {
    return &gUICurTransform;
}

// The studio's transform callback (UISTransformFncT, registered by UI_OpenInterface). Op 0: reset
// the current level's running totals (the moves, pivots, z rotation and f5C to 0, the scales to 1)
// and set up the view: a 50-degree field of view, 512 units wide at distance 256 / tan(25 degrees)
// (gUIViewParams). Op 1: push element p's transform (a copy of the current level, then
// UITransform_Apply); nothing checks the stack's eight levels. Op 2: pop. Op 3: clear p->n30. After
// a push or a pop gUICurTransform holds the new current level.
void UITransform_HandleOp(int nOp, UITransformDesc* p) {
    switch (nOp) {
    case 0:
        gpUITransformStack->aLevel[gpUITransformStack->nTop].f60 = 0.0f;
        gpUITransformStack->aLevel[gpUITransformStack->nTop].f5C = 0.0f;
        gpUITransformStack->aLevel[gpUITransformStack->nTop].f64 = 0.0f;
        gpUITransformStack->aLevel[gpUITransformStack->nTop].f68 = 0.0f;
        gpUITransformStack->aLevel[gpUITransformStack->nTop].f74 = 0.0f;
        gpUITransformStack->aLevel[gpUITransformStack->nTop].f78 = 0.0f;
        gpUITransformStack->aLevel[gpUITransformStack->nTop].f6C = 1.0f;
        gpUITransformStack->aLevel[gpUITransformStack->nTop].f70 = 1.0f;
        gUIViewParams[0] = DEG(50.0f);
        gUIViewParams[1] = Math_Tan(0.5f * gUIViewParams[0]);
        gUIViewParams[2] = 256.0f * (1.0f / gUIViewParams[1]);
        break;
    case 1:
        gpUITransformStack->aLevel[gpUITransformStack->nTop + 1] = gpUITransformStack->aLevel[gpUITransformStack->nTop];
        gpUITransformStack->nTop++;
        UITransform_Apply(p);
        gUICurTransform = gpUITransformStack->aLevel[gpUITransformStack->nTop];
        break;
    case 2:
        gpUITransformStack->nTop--;
        gUICurTransform = gpUITransformStack->aLevel[gpUITransformStack->nTop];
        break;
    case 3:
        p->n30 = 0;
        break;
    }
}

// Allocate the UI's transform stack (gpUITransformStack; UI_OpenInterface) and set its bottom
// level's matrix to identity; the level's other fields are left as allocated.
void UITransform_Init(void) {
    gpUITransformStack = StaticMem_Alloc(sizeof(UITransformStack), 2, 16, "uiTransform.c", 203);
    gpUITransformStack->nTop = 0;
    LLMath_IdentifyMat(gpUITransformStack->aLevel[gpUITransformStack->nTop].m);
}

// Free the UI's transform stack (UI_vCloseModule).
void UITransform_Shutdown(void) {
    StaticMem_Free(gpUITransformStack);
    gpUITransformStack = NULL;
}
