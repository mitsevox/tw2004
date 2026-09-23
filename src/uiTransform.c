// uiTransform.c (EA's name, from its asserts; also in EA's 2002 source tree): the menu UI's
// transform stack (lbl_80281F38). Pushing a UI element multiplies its move, rotation and scale
// into a copy of the current level; the current level is also kept in lbl_801D91FC.

#include "golfer.h"
#include "game/frontend.h"

// The file's globals, defined last-address-first (the compiler lays .bss out in reverse).
UITransform       lbl_801D91FC;         // a copy of the current level
f32               lbl_801D91F0[3];      // the UI view: field of view, tan of half of it, distance
UITransformStack* lbl_80281F38;         // the stack (fn_8009349C)

f32  fn_80014280(f32 x);                // tan

// 4x4 matrix helpers (the engine's; declared here until their own files are written).
void fn_8000ADC0(f32 m[4][4]);                                          // identity
void fn_800BADF8(f32 a[4][4], f32 b[4][4], f32 out[4][4], int nRows);   // out = a x b, 4 rows
void fn_800BAE5C(f32 a[4][4], f32 b[4][4], f32 out[4][4], int nRows);   // out = a x b, 3 rows
void fn_8000A0E8(f32 src[4][4], f32 dst[4][4]);                         // copy 4 rows
void fn_8000A144(f32 src[4][4], f32 dst[4][4]);                         // copy 3 rows

// Multiply a translation, a scale or a rotation (radians, about x, y or z) into a level's matrix.
void fn_80092CE8(UITransform* p, f32 x, f32 y, f32 z);
void fn_80092D68(UITransform* p, f32 x, f32 y, f32 z);
void fn_80092DE8(UITransform* p, f32 fAngle);
void fn_80092E74(UITransform* p, f32 fAngle);
void fn_80092F00(UITransform* p, f32 fAngle);

void         fn_80092F8C(UITransformDesc* p);
f32*         fn_80093268(void);
UITransform* fn_80093274(void);
void         fn_80093280(int nOp, UITransformDesc* p);
void         fn_8009349C(void);
void         fn_800934F8(void);

// fake match: stands in for a function the original linker stripped. The file's pool starts with
// 1.0f and then 0.0f (0x80283C20), before the constants the functions below use first; its body is
// unknown.
static void uiTransform_StrippedFn(f32* p) {
    p[0] = 1.0f;
    p[1] = 0.0f;
}

void fn_80092CE8(UITransform* p, f32 x, f32 y, f32 z) {
    f32 mMove[4][4];
    f32 mOut[4][4];
    fn_8000ADC0(mMove);
    mMove[3][0] = x;
    mMove[3][1] = y;
    mMove[3][2] = z;
    fn_800BADF8(p->m, mMove, mOut, 4);
    fn_8000A0E8(mOut, p->m);
}

void fn_80092D68(UITransform* p, f32 x, f32 y, f32 z) {
    f32 mScale[4][4];
    f32 mOut[4][4];
    fn_8000ADC0(mScale);
    mScale[0][0] = x;
    mScale[1][1] = y;
    mScale[2][2] = z;
    fn_800BAE5C(p->m, mScale, mOut, 3);
    fn_8000A144(mOut, p->m);
}

void fn_80092DE8(UITransform* p, f32 fAngle) {
    f32 mRot[4][4];
    f32 mOut[4][4];
    f32 fSin;
    f32 fCos;
    fn_8000ADC0(mRot);
    fSin = fn_800095F0(fAngle);
    fCos = fn_80009638(fAngle);
    mRot[1][1] = fCos;
    mRot[1][2] = fSin;
    mRot[2][1] = -fSin;
    mRot[2][2] = fCos;
    fn_800BAE5C(p->m, mRot, mOut, 3);
    fn_8000A144(mOut, p->m);
}

void fn_80092E74(UITransform* p, f32 fAngle) {
    f32 mRot[4][4];
    f32 mOut[4][4];
    f32 fSin;
    f32 fCos;
    fn_8000ADC0(mRot);
    fSin = fn_800095F0(fAngle);
    fCos = fn_80009638(fAngle);
    mRot[0][0] = fCos;
    mRot[0][2] = -fSin;
    mRot[2][0] = fSin;
    mRot[2][2] = fCos;
    fn_800BAE5C(p->m, mRot, mOut, 3);
    fn_8000A144(mOut, p->m);
}

void fn_80092F00(UITransform* p, f32 fAngle) {
    f32 mRot[4][4];
    f32 mOut[4][4];
    f32 fSin;
    f32 fCos;
    fn_8000ADC0(mRot);
    fSin = fn_800095F0(fAngle);
    fCos = fn_80009638(fAngle);
    mRot[0][0] = fCos;
    mRot[0][1] = fSin;
    mRot[1][0] = -fSin;
    mRot[1][1] = fCos;
    fn_800BAE5C(p->m, mRot, mOut, 3);
    fn_8000A144(mOut, p->m);
}

