// CamUtils.c: the camera paths' maths for the camera scripts, static cameras and fly-bys: Catmull-Rom
// splines through four shots (position, look angles, field of view) and a segment's length, a
// fly-by's time step and timing curve, and points between two points or swung round a centre.

#include "game_types.h"
#include "engine.h"
#include "camera.h"
#include "unsorted/cull.h"

// The Catmull-Rom basis matrix: (1, t, t^2, t^3) times it gives the four points' weights.
static f32 gCatmullRomBasis[4][4] = {
    { 0.0f, 1.0f, 0.0f, 0.0f },
    { -0.5f, 0.0f, 0.5f, 0.0f },
    { 1.0f, -2.5f, 2.0f, -0.5f },
    { -0.5f, 1.5f, -1.5f, 0.5f },
};

static void LLMath_Subtract3(const f32* src1, const f32* src2, f32* dest);

// The splined camera: finalPos on the spline through pos0..3, finalLook (look angles) on the one
// through look0..3, each of look1..3 first unwrapped by whole turns to within half a turn of the
// one before, and finalFOV between fov0 and fov1, all at share param between the middle two.
void CamScript_SplineCamerasByPositionAndLook(f32* pos0, f32* pos1, f32* pos2, f32* pos3, f32* look0,
                                              f32* look1, f32* look2, f32* look3, f32* finalPos,
                                              f32* finalLook, f32* finalFOV, f32 fov0, f32 fov1,
                                              f32 param) {
    Vec4 paramVec;
    Vec4 resultVec;
    Vec4 paramMultCatmulVec;
    f32 pointMatrix[4][4];
    Vec4 adjustedLook1;
    Vec4 adjustedLook2;
    Vec4 adjustedLook3;
    f32 completeCircle;
    f32 halfCircle;

    completeCircle = 2.0f * PI;
    halfCircle = PI;
    paramVec.x = 1.0f;
    paramVec.y = param;
    paramVec.z = param * param;
    paramVec.w = param * (param * param);
    CamScript_SplineCamerasByPosition(pos0, pos1, pos2, pos3, finalPos, param);

    Vec3Copy(look1, &adjustedLook1.x);
    while (adjustedLook1.x - look0[0] > halfCircle) {
        adjustedLook1.x -= completeCircle;
    }
    while (look0[0] - adjustedLook1.x > halfCircle) {
        adjustedLook1.x += completeCircle;
    }
    while (adjustedLook1.y - look0[1] > halfCircle) {
        adjustedLook1.y -= completeCircle;
    }
    while (look0[1] - adjustedLook1.y > halfCircle) {
        adjustedLook1.y += completeCircle;
    }
    while (adjustedLook1.z - look0[2] > halfCircle) {
        adjustedLook1.z -= completeCircle;
    }
    while (look0[2] - adjustedLook1.z > halfCircle) {
        adjustedLook1.z += completeCircle;
    }

    Vec3Copy(look2, &adjustedLook2.x);
    while (adjustedLook2.x - adjustedLook1.x > halfCircle) {
        adjustedLook2.x -= completeCircle;
    }
    while (adjustedLook1.x - adjustedLook2.x > halfCircle) {
        adjustedLook2.x += completeCircle;
    }
    while (adjustedLook2.y - adjustedLook1.y > halfCircle) {
        adjustedLook2.y -= completeCircle;
    }
    while (adjustedLook1.y - adjustedLook2.y > halfCircle) {
        adjustedLook2.y += completeCircle;
    }
    while (adjustedLook2.z - adjustedLook1.z > halfCircle) {
        adjustedLook2.z -= completeCircle;
    }
    while (adjustedLook1.z - adjustedLook2.z > halfCircle) {
        adjustedLook2.z += completeCircle;
    }

    Vec3Copy(look3, &adjustedLook3.x);
    while (adjustedLook3.x - adjustedLook2.x > halfCircle) {
        adjustedLook3.x -= completeCircle;
    }
    while (adjustedLook2.x - adjustedLook3.x > halfCircle) {
        adjustedLook3.x += completeCircle;
    }
    while (adjustedLook3.y - adjustedLook2.y > halfCircle) {
        adjustedLook3.y -= completeCircle;
    }
    while (adjustedLook2.y - adjustedLook3.y > halfCircle) {
        adjustedLook3.y += completeCircle;
    }
    while (adjustedLook3.z - adjustedLook2.z > halfCircle) {
        adjustedLook3.z -= completeCircle;
    }
    while (adjustedLook2.z - adjustedLook3.z > halfCircle) {
        adjustedLook3.z += completeCircle;
    }

    Vec3Copy(look0, pointMatrix[0]);
    Vec3Copy(&adjustedLook1.x, pointMatrix[1]);
    Vec3Copy(&adjustedLook2.x, pointMatrix[2]);
    Vec3Copy(&adjustedLook3.x, pointMatrix[3]);
    pointMatrix[0][3] = 0.0f;
    pointMatrix[1][3] = 0.0f;
    pointMatrix[2][3] = 0.0f;
    pointMatrix[3][3] = 0.0f;
    LLMath_mat44fltMultiply(gCatmullRomBasis, &paramVec, &paramMultCatmulVec);
    LLMath_mat44fltMultiply(pointMatrix, &paramMultCatmulVec, &resultVec);
    Vec3Copy(&resultVec.x, finalLook);
    *finalFOV = param * (fov1 - fov0) + fov0;
}

