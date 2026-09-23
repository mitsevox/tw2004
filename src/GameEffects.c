// GameEffects.c (TW06's name, GameEffects_*): slow motion, the letterbox, the time rate and the
// other shot effects. TW06's copy of the file keeps the same function order.

#include "golfer.h"

extern u8 lbl_80202898[];                   // the effects state

// TW06: GameEffects_ResetGameEffectTimeSettings (by position and size).
u8* fn_800DAF74(void) {
    lbl_80202898[0x10] = 0;
    lbl_80202898[0x11] = 0;
    lbl_80202898[0] = 0;
    lbl_80202898[9] = 0;
    *(s32*)(lbl_80202898 + 0x28) = 0;
    return lbl_80202898;
}

f32 fn_800DC45C(void) {
    return 0.0f;
}

s32 fn_800DC514(void) {
    return lbl_80202898[0];
}

// The letterbox grows to 15% of the screen over its first 0.8 seconds.
f32 GameEffects_GetLetterboxHeight(void) {
    if (lbl_80202898[0x12] != 0) {
        if (*(f32*)(lbl_80202898 + 0x1C) < 0.8f) {
            return 0.15f * (*(f32*)(lbl_80202898 + 0x1C) / 0.8f);
        }
        return 0.15f;
    }
    return 0.0f;
}
