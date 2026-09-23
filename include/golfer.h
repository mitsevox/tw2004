// golfer.h: the golfer record, the player struct and the attribute accessor, shared by
// Golfer.c (the CPU golfer) and Swing.c (the human swing). Layouts are from the reads in
// those files; see docs/formats/game-data.md and docs/gameplay.md.

#ifndef GOLFER_H
#define GOLFER_H

#include "game_types.h"
#include "engine.h"
#include "physics.h"
#include "camera.h"
#include "character.h"
#include "ball.h"

// ---- attributes -----------------------------------------------------------------------------

enum {
    ATTR_POWER,             // 0
    ATTR_POWER_BOOST,       // 1
    ATTR_AGGRESSION,        // 2   hidden: CPU target choice
    ATTR_BALL_STRIKING,     // 3
    ATTR_DRIVING_ACCURACY,  // 4
    ATTR_APPROACH,          // 5
    ATTR_PUTTING,           // 6
    ATTR_RECOVERY,          // 7
    ATTR_IQ,                // 8   hidden: CPU overconfidence
    ATTR_SPEED,             // 9   hidden: animation
    ATTR_SPIN,              // 10
    ATTR_LUCK,              // 11
    NUM_ATTRS
};

#define ATTR_BASE      0    // Golfer_GetAttribute modes: the golfer's own value plus equipment
#define ATTR_MODIFIERS 1    // the per-player modifier only
#define ATTR_TOTAL     2    // both

#define NUM_GOLFERS          34   // records in gGolferTable
#define FIRST_CREATED_GOLFER 30   // table slots 30..33 are the created golfers
#define CONTROLLER_CPU       9
// A CPU shot's shape (TW06: ShotShape_t), from the authored aim point. The rehearsal compensates
// each one when it fails: the curves by turning the aim 1 or 2 degrees, the trajectories by 5
// yards of distance. High/low pick gTrajLoft's +5 / -5 degrees (Shot_Trajectory).
enum {
    SHAPE_NORMAL = 0,
    SHAPE_FADE   = 1,       // clubface x +0.02
    SHAPE_DRAW   = 2,       // clubface x -0.02
    SHAPE_HIGH   = 3,       // trajectory 2: +5 degrees of loft
    SHAPE_LOW    = 4,       // trajectory 0: -5 degrees
    SHAPE_SLICE  = 5,       // clubface x +0.04
    SHAPE_HOOK   = 6        // clubface x -0.04
};

#define NUM_AI_LINKS         10   // candidate aim points per zone
#define NUM_AI_TARGETS       25   // aim points per hole (gAITargets)

// A player slot, TW06's PlayerNumber_t. The save profiles (gpSaveData) are indexed by it.
typedef enum PlayerNumber_t {
    PLR_1_e,
    PLR_2_e,
    PLR_3_e,
    PLR_4_e,
    PLR_MAX_e
} PlayerNumber_t;

#define PI    3.14159265f
#define TWOPI 6.28318531f
#define DEG(x) ((x) * (PI / 180.0f))

#define IABS(v) (((v) ^ ((v) >> 31)) - ((v) >> 31))   // what the compiler emits for abs()

// One 320-byte row of STATS_GC.BIN as it sits in gGolferTable. The file has a 2-byte header,
// so every field is 2 bytes later than in the file; the game reuses the first byte as the
// golfer index once a row is copied into a player. TW06: GolferData_t (0x1F0), the same up to
// 0x62, then 4 bytes later from the stats on (it added fields in 0x62..0x6C).
typedef struct GolferRecord {
    u8   nIndex;                // 0x000
    u8   nModelID;              // 0x001  Golfer_FindById searches on it. TW06: modelID
    char szFirst[32];           // 0x002
    char szLast[32];            // 0x022
    char szNick[32];            // 0x042
    u8   nOutfit;               // 0x062  the outfit (copied to PlayerProfile.nOutfit). TW06 has ballID here
    s8   nEarningsRating;       // 0x063  0..25, what beating this golfer pays (Earnings.c). TW06: earningsRating
    u8   unk64[4];              // 0x064  TW06 has trajectory[3], characteristic, severity, chance here
    s8   attr[NUM_ATTRS];       // 0x068  block A. TW06: baseStats
    s8   attrAlt[NUM_ATTRS];    // 0x074  block B: used for CPU pros in game mode 4. TW06: crapStats
    s8   tier[NUM_ATTRS];       // 0x080  equipment tiers 0..4, one per attribute. TW06: modLevel
    u8   stance[2];             // 0x08C  TW06's name; unused here
    u8   bAvailable;            // 0x08E  non-zero in gCurGolferRecord when there is one. TW06: available
    u8   unk8F;                 // 0x08F
    u32  uBagMask;              // 0x090  bit n set = club n is in the bag. TW06: clubAvailable
    u8   unk94[0x140 - 0x94];
} GolferRecord;
LAYOUT_ASSERT(GolferRecord, 0x140);

// Golfer states: the rows of sGolferStateEngineTable (init, update, exit), kept on a per-player
// stack by GOLFERSTATE_Push/Pop/Switch/Set. Names from TW06's table (docs/tw06-names.md); TW06
// added two cut-scene states after PreShot and dropped the knee cam (our name for state 8).
enum {
    GS_NONE                = 0,
    GS_PRE_SHOT            = 1,     // walking up and addressing the ball
    GS_SHOT_SETUP          = 2,     // a CPU thinks here; a human goes straight to GS_SWING
    GS_ZOOM                = 3,     // zoom-to-aim camera
    GS_ELEVATOR            = 4,     // elevator (raised) camera
    GS_GREEN               = 5,     // green camera
    GS_GREEN_WATCH_ROLL    = 6,     // the putt preview
    GS_GREEN_REVERSE_PUTT  = 7,     // reverse-putt camera
    GS_KNEE_CAM            = 8,     // camera 7, TW06's kCameraMode_KneeCam; no TW06 state
    GS_GREEN_MORPH         = 9,     // the putt-line view
    GS_SWING               = 10,    // over the ball with the HUD: the stick swing
    GS_REPLAY_SWING        = 11,    // the swing animation playing out to impact
    GS_SIMULATE            = 12,    // ball in flight
    GS_IN_THE_HOLE         = 13,
    GS_SHOW_YARDAGE        = 14,    // ball at rest, not holed
    GS_FADE_TO_TAP_IN      = 15,    // only when a gimme is allowed: the rehearsal solves the tap-in
    GS_TAP_IN              = 16,
    GS_FADE_TO_REMOVE_BALL = 17,
    GS_REMOVE_BALL         = 18,    // picking the ball out of the cup
    GS_WAIT                = 19,
    GS_INITIAL_FLY_BY      = 20,    // the hole flyover
    GS_MID_HOLE_FLY_BY     = 21,
    GS_PLACE_BALL          = 22,
    GS_CONCEDED            = 23,
    GS_NUM                 = 27     // table rows (24..26 empty)
};

// Swing states (TW06: SW_eSwingState), the value of SwingData.nState. Stepped by the table
// gSwingPhaseFns; "fidget" is holding the stick at the top of the backswing.
enum {
    SW_IDLE_SWING       = 0,
    SW_BACK_SWING       = 1,
    SW_BACK_FIDGET_SWING = 2,   // TW06 spells it SW_BACK_FIGIT_SWING
    SW_DOWN_SWING       = 3,
    SW_FOLLOW_SWING     = 4,
    SW_POST_SWING       = 5,
    SW_CANCEL_SWING     = 6
};

// The swing meter's per-player state, embedded in Player at 0x3D4 (offsets below are within
// this struct; add 0x3D4 for the player offset). Names are TW06's SW_sSwingData, which is the
// same struct with a few fields added: 4 bytes after 0x14, 12 after 0x2C (the second stick),
// 8 after 0x37C, 0x18 in all by 0x390. The fields our code uses were checked against it
// (docs/tw06-names.md, "Structs").
typedef struct SwingData {
    s32  nState;                // 0x000  (0x3D4) SW_* above
    f32  fTimeSwingTop;         // 0x004  animation times, from the clip's marks 1, 0, 2
    f32  fTimeSwingStart;       // 0x008
    f32  fTimeBallHit;          // 0x00C
    f32  f10;                   // 0x010  TW06 has time_followEnd, fClubOffScale and
    f32  f14;                   // 0x014    fClubOffTargetScale here: one of the three is new
    s32  nBackSwingX;           // 0x018  (0x3EC) stick at the top of the backswing
    s32  nBackSwingY;           // 0x01C  (0x3F0)
    s32  nFollowThroughX;       // 0x020  (0x3F4) stick at impact
    s32  nFollowThroughY;       // 0x024  (0x3F8)
    s32  nMishitX;              // 0x028  (0x3FC)
    s32  nMishitY;              // 0x02C  (0x400)
    u8   unk30[0x3C - 0x30];    // 0x030  TW06: ballFlightX, ballFlightY, fForwardSwingMagnitude
    f32  fMishitAngle;          // 0x03C  (0x410) the stick's miss after forgiveness, added to the aim
    f32  fShotPower;            // 0x040  (0x414) Swing_ComputePower's result
    f32  fHookSlice;            // 0x044  (0x418) copy of the face vector's x (vLaunchA[0])
    f32  fNonPowerShotPower;    // 0x048  (0x41C) boosted power minus the error. TW06: fNonPowerAttribAffectedShotPower
    f32  fControllerSliceAngle; // 0x04C  (0x420) the backswing's sideways angle, radians (0 on a putt)
    struct { f32 vClubPos[4]; f32 vHandPos[4]; } prevClub[25];  // 0x050  (0x424) the club's last 25 positions, newest first (the trail)
    s32  nNumInBlurQueue;       // 0x370  (0x744) trail points in use, up to 25
    u8   bUIInit;               // 0x374  unused here
    u8   bDrawBoostUI;          // 0x375  (0x749) set by fn_8005A788
    u8   unk376[2];
    s32  nCalibrateX;           // 0x378  (0x74C) stick at the start of the swing
    s32  nCalibrateY;           // 0x37C  (0x750)
    s32  nRestCX;               // 0x380  (0x754) the sticks' rest positions while waiting. TW06 has six
    s32  nRestCY;               // 0x384  (0x758)   fields here (iCalibrateXstick2, ...Left..., ...Right...);
    s32  nRestX;                // 0x388  (0x75C)   which four these are is not settled
    s32  nRestY;                // 0x38C  (0x760)
    s32  nStickUsed;            // 0x390  (0x764) nonzero: the C stick is swinging
    s32  nCtrlListX[25];        // 0x394  (0x768) the last 25 stick samples
    s32  nCtrlListY[25];        // 0x3F8  (0x7CC)
    s32  nCtrlListIndex;        // 0x45C  (0x830)
    s32  nVibrateCount;         // 0x460  (0x834) rumble frames left
    u8   bVibrating;            // 0x464  (0x838)
    u8   unk465[0x470 - 0x465]; // 0x465  TW06: iVibrateStrength (0x468), fCurrentStickPower (0x46C)
    f32  fFidgetPauseTime;      // 0x470  (0x844) animation time when the backswing settled at the top
    s32  nFidgetPauseStickY;    // 0x474  stick y at that moment. TW06: iFigitControllerPauseVal
    f32  fFidgetTargetTime;     // 0x478
    f32  fFidgetWaitToIdle;     // 0x47C  time with the stick back near centre; past 0.1 s the swing goes idle
    f32  fFidgetTimeElapsed;    // 0x480  (0x854) seconds held at the top
    f32  fBlueColor;            // 0x484  (0x858) the trail's colour: blue when the stick is left of
    f32  fRedColor;             // 0x488    centre, red + green (yellow) when right
    f32  fGreenColor;           // 0x48C
    f32  fAlpha;                // 0x490
    s32  nPowerBoost;           // 0x494  (0x868) power boost level pressed, 0..8
    f32  fPowerBoostDieTime;    // 0x498  1/12 s once the stick backs down; at 0 the boost is lost
    u8   unk49C[0x604 - 0x49C]; // 0x49C  TW06: the swing-boost list (bSwingBoostsOn .. iCurBoostNum), same size
    f32  fTimeSinceContact;     // 0x604
    f32  fSpinAmount;           // 0x608  unused here
    u8   bSpun;                 // 0x60C
    u8   bSpinning;             // 0x60D
    u8   unk60E[2];
    s32  nSpinBoost;            // 0x610  (0x9E4) how much spin was asked for, 0..20
    s32  nSpinCtrlX;            // 0x614  (0x9E8) 0..255, 128 centre
    s32  nSpinCtrlY;            // 0x618  (0x9EC)
    f32  fForwardSpin;          // 0x61C  (0x9F0) from the distance error on a CPU shot
    f32  fSideSpin;             // 0x620  (0x9F4) from the aim error
    u8   bCanSpin;              // 0x624  (0x9F8) a human struck the ball (not in a replay): spin input is live
    u8   unk625[3];
    f32  fTargetTurnAngle;      // 0x628
    f32  fCurrentTurnAngle;     // 0x62C
    u8   unk630;                // 0x630  (0xA04) cleared by Player_SetGolfer; not in TW06
    u8   unk631[3];
} SwingData;
LAYOUT_ASSERT(SwingData, 0x634);

// Money by kind (0x40 bytes; TW06: CourseMoneyTracking_t): how a payout was made up (Earnings.c
// fills it in), and a player's totals (Player.money), which fn_800D3548 adds it to field by field.
typedef struct CourseMoneyTracking {
    s32  n0;                    // 0x00  the payout
    s32  n4;                    // 0x04  a PGA TOUR tournament's prize money (GameModeDriverPGATour)
    s32  n8;                    // 0x08  bonuses won (GameMode5 EndGame)
    s32  nC;                    // 0x0C  a ladder event's prize (GameMode4 EndGame)
    s32  n10;                   // 0x10  n24 minus the last match prize (GameModeMatch EndGame)
    s32  n14;                   // 0x14  a total the match modes add their prize (or money) to
    s32  n18;                   // 0x18  skins money won (GameMode2 EndGame)
    s32  n1C;                   // 0x1C  match money won (GameMode8 EndGame)
    s32  nBase;                 // 0x20  the points, rounded to $25
    s32  n24;                   // 0x24  the payout
    s32  nCourse;               // 0x28  what the course multiplier added
    s32  n2C;                   // 0x2C  what the multiplier for the hole's pin set added
    s32  nTee;                  // 0x30  what the tee multiplier added
    s32  nTourCard;             // 0x34  what the TOUR card level added
    s32  n38;                   // 0x38
    s32  n3C;                   // 0x3C
} CourseMoneyTracking;