// The point at share param between pos1 and pos2 on the Catmull-Rom spline through pos0..pos3.
void CamScript_SplineCamerasByPosition(f32* pos0, f32* pos1, f32* pos2, f32* pos3, f32* finalPos,
                                       f32 param) {
    Vec4 paramVec;
    Vec4 resultVec;
    Vec4 paramMultCatmulVec;
    f32 pointMatrix[4][4];

    paramVec.y = param;
    paramVec.x = 1.0f;
    paramVec.z = param * param;
    paramVec.w = param * (param * param);
    Vec3Copy(pos0, pointMatrix[0]);
    Vec3Copy(pos1, pointMatrix[1]);
    Vec3Copy(pos2, pointMatrix[2]);
    Vec3Copy(pos3, pointMatrix[3]);
    pointMatrix[0][3] = 0.0f;
    pointMatrix[1][3] = 0.0f;
    pointMatrix[2][3] = 0.0f;
    pointMatrix[3][3] = 0.0f;
    LLMath_mat44fltMultiply(gCatmullRomBasis, &paramVec, &paramMultCatmulVec);
    LLMath_mat44fltMultiply(pointMatrix, &paramMultCatmulVec, &resultVec);
    Vec3Copy(&resultVec.x, finalPos);
}

// A fly-by's time step while it speeds up: over the first accelParam share of time1 the rate goes
// in a straight line from rate0 to rate1, and the step is timeStep scaled by the rate over rate1.
// From there on, or with accelParam 0, the step is timeStep; a 0 step stays 0.
f32 CamScript_GetFlybyTimeStep(f32 rate0, f32 rate1, f32 time1, f32 accelParam, f32 currentTime,
                               f32 timeStep) {
    f32 rateDiff;
    f32 accelPercent;
    f32 adjustedRate;
    f32 adjustedRatePercent;
    f32 finalTimeStep;

    if (0.0f == timeStep) {
        return 0.0f;
    }
    if (0.0f == accelParam || currentTime >= accelParam * time1) {
        return timeStep;
    }
    rateDiff = rate1 - rate0;
    accelPercent = currentTime / (accelParam * time1);
    adjustedRate = accelPercent * rateDiff + rate0;
    adjustedRatePercent = adjustedRate / rate1;
    finalTimeStep = timeStep * adjustedRatePercent;
    return finalTimeStep;
}

