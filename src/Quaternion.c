// Quaternion.c (our name): rotations as quaternions (x, y, z, w in f32[4]) for the skeleton,
// the cameras and the ball, and the float math helpers they use. It may be the tail of GoEntry.c:
// no assert or data block separates the two.

#include "engine.h"

f32 fn_80008EF0(f32* pQ);
void fn_8000972C(f32* pQ);

// Spherical interpolation from a to b by fT, into b. b is flipped when the two are more than a
// half turn apart, and a straight blend is used when they are almost the same.
void fn_8000883C(f32* pA, f32* pB, f32 fT) {
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
        fAngle = fn_80009614(fCos);
        fSin = fn_800095F0(fAngle);
        fScaleA = fn_800095F0((1.0f - fT) * fAngle) / fSin;
        fScaleB = fn_800095F0(fT * fAngle) / fSin;
    } else {
        fScaleA = 1.0f - fT;
        fScaleB = fT;
    }
    pB[0] = fScaleA * pA[0] + fScaleB * fX;
    pB[1] = fScaleA * pA[1] + fScaleB * fY;
    pB[2] = fScaleA * pA[2] + fScaleB * fZ;
    pB[3] = fScaleA * pA[3] + fScaleB * fW;
}

// The unit quaternion of a rotation matrix, into pQ: from the trace when it is positive,
// otherwise from the largest diagonal element.
void fn_800089D4(f32 (*m)[4], f32* pQ) {
    int anNext[3] = {1, 2, 0};
    f32 aQ[4];
    f32 fTrace;
    f32 fS;
    int i;
    int j;
    int k;

    fTrace = m[2][2] + (m[0][0] + m[1][1]);
    if (fTrace > 0.0f) {
        fS = fn_80009680(fTrace + 1.0f);
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
    fS = fn_80009680(m[i][i] - (m[j][j] + m[k][k]) + 1.0f);
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

// fake match: puts fn_80009680's double constants (0.0, 0.5, 3.0) in the pool where the original
// has them, right after fn_800089D4's (0x80282A98); why EA's pool has them there is unknown.
static double Quaternion_StrippedFn(double x) {
    double g = 0.0;

    if (x > g) {
        g = 0.5 * x * (3.0 - x);
    }
    return g;
}

// The quaternion of three angles (each negated), into pOut. An angle of exactly 0 skips its sin
// and cos.
void fn_80008BB8(f32 fA, f32 fB, f32 fC, f32* pOut) {
    f32 fHalf;
    f32 fSinA;
    f32 fCosA;
    f32 fSinB;
    f32 fCosB;
    f32 fSinC;
    f32 fCosC;
    f32 fSS;
    f32 fCC;
    f32 fSC;
    f32 fCS;

    fA = -fA;
    fB = -fB;
    fC = -fC;
    if (fA != 0.0f) {
        if (fB != 0.0f) {
            if (fC != 0.0f) {
                fHalf = 0.5f * fA;
                fSinA = fn_800095F0(fHalf);
                fCosA = fn_80009638(fHalf);
                fHalf = 0.5f * fB;
                fSinB = fn_800095F0(fHalf);
                fCosB = fn_80009638(fHalf);
                fHalf = 0.5f * fC;
                fSinC = fn_800095F0(fHalf);
                fCosC = fn_80009638(fHalf);
                fSS = fSinB * fSinA;
                fCS = fCosB * fSinA;
                fSC = fSinB * fCosA;
                fCC = fCosB * fCosA;
                pOut[3] = fCosC * fCC + fSinC * fSS;
                pOut[0] = fSinC * fCC - fCosC * fSS;
                pOut[1] = fCosC * fSC + fSinC * fCS;
                pOut[2] = fCosC * fCS - fSinC * fSC;
                return;
            }
            fHalf = 0.5f * fA;
            fSinA = fn_800095F0(fHalf);
            fCosA = fn_80009638(fHalf);
            fHalf = 0.5f * fB;
            fSinB = fn_800095F0(fHalf);
            fCosB = fn_80009638(fHalf);
            pOut[3] = fCosB * fCosA;
            pOut[0] = -fSinB * fSinA;
            pOut[1] = fSinB * fCosA;
            pOut[2] = fCosB * fSinA;
            return;
        }
        if (fC != 0.0f) {
            fHalf = 0.5f * fA;
            fSinA = fn_800095F0(fHalf);
            fCosA = fn_80009638(fHalf);
            fHalf = 0.5f * fC;
            fSinC = fn_800095F0(fHalf);
            fCosC = fn_80009638(fHalf);
            pOut[3] = fCosC * fCosA;
            pOut[0] = fSinC * fCosA;
            pOut[1] = fSinC * fSinA;
            pOut[2] = fCosC * fSinA;
            return;
        }
        fHalf = 0.5f * fA;
        fSinA = fn_800095F0(fHalf);
        pOut[3] = fn_80009638(fHalf);
        pOut[0] = 0.0f;
        pOut[1] = 0.0f;
        pOut[2] = fSinA;
        return;
    }
    if (fB != 0.0f) {
        if (fC != 0.0f) {
            fHalf = 0.5f * fB;
            fSinB = fn_800095F0(fHalf);
            fCosB = fn_80009638(fHalf);
            fHalf = 0.5f * fC;
            fSinC = fn_800095F0(fHalf);
            fCosC = fn_80009638(fHalf);
            pOut[3] = fCosC * fCosB;
            pOut[0] = fSinC * fCosB;
            pOut[1] = fCosC * fSinB;
            pOut[2] = -fSinC * fSinB;
            return;
        }
        fHalf = 0.5f * fB;
        fSinB = fn_800095F0(fHalf);
        pOut[3] = fn_80009638(fHalf);
        pOut[0] = 0.0f;
        pOut[1] = fSinB;
        pOut[2] = 0.0f;
        return;
    }
    if (fC != 0.0f) {
        fHalf = 0.5f * fC;
        fSinC = fn_800095F0(fHalf);
        pOut[3] = fn_80009638(fHalf);
        pOut[0] = fSinC;
        pOut[1] = 0.0f;
        pOut[2] = 0.0f;
        return;
    }
    fn_80009710(pOut);
}

// The quaternion's squared length.
f32 fn_80008EF0(f32* pQ) {
    return pQ[2] * pQ[2] + (pQ[1] * pQ[1] + (pQ[3] * pQ[3] + pQ[0] * pQ[0]));
}

// The inverse rotation, into pOut.
void fn_80008F20(f32* pQ, f32* pOut) {
    f32 fScale;

    fScale = 1.0f / fn_80008EF0(pQ);
    pOut[3] = pQ[3] * fScale;
    pOut[0] = -pQ[0] * fScale;
    pOut[1] = -pQ[1] * fScale;
    pOut[2] = -pQ[2] * fScale;
}

// The conjugate (the inverse of a unit quaternion), into pOut.
void fn_80008F9C(f32* pQ, f32* pOut) {
    pOut[3] = pQ[3];
    pOut[0] = -pQ[0];
    pOut[1] = -pQ[1];
    pOut[2] = -pQ[2];
}

// The product a x b, into pOut.
void fn_80008FCC(f32* pA, f32* pB, f32* pOut) {
    pOut[3] = pA[3] * pB[3] - (pA[2] * pB[2] + (pA[0] * pB[0] + pA[1] * pB[1]));
    pOut[0] = pB[3] * pA[0] + (pA[3] * pB[0] + (pA[1] * pB[2] - pA[2] * pB[1]));
    pOut[1] = pB[3] * pA[1] + (pA[3] * pB[1] + (pA[2] * pB[0] - pA[0] * pB[2]));
    pOut[2] = pB[3] * pA[2] + (pA[3] * pB[2] + (pA[0] * pB[1] - pA[1] * pB[0]));
}

// The sum a + b, into pOut.
void fn_800090A0(f32* pA, f32* pB, f32* pOut) {
    pOut[3] = pA[3] + pB[3];
    pOut[0] = pA[0] + pB[0];
    pOut[1] = pA[1] + pB[1];
    pOut[2] = pA[2] + pB[2];
}

// b turned by the inverse of the unit quaternion a (conj(a) x b x a), into pOut.
void fn_800090E4(f32* pA, f32* pB, f32* pOut) {
    f32 aTmp[4];
    f32 aConj[4];

    fn_80008F9C(pA, aConj);
    fn_80008FCC(aConj, pB, aTmp);
    fn_80008FCC(aTmp, pA, pOut);
}

// The rotation matrix (3 rows of 4, no translation) of a unit quaternion.
void fn_8000914C(f32* pQ, f32 (*m)[4]) {
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

// A rotation vector (axis * angle) as a quaternion, into pOut; the identity when the angle is
// below 0.001.
void fn_8000923C(f32* pRot, f32* pOut) {
    f32 fAngle;
    f32 fScale;

    fAngle = fn_80009680(fn_80009744(pRot));
    if (fAngle < 0.001f) {
        fn_80009710(pOut);
        return;
    }
    pOut[3] = fn_80009638(fAngle / 2.0f);
    fScale = -(f32)fn_80009680(1.0f - pOut[3] * pOut[3]) / fAngle;
    pOut[0] = pRot[0] * fScale;
    pOut[1] = pRot[1] * fScale;
    pOut[2] = pRot[2] * fScale;
}

// A rotation by fAngle about the unit axis pAxis as a quaternion, into pOut; the identity when
// the angle is below 0.001.
void fn_800092F8(f32* pAxis, f32* pOut, f32 fAngle) {
    if (fAngle < 0.001f) {
        fn_80009710(pOut);
        return;
    }
    pOut[3] = fn_80009638(fAngle / 2.0f);
    // fake match: fAngle is reused for the axis scale (the original keeps both in one register)
    fAngle = -(f32)fn_80009680(1.0f - pOut[3] * pOut[3]) / fAngle;
    pOut[0] = pAxis[0] * fAngle;
    pOut[1] = pAxis[1] * fAngle;
    pOut[2] = pAxis[2] * fAngle;
}

// A rotation by -fAngle about z, as a quaternion into pOut.
void fn_800093AC(f32 fAngle, f32* pOut) {
    f32 fHalf;

    fn_8000972C(pOut);
    fHalf = 0.5f * -fAngle;
    pOut[2] = fn_800095F0(fHalf);
    pOut[3] = fn_80009638(fHalf);
}

// A rotation by -fAngle about y, as a quaternion into pOut.
void fn_80009410(f32 fAngle, f32* pOut) {
    f32 fHalf;

    fn_8000972C(pOut);
    fHalf = 0.5f * -fAngle;
    pOut[1] = fn_800095F0(fHalf);
    pOut[3] = fn_80009638(fHalf);
}

// A rotation by -fAngle about x, as a quaternion into pOut.
void fn_80009474(f32 fAngle, f32* pOut) {
    f32 fHalf;

    fn_8000972C(pOut);
    fHalf = 0.5f * -fAngle;
    pOut[0] = fn_800095F0(fHalf);
    pOut[3] = fn_80009638(fHalf);
}

// The three angles of a unit quaternion, into *pA, *pB and *pC (the middle one from an asin,
// its sine clamped to -1..1).
void fn_800094D8(f32* pQ, f32* pA, f32* pB, f32* pC) {
    f32 fTanA;
    f32 fSinB;
    f32 fTanC;

    fTanA = 2.0f * (pQ[0] * pQ[1] + pQ[3] * pQ[2]) /
            (pQ[3] * pQ[3] + pQ[0] * pQ[0] - pQ[1] * pQ[1] - pQ[2] * pQ[2]);
    fSinB = -2.0f * (pQ[0] * pQ[2] - pQ[3] * pQ[1]);
    fTanC = 2.0f * (pQ[3] * pQ[0] + pQ[1] * pQ[2]) /
            (pQ[2] * pQ[2] + (pQ[3] * pQ[3] - pQ[0] * pQ[0] - pQ[1] * pQ[1]));
    fSinB = (fSinB < -1.0f) ? -1.0f : ((fSinB > 1.0f) ? 1.0f : fSinB);
    *pA = atan(fTanA);
    *pB = fn_8000965C(fSinB);
    *pC = atan(fTanC);
}

f32 fn_800095F0(f32 fAngle) {
    return sin(fAngle);
}

f32 fn_80009614(f32 x) {
    return acos(x);
}

f32 fn_80009638(f32 fAngle) {
    return cos(fAngle);
}

f32 fn_8000965C(f32 x) {
    return asin(x);
}

// Square root: four Newton steps from the reciprocal-root estimate; 0 for 0, NaN for a negative
// x or a NaN (the infinity at the end is never reached).
double fn_80009680(double x) {
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

// The identity rotation.
void fn_80009710(f32* pQ) {
    pQ[3] = 1.0f;
    pQ[0] = 0.0f;
    pQ[1] = 0.0f;
    pQ[2] = 0.0f;
}

void fn_8000972C(f32* pQ) {
    pQ[3] = 0.0f;
    pQ[0] = 0.0f;
    pQ[1] = 0.0f;
    pQ[2] = 0.0f;
}

// The vector's squared length, at most FLT_MAX.
f32 fn_80009744(f32* pVec) {
    f32 f;

    f = pVec[0] * pVec[0] + pVec[1] * pVec[1] + pVec[2] * pVec[2];
    if (f > lbl_80281B40[0]) {
        f = lbl_80281B40[0];
    }
    return f;
}
