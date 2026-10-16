// MathQuat.c: rotations as quaternions (four floats x, y, z, w) for the golfer's skeleton and
// animation, the cameras and the course's objects; a quaternion turns a vector as Quat_RotateVector
// does, as the row vector times Quat_QuatToMatrix's matrix. Then the float sin, cos, acos, asin, sqrt.

#include "engine.h"

static void Quat_Clear(f32* Q);

// Spherical interpolation from the unit quaternion Q1 to Q2 by fFactor_p (0 gives Q1, 1 gives Q2 or
// -Q2, the same turn), written over Q2. Q2 is negated first when Q1 . Q2 < 0, so the blend takes
// the shorter way round. When |Q1 . Q2| is within tlC_SLERP_DELTA (0.01) of 1, the two almost the
// same, the blend is a straight one.
void Quat_Slerp(f32* Q1, f32* Q2, f32 fFactor_p) {
    const f32 tlC_SLERP_DELTA = 0.01f;
    f32 fX;
    f32 fY;
    f32 fZ;
    f32 fW;
    f32 tlOmega;
    f32 tlCosOmega;
    f32 tlSinOmega;
    f32 tlScale0;
    f32 tlScale1;

    // fake match: Q2 is copied inside the dot product, for the original's load order
    tlCosOmega = Q1[3] * (fW = Q2[3]) +
                 (Q1[2] * (fZ = Q2[2]) + (Q1[0] * (fX = Q2[0]) + Q1[1] * (fY = Q2[1])));
    if (tlCosOmega < 0.0f) {
        tlCosOmega = -tlCosOmega;
        fX = -fX;
        fY = -fY;
        fZ = -fZ;
        fW = -fW;
    }
    if (1.0f - tlCosOmega > tlC_SLERP_DELTA) {
        tlOmega = acosf(tlCosOmega);
        tlSinOmega = sinf(tlOmega);
        tlScale0 = sinf((1.0f - fFactor_p) * tlOmega) / tlSinOmega;
        tlScale1 = sinf(fFactor_p * tlOmega) / tlSinOmega;
    } else {
        tlScale0 = 1.0f - fFactor_p;
        tlScale1 = fFactor_p;
    }
    Q2[0] = tlScale0 * Q1[0] + tlScale1 * fX;
    Q2[1] = tlScale0 * Q1[1] + tlScale1 * fY;
    Q2[2] = tlScale0 * Q1[2] + tlScale1 * fZ;
    Q2[3] = tlScale0 * Q1[3] + tlScale1 * fW;
}

// The unit quaternion of the rotation in M's 3x3 part, into Q: the reverse of Quat_QuatToMatrix (it
// gives back q or -q, the same turn). From the trace when it is positive, otherwise from the largest
// diagonal element.
void Quat_BuildFromMatrix(f32 (*M)[4], f32* Q) {
    f32 tlMatrixDiagSum;
    f32 tlScale;
    int iIndex0;
    int iIndex1;
    int iIndex2;
    const int iNextIndex[3] = {1, 2, 0};
    Quat tlTempQuat;

    tlMatrixDiagSum = M[2][2] + (M[0][0] + M[1][1]);
    if (tlMatrixDiagSum > 0.0f) {
        tlScale = Math_Sqrt(tlMatrixDiagSum + 1.0f);
        Q[3] = 0.5f * tlScale;
        tlScale = 0.5f / tlScale;
        Q[0] = tlScale * -(M[1][2] - M[2][1]);
        Q[1] = tlScale * -(M[2][0] - M[0][2]);
        Q[2] = tlScale * -(M[0][1] - M[1][0]);
        return;
    }
    iIndex0 = 0;
    if (M[1][1] > M[0][0]) {
        iIndex0 = 1;
    }
    if (M[2][2] > M[iIndex0][iIndex0]) {
        iIndex0 = 2;
    }
    iIndex1 = iNextIndex[iIndex0];
    iIndex2 = iNextIndex[iIndex1];
    tlScale = Math_Sqrt(M[iIndex0][iIndex0] - (M[iIndex1][iIndex1] + M[iIndex2][iIndex2]) + 1.0f);
    tlTempQuat[iIndex0] = 0.5f * tlScale;
    if (tlScale != 0.0f) {
        tlScale = 0.5f / tlScale;
    }
    tlTempQuat[3] = tlScale * (M[iIndex1][iIndex2] - M[iIndex2][iIndex1]);
    tlTempQuat[iIndex1] = tlScale * (M[iIndex0][iIndex1] + M[iIndex1][iIndex0]);
    tlTempQuat[iIndex2] = tlScale * (M[iIndex0][iIndex2] + M[iIndex2][iIndex0]);
    Q[3] = tlTempQuat[3];
    Q[0] = -tlTempQuat[0];
    Q[1] = -tlTempQuat[1];
    Q[2] = -tlTempQuat[2];
}

