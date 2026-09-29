// CamSpline.c (our name; TW07 has these functions, under the same names, in EA's
// Shared/Cameras/CamUtils.c): the path helpers of the camera scripts, static cameras and fly-bys.
// Catmull-Rom splines through four points (gCatmullRomBasis applied with VecMath.c's
// LLMath_mat44fltMultiply) for the camera's position, look angles and field of view, a spline
// segment's length, the time step of an accelerating fly-by, a fly-by's Hermite curve, a point
// between two points and a point swung on an arc around a centre.

#include "game_types.h"
#include "engine.h"
#include "camera.h"
#include "unsorted/cull.h"

f32  CamScript_fGetDistanceBetweenSplinePoints(f32* p0, f32* p1, f32* p2, f32* p3);
void CamUtils_Vec3Sub(f32* pA, f32* pB, f32* pOut);

// The Catmull-Rom basis matrix: times (1, t, t^2, t^3) it gives the four points' weights.
f32 gCatmullRomBasis[4][4] = {
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { -0.5f, 0.0f, 0.5f, 0.0f },
    { 1.0f, -2.5f, 2.0f, -0.5f },
    { -0.5f, 1.5f, -1.5f, 0.5f },
};

// The splined camera: the camera position on the spline through pPos0..3, the look angles on the
// one through pLook0..3 (each angle first unwrapped to within half a turn of the one before), and
// the field of view between fFov1 and fFov2, at share fT between the middle two.
void CamScript_SplineCamerasByPositionAndLook(f32* pPos0, f32* pPos1, f32* pPos2, f32* pPos3, f32* pLook0,
                                              f32* pLook1, f32* pLook2,
                 f32* pLook3, f32* pCam, f32* pSub, f32* pFov, f32 fFov1, f32 fFov2, f32 fT) {
    f32 aPoints[4][4];
    Vec4 vT;
    Vec4 vOut;
    Vec4 vWeights;
    Vec4 v1;
    Vec4 v2;
    Vec4 v3;
    f32 fT2;

    fT2 = fT * fT;
    vT.x = 1.0f;
    vT.y = fT;
    vT.z = fT2;
    vT.w = fT * fT2;
    CamScript_SplineCamerasByPosition(pPos0, pPos1, pPos2, pPos3, pCam, fT);

    Vec3Copy(pLook1, &v1.x);
    while (v1.x - pLook0[0] > PI) {
        v1.x -= 2.0f * PI;
    }
    while (pLook0[0] - v1.x > PI) {
        v1.x += 2.0f * PI;
    }
    while (v1.y - pLook0[1] > PI) {
        v1.y -= 2.0f * PI;
    }
    while (pLook0[1] - v1.y > PI) {
        v1.y += 2.0f * PI;
    }
    while (v1.z - pLook0[2] > PI) {
        v1.z -= 2.0f * PI;
    }
    while (pLook0[2] - v1.z > PI) {
        v1.z += 2.0f * PI;
    }

    Vec3Copy(pLook2, &v2.x);
    while (v2.x - v1.x > PI) {
        v2.x -= 2.0f * PI;
    }
    while (v1.x - v2.x > PI) {
        v2.x += 2.0f * PI;
    }
    while (v2.y - v1.y > PI) {
        v2.y -= 2.0f * PI;
    }
    while (v1.y - v2.y > PI) {
        v2.y += 2.0f * PI;
    }
    while (v2.z - v1.z > PI) {
        v2.z -= 2.0f * PI;
    }
    while (v1.z - v2.z > PI) {
        v2.z += 2.0f * PI;
    }

    Vec3Copy(pLook3, &v3.x);
    while (v3.x - v2.x > PI) {
        v3.x -= 2.0f * PI;
    }
    while (v2.x - v3.x > PI) {
        v3.x += 2.0f * PI;
    }
    while (v3.y - v2.y > PI) {
        v3.y -= 2.0f * PI;
    }
    while (v2.y - v3.y > PI) {
        v3.y += 2.0f * PI;
    }
    while (v3.z - v2.z > PI) {
        v3.z -= 2.0f * PI;
    }
    while (v2.z - v3.z > PI) {
        v3.z += 2.0f * PI;
    }

    Vec3Copy(pLook0, aPoints[0]);
    Vec3Copy(&v1.x, aPoints[1]);
    Vec3Copy(&v2.x, aPoints[2]);
    Vec3Copy(&v3.x, aPoints[3]);
    aPoints[0][3] = 0.0f;
    aPoints[1][3] = 0.0f;
    aPoints[2][3] = 0.0f;
    aPoints[3][3] = 0.0f;
    LLMath_mat44fltMultiply(gCatmullRomBasis, &vT, &vWeights);
    LLMath_mat44fltMultiply(aPoints, &vWeights, &vOut);
    Vec3Copy(&vOut.x, pSub);
    *pFov = fT * (fFov2 - fFov1) + fFov1;
}

