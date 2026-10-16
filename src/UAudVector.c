// UAudVector.c: the sound engine's vector maths, a length estimate without a square root, which the
// sound emitters use for each sound's distance from the listeners.

#include "engine.h"

static void audfrac_Swap(f32* pV1, f32* pV2);
static void audvec2_Set(f32* dest, f32 x, f32 y);

// Estimate the length of the 2D vector v without a square root. With x the longer side and y the
// shorter (both made positive): x + 31/128 y when y is at most half of x, else (106 x + 75 y) /
// 128.
static f32 audvec2_ApproxLength(f32* v) {
    f32 result;
    f32 x = v[0];
    f32 y = v[1];
    f32 tmp;
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
        result = x + y;
        tmp = x - y;
        result = 32.0f * y + (x + (2.0f * result + (64.0f * result + 8.0f * result)));
        y = tmp;
        x = (1.0f / 128.0f) * result;
    }
    return x + 0.25f * y - 0.0078125f * y;
}

// The same estimate for the 3D vector v: the length of (v[2], the length of (v[0], v[1])).
f32 audvec3_ApproxLength(f32* v) {
    f32 tmpv[2];
    f32 tmp;

    tmp = audvec2_ApproxLength(v);
    audvec2_Set(tmpv, v[2], tmp);
    return audvec2_ApproxLength(tmpv);
}

static void audfrac_Swap(f32* pV1, f32* pV2) {
    f32 temp;

    temp = *pV2;
    *pV2 = *pV1;
    *pV1 = temp;
}

static void audvec2_Set(f32* dest, f32 x, f32 y) {
    dest[0] = x;
    dest[1] = y;
}
