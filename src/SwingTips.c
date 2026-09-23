#include "game_types.h"

s32 fn_80035574();
s32 fn_800D1AE0(void);
s32 fn_800D1D30(void);

s32 fn_800D1AE0(void) {
    s32 t0;
    t0 = fn_80035574();
    return ((u32)((-(t0 & 0xFF)) | (t0 & 0xFF)) >> 31);
}

s32 fn_800D1D30(void) {
    return 0;
}