// The point at share fT between p1 and p2 on the Catmull-Rom spline through p0..p3.
void CamScript_SplineCamerasByPosition(f32* p0, f32* p1, f32* p2, f32* p3, f32* pOut, f32 fT) {
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
    LLMath_mat44fltMultiply(gCatmullRomBasis, &vT, &vWeights);
    LLMath_mat44fltMultiply(aPoints, &vWeights, &vOut);
    Vec3Copy(&vOut.x, pOut);
}

// The fly-by time step fStep while the fly-by speeds up: the rate goes linearly from fRate0 to
// fRate1 over the first fAccel share of fTime1, and the step is fStep times rate / fRate1. From
// fCurTime past that, or with fAccel 0, the step is fStep; a 0 step stays 0 (parameters after
// TW07's rate0, rate1, time1, accelParam, currentTime, timeStep). CamScript_AccelerateTime calls it.
f32 CamScript_GetFlybyTimeStep(f32 fRate0, f32 fRate1, f32 fTime1, f32 fAccel, f32 fCurTime, f32 fStep) {
    f32 fEnd;

    if (0.0f == fStep) {
        return 0.0f;
    }
    if (0.0f == fAccel || fCurTime >= (fEnd = fAccel * fTime1)) {
        return fStep;
    }
    return fStep * (((fCurTime / fEnd) * (fRate1 - fRate0) + fRate0) / fRate1);
}

// The length of the spline between p1 and p2, added up over 32 straight steps.
f32 CamScript_fGetDistanceBetweenSplinePoints(f32* p0, f32* p1, f32* p2, f32* p3) {
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
        CamScript_SplineCamerasByPosition(p0, p1, p2, p3, &vPoint.x, fT);
        CamUtils_Vec3Sub(&vPoint.x, &vLast.x, &vDelta.x);
        fLength += (f32)Math_Sqrt(Vec3_LengthSqClamped(&vDelta.x));
        Vec3Copy(&vPoint.x, &vLast.x);
    }
    return fLength;
}

// fake match: puts -1.0f in the constant pool before CamScript_fEvaluateCurve's step-count array, which the
// compiler emits ahead of that function's own literals (EA's 128.0f was a pooled literal, after
// the -1.0f). Unused, so the linker strips it.
static f32 CamSpline_StrippedFn(f32 x) {
    return x + -1.0f;
}

// The value of a fly-by path's curve at time fT (0..1; -1 when out of range): each key holds a
// Hermite segment from (af[0], af[1]) to (af[2], af[3]) with tangents af[4..5] and af[6..7]. The
// segment holding fT is walked in 128 steps and the value read off the straight step around fT.
// fake match: loop-invariant motion off for this function only (not EA's build setting). With it
// on, the frontend hoists the step count's load into its own temporary, numbered below the
// conversion constant's, and the two trade f27/f28; with it off the backend hoists the load where
// the divide first uses it, as in EA's code. The key count is read into nKeys so that the key
// search still counts down in ctr.
#pragma opt_loop_invariants off
f32 CamScript_fEvaluateCurve(FlyByPath* pPath, f32 fT) {
    // fake match: the step count as a one-element const array, not the literal 128.0f: the literal
    // divide becomes a multiply by 1/128 (CW does that for powers of two from 2 to 1024), while EA's
    // code divides by the .sdata2 constant.
    static const f32 aStep[1] = {128.0f};
    f32 fH00;
    f32 fH01;
    f32 fH10;
    f32 fH11;
    f32 fX;
    f32 fY;
    f32 fT1;
    f32 fLastX;
    f32 fLastY;
    f32 fStep;
    u32 i;
    u32 nStep;
    u32 nKeys;

    if (pPath == NULL) {
        return -1.0f;
    }
    if (fT < 0.0f || fT > 1.0f) {
        return -1.0f;
    }
    if (0.0f == fT) {
        return pPath->aKeys[0].af[1];
    }
    if (1.0f == fT) {
        return pPath->aKeys[pPath->nKeys - 1].af[3];
    }
    nKeys = pPath->nKeys;
    for (i = 0; i < nKeys; i++) {
        if (fT > pPath->aKeys[i].af[0] && fT <= pPath->aKeys[i].af[2]) {
            break;
        }
    }
    fLastX = pPath->aKeys[i].af[0];
    fLastY = pPath->aKeys[i].af[1];
    // fake match: the loop test reads the step count through a (no-op) pointer cast, so the
    // frontend does not share one temporary between the test and the divide.
    for (nStep = 0; nStep < *(const f32*)aStep; nStep++) {
        fT1 = nStep / aStep[0];
        fH00 = 1.0f + (fT1 * (2.0f * fT1 * fT1) - 3.0f * fT1 * fT1);
        fH01 = -(fT1 * (2.0f * fT1 * fT1)) + 3.0f * fT1 * fT1;
        fH10 = fT1 + (fT1 * (fT1 * fT1) - 2.0f * fT1 * fT1);
        fH11 = fT1 * (fT1 * fT1) - fT1 * fT1;
        fX = fH11 * pPath->aKeys[i].af[6] +
             (fH10 * pPath->aKeys[i].af[4] + (fH00 * pPath->aKeys[i].af[0] + fH01 * pPath->aKeys[i].af[2]));
        fY = fH11 * pPath->aKeys[i].af[7] +
             (fH10 * pPath->aKeys[i].af[5] + (fH00 * pPath->aKeys[i].af[1] + fH01 * pPath->aKeys[i].af[3]));
        if (fX > fT) {
            break;
        }
        fLastX = fX;
        fLastY = fY;
    }
    if (nStep == aStep[0]) {
        fX = pPath->aKeys[i].af[2];
        fY = pPath->aKeys[i].af[3];
    }
    fStep = fX - fLastX;
    return fLastY * (1.0f - (fT - fLastX) / fStep) + fY * (1.0f - (fX - fT) / fStep);
}
#pragma opt_loop_invariants reset

