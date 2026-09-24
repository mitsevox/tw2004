// CamSpline.c (our name): the spline paths the scripted, static and dynamic cameras move along
// (a Catmull-Rom basis matrix in lbl_80191440 applied with VecMath.c's fn_800BAD60).

#include "game_types.h"
#include "engine.h"
#include "camera.h"
#include "unsorted/cull.h"

void fn_800C7898(f32* p0, f32* p1, f32* p2, f32* p3, f32* pOut, f32 fT);
f32  fn_800C7970(f32 fA, f32 fB, f32 fC, f32 fD, f32 fE, f32 fF);
f32  fn_800C79BC(f32* p0, f32* p1, f32* p2, f32* p3);
void fn_800C8068(f32* pA, f32* pB, f32* pOut);

// The point at share fT between p1 and p2 on the Catmull-Rom spline through p0..p3.
void fn_800C7898(f32* p0, f32* p1, f32* p2, f32* p3, f32* pOut, f32 fT) {
    Vec4 vT;
    Vec4 vOut;
    Vec4 vWeights;
    f32 aPoints[4][4];
    f32 fT2;

    fT2 = fT * fT;
    vT.y = fT;
    vT.x = 1.0f;
    vT.z = fT2;
    vT.w = fT * fT2;
    Vec3Copy(p0, aPoints[0]);
    Vec3Copy(p1, aPoints[1]);
    Vec3Copy(p2, aPoints[2]);
    Vec3Copy(p3, aPoints[3]);
    aPoints[0][3] = 0.0f;
    aPoints[1][3] = 0.0f;
    aPoints[2][3] = 0.0f;
    aPoints[3][3] = 0.0f;
    fn_800BAD60(lbl_80191440, &vT, &vWeights);
    fn_800BAD60(aPoints, &vWeights, &vOut);
    Vec3Copy(&vOut.x, pOut);
}

// fF scaled by where fE falls in fC * fD, mapped from fA..fB and divided by fB; fF itself when fD
// is 0 or fE is past the end.
f32 fn_800C7970(f32 fA, f32 fB, f32 fC, f32 fD, f32 fE, f32 fF) {
    f32 fEnd;

    if (0.0f == fF) {
        return 0.0f;
    }
    if (0.0f == fD || fE >= (fEnd = fD * fC)) {
        return fF;
    }
    return fF * (((fE / fEnd) * (fB - fA) + fA) / fB);
}

// The length of the spline between p1 and p2, added up over 32 straight steps.
f32 fn_800C79BC(f32* p0, f32* p1, f32* p2, f32* p3) {
    Vec4 vLast;
    Vec4 vPoint;
    Vec4 vDelta;
    f32 fLength;
    f32 fT;
    int i;

    fLength = 0.0f;
    fT = fLength;
    Vec3Copy(p1, &vLast.x);
    for (i = 0; i < 32; i++) {
        fT += 1.0f / 32.0f;
        fn_800C7898(p0, p1, p2, p3, &vPoint.x, fT);
        fn_800C8068(&vPoint.x, &vLast.x, &vDelta.x);
        fLength += (f32)fn_80009680(fn_80009744(&vDelta.x));
        Vec3Copy(&vPoint.x, &vLast.x);
    }
    return fLength;
}

// Three floats: pOut gets pA minus pB.
#ifdef __MWERKS__
asm void fn_800C8068(register f32* pA, register f32* pB, register f32* pOut) {
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
void fn_800C8068(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif
