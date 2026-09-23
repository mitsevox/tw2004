// Small functions found by the sweep (sweep.py). Original file and meanings unknown.

#include "game_types.h"

double cos();
double acos();
double asin();
double sin();

f32 fn_800095F0(f32 x0);
f32 fn_80009614(f32 x0);
f32 fn_80009638(f32 x0);
f32 fn_8000965C(f32 x0);
f32 fn_800095F0(f32 x0) {
    f32 t0;
    t0 = sin(x0);
    return t0;
}

f32 fn_80009614(f32 x0) {
    f32 t0;
    t0 = acos(x0);
    return t0;
}

f32 fn_80009638(f32 x0) {
    f32 t0;
    t0 = cos(x0);
    return t0;
}

f32 fn_8000965C(f32 x0) {
    f32 t0;
    t0 = asin(x0);
    return t0;
}