// fake match: never called, orders Math_Sqrt's double constants after Quat_BuildFromMatrix's
static double Quat_OrderSqrtConstants(double x) {
    double guess = 0.0;

    if (x > guess) {
        guess = 0.5 * x * (3.0 - x);
    }
    return guess;
}

// The turn by fYaw_p about z, then fPitch_p about y, then fRoll_p about x (radians, the axes fixed),
// into Q: the product Legacy_Quat_BuildFromYaw x Legacy_Quat_BuildFromPitch x
// Legacy_Quat_BuildFromRoll (Quat_Multiply). An angle of exactly 0 skips its sin and cos; all three
// 0 give the identity.
void Quat_EulerAngles(f32 fYaw_p, f32 fPitch_p, f32 fRoll_p, f32* Q) {
    f32 tlSinYaw;
    f32 tlCosYaw;
    f32 tlSinPitch;
    f32 tlCosPitch;
    f32 tlSinRoll;
    f32 tlCosRoll;
    f32 tlCosPitchTimeCosYaw;
    f32 tlSinPitchTimeSinYaw;
    f32 tlSinPitchTimeCosYaw;
    f32 tlCosPitchTimeSinYaw;

    fYaw_p = -fYaw_p;
    fPitch_p = -fPitch_p;
    fRoll_p = -fRoll_p;
    if (fYaw_p != 0.0f) {
        if (fPitch_p != 0.0f) {
            if (fRoll_p != 0.0f) {
                tlSinYaw = sinf(0.5f * fYaw_p);
                tlCosYaw = cosf(0.5f * fYaw_p);
                tlSinPitch = sinf(0.5f * fPitch_p);
                tlCosPitch = cosf(0.5f * fPitch_p);
                tlSinRoll = sinf(0.5f * fRoll_p);
                tlCosRoll = cosf(0.5f * fRoll_p);
                tlSinPitchTimeSinYaw = tlSinPitch * tlSinYaw;
                tlCosPitchTimeSinYaw = tlCosPitch * tlSinYaw;
                tlSinPitchTimeCosYaw = tlSinPitch * tlCosYaw;
                tlCosPitchTimeCosYaw = tlCosPitch * tlCosYaw;
                Q[3] = tlCosRoll * tlCosPitchTimeCosYaw + tlSinRoll * tlSinPitchTimeSinYaw;
                Q[0] = tlSinRoll * tlCosPitchTimeCosYaw - tlCosRoll * tlSinPitchTimeSinYaw;
                Q[1] = tlCosRoll * tlSinPitchTimeCosYaw + tlSinRoll * tlCosPitchTimeSinYaw;
                Q[2] = tlCosRoll * tlCosPitchTimeSinYaw - tlSinRoll * tlSinPitchTimeCosYaw;
                return;
            }
            tlSinYaw = sinf(0.5f * fYaw_p);
            tlCosYaw = cosf(0.5f * fYaw_p);
            tlSinPitch = sinf(0.5f * fPitch_p);
            tlCosPitch = cosf(0.5f * fPitch_p);
            Q[3] = tlCosPitch * tlCosYaw;
            Q[0] = -tlSinPitch * tlSinYaw;
            Q[1] = tlSinPitch * tlCosYaw;
            Q[2] = tlCosPitch * tlSinYaw;
            return;
        }
        if (fRoll_p != 0.0f) {
            tlSinYaw = sinf(0.5f * fYaw_p);
            tlCosYaw = cosf(0.5f * fYaw_p);
            tlSinRoll = sinf(0.5f * fRoll_p);
            tlCosRoll = cosf(0.5f * fRoll_p);
            Q[3] = tlCosRoll * tlCosYaw;
            Q[0] = tlSinRoll * tlCosYaw;
            Q[1] = tlSinRoll * tlSinYaw;
            Q[2] = tlCosRoll * tlSinYaw;
            return;
        }
        tlSinYaw = sinf(0.5f * fYaw_p);
        Q[3] = cosf(0.5f * fYaw_p);
        Q[0] = 0.0f;
        Q[1] = 0.0f;
        Q[2] = tlSinYaw;
        return;
    }
    if (fPitch_p != 0.0f) {
        if (fRoll_p != 0.0f) {
            tlSinPitch = sinf(0.5f * fPitch_p);
            tlCosPitch = cosf(0.5f * fPitch_p);
            tlSinRoll = sinf(0.5f * fRoll_p);
            tlCosRoll = cosf(0.5f * fRoll_p);
            Q[3] = tlCosRoll * tlCosPitch;
            Q[0] = tlSinRoll * tlCosPitch;
            Q[1] = tlCosRoll * tlSinPitch;
            Q[2] = -tlSinRoll * tlSinPitch;
            return;
        }
        tlSinPitch = sinf(0.5f * fPitch_p);
        Q[3] = cosf(0.5f * fPitch_p);
        Q[0] = 0.0f;
        Q[1] = tlSinPitch;
        Q[2] = 0.0f;
        return;
    }
    if (fRoll_p != 0.0f) {
        tlSinRoll = sinf(0.5f * fRoll_p);
        Q[3] = cosf(0.5f * fRoll_p);
        Q[0] = tlSinRoll;
        Q[1] = 0.0f;
        Q[2] = 0.0f;
        return;
    }
    Quat_IdentifyForMul(Q);
}

