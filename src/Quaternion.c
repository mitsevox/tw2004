// Quaternion.c (our name): rotations as quaternions (x, y, z, w in f32[4]) for the skeleton,
// the cameras and the ball, and the float math helpers they use. It may be the tail of GoEntry.c:
// no assert or data block separates the two.

#include "engine.h"

f32 fn_80008EF0(f32* pQ);

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

// The rotation matrix (3 rows of 4, no translation) of a unit quaternion.
void fn_8000914C(f32* pQ, f32 (*m)[4]) {
    f32 fY2;
    f32 fZ2;
    f32 fX2;
    f32 fYY;
    f32 fZZ;
    f32 fXX;
    f32 fXY;
    f32 fWZ;
    f32 fXZ;
    f32 fWY;
    f32 fYZ;
    f32 fWX;

    fY2 = pQ[1] + pQ[1];
    fZ2 = pQ[2] + pQ[2];
    fX2 = pQ[0] + pQ[0];
    fYY = pQ[1] * fY2;
    fZZ = pQ[2] * fZ2;
    fXX = pQ[0] * fX2;
    fXY = pQ[0] * fY2;
    fWZ = pQ[3] * fZ2;
    fXZ = pQ[0] * fZ2;
    fWY = pQ[3] * fY2;
    fYZ = pQ[1] * fZ2;
    fWX = pQ[3] * fX2;
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

f32 fn_800095F0(f32 fAngle) {
    return sin(fAngle);
}

f32 fn_80009614(f32 x) {
    return fn_8015F784(x);
}

f32 fn_80009638(f32 fAngle) {
    return cos(fAngle);
}

f32 fn_8000965C(f32 x) {
    return fn_8015F7A4(x);
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