// A point fDist along the direction from pA to pB (flattened unless bKeepY; normalised unless bRaw,
// so with bRaw fDist is a share of the way), then moved fSide sideways (across the flat direction).
void CamUtils_vGetPositionBetweenTwoPoints(f32* pA, f32* pB, u8 bKeepY, u8 bRaw, f32* pOut, f32 fDist,
                                           f32 fSide) {
    f32 aDir[3];

    CamUtils_Vec3Sub(pB, pA, aDir);
    if (!bKeepY) {
        aDir[1] = 0.0f;
    }
    if (!bRaw && (0.0f != aDir[0] || 0.0f != aDir[1] || 0.0f != aDir[2])) {
        LLMath_Normalize3(aDir, aDir);
    }
    fn_8000C5D4(pA, aDir, fDist, pOut);
    aDir[1] = 0.0f;
    if (0.0f != aDir[0] || 0.0f != aDir[1] || 0.0f != aDir[2]) {
        LLMath_Normalize3(aDir, aDir);
    }
    pOut[0] = fSide * -aDir[2] + pOut[0];
    pOut[2] = fSide * aDir[0] + pOut[2];
}

// A point swung around pC from pA towards pB, at share fT: the flat angle and distance from pC and
// the height are each blended. nDir picks the way round: 0 the short way, 1 decreasing, else
// increasing.
void CamUtils_vCalcArcPosition(f32* pA, f32* pB, f32* pC, int nDir, f32* pOut, f32 fT) {
    Vec4 vFrom;
    Vec4 vTo;
    f32 fAngleFrom;
    f32 fAngleTo;
    f32 fDistTo;
    f32 fDistFrom;
    f32 fAngle;
    f32 fDist;

    CamUtils_Vec3Sub(pA, pC, &vFrom.x);
    CamUtils_Vec3Sub(pB, pC, &vTo.x);
    vFrom.y = 0.0f;
    vTo.y = 0.0f;
    if (0.0f == vFrom.x) {
        if (vFrom.z > 0.0f) {
            fAngleFrom = PI / 2.0f;
        } else {
            fAngleFrom = 3.0f * PI / 2.0f;
        }
    } else {
        fAngleFrom = atan2f(vFrom.z, vFrom.x);
    }
    if (0.0f == vTo.x) {
        if (vTo.z > 0.0f) {
            fAngleTo = PI / 2.0f;
        } else {
            fAngleTo = 3.0f * PI / 2.0f;
        }
    } else {
        fAngleTo = atan2f(vTo.z, vTo.x);
    }
    if (fAngleFrom < -PI / 2.0f && fAngleTo > PI / 2.0f) {
        fAngleFrom += 2.0f * PI;
    } else if (fAngleFrom > PI / 2.0f && fAngleTo < -PI / 2.0f) {
        fAngleTo += 2.0f * PI;
    }
    if (nDir == 0) {
        if (fAngleTo - fAngleFrom > PI) {
            fAngleFrom += 2.0f * PI;
        }
    } else if (nDir == 1) {
        while (fAngleTo > fAngleFrom) {
            fAngleTo -= 2.0f * PI;
        }
    } else {
        while (fAngleTo < fAngleFrom) {
            fAngleTo += 2.0f * PI;
        }
    }
    fDistFrom = Math_Sqrt(Vec3_LengthSqClamped(&vFrom.x));
    fDistTo = Math_Sqrt(Vec3_LengthSqClamped(&vTo.x));
    fDist = fT * (fDistTo - fDistFrom) + fDistFrom;
    fAngle = fT * (fAngleTo - fAngleFrom) + fAngleFrom;
    pOut[0] = fDist * Math_Cos(fAngle) + pC[0];
    pOut[1] = fT * (pB[1] - pA[1]) + pA[1];
    pOut[2] = fDist * Math_Sin(fAngle) + pC[2];
}

// Three floats: pOut gets pA minus pB.
#ifdef __MWERKS__
asm void CamUtils_Vec3Sub(register f32* pA, register f32* pB, register f32* pOut) {
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
void CamUtils_Vec3Sub(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif
