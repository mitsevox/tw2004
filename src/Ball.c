// Ball.c (our name): the ball's flight and roll. No assert names this file (the leaked list's
// PsBallFx.c is elsewhere). Units are yards and seconds. Many constants are inches or miles per
// hour converted to yards (INCHES, MPH): the original's float bits are exactly those quotients,
// not the rounded decimals. The cup is real geometry (surface classes 12/18, surface type 90): the
// ball is holed when it has dropped below the pin height. What is written up in docs/gameplay.md
// is the near-cup pull below.

#include "golfer.h"
#include "physics.h"
#include "ball.h"
#include "game.h"
#include "engine.h"

#define INCHES(x) ((x) / 36.0f)                  // inches to yards
#define MPH(x)    ((x) * (1760.0f / 3600.0f))    // miles per hour to yards per second

#define BALL_RADIUS  INCHES(0.924f)   // 1.1 x a real ball's 0.84 in
#define CUP_DIAMETER 0.10717f         // 3.86 in (a real cup is 4.25)

void   Ball_Stop(Ball* pBall);                   // 0x80054340
void   Ball_SetLie(Ball* pBall, SurfaceType* pSurface);
void   Ball_Tick(Ball* pBall, f32 fTicks);
void   Physics_FixBallHeight(Ball* pBall, u8 bSettle, f32 fTicks);
u8     fn_80050DE4(int nKind, int nClub, int a, const ClubRow** ppRow, s32* pSurface);
u8     Physics_GetShotData(Ball* pBall, int nClub, int nKind, f32 fPower, f32 fAim, int nTrajectory, f32* pA,
                           f32* pB, f32* pVel, f32* pSpin);

// ---- the club and shot tables (0x80181328 - 0x801834C0, in the original's order) -------------
// Shot kinds are ShotType_t (0 putt, 1 full swing, 2 chip, 3 pitch, 4 punch, 5 flop; 6 and 7 are
// unnamed); clubs are Club_t (0..24 the bag, 25 the putter). Speeds are launch speeds before
// Physics_GetShotData's scaling, angles are in degrees (the values are exact as DEG(x)).

// Per shot kind: launch speed x, launch angle +, spin x.
const f32 gKindSpeed[8] = {
    1.0f, 1.148f, 0.5f, 0.58f, 1.15f, 0.4f, 0.85f, 0.4f
};
const f32 gKindLoft[8] = {
    0.0f, 0.0f, DEG(-4.0f), DEG(10.0f), DEG(-6.0f), DEG(17.0f), DEG(4.0f), DEG(26.0f)
};
const f32 gKindSpin[8] = {
    1.0f, 1.0f, 1.0f, 1.0f, 1.5f, 0.01f, 1.0f, 1.0f
};

// The trajectory setting (low, normal, high) added to the launch angle.
const f32 gTrajLoft[3] = {
    DEG(-5.0f), 0.0f, DEG(5.0f)
};

// Per club: launch speed.
const f32 gClubSpeed[CLUB_MAX_e] = {
    0.5049f, 0.4977f, 0.4927f, 0.4902f, 0.4873f, 0.4845f, 0.449f, 0.415f, 0.392f,  // drivers 1-6, 3/5/7 wood
    0.37f, 0.356f, 0.342f, 0.329f, 0.319f, 0.312f, 0.308f, 0.303f, 0.301f,         // irons 1-9
    0.311f, 0.3343f, 0.3297f, 0.3284f, 0.324f, 0.3302f, 0.337f,                    // PW AW GW SW LBW LW HLW
    0.0471f                                                                        // putter
};

// Per club: launch angle (the loft).
const f32 gClubLoft[CLUB_MAX_e] = {
    DEG(6.0f),    // driver 1
    DEG(7.0f),    // driver 2
    DEG(8.0f),    // driver 3
    DEG(9.0f),    // driver 4
    DEG(9.5f),    // driver 5
    DEG(10.0f),   // driver 6
    DEG(9.75f),   // 3 wood
    DEG(11.5f),   // 5 wood
    DEG(15.0f),   // 7 wood
    DEG(16.25f),  // 1 iron
    DEG(17.84f),  // 2 iron
    DEG(19.46f),  // 3 iron
    DEG(21.3f),   // 4 iron
    DEG(23.3f),   // 5 iron
    DEG(25.9f),   // 6 iron
    DEG(28.7f),   // 7 iron
    DEG(31.75f),  // 8 iron
    DEG(34.75f),  // 9 iron
    DEG(39.25f),  // pitching wedge
    DEG(44.75f),  // approach wedge
    DEG(48.5f),   // gap wedge
    DEG(52.0f),   // sand wedge
    DEG(54.0f),   // low-bounce wedge
    DEG(57.0f),   // lob wedge
    DEG(60.0f),   // high lob wedge
    0.0f          // putter
};

// Per club: its step down the bag (a punch loses 0.01 speed and 0.8 degrees of loft per step; a
// lie's lost speed comes back 1.25% per step).
const s32 gClubStep[CLUB_MAX_e] = {
    0, 0, 0, 0, 0, 0, 1, 2, 3,     // drivers 1-6, 3/5/7 wood
    4, 5, 6, 7, 8, 9, 10, 11, 12,  // irons 1-9
    13, 13, 13, 14, 14, 15, 15,    // PW AW GW SW LBW LW HLW
    0                              // putter
};

// Per club: launch angle and speed for a chip (5 iron and up only).
const f32 gChipLoft[CLUB_MAX_e] = {
    0.0f,         // driver 1
    0.0f,         // driver 2
    0.0f,         // driver 3
    0.0f,         // driver 4
    0.0f,         // driver 5
    0.0f,         // driver 6
    0.0f,         // 3 wood
    0.0f,         // 5 wood
    0.0f,         // 7 wood
    0.0f,         // 1 iron
    0.0f,         // 2 iron
    0.0f,         // 3 iron
    0.0f,         // 4 iron
    DEG(28.91f),  // 5 iron
    DEG(31.83f),  // 6 iron
    DEG(34.76f),  // 7 iron
    DEG(38.42f),  // 8 iron
    DEG(43.18f),  // 9 iron
    DEG(45.99f),  // pitching wedge
    DEG(46.16f),  // approach wedge
    DEG(46.22f),  // gap wedge
    DEG(50.34f),  // sand wedge
    DEG(50.34f),  // low-bounce wedge
    DEG(50.51f),  // lob wedge
    DEG(50.57f),  // high lob wedge
    0.0f          // putter
};
const f32 gChipSpeed[CLUB_MAX_e] = {
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,            // drivers 1-6, 3/5/7 wood
    0.0f, 0.0f, 0.0f, 0.0f, 0.169f, 0.175f, 0.182f, 0.193f, 0.211f,  // irons 1-9
    0.224f, 0.225f, 0.225f, 0.247f, 0.247f, 0.248f, 0.248f,          // PW AW GW SW LBW LW HLW
    0.0f                                                             // putter
};

// Per club: spin.
const f32 gClubSpin[CLUB_MAX_e] = {
    0.87f, 0.87f, 0.87f, 0.87f, 0.87f, 0.87f, 0.85f, 0.825f, 0.8f,  // drivers 1-6, 3/5/7 wood
    0.84f, 0.76f, 0.69f, 0.63f, 0.58f, 0.54f, 0.5f, 0.46f, 0.43f,   // irons 1-9
    0.36f, 0.31f, 0.25f, 0.21f, 0.2f, 0.15f, 0.11f,                 // PW AW GW SW LBW LW HLW
    0.0f                                                            // putter
};

// Putt distance (yards, medium green) at power 0, 0.05 .. 1.1, and its scale by gTurfSpeed.
const f32 gPuttDist[23] = {
    0.0f, 0.11f, 0.43f, 0.97f, 1.73f, 2.71f, 3.91f, 5.32f, 6.96f, 8.81f, 10.88f, 13.09f,
    15.59f, 18.3f, 21.24f, 24.39f, 27.77f, 31.35f, 35.16f, 39.19f, 43.43f, 47.75f, 52.42f
};
const f32 gPuttSpeedScale[5] = {
    0.606f, 0.65f, 1.0f, 1.3f, 1.82f
};

// Carry by power (ClubRow), one table per shot kind 1..7 (fn_80050DE4).
// Kind 1, the full swing.
const ClubRow gClubRows1[25] = {
    {{15.1f, 46.1f, 78.9f, 112.2f, 147.3f, 184.1f, 220.9f, 254.3f, 283.7f, 309.7f, 333.5f}},  // driver 1
    {{14.8f, 45.6f, 78.7f, 112.7f, 148.6f, 186.0f, 221.8f, 254.6f, 282.7f, 307.8f, 331.4f}},  // driver 2
    {{14.6f, 45.3f, 78.7f, 113.1f, 149.7f, 187.5f, 222.8f, 254.0f, 281.1f, 305.9f, 329.5f}},  // driver 3
    {{14.4f, 45.3f, 79.0f, 114.3f, 151.6f, 189.3f, 223.5f, 253.5f, 279.6f, 304.1f, 327.7f}},  // driver 4
    {{14.3f, 45.0f, 78.7f, 114.3f, 151.6f, 188.8f, 222.7f, 251.9f, 277.9f, 302.1f, 325.8f}},  // driver 5
    {{14.2f, 44.6f, 78.4f, 114.2f, 151.4f, 188.5f, 221.8f, 250.3f, 276.0f, 299.8f, 324.3f}},  // driver 6
    {{12.4f, 39.8f, 70.7f, 103.2f, 137.0f, 171.9f, 204.8f, 233.9f, 259.7f, 282.9f, 305.0f}},  // 3 wood
    {{10.7f, 35.5f, 63.9f, 94.6f, 126.3f, 158.5f, 189.3f, 216.9f, 241.1f, 263.2f, 283.9f}},  // 5 wood
    {{9.6f, 32.1f, 59.4f, 88.6f, 119.0f, 149.0f, 177.0f, 201.7f, 223.4f, 243.7f, 263.2f}},  // 7 wood
    {{8.5f, 29.0f, 54.5f, 81.9f, 110.4f, 138.9f, 165.4f, 188.8f, 209.3f, 228.5f, 246.3f}},  // 1 iron
    {{7.9f, 26.9f, 51.2f, 77.2f, 104.4f, 131.4f, 157.0f, 179.7f, 200.1f, 218.6f, 236.1f}},  // 2 iron
    {{7.3f, 24.7f, 47.6f, 72.4f, 98.2f, 123.8f, 148.3f, 170.4f, 190.2f, 208.4f, 225.6f}},  // 3 iron
    {{6.7f, 22.6f, 44.2f, 67.5f, 92.0f, 116.3f, 139.6f, 161.0f, 180.2f, 198.1f, 214.9f}},  // 4 iron
    {{6.1f, 20.8f, 41.0f, 63.3f, 86.5f, 109.7f, 131.9f, 152.5f, 171.3f, 188.6f, 204.6f}},  // 5 iron
    {{5.7f, 19.2f, 38.1f, 59.2f, 81.2f, 103.1f, 124.2f, 144.0f, 161.9f, 178.8f, 193.9f}},  // 6 iron
    {{5.2f, 17.8f, 35.4f, 55.5f, 76.4f, 97.3f, 117.2f, 135.8f, 153.3f, 169.5f, 183.8f}},  // 7 iron
    {{4.8f, 16.2f, 32.2f, 50.9f, 70.4f, 90.0f, 108.9f, 126.6f, 143.3f, 158.4f, 172.0f}},  // 8 iron
    {{4.3f, 14.6f, 29.5f, 46.9f, 65.2f, 83.7f, 101.3f, 118.3f, 134.4f, 148.7f, 161.5f}},  // 9 iron
    {{4.0f, 13.5f, 27.3f, 43.4f, 60.7f, 78.1f, 95.0f, 111.4f, 126.4f, 140.0f, 151.8f}},  // pitching wedge
    {{3.6f, 12.5f, 25.4f, 40.5f, 56.6f, 72.9f, 89.2f, 104.1f, 117.9f, 130.1f, 139.8f}},  // approach wedge
    {{2.9f, 10.4f, 21.3f, 34.3f, 48.6f, 63.3f, 78.5f, 92.9f, 106.9f, 120.0f, 131.6f}},  // gap wedge
    {{2.4f, 8.7f, 17.7f, 29.3f, 41.8f, 55.2f, 69.3f, 83.0f, 96.5f, 109.9f, 122.4f}},  // sand wedge
    {{2.1f, 7.6f, 15.6f, 25.9f, 37.2f, 49.5f, 62.0f, 75.1f, 87.6f, 100.1f, 111.8f}},  // low-bounce wedge
    {{1.8f, 6.5f, 13.6f, 22.7f, 32.9f, 43.8f, 55.3f, 67.1f, 78.8f, 90.0f, 100.9f}},  // lob wedge
    {{1.5f, 5.6f, 11.7f, 19.3f, 28.5f, 38.4f, 48.6f, 59.1f, 69.9f, 80.1f, 90.0f}}    // high lob wedge
};
// Kind 2, the chip: 5 iron and up (the rows before are -0.0 in the original).
const ClubRow gClubRows2[25] = {
    {{-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f}},  // driver 1
    {{-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f}},  // driver 2
    {{-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f}},  // driver 3
    {{-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f}},  // driver 4
    {{-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f}},  // driver 5
    {{-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f}},  // driver 6
    {{-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f}},  // 3 wood
    {{-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f}},  // 5 wood
    {{-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f}},  // 7 wood
    {{-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f}},  // 1 iron
    {{-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f}},  // 2 iron
    {{-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f}},  // 3 iron
    {{-0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f, -0.0f}},  // 4 iron
    {{0.8f, 3.1f, 6.2f, 8.5f, 10.9f, 14.2f, 17.3f, 21.1f, 25.3f, 29.7f, 34.3f}},  // 5 iron
    {{0.8f, 3.0f, 6.0f, 8.4f, 11.1f, 14.1f, 17.6f, 21.3f, 25.4f, 29.7f, 34.4f}},  // 6 iron
    {{0.8f, 2.9f, 6.3f, 8.7f, 11.0f, 14.3f, 17.3f, 21.5f, 25.5f, 29.8f, 34.3f}},  // 7 iron
    {{0.7f, 2.8f, 6.0f, 8.7f, 11.1f, 13.9f, 17.6f, 21.3f, 25.7f, 29.8f, 34.2f}},  // 8 iron
    {{0.7f, 2.7f, 5.7f, 8.4f, 11.1f, 14.1f, 17.6f, 21.7f, 25.6f, 29.8f, 34.3f}},  // 9 iron
    {{0.7f, 2.6f, 5.5f, 8.6f, 11.3f, 14.2f, 17.7f, 21.4f, 25.8f, 29.7f, 34.5f}},  // pitching wedge
    {{0.7f, 2.6f, 5.5f, 8.6f, 11.2f, 14.1f, 17.7f, 21.5f, 25.8f, 29.8f, 34.5f}},  // approach wedge
    {{0.7f, 2.6f, 5.5f, 8.5f, 11.3f, 14.1f, 17.7f, 21.4f, 25.8f, 29.7f, 34.5f}},  // gap wedge
    {{0.7f, 2.5f, 5.2f, 7.9f, 11.2f, 14.4f, 17.4f, 21.4f, 25.4f, 29.8f, 34.0f}},  // sand wedge
    {{0.7f, 2.5f, 5.2f, 8.0f, 11.2f, 14.4f, 17.4f, 21.4f, 25.4f, 29.8f, 34.0f}},  // low-bounce wedge
    {{0.6f, 2.4f, 5.1f, 7.9f, 11.2f, 14.4f, 17.4f, 21.4f, 25.4f, 29.9f, 34.0f}},  // lob wedge
    {{0.6f, 2.4f, 5.1f, 7.9f, 11.1f, 14.4f, 17.4f, 21.4f, 25.3f, 29.8f, 34.1f}}   // high lob wedge
};
// Kind 3, the pitch.
const ClubRow gClubRows3[25] = {
    {{4.5f, 15.2f, 30.7f, 48.2f, 66.6f, 85.9f, 105.6f, 125.4f, 144.8f, 163.0f, 179.7f}},  // driver 1
    {{4.3f, 14.7f, 29.7f, 47.1f, 65.1f, 84.0f, 103.4f, 122.7f, 141.6f, 159.3f, 175.5f}},  // driver 2
    {{4.3f, 14.4f, 29.0f, 45.9f, 63.9f, 82.5f, 101.6f, 120.6f, 139.0f, 156.2f, 171.9f}},  // driver 3
    {{4.2f, 14.1f, 28.6f, 45.3f, 63.1f, 81.5f, 100.5f, 119.1f, 137.2f, 153.9f, 169.1f}},  // driver 4
    {{4.1f, 13.9f, 28.1f, 44.7f, 62.4f, 80.6f, 99.3f, 117.7f, 135.6f, 152.1f, 167.1f}},  // driver 5
    {{4.0f, 13.5f, 27.7f, 44.1f, 61.6f, 79.7f, 98.2f, 116.5f, 133.9f, 150.2f, 164.9f}},  // driver 6
    {{3.6f, 11.9f, 24.4f, 39.3f, 55.3f, 71.8f, 88.8f, 106.0f, 122.9f, 138.8f, 153.8f}},  // 3 wood
    {{3.1f, 10.1f, 20.8f, 34.0f, 48.3f, 63.2f, 78.6f, 94.2f, 109.7f, 124.5f, 138.6f}},  // 5 wood
    {{2.6f, 8.7f, 17.9f, 29.3f, 42.2f, 55.7f, 69.8f, 83.8f, 97.8f, 111.4f, 124.1f}},  // 7 wood
    {{2.2f, 7.7f, 15.9f, 26.1f, 37.7f, 50.2f, 63.0f, 76.0f, 89.1f, 101.6f, 113.7f}},  // 1 iron
    {{2.0f, 7.0f, 14.2f, 23.6f, 34.4f, 46.1f, 58.1f, 70.3f, 82.6f, 94.5f, 106.2f}},  // 2 iron
    {{1.8f, 6.3f, 12.8f, 21.3f, 31.2f, 41.8f, 53.2f, 64.6f, 76.0f, 87.4f, 98.6f}},  // 3 iron
    {{1.6f, 5.7f, 11.5f, 19.2f, 28.0f, 37.8f, 48.2f, 58.8f, 69.5f, 80.3f, 90.9f}},  // 4 iron
    {{1.4f, 5.1f, 10.3f, 17.3f, 25.3f, 34.3f, 43.8f, 53.7f, 63.8f, 73.9f, 83.8f}},  // 5 iron
    {{1.2f, 4.6f, 9.2f, 15.3f, 22.7f, 30.8f, 39.5f, 48.7f, 58.0f, 67.3f, 76.5f}},  // 6 iron
    {{1.1f, 4.1f, 8.4f, 13.7f, 20.5f, 27.8f, 35.7f, 44.0f, 52.6f, 61.3f, 69.8f}},  // 7 iron
    {{0.9f, 3.5f, 7.3f, 12.1f, 17.9f, 24.5f, 31.5f, 39.0f, 46.8f, 54.6f, 62.6f}},  // 8 iron
    {{0.8f, 3.0f, 6.4f, 10.6f, 15.6f, 21.7f, 28.0f, 34.7f, 41.7f, 48.9f, 56.1f}},  // 9 iron
    {{1.0f, 3.8f, 8.1f, 13.5f, 20.0f, 27.2f, 34.8f, 42.8f, 51.0f, 59.2f, 67.5f}},  // pitching wedge
    {{0.7f, 2.6f, 5.6f, 9.5f, 14.2f, 19.3f, 25.4f, 31.5f, 37.9f, 44.4f, 50.9f}},  // approach wedge
    {{0.5f, 2.1f, 4.4f, 7.5f, 11.3f, 15.5f, 20.2f, 25.7f, 31.2f, 36.9f, 42.6f}},  // gap wedge
    {{0.4f, 1.6f, 3.4f, 5.8f, 8.8f, 12.3f, 16.1f, 20.2f, 25.1f, 29.9f, 34.9f}},  // sand wedge
    {{0.2f, 0.9f, 2.0f, 3.4f, 5.1f, 7.2f, 9.5f, 12.1f, 15.0f, 18.0f, 21.2f}},  // low-bounce wedge
    {{0.2f, 0.7f, 1.5f, 2.6f, 4.0f, 5.7f, 7.6f, 9.6f, 11.9f, 14.3f, 17.1f}},  // lob wedge
    {{0.1f, 0.5f, 1.2f, 2.1f, 3.1f, 4.4f, 5.9f, 7.5f, 9.3f, 11.3f, 13.3f}}  // high lob wedge
};
// Kind 4, the punch.
const ClubRow gClubRows4[25] = {
    {{14.4f, 42.7f, 70.3f, 94.6f, 116.7f, 136.8f, 155.7f, 173.8f, 192.0f, 210.1f, 230.4f}},  // driver 1
    {{14.2f, 42.5f, 70.7f, 96.1f, 120.2f, 144.2f, 169.4f, 197.8f, 228.2f, 258.6f, 286.6f}},  // driver 2
    {{14.1f, 42.5f, 71.5f, 98.4f, 124.8f, 152.3f, 182.8f, 215.7f, 248.0f, 278.0f, 304.8f}},  // driver 3
    {{14.1f, 42.8f, 72.5f, 100.9f, 129.9f, 161.2f, 195.4f, 230.3f, 262.5f, 291.6f, 317.2f}},  // driver 4
    {{14.0f, 42.8f, 72.7f, 101.8f, 131.8f, 164.4f, 199.7f, 235.1f, 266.9f, 295.2f, 320.7f}},  // driver 5
    {{14.0f, 42.7f, 72.7f, 102.7f, 133.6f, 167.5f, 203.7f, 238.5f, 270.3f, 297.9f, 322.6f}},  // driver 6
    {{11.6f, 36.7f, 63.1f, 88.9f, 114.3f, 140.7f, 169.2f, 200.0f, 230.8f, 260.0f, 286.4f}},  // 3 wood
    {{9.6f, 31.7f, 56.0f, 80.5f, 104.5f, 129.2f, 155.9f, 184.5f, 213.5f, 241.4f, 267.2f}},  // 5 wood
    {{8.4f, 28.2f, 51.3f, 75.6f, 100.2f, 126.2f, 153.5f, 182.0f, 209.7f, 235.0f, 257.6f}},  // 7 wood
    {{7.1f, 24.3f, 45.4f, 67.4f, 89.9f, 113.3f, 138.2f, 164.3f, 190.5f, 214.9f, 237.2f}},  // 1 iron
    {{6.3f, 21.4f, 40.9f, 61.4f, 82.4f, 104.0f, 126.8f, 150.8f, 174.9f, 198.5f, 220.4f}},  // 2 iron
    {{5.5f, 18.6f, 36.3f, 55.0f, 74.6f, 94.8f, 115.6f, 137.1f, 159.3f, 181.4f, 202.4f}},  // 3 iron
    {{4.7f, 16.0f, 32.0f, 49.2f, 67.0f, 85.8f, 104.7f, 124.5f, 144.7f, 165.1f, 184.8f}},  // 4 iron
    {{4.1f, 13.9f, 28.1f, 44.0f, 60.4f, 77.7f, 95.4f, 113.5f, 132.1f, 150.8f, 169.2f}},  // 5 iron
    {{3.6f, 12.0f, 24.7f, 39.5f, 54.9f, 70.7f, 87.3f, 104.2f, 121.5f, 138.8f, 156.0f}},  // 6 iron
    {{3.2f, 10.6f, 21.9f, 35.5f, 49.9f, 64.8f, 80.3f, 96.0f, 112.2f, 128.4f, 144.5f}},  // 7 iron
    {{2.8f, 9.2f, 19.0f, 31.0f, 44.2f, 58.0f, 72.1f, 86.7f, 101.5f, 116.5f, 131.1f}},  // 8 iron
    {{2.4f, 8.0f, 16.6f, 27.4f, 39.5f, 52.2f, 65.2f, 78.6f, 92.3f, 106.2f, 119.8f}},  // 9 iron
    {{2.3f, 7.7f, 15.9f, 26.2f, 37.9f, 50.4f, 63.2f, 76.3f, 89.6f, 102.9f, 116.0f}},  // pitching wedge
    {{2.5f, 8.7f, 17.8f, 29.2f, 42.0f, 55.6f, 69.5f, 83.6f, 97.5f, 111.0f, 124.3f}},  // approach wedge
    {{2.2f, 7.7f, 15.8f, 25.8f, 37.4f, 49.8f, 62.6f, 75.5f, 88.5f, 101.3f, 113.7f}},  // gap wedge
    {{1.8f, 6.5f, 13.1f, 21.9f, 31.7f, 42.6f, 54.0f, 65.7f, 77.4f, 89.0f, 100.4f}},  // sand wedge
    {{1.6f, 5.9f, 11.9f, 19.8f, 28.9f, 38.8f, 49.4f, 60.4f, 71.4f, 82.2f, 93.1f}},  // low-bounce wedge
    {{1.4f, 5.3f, 10.7f, 17.9f, 26.2f, 35.2f, 45.0f, 55.1f, 65.4f, 75.8f, 86.2f}},  // lob wedge
    {{1.3f, 5.0f, 10.3f, 16.9f, 25.2f, 34.0f, 43.4f, 53.2f, 63.3f, 73.5f, 83.6f}}  // high lob wedge
};
// Kind 5, the flop.
const ClubRow gClubRows5[25] = {
    {{1.3f, 4.6f, 9.3f, 15.5f, 22.7f, 30.8f, 39.4f, 48.1f, 56.8f, 65.7f, 74.3f}},  // driver 1
    {{1.3f, 4.6f, 9.3f, 15.6f, 22.8f, 30.9f, 39.5f, 48.3f, 57.1f, 65.9f, 74.7f}},  // driver 2
    {{1.3f, 4.7f, 9.4f, 15.7f, 22.9f, 31.0f, 39.6f, 48.5f, 57.3f, 66.1f, 74.9f}},  // driver 3
    {{1.3f, 4.7f, 9.4f, 15.7f, 23.0f, 31.1f, 39.7f, 48.5f, 57.5f, 66.4f, 75.1f}},  // driver 4
    {{1.3f, 4.7f, 9.4f, 15.8f, 23.0f, 31.2f, 39.7f, 48.5f, 57.6f, 66.4f, 75.2f}},  // driver 5
    {{1.3f, 4.7f, 9.5f, 15.8f, 23.0f, 31.2f, 39.8f, 48.6f, 57.6f, 66.5f, 75.3f}},  // driver 6
    {{1.3f, 4.7f, 9.5f, 15.8f, 23.0f, 31.2f, 39.7f, 48.6f, 57.6f, 66.5f, 75.3f}},  // 3 wood
    {{1.3f, 4.7f, 9.5f, 15.8f, 23.1f, 31.1f, 39.8f, 48.7f, 57.6f, 66.5f, 75.4f}},  // 5 wood
    {{1.2f, 4.6f, 9.5f, 15.5f, 23.0f, 30.9f, 39.5f, 48.4f, 57.3f, 66.3f, 75.2f}},  // 7 wood
    {{1.2f, 4.6f, 9.4f, 15.4f, 22.9f, 30.8f, 39.3f, 48.1f, 57.1f, 66.0f, 74.9f}},  // 1 iron
    {{1.2f, 4.5f, 9.3f, 15.3f, 22.7f, 30.6f, 39.0f, 47.7f, 56.6f, 65.5f, 74.4f}},  // 2 iron
    {{1.2f, 4.5f, 9.2f, 15.2f, 22.5f, 30.3f, 38.6f, 47.2f, 56.1f, 65.0f, 73.8f}},  // 3 iron
    {{1.2f, 4.4f, 9.1f, 14.9f, 22.1f, 29.8f, 38.1f, 46.7f, 55.4f, 64.2f, 73.1f}},  // 4 iron
    {{1.1f, 4.3f, 8.9f, 14.6f, 21.7f, 29.3f, 37.4f, 45.9f, 54.5f, 63.3f, 72.2f}},  // 5 iron
    {{1.1f, 4.1f, 8.6f, 14.3f, 21.3f, 28.9f, 37.2f, 46.0f, 55.0f, 64.4f, 74.0f}},  // 6 iron
    {{1.0f, 3.9f, 8.2f, 13.7f, 20.4f, 27.7f, 35.8f, 44.3f, 53.1f, 62.2f, 71.9f}},  // 7 iron
    {{1.0f, 3.6f, 7.6f, 12.8f, 18.9f, 26.2f, 33.9f, 42.0f, 50.6f, 59.4f, 68.6f}},  // 8 iron
    {{0.9f, 3.3f, 7.0f, 11.9f, 17.7f, 24.6f, 31.8f, 39.6f, 47.8f, 56.3f, 65.1f}},  // 9 iron
    {{0.8f, 2.9f, 6.1f, 10.4f, 15.7f, 21.5f, 28.4f, 35.5f, 43.0f, 50.9f, 59.1f}},  // pitching wedge
    {{0.6f, 2.4f, 5.0f, 8.7f, 13.0f, 18.2f, 23.9f, 30.1f, 36.7f, 43.6f, 50.6f}},  // approach wedge
    {{0.5f, 2.0f, 4.3f, 7.4f, 11.3f, 15.8f, 20.8f, 26.3f, 32.1f, 38.2f, 44.5f}},  // gap wedge
    {{0.4f, 1.7f, 3.6f, 6.3f, 9.6f, 13.5f, 17.9f, 22.6f, 27.7f, 33.2f, 38.8f}},  // sand wedge
    {{0.4f, 1.5f, 3.3f, 5.7f, 8.7f, 12.2f, 16.2f, 20.5f, 25.3f, 30.2f, 35.3f}},  // low-bounce wedge
    {{0.3f, 1.2f, 2.8f, 4.8f, 7.4f, 10.3f, 13.8f, 17.5f, 21.5f, 25.7f, 30.1f}},  // lob wedge
    {{0.3f, 1.0f, 2.3f, 3.9f, 6.1f, 8.6f, 11.4f, 14.5f, 17.9f, 21.4f, 25.0f}}  // high lob wedge
};
// Kind 6.
const ClubRow gClubRows6[25] = {
    {{3.6f, 12.1f, 24.8f, 39.3f, 54.2f, 69.8f, 85.8f, 102.1f, 118.7f, 135.8f, 153.3f}},  // driver 1
    {{3.7f, 12.1f, 24.8f, 39.3f, 54.5f, 70.1f, 86.5f, 103.1f, 120.1f, 137.4f, 154.5f}},  // driver 2
    {{3.6f, 12.0f, 24.7f, 39.4f, 54.7f, 70.5f, 86.9f, 103.9f, 121.2f, 138.3f, 155.7f}},  // driver 3
    {{3.6f, 12.0f, 24.7f, 39.4f, 54.8f, 70.7f, 87.3f, 104.3f, 121.6f, 139.1f, 156.4f}},  // driver 4
    {{3.6f, 12.0f, 24.6f, 39.4f, 54.8f, 70.8f, 87.4f, 104.5f, 121.9f, 139.3f, 156.5f}},  // driver 5
    {{3.6f, 12.0f, 24.6f, 39.3f, 54.8f, 70.9f, 87.6f, 104.7f, 122.2f, 139.6f, 156.4f}},  // driver 6
    {{3.6f, 12.0f, 24.6f, 39.4f, 54.8f, 70.9f, 87.6f, 104.6f, 122.0f, 139.4f, 156.2f}},  // 3 wood
    {{3.6f, 11.9f, 24.4f, 39.1f, 54.7f, 70.9f, 87.7f, 104.8f, 122.1f, 139.2f, 155.7f}},  // 5 wood
    {{3.5f, 11.5f, 23.7f, 38.2f, 53.9f, 70.0f, 86.7f, 103.7f, 120.4f, 136.7f, 151.8f}},  // 7 wood
    {{3.4f, 11.4f, 23.4f, 37.8f, 53.3f, 69.5f, 86.0f, 102.7f, 119.2f, 134.9f, 149.6f}},  // 1 iron
    {{3.3f, 11.2f, 23.0f, 37.1f, 52.5f, 68.5f, 84.8f, 101.3f, 117.4f, 132.7f, 147.2f}},  // 2 iron
    {{3.3f, 10.9f, 22.4f, 36.3f, 51.6f, 67.3f, 83.4f, 99.5f, 115.2f, 130.4f, 144.4f}},  // 3 iron
    {{3.2f, 10.6f, 21.7f, 35.4f, 50.2f, 65.8f, 81.5f, 97.2f, 112.7f, 127.3f, 141.1f}},  // 4 iron
    {{3.1f, 10.2f, 21.0f, 34.1f, 48.7f, 63.7f, 79.1f, 94.5f, 109.4f, 123.7f, 137.0f}},  // 5 iron
    {{2.9f, 9.7f, 19.9f, 32.4f, 46.3f, 60.9f, 75.7f, 90.4f, 104.8f, 118.4f, 131.3f}},  // 6 iron
    {{2.6f, 9.1f, 18.6f, 30.3f, 43.5f, 57.3f, 71.5f, 85.5f, 99.2f, 112.1f, 124.7f}},  // 7 iron
    {{2.4f, 8.4f, 17.2f, 28.0f, 40.1f, 53.2f, 66.6f, 79.7f, 92.6f, 105.1f, 116.9f}},  // 8 iron
    {{2.1f, 7.6f, 15.5f, 25.6f, 36.9f, 48.9f, 61.3f, 73.8f, 85.9f, 97.9f, 109.4f}},  // 9 iron
    {{1.8f, 6.5f, 13.2f, 21.9f, 31.8f, 42.4f, 53.5f, 64.7f, 75.9f, 86.6f, 97.5f}},  // pitching wedge
    {{1.3f, 5.0f, 10.5f, 17.2f, 25.6f, 34.3f, 43.7f, 53.3f, 62.9f, 72.6f, 81.7f}},  // approach wedge
    {{1.1f, 4.1f, 8.7f, 14.4f, 21.5f, 29.1f, 37.3f, 45.8f, 54.6f, 63.4f, 72.4f}},  // gap wedge
    {{0.9f, 3.3f, 7.1f, 11.9f, 17.6f, 24.4f, 31.5f, 39.0f, 46.8f, 54.9f, 62.9f}},  // sand wedge
    {{0.8f, 2.9f, 6.2f, 10.6f, 15.7f, 21.4f, 28.2f, 35.0f, 42.2f, 49.6f, 56.9f}},  // low-bounce wedge
    {{0.6f, 2.4f, 5.1f, 8.6f, 12.9f, 17.8f, 23.5f, 29.3f, 35.5f, 41.8f, 48.4f}},  // lob wedge
    {{0.5f, 1.9f, 4.1f, 7.0f, 10.4f, 14.5f, 18.9f, 23.8f, 29.3f, 34.7f, 40.3f}}  // high lob wedge
};
// Kind 7.
const ClubRow gClubRows7[25] = {
    {{1.5f, 5.4f, 11.0f, 18.4f, 26.8f, 36.3f, 46.2f, 56.4f, 66.7f, 76.8f, 86.4f}},  // driver 1
    {{1.4f, 5.3f, 10.7f, 18.0f, 26.2f, 35.3f, 45.1f, 55.1f, 65.1f, 75.0f, 84.4f}},  // driver 2
    {{1.4f, 5.1f, 10.5f, 17.5f, 25.5f, 34.4f, 44.0f, 53.7f, 63.5f, 73.1f, 82.3f}},  // driver 3
    {{1.3f, 5.0f, 10.2f, 17.0f, 24.8f, 33.5f, 42.7f, 52.3f, 61.8f, 71.2f, 80.4f}},  // driver 4
    {{1.3f, 5.0f, 10.0f, 16.7f, 24.5f, 33.0f, 42.2f, 51.5f, 61.0f, 70.3f, 79.2f}},  // driver 5
    {{1.3f, 4.9f, 9.9f, 16.3f, 24.2f, 32.6f, 41.6f, 50.9f, 60.2f, 69.3f, 78.3f}},  // driver 6
    {{1.3f, 4.9f, 9.9f, 16.6f, 24.3f, 32.8f, 41.9f, 51.2f, 60.6f, 69.9f, 78.9f}},  // 3 wood
    {{1.2f, 4.6f, 9.5f, 15.5f, 23.1f, 31.2f, 39.9f, 48.8f, 57.8f, 66.7f, 75.3f}},  // 5 wood
    {{1.1f, 4.1f, 8.4f, 13.9f, 20.6f, 27.9f, 35.7f, 43.8f, 52.0f, 60.2f, 68.1f}},  // 7 wood
    {{1.0f, 3.9f, 8.0f, 13.2f, 19.8f, 26.7f, 34.2f, 42.0f, 49.8f, 57.5f, 65.1f}},  // 1 iron
    {{0.9f, 3.6f, 7.5f, 12.5f, 18.6f, 25.2f, 32.4f, 39.8f, 47.3f, 54.9f, 62.2f}},  // 2 iron
    {{0.9f, 3.3f, 7.0f, 11.7f, 17.2f, 23.7f, 30.4f, 37.6f, 44.8f, 52.1f, 59.1f}},  // 3 iron
    {{0.8f, 3.0f, 6.5f, 10.8f, 15.9f, 22.0f, 28.3f, 35.0f, 41.8f, 48.7f, 55.6f}},  // 4 iron
    {{0.7f, 2.8f, 5.9f, 9.9f, 14.6f, 20.2f, 26.0f, 32.3f, 38.6f, 45.1f, 51.6f}},  // 5 iron
    {{0.6f, 2.4f, 5.1f, 8.7f, 12.8f, 17.6f, 23.1f, 28.6f, 34.5f, 40.4f, 46.2f}},  // 6 iron
    {{0.5f, 2.0f, 4.3f, 7.4f, 11.1f, 15.2f, 19.7f, 24.9f, 30.1f, 35.4f, 40.7f}},  // 7 iron
    {{0.4f, 1.7f, 3.6f, 6.1f, 9.2f, 12.7f, 16.6f, 20.8f, 25.5f, 30.2f, 34.9f}},  // 8 iron
    {{0.4f, 1.3f, 2.9f, 5.0f, 7.5f, 10.4f, 13.7f, 17.3f, 21.0f, 25.3f, 29.4f}},  // 9 iron
    {{0.2f, 0.9f, 2.1f, 3.6f, 5.4f, 7.6f, 10.0f, 12.6f, 15.4f, 18.6f, 21.7f}},  // pitching wedge
    {{0.2f, 0.6f, 1.3f, 2.2f, 3.4f, 4.8f, 6.3f, 8.1f, 10.0f, 12.0f, 14.2f}},  // approach wedge
    {{0.1f, 0.4f, 0.9f, 1.6f, 2.4f, 3.4f, 4.5f, 5.8f, 7.2f, 8.7f, 10.2f}},  // gap wedge
    {{0.1f, 0.3f, 0.6f, 1.1f, 1.7f, 2.4f, 3.2f, 4.1f, 5.1f, 6.2f, 7.4f}},  // sand wedge
    {{0.1f, 0.2f, 0.5f, 0.9f, 1.4f, 1.9f, 2.6f, 3.4f, 4.2f, 5.1f, 6.0f}},  // low-bounce wedge
    {{0.1f, 0.2f, 0.5f, 0.9f, 1.4f, 1.9f, 2.6f, 3.4f, 4.2f, 5.1f, 6.0f}},  // lob wedge
    {{0.1f, 0.2f, 0.5f, 0.9f, 1.4f, 1.9f, 2.6f, 3.4f, 4.2f, 5.1f, 6.0f}}   // high lob wedge
};

// The ground's pace by course setting: x gTurfSpeed's (0.6 .. 1.4, classes 2, 3, 4), then the
// green speed (options +0x18, class 3), the fairway (gFairwaySetting, class 2) and the rough
// (options +0x1C, class 5).
const f32 gTurfSpeedMul[5] = {
    0.6f, 0.8f, 1.0f, 1.2f, 1.4f
};
const f32 gGreenSpeedMul[3] = {
    1.0f, 1.1f, 1.2f
};
const f32 gFairwaySpeedMul[3] = {
    1.0f, 1.1f, 1.2f
};
const f32 gRoughMul[3] = {
    1.3f, 1.0f, 0.7f
};
void   PsBallFx_TriggerTrail(Ball* pBall, int nPlayer);    // rolling sound / effect
void   Ball_CupPull(Ball* pBall, f32 fDt);
void   fn_80055E7C(f32* pA, f32* pB, f32* pOut);
void   fn_80055EA0(f32* pA, f32* pB, f32* pOut);
void   fn_80055EC4(f32* pA, f32* pB, f32* pOut);
void   fn_80055EF8(f32* pA, f32* pOut);
f32    fn_80051124(Ball* pBall, f32 fAim, f32* pNormal);
f32    fn_800511F0(Ball* pBall, f32 fAim, f32* pNormal);
int    fn_801021FC(void);
void   fn_800348DC(void);

f32    fn_80055E10(f32 a, f32 b, f32 fSin, f32 fCos);
f32    fn_80055E1C(f32 a, f32 b, f32 fSin, f32 fCos);
void   fn_80055E28(f32 fAngle, f32* pSin, f32* pCos);
void   Physics_OutOfBounds(Ball* pBall, u8 bSound);
f32    Ball_DistanceToPin(f32* pPos);
void   Ball_Holed(Ball* pBall);
void   Ball_SimSeconds(Ball* pBall, f32 fSeconds, f32 fTick);

u8     fn_800B1B18(int nPlayer, f32* pTo, f32* pFrom, f32* pHit, f32* pNormal, HitObject** ppWhat);
u8     fn_80053E98(Ball* pBall, void* pv, f32* pHit, f32* pNormal);
u8     Physics_ProcessCollision(Ball* pBall, f32* pHit, f32* pNormal, SurfaceType* pSurface, TerObject* pObj,
                                f32* pFrac, f32 fTicks);
u8     fn_80053240(Ball* pBall, f32 fTicks);
// the bounce; returns the impact
f32    Physics_HandleCollision(Ball* pBall, f32* pNormal, SurfaceType* pSurface);
void   fn_800B1AB0(HitObject* pObj, f32* pPos, f32* pRadius);   // the flagstick's position and radius
u8     Ball_Collide(Ball* pBall, f32 fTicks);
void   Ball_GroundContact(Ball* pBall, f32 fTicks);
void   fn_80052268(Ball* pBall, f32 fTicks);
void   Ball_FlightStep(Ball* pBall, f32 fTicks);
f32    fn_80055324(Ball* pBall);
u8     fn_80054040(Ball* pBall, f32 fTicks);

extern u8  gSimulating;                          // 0x80281DD0  a rehearsal: no sounds or effects
extern u8  gSimFullCup;                          // a sim that still gets the cup pull and near-cup gravity
                                                 // (state 15, look-ahead)
extern u8  lbl_80281DD2;
// options +0x18 (GREEN SPEED?), 0..2: green friction x 1, 0.9, 0.8
extern s32 gGreenSpeedSetting;
extern s32 gFairwaySetting;                      // 0..2: class-2 friction x 1.0 / 0.9 / 0.8
// 0..4, default 2; rain sets 1 (light) or 0 (heavy): the putt table and friction
extern s32 gTurfSpeed;
// options +0x1C (ROUGH LENGTH?), 0..2: class-5 friction x 0.7, 1, 1.3
extern s32 gRoughSetting;
extern u8  lbl_80281DE4;                          // the two ground heights below are current
extern f32 lbl_80281DE0;                         // ground height under the ball
extern f32 lbl_80281DDC;                         // the other ground height (Ter_GetEnclosingGroundData)
extern f32 gWindSpeed;                           // 0x80281DE8
extern s32 gWindDir;                             // 0x80281DEC  0..7
extern f32 gWindDirs[8][4];                      // 0x80187EF8  unit vectors, 45 degrees apart

static inline u8 Ball_NoGround(f32 fHeight) {
    return fHeight < -60000.0f;
}

static inline f32 Ball_Clamp(f32 x, f32 fLo, f32 fHi) {
    if (x < fLo) return fLo;
    if (x > fHi) return fHi;
    return x;
}

#define PIN(pBall) (&(pBall)->pCourse->pin[Game_CurrentPinSet()].x)

// A landing: a player's ball fires 0x25 on flagged surfaces, then 0x26 on the cup (surface 90),
// 0x24 on an object, 0x23 on the ground; a look-ahead ball's first landing (Player.ballBefore) tells
// its player (0x49).
#define BALL_LANDING_EVENTS(pBall, pSurface, pObj)                                                 \
    if ((pBall)->nPlayer >= 0) {                                                                   \
        if ((pSurface)->u34 & 0x10) {                                                              \
            EVENT_Trigger((pBall)->nPlayer, 0x25, (pBall), !gSimulating);                            \
        }                                                                                          \
        if (fn_80050BEC(pSurface) == 90) {                                                         \
            EVENT_Trigger((pBall)->nPlayer, 0x26, (pBall), !gSimulating);                            \
        } else if (pObj) {                                                                         \
            EVENT_Trigger((pBall)->nPlayer, 0x24, (pBall), !gSimulating);                            \
        } else {                                                                                   \
            EVENT_Trigger((pBall)->nPlayer, 0x23, (pBall), !gSimulating);                            \
        }                                                                                          \
    } else if ((pBall)->nCollideCount == 0) {                                                                \
        int nOwner;                                                                                \
        for (nOwner = 0; nOwner < 5; nOwner++) {                                                   \
            if ((pBall) == &PLAYER(nOwner)->ballBefore) break;                                     \
        }                                                                                          \
        EVENT_Trigger(nOwner, 0x49, (pBall), 1);                                                     \
    }


// The ball dropped in: park it in the cup and stop it.
void Ball_Holed(Ball* pBall) {
    Vec3Copy(PIN(pBall), pBall->vPos);
    pBall->vPos[0] += INCHES(0.5f);
    pBall->vPos[1] -= INCHES(3.0f);
    pBall->vPos[2] += INCHES(0.5f);
    pBall->bHoled = 1;
    Ball_Stop(pBall);
}

// The ball ends in a hazard: state 5, lie 16, and (for a real ball) the splash event 0x22.
// Surfaces 98 and 105 send it to Ball_Holed instead.
void Physics_OutOfBounds(Ball* pBall, u8 bSound) {
    if (pBall->nSurface == 105 || pBall->nSurface == 98) {
        Ball_Holed(pBall);
        return;
    }
    pBall->nState = PHYSICS_BALLSTATE_BallOutOfBounds_e;
    pBall->nLie   = LIE_OUT_OF_BOUNDS_e;
    if (bSound && pBall->nPlayer >= 0) {
        EVENT_Trigger(pBall->nPlayer, 0x22, pBall, !gSimulating);
    }
}

// On for rehearsals and look-aheads: no sounds, no effects, no tree roll.
void Ball_SetSimulating(u8 bOn) {
    gSimulating = bOn;
}

void fn_80050D2C(u8 b) {
    gSimFullCup = b;
}

// Putt power for a distance: the putt table (22 steps of 0.05 power, distance ~ 43.4 x power
// squared on a medium green) scaled by the green-speed setting, interpolated; 1.1 beyond it.
f32 fn_80050D34(f32 fDist) {
    int i;
    f32 fScale = gPuttSpeedScale[gTurfSpeed];
    f32 fPower;
    for (i = 1; i < 23; i++) {
        f32 fHi = fScale * gPuttDist[i];
        if (fDist <= fHi) {
            f32 fLo = fScale * gPuttDist[i - 1];
            f32 fT = (fDist - fLo) / (fHi - fLo);
            fPower = 0.05f * (i - 1);
            fPower = 0.05f * fT + fPower;
            return fPower;
        }
    }
    fPower = 1.1f;
    return fPower;
}

// fake match: stands in for a function the original linker stripped. The file's pool has 20.0,
// 0.5, 2.0, 1.0, 0.0, 0.1 in that order right after fn_80050D34's constants, before the
// functions below use them (fn_80050F88 would put -60000 and 0.375 before 0.1); its body is
// unknown, this one only reproduces the order.
static f32 Ball_StrippedFn(f32 x) {
    x = (x + 2.0f) * 0.5f + 20.0f;
    if (x < 0.1f) return 1.0f;
    return 0.0f;
}

// A club's distance row for a shot kind (1..7, clubs 0..24): the row, and the surface the
// table assumes (45; 14 for the chip table). 0 for a putt or a bad club.
u8 fn_80050DE4(int nKind, int nClub, int a, const ClubRow** ppRow, s32* pSurface) {
    if (nClub < 0 || nClub >= 25) return 0;
    switch (nKind) {
    case 1:
        *ppRow = &gClubRows1[nClub];
        if (pSurface != NULL) {
            *pSurface = 45;
        }
        break;
    case 2:
        *ppRow = &gClubRows2[nClub];
        if (pSurface != NULL) {
            *pSurface = 14;
        }
        break;
    case 3:
        *ppRow = &gClubRows3[nClub];
        if (pSurface != NULL) {
            *pSurface = 45;
        }
        break;
    case 4:
        *ppRow = &gClubRows4[nClub];
        if (pSurface != NULL) {
            *pSurface = 45;
        }
        break;
    case 5:
        *ppRow = &gClubRows5[nClub];
        if (pSurface != NULL) {
            *pSurface = 45;
        }
        break;
    case 6:
        *ppRow = &gClubRows6[nClub];
        if (pSurface != NULL) {
            *pSurface = 45;
        }
        break;
    case 7:
        *ppRow = &gClubRows7[nClub];
        if (pSurface != NULL) {
            *pSurface = 45;
        }
        break;
    default:
        return 0;
    }
    return 1;
}

// A club's reach for a shot kind (the table entry's +0x24), 1 if there is none.
f32 fn_80050F44(int nKind, int nClub) {
    const ClubRow* pRow;
    if (fn_80050DE4(nKind, nClub, 0, &pRow, NULL)) {
        return pRow->fDist[9];
    }
    return 1.0f;
}

// Power for a distance with a club: the row's 11 distances are power 0.1 to 1.1, interpolated,
// plus the difference between the table's surface and the one under the ball (a surface that
// is not a stopping surface counts as 14); 1.1 beyond the row.
// A row's distance at column i (an accessor in the original: reading the array directly gives
// different registers).
static inline f32 ClubRow_Dist(const ClubRow* pRow, int i) {
    return pRow->fDist[i];
}

f32 fn_80050F88(f32 fDist, Ball* pBall, int nKind, int nClub) {
    s32          nSurface;
    SurfaceType* pSurface;
    const ClubRow* pRow;
    f32          vNormal[4];
    f32          fBase, fAdj, fFrac, fPower;
    int          i;
    if (pBall == NULL) return 0.0f;
    if (!fn_80050DE4(nKind, nClub, 0, &pRow, &nSurface)) return 1.0f;
    fBase = gSurfaceTypes[nSurface].f00;
    if (Ter_GetSupportingGroundData(pBall->pCourse, pBall->vPos, &pSurface, vNormal) < -60000.0f ||
        0.375f != pSurface->f1C) {
        pSurface = &gSurfaceTypes[14];
    }
    fAdj = fBase - pSurface->f00;
    // EA bug: a row has 11 entries (0..10), but a distance beyond fDist[10] reads fDist[11]: the
    // next club's first carry (for club 24, the start of whatever follows the table)
    for (i = 1; i < 12; i++) {
        if (fDist <= ClubRow_Dist(pRow, i)) {
            fFrac = (fDist - ClubRow_Dist(pRow, i - 1)) / (ClubRow_Dist(pRow, i) - ClubRow_Dist(pRow, i - 1));
            fPower = 0.1f * i;
            fPower = 0.1f * fFrac + fPower;
            fPower += fAdj;
            return fPower;
        }
    }
    fPower = 1.1f;
    return fPower;
}

// The ball's f70 plus its surface's first value; 1 without a ball or a surface.
f32 fn_800510EC(Ball* pBall) {
    if (pBall != NULL && pBall->nSurface >= 0) {
        return pBall->f70 + gSurfaceTypes[pBall->nSurface].f00;
    }
    return 1.0f;
}

// The slope along the aim: the ground normal turned into the aim's frame, then the angle of
// its x against its y, clamped to +-44 degrees.
f32 fn_80051124(Ball* pBall, f32 fAim, f32* pNormal) {
    f32 vN[4];
    f32 fSin, fCos;
    f32 fAngle;
    f32 fEps;
    Vec3Copy(pNormal, vN);
    fn_80055E28(fAim, &fSin, &fCos);
    fn_80055D70(&vN[2], &vN[0], fSin, fCos);
    fEps = 1e-6f;
    if (vN[1] < fEps && vN[1] > -fEps) {
        if (vN[0] < 0.0f) {
            fAngle = -1.5707964f;
        } else {
            fAngle = 1.5707964f;
        }
    } else {
        fAngle = -fn_8000AD78(vN[0], vN[1]);
    }
    if (fAngle < -0.76794487f) return -0.76794487f;
    if (fAngle > 0.76794487f) return 0.76794487f;
    return fAngle;
}

// The slope across the aim: as fn_80051124 with the normal's z.
f32 fn_800511F0(Ball* pBall, f32 fAim, f32* pNormal) {
    f32 vN[4];
    f32 fSin, fCos;
    f32 fAngle;
    f32 fEps;
    Vec3Copy(pNormal, vN);
    fn_80055E28(fAim, &fSin, &fCos);
    fn_80055D70(&vN[2], &vN[0], fSin, fCos);
    fEps = 1e-6f;
    if (vN[1] < fEps && vN[1] > -fEps) {
        if (vN[0] < 0.0f) {
            fAngle = -1.5707964f;
        } else {
            fAngle = 1.5707964f;
        }
    } else {
        fAngle = -fn_8000AD78(vN[2], vN[1]);
    }
    if (fAngle < -0.76794487f) return -0.76794487f;
    if (fAngle > 0.76794487f) return 0.76794487f;
    return fAngle;
}

