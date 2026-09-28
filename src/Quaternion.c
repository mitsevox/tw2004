// Quaternion.c (our name; EA's file is MathQuat.c, TW07's legacy/lib/MathQuat.c has the same
// functions in the same order from Quat_Slerp to Quat_ExtractEulerAngles, with Quat_Add and
// Quat_Multiply swapped): rotations as quaternions (x, y, z, w in f32[4]) for the skeleton,
// animation and the cameras. After them come the float math wrappers (Math_Sin, Math_Acos,
// Math_Cos, Math_Asin, Math_Sqrt; the ball uses them too), Quat_IdentifyForMul and Quat_Clear
// (inlines in TW07) and Vec3_LengthSqClamped.

#include "engine.h"

f32 Quat_GetNorm(f32* pQ);
void Quat_Clear(f32* pQ);

// Spherical interpolation between the unit quaternions a and b by fT (0 gives a, 1 gives b),
// written over b. b is negated first when a . b < 0 (the shorter way round), and a straight blend
// is used when 1 - a . b is at most 0.01 (the two almost the same).
void Quat_Slerp(f32* pA, f32* pB, f32 fT) {
    f32 fX;
    f32 fY;
    f32 fZ;
    f32 fW;
    f32 fScaleA;
    f32 fSin;
    f32 fAngle;
    f32 fScaleB;
    f32 fCos;

    // fake match: b is copied inside the dot product, for the original's load order
    fCos = pA[3] * (fW = pB[3]) +
           (pA[2] * (fZ = pB[2]) + (pA[0] * (fX = pB[0]) + pA[1] * (fY = pB[1])));
    if (fCos < 0.0f) {
        fCos = -fCos;
        fX = -fX;
        fY = -fY;
        fZ = -fZ;
        fW = -fW;
    }
    if (1.0f - fCos > 0.01f) {
        fAngle = Math_Acos(fCos);
        fSin = Math_Sin(fAngle);
        fScaleA = Math_Sin((1.0f - fT) * fAngle) / fSin;
        fScaleB = Math_Sin(fT * fAngle) / fSin;
    } else {
        fScaleA = 1.0f - fT;
        fScaleB = fT;
    }
    pB[0] = fScaleA * pA[0] + fScaleB * fX;
    pB[1] = fScaleA * pA[1] + fScaleB * fY;
    pB[2] = fScaleA * pA[2] + fScaleB * fZ;
    pB[3] = fScaleA * pA[3] + fScaleB * fW;
}

// The unit quaternion of a rotation matrix's 3x3 part, into pQ (the reverse of Quat_QuatToMatrix):
// from the trace when it is positive, otherwise from the largest diagonal element.
void Quat_BuildFromMatrix(f32 (*m)[4], f32* pQ) {
    int anNext[3] = {1, 2, 0};
    f32 aQ[4];
    f32 fTrace;
    f32 fS;
    int i;
    int j;
    int k;

    fTrace = m[2][2] + (m[0][0] + m[1][1]);
    if (fTrace > 0.0f) {
        fS = Math_Sqrt(fTrace + 1.0f);
        pQ[3] = 0.5f * fS;
        fS = 0.5f / fS;
        pQ[0] = fS * -(m[1][2] - m[2][1]);
        pQ[1] = fS * -(m[2][0] - m[0][2]);
        pQ[2] = fS * -(m[0][1] - m[1][0]);
        return;
    }
    i = 0;
    if (m[1][1] > m[0][0]) {
        i = 1;
    }
    if (m[2][2] > m[i][i]) {
        i = 2;
    }
    j = anNext[i];
    k = anNext[j];
    fS = Math_Sqrt(m[i][i] - (m[j][j] + m[k][k]) + 1.0f);
    aQ[i] = 0.5f * fS;
    if (fS != 0.0f) {
        fS = 0.5f / fS;
    }
    aQ[3] = fS * (m[j][k] - m[k][j]);
    aQ[j] = fS * (m[i][j] + m[j][i]);
    aQ[k] = fS * (m[i][k] + m[k][i]);
    pQ[3] = aQ[3];
    pQ[0] = -aQ[0];
    pQ[1] = -aQ[1];
    pQ[2] = -aQ[2];
}

// fake match: puts Math_Sqrt's double constants (0.0, 0.5, 3.0) in the pool where the original
// has them, right after Quat_BuildFromMatrix's (0x80282A98); why EA's pool has them there is unknown.
static double Quaternion_StrippedFn(double x) {
    double g = 0.0;

    if (x > g) {
        g = 0.5 * x * (3.0 - x);
    }
    return g;
}

// The quaternion yaw x pitch x roll, into pOut: yaw turns about z, pitch about y and roll about x
// (radians), each angle negated first as in Legacy_Quat_BuildFromYaw/Pitch/Roll. An angle of
// exactly 0 skips its sin and cos; all three 0 give the identity.
void Quat_EulerAngles(f32 fYaw, f32 fPitch, f32 fRoll, f32* pOut) {
    f32 fHalf;
    f32 fSinYaw;
    f32 fCosYaw;
    f32 fSinPitch;
    f32 fCosPitch;
    f32 fSinRoll;
    f32 fCosRoll;
    f32 fSinPitchSinYaw;
    f32 fCosPitchCosYaw;
    f32 fSinPitchCosYaw;
    f32 fCosPitchSinYaw;

    fYaw = -fYaw;
    fPitch = -fPitch;
    fRoll = -fRoll;
    if (fYaw != 0.0f) {
        if (fPitch != 0.0f) {
            if (fRoll != 0.0f) {
                fHalf = 0.5f * fYaw;
                fSinYaw = Math_Sin(fHalf);
                fCosYaw = Math_Cos(fHalf);
                fHalf = 0.5f * fPitch;
                fSinPitch = Math_Sin(fHalf);
                fCosPitch = Math_Cos(fHalf);
                fHalf = 0.5f * fRoll;
                fSinRoll = Math_Sin(fHalf);
                fCosRoll = Math_Cos(fHalf);
                fSinPitchSinYaw = fSinPitch * fSinYaw;
                fCosPitchSinYaw = fCosPitch * fSinYaw;
                fSinPitchCosYaw = fSinPitch * fCosYaw;
                fCosPitchCosYaw = fCosPitch * fCosYaw;
                pOut[3] = fCosRoll * fCosPitchCosYaw + fSinRoll * fSinPitchSinYaw;
                pOut[0] = fSinRoll * fCosPitchCosYaw - fCosRoll * fSinPitchSinYaw;
                pOut[1] = fCosRoll * fSinPitchCosYaw + fSinRoll * fCosPitchSinYaw;
                pOut[2] = fCosRoll * fCosPitchSinYaw - fSinRoll * fSinPitchCosYaw;
                return;
            }
            fHalf = 0.5f * fYaw;
            fSinYaw = Math_Sin(fHalf);
            fCosYaw = Math_Cos(fHalf);
            fHalf = 0.5f * fPitch;
            fSinPitch = Math_Sin(fHalf);
            fCosPitch = Math_Cos(fHalf);
            pOut[3] = fCosPitch * fCosYaw;
            pOut[0] = -fSinPitch * fSinYaw;
            pOut[1] = fSinPitch * fCosYaw;
            pOut[2] = fCosPitch * fSinYaw;
            return;
        }
        if (fRoll != 0.0f) {
            fHalf = 0.5f * fYaw;
            fSinYaw = Math_Sin(fHalf);
            fCosYaw = Math_Cos(fHalf);
            fHalf = 0.5f * fRoll;
            fSinRoll = Math_Sin(fHalf);
            fCosRoll = Math_Cos(fHalf);
            pOut[3] = fCosRoll * fCosYaw;
            pOut[0] = fSinRoll * fCosYaw;
            pOut[1] = fSinRoll * fSinYaw;
            pOut[2] = fCosRoll * fSinYaw;
            return;
        }
        fHalf = 0.5f * fYaw;
        fSinYaw = Math_Sin(fHalf);
        pOut[3] = Math_Cos(fHalf);
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = fSinYaw;
        return;
    }
    if (fPitch != 0.0f) {
        if (fRoll != 0.0f) {
            fHalf = 0.5f * fPitch;
            fSinPitch = Math_Sin(fHalf);
            fCosPitch = Math_Cos(fHalf);
            fHalf = 0.5f * fRoll;
            fSinRoll = Math_Sin(fHalf);
            fCosRoll = Math_Cos(fHalf);
            pOut[3] = fCosRoll * fCosPitch;
            pOut[0] = fSinRoll * fCosPitch;
            pOut[1] = fCosRoll * fSinPitch;
            pOut[2] = -fSinRoll * fSinPitch;
            return;
        }
        fHalf = 0.5f * fPitch;
        fSinPitch = Math_Sin(fHalf);
        pOut[3] = Math_Cos(fHalf);
        pOut[0] = 0.0f;
        pOut[1] = fSinPitch;
        pOut[2] = 0.0f;
        return;
    }
    if (fRoll != 0.0f) {
        fHalf = 0.5f * fRoll;
        fSinRoll = Math_Sin(fHalf);
        pOut[3] = Math_Cos(fHalf);
        pOut[0] = fSinRoll;
        pOut[1] = 0.0f;
        pOut[2] = 0.0f;
        return;
    }
    Quat_IdentifyForMul(pOut);
}

// The quaternion's squared length.
f32 Quat_GetNorm(f32* pQ) {
    return pQ[2] * pQ[2] + (pQ[1] * pQ[1] + (pQ[3] * pQ[3] + pQ[0] * pQ[0]));
}

// The inverse quaternion, into pOut: the conjugate divided by the squared length (Quat_GetNorm), so
// it also works for a quaternion that is not unit length.
void Quat_Invert(f32* pQ, f32* pOut) {
    f32 fScale;

    fScale = 1.0f / Quat_GetNorm(pQ);
    pOut[3] = pQ[3] * fScale;
    pOut[0] = -pQ[0] * fScale;
    pOut[1] = -pQ[1] * fScale;
    pOut[2] = -pQ[2] * fScale;
}

// The conjugate (the inverse of a unit quaternion), into pOut.
void Quat_Conjugate(f32* pQ, f32* pOut) {
    pOut[3] = pQ[3];
    pOut[0] = -pQ[0];
    pOut[1] = -pQ[1];
    pOut[2] = -pQ[2];
}

// The quaternion product a x b, into pOut. pOut must not be pA or pB: it is written while they are
// still being read.
void Quat_Multiply(f32* pA, f32* pB, f32* pOut) {
    pOut[3] = pA[3] * pB[3] - (pA[2] * pB[2] + (pA[0] * pB[0] + pA[1] * pB[1]));
    pOut[0] = pB[3] * pA[0] + (pA[3] * pB[0] + (pA[1] * pB[2] - pA[2] * pB[1]));
    pOut[1] = pB[3] * pA[1] + (pA[3] * pB[1] + (pA[2] * pB[0] - pA[0] * pB[2]));
    pOut[2] = pB[3] * pA[2] + (pA[3] * pB[2] + (pA[0] * pB[1] - pA[1] * pB[0]));
}

// The sum a + b, into pOut.
void Quat_Add(f32* pA, f32* pB, f32* pOut) {
    pOut[3] = pA[3] + pB[3];
    pOut[0] = pA[0] + pB[0];
    pOut[1] = pA[1] + pB[1];
    pOut[2] = pA[2] + pB[2];
}

// Turns the vector pB, held as a quaternion (x, y, z, w), by the inverse of the unit quaternion pA:
// conj(a) x b x a, into pOut (pOut may be pB, but not pA).
void Quat_RotateVector(f32* pA, f32* pB, f32* pOut) {
    f32 aTmp[4];
    f32 aConj[4];

    Quat_Conjugate(pA, aConj);
    Quat_Multiply(aConj, pB, aTmp);
    Quat_Multiply(aTmp, pA, pOut);
}

// The rotation matrix of a unit quaternion, into rows 0-2 of m (their fourth float set to 0); row 3
// is left as it is.
void Quat_QuatToMatrix(f32* pQ, f32 (*m)[4]) {
    f32 fWX;
    f32 fWY;
    f32 fWZ;
    f32 fXX;
    f32 fYY;
    f32 fYZ;
    f32 fXY;
    f32 fXZ;
    f32 fZZ;
    f32 fX2;
    f32 fY2;
    f32 fZ2;

    fX2 = pQ[0] + pQ[0];
    fY2 = pQ[1] + pQ[1];
    fZ2 = pQ[2] + pQ[2];
    fXX = pQ[0] * fX2;
    fXY = pQ[0] * fY2;
    fXZ = pQ[0] * fZ2;
    fYY = pQ[1] * fY2;
    fYZ = pQ[1] * fZ2;
    fZZ = pQ[2] * fZ2;
    fWX = pQ[3] * fX2;
    fWY = pQ[3] * fY2;
    fWZ = pQ[3] * fZ2;
    m[0][0] = 1.0f - (fYY + fZZ);
    m[0][1] = fXY - fWZ;
    m[0][2] = fXZ + fWY;
    m[0][3] = 0.0f;
    m[1][0] = fXY + fWZ;
    m[1][1] = 1.0f - (fXX + fZZ);
    m[1][2] = fYZ - fWX;
    m[1][3] = 0.0f;
    m[2][0] = fXZ - fWY;
    m[2][1] = fYZ + fWX;
    m[2][2] = 1.0f - (fXX + fYY);
    m[2][3] = 0.0f;
    m[2][3] = 0.0f;
    m[1][3] = 0.0f;
    m[0][3] = 0.0f;
}

// The quaternion of a rotation vector pRot (axis times angle, radians), into pOut. Like
// Quat_EulerAngles it turns by minus the angle: x, y, z are -axis * sin(angle / 2). The identity
// when the angle is below 0.001.
void Quat_BuildFromVector(f32* pRot, f32* pOut) {
    f32 fAngle;
    f32 fScale;

    fAngle = Math_Sqrt(Vec3_LengthSqClamped(pRot));
    if (fAngle < 0.001f) {
        Quat_IdentifyForMul(pOut);
        return;
    }
    pOut[3] = Math_Cos(fAngle / 2.0f);
    fScale = -(f32)Math_Sqrt(1.0f - pOut[3] * pOut[3]) / fAngle;
    pOut[0] = pRot[0] * fScale;
    pOut[1] = pRot[1] * fScale;
    pOut[2] = pRot[2] * fScale;
}

// Quat_BuildFromVector with the angle passed in: pAxis is still axis times angle (it is divided by
// fAngle), and fAngle is its length. The identity when fAngle is below 0.001.
void Quat_BuildFromVectorAndScale(f32* pAxis, f32* pOut, f32 fAngle) {
    if (fAngle < 0.001f) {
        Quat_IdentifyForMul(pOut);
        return;
    }
    pOut[3] = Math_Cos(fAngle / 2.0f);
    // fake match: fAngle is reused for the axis scale (the original keeps both in one register)
    fAngle = -(f32)Math_Sqrt(1.0f - pOut[3] * pOut[3]) / fAngle;
    pOut[0] = pAxis[0] * fAngle;
    pOut[1] = pAxis[1] * fAngle;
    pOut[2] = pAxis[2] * fAngle;
}

// A rotation by -fAngle about z, as a quaternion into pOut.
void Legacy_Quat_BuildFromYaw(f32 fAngle, f32* pOut) {
    f32 fHalf;

    Quat_Clear(pOut);
    fHalf = 0.5f * -fAngle;
    pOut[2] = Math_Sin(fHalf);
    pOut[3] = Math_Cos(fHalf);
}

// A rotation by -fAngle about y, as a quaternion into pOut.
void Legacy_Quat_BuildFromPitch(f32 fAngle, f32* pOut) {
    f32 fHalf;

    Quat_Clear(pOut);
    fHalf = 0.5f * -fAngle;
    pOut[1] = Math_Sin(fHalf);
    pOut[3] = Math_Cos(fHalf);
}