// The quaternion's squared length.
static f32 Quat_GetNorm(f32* Q) {
    return Q[2] * Q[2] + (Q[1] * Q[1] + (Q[3] * Q[3] + Q[0] * Q[0]));
}

// The inverse quaternion, into dest: the conjugate divided by the squared length (Quat_GetNorm), so
// it also works for a quaternion that is not unit length.
void Quat_Invert(f32* src, f32* dest) {
    f32 tlOneOverNorm;

    tlOneOverNorm = 1.0f / Quat_GetNorm(src);
    dest[3] = src[3] * tlOneOverNorm;
    dest[0] = -src[0] * tlOneOverNorm;
    dest[1] = -src[1] * tlOneOverNorm;
    dest[2] = -src[2] * tlOneOverNorm;
}

// The conjugate (the inverse of a unit quaternion), into dest.
static void Quat_Conjugate(f32* src, f32* dest) {
    dest[3] = src[3];
    dest[0] = -src[0];
    dest[1] = -src[1];
    dest[2] = -src[2];
}

// The quaternion product Q1 x Q2, into Q: the turn by Q1 followed by the turn by Q2. Q must not be
// Q1 or Q2: it is written while they are still being read.
void Quat_Multiply(f32* Q1, f32* Q2, f32* Q) {
    Q[3] = Q1[3] * Q2[3] - (Q1[2] * Q2[2] + (Q1[0] * Q2[0] + Q1[1] * Q2[1]));
    Q[0] = Q2[3] * Q1[0] + (Q1[3] * Q2[0] + (Q1[1] * Q2[2] - Q1[2] * Q2[1]));
    Q[1] = Q2[3] * Q1[1] + (Q1[3] * Q2[1] + (Q1[2] * Q2[0] - Q1[0] * Q2[2]));
    Q[2] = Q2[3] * Q1[2] + (Q1[3] * Q2[2] + (Q1[0] * Q2[1] - Q1[1] * Q2[0]));
}