// The length of the spline between pos1 and pos2, added up over 32 straight steps.
f32 CamScript_fGetDistanceBetweenSplinePoints(f32* pos0, f32* pos1, f32* pos2, f32* pos3) {
    f32 sampleParam;
    int i;
    f32 distance;
    f32 curParam;
    Vec4 prevLocation;
    Vec4 newLocation;
    Vec4 newToPrev;
    f32 newToPrevDist;

    sampleParam = 1.0f / 32.0f;
    distance = 0.0f;
    curParam = 0.0f;
    Vec3Copy(pos1, &prevLocation.x);
    for (i = 0; i < 32; i++) {
        curParam += sampleParam;
        CamScript_SplineCamerasByPosition(pos0, pos1, pos2, pos3, &newLocation.x, curParam);
        LLMath_Subtract3(&newLocation.x, &prevLocation.x, &newToPrev.x);
        newToPrevDist = Math_Sqrt(LLMath_LengthSquared3(&newToPrev.x));
        distance += newToPrevDist;
        Vec3Copy(&newLocation.x, &prevLocation.x);
    }
    return distance;
}

static f32 CamUtils_OrderConstants(f32 x) {   // fake match: never called, pools -1.0f before 128.0f
    return x + -1.0f;
}

// A fly-by path's timing curve c at share time of the fly-by's time (-1 for no curve or a time
// outside 0..1). The key whose start.x..end.x holds time is walked in 128 steps of its Hermite
// segment, and the value is read off the straight step around time.
#pragma opt_loop_invariants off   // fake match: keeps the step count's load where the divide uses it
f32 CamScript_fEvaluateCurve(Curve* c, f32 time) {
    static const f32 steps[1] = {128.0f};   // fake match: a divide by 128.0f, not a multiply by 1/128
    f32 h1;
    f32 h2;
    f32 h3;
    f32 h4;
    f32 s;
    CurvePoint2D p;
    CurvePoint2D lastP;
    f32 y;
    f32 w1;
    f32 w2;
    u32 num1;
    u32 t;
    u32 nKeys;

    if (c == NULL) {
        return -1.0f;
    }
    if (time < 0.0f || time > 1.0f) {
        return -1.0f;
    }
    if (0.0f == time) {
        return c->aKeys[0].start.y;
    }
    if (1.0f == time) {
        return c->aKeys[c->nKeys - 1].end.y;
    }
    nKeys = c->nKeys;
    for (num1 = 0; num1 < nKeys; num1++) {
        if (time > c->aKeys[num1].start.x && time <= c->aKeys[num1].end.x) {
            break;
        }
    }
    lastP.x = c->aKeys[num1].start.x;
    lastP.y = c->aKeys[num1].start.y;
    for (t = 0; t < *(const f32*)steps; t++) {   // fake match: the cast keeps two loads of the count
        s = t / steps[0];
        h1 = 1.0f + (s * (2.0f * s * s) - 3.0f * s * s);
        h2 = -(s * (2.0f * s * s)) + 3.0f * s * s;
        h3 = s + (s * (s * s) - 2.0f * s * s);
        h4 = s * (s * s) - s * s;
        p.x = h4 * c->aKeys[num1].endTangent.x +
            (h3 * c->aKeys[num1].startTangent.x + (h1 * c->aKeys[num1].start.x + h2 * c->aKeys[num1].end.x));
        p.y = h4 * c->aKeys[num1].endTangent.y +
            (h3 * c->aKeys[num1].startTangent.y + (h1 * c->aKeys[num1].start.y + h2 * c->aKeys[num1].end.y));
        if (p.x > time) {
            break;
        }
        lastP.x = p.x;
        lastP.y = p.y;
    }
    if (t == steps[0]) {
        p.x = c->aKeys[num1].end.x;
        p.y = c->aKeys[num1].end.y;
    }
    w1 = 1.0f - (time - lastP.x) / (p.x - lastP.x);
    w2 = 1.0f - (p.x - time) / (p.x - lastP.x);
    y = lastP.y * w1 + p.y * w2;
    return y;
}
#pragma opt_loop_invariants reset

