// Wind.c (EA's name: TW07's Golf/Physics/Wind.c holds the same functions in this order, from
// Wind_vInitModule and Wind_vCloseModule (both empty; gomainloop calls Wind_vInitModule and Wind_vCloseModule
// at module init and close) to Wind_GetDirection; TW06 has golf/physics/wind.c): split off Ball.c
// at 0x80055F14. Its .sbss starts on the 8-aligned address after Ball.c's padding at
// 0x80281DE5..0x80281DE8 and its .sdata2 after the padding at 0x80283584..0x80283588.

#include "ball.h"
#include "game.h"
#include "golfer.h"
#include "engine.h"

int    GameMode4_GetCurrentEvent(void);
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

// Wind module init, called by the main loop's module init: empty in this build.
void Wind_vInitModule(void) {
}

// Wind module close, called by the main loop's module close: empty in this build.
void Wind_vCloseModule(void) {
}

// The wind's speed, and (when pOut is not NULL) its velocity: the direction's unit vector times the
// speed.
f32 Wind_GetPhysicsWindVelocity(f32* pOut) {
    if (pOut != NULL) {
        f32 v[4];
        LLMath_Scale(gWindSpeed, gWindDirs[gWindDir], v);
        LLMath_CopyVec(v, pOut);
    }
    return gWindSpeed;
}

int Wind_GetPhysicsDirection(void) {
    return gWindDir;
}

f32 Wind_GetPhysicsSpeed(void) {
    return gWindSpeed;
}

// Sets the wind: direction nDir (0..7, gWindDirs) and speed fSpeed (0 or less is stored as 0.1),
// then turns the course's flag to it (fn_800348DC).
void Wind_SetPhysicsWind(int nDir, f32 fSpeed) {
    gWindDir   = nDir;
    gWindSpeed = fSpeed;
    if (fSpeed <= 0.0f) {
        gWindSpeed = 0.1f;
    }
    fn_800348DC();
}

// The hole's wind: the one authored in the course table; none when the wind is off; or, when the
// hole has none authored (direction 0 and speed 0), one rolled from the wind setting
// (gSession.options.nWind 0..3). On Royal Birkdale and St Andrews (courses 6 and 15) the setting is
// raised to 2 (from 0 or 1) or 3 (from 2 or 3), except on the ladder's (mode 4) first event
// (GameMode4_GetCurrentEvent 0). Speed by setting: 0..6, 2..12, 5..20 or 12..31; direction one of
// eight at random.
void Wind_InitForHole(void) {
    int n      = GM_GetCurrentHolePrevailingWindDir();
    f32 fSpeed = GM_GetCurrentHolePrevailingWindSpeed();
    if (gpGame->bNoWind) {
        fSpeed = 0.0f;
        n      = 0;
    } else if (n == 0 && 0.0f == fSpeed) {
        n = gSession.options.nWind;
        switch (Game_GetCourse()) {
        case 6:
        case 15:
            if (Game_GetMode() != 4 || GameMode4_GetCurrentEvent() > 0) {
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
    Wind_SetPhysicsWind(n, fSpeed);
}

// The wind's direction as a unit vector.
void Wind_GetDirection(f32* pOut) {
    LLMath_CopyVec(gWindDirs[gWindDir], pOut);
}