// A player in the current round (human or CPU). 0xEF8 bytes; only the fields read so far.
// TW06: GamePlayer (0xFE0). EA later grouped these fields into sub-structs in a different
// order, so only blocks are matched: the score block is the first 0x200 bytes of TW06's
// GolferScore_t and the shot block at 0x354 is its AIshot_t (see docs/tw06-names.md).
typedef struct Player {
    s32  nIndex;                // 0x000
    s32  unk4;                  // 0x004
    GolferRecord golfer;        // 0x008
    s8   attrMod[NUM_ATTRS];    // 0x148  modifiers on top of the record. TW06: modStats
    // Score block, TW06 GolferScore_t: strokes, putts, modepoints, skinwin (18 each), skinwins,
    // matchwins, roundscore[4], playercut, timetaken[18], puttDistances[18], playoffrelscore,
    // longestdrive, longestputt, fairways[18], gir[18], roundEventFlag - which fills 0x154..0x354
    // exactly. Only strokes and matchwins are confirmed by our code so far.
    s32  nStrokes[18];          // 0x154  strokes taken per hole
    s32  nPutts[18];            // 0x19C  putts per hole (GM_PlayerAddStroke). TW06: putts
    s32  nModePoints[18];       // 0x1E4  per hole, the mode's points (match play: 1 = hole won). TW06: modepoints
    s32  n22C[18];              // 0x22C  per hole. TW06: skinwin
    s32  n274;                  // 0x274  TW06: skinwins
    s32  nHolesWon;             // 0x278  match play. TW06: matchwins
    s32  nRoundScore[4];        // 0x27C  TW06: roundscore
    u8   bPlayerCut;            // 0x28C  TW06: playercut
    u8   unk28D[3];
    s32  n290[18];              // 0x290  per hole
    s32  n2D8;                  // 0x2D8
    s32  n2DC;                  // 0x2DC
    s32  n2E0;                  // 0x2E0
    u8   b2E4[18];              // 0x2E4  per hole
    u8   b2F6[18];              // 0x2F6  per hole
    s32  n308;                  // 0x308
    u8   b30C;                  // 0x30C  the start of the block GameModeReplay restores from a replay
    u8   b30D;                  // 0x30D  tested with the course check by GameEffects
    u8   b30E;                  // 0x30E  a replaced ball must be dropped (GM_ReplaceOOBBall)
    u8   b30F;                  // 0x30F  copied to b310 after a shot (fn_800D9350)
    u8   b310;                  // 0x310  cleared by fn_800D8D38
    u8   b311;                  // 0x311  set after a shot with b30E (fn_800D9350)
    u8   b312;                  // 0x312  set when a shot finished on the green or in the hole (fn_800D9350)
    u8   unk313;
    CourseMoneyTracking money;  // 0x314  the round's money by kind (fn_800D3548)
    // Shot block, TW06 AIshot_t (which has 6 preferred clubs where we have 8).
    s32  nClub;                 // 0x354  TW06: club
    s32  nClubPerKind[8];       // 0x358  the club Shot_Prepare would pick for each shot kind. TW06: preferredClub
    f32  fAim;                  // 0x378  aim angle, radians. TW06: direction
    f32  fPower;                // 0x37C  0..1 (up to 1.5). TW06: strength
    s32  nShotKind;             // 0x380  0 putt, 2/3 approach, 5..7 recovery. TW06: type (ShotType_t)
    s32  nTrajectory;           // 0x384  from Shot_Trajectory: 0 low, 1 normal, 2 high. TW06 has a float stance here
    f32  vLaunchA[4];           // 0x388  launch parameter blocks handed to Ball_Launch. TW06: clubDirection (the face)
    f32  vLaunchB[4];           // 0x398  TW06: strokeDirection (the swing path, which carries the shape)
    s32  nShotShape;            // 0x3A8  SHAPE_*: what the aim point (or a lesson) asks the CPU to play. TW06: shape
    u8   bPerfect;              // 0x3AC  no error / no forgiveness when set. TW06: perfect
    u8   unk3AD[3];
    s32  nShotKind2;            // 0x3B0
    f32  vBall[4];              // 0x3B4
    f32  vPreShot[4];           // 0x3C4  where the ball lay before the shot (GM_BumpBallForObstructions drops it
                                //        back here). TW06: PreShotBallPos
    SwingData swing;            // 0x3D4  the swing meter's state for this player
    s32  nController;           // 0xA08  CONTROLLER_CPU for the AI. TW06: Controller (PlayerCtrl_t, 9 = AI)
    s32  nView[2];              // 0xA0C  the views (ViewController) the player uses. TW06: viewControllerID[2]
    f32  vTarget[4];            // 0xA14
    f32  vTargetCopy[4];        // 0xA24  copy of the planned target. Probably TW06's originalTargetPos
    f32  vTarget2[4];           // 0xA34  copy of the chosen aim point
    f32  vA44[4];               // 0xA44  compared with the ball position (GM_BumpBallForObstructions)
    f32  fDistance;             // 0xA54  to the target. TW06: targetDistance
    f32  fDistance2;            // 0xA58
    f32  fA5C;                  // 0xA5C  } placing the ball (state 22): how fast the spot moves along x and
    f32  fA60;                  // 0xA60  } z, -1..1, built up while the stick is held (target.c)
    f32  fA64;                 // 0xA64  a distance, set when a swing state 16 begins
    s32  nSurface;              // 0xA68  surface type under the target, -1 none, 16 water. TW06: targetedSurfaceID
    f32  vPlacement[4];         // 0xA6C  where the ball may be placed (swing state 22)
    f32  fA7C;                  // 0xA7C  pad stick x, -1..1 (GameMode9 fn_800EDAE0)
    f32  fA80;                  // 0xA80
    f32  fA84;                  // 0xA84
    f32  fA88;                  // 0xA88  an angle (speed golf: the run's heading)
    f32  fA8C;                  // 0xA8C  pad stick y, -1..1 (GameMode8 fn_800FB460)
    Ball ball;                  // 0xA90  the player's ball
    f32  vOrient[4];            // 0xB4C  a quaternion, identity at setup. TW06: ballRot
    Ball ballBefore;            // 0xB5C  a copy of the ball: as it lay before the shot, then the look-ahead
                                //        copy launched with it (STATEFUNC_SimulateInit)
    Character* pChar;           // 0xC18  the golfer on screen
    f32  fThinkTime;            // 0xC1C  seconds a CPU has spent in state 2
    f32  fC20;                  // 0xC20
    s32  nC24;                  // 0xC24  added to PgaStatCounts.nDriveDistance at the end of a hole (GameModeDriverPGATour_EndHole)
    u8   bMulliganUsed;         // 0xC28  the one mulligan of a one-per-player mode is used (GM_PlayerTakeMulligan)
    u8   bLowIQPenalty;         // 0xC29  quarters the IQ overconfidence term when set
    s8   nLevel;                // 0xC2A  CPU difficulty level: 25 modifier points per level
    u8   bPlanReady;            // 0xC2B  the gimme's tap-in was solved when the camera arrived
    u8   bRehearsalDone;        // 0xC2C  the gimme's tap-in rehearsal (GS_FADE_TO_TAP_IN) has settled
    u8   bC2D;                  // 0xC2D  set when the stroke limit holes the ball; no mulligan then
    u8   bC2E;                  // 0xC2E  set when a mulligan is taken (GameManager.c), cleared by Swing.c
    u8   bC2F;                  // 0xC2F  set with bC2E when a mulligan is taken
    s32  nRehearseState;        // 0xC30  AI_RehearseShot state machine
    u8   unkC34[4];
    s32  nC38;                  // 0xC38  a frame countdown (speed golf)
    s32  nC3C;                  // 0xC3C
    s32  nC40;                  // 0xC40
    s32  nC44;                  // 0xC44  3000 at the start of a round
    u64  uC48;                  // 0xC48  speed golf: the events' flags (SGEvent), 64 bits
    f32  fC50;                  // 0xC50  speed golf: a distance from the ball to vA44
    s32  nC54;                  // 0xC54  a frame countdown (speed golf's run to the ball)
    s32  nC58;                  // 0xC58
    s32  nC5C;                  // 0xC5C
    s32  nC60;                  // 0xC60  speed golf: strokes when the player holed out
    s32  nC64;                  // 0xC64  speed golf: strokes when the ball reached the green
    f32  fC68;                  // 0xC68  speed golf: the ball's distance from gpGame->p130 then
    s32  nC6C[18];              // 0xC6C  cleared at the start of a round
    f32  fCB4;                  // 0xCB4  speed golf: raised by a button, falls every frame
    s32  nCB8;                  // 0xCB8  speed golf: cleared by that button
    f32  vCBC[3];               // 0xCBC  a vector (the run's velocity?): the first-person camera's step is
                                //        three times the length of its x and z
    u8   unkCC8[0xCD0 - 0xCC8];
    s32  nCD0;                  // 0xCD0  cleared per game (fn_800F2030)
    s32  aCD4[20];              // 0xCD4
    s32  nD24;                  // 0xD24  mode 12: a bonus meter, 0..100
    s32  nD28[18];              // 0xD28  per hole
    s32  nD70[18];              // 0xD70  per hole
    s32  nDB8;                  // 0xDB8
    s32  nDBC;                  // 0xDBC  the shot's multiplier in the target games (1, 2, 3 or 5)
    s32  nDC0;                  // 0xDC0
    s32  aDC4[5];               // 0xDC4
    s32  nDD8;                  // 0xDD8  a count a challenge medal can ask for (GameMode5)
    s32  nDDC;                  // 0xDDC
    s32  nDE0;                  // 0xDE0
    s32  nDE4[40];              // 0xDE4  per target (fn_800F1D34), 0 = not yet hit
    s8   nTarget;               // 0xE84  the current target (set in GameTargets.c)
    u8   unkE85[3];
    s32  nE88;                  // 0xE88  below 5: still in (mode 15)
    s32  nE8C;                  // 0xE8C
    s32  nE90;                  // 0xE90
    s32  nE94;                  // 0xE94
    s32  nE98;                  // 0xE98  shots in a row without a multiplier
    s8   nNextTarget;           // 0xE9C  the next target to hit, in order (GameMode17)
    s8   bE9D;                  // 0xE9D
    u8   bE9E;                  // 0xE9E
    u8   unkE9F;
    s32  nEA0;                  // 0xEA0  } values of the modes' own the menus ask for
    u8   unkEA4[4];             //        } (GameUICommands.c fn_80089DB0)
    s32  nEA8;                  // 0xEA8  }
    u8   unkEAC[0xEBC - 0xEAC];
    s32  nEBC;                  // 0xEBC  }
    s32  nEC0;                  // 0xEC0  }
    u8   unkEC4[0xECC - 0xEC4];
    s32  nECC;                  // 0xECC  }
    u8   unkED0[4];
    s32  nED4;                  // 0xED4  }
    s32  nED8;                  // 0xED8  }
    s32  nEDC;                  // 0xEDC  }
    u8   bEE0;                  // 0xEE0
    u8   unkEE1[3];
    s32  nEE4;                  // 0xEE4  2 or 3 picks a message after a shot (GM_PlayerTookShot)
    u32  uFlags;                // 0xEE8  bit 0: scripted reaction, bit 2: the early reaction has played, bit 3: score display
    f32  fEEC;                  // 0xEEC  distance to the pin when the early reaction started (GM_SimulateBallMovement)
    u32  uFlagsEF0;             // 0xEF0  0x1: the ball can be placed at vPlacement (fn_800693A4);
                                //        0x2: target is over water
    u8   unkEF4[0xEF8 - 0xEF4];
} Player;
LAYOUT_ASSERT(Player, 0xEF8);

// An all-time record: the value and who holds it (gSession.recA/B/C).
typedef struct RecordEntry {
    s32  nValue;                // 0x00
    char szName[16];            // 0x04
} RecordEntry;

// The round / session state at gSession (0x5BD0 bytes); only what this file reads.
// The game options (Session.options, 0x88 bytes).
typedef struct GameOptions {
    u8   a0[5];                 // 0x00  [4] (0xE7C): 4 while the lessons run, tested by GameUI
    u8   bGimmes;               // 0x05  (gSession + 0xE7D) the Gimmes option, default on
    u8   bSkipCameras;          // 0x06  (gSession + 0xE7E) camera states end at once (inferred)
    u8   a7[5];                 // 0x07  [1] and [2] default to 1
    s32  nC;                    // 0x0C  0, 3 or 4, set by the modes while they run
    s32  nWind;                 // 0x10  0..3 calm..gusty, 4+ none
    s32  n14;                   // 0x14
    s32  n18;                   // 0x18  -> fn_80055C40
    s32  n1C;                   // 0x1C  -> fn_80055CD0
    u8   unk20[4];
    u8   a24[8];                // 0x24  eight on/off options, default on; [7] (0xEA3) the swing trail
    u8   bBoostEnabled;         // 0x2C  (gSession + 0xEA4)
    u8   bSpinEnabled;          // 0x2D  (gSession + 0xEA5)
    u8   rows[4][19];           // 0x2E  four rows of 19 flags
    u8   b7A;                   // 0x7A
    u8   b7B;                   // 0x7B
    u8   b7C;                   // 0x7C
    u8   b7D;                   // 0x7D
    u8   b7E;                   // 0x7E
    u8   unk7F;
    s32  n80;                   // 0x80
    u8   b84;                   // 0x84  cleared while the lessons run (GameMode11)
} GameOptions;
LAYOUT_ASSERT(GameOptions, 0x88);

// A player's profile block (Session.aProfile, 0x40 bytes each).
typedef struct PlayerProfile {
    s8   n0;                    // 0x00  0..3; bumped for a CPU opponent playing the same golfer (GameMode5)
    u8   n1;                    // 0x01  cleared by Session_Init and the golfer setup
    u8   n2;                    // 0x02  a created golfer's byte 0x54C2 of its save slot, else 0
    u8   unk3[5];
    char szNames[6][8];         // 0x08
    u8   nOutfit;               // 0x38  the golfer record's nOutfit, or the created golfer's
    u8   nBallType;             // 0x39  0..3, from the SPIN attribute for a pro
    u8   unk3A[6];
} PlayerProfile;
LAYOUT_ASSERT(PlayerProfile, 0x40);

// A course's records (the 'rcrd' stream block, Session_OnRecordsLoaded; 0x320 bytes per course).
// Like the all-time records (recA): 8 kinds, the top 5 of each (fn_800D8458 reads them).
// GameEffects compares kind 0's best with a player's strokes + 1, and kind 2's with three times
// Player.fA64.
typedef struct CourseRecord {
    RecordEntry aRecord[8][5];  // 0x000
} CourseRecord;
LAYOUT_ASSERT(CourseRecord, 0x320);

#define NUM_COURSE_RECORDS 21   // 0xF00..0x50A0 of the session

