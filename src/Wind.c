// Wind.c (EA's name: TW07's Golf/Physics/Wind.c holds the same functions in this order, from
// Wind_vInitModule and Wind_vCloseModule (both empty; gomainloop calls fn_80055F14 and fn_80055F18
// at module init and close) to Wind_GetDirection; TW06 has golf/physics/wind.c): split off Ball.c
// at 0x80055F14. Its .sbss starts on the 8-aligned address after Ball.c's padding at
// 0x80281DE5..0x80281DE8 and its .sdata2 after the padding at 0x80283584..0x80283588.

#include "ball.h"
#include "game.h"
#include "golfer.h"
#include "engine.h"

int    fn_801021FC(void);
void   fn_800348DC(void);

f32 gWindDirs[8][4] = {                          // 0x80187EF8  unit vectors, 45 degrees apart
    {0.0f, 0.0f, 1.0f, 0.0f},
    {-0.7071067f, 0.0f, 0.7071067f, 0.0f},
    {-1.0f, 0.0f, 0.0f, 0.0f},
    {-0.7071067f, 0.0f, -0.7071067f, 0.0f},
    {0.0f, 0.0f, -1.0f, 0.0f},
    {0.7071067f, 0.0f, -0.7071067f, 0.0f},
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.7071067f, 0.0f, 0.7071067f, 0.0f},
};

// Defined here, last address first (CodeWarrior lays out .sbss in reverse).
s32 gWindDir;                             // 0x80281DEC  0..7
f32 gWindSpeed;                           // 0x80281DE8

void fn_80055F14(void) {
}

void fn_80055F18(void) {
}

// The wind: its speed, and (when asked) its vector - the direction's unit vector x speed.
f32 Wind_Get(f32* pOut) {
    if (pOut != NULL) {
        f32 v[4];
        fn_8000AE28(gWindSpeed, gWindDirs[gWindDir], v);
        Vec_Copy(v, pOut);
    }
    return gWindSpeed;
}

int fn_80055F78(void) {
    return gWindDir;
}

f32 fn_80055F80(void) {
    return gWindSpeed;
}

// Set the wind (a speed of 0 or less is stored as 0.1) and tell fn_800348DC.
void Wind_Set(int nDir, f32 fSpeed) {
    gWindDir   = nDir;
    gWindSpeed = fSpeed;
    if (fSpeed <= 0.0f) {
        gWindSpeed = 0.1f;
    }
    fn_800348DC();
}

// The hole's wind: the authored one, none with the wind off, or - when the hole has none - one
// rolled from the wind setting (courses 6 and 15 at least 2, except in mode 4 without a
// challenge): speed 0..6, 2..12, 5..20 or 12..31, direction one of eight.
void Wind_Generate(void) {
    int n      = fn_800D2CB0_HoleWindDir();
    f32 fSpeed = fn_800D2CF8_HoleWindSpeed();
    if (gpGame->bNoWind) {
        fSpeed = 0.0f;
        n      = 0;
    } else if (n == 0 && 0.0f == fSpeed) {
        n = gSession.options.nWind;
        switch (Game_GetCourse()) {
        case 6:
        case 15:
            if (Game_GetMode() != 4 || fn_801021FC() > 0) {
                if (n < 2) {
                    n = 2;
                } else {
                    n = 3;
                }
            }
            break;
        }
        switch (n) {
        case 0:
            fSpeed = Misc_RandFunc(0) % 7;
            break;
        case 1:
            fSpeed = 2.0f + Misc_RandFunc(0) % 11;
            break;
        case 2:
            fSpeed = 5.0f + Misc_RandFunc(0) % 16;
            break;
        case 3:
            fSpeed = 12.0f + Misc_RandFunc(0) % 20;
            break;
        }
        n = Misc_RandFunc(0) % 8;
    }
    Wind_Set(n, fSpeed);
}

// The wind's direction as a unit vector.
void fn_800561CC(f32* pOut) {
    Vec_Copy(gWindDirs[gWindDir], pOut);
}