// The strike: club, shot kind, power, aim, trajectory and the two launch blocks become the
// ball's velocity and spin, on the ground under the ball (no ground: 0).
//
// Speed: a per-club (chips: per-club chip; pitches with clubs 18..24 and kinds 5..7: fixed)
// speed, less 0.01 per club step for kind 4, x the kind's factor x 8.33 x power.
// A putt (kind 0 or the putter) is x 7.2 along pB, turned to the aim, laid onto the ground
// plane, x 1.8, no spin. Anything else is x 12.83 along pB, then:
// - uphill along the aim (fn_800511F0 > 0) costs speed, (max - slope) / max with max = 125
//   degrees (75 for a pitch) - 1.8 per club step + 40 x the slope, and tilts the ball up;
// - kind 5 is tilted up another 42 degrees;
// - the launch angle is the club's loft (chips: their own), + the kind's, - 0.8 degrees per
//   club step for kind 4, + the trajectory's (-5, 0, +5 degrees), clamped to 0..80 degrees,
//   and turns pA into the spin axis;
// - a sidehill lie (fn_80051124, +-45 degrees) turns that axis by 0.2 of the slope - not for
//   slot 4 or a perfect shot;
// - the lie (sand 6/7/8, rough 3/4) or the surface sets how much of the speed survives and
//   how much spin; the club step adds 1.25% of the loss back per step; a chip from lie 3..5
//   loses another 0.1;
// - spin = cross(the part of the direction off the axis, the axis turned by 0.9 of the side
//   slope) x club spin x kind spin x 0.85 x the lie's spin / 0.84.
// Velocity and spin are then turned to the aim.
u8 Physics_GetShotData(Ball* pBall, int nClub, int nKind, f32 fPower, f32 fAim, int nTrajectory, f32* pA,
               f32* pB, f32* pVel, f32* pSpin) {
    f32          vNormal[4];
    f32          vDir[4];
    f32          vAxis[4];
    f32          vAlong[4];
    f32          vOff[4];
    f32          vOffPart[4];
    f32          fSinAim, fCosAim;
    f32          fSin, fCos;
    f32          fSinF, fCosF;
    f32          fSinS, fCosS;
    SurfaceType* pSurface;
    f32          fSpeed, fSlope, fSide, fMax, fLaunch, fKeep, fSpin;
    int          nLie;

    if (Ter_GetSupportingGroundData(pBall->pCourse, pBall->vPos, &pSurface, vNormal) < -60000.0f) return 0;
    if (0.375f != pSurface->f1C) {
        pSurface = &gSurfaceTypes[14];
    }
    switch (nKind) {
    case 2:
        fSpeed = gChipSpeed[nClub];
        break;
    case 5:
        fSpeed = 0.38f;
        break;
    case 6:
        fSpeed = 0.3f;
        break;
    case 7:
        fSpeed = 0.47f;
        break;
    case 3:
        switch (nClub) {
        case 18:
            fSpeed = 0.386f;
            break;
        case 19:
            fSpeed = 0.373f;
            break;
        case 20:
            fSpeed = 0.372f;
            break;
        case 21:
            fSpeed = 0.37f;
            break;
        case 22:
            fSpeed = 0.299f;
            break;
        case 23:
            fSpeed = 0.299f;
            break;
        case 24:
            fSpeed = 0.299f;
            break;
        default:
            goto normal;  // fake match: the outer default's code, as in the original (a copy here: 98.9%)
        }
        break;
    default:
    normal:
        fSpeed = gClubSpeed[nClub];
        if (nKind == 4) {
            fSpeed -= 0.01f * gClubStep[nClub];
        }
        break;
    }
    fSpeed *= gKindSpeed[nKind];
    fSpeed = fPower * (8.333333f * fSpeed);
    fn_80055E28(fAim, &fSinAim, &fCosAim);
    if (nKind == SHOT_TYPE_PUTT_e || nClub == CLUB_PUTTER_e) {
        fSpeed *= 7.2f;
        fn_8001EF34(pB, fSpeed, vDir);
        fn_80055D70(&vDir[0], &vDir[2], fSinAim, fCosAim);
        fn_800BAF04(vNormal, vNormal);
        fn_8000C5D4(vDir, vNormal, -fn_8000C5FC(vDir, vNormal), pVel);
        fn_8001EF34(pVel, 1.8f, pVel);
        pSpin[0] = 0.0f;
        pSpin[1] = 0.0f;
        pSpin[2] = 0.0f;
        goto done;  // fake match: the shared return (a return here: 99.8%)
    }
    fSpeed *= 12.833333f;
    Vec3Copy(pB, vDir);
    fSlope = fn_800511F0(pBall, fAim, vNormal);
    if (fSlope > 0.0f) {
        if (nKind == 3) {
            fMax = 40.0f * fabsf(fSlope) + (75.0f - 1.8f * gClubStep[nClub]);
        } else {
            fMax = 40.0f * fabsf(fSlope) + (125.0f - 1.8f * gClubStep[nClub]);
        }
        fMax = 0.017453292f * fMax;
        fSpeed *= (1.0f / fMax) * (fMax - fSlope);
        fn_80055E28(fSlope, &fSinF, &fCosF);
        fn_80055D70(&vDir[2], &vDir[1], fSinF, fCosF);
    }
    fn_8001EF34(vDir, fSpeed, vDir);
    if (nKind == 5) {
        fn_80055E28(0.7330383f, &fSin, &fCos);
        fn_80055D70(&vDir[2], &vDir[1], fSin, fCos);
    }
    if (nKind == 2) {
        fLaunch = gChipLoft[nClub];
    } else {
        fLaunch = gClubLoft[nClub];
    }
    fLaunch += gKindLoft[nKind];
    if (nKind == 4) {
        fLaunch -= 0.013962634f * gClubStep[nClub];
    }
    fLaunch += gTrajLoft[nTrajectory];
    fn_80055E28(Ball_Clamp(fLaunch, 0.0f, 1.3962634f), &fSin, &fCos);
    Vec3Copy(pA, vAxis);
    vAxis[3] = 0.0f;
    fn_80055D70(&vAxis[2], &vAxis[1], fSin, fCos);
    if (pBall->nPlayer == 4 ||
        (pBall->nPlayer >= 0 && pBall->nPlayer <= 3 && gPlayers[pBall->nPlayer].bPerfect)) {
        fSide = 0.0f;
    } else {
        fSide = fn_80051124(pBall, fAim, vNormal);
        if (fSide) {
            if (fSide < -0.7853982f) {
                fSide = -0.7853982f;
            } else if (fSide > 0.7853982f) {
                fSide = 0.7853982f;
            }
            fn_80055E28(0.2f * fSide, &fSinS, &fCosS);
            fn_80055D70(&vAxis[0], &vAxis[1], fSinS, fCosS);
        }
    }
    fn_80055EC4(vDir, vAxis, vAlong);
    fn_80055EA0(vDir, vAlong, vOff);
    fn_8001EF34(vOff, 1.0f - 0.9f, vOffPart);   // 0x3DCCCCD0, one bit above 0.1f
    nLie = pBall->nLie;
    switch (nLie) {
    case LIE_SAND_HIGH_e:
        fKeep = 0.8f;
        fSpin = 0.9f;
        break;
    case LIE_SAND_MEDIUM_e:
        fKeep = 0.7f;
        fSpin = 0.8f;
        break;
    case LIE_SAND_DEEP_e:
        fKeep = 0.6f;
        fSpin = 0.7f;
        break;
    case LIE_ROUGH_HIGH_e:
        fKeep = 0.9f;
        fSpin = 0.7f;
        break;
    case LIE_ROUGH_e:
        fKeep = 0.8f;
        fSpin = 0.7f;
        break;
    default:
        fKeep = pSurface->f00;
        fSpin = pSurface->f08;
        break;
    }
    fKeep += pBall->f70;
    fKeep = 0.0125f * (1.0f - fKeep) * gClubStep[nClub] + fKeep;
    if (nKind == SHOT_TYPE_CHIP_e
        && (nLie == LIE_ROUGH_HIGH_e || nLie == LIE_ROUGH_e || nLie == LIE_THICK_ROUGH_e)) {
        fKeep -= 0.1f;
    }
    fn_80055E7C(vAlong, vOffPart, vAlong);
    fn_8001EF34(vAlong, fKeep, pVel);
    fKeep = gClubSpin[nClub] * gKindSpin[nKind];
    fKeep *= 0.85f;
    fSpin = 1.1904762f * (fKeep * fSpin);
    fn_80055EA0(vOff, vOffPart, vOff);
    if (fSide) {
        fn_80055E28(0.9f * fSide, &fSinS, &fCosS);
        fn_80055D70(&vAxis[0], &vAxis[1], fSinS, fCosS);
    }
    vec4flt_CrossProduct(vOff, vAxis, pSpin);
    fn_8001EF34(pSpin, fSpin, pSpin);
    fn_80055D70(&pVel[0], &pVel[2], fSinAim, fCosAim);
    fn_80055D70(&pSpin[0], &pSpin[2], fSinAim, fCosAim);
done:
    return 1;
}

// Launch the ball from a point along a direction at a speed (x 0.489): in the air, no spin.
void fn_80051A18(Ball* pBall, f32* pDir, f32 fSpeed, f32* pFrom) {
    pBall->nState = 2;
    Vec_Copy(pFrom, pBall->vStart);
    Vec_Copy(pFrom, pBall->vPos);
    Vec_Copy(pFrom, pBall->vPrev);
    fn_8001EF34(pDir, 0.48888889f * fSpeed, pBall->vVel);
    pBall->vSpin[0] = 0.0f;
    pBall->vSpin[1] = 0.0f;
    pBall->vSpin[2] = 0.0f;
    pBall->f3C      = 0.0f;
    pBall->fAC      = 0.0f;
    pBall->fLastDistFromInitShotPos      = 0.0f;
    pBall->fTimeSinceLastCheck      = 0.0f;
    pBall->fSpinY   = 0.0f;
    pBall->fSpinX   = 0.0f;
    pBall->nSolidCollideCount      = 0;
    pBall->nCollideCount      = 0;
    pBall->bGotFirstSandPos    = 0;
    pBall->b99      = 1;
    pBall->b9B      = 1;
    pBall->bHitTopArc      = 1;
}

// Strike the ball: Physics_GetShotData turns club, kind, power, aim, trajectory and the two launch
// blocks into its velocity and spin (failing that it is a hazard). A putt (kind 0, or the
// putter) starts rolling (state 3), anything else is in the air (state 2). Event 10.
void Ball_Launch(Ball* pBall, int nClub, int nKind, f32 fPower, f32 fAim, int nTrajectory, f32* pA, f32* pB) {
    Vec_Copy(pBall->vPos, pBall->vStart);
    pBall->fAC    = 0.0f;
    pBall->fLastDistFromInitShotPos    = 0.0f;
    pBall->fTimeSinceLastCheck    = 0.0f;
    pBall->fSpinY = 0.0f;
    pBall->fSpinX = 0.0f;
    pBall->nSolidCollideCount    = 0;
    pBall->nCollideCount    = 0;
    pBall->bGotFirstSandPos  = 0;
    if (!Physics_GetShotData(pBall, nClub, nKind, fPower, fAim, nTrajectory, pA, pB, pBall->vVel,
                             pBall->vSpin)) {
        Physics_OutOfBounds(pBall, 1);
        return;
    }
    if (nKind == SHOT_TYPE_PUTT_e || nClub == CLUB_PUTTER_e) {
        pBall->b99    = 1;
        pBall->b9B    = 1;
        pBall->bHitTopArc    = 1;
        pBall->nState = 3;
    } else {
        pBall->b99    = 0;
        pBall->b9B    = 0;
        pBall->bHitTopArc    = 0;
        pBall->nState = 2;
    }
    pBall->f70 = 0.0f;
    if (pBall->nPlayer >= 0 && pBall->nPlayer <= 3) {
        Vec3Copy(pBall->vPos, lbl_801D5888[pBall->nPlayer]);
        Vec3Copy(pBall->vPos, lbl_801D58C8[pBall->nPlayer]);
    }
    if (pBall->nPlayer >= 0) {
        EVENT_Trigger(pBall->nPlayer, 10, pBall, !gSimulating);
    }
}

// The spin stick's input: each axis must be within -1..1; stored x 15.
void fn_80051C84(Ball* pBall, f32 fX, f32 fY) {
    if (fX < -1.0f || fX > 1.0f || fY < -1.0f || fY > 1.0f) return;
    if (pBall == NULL) return;
    pBall->fSpinX = 15.0f * fX;
    pBall->fSpinY = 15.0f * fY;
}

// One tick in the air. The wind (a CPU's clamped to +-15 on each axis) is weaker near the
// ground: x (0.25 + 0.75 x height / 25 ft) below 25 ft. Air speed is the velocity less 0.19 x
// the wind. Drag and lift are quadratic in air speed with coefficients that depend on speed and
// spin; lift is along spin x air velocity. Gravity 0.10717 - and, for a real ball (or a sim
// with gSimFullCup set), three times that within 2.25 in of the top of the cup. Event 0x1C
// at the top of the flight. Spin decays 0.3% a tick, faster flying into the wind.
void Ball_FlightStep(Ball* pBall, f32 fTicks) {
    f32 vWind[4];
    f32 vRel[4];
    f32 vDrag[4];
    f32 vLift[4];
    f32 vAccel[4];
    f32 vPin[4];
    f32 fDrag, fSpeed2, fSpin, fSpeed, fLift, fLen, fInto, fExtra, fHeight, fK, fX, fZ;

    Wind_Get(vWind);
    if (Player_IsCPU(pBall->nPlayer)) {
        vWind[0] = Ball_Clamp(vWind[0], -15.0f, 15.0f);
        vWind[1] = Ball_Clamp(vWind[1], -15.0f, 15.0f);
        vWind[2] = Ball_Clamp(vWind[2], -15.0f, 15.0f);
    }
    if (pBall->fHeight < 8.333333f) {
        fHeight = pBall->fHeight;
        if (fHeight < 0.0f) {
            fHeight = 0.0f;
        }
        fn_8001EF34(vWind, 0.75f * (fHeight / 8.333333f) + 0.25f, vWind);
    }
    fn_8000C5D4(pBall->vVel, vWind, -0.190666676f, vRel);
    fSpeed2 = fn_80009744(vRel);
    fSpeed  = fn_80009680(fSpeed2);
    fSpin   = fn_80009680(fn_80009744(pBall->vSpin));
    if (fSpeed != 0.0f) {
        fDrag = 0.000780952396f * fSpeed;
        fDrag = -(fSpeed2 * (0.000474568689f
                             * (0.225790471f + (fSpin * (-0.000348685688f * fSpeed + 0.0168940704f) + fDrag)))
                  / fSpeed);
    } else {
        fDrag = 0.0f;
    }
    fn_8001EF34(vRel, fDrag, vDrag);
    fLift = -0.000201047602f * fSpeed;
    fLift = fSpeed2 * (0.000474568689f
                       * (0.0847342834f + (fSpin * (-0.000628289126f * fSpeed + 0.0407094695f) + fLift)));
    vec4flt_CrossProduct(pBall->vSpin, vRel, vLift);
    fLen = fn_80009680(fn_80009744(vLift));
    fn_8001EF34(vLift, fLen != 0.0f ? fLift / fLen : 0.0f, vAccel);
    fn_80055E7C(vDrag, vAccel, vAccel);
    vAccel[1] -= 0.107170001f;
    if (!gSimulating || gSimFullCup) {
        Vec_Copy(PIN(pBall), vPin);
        vPin[1] += BALL_RADIUS;
        if (Vec_Distance(vPin, pBall->vPos) < 0.0625f) {
            vAccel[1] -= 0.214340001f;
        }
    }
    fn_8000C5D4(pBall->vVel, vAccel, fTicks, pBall->vVel);
    if (!pBall->bHitTopArc && pBall->nPlayer >= 0 && pBall->vVel[1] < 0.0f
        && pBall->vVel[1] - fTicks * vAccel[1] >= 0.0f) {
        pBall->bHitTopArc = 1;
        EVENT_Trigger(pBall->nPlayer, 0x1C, pBall, !gSimulating);
    }
    fInto  = vWind[0] * pBall->vVel[0] + vWind[2] * pBall->vVel[2];
    if (fInto < 0.0f) {
        fK = fTicks * fInto / fn_80009744(pBall->vVel);
        fX = fK * pBall->vVel[0];
        fZ = fK * pBall->vVel[2];
        fExtra = 0.3f * (f32)fn_80009680(fX * fX + fZ * fZ);
    } else {
        fExtra = 0.0f;
    }
    fn_8001EF34(pBall->vSpin, 1.0f - fTicks * (0.003f * (1.0f + fExtra)), pBall->vSpin);
}

// The player a ball belongs to, 4 for a ball with none. An inline in the original: its early
// exit is what gives the branch layout.
static inline int Ball_Owner(Ball* pBall) {
    if (pBall->nPlayer >= 0) return pBall->nPlayer;
    return 4;
}