// Apply an element's transform to the current level: move it, then rotate and scale it about its
// pivot, keeping the running totals.
void fn_80092F8C(UITransformDesc* p) {
    fn_80092CE8(&lbl_80281F38->aLevel[lbl_80281F38->nTop], p->vMove[0], p->vMove[1], p->vMove[2]);
    fn_80092CE8(&lbl_80281F38->aLevel[lbl_80281F38->nTop], p->vPivot[0], p->vPivot[1], p->vPivot[2]);
    lbl_80281F38->aLevel[lbl_80281F38->nTop].f74 += p->vMove[0];
    lbl_80281F38->aLevel[lbl_80281F38->nTop].f78 += p->vMove[1];
    lbl_80281F38->aLevel[lbl_80281F38->nTop].f64 += p->vPivot[0];
    lbl_80281F38->aLevel[lbl_80281F38->nTop].f68 += p->vPivot[1];
    fn_80092DE8(&lbl_80281F38->aLevel[lbl_80281F38->nTop], PI * p->vRotate[0] / 180.0f);
    fn_80092E74(&lbl_80281F38->aLevel[lbl_80281F38->nTop], PI * p->vRotate[1] / 180.0f);
    lbl_80281F38->aLevel[lbl_80281F38->nTop].f60 += PI * p->vRotate[2] / 180.0f;
    fn_80092F00(&lbl_80281F38->aLevel[lbl_80281F38->nTop], PI * p->vRotate[2] / 180.0f);
    lbl_80281F38->aLevel[lbl_80281F38->nTop].f6C *= p->vScale[0];
    lbl_80281F38->aLevel[lbl_80281F38->nTop].f70 *= p->vScale[1];
    fn_80092D68(&lbl_80281F38->aLevel[lbl_80281F38->nTop], p->vScale[0], p->vScale[1], p->vScale[2]);
    fn_80092CE8(&lbl_80281F38->aLevel[lbl_80281F38->nTop], -p->vPivot[0], -p->vPivot[1], -p->vPivot[2]);
    lbl_80281F38->aLevel[lbl_80281F38->nTop].w40 = p->w34;
    lbl_80281F38->aLevel[lbl_80281F38->nTop].f50[0] = p->f44[0] / 511.0f;
    lbl_80281F38->aLevel[lbl_80281F38->nTop].f50[1] = p->f44[1] / 511.0f;
    lbl_80281F38->aLevel[lbl_80281F38->nTop].f50[2] = p->f44[2] / 511.0f;
    lbl_80281F38->aLevel[lbl_80281F38->nTop].f5C += p->f44[3] / 511.0f;
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
        lbl_80281F38->aLevel[lbl_80281F38->nTop].f60 = 0.0f;
        lbl_80281F38->aLevel[lbl_80281F38->nTop].f5C = 0.0f;
        lbl_80281F38->aLevel[lbl_80281F38->nTop].f64 = 0.0f;
        lbl_80281F38->aLevel[lbl_80281F38->nTop].f68 = 0.0f;
        lbl_80281F38->aLevel[lbl_80281F38->nTop].f74 = 0.0f;
        lbl_80281F38->aLevel[lbl_80281F38->nTop].f78 = 0.0f;
        lbl_80281F38->aLevel[lbl_80281F38->nTop].f6C = 1.0f;
        lbl_80281F38->aLevel[lbl_80281F38->nTop].f70 = 1.0f;
        lbl_801D91F0[0] = DEG(50.0f);
        lbl_801D91F0[1] = fn_80014280(0.5f * lbl_801D91F0[0]);
        lbl_801D91F0[2] = 256.0f * (1.0f / lbl_801D91F0[1]);
        break;
    case 1:
        lbl_80281F38->aLevel[lbl_80281F38->nTop + 1] = lbl_80281F38->aLevel[lbl_80281F38->nTop];
        lbl_80281F38->nTop++;
        fn_80092F8C(p);
        lbl_801D91FC = lbl_80281F38->aLevel[lbl_80281F38->nTop];
        break;
    case 2:
        lbl_80281F38->nTop--;
        lbl_801D91FC = lbl_80281F38->aLevel[lbl_80281F38->nTop];
        break;
    case 3:
        p->n30 = 0;
        break;
    }
}

// Allocate the stack with its bottom level at identity.
void fn_8009349C(void) {
    lbl_80281F38 = fn_80009B34(sizeof(UITransformStack), 2, 16, "uiTransform.c", 203);
    lbl_80281F38->nTop = 0;
    fn_8000ADC0(lbl_80281F38->aLevel[lbl_80281F38->nTop].m);
}

void fn_800934F8(void) {
    fn_80009E70(lbl_80281F38);
    lbl_80281F38 = NULL;
}