typedef struct Session {
    u32  uFlags;                // 0x000  bit 1: use the alternate attribute block everywhere;
                                //        bit 9: every club in the bag
    s32  nGameType;             // 0x004  4 gets a second view
    u8   a8[4];                 // 0x008  [0] nonzero: no GameBreaker (GameEffects.c)
    s32  nC;                    // 0x00C
    u8   nSplitScreen;          // 0x010  0 single view, else split screen (2 = side by side); no luck, no caddie
    u8   b11;                   // 0x011  cleared by Session_Init
    u8   b12;                   // 0x012  set by the pause menu, a replay and the lessons; GameManager
                                //        tests it
    u8   bReplay;               // 0x013  a saved replay is playing: no luck swap, no spin, instant launch
    s32  nPaused;               // 0x014  0 running, 1 paused (GameUI fn_800E3E3C), 2 paused until the last
                                //        menu screen closes (fn_800E5240 then unpauses)
    f32  fFrameTime;            // 0x018  seconds per frame
    f32  f1C;                   // 0x01C
    s32  n20;                   // 0x020
    s32  nFrameCount;           // 0x024  frames counted; GameRound turns a difference of it into seconds
    s32  n28;                   // 0x028
    s32  nNumPlayers;           // 0x02C
    s32  nController[5];        // 0x030  per player (slot 4 is the caddie / lucky-shot copy)
    s32  nGolfer[5];            // 0x044  golfer index per player
    s32  nTeeSet[5];            // 0x058
    u32  uBag[5];               // 0x06C  per player, 0 = the record's own
    u8   unk80[0xD28 - 0x80];
    u8   aD28[5];               // 0xD28  per index, set by fn_8001D6D8
    u8   aD2D[5];               // 0xD2D  per index, set by fn_8001D624
    u8   unkD32[0xD38 - 0xD32];
    PlayerProfile aProfile[5];  // 0x0D38
    GameOptions options;        // 0x0E78
    CourseRecord aCourseRecord[NUM_COURSE_RECORDS];    // 0x0F00  per course
    RecordEntry recA[8][5];     // 0x50A0  all-time records: 8 kinds, top 5 each
    RecordEntry recB[3][3][5];  // 0x53C0  3 x 3 kinds, top 5 each
    RecordEntry recC[5][2][5];  // 0x5744  5 x 2 kinds, top 5 each
    u32  nSeed;                 // 0x5B2C
    u8   unk5B30[4];
    s32  n5B34;                 // 0x5B34
    s8   nPinSet;               // 0x5B38  the pin position every hole uses (0..3; -1 = 0), copied to
                                //         gpGame->nPinSet[] at the start of a round
    u8   bStrokeLimit;          // 0x5B39  the stroke-limit option (GameRound.c), on by default
    u8   unk5B3A[2];
    f32  f5B3C;                 // 0x5B3C
    f32  f5B40;                 // 0x5B40  150
    f32  f5B44;                 // 0x5B44  -400
    f32  f5B48;                 // 0x5B48  1
    u8   unk5B4C[0x5BD0 - 0x5B4C];
} Session;
LAYOUT_ASSERT(Session, 0x5BD0);

// The game state gpGame points at: the current game mode's rules (data and callbacks; TW06
// turned this into the GameModeDriver class). Only what our files use is named.
typedef struct GameState {
    s32  nMode;                 // 0x000
    s32  n4;                    // 0x004
    s32  nMulligans;            // 0x008  0 none, 1 any number, 2 one per player per round (GM_PlayerTakeMulligan)
    s32  nC;                    // 0x00C  4 in the team modes
    s32  n10;                   // 0x010  4 in the team modes
    s32  nCurCourse;            // 0x014  the course of the current hole
    s32  nHoleCourse[18];       // 0x018  the round's 18 holes: which course each comes from
    s32  nCurHoleNum;           // 0x060  the current hole's number on its course
    s32  nCurHole;              // 0x064  0..17 in the round
    s32  nHoleNum[18];          // 0x068  and which hole of that course (a custom round mixes courses)
    u8   bHoleSelected[18];     // 0x0B0  holes this round plays (GM_GotoNextSelectedHole)
    u8   bHoleSaved[18];        // 0x0C2  a copy of the selection (random-hole play, fn_800E2BA4)
    u8   bD4;                   // 0x0D4
    u8   bD5;                   // 0x0D5
    u8   unkD6[2];
    s32  nD8;                   // 0x0D8
    s32  nDC;                   // 0x0DC
    s32  nE0;                   // 0x0E0
    s32  nPinSet[18];           // 0x0E4  per hole: which of its four pin positions (CourseInfo.pin) is used
    s32  n12C;                  // 0x12C
    f32* p130;                  // 0x130  a position: speed golf measures the ball's distance to it
    u8   b134;                  // 0x134  cleared at the start of a hole
    u8   b135;                  // 0x135  set by fn_800E0A84
    u8   b136;                  // 0x136  the holes are not one course's 1..18 (four kinds, 0x136..0x139)
    u8   b137;                  // 0x137
    u8   b138;                  // 0x138
    u8   b139;                  // 0x139  1..6
    u8   unk13A[2];
    s32  nSaveSlot;             // 0x13C  the save slot (0x10600 bytes each) of a custom round
    s32  nSaveCourse;           // 0x140  the custom round in it (0x70 bytes each)
    s32  n144[5];               // 0x144  per player, cleared at the start of a hole
    s32  n158[5];               // 0x158  per player, cleared at the start of a hole
    u8   b16C[5][18];           // 0x16C  per player and hole, cleared with the hole's score
    u8   unk1C6[0x1C8 - 0x1C6];
    // The mode's callbacks (0x1C8..0x26C). fn_800E0B38 sets them all to defaults (mostly empty
    // stubs), then the mode's own setup replaces the ones it needs. The names are TW06's
    // GameModeBase methods, from the modes' implementations (GameModeStroke, GameModeMatch, ...).
    void (*pfnInit)(void);      // 0x1C8  the mode's setup. TW06: Init
    void (*pfnShutdown)(void);  // 0x1CC  the mode ends. TW06: Shutdown (GameModeBattle)
    void (*pfnSetupNextGolfer)(void);   // 0x1D0  the hole starts. TW06: SetupNextGolfer
    s32  (*pfnGetHonors)(int nPlayer);  // 0x1D4  who plays after nPlayer (5 = nobody). TW06: GetHonors
    u8   (*pfnHoleFinished)(int nPlayer, u8 bCheck);   // 0x1D8  the hole is over; bCheck 1 only asks
                                //        (Gimme_Allowed). TW06: HoleFinished(PlayerNumber_t, u8)
    u8   (*pfnGameFinished)(u8 bCheck);     // 0x1DC  the game is over. TW06: GameFinished(u8)
    u8   (*pfnGoToPlayoff)(u8 bCheck);      // 0x1E0  TW06: GoToPlayoff(u8). Nothing in the binary
                                //        calls it (0x800CFB88 only adds the slots up)
    void (*pfn1E4)(void);       // 0x1E4  hole start
    void (*pfnEndHole)(void);   // 0x1E8  hole finished. TW06: EndHole
    void (*pfn1EC)(void);       // 0x1EC
    void (*pfn1F0)(void);       // 0x1F0
    void (*pfnEndGame)(void);   // 0x1F4  game finished. TW06: EndGame
    u8   (*pfn1F8)(int nPlayer); // 0x1F8  fn_800DCB10 returns its answer
    u8   (*pfn1FC)(int nPlayer); // 0x1FC  asked before the special ball pick-up
    s32  (*pfn200)(int nPlayer); // 0x200  strokes behind the leader. TW06: GetCurrentLead
    s32  (*pfn204)(int nPlayer); // 0x204  the same if this putt drops. TW06: GetPotentialLead
    s32  (*pfn208)(int nPlayer); // 0x208
    void (*pfn20C)(int nPlayer); // 0x20C  called as a swing begins (state 1)
    void (*pfn210)(int nPlayer); // 0x210  the hole is over, the game is not
    void (*pfn214)(void);       // 0x214
    void (*pfn218)(int nPlayer); // 0x218
    void (*pfn21C)(int nPlayer); // 0x21C
    void (*pfn220)(void);       // 0x220  every frame in game type 6
    void (*pfn224)(void);       // 0x224  the hole restarts
    void (*pfn228)(int nPlayer); // 0x228  called every frame of the shot setup (state 10)
    void (*pfn22C)(int nPlayer); // 0x22C  called after a re-plan in swing state 9
    u8   (*pfn230)(int nPlayer); // 0x230
    u8   (*pfn234)(void);       // 0x234  GM_CheckControllerPulled asks it (TW06 CheckControllerPulled)
    u8   (*pfn238)(int nPlayer); // 0x238  nonzero: skip addressing the ball (swing state 1)
    void (*pfn23C)(int nPlayer); // 0x23C
    s32  (*pfn240)(int nPlayer); // 0x240  called from 0x800A3460 with the player
    void (*pfn244)(int nPlayer); // 0x244
    void (*pfnEndGolferTurn)(int nPlayer); // 0x248  end of a golfer's turn. TW06: EndGolferTurn
    void (*pfn24C)(int nPlayer); // 0x24C  called when a swing leaves state 20
    void (*pfn250)(int nPlayer); // 0x250  the ball went out of bounds
    void (*pfn254)(int nPlayer); // 0x254  a mulligan was taken
    u8   (*pfn258)(int nPlayer); // 0x258  the re-plan button is allowed
    void (*pfn25C)(int nPlayer, int nTime); // 0x25C  set the time left (GameMode13)
    void (*pfn260)(int nPlayer); // 0x260
    u8   (*pfn264)(int nPlayer); // 0x264  "aim at the pin?" for a re-plan
    void (*pfn268)(int nPlayer, int nId); // 0x268  a bonus was collected (GameMode16)
    s32  (*pfn26C)(int a, int nTarget); // 0x26C  a target's state for the HUD (GameMode14)
    u8   bShowYardage;          // 0x270  show how far each shot went
    u8   b271;                  // 0x271
    u8   bStrokeLimit;          // 0x272  a hole ends at 10 strokes
    u8   b273;                  // 0x273
    u8   b274;                  // 0x274
    u8   b275;                  // 0x275
    u8   b276;                  // 0x276  re-plan the shot as the swing begins
    u8   b277;                  // 0x277
    u8   bGimmesAllowed;        // 0x278  this mode allows gimmes
    u8   b279;                  // 0x279
    u8   bAIConcedes;           // 0x27A  a CPU may concede the hole (GM_CheckForAIConcede)
    u8   b27B;                  // 0x27B
    u8   b27C;                  // 0x27C
    u8   b27D;                  // 0x27D
    u8   b27E;                  // 0x27E
    u8   b27F;                  // 0x27F
    u8   b280;                  // 0x280  the mid-hole flyover button works
    u8   b281;                  // 0x281  tutorial tips may show at setup
    u8   b282;                  // 0x282
    u8   b283;                  // 0x283  the special swing cameras may be used
    u8   b284;                  // 0x284  the re-plan button works
    u8   b285;                  // 0x285
    u8   b286;                  // 0x286  the flight camera toggles are allowed
    u8   b287;                  // 0x287  in-flight replays are allowed
    u8   b288;                  // 0x288
    u8   b289;                  // 0x289
    u8   b28A;                  // 0x28A
    u8   bNoWind;               // 0x28B  wind off
    u8   bBumpObstructions;     // 0x28C  move a ball resting against an obstruction
    u8   b28D;                  // 0x28D
    u8   b28E;                  // 0x28E  set when the game finishes
    u8   unk28F;
    s32  n290;                  // 0x290
    s32  n294;                  // 0x294
} GameState;
LAYOUT_ASSERT(GameState, 0x298);