// The ground under a rolling ball: its surface and normal. No ground: if there is none under
// the other source either (or it is more than 1 in above the ball), the ball may coast on for
// up to two ticks on surface 109 (or its own) as if on flat ground - unless fn_800E2B40 says
// otherwise - and is a hazard after that. Returns 0 for a hazard.
u8 Physics_GetSurfaceInfo(Ball* pBall, SurfaceType** ppSurface, f32* pNormal) {
    SurfaceType* pSurface;
    SurfaceType* pSurface2;
    f32          vNormal[4];
    f32          vNormal2[4];
    f32          fDrop;
    int          nPlayer;
    Ter_GetEnclosingGroundData(pBall->pCourse, pBall->vPos, &lbl_80281DE0, &pSurface, vNormal, &lbl_80281DDC,
                               &pSurface2, vNormal2);
    lbl_80281DE4 = 1;
    if (lbl_80281DE0 < -60000.0f) {
        if (!Ball_NoGround(lbl_80281DDC)) {
            fDrop = lbl_80281DDC - pBall->vPos[1];
        }
        if (lbl_80281DDC < -60000.0f || fDrop > 0.028f) {
            nPlayer = Ball_Owner(pBall);
            if (lbl_80281DDC < -60000.0f && pBall->fAC < 2.0f && !fn_800E2B40(nPlayer, pBall)) {
                // EA bug: meant as a range check (nSurface >= 156); as written any valid surface
                // becomes 109 and one past the table is kept and read below
                if (pBall->nSurface < 0 || pBall->nSurface < 156) {
                    pBall->nSurface = 109;
                }
                pSurface = &gSurfaceTypes[pBall->nSurface];
                lbl_80281DE0 = pBall->vPrev[1] - BALL_RADIUS - 0.0013888889f;
                pBall->fAC += 1.0f;
                vNormal[0] = 0.0f;
                vNormal[1] = 1.0f;
                vNormal[2] = 0.0f;
                vNormal[3] = 0.0f;
                goto check;  // fake match: past the fAC reset, as in the original (if/else: 97.5%)
            }
            Physics_OutOfBounds(pBall, 1);
            return 0;
        }
        pSurface = pSurface2;
        Vec3Copy(vNormal2, vNormal);
    }
    pBall->fAC = 0.0f;
check:
    if (0.375f != pSurface->f1C) {
        pSurface = &gSurfaceTypes[14];
    }
    *ppSurface = pSurface;
    Vec3Copy(vNormal, pNormal);
    return 1;
}

// State 3, skidding: a ball on the ground whose spin has not caught up with its speed. Gravity
// along the ground plane (x (1 - surface +0x14) and the course settings) accelerates it; it is
// kept on the plane at its speed; friction (1.5 x surface +0x18 x the normal force, same
// settings) builds roll spin about dir x normal. Once 0.84 x the spin reaches the speed it
// is rolling (state 4).
void fn_80052268(Ball* pBall, f32 fTicks) {
    f32          vNormal[4];
    f32          vAccel[4];
    f32          vTmp[4];
    f32          vDir[4];
    f32          vSpinAdd[4];
    SurfaceType* pSurface;
    f32          fPull, fDot, fFric;
    if (!Physics_GetSurfaceInfo(pBall, &pSurface, vNormal)) return;
    pBall->nSurface = fn_80050BEC(pSurface);
    fn_800BAF04(vNormal, vNormal);
    fPull = 1.0f - pSurface->f14;
    if (pSurface->nClass == 3) {
        fPull *= 2.0f - gGreenSpeedMul[gGreenSpeedSetting];
    }
    if (pSurface->nClass == 2) {
        fPull *= 2.0f - gFairwaySpeedMul[gFairwaySetting];
    }
    if (pSurface->nClass == 5) {
        fPull *= 2.0f - gRoughMul[gRoughSetting];
    }
    if (pSurface->nClass == 3 || pSurface->nClass == 4 || pSurface->nClass == 2) {
        fPull *= 2.0f - gTurfSpeedMul[gTurfSpeed];
    }
    fDot = -0.107170001f * vNormal[1];
    vAccel[0] = -(fPull * (vNormal[0] * fDot));
    vAccel[1] = fPull * (-0.107170001f - vNormal[1] * fDot);
    vAccel[2] = -(fPull * (vNormal[2] * fDot));
    fn_8000C5D4(pBall->vVel, vNormal, -fn_8000C5FC(pBall->vVel, vNormal), vTmp);
    fn_800BAF04(vTmp, vDir);
    fn_8001EF34(vDir, fn_80009680(fn_80009744(pBall->vVel)), pBall->vVel);
    fFric = 1.5f * (pSurface->f18 * (-0.107170001f * vNormal[1]));
    if (pSurface->nClass == 3) {
        fFric *= 2.0f - gGreenSpeedMul[gGreenSpeedSetting];
    }
    if (pSurface->nClass == 2) {
        fFric *= 2.0f - gFairwaySpeedMul[gFairwaySetting];
    }
    if (pSurface->nClass == 5) {
        fFric *= 2.0f - gRoughMul[gRoughSetting];
    }
    if (pSurface->nClass == 3 || pSurface->nClass == 4 || pSurface->nClass == 2) {
        fFric *= 2.0f - gTurfSpeedMul[gTurfSpeed];
    }
    fFric = 2.97619057f * fFric;
    vec4flt_CrossProduct(vDir, vNormal, vSpinAdd);
    fn_8001EF34(vSpinAdd, fFric, vSpinAdd);
    fn_8000C5D4(pBall->vVel, vAccel, fTicks, pBall->vVel);
    fn_8000C5D4(pBall->vSpin, vSpinAdd, fTicks, pBall->vSpin);
    fTicks = fn_80009680(fn_80009744(pBall->vVel));
    if (0.84f * (f32)fn_80009680(fn_80009744(pBall->vSpin)) >= fTicks) {
        pBall->nState = 4;
    }
    if (pBall->nPlayer >= 0) {
        PsBallFx_TriggerTrail(pBall, pBall->nPlayer);
    }
}

// The bounce. Returns the square of the speed into the surface (Ball_Collide keeps its root as
// the sand-landing impact).
// - Water (class 7, 16) quarters the spin; elsewhere sideways spin is capped (2.93, or the
//   ball's ground speed once that is 5.28 or more).
// - The normal is bent toward the incoming ball by how soft the ground is: speed into it /
//   surface +0x24 (x the course settings; the green-speed setting and rain make it firmer or
//   softer) + surface +0x28 (x 0.667 in rough, x 0.5 for class 11); past a full bend the ball
//   comes straight back.
// - In the frame of that normal: restitution is surface +0x0C (0.5..1 counts as 0.5); on
//   surfaces with a negative +0x0C (branches, leaves) a real ball's value is randomised by
//   +-0.75 x (1 + it), pushed down by 0.005 x LUCK - the "kinder" deflection - and course 9
//   halves it first. Fast balls on short grass bounce less (x 1 - 1.2 x (speed - 8.8) / +0x24),
//   short grass and rough lose a bit more when slow, and rain deadens the bounce.
// - Friction at the contact point (surface +0x10 x 0.3, x rain) takes sliding speed and turns
//   it into spin; a soft surface scales spin by (1 + restitution) instead. Rough x 0.8 and class
//   11 x 0.6 on everything.
// - A ball that has stopped bouncing (rising under 0.67, ground speed under 1.91) starts to
//   roll; on its first five bounces, on dry short grass more than 63 yd from where it was
//   hit, backspin makes it check or spin back (x 13.3 x friction; a tenth of that if the spin
//   stick was used or the spin is not backspin).
// - Short grass with a soft landing speed (+0x24 of 80 or less) loses up to 35% of its rise.
f32 Physics_HandleCollision(Ball* pBall, f32* pNormal, SurfaceType* pSurface) {
    f32 vBent[4];
    f32 vDown[4];
    f32 vCon[4];
    f32 vSlide[4];
    f32 vDir[4];
    f32 vSlip[4];
    f32 fA;
    f32 fSpeed;
    f32 fImpact;
    f32 fRest;
    f32 fCo;
    f32 fC;
    f32 fD;
    f32 fB;
    f32 fSi;
    f32 fT, fS, fLen, fBounce, fGrip, fBite, fScale;
    u8  bFlip;

    pBall->nCollideCount++;
    if (pSurface->nClass == 7 || pSurface->nClass == 16) {
        fn_8001EF34(pBall->vSpin, 0.25f, pBall->vSpin);
    } else {
        fT = 0.84f * pBall->vSpin[0];
        fS = 0.84f * pBall->vSpin[2];
        fT = fT * fT;
        fS = fS * fS;
        fSi = fn_80009680(fT + fS);
        fLen = fn_80009680(pBall->vVel[0] * pBall->vVel[0] + pBall->vVel[2] * pBall->vVel[2]);
        if (fLen < 5.28000021f) {
            if (fSi > 2.9333334f) {
                fn_8001EF34(pBall->vSpin, 2.9333334f / fSi, pBall->vSpin);
            }
        } else if (fSi > fLen) {
            fn_8001EF34(pBall->vSpin, fLen / fSi, pBall->vSpin);
        }
    }
    bFlip = 0;
    if (pNormal[1] < 0.0f) {
        fn_80055EF8(pNormal, pNormal);
        fn_80055EF8(pBall->vVel, pBall->vVel);
        fn_80055EF8(pBall->vSpin, pBall->vSpin);
        bFlip = 1;
    }
    fSpeed = fn_80009680(fn_80009744(pBall->vVel));
    fD     = fn_8000C5FC(pBall->vVel, pNormal);
    fImpact = (pNormal[0] * fD) * (pNormal[0] * fD) + (pNormal[1] * fD) * (pNormal[1] * fD)
              + (pNormal[2] * fD) * (pNormal[2] * fD);
    fn_8000C5D4(pNormal, pBall->vVel, 1.0f / fSpeed, vBent);
    fn_80055EF8(vBent, vBent);
    fA = fabsf(fD) / pSurface->f24;
    if (pSurface->nClass == 3) {
        fA *= 2.0f - gGreenSpeedMul[gGreenSpeedSetting];
    }
    if (pSurface->nClass == 2) {
        fA *= 2.0f - gFairwaySpeedMul[gFairwaySetting];
    }
    if (pSurface->nClass == 5) {
        fA *= 2.0f - gRoughMul[gRoughSetting];
    }
    if (pSurface->nClass == 3 || pSurface->nClass == 4 || pSurface->nClass == 2) {
        fA *= 2.0f - (1.4f * (gTurfSpeedMul[gTurfSpeed] - 1.0f) + 1.0f);
    }
    fT = pSurface->f28;
    if (pSurface->nClass == 5) {
        fT *= 0.667f;
    }
    if (pSurface->nClass == 11) {
        fT *= 0.5f;
    }
    fA += fT;
    fLen = fn_80009680(fn_80009744(vBent));
    if (fLen < fA) {
        fn_80055EF8(pBall->vVel, pNormal);
    } else if (fLen != 0.0f) {
        fn_8000C5D4(pNormal, vBent, fA / fLen, pNormal);
    }
    fn_800BAF04(pNormal, pNormal);
    fn_8001EF34(pNormal, -0.839999974f, vDown);
    vec4flt_CrossProduct(pBall->vSpin, vDown, vCon);
    fn_80055E7C(vCon, pBall->vVel, vCon);
    fn_8000C5D4(vCon, pNormal, -fn_8000C5FC(vCon, pNormal), vSlide);
    if (fn_80009744(vSlide) != 0.0f) {
        fn_800BAF04(vSlide, vDir);
    } else {
        Vec3Copy(pNormal, vDir);
    }
    fA  = fn_8000AD78(pNormal[0], pNormal[1]);
    fB  = fn_800095F0(fA);
    fCo = fn_80009638(fA);
    vBent[0] = fn_80055E1C(pNormal[0], pNormal[1], fB, fCo);
    vBent[1] = fn_80055E10(pNormal[1], pNormal[0], fB, fCo);
    vBent[2] = pNormal[2];
    fD    = fn_80055E1C(vDir[0], vDir[1], fB, fCo);
    fC    = fn_80055E10(vDir[1], vDir[0], fB, fCo);
    fRest = vDir[2];
    fn_80055D70(&pBall->vVel[0], &pBall->vVel[1], fB, fCo);
    fn_80055D70(&pBall->vSpin[0], &pBall->vSpin[1], fB, fCo);
    fB  = -fn_8000AD78(vBent[2], vBent[1]);
    fCo = fn_800095F0(fB);
    fSi = fn_80009638(fB);
    fT  = fn_80055E1C(fC, fRest, fCo, fSi);
    fC  = fn_80055E10(fRest, fT, fCo, fSi);
    fn_80055D70(&pBall->vVel[1], &pBall->vVel[2], fCo, fSi);
    fn_80055D70(&pBall->vSpin[1], &pBall->vSpin[2], fCo, fSi);
    fC  = fn_8000AD78(fD, fC);
    fSi = fn_800095F0(fC);
    fCo = fn_80009638(fC);
    fn_80055D70(&pBall->vVel[0], &pBall->vVel[2], fSi, fCo);
    fn_80055D70(&pBall->vSpin[0], &pBall->vSpin[2], fSi, fCo);
    vSlip[0] = 0.462857157f * (0.839999974f * pBall->vSpin[2] + pBall->vVel[0]);
    fRest = pSurface->f0C;
    if (fRest > 0.5f && fRest < 1.0f) {
        fRest = 0.5f;
    }
    if (fRest < 0.0f) {
        if (!(gSimulating || lbl_80281DD2 || pBall->nPlayer == 4 ||
              (pBall->nPlayer >= 0 && pBall->nPlayer <= 3 && gPlayers[pBall->nPlayer].bPerfect))) {
            fD = 1.0f - 2.0f * Rand_Float(0);
            fD -= 0.005f * (s8)Golfer_GetAttribute(&gPlayers[pBall->nPlayer], ATTR_LUCK, ATTR_TOTAL);
            if (fD < -1.0f) {
                fD = -1.0f;
            }
            if (Game_GetCourse() == 9) {
                fRest -= 0.5f * (1.0f + fRest);
            }
            fRest = (0.75f * (1.0f + fRest)) * fD + fRest;
        }
    } else {
        pBall->nSolidCollideCount++;
    }
    if (fSpeed > 8.80000019f && fRest >= 0.0f) {
        fT = 1.0f - 1.20000005f * ((fSpeed - 8.80000019f) / pSurface->f24);
    } else {
        fT = 1.0f;
    }
    fBounce = 0.0350000001f + (fRest * fT + 1.0f);
    if (fRest >= 0.0f) {
        if (pSurface->nClass == 4 || pSurface->nClass == 2 || pSurface->nClass == 3
            || pSurface->nClass == 5) {
            if (pSurface->f24 <= 80.0f && fSpeed < pSurface->f24) {
                fBounce -= ((0.150000006f * fRest) * (pSurface->f24 - fSpeed)) / pSurface->f24;
            }
            if (pSurface->nClass == 3 || pSurface->nClass == 4 || pSurface->nClass == 2) {
                fBounce *= 0.109999999f * (gTurfSpeedMul[gTurfSpeed] - 1.0f) + 1.0f;
            }
        }
    } else if (pBall->fSpeed < 2.44444442f) {
        fBounce = 0.0f;
    }
    vSlip[1] = 1.62f * (fBounce * pBall->vVel[1]);
    vSlip[2] = 0.462857157f * (pBall->vVel[2] - 0.839999974f * pBall->vSpin[0]);
    fT = vSlip[0] * vSlip[0];
    fS = vSlip[2] * vSlip[2];
    fD = fn_80009680(fT + fS);
    fGrip = pSurface->f10 * fabsf(vSlip[1]) * 0.3f;
    if (pSurface->nClass == 4 || pSurface->nClass == 3 || pSurface->nClass == 2) {
        fGrip *= gTurfSpeedMul[gTurfSpeed];
    }
    if (fGrip < fD && fD != 0.0f) {
        fGrip = pSurface->f10 * 0.3f;
        if (pSurface->nClass == 3 || pSurface->nClass == 4 || pSurface->nClass == 2) {
            fGrip *= gTurfSpeedMul[gTurfSpeed];
        }
        fScale = fabsf(fGrip * vSlip[1] / fD);
        vSlip[0] *= fScale;
        vSlip[2] *= fScale;
    }
    fn_8000C5D4(pBall->vVel, vSlip, -0.61728394f, pBall->vVel);
    if (fRest >= 0.0f) {
        pBall->vSpin[0] = 1.83715463f * vSlip[2] + pBall->vSpin[0];
        pBall->vSpin[2] = 1.83715463f * vSlip[0] + pBall->vSpin[2];
    } else {
        fn_8001EF34(pBall->vSpin, 1.0f + fRest, pBall->vSpin);
    }
    if (pSurface->nClass == 5 || pSurface->nClass == 11) {
        if (pSurface->nClass == 5) {
            fSi = 0.8f;
        } else {
            fSi = 0.6f;
        }
        fn_8001EF34(pBall->vVel, fSi, pBall->vVel);
        fn_8001EF34(pBall->vSpin, fSi, pBall->vSpin);
    }
    if (pSurface != NULL && pSurface->f0C >= 0.0f && pBall->vVel[1] < 0.674666584f &&
        (f32)fn_80009680(pBall->vVel[0] * pBall->vVel[0] + pBall->vVel[2] * pBall->vVel[2]) < 1.90666652f) {
        if (pBall->nSolidCollideCount < 6
            && (pSurface->nClass == 4 || pSurface->nClass == 3 || pSurface->nClass == 2) &&
            gTurfSpeedMul[gTurfSpeed] > 0.7f &&
            (f32)fn_80009680((pBall->vPos[0] - pBall->vStart[0]) * (pBall->vPos[0] - pBall->vStart[0]) +
                             (pBall->vPos[2] - pBall->vStart[2]) * (pBall->vPos[2] - pBall->vStart[2]))
                > 63.0f) {
            fBite = 13.333333f * (0.3f * pSurface->f10);
            if (pBall->fSpinX != 0.0f || pBall->fSpinY != 0.0f) {
                fBite *= 0.1f;
            } else if (-0.839999974f * (pBall->vVel[0] * pBall->vSpin[2])
                           + -0.839999974f * (pBall->vVel[2] * -pBall->vSpin[0]) >= 0.0f) {
                fBite *= 0.1f;
            }
            pBall->vVel[0] = pBall->vVel[0] - fBite * (-0.839999974f * -pBall->vSpin[2]);
            pBall->vVel[2] = pBall->vVel[2] - fBite * (-0.839999974f * pBall->vSpin[0]);
        }
        if (pBall->nState != 4) {
            pBall->nState   = 4;
            pBall->nSurface = fn_80050BEC(pSurface);
        }
    }
    fD  = -fC;
    fC  = fn_800095F0(fD);
    fD  = fn_80009638(fD);
    fn_80055D70(&pBall->vVel[0], &pBall->vVel[2], fC, fD);
    fn_80055D70(&pBall->vSpin[0], &pBall->vSpin[2], fC, fD);
    fD  = -fB;
    fC  = fn_800095F0(fD);
    fD  = fn_80009638(fD);
    fn_80055D70(&pBall->vVel[1], &pBall->vVel[2], fC, fD);
    fn_80055D70(&pBall->vSpin[1], &pBall->vSpin[2], fC, fD);
    fD  = -fA;
    fC  = fn_800095F0(fD);
    fD  = fn_80009638(fD);
    fn_80055D70(&pBall->vVel[0], &pBall->vVel[1], fC, fD);
    fn_80055D70(&pBall->vSpin[0], &pBall->vSpin[1], fC, fD);
    if (pSurface != NULL && pSurface->f0C >= 0.0f &&
        (pSurface->nClass == 4 || pSurface->nClass == 3 || pSurface->nClass == 2) &&
        pSurface->f24 <= 80.0f && fSpeed < pSurface->f24) {
        pBall->vVel[1] *= 1.0f - 0.349999994f * ((pSurface->f24 - fSpeed) / pSurface->f24);
    }
    if (bFlip) {
        fn_80055EF8(pNormal, pNormal);
        fn_80055EF8(pBall->vVel, pBall->vVel);
        fn_80055EF8(pBall->vSpin, pBall->vSpin);
    }
    return fImpact;
}

// A ball on the ground: did it run into anything between last tick and this one? Out of
// bounds (600 yd from the start) is a hazard. Within 10 ft of the pin the detailed test
// (Ter_CheckForPinCollision) runs, elsewhere fn_8004FF34. On a hit: the landing events, the bounce, and a
// nudge along the velocity. Returns 1 on a hit.
u8 fn_80053240(Ball* pBall, f32 fTicks) {
    f32          vHit[4];
    f32          vNormal[4];
    f32          vFrom[4];
    f32          vTo[4];
    TerObject*   pObj;
    SurfaceType* pSurface;
    if (pBall->pCourse == NULL) return 0;
    Vec3Copy(pBall->vPrev, vFrom);
    vFrom[1] -= BALL_RADIUS;
    Vec3Copy(pBall->vPos, vTo);
    vTo[1] -= BALL_RADIUS;
    if (fn_800BB028(vTo, pBall->vStart) > 360000.0f) {
        Physics_OutOfBounds(pBall, 1);
        return 0;
    }
    if (pBall->nSurface < 0 || pBall->nSurface >= 156) return 0;
    pSurface = &gSurfaceTypes[pBall->nSurface];
    if (fn_800BB028(PIN(pBall), pBall->vPos) > 11.1111107f) {
        if (!fn_8004FF34(pBall->pCourse, vFrom, vTo, vHit, vNormal, &pSurface, &pObj)) return 0;
    } else {
        if (!Ter_CheckForPinCollision(pBall->pCourse, pBall->nPlayer, vFrom, vTo, vHit, vNormal, &pSurface,
                                      &pObj)) {
            return 0;
        }
    }
    if (pSurface == NULL) return 0;
    if (0.375f != pSurface->f1C) {
        pSurface = &gSurfaceTypes[14];
    }
    fn_800BAF04(vNormal, vNormal);
    if (pSurface->f0C >= 0.0f) {
        Vec3Copy(vHit, pBall->vPos);
        pBall->vPos[1] += 0.027055556f;
    }
    pBall->pHitSurface = pSurface;
    pBall->pHitObject = pObj;
    BALL_LANDING_EVENTS(pBall, pSurface, pObj);
    Physics_HandleCollision(pBall, vNormal, pSurface);
    if (pSurface->f0C >= 0.0f) {
        Vec3Copy(pBall->vPos, pBall->vPrev);
        fn_8000C5D4(pBall->vPos, pBall->vVel, 9.25925906e-06f, pBall->vPos);
    }
    return 1;
}

// The lie from the surface class, with LUCK (0..110, players 0..3) in the rolls; a sim or
// lbl_80281DD2 rolls 0 (the kind result):
//   1, 2: lie 1.  3, 18: lie 9 (green).  4: lie 10.  7, 16: lie 13.  8: lie 11.
//   5 (rough): surface 145 is lie 4; else a coin flip, forced good when (r & 127) < LUCK/2 -
//     good: lie 3 / surface 27 (never on course 6); bad: lie 4 / surface 28.
//   11: (r & 127) < LUCK/4 + 16 gives the rough treatment instead of lie 5 / surface 26.
//   6, 20 (sand): lie 6 (clean, surface 35), 7 (surface 36) or 8 (plugged, surface 34),
//     from where and how hard it landed and three LUCK rolls; class 20 is surface 146.
//   12: holed (event 0x21 the first time).  Anything else: lie 17.
// Then a random lie quality (+0x70), +-(Rand_Float - LUCK/200, not below 0) x surface +0x04.
// The cases share code through gotos, as the original's branches do: a roll can move a lie up a
// level (class 11 to the rough; plugged to 7 to clean). Copies of the shared code do not match
// (the rough code copied into case 11: 91.8%).
void Ball_SetLie(Ball* pBall, SurfaceType* pSurface) {
    SurfaceType* pLie;
    u32          uLuck;
    u32          r;
    int          nLuck;
    f32          f;
    u32          rSign;
    pBall->nSurface = fn_80050BEC(pSurface);
    uLuck = 0;
    if (pBall->nPlayer >= 0 && pBall->nPlayer <= 3) {
        nLuck = (s8)Golfer_GetAttribute(&gPlayers[pBall->nPlayer], ATTR_LUCK, ATTR_TOTAL);
        if (nLuck < 0) {
            uLuck = 0;
        } else {
            uLuck = nLuck > 110 ? 110 : nLuck;
        }
    }
    switch (pSurface->nClass) {
    case 1:
    case 2:
        pBall->nLie = LIE_FAIRWAY_e;
        break;
    case 3:
    case 18:
        pBall->nLie = LIE_GREEN_e;
        break;
    case 4:
        pBall->nLie = LIE_FRINGE_e;
        break;
    case 5:
        if (pBall->nSurface == 145) {
            pBall->nLie = LIE_ROUGH_e;
            break;
        }
        if (gSimulating || lbl_80281DD2) {
            r = 0;
        } else {
            r = Rand_Next(0);
        }
    rough:
        if ((r & 127) < uLuck / 2) {
            r = 0;
        }
        if (!(r & 1) && gpGame != NULL && Game_GetCourse() != 6) {
            pBall->nLie     = LIE_ROUGH_HIGH_e;
            pBall->nSurface = 27;
        } else {
            pBall->nLie     = LIE_ROUGH_e;
            pBall->nSurface = 28;
        }
        break;
    case 11:
        if (gSimulating || lbl_80281DD2) {
            r = 0;
        } else {
            r = Rand_Next(0);
        }
        if ((r & 127) < uLuck / 4 + 16) {
            r >>= 8;
            goto rough;  // fake match: shared lie code, as in the original (see Ball_SetLie)
        }
        pBall->nLie     = LIE_THICK_ROUGH_e;
        pBall->nSurface = 26;
        break;
    case 6:
    case 20:
        if (gSimulating || lbl_80281DD2) {
            r = 0;
        } else {
            r = Rand_Next(0);
        }
        if (!pBall->bGotFirstSandPos
            || fabsf(pBall->vPos[0] - pBall->vFirstSandPos[0]) >= 0.16666667f ||
            fabsf(pBall->vPos[1] - pBall->vFirstSandPos[1]) >= 0.16666667f ||
            fabsf(pBall->vPos[2] - pBall->vFirstSandPos[2]) >= 0.16666667f
            || pBall->fFirstSandVMag < 5.0f) {
            if ((r & 127) < 16 - uLuck / 16) {
                r >>= 8;
                goto sandC;  // fake match: shared lie code, as in the original (see Ball_SetLie)
            }
        sandClean:
            pBall->nLie     = LIE_SAND_HIGH_e;
            pBall->nSurface = 35;
            goto sandEnd;  // fake match: shared lie code, as in the original (see Ball_SetLie)
        }
        if (!(pBall->fFirstSandVMag < 6.0f)) {
            goto sandC;  // fake match: shared lie code, as in the original (see Ball_SetLie)
        }
    sandB:
        if ((r & 127) < uLuck / 4) {
            goto sandClean;  // fake match: shared lie code, as in the original (see Ball_SetLie)
        }
        pBall->nLie     = LIE_SAND_MEDIUM_e;
        pBall->nSurface = 36;
        goto sandEnd;  // fake match: shared lie code, as in the original (see Ball_SetLie)
    sandC:
        if ((r & 127) < uLuck / 4 + 16) {
            r >>= 8;
            goto sandB;  // fake match: shared lie code, as in the original (see Ball_SetLie)
        }
        pBall->nLie     = LIE_SAND_DEEP_e;
        pBall->nSurface = 34;
    sandEnd:
        if (pSurface->nClass == 20) {
            pBall->nSurface = 146;
        }
        break;
    case 7:
    case 16:
        pBall->nLie = LIE_WATER_e;
        break;
    case 8:
        pBall->nLie = LIE_CARTPATH_e;
        break;
    case 12:
        pBall->bHoled = 1;
        if (pBall->nLie != LIE_INCUP_e) {
            pBall->nLie = LIE_INCUP_e;
            if (pBall->nPlayer >= 0) {
                EVENT_Trigger(pBall->nPlayer, 0x21, pBall, !gSimulating);
            }
        }
        break;
    default:
        pBall->nLie = LIE_MISC_e;
        break;
    }
    pBall->n6C = 0;
    pBall->f70 = 0.0f;
    if (pBall->nSurface >= 0 && pBall->nSurface < 156) {
        pLie = &gSurfaceTypes[pBall->nSurface];
        if (pLie->f04) {
            if (gSimulating || lbl_80281DD2) {
                f = 0.0f;
                rSign = 0;
            } else {
                rSign = Rand_Next(0);
                f = Rand_Float(0) - 0.5f * (0.01f * uLuck);
                if (f < 0.0f) {
                    f = 0.0f;
                }
                f *= pLie->f04;
            }
            if (rSign & 1) {
                f = -f;
            }
            pBall->f70 = f;
        }
    }
}

// The stick spin's weight, 1 - nSolidCollideCount (an inline in the original: written out
// twice, the registers come out differently).
static inline f32 Ball_SpinKeep(Ball* pBall) {
    return (f32)(1 - pBall->nSolidCollideCount);
}

// The ball has hit something (a surface, or an object as surface 13). A tree (class 17) tilts
// the hit normal by 12..19 degrees on two axes - negative unless the roll's low 5 bits are 0,
// so almost always negative; a sim, lbl_80281DD2, slot 4 or a perfect shot rolls 0 (+12, +12).
// The ball is put at the hit point (backed off along its velocity on a solid surface), a
// fraction of the tick used is returned in *pFrac, a surface-108 hit is a hazard, and gravity is
// given back for the part of the tick not flown. First bounce of a shot in flight: event 0x1D,
// and the spin stick's input becomes the ball's spin (backspin x (1.9 - green setting), side x
// (1.95 - green setting)), turned to the direction of travel; event 0x1F.
u8 Physics_ProcessCollision(Ball* pBall, f32* pHit, f32* pNormal, SurfaceType* pSurface, TerObject* pObj,
                            f32* pFrac, f32 fTicks) {
    f32 vSpin[4];
    f32 fSin, fCos;
    s32 r;
    int nA, nB;
    f32 fPrev, fNow, fBack, fAngle, fS, fC;
    if (pSurface->nClass == 17) {
        if (gSimulating || lbl_80281DD2 || pBall->nPlayer == 4 ||
            (pBall->nPlayer >= 0 && pBall->nPlayer <= 3 && gPlayers[pBall->nPlayer].bPerfect)) {
            r = 0;
        } else {
            r = Rand_Next(0);
        }
        nA = (r & 7) + 12;
        if (r & 31) {
            nA = -nA;
        }
        r >>= 8;
        nB = (r & 7) + 12;
        if (r & 31) {
            nB = -nB;
        }
        fn_800BAF04(pNormal, pNormal);
        fn_80055E28(0.017453292f * nA, &fSin, &fCos);
        fn_80055D70(&pNormal[0], &pNormal[1], fSin, fCos);
        fn_80055E28(0.017453292f * nB, &fSin, &fCos);
        fn_80055D70(&pNormal[2], &pNormal[1], fSin, fCos);
    }
    fn_800BAF04(pNormal, pNormal);
    pHit[1] += BALL_RADIUS;
    if (pNormal[1] > 0.0f) {
        pHit[1] += 0.0013888889f;
    }
    fPrev = Vec_Distance(pBall->vPrev, pBall->vPos);
    fNow  = Vec_Distance(pBall->vPrev, pHit);
    if (fPrev <= 2.77777799e-05f) {
        fPrev = 2.77777799e-05f;
    }
    if (fPrev < fNow) {
        fPrev = 2.77777799e-05f + fNow;
    }
    if (fPrev != 0.0f) {
        *pFrac = fNow / fPrev;
        if (*pFrac > 0.97f) {
            *pFrac = 0.97f;
        }
    } else {
        *pFrac = 0.97f;
    }
    Vec3Copy(pHit, pBall->vPos);
    if (pSurface->f0C >= 0.0f) {
        fn_8000C5D4(pBall->vPos, pBall->vVel, -9.25925906e-06f, pBall->vPos);
    }
    if (pSurface->nClass == 19 && pSurface == &gSurfaceTypes[108]) {
        Physics_OutOfBounds(pBall, 1);
        return 0;
    }
    if (pNormal[1] >= 0.0f && pBall->vVel[1] < 0.0f) {
        pBall->vVel[1] += (1.05f - *pFrac) * (0.107170001f * fTicks);
        if (pBall->vVel[1] >= 0.0f) {
            pBall->vVel[1] = -0.000178616669f;
        }
    }
    pBall->pHitSurface = pSurface;
    pBall->pHitObject  = pObj;
    if (pSurface->f0C >= 0.0f && !pBall->b99 && pBall->nPlayer >= 0 && pBall->nState == 2) {
        if (!pBall->b99) {
            pBall->b99 = 1;
            EVENT_Trigger(pBall->nPlayer, 0x1D, pBall, !gSimulating);
        }
        if (pBall->fSpinX != 0.0f || pBall->fSpinY != 0.0f) {
            fBack = pBall->fSpinY;
            if (fBack < 0.0f) {
                fBack *= 1.9f - gGreenSpeedMul[gGreenSpeedSetting];
            }
            vSpin[0] = fBack * Ball_SpinKeep(pBall);
            vSpin[1] = 0.0f;
            fPrev = pBall->fSpinX;
            fPrev *= 1.95f - gGreenSpeedMul[gGreenSpeedSetting];
            vSpin[2] = fPrev * Ball_SpinKeep(pBall);
            fAngle = -fn_8000AD78(pBall->vVel[0], pBall->vVel[2]);
            fS = fn_800095F0(fAngle);
            fC = fn_80009638(fAngle);
            fn_80055D70(&vSpin[0], &vSpin[2], fS, fC);
            Vec3Copy(vSpin, pBall->vSpin);
        }
        pBall->b9B = 1;
        EVENT_Trigger(pBall->nPlayer, 0x1F, pBall, !gSimulating);
    }
    return 1;
}

// The flagstick (object kind 11; any other object is a plain hit). Not while it is still
// swaying. A hit is within the pole's radius (at least 1 in) plus 2 ft and within
// 8 in of it along z; the ball is stopped against it along z (normal +-z), and a real ball sets
// the flag swaying by how far off centre and how fast it hit.
u8 fn_80053E98(Ball* pBall, void* pv, f32* pHit, f32* pNormal) {
    HitObject* pObj = (HitObject*)pv;   // fake match: typed here, not in the parameter (that: 95.5%)
    f32 vPole[4];
    f32 fRadius;
    f32 fDZ, fDist2;
    if (pObj->nKind != 11) return 1;
    if (0.0f != pObj->fSway) return 0;
    fDist2 = (pHit[0] - pBall->vPos[0]) * (pHit[0] - pBall->vPos[0])
             + (pHit[2] - pBall->vPos[2]) * (pHit[2] - pBall->vPos[2]);
    fn_800B1AB0(pObj, vPole, &fRadius);
    if (fRadius < 0.027777778f || fDist2 >= fRadius * fRadius + 0.444444478f) return 0;
    fDZ = vPole[2] - pBall->vPos[2];
    if (fabsf(fDZ) > 0.222222224f) return 0;
    pNormal[1] = 0.0f;
    pNormal[0] = 0.0f;
    if (fDZ <= 0.0f) {
        pNormal[2] = 1.0f;
        pHit[2]    = vPole[2] - 0.222222224f;
    } else {
        pNormal[2] = -1.0f;
        pHit[2]    = 0.222222224f + vPole[2];
    }
    pHit[0] = pBall->vPos[0];
    pHit[1] = pBall->vPos[1];
    if (!gSimulating) {
        fDZ = 18.849556f;
        fDZ = fDZ * ((vPole[0] - pBall->vPos[0]) / fRadius);
        fDZ = fDZ * (pBall->fSpeed / 29.333334f);
        pObj->fSway = -fDZ;
    }
    return 1;
}

// Did the ball hit something (a tree, an object) between last tick and this one? fn_800B1B18
// sweeps the path, fn_80053E98 and Physics_ProcessCollision decide; then event 0x27 and the bounce off it
// as surface 13. Returns 1 on a hit.
u8 fn_80054040(Ball* pBall, f32 fTicks) {
    f32          vHit[4];
    f32          vNormal[4];
    f32          vFrom[4];
    f32          vTo[4];
    HitObject*   pWhat;
    f32          fFrac;
    SurfaceType* pSurface;
    Vec3Copy(pBall->vPrev, vFrom);
    vFrom[1] -= BALL_RADIUS;
    Vec3Copy(pBall->vPos, vTo);
    vTo[1] -= BALL_RADIUS;
    if (!fn_800B1B18(pBall->nPlayer, vTo, vFrom, vHit, vNormal, &pWhat)) return 0;
    if (!fn_80053E98(pBall, pWhat, vHit, vNormal)) return 0;
    pSurface = &gSurfaceTypes[13];
    if (!Physics_ProcessCollision(pBall, vHit, vNormal, pSurface, NULL, &fFrac, fTicks)) return 0;
    if (pBall->nPlayer >= 0) {
        pBall->pHitActor = pWhat;
        EVENT_Trigger(pBall->nPlayer, 0x27, pBall, !gSimulating);
    }
    Physics_HandleCollision(pBall, vNormal, pSurface);
    return 1;
}