// The sum Q1 + Q2, into Q.
void Quat_Add(f32* Q1, f32* Q2, f32* Q) {
    Q[3] = Q1[3] + Q2[3];
    Q[0] = Q1[0] + Q2[0];
    Q[1] = Q1[1] + Q2[1];
    Q[2] = Q1[2] + Q2[2];
}

// src turned by the unit quaternion Q, into dest: conj(Q) x src x Q. src is a vector (its w comes
// back as it was) or a quaternion (the same turn about the axis turned by Q). dest may be src, but
// not Q.
void Quat_RotateVector(f32* Q, f32* src, f32* dest) {
    Quat cTempVectorQuat;
    Quat cConjugateOfVectorQuatSrc;

    Quat_Conjugate(Q, cConjugateOfVectorQuatSrc);
    Quat_Multiply(cConjugateOfVectorQuatSrc, src, cTempVectorQuat);
    Quat_Multiply(cTempVectorQuat, Q, dest);
}

// The rotation matrix of the unit quaternion Q, into rows 0-2 of M (their fourth float set to 0);
// row 3 is left as it is.
void Quat_QuatToMatrix(f32* Q, f32 (*M)[4]) {
    f32 wx;
    f32 wy;
    f32 wz;
    f32 xx;
    f32 yy;
    f32 yz;
    f32 xy;
    f32 xz;
    f32 zz;
    f32 x2;
    f32 y2;
    f32 z2;

    x2 = Q[0] + Q[0];
    y2 = Q[1] + Q[1];
    z2 = Q[2] + Q[2];
    xx = Q[0] * x2;
    xy = Q[0] * y2;
    xz = Q[0] * z2;
    yy = Q[1] * y2;
    yz = Q[1] * z2;
    zz = Q[2] * z2;
    wx = Q[3] * x2;
    wy = Q[3] * y2;
    wz = Q[3] * z2;
    M[0][0] = 1.0f - (yy + zz);
    M[0][1] = xy - wz;
    M[0][2] = xz + wy;
    M[0][3] = 0.0f;
    M[1][0] = xy + wz;
    M[1][1] = 1.0f - (xx + zz);
    M[1][2] = yz - wx;
    M[1][3] = 0.0f;
    M[2][0] = xz - wy;
    M[2][1] = yz + wx;
    M[2][2] = 1.0f - (xx + yy);
    M[2][3] = 0.0f;
    M[2][3] = 0.0f;
    M[1][3] = 0.0f;
    M[0][3] = 0.0f;
}

// The turn by the rotation vector v (its direction the axis, its length the angle in radians), into
// Q; the identity when the angle is below 0.001.
void Quat_BuildFromVector(f32* v, f32* Q) {
    f32 angle;
    f32 scale;

    angle = Math_Sqrt(LLMath_LengthSquared3(v));
    if (angle < 0.001f) {
        Quat_IdentifyForMul(Q);
        return;
    }
    Q[3] = cosf(angle / 2.0f);
    scale = -(f32)Math_Sqrt(1.0f - Q[3] * Q[3]) / angle;
    Q[0] = v[0] * scale;
    Q[1] = v[1] * scale;
    Q[2] = v[2] * scale;
}

// Quat_BuildFromVector with the angle passed in: v is still the axis times the angle (it is divided
// by scale), and scale is that angle. The identity when it is below 0.001.
void Quat_BuildFromVectorAndScale(f32* v, f32* Q, f32 scale) {
    if (scale < 0.001f) {
        Quat_IdentifyForMul(Q);
        return;
    }
    Q[3] = cosf(scale / 2.0f);
    scale = -(f32)Math_Sqrt(1.0f - Q[3] * Q[3]) / scale;
    Q[0] = v[0] * scale;
    Q[1] = v[1] * scale;
    Q[2] = v[2] * scale;
}

// The turn by fYaw_p radians about z (x toward y), into Q.
void Legacy_Quat_BuildFromYaw(f32 fYaw_p, f32* Q) {
    Quat_Clear(Q);
    Q[2] = sinf(0.5f * -fYaw_p);
    Q[3] = cosf(0.5f * -fYaw_p);
}