// A rotation by -fAngle about x, as a quaternion into pOut.
void Legacy_Quat_BuildFromRoll(f32 fAngle, f32* pOut) {
    f32 fHalf;

    Quat_Clear(pOut);
    fHalf = 0.5f * -fAngle;
    pOut[0] = Math_Sin(fHalf);
    pOut[3] = Math_Cos(fHalf);
}

// The yaw (about z), pitch (about y) and roll (about x) of a unit quaternion, in radians, into
// *pYaw, *pPitch and *pRoll. Yaw and roll come from atan of a ratio, not atan2, so they stay within
// -pi/2..pi/2; pitch is the asin of a sine clamped to -1..1. They are the angles of q itself, so
// they have the opposite sign of the angles Quat_EulerAngles takes.
void Quat_ExtractEulerAngles(f32* pQ, f32* pYaw, f32* pPitch, f32* pRoll) {
    f32 fTanYaw;
    f32 fSinPitch;
    f32 fTanRoll;

    fTanYaw = 2.0f * (pQ[0] * pQ[1] + pQ[3] * pQ[2]) /
              (pQ[3] * pQ[3] + pQ[0] * pQ[0] - pQ[1] * pQ[1] - pQ[2] * pQ[2]);
    fSinPitch = -2.0f * (pQ[0] * pQ[2] - pQ[3] * pQ[1]);
    fTanRoll = 2.0f * (pQ[3] * pQ[0] + pQ[1] * pQ[2]) /
               (pQ[2] * pQ[2] + (pQ[3] * pQ[3] - pQ[0] * pQ[0] - pQ[1] * pQ[1]));
    fSinPitch = (fSinPitch < -1.0f) ? -1.0f : ((fSinPitch > 1.0f) ? 1.0f : fSinPitch);
    *pYaw = atan(fTanYaw);
    *pPitch = Math_Asin(fSinPitch);
    *pRoll = atan(fTanRoll);
}

// Sine of fAngle (radians), through the double-precision sin().
f32 Math_Sin(f32 fAngle) {
    return sin(fAngle);
}

// Arc cosine in radians (0..pi), through the double-precision acos().
f32 Math_Acos(f32 x) {
    return acos(x);
}

// Cosine of fAngle (radians), through the double-precision cos().
f32 Math_Cos(f32 fAngle) {
    return cos(fAngle);
}

// Arc sine in radians (-pi/2..pi/2), through the double-precision asin().
f32 Math_Asin(f32 x) {
    return asin(x);
}

// Square root of a double: four Newton steps from the reciprocal-root estimate (__frsqrte). 0 for
// 0; NaN for a negative x, a NaN, and also +infinity (the estimate is 0 there). The TW_INFINITY at
// the end is never reached.
double Math_Sqrt(double x) {
    double g;

    if (x > 0.0) {
        g = __frsqrte(x);
        g = 0.5 * g * (3.0 - g * g * x);
        g = 0.5 * g * (3.0 - g * g * x);
        g = 0.5 * g * (3.0 - g * g * x);
        g = 0.5 * g * (3.0 - g * g * x);
        return x * g;
    } else if (x == 0.0) {
        return 0.0;
    } else if (x) {
        return TW_NAN;
    }
    return TW_INFINITY;
}

// Sets pQ to the identity rotation (0, 0, 0, 1).
void Quat_IdentifyForMul(f32* pQ) {
    pQ[3] = 1.0f;
    pQ[0] = 0.0f;
    pQ[1] = 0.0f;
    pQ[2] = 0.0f;
}

// Sets all four floats of pQ to 0 (not a rotation: the Legacy_Quat_BuildFrom* functions then fill
// in two of them).
void Quat_Clear(f32* pQ) {
    pQ[3] = 0.0f;
    pQ[0] = 0.0f;
    pQ[1] = 0.0f;
    pQ[2] = 0.0f;
}

// The vector's squared length, at most FLT_MAX.
f32 Vec3_LengthSqClamped(f32* pVec) {
    f32 f;

    f = pVec[0] * pVec[0] + pVec[1] * pVec[1] + pVec[2] * pVec[2];
    if (f > __float_max[0]) {
        f = __float_max[0];
    }
    return f;
}