// Keep a ball that is on the ground on the ground: find the ground under it (no ground at all:
// hazard). Unless settling, fn_80053240 may throw it back into the air. More than 1.68 in above
// the ground: take the other ground height if it is within 2.5 in, else look again once, else
// (unless settling) it is in the air. Then sit it on the ground.
void Physics_FixBallHeight(Ball* pBall, u8 bSettle, f32 fTicks) {
    u8  bRetried;
    f32 fGround;
    if (pBall->bHoled || pBall->nState == 2) return;
    bRetried = 0;
    for (;;) {
        if (!lbl_80281DE4) {
            Ter_GetEnclosingGroundHeight(pBall->pCourse, pBall->vPos, &lbl_80281DE0, &lbl_80281DDC);
        }
        fGround = lbl_80281DE0;
        if (fGround < -60000.0f) {
            fGround = lbl_80281DDC;
            if (fGround < -60000.0f) {
                Physics_OutOfBounds(pBall, 1);
                return;
            }
        }
        if (!bSettle && fn_80053240(pBall, fTicks)) {
            pBall->nState = 2;
            return;
        }
        if (pBall->vPos[1] - fGround > 0.0466666669f) {
            if (pBall->nSurface != 98 && !(lbl_80281DDC < -60000.0f)
                && lbl_80281DDC - pBall->vPos[1] <= 0.07f) {
                fGround = lbl_80281DDC;
            } else if (!bRetried) {
                bRetried     = 1;
                lbl_80281DE4 = 0;
                continue;
            } else if (!bSettle) {
                pBall->nState = 2;
                return;
            }
        }
        break;
    }
    pBall->vPos[1] = 0.0013888889f + (BALL_RADIUS + fGround);
}

// The ball has stopped. Holed: state 1, lie 12, events 0x21 and 0x20. Otherwise it is settled
// (Physics_FixBallHeight) and given the lie of the surface under it - a surface whose +0x1C is not 0.375
// counts as surface 14 - or, with no surface at all, it is a hazard.
void Ball_Stop(Ball* pBall) {
    SurfaceType* pSurface;
    if (pBall->bHoled) {
        pBall->nState = 1;
        pBall->nLie   = LIE_INCUP_e;
        pBall->n6C    = 0;
        if (pBall->nPlayer >= 0) {
            EVENT_Trigger(pBall->nPlayer, 0x21, pBall, !gSimulating);
            EVENT_Trigger(pBall->nPlayer, 0x20, pBall, !gSimulating);
        }
        return;
    }
    Physics_FixBallHeight(pBall, 1, 0.0f);
    pSurface = Ter_GetSupportingWorldMaterial(pBall->pCourse, pBall->vPos);
    if (pSurface == NULL) {
        Physics_OutOfBounds(pBall, 1);
        return;
    }
    if (0.375f != pSurface->f1C) {
        pSurface = &gSurfaceTypes[14];
    }
    pBall->nState = 1;
    Ball_SetLie(pBall, pSurface);
    if (pBall->nPlayer >= 0) {
        EVENT_Trigger(pBall->nPlayer, 0x20, pBall, !gSimulating);
    }
}

// A ball in the air meets the ground. Out of bounds (600 yd) is a hazard. fn_8004E558 finds
// the hit; Physics_ProcessCollision handles it; landing events; the cup (surface 90) kills spin; the bounce
// (Physics_HandleCollision) - on sand (class 6) the first landing point and impact are kept for the lie. On
// a cup surface more than 2.5 in below the pin: holed. Then the ball flies on for the rest of the
// tick (fn_8004EE20 for a second hit). Water (class 7 or 16): surfaces 47, 41 and 104 let a ball
// that bounces up faster than 2.93 and moves on more than 1.71 x that (and over 5.87) skip, its
// rise quartered; anything else is a hazard.
u8 Ball_Collide(Ball* pBall, f32 fTicks) {
    f32          vHit[4];
    f32          vNormal[4];
    f32          vFrom[4];
    f32          vTo[4];
    f32          fFrac;
    SurfaceType* pSurface;
    TerObject*   pObj;
    u8           bOut;
    int          nIndex;
    f32          fImpact, fAcross;
    if (pBall->pCourse == NULL) return 0;
    Vec3Copy(pBall->vPrev, vFrom);
    vFrom[1] -= BALL_RADIUS;
    Vec3Copy(pBall->vPos, vTo);
    vTo[1] -= BALL_RADIUS;
    if (fn_800BB028(vTo, pBall->vStart) > 360000.0f) {
        Physics_OutOfBounds(pBall, 1);
        return 0;
    }
    if (!fn_8004E558(pBall->pCourse, pBall->nPlayer, vFrom, vTo, vHit, vNormal, &pSurface, &pObj, &bOut)) {
        return 0;
    }
    if (pSurface == NULL) return 0;
    if (0.375f != pSurface->f1C) {
        pSurface = &gSurfaceTypes[14];
    }
    if ((pSurface->u34 & 0x10) && pSurface->f0C < 0.0f && bOut <= 1) return 0;
    if (!Physics_ProcessCollision(pBall, vHit, vNormal, pSurface, pObj, &fFrac, fTicks)) return 0;
    BALL_LANDING_EVENTS(pBall, pSurface, pObj);
    nIndex = fn_80050BEC(pSurface);
    if (nIndex == 90) {
        pBall->vSpin[0] = 0.0f;
        pBall->vSpin[1] = 0.0f;
        pBall->vSpin[2] = 0.0f;
    }
    fImpact = Physics_HandleCollision(pBall, vNormal, pSurface);
    if (pSurface->nClass == 6 && !pBall->bGotFirstSandPos) {
        pBall->bGotFirstSandPos       = 1;
        pBall->fFirstSandVMag = fn_80009680(fImpact);
        Vec3Copy(pBall->vPos, pBall->vFirstSandPos);
    }
    if (pSurface->nClass == 12 || pSurface->nClass == 18 ||
        (nIndex == 90 && Vec_Distance(pBall->vPos, PIN(pBall)) < 2.0f)) {
        if (pBall->pCourse->pin[Game_CurrentPinSet()].y - pBall->vPos[1] > 0.0694444478f) {
            Ball_Holed(pBall);
            return 1;
        }
    }
    if (pBall->nState == 4) {
        if (pSurface->nClass == 12) {
            pBall->bHoled = 1;
            Ball_Stop(pBall);
            return 1;
        }
        pBall->vVel[1] = 0.0293333326f;
    }
    Vec3Copy(pBall->vPos, vFrom);
    vFrom[1] -= BALL_RADIUS;
    Vec3Copy(vFrom, vTo);
    if (vNormal[1] >= 0.0f && pBall->vVel[1] > 0.0f) {
        pBall->vVel[1] -= (1.05f - fFrac) * (0.107170001f * fTicks);
        if (pBall->vVel[1] <= 0.0f) {
            pBall->vVel[1] = 0.000178616669f;
        }
    }
    fn_8000C5D4(vTo, pBall->vVel, ((fTicks - 0.05f) * (1.0f - fFrac)) / 36.0f, vTo);
    if (fn_8004EE20(pBall->pCourse, pBall->nPlayer, vFrom, vTo, vHit, vNormal, &pSurface, &pObj)) {
        Vec3Copy(pBall->vPos, pBall->vPrev);
        fn_8000C5D4(pBall->vPos, pBall->vVel, 9.25925906e-06f, pBall->vPos);
    } else {
        vFrom[1] += BALL_RADIUS;
        Vec3Copy(vFrom, pBall->vPrev);
        vTo[1] += BALL_RADIUS;
        Vec3Copy(vTo, pBall->vPos);
    }
    if (pSurface->nClass == 7 || pSurface->nClass == 16) {
        nIndex = fn_80050BEC(pSurface);
        if ((nIndex == 47 || nIndex == 41 || nIndex == 104) && pBall->vVel[1] > 2.9333334f) {
            fAcross = fn_80009680(pBall->vVel[0] * pBall->vVel[0] + pBall->vVel[2] * pBall->vVel[2]);
            if (fAcross > 1.71f * pBall->vVel[1] && fAcross > 5.86666679f) {
                pBall->vVel[1] *= 0.25f;
                return 1;
            }
        }
        Physics_OutOfBounds(pBall, 1);
        return 0;
    }
    return 1;
}

// Distance from a point to the pin, 1000 when there is no course.
f32 Ball_DistanceToPin(f32* pPos) {
    CourseInfo* pCourse = fn_8000C594();
    if (pCourse != NULL) {
        return Vec_Distance(pPos, &pCourse->pin[Game_CurrentPinSet()].x);
    }
    return 1000.0f;
}

// The shot starts here: remember the surface and the position, and the distance to the pin
// as the closest so far.
void fn_80054A6C(Ball* pBall) {
    pBall->nStartSurface = pBall->nSurface;
    Vec_Copy(pBall->vPos, pBall->vStart);
    pBall->fClosest = Ball_DistanceToPin(pBall->vPos);
}

// The pull toward the cup. Inside 5.5 in of the pin, while the ball is still short of it, a
// ball heading within 30 degrees of the cup (or within 3.5 in whatever its heading) gets
// 0.455 x dt x (pin - ball) added to its velocity - but never on an axis where that would speed
// it up while it already moves faster than 0.6 mph along that axis. A ball crossing over the cup
// faster than 0.75 mph and off line loses up to 67% of its speed instead: the lip.
void Ball_CupPull(Ball* pBall, f32 fDt) {
    f32 vPin[3];
    f32 fDist;

    fDist = Vec_Distance(PIN(pBall), (f32*)pBall);
    if (fDist >= INCHES(5.5f)) return;
    Vec_Copy(PIN(pBall), vPin);
    vPin[1] += BALL_RADIUS;
    {
        f32 fStartDist = Vec_Distance(pBall->vStart, vPin);
        f32 fAngle, fK, fPull;
        if (fStartDist < INCHES(6.0f)) return;
        if (Vec_Distance(pBall->vStart, (f32*)pBall) > fStartDist - INCHES(1.5f)) return;

        fAngle = fn_8000AD78(pBall->vPos[0] - pBall->vPrev[0], pBall->vPos[2] - pBall->vPrev[2]);
        fAngle = fabsf(fAngle - fn_8000AD78(vPin[0] - pBall->vPos[0], vPin[2] - pBall->vPos[2]));
        while (fAngle > 3.14159265f) {
            fAngle -= 3.14159265f;
        }
        if (fDist < 0.0625f) {
            if (fAngle > DEG(30.0f) && pBall->fSpeed > MPH(0.75f)) {
                f32 fSlow = 1.0f - 16.0f * (0.67f * fDist);
                pBall->vVel[0] *= fSlow;
                pBall->vVel[2] *= fSlow;
            }
            return;
        }
        if (fAngle < DEG(30.0f) || fDist < INCHES(3.5f)) {
            fK    = (CUP_DIAMETER * 4.25f) * fDt;   // the bits are exactly this product (0x3EE933B1)
            fPull = fK * (vPin[0] - pBall->vPos[0]);
            if ((pBall->vVel[0] < 0.0f && fPull < 0.0f) || (pBall->vVel[0] > 0.0f && fPull > 0.0f)) {
                if (fabsf(pBall->vVel[0]) > MPH(0.6f)) {
                    fPull = 0.0f;
                }
            }
            pBall->vVel[0] += fPull;
            fPull = fK * (vPin[2] - pBall->vPos[2]);
            if ((pBall->vVel[2] < 0.0f && fPull < 0.0f) || (pBall->vVel[2] > 0.0f && fPull > 0.0f)) {
                if (fabsf(pBall->vVel[2]) > MPH(0.6f)) {
                    fPull = 0.0f;
                }
            }
            pBall->vVel[2] += fPull;
        }
    }
}

// State 4, rolling. Holed when on a cup surface (class 12 or 18, or surface 90 within 2 yd of
// the pin) more than 2 in below the pin. Otherwise the velocity is laid onto the ground plane
// at its speed, then:
// - break: the slope's sideways part (surface +0x1C, x 0.6 on a green) turns the velocity by
//   slope / (0.457 x spin) a tick about an axis built from the spin and the slope - so a slow
//   ball breaks more;
// - gravity along the direction of travel, x 5/7 (a rolling sphere);
// - the cup pull (real ball; sims only with gSimFullCup);
// - rolling friction: normal x 0.0766 x surface +0x20 (capped on slopes over 30 degrees),
//   x 0.575 on a green, x the course settings, x the tick; when the speed is below it the ball
//   stops;
// - spin set to pure roll.
void Ball_GroundContact(Ball* pBall, f32 fTicks) {
    f32          vNormal[4];
    f32          vAccel[4];
    f32          vTmp[4];
    f32          vDown[4];
    f32          vDir[4];
    f32          vAxis[4];
    SurfaceType* pSurface;
    f32          fSin, fCos;
    f32          fA, fB;
    f32          fTurn, fZ, fX, fLen, fAngle, fK, fV, fRough;
    u8           bFlip;
    if (!Physics_GetSurfaceInfo(pBall, &pSurface, vNormal)) return;
    pBall->nSurface = fn_80050BEC(pSurface);
    if (pSurface->nClass == 12 || pSurface->nClass == 18 ||
        (pBall->nSurface == 90 && Vec_Distance(pBall->vPos, PIN(pBall)) < 2.0f)) {
        if (pBall->pCourse->pin[Game_CurrentPinSet()].y - pBall->vPos[1] > 0.055555556f) {
            Ball_Holed(pBall);
            return;
        }
    }
    fn_800BAF04(vNormal, vNormal);
    fn_8000C5D4(pBall->vVel, vNormal, -fn_8000C5FC(pBall->vVel, vNormal), vTmp);
    fn_800BAF04(vTmp, vDir);
    fn_8001EF34(vDir, fn_80009680(fn_80009744(pBall->vVel)), pBall->vVel);
    fn_8001EF34(vNormal, -0.839999974f, vDown);
    fZ   = -0.173615396f * vDown[2];
    fX   = 0.173615396f * vDown[0];
    fLen = fn_80009680(fZ * fZ + fX * fX);
    if (fLen != 0.0f) {
        fK = fabsf(fZ * vDir[0] + fX * vDir[2]) / fLen;
        fX *= fK;
        fZ *= fK;
        fLen = pSurface->f1C * (f32)fn_80009680(fZ * fZ + fX * fX);
        if (pSurface->nClass == 3) {
            fLen *= 0.6f;
        }
        vec4flt_CrossProduct(pBall->vVel, vDown, pBall->vSpin);
        fn_8001EF34(pBall->vSpin, 1.41723347f, pBall->vSpin);
        fAngle = 0.45722881f * (f32)fn_80009680(fn_80009744(pBall->vSpin));
        fTurn = 0.0f;
        if (fTurn != fAngle) {
            fTurn = fLen / fAngle;
        }
        vAxis[0] = pBall->vSpin[1] * fX;
        vAxis[1] = pBall->vSpin[2] * fZ - pBall->vSpin[0] * fX;
        vAxis[2] = -(pBall->vSpin[1] * fZ);
        vAxis[3] = 1.0f;
        if (vAxis[1] < 0.0f) {
            fn_80055EF8(vAxis, vAxis);
            bFlip = 1;
        } else {
            bFlip = 0;
        }
        fX = fn_8000AD78(vAxis[0], vAxis[1]);
        fn_80055E28(fX, &fSin, &fCos);
        fA   = vAxis[0];
        fAngle = vAxis[2];
        fB   = vAxis[1];
        fn_80055D70(&fA, &fB, fSin, fCos);
        fn_80055D70(&pBall->vVel[0], &pBall->vVel[1], fSin, fCos);
        fAngle = -fn_8000AD78(fAngle, fB);
        fn_80055E28(fAngle, &fSin, &fCos);
        fn_80055D70(&pBall->vVel[1], &pBall->vVel[2], fSin, fCos);
        if (!bFlip) {
            fTurn = -fTurn;
        }
        fn_80055E28(fTurn, &fSin, &fCos);
        fn_80055D70(&pBall->vVel[0], &pBall->vVel[2], fSin, fCos);
        fn_80055E28(-fAngle, &fSin, &fCos);
        fn_80055D70(&pBall->vVel[1], &pBall->vVel[2], fSin, fCos);
        fn_80055E28(-fX, &fSin, &fCos);
        fn_80055D70(&pBall->vVel[0], &pBall->vVel[1], fSin, fCos);
    }
    fn_8001EF34(vDir, -(0.714285731f * (0.107170001f * vDir[1])), vAccel);
    fn_8000C5D4(pBall->vVel, vAccel, fTicks, pBall->vVel);
    if (!gSimulating || gSimFullCup) {
        Ball_CupPull(pBall, fTicks);
    }
    fRough = pSurface->f20;
    if (vNormal[1] < 0.866f) {
        fRough *= 0.5f * vNormal[1];
        if (fRough > 0.14f) {
            fRough = 0.14f;
        }
    }
    fTurn = vNormal[1] * (0.0765499994f * fRough);
    if (pSurface->nClass == 3) {
        fTurn *= 0.575f;
    }
    if (pSurface->nClass == 3 || pSurface->nClass == 4 || pSurface->nClass == 2) {
        fV = gTurfSpeedMul[gTurfSpeed];
        if (fV > 1.0f) {
            fTurn *= 2.0f - (1.16f * (fV - 1.0f) + 1.0f);
        } else if (fV < 1.0f) {
            fTurn *= 0.55f * -fV + 2.0f;
        } else {
            fTurn *= 2.0f - fV;
        }
    }
    if (pSurface->nClass == 3) {
        fTurn *= 2.0f - gGreenSpeedMul[gGreenSpeedSetting];
    }
    if (pSurface->nClass == 2) {
        fTurn *= 2.0f - gFairwaySpeedMul[gFairwaySetting];
    }
    if (pSurface->nClass == 5) {
        fTurn *= 2.0f - gRoughMul[gRoughSetting];
    }
    fTurn *= fTicks;
    if ((f32)fn_80009680(fn_80009744(pBall->vVel)) < fTurn) {
        Ball_Stop(pBall);
        return;
    }
    fB = pBall->vVel[1];
    fTurn = -fTurn;
    fn_8000C5D4(pBall->vVel, vDir, fTurn, pBall->vVel);
    if (pBall->nSurface == 98) {
        pBall->vVel[1] = 0.9f * fB;
    }
    pBall->vSpin[0] = -1.41723347f * (pBall->vVel[2] * vDown[1]);
    pBall->vSpin[1] = 0.0f;
    pBall->vSpin[2] = 1.41723347f * (pBall->vVel[0] * vDown[1]);
    if (pBall->nPlayer >= 0) {
        PsBallFx_TriggerTrail(pBall, pBall->nPlayer);
    }
}