// The turn by fPitch_p radians about y (z toward x), into Q.
void Legacy_Quat_BuildFromPitch(f32 fPitch_p, f32* Q) {
    Quat_Clear(Q);
    Q[1] = sinf(0.5f * -fPitch_p);
    Q[3] = cosf(0.5f * -fPitch_p);
}

// The turn by fRoll_p radians about x (y toward z), into Q.
void Legacy_Quat_BuildFromRoll(f32 fRoll_p, f32* Q) {
    Quat_Clear(Q);
    Q[0] = sinf(0.5f * -fRoll_p);
    Q[3] = cosf(0.5f * -fRoll_p);
}

// The yaw, pitch and roll of the unit quaternion Q, in radians, into *fYaw_p, *fPitch_p and
// *fRoll_p: Quat_EulerAngles(-yaw, -pitch, -roll) gives Q (or -Q) back. Yaw and roll come from atan
// of a ratio, not atan2, so they stay within -pi/2..pi/2 (a turn beyond that does not come back);
// pitch is the asin of a sine clamped to -1..1.
void Quat_ExtractEulerAngles(f32* Q, f32* fYaw_p, f32* fPitch_p, f32* fRoll_p) {
    f32 tan_yaw;
    f32 sin_pitch;
    f32 tan_roll;

    tan_yaw = 2.0f * (Q[0] * Q[1] + Q[3] * Q[2]) /
              (Q[3] * Q[3] + Q[0] * Q[0] - Q[1] * Q[1] - Q[2] * Q[2]);
    sin_pitch = -2.0f * (Q[0] * Q[2] - Q[3] * Q[1]);
    tan_roll = 2.0f * (Q[3] * Q[0] + Q[1] * Q[2]) /
               (Q[2] * Q[2] + (Q[3] * Q[3] - Q[0] * Q[0] - Q[1] * Q[1]));
    sin_pitch = (sin_pitch < -1.0f) ? -1.0f : ((sin_pitch > 1.0f) ? 1.0f : sin_pitch);
    *fYaw_p = atan(tan_yaw);
    *fPitch_p = asinf(sin_pitch);
    *fRoll_p = atan(tan_roll);
}

// The float sin: sin() of x (radians) rounded to a float.
f32 sinf(f32 x) {
    return sin(x);
}

// The float acos: acos() of x rounded to a float, 0..pi radians.
f32 acosf(f32 x) {
    return acos(x);
}

// The float cos: cos() of x (radians) rounded to a float.
f32 cosf(f32 x) {
    return cos(x);
}

// The float asin: asin() of x rounded to a float, -pi/2..pi/2 radians.
f32 asinf(f32 x) {
    return asin(x);
}

// The C library's double square root: four Newton steps from the reciprocal-root estimate
// (__frsqrte); 0 for 0; NaN for a negative x, a NaN, and also +infinity (the estimate is 0 there).
// The TW_INFINITY at the end is never reached.
double Math_Sqrt(double x) {
    double guess;

    if (x > 0.0) {
        guess = __frsqrte(x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        return x * guess;
    } else if (x == 0.0) {
        return 0.0;
    } else if (x) {
        return TW_NAN;
    }
    return TW_INFINITY;
}

// Sets Q to the identity, no turn: (0, 0, 0, 1).
void Quat_IdentifyForMul(f32* Q) {
    Q[3] = 1.0f;
    Q[0] = 0.0f;
    Q[1] = 0.0f;
    Q[2] = 0.0f;
}

// Sets all four floats of Q to 0 (not a rotation: the Legacy_Quat_BuildFrom* functions then fill
// in two of them).
static void Quat_Clear(f32* Q) {
    Q[3] = 0.0f;
    Q[0] = 0.0f;
    Q[1] = 0.0f;
    Q[2] = 0.0f;
}

// The vector's squared length (x, y and z), at most FLT_MAX.
f32 LLMath_LengthSquared3(const f32* A) {
    f32 fLengthSq;

    fLengthSq = A[0] * A[0] + A[1] * A[1] + A[2] * A[2];
    if (fLengthSq > __float_max[0]) {
        fLengthSq = __float_max[0];
    }
    return fLengthSq;
}
