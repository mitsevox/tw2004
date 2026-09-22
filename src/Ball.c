// Ball.c: the ball's flight and roll. No assert names this file; "Ball.c" is our name (the
// leaked list's PsBallFx.c is elsewhere). CodeWarrior GC/2.5, -O4,p. Units are yards and
// seconds; every constant below is a whole number of inches (1/36 yd). The cup is real geometry (surface kinds 12/18, type 90): the ball is holed when it
// has dropped below the pin height. What is written up in docs/gameplay.md is the near-cup
// pull below.

#include "golfer.h"

#define BALL_RADIUS 0.0256667f      // 0.92 in (a real one is 0.84)
#define CUP_DIAMETER 0.10717f       // 3.86 in (a real cup is 4.25)

typedef struct Ball {
    f32  vPos[3];               // 0x00
    u8   unkC[4];
    f32  vPrev[3];              // 0x10  position last step
    u8   unk1C[4];
    f32  vVel[3];               // 0x20
    u8   unk2C[4];
    f32  vSpin[3];              // 0x30
    u8   unk3C[4];
    f32  vStart[3];             // 0x40  where the shot started
    u8   unk4C[4];
    f32  fSpeed;                // 0x50
    u8   unk54[0x74 - 0x54];
    s32  nSurface;              // 0x74  surface type under the ball (90 = the cup)
    u8   unk78[4];
    CourseInfo* pCourse;        // 0x7C
    u8   unk80[0x94 - 0x80];
    s32  nPlayer;               // 0x94  -1 when nobody's
    u8   bHoled;                // 0x98
    u8   unk99[0x9C - 0x99];
    u8   unk9C;                 // 0x9C
} Ball;

void   Vec3Copy(f32* pSrc, f32* pDst);           // 0x80008304
f32    Vec_Distance(f32* pA, f32* pB);           // 0x800BB050
f32    fn_8000AD78(f32 y, f32 x);                // atan2f
f32    fn_8000AD9C(f32 x);                       // fabsf
void   Ball_Stop(Ball* pBall);                   // 0x80054340

#define PIN(pBall) ((f32*)&(pBall)->pCourse->pin[Game_CurrentHole()])

// The ball dropped in: park it in the cup and stop it.
void Ball_Holed(Ball* pBall) {
    Vec3Copy(PIN(pBall), pBall->vPos);
    pBall->vPos[0] += 0.0138889f;
    pBall->vPos[1] -= 0.0833333f;
    pBall->vPos[2] += 0.0138889f;
    pBall->bHoled = 1;
    Ball_Stop(pBall);
}

// Distance from a point to the pin, 1000 when there is no course.
f32 Ball_DistanceToPin(f32* pPos) {
    CourseInfo* pCourse = fn_8000C594();
    if (pCourse != NULL) {
        return Vec_Distance(pPos, (f32*)&pCourse->pin[Game_CurrentHole()]);
    }
    return 1000.0f;
}

// The pull toward the cup. Inside 5.5 in of the pin, while the ball is still short of it, a
// ball heading within 30 degrees of the cup (or within 3.5 in whatever its heading) gets
// 0.455 x dt x (pin - ball) added to its velocity - but never on an axis where that would speed
// it up while it is more than 16.8 degrees off line. A ball crossing over the cup fast and
// off line loses up to 67% of its speed instead: the lip.
void Ball_CupPull(Ball* pBall, f32 fDt) {
    f32 vPin[3];
    f32 fDist;

    fDist = Vec_Distance(PIN(pBall), (f32*)pBall);
    if (fDist >= 0.152778f) return;
    Vec_Copy(PIN(pBall), vPin);
    vPin[1] += BALL_RADIUS;
    {
        f32 fStartDist = Vec_Distance(pBall->vStart, vPin);
        f32 fAngle, fK, fPull;
        if (fStartDist < 0.166667f) return;
        if (Vec_Distance(pBall->vStart, (f32*)pBall) > fStartDist - 0.0416667f) return;

        fAngle = fn_8000AD78(pBall->vPos[0] - pBall->vPrev[0], pBall->vPos[2] - pBall->vPrev[2]);
        fAngle = fn_8000AD9C(fAngle - fn_8000AD78(vPin[0] - pBall->vPos[0], vPin[2] - pBall->vPos[2]));
        while (fAngle > 3.14159265f) {
            fAngle -= 3.14159265f;
        }
        if (fDist < 0.0625f) {
            if (fAngle > 0.523599f && pBall->fSpeed > 0.366667f) {
                f32 fSlow = 1.0f - 16.0f * (0.67f * fDist);
                pBall->vVel[0] *= fSlow;
                pBall->vVel[2] *= fSlow;
            }
            return;
        }
        if (fAngle >= 0.523599f && fDist >= 0.0972222f) return;

        fK    = 0.455472f * fDt;
        fPull = fK * (vPin[0] - pBall->vPos[0]);
        if ((pBall->vVel[0] < 0.0f && fPull < 0.0f) || (pBall->vVel[0] > 0.0f && fPull > 0.0f)) {
            if (fn_8000AD9C(fAngle) > 0.293333f) fPull = 0.0f;
        }
        pBall->vVel[0] += fPull;
        fPull = fK * (vPin[2] - pBall->vPos[2]);
        if ((pBall->vVel[2] < 0.0f && fPull < 0.0f) || (pBall->vVel[2] > 0.0f && fPull > 0.0f)) {
            if (fn_8000AD9C(fAngle) > 0.293333f) fPull = 0.0f;
        }
        pBall->vVel[2] += fPull;
    }
}