// The height of a ball in the air above the ground (0 when not in the air). With no ground
// under it: within 8 ft of the pin it is set on the other ground height; if that is less than
// 1.68 in above it, that is the ground; else above the course floor it is still in play
// (surface -1), and below it a hazard.
f32 fn_80055324(Ball* pBall) {
    f32          vNormal[4];
    f32          vNormal2[4];
    f32          vPin[4];
    f32          fHeight;
    f32          fHeight2;
    SurfaceType* pSurface;
    SurfaceType* pSurface2;
    if (pBall->nState != 2) return 0.0f;
    Ter_GetEnclosingGroundData(pBall->pCourse, pBall->vPos, &fHeight, &pSurface, vNormal, &fHeight2,
                               &pSurface2, vNormal2);
    if (fHeight < -60000.0f) {
        Vec_Copy(PIN(pBall), vPin);
        vPin[1] += BALL_RADIUS;
        if (Vec_Distance(vPin, pBall->vPos) < 2.66666675f) {
            pBall->vPos[1]  = 0.0013888889f + (BALL_RADIUS + fHeight2);
            pBall->nSurface = -1;
            return 0.0f;
        }
        if (!(fHeight2 < -60000.0f) && fHeight2 - pBall->vPos[1] < 0.0466666669f) {
            pBall->vPos[1] = 0.0013888889f + (BALL_RADIUS + fHeight2);
            fHeight  = fHeight2;
            pSurface = pSurface2;
        } else {
            if (pBall->vPos[1] > fn_8000C594()->fFloor) {
                pBall->nSurface = -1;
                return pBall->vPos[1] - fn_8000C594()->fFloor;
            }
            Physics_OutOfBounds(pBall, 1);
            return 0.0f;
        }
    }
    pBall->nSurface = fn_80050BEC(pSurface);
    return pBall->vPos[1] - fHeight;
}

// One tick: the state's step (air, rolling, bouncing), then move by velocity / 36. A ball
// still in play is kept on the ground or collided, checked against objects, and every 4 s
// (0.0167 a tick) it must have moved 4 in, or it stops. Then its speed (x 60 / 36), its height
// and its closest approach to the pin.
void Ball_Tick(Ball* pBall, f32 fTicks) {
    f32 fDist;
    lbl_80281DE4 = 0;
    switch (pBall->nState) {
    case 2:
        Ball_FlightStep(pBall, fTicks);
        break;
    case 3:
        fn_80052268(pBall, fTicks);
        break;
    case 4:
        Ball_GroundContact(pBall, fTicks);
        break;
    default:
        goto done;  // fake match: straight to the speed update (the rest in an if: 88.7%)
    }
    Vec3Copy(pBall->vPos, pBall->vPrev);
    fn_8000C5D4(pBall->vPos, pBall->vVel, fTicks / 36.0f, pBall->vPos);
    if (pBall->nState != 1 && pBall->nState != 5) {
        if (!pBall->bHoled) {
            if (pBall->nState != 2) {
                Physics_FixBallHeight(pBall, 0, fTicks);
            } else {
                Ball_Collide(pBall, fTicks);
            }
            fn_80054040(pBall, fTicks);
        }
        pBall->fTimeSinceLastCheck += 0.0166666675f * fTicks;
        if (pBall->fTimeSinceLastCheck > 4.0f) {
            fDist = Vec_Distance(pBall->vStart, pBall->vPos);
            if (fabsf(fDist - pBall->fLastDistFromInitShotPos) < 0.111111112f) {
                Ball_Stop(pBall);
            } else {
                pBall->fLastDistFromInitShotPos = fDist;
                pBall->fTimeSinceLastCheck = 0.0f;
            }
        }
    }
done:
    lbl_80281DE4 = 0;
    if (pBall->bHoled || pBall->nState == 1 || pBall->nState == 5) {
        pBall->fSpeed = 0.0f;
    } else {
        fDist = fn_80009680(fn_80009744(pBall->vVel));
        pBall->fSpeed  = 60.0f * (fDist / 36.0f);
        pBall->fHeight = fn_80055324(pBall);
        fDist = Ball_DistanceToPin(pBall->vPos);
        if (fDist < pBall->fClosest) {
            pBall->fClosest = fDist;
        }
    }
}

// Step a ball by nMs milliseconds (at least 20; one tick is 20 ms) unless it is placed, stopped
// or in a hazard. For players 0..3, Ter_CheckForDropLocation may record the position in one of two tables.
int Physics_Simulate(Ball* pBall, int nMs) {
    u8 bA, bB;
    if (nMs < 20) return 0;
    if (pBall->nState == 0 || pBall->nState == 1 || pBall->nState == 5) return 0;
    Ball_Tick(pBall, (f32)nMs / 20.0f);
    if (pBall->nPlayer >= 0 && pBall->nPlayer <= 3) {
        if (Ter_IsValidDropSurface(pBall->nSurface)) {
            Ter_CheckForDropLocation(pBall->pCourse, pBall->vPos, 1, &bA, &bB, NULL);
        } else {
            Ter_CheckForDropLocation(pBall->pCourse, pBall->vPos, 0, &bA, &bB, NULL);
        }
        if (bA) {
            Vec3Copy(pBall->vPos, lbl_801D5888[pBall->nPlayer]);
        }
        if (bB) {
            Vec3Copy(pBall->vPos, lbl_801D58C8[pBall->nPlayer]);
        }
    }
    return 0;
}

// Run a ball ahead silently: ticks of fTick for fSeconds x 60 ticks' worth, or until it is no
// longer in the air or rolling.
void Ball_SimSeconds(Ball* pBall, f32 fSeconds, f32 fTick) {
    gSimulating = 1;
    fSeconds *= 60.0f;
    do {
        Ball_Tick(pBall, fTick);
        fSeconds -= fTick;
    } while (fSeconds > 0.0f && (pBall->nState == 2 || pBall->nState == 3 || pBall->nState == 4));
    gSimulating = 0;
}

void Ball_SimStep(Ball* pBall, f32 fSeconds, f32 fTick) {
    Ball_SimSeconds(pBall, fSeconds, fTick);
}

// Put the ball on the ground at a point: find the ground under it (from 2 in up), sit the ball
// on it (radius + 0.05 in), placed, with the lie of the surface there and f70 halved.
u8 Physics_DropBall(Ball* pBall, f32* pPos) {
    f32          v[4];
    SurfaceType* pSurface;
    f32          fGround;
    Vec3Copy(pPos, v);
    v[1] += INCHES(2.0f);
    fGround = fn_8004D620(pBall->pCourse, v);
    if (fGround < -60000.0f) {
        fGround = fn_8004D5C0(pBall->pCourse, v);
        if (fGround < -60000.0f) return 0;
    }
    v[1] = 0.0013888889f + (BALL_RADIUS + fGround);
    pSurface = Ter_GetSupportingWorldMaterial(pBall->pCourse, v);
    if (pSurface == NULL) return 0;
    Vec3Copy(v, pBall->vPos);
    Vec3Copy(v, pBall->vPrev);
    pBall->nState = 0;
    pBall->bHoled = 0;
    Ball_SetLie(pBall, pSurface);
    pBall->f70 = 0.5f * pBall->f70;
    if (pBall->nPlayer >= 0 && pBall->nPlayer <= 3) {
        Vec3Copy(pBall->vPos, lbl_801D5888[pBall->nPlayer]);
        Vec3Copy(pBall->vPos, lbl_801D58C8[pBall->nPlayer]);
    }
    return 1;
}

// Drop the ball from a point: at least 0.25 in above the ground, its height remembered, and it
// is in the air (state 2) with no velocity or spin.
u8 fn_800559BC(Ball* pBall, f32* pPos) {
    f32 v[4];
    f32 fGround;
    Vec3Copy(pPos, v);
    fGround = fn_8004D620(pBall->pCourse, v);
    if (fGround < -60000.0f) return 0;
    if (v[1] <= fGround) {
        v[1] = 0.0069444445f + fGround;
    }
    pBall->fHeight = v[1] - fGround;
    v[1] += 0.027055556f;
    Vec3Copy(v, pBall->vPos);
    Vec3Copy(v, pBall->vPrev);
    pBall->nState  = 2;
    pBall->bHoled  = 0;
    pBall->vVel[0] = 0.0f;
    pBall->vVel[1] = 0.0f;
    pBall->vVel[2] = 0.0f;
    pBall->f2C     = 0.0f;
    pBall->vSpin[0] = 0.0f;
    pBall->vSpin[1] = 0.0f;
    pBall->vSpin[2] = 0.0f;
    pBall->f3C     = 0.0f;
    pBall->fSpeed  = 0.0f;
    pBall->nSolidCollideCount     = 0;
    pBall->nCollideCount     = 0;
    pBall->bGotFirstSandPos   = 0;
    return 1;
}

// A fresh ball for a player at a point: everything cleared (surface 45), the course looked up,
// and the ball set on the ground there (radius + 1 in; the point itself if there is no
// ground). Returns 1 when there is no course.
u8 fn_80055AA8(Ball* pBall, f32* pPos, int nPlayer) {
    f32 fGround;
    Vec_Copy(pPos, pBall->vPos);
    pBall->nState        = 0;
    pBall->nLie          = LIE_TEE_e;
    pBall->n6C           = 0;
    pBall->fHeight       = 0.0f;
    pBall->f70           = 0.0f;
    pBall->nStartSurface = 45;
    pBall->nSurface      = 45;
    pBall->nPlayer       = nPlayer;
    pBall->nSolidCollideCount           = 0;
    pBall->nCollideCount           = 0;
    pBall->bHoled        = 0;
    pBall->b99           = 0;
    pBall->b9B           = 0;
    pBall->bHitTopArc           = 0;
    pBall->fSpeed        = 0.0f;
    pBall->vSpin[0]      = 0.0f;
    pBall->vSpin[1]      = 0.0f;
    pBall->vSpin[2]      = 0.0f;
    pBall->f3C           = 0.0f;
    pBall->vVel[0]       = 0.0f;
    pBall->vVel[1]       = 0.0f;
    pBall->vVel[2]       = 0.0f;
    pBall->f2C           = 0.0f;
    pBall->pCourse = fn_8000C594();
    if (pBall->pCourse == NULL) return 1;
    pBall->vPos[1] += 0.053444445f;
    fGround = fn_8004D620(pBall->pCourse, pBall->vPos);
    if (fGround < -60000.0f) {
        fGround = fn_8004D5C0(pBall->pCourse, pBall->vPos);
        if (fGround < -60000.0f) {
            fGround = pBall->vPos[1];
        }
    }
    pBall->vPos[1] = 0.027777778f + (BALL_RADIUS + fGround);
    Vec_Copy(pPos, pBall->vPrev);
    if (nPlayer >= 0 && nPlayer <= 3) {
        Vec_Copy(pPos, lbl_801D58C8[nPlayer]);
        Vec_Copy(pPos, lbl_801D5888[nPlayer]);
    }
    return 0;
}

void fn_80055C1C(u8 b) {
    lbl_80281DD2 = b;
}

// Course setting (0..4, else 2).
void fn_80055C24(int n) {
    if (n < 0 || n >= 5) {
        n = 2;
    }
    gTurfSpeed = n;
}

// Course setting from options +0x18 (0..2, else 0); courses 6 and 15 take it one lower, not
// below 1.
void fn_80055C40(int n) {
    if (n < 0 || n >= 3) {
        n = 0;
    }
    switch (Game_GetCourse()) {
    case 6:
    case 15:
        if (n > 1) {
            n--;
        }
        break;
    }
    gGreenSpeedSetting = n;
}

int fn_80055CA4(void) {
    return gGreenSpeedSetting;
}

// Course setting (0..2, else 0).
void fn_80055CAC(int n) {
    if (n < 0 || n >= 3) {
        n = 0;
    }
    gFairwaySetting = n;
}

int fn_80055CC8(void) {
    return gFairwaySetting;
}

// Course setting from options +0x1C (0..2, else 1); courses 6 and 15 take it one higher, not
// above 2.
void fn_80055CD0(int n) {
    if (n < 0 || n >= 3) {
        n = 1;
    }
    switch (Game_GetCourse()) {
    case 6:
    case 15:
        if (n < 2) {
            n++;
        }
        break;
    }
    gRoughSetting = n;
}

int fn_80055D34(void) {
    return gRoughSetting;
}

void fn_80055D3C(void) {
    gGreenSpeedSetting = 0;
    gFairwaySetting = 0;
    gRoughSetting = 1;
}

void fn_80055D54(void) {
    gTurfSpeed = 2;
    lbl_80281DE4 = 0;
    lbl_80281DD2 = 0;
}

void fn_80055D6C(void) {
}

// Rotate the pair (*pA, *pB) by the angle whose sine and cosine are given.
void fn_80055D70(f32* pA, f32* pB, f32 fSin, f32 fCos) {
    f32 fA = fn_80055E1C(*pA, *pB, fSin, fCos);
    *pB = fn_80055E10(*pB, *pA, fSin, fCos);
    *pA = fA;
}

f32 fn_80055E10(f32 a, f32 b, f32 fSin, f32 fCos) {
    return a * fCos + b * fSin;
}

f32 fn_80055E1C(f32 a, f32 b, f32 fSin, f32 fCos) {
    return a * fCos - b * fSin;
}

void fn_80055E28(f32 fAngle, f32* pSin, f32* pCos) {
    *pSin = fn_800095F0(fAngle);
    *pCos = fn_80009638(fAngle);
}

// b + a into out (three floats)
#ifdef __MWERKS__
asm void fn_80055E7C(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_add f2, f2, f0
    ps_add f3, f3, f1
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_80055E7C(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pB[0] + pA[0];
    pOut[1] = pB[1] + pA[1];
    pOut[2] = pB[2] + pA[2];
}
#endif

// a - b into out (three floats)
#ifdef __MWERKS__
asm void fn_80055EA0(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    psq_l  f2, 0(pB), 0, 0
    psq_l  f3, 8(pB), 1, 0
    ps_sub f2, f0, f2
    ps_sub f3, f1, f3
    psq_st f2, 0(pOut), 0, 0
    psq_st f3, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_80055EA0(f32* pA, f32* pB, f32* pOut) {
    pOut[0] = pA[0] - pB[0];
    pOut[1] = pA[1] - pB[1];
    pOut[2] = pA[2] - pB[2];
}
#endif

// b scaled by a . b into out (b's fourth float included)
#ifdef __MWERKS__
asm void fn_80055EC4(register f32* pA, register f32* pB, register f32* pOut) {
    nofralloc
    psq_l    f0, 0(pA), 0, 0
    psq_l    f1, 8(pA), 1, 0
    psq_l    f2, 0(pB), 0, 0
    psq_l    f3, 8(pB), 0, 0
    ps_mul   f0, f0, f2
    ps_mul   f1, f1, f3
    ps_sum1  f1, f0, f1, f0
    ps_sum0  f1, f1, f1, f1
    ps_muls0 f2, f2, f1
    ps_muls0 f3, f3, f1
    psq_st   f2, 0(pOut), 0, 0
    psq_st   f3, 8(pOut), 0, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_80055EC4(f32* pA, f32* pB, f32* pOut) {
    f32 fDot = pA[2] * pB[2] + (pA[0] * pB[0] + pA[1] * pB[1]);
    pOut[0] = pB[0] * fDot;
    pOut[1] = pB[1] * fDot;
    pOut[2] = pB[2] * fDot;
    pOut[3] = pB[3] * fDot;
}
#endif

// -a into out (three floats)
#ifdef __MWERKS__
asm void fn_80055EF8(register f32* pA, register f32* pOut) {
    nofralloc
    psq_l  f0, 0(pA), 0, 0
    psq_l  f1, 8(pA), 1, 0
    ps_neg f0, f0
    ps_neg f1, f1
    psq_st f0, 0(pOut), 0, 0
    psq_st f1, 8(pOut), 1, 0
    blr
}
#else
// port: untested, the plain-C version for compilers without paired singles.
void fn_80055EF8(f32* pA, f32* pOut) {
    pOut[0] = -pA[0];
    pOut[1] = -pA[1];
    pOut[2] = -pA[2];
}
#endif

void fn_80055F14(void) {
}

void fn_80055F18(void) {
}

// The wind: its speed, and (when asked) its vector - the direction's unit vector x speed.
f32 Wind_Get(f32* pOut) {
    if (pOut != NULL) {
        f32 v[4];
        fn_8000AE28(gWindDirs[gWindDir], gWindSpeed, v);
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
    int n      = Hole_WindDir();
    f32 fSpeed = Hole_WindSpeed();
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
            fSpeed = Rand_Next(0) % 7;
            break;
        case 1:
            fSpeed = 2.0f + Rand_Next(0) % 11;
            break;
        case 2:
            fSpeed = 5.0f + Rand_Next(0) % 16;
            break;
        case 3:
            fSpeed = 12.0f + Rand_Next(0) % 20;
            break;
        }
        n = Rand_Next(0) % 8;
    }
    Wind_Set(n, fSpeed);
}

// The wind's direction as a unit vector.
void fn_800561CC(f32* pOut) {
    Vec_Copy(gWindDirs[gWindDir], pOut);
}

void fn_80056204(void) {
}

void fn_80056208(void) {
}