// An authored aim point. pDef points at its position and the up-to-ten other points a golfer
// standing in its zone may aim at; the bytes are filters and requirements (negative = at most).
typedef struct AITargetDef {
    f32  x, y, z;               // 0x00
    u32  unkC;                  // 0x0C
    s16  nLinks[NUM_AI_LINKS];  // 0x10  indices into gAITargets, -1 = none
    u8   unk24[0x30 - 0x24];
} AITargetDef;                  // 0x30 in the course chunk

typedef struct AITarget {
    AITargetDef* pDef;          // 0x00
    u8   bEnabled;              // 0x04
    s8   nTeeSet;               // 0x05  -1 = any
    s8   nPinSet;               // 0x06  the pin position it is for, -1 = any
    s8   nSkillReq;             // 0x07
    s8   nAggrReq;              // 0x08
    s8   bPriority;             // 0x09  taken when nothing else qualifies (and by humans)
    s8   nType;                 // 0x0A
    s8   nPowerReq;             // 0x0B
} AITarget;

// How a player's aim marker is drawn (our name; 0x2C bytes, one per player at lbl_801D5BF0,
// target.c): the "tball" texture drawn at the target. Putts get a different set (fn_800689D4).
typedef struct TargetMarker {
    f32  f0;                    // 0x00
    f32  f4;                    // 0x04
    f32  f8;                    // 0x08
    f32  fC;                    // 0x0C
    s32  n10;                   // 0x10
    f32  f14;                   // 0x14
    f32  f18;                   // 0x18
    f32  f1C;                   // 0x1C
    f32  f20;                   // 0x20
    f32  f24;                   // 0x24  twice this is a size (fn_80067DAC)
    u8   unk28[4];
} TargetMarker;
LAYOUT_ASSERT(TargetMarker, 0x2C);

extern TargetMarker lbl_801D5BF0[5];   // per player
extern TNetwork*    lbl_80281E30;       // the hole's chunk 3 (fn_8006A7A8): an outline the placed ball
                                        // must be inside, NULL when the hole has none
extern TexBank*     lbl_80281E34;       // } the "shadow" texture
extern TexEntry*    lbl_80281E38;       // }
extern TexBank*     lbl_80281E3C;       // } the "tball" texture (the aim marker)
extern TexEntry*    lbl_80281E40;       // }