// endPt: param along the way from pt1 to pt2 (flattened unless use3D; normalised unless
// usePercent, which makes param a share of the way), then sideOffset across the flattened way.
void CamUtils_vGetPositionBetweenTwoPoints(f32* pt1, f32* pt2, u8 use3D, u8 usePercent, f32* endPt,
                                           f32 param, f32 sideOffset) {
    Vec4 pointDiff;

    LLMath_Subtract3(pt2, pt1, &pointDiff.x);
    if (!use3D) {
        pointDiff.y = 0.0f;
    }
    if (!usePercent && (0.0f != pointDiff.x || 0.0f != pointDiff.y || 0.0f != pointDiff.z)) {
        LLMath_Normalize3(&pointDiff.x, &pointDiff.x);
    }
    LLMath_AddScale3(pt1, &pointDiff.x, param, endPt);
    pointDiff.y = 0.0f;
    if (0.0f != pointDiff.x || 0.0f != pointDiff.y || 0.0f != pointDiff.z) {
        LLMath_Normalize3(&pointDiff.x, &pointDiff.x);
    }
    endPt[0] = sideOffset * -pointDiff.z + endPt[0];
    endPt[2] = sideOffset * pointDiff.x + endPt[2];
}

// endPoint: swung round centerPoint from pt1 towards pt2, at share param. The flat angle round the
// centre (from x towards z) and the flat distance from it are blended, the height goes from pt1's
// to pt2's; arcMode gives the way round.
void CamUtils_vCalcArcPosition(f32* pt1, f32* pt2, f32* centerPoint, CameraArcMode_e arcMode,
                               f32* endPoint, f32 param) {
    Vec4 dest1;
    Vec4 dest2;
    f32 angle1;
    f32 angle2;
    f32 r2;
    f32 r1;
    f32 newAngle;
    f32 newR;

    LLMath_Subtract3(pt1, centerPoint, &dest1.x);
    LLMath_Subtract3(pt2, centerPoint, &dest2.x);
    dest1.y = 0.0f;
    dest2.y = 0.0f;
    if (0.0f == dest1.x) {
        if (dest1.z > 0.0f) {
            angle1 = PI / 2.0f;
        } else {
            angle1 = 3.0f * PI / 2.0f;
        }
    } else {
        angle1 = atan2f(dest1.z, dest1.x);
    }
    if (0.0f == dest2.x) {
        if (dest2.z > 0.0f) {
            angle2 = PI / 2.0f;
        } else {
            angle2 = 3.0f * PI / 2.0f;
        }
    } else {
        angle2 = atan2f(dest2.z, dest2.x);
    }
    if (angle1 < -PI / 2.0f && angle2 > PI / 2.0f) {
        angle1 += 2.0f * PI;
    } else if (angle1 > PI / 2.0f && angle2 < -PI / 2.0f) {
        angle2 += 2.0f * PI;
    }
    if (arcMode == CAMERA_ARC_EITHER_WAY) {
        if (angle2 - angle1 > PI) {
            angle1 += 2.0f * PI;
        }
    } else if (arcMode == CAMERA_ARC_ANGLE_DOWN) {
        while (angle2 > angle1) {
            angle2 -= 2.0f * PI;
        }
    } else {
        while (angle2 < angle1) {
            angle2 += 2.0f * PI;
        }
    }
    r1 = Math_Sqrt(LLMath_LengthSquared3(&dest1.x));
    r2 = Math_Sqrt(LLMath_LengthSquared3(&dest2.x));
    newR = param * (r2 - r1) + r1;
    newAngle = param * (angle2 - angle1) + angle1;
    endPoint[0] = newR * cosf(newAngle) + centerPoint[0];
    endPoint[1] = param * (pt2[1] - pt1[1]) + pt1[1];
    endPoint[2] = newR * sinf(newAngle) + centerPoint[2];
}

// Subtracts src2 from src1 into dest: x, y and z (dest's w is left as it was).
static asm void LLMath_Subtract3(register const f32* src1, register const f32* src2, register f32* dest) {
    nofralloc
    psq_l  f0, 0(src1), 0, 0
    psq_l  f1, 8(src1), 1, 0
    psq_l  f2, 0(src2), 0, 0
    psq_l  f3, 8(src2), 1, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(dest), 0, 0
    psq_st f3, 8(dest), 1, 0
    blr
}
