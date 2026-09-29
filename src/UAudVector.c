// UAudVector.c (EA's name: TW07's Golf/Audio/Engine/Utils/UAudVector.c has audvec2_ApproxLength
// and audvec3_ApproxLength, audvec3's locals as here; TW06 lists uaudvector.c with the same audio
// utilities): the sound engine's vector maths. This build keeps only the length estimate without a
// square root, which AudTable.c uses for a sound's distance. audfrac_Swap and audvec2_Set are
// inlines of TW07's UAudFrac.h and UAudVector.h that the compiler emitted out of line at the unit's
// end, which is how this unit's end (0x800B1AA8) shows. Linked between startUp.c and
// Code800B1AA8.c.

#include "engine.h"

void   audfrac_Swap(f32* pV1, f32* pV2);
void   audvec2_Set(f32* dest, f32 x, f32 y);

// Estimate the length of the 2D vector v without a square root. With x the longer side and y the
// shorter (both made positive): x + 31/128 y when y is at most half of x, else (106 x + 75 y) /
// 128.
f32 audvec2_ApproxLength(f32* v) {
    f32 x = v[0];
    f32 y = v[1];
    f32 fSum;
    f32 fDiff;
    if (x < 0.0f) {
        x = -x;
    }
    if (y < 0.0f) {
        y = -y;
    }
    if (y > x) {
        audfrac_Swap(&x, &y);
    }
    if (y > 0.5f * x) {
        fSum = x + y;
        fDiff = x - y;
        fSum = 32.0f * y + (x + (2.0f * fSum + (64.0f * fSum + 8.0f * fSum)));
        y = fDiff;
        x = (1.0f / 128.0f) * fSum;
    }
    return x + 0.25f * y - 0.0078125f * y;
}

// The same estimate for the 3D vector v: the length of (v[2], the length of (v[0], v[1])).
// AudTable.c's Distance3D measures a sound's distance with it.
f32 audvec3_ApproxLength(f32* v) {
    f32 tmpv[2];
    f32 tmp;

    tmp = audvec2_ApproxLength(v);
    audvec2_Set(tmpv, v[2], tmp);
    return audvec2_ApproxLength(tmpv);
}

// Swap the floats *pV1 and *pV2.
void audfrac_Swap(f32* pV1, f32* pV2) {
    f32 temp;

    temp = *pV2;
    *pV2 = *pV1;
    *pV1 = temp;
}

void audvec2_Set(f32* dest, f32 x, f32 y) {
    dest[0] = x;
    dest[1] = y;
}