// A player's emotion state (our name; 0x24 bytes, one per player at lbl_801D5F78): what the golfer
// feels about the last shot, which picks his reaction (TW06's emotion.c, golf/ai/emotion.c).
typedef struct PlayerEmotion {
    s32  n0;                    // 0x00  0 or 1 from the shot's outcome (fn_8006AAB4), 2 on the green
    s32  n4;                    // 0x04  0..3 from the shot's outcome
    s32  n8;                    // 0x08  the shot's outcome as reported
    s32  nC;                    // 0x0C  n0, n4 and n8 kept after the shot
    s32  n10;                   // 0x10
    s32  n14;                   // 0x14  how the shot turned out (0..4, 8+; fn_8006AA9C)
    s32  n18;                   // 0x18  0..3, the reaction to play (fn_8006B250)
    u8   b1C;                   // 0x1C
    u8   b1D;                   // 0x1D
    u8   b1E;                   // 0x1E  read once, then cleared (fn_8006BAD8)
    u8   b1F;                   // 0x1F
    s32  n20;                   // 0x20  -1 = none
} PlayerEmotion;
LAYOUT_ASSERT(PlayerEmotion, 0x24);

extern PlayerEmotion lbl_801D5F78[5];   // per player

extern GolferRecord gGolferTable[34];   // 0x801CB300  STATS_GC.BIN as loaded
extern GolferRecord gCurGolferRecord;   // 0x801CB1C0  the created golfer being edited
extern Player       gPlayers[5];        // 0x801C66E8
extern u8           gNumPlayersSetUp;   // 0x80281D48  the players set up for the round (Golfer.c)

// Player i by byte offset. Some of EA's loops index the players this way: it is the only form
// that gives the original's separate base and offset registers (tested against the compiler).
#define PLAYER(i) ((Player*)((u8*)gPlayers + (i) * sizeof(Player)))
extern Session      gSession;           // 0x801CDD80
extern GameState*   gpGame;             // 0x80281588

extern AITarget     gAITargets[25];     // 0x801C65B8
extern s32          gNumAITargets;      // 0x80281D44
extern u8           gClubKindTable[8][CLUB_MAX_e];   // 0x801874B0  which clubs each shot kind allows
extern f32          gClubDistAtPower0[CLUB_MAX_e];   // 0x80187580  reach at POWER 0 (245 for the woods)
extern f32          gClubPowerStep[CLUB_MAX_e];      // 0x801875E8  reach gained per POWER point over 100

int  Game_GetMode(void);                // 0x8000BED8
void fn_8002EBA4(u8* pObj, u8 nValue);  // set byte 7 of the options (a7[0]) and apply it (Golfer.c)
int  fn_800D2B08(void);
int  fn_80100744(void);                 // shot kind override, 8 = none
int  fn_801006F0(int nPlayer);          // club override, 26 = none
int  fn_80015464(void);
u8   fn_80101DF4(void);
f32  Swing_SpinScale(int nSpin);         // how much spin SPIN allows: 0.15 at 0 .. 1.0 at 110 (Swing.c)
f32  fn_8005C280(int nPlayer);          // the swing's fNonPowerShotPower (Swing.c)
void Swing_RumbleOff(int nPlayer);      // stops the pad rumble (Swing.c)

u8   Club_UsableForKind(int nPlayer, int nClub, int nKind);
int  AI_FirstUsableClub(int nPlayer, int nKind);
f32  AI_MaxDistance(int nPlayer, int nKind, int nClub);
int  AI_ShotKindForDistance(int nPlayer, f32 fDist);
int  AI_ClubForShot(int nPlayer, int nKind, u8 bUnderOnly, f32 fDist);
f32  AI_PowerForTarget(int nPlayer);
s8   AI_NearestTarget(f32* pPos, f32* pOut);
void AI_DefaultTarget(int nPlayer);
u8   Player_IsCPU(int nPlayer);
u8   Controller_IsCPU(int nController);
u8   Player_HasPad(int nPlayer);
u8   Controller_IsPad(int nController);
u8   Player_IsController8(int nPlayer);
u8   Player_OnTee(int nPlayer);
u8   Player_IsHoled(int nPlayer);
u8   Team_IsAllHuman(int nTeam);        // team 0 is players 0 and 1, team 1 players 2 and 3
void AI_PlanShot(int nPlayer, f32* pTarget);
u8   AI_GreenTowardPin(int nPlayer, f32 fDist);
u8   AI_RehearseShot(int nPlayer, f32* pOutDist2, u8 bFast, f32 fTolerance);
void AI_ApplyError(int nPlayer);
u8   Lie_AllowsFullSwing(int nPlayer);
void Shot_FitTargetToClub(int nPlayer);
void Shot_Plan(int nPlayer, u8 bNotify);
void Shot_Prepare(int nPlayer, u8 bNotify);
int  Shot_Trajectory(int nPlayer);
void Shot_DefaultSpin(int nPlayer, f32* pOut);
void Shot_FaceVector(int nPlayer, f32* pOut);
f32  Shot_AimAngle(int nPlayer);
void AI_ClubLonger(int nPlayer, s32* pClub, int nStep);
void AI_ClubShorter(int nPlayer, s32* pClub, int nStep);
void AI_ChooseTarget(int nPlayer);
void GOLFERSTATE_Kill(int nPlayer);      // Swing.c: pop every state
f32  AI_PowerScale(int nPlayer);
void AI_FaceVector(int nPlayer, f32* pOut);
void Caddie_Start(int nPlayer);
void Caddie_Stop(void);
void Caddie_Update(int nPlayer);
void Luck_TakePerfectShot(int nPlayer);
void Caddie_ApplyTip(int nPlayer);
int  Golfer_GetAttribute(Player* pPlayer, int nAttr, int nMode);
u8   Player_IsHoledNotState23(int nPlayer);
u8   Team_IsAllCPU(int nTeam);
u8   Player_IsNotCPU(int nPlayer);
u8   Bag_AddClub(int nPlayer, int nBit);
u8   Bag_RemoveClub(int nPlayer, int nBit);
u8   Bag_HasClub(int nPlayer, int nBit);
int  Bag_CountClubs(int nPlayer);
void Session_SetGolfer(int nGolfer, int nPlayer);


#endif
