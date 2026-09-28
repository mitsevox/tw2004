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

#define IABS(v) (((v) ^ ((v) >> 31)) - ((v) >> 31))   // what the compiler emits for abs()

// One club skin's look in a GolferRecord: the variant of its part ("Drivers", "Putters", ...) and
// the options of its three sets ("EA_Driver", "fwd_shaft", "fwd_grip", ...), as name codes.
typedef struct ClubLook {
    u64  uPart;                 // 0x00
    u64  uModel;                // 0x08
    u64  uShaft;                // 0x10
    u64  uGrip;                 // 0x18
} ClubLook;

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
    s8   nOutfit;               // 0x062  the outfit (copied to PlayerProfile.nOutfit). TW06 has ballID here;
                                //        FEgolferanim.c passes it to fn_800484E0 as a ball index
    s8   nEarningsRating;       // 0x063  0..25, what beating this golfer pays (Earnings.c). TW06: earningsRating
    u8   unk64[4];              // 0x064  TW06 has trajectory[3], characteristic, severity, chance here
    s8   attr[NUM_ATTRS];       // 0x068  block A. TW06: baseStats
    s8   attrAlt[NUM_ATTRS];    // 0x074  block B: used for CPU pros in game mode 4. TW06: crapStats
    s8   tier[NUM_ATTRS];       // 0x080  equipment tiers 0..4, one per attribute. TW06: modLevel
    u8   stance[2];             // 0x08C  TW06's name; unused here
    u8   bAvailable;            // 0x08E  non-zero in gCurGolferRecord when there is one. TW06: available
    u8   unk8F;                 // 0x08F
    u32  uBagMask;              // 0x090  bit n set = club n is in the bag. TW06: clubAvailable
    u8   unk94[4];
    // The golfer's club models as name codes (SKA_PackName) for the character's club skins
    // (Character_SetClubStatesForCharacter). The irons' two skins share one set of options.
    ClubLook aClubs[3];         // 0x098  drivers, fairway woods, putters
    u64  aIronPart[2];          // 0x0F8  the "3Irons" and "7Irons" parts' variants
    u64  uIronModel;            // 0x108  } the irons' "EA_3Iron"/"EA_7Iron", "pwi_shaft" and
    u64  uIronShaft;            // 0x110  } "pwi_grip" options
    u64  uIronGrip;             // 0x118  }
    ClubLook wedges;            // 0x120
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
    f32  fShotPower;            // 0x040  (0x414) SW_vCalculateShotPower's result
    f32  fHookSlice;            // 0x044  (0x418) copy of the face vector's x (vLaunchA[0])
    f32  fNonPowerShotPower;    // 0x048  (0x41C) boosted power minus the error. TW06: fNonPowerAttribAffectedShotPower
    f32  fControllerSliceAngle; // 0x04C  (0x420) the backswing's sideways angle, radians (0 on a putt)
    struct { f32 vClubPos[4]; f32 vHandPos[4]; } prevClub[25];  // 0x050  (0x424) the club's last 25 positions, newest first (the trail)
    s32  nNumInBlurQueue;       // 0x370  (0x744) trail points in use, up to 25
    u8   bUIInit;               // 0x374  unused here
    u8   bDrawBoostUI;          // 0x375  (0x749) set by SW_vSetDisplayBoostUI
    u8   unk376[2];
    s32  nCalibrateX;           // 0x378  (0x74C) stick at the start of the swing
    s32  nCalibrateY;           // 0x37C  (0x750)
    s32  nRestCX;               // 0x380  (0x754) } the sticks' rest positions while waiting
    s32  nRestCY;               // 0x384  (0x758) }   (SW_vStateIdleSwing, all 128): nRestCX / CY
    s32  nRestX;                // 0x388  (0x75C) }   become nCalibrateX / Y for the main stick,
    s32  nRestY;                // 0x38C  (0x760) }   nRestX / Y for the C stick. TW06 has six
                                //                    fields here (iCalibrateXstick2, ...Left...)
    s32  nStickUsed;            // 0x390  (0x764) nonzero: the main stick (pad bytes 2 and 3) is
                                //                swinging, 0 the C stick (bytes 0 and 1;
                                //                SW_vGetStickX / Y)
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
    f32  fForwardSpin;          // 0x61C  (0x9F0) } the spin asked for: a human's from the spin
    f32  fSideSpin;             // 0x620  (0x9F4) }   stick (y, sign flipped, and x), a CPU's from
                                //                    its distance and aim errors (ai_brain.c)
    u8   bCanSpin;              // 0x624  (0x9F8) a human struck the ball (not in a replay): spin input is live
    u8   unk625[3];
    f32  fTargetTurnAngle;      // 0x628
    f32  fCurrentTurnAngle;     // 0x62C
    u8   unk630;                // 0x630  (0xA04) cleared by Player_SetGolfer; not in TW06
    u8   unk631[3];
} SwingData;
LAYOUT_ASSERT(SwingData, 0x634);

// Money by kind (0x40 bytes; TW06: CourseMoneyTracking_t): how a payout was made up (Earnings.c
// fills it in), and a player's totals (Player.money), which GM_Earnings_AwardMoney adds it to field by field.
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
    s32  nSkinsWon[18];         // 0x22C  skins: the money won per hole (GameModeSkins_ScoreHole).
                                //        TW06: skinwin
    s32  nSkinsTotal;           // 0x274  skins: their sum, paid at the end. TW06: skinwins
    s32  nHolesWon;             // 0x278  match play. TW06: matchwins
    s32  nRoundScore[4];        // 0x27C  TW06: roundscore
    u8   bPlayerCut;            // 0x28C  TW06: playercut
    u8   unk28D[3];
    s32  n290[18];              // 0x290  per hole
    s32  n2D8;                  // 0x2D8
    s32  nLongestDrive;         // 0x2DC  the round's longest drive, in yards
                                //        (GM_RecordIndividualShotStats). TW06: longestdrive
    s32  nLongestPutt;          // 0x2E0  the round's longest holed putt, in feet. TW06: longestputt
    u8   bFairwayHit[18];       // 0x2E4  per hole: the drive of a par 4 or 5 found the fairway (or
                                //        the green, or the hole). TW06: fairways
    u8   bGreenInReg[18];       // 0x2F6  per hole: a green in regulation (on it in
                                //        par - 2 strokes or fewer). TW06: gir
    s32  n308;                  // 0x308
    u8   bHitObject;            // 0x30C  the shot hit a course object (event 36, Collision);
                                //        the start of the block GameModeReplay restores from a
                                //        replay
    u8   bHitPin;               // 0x30D  the shot hit the flagstick (event 38, Collision)
    u8   b30E;                  // 0x30E  a replaced ball must be dropped (GM_ReplaceOOBBall)
    u8   bBunkerThisShot;       // 0x30F  the ball touched a bunker (surface class 6, PsBallFx.c)
    u8   bBunkerThisHole;       // 0x310  set from bBunkerThisShot after a shot
                                //        (GM_RecordBonusShotStats), cleared by GM_ClearHoleBonusStats;
                                //        the tour's bunker stats
    u8   b311;                  // 0x311  set after a shot with b30E (GM_RecordBonusShotStats)
    u8   b312;                  // 0x312  set when a shot finished on the green or in the hole (GM_RecordBonusShotStats)
    u8   unk313;
    CourseMoneyTracking money;  // 0x314  the round's money by kind (GM_Earnings_AwardMoney)
    // Shot block, TW06 AIshot_t (which has 6 preferred clubs where we have 8).
    s32  nClub;                 // 0x354  TW06: club
    s32  nClubPerKind[8];       // 0x358  the club Shot_Prepare would pick for each shot kind. TW06: preferredClub
    f32  fAim;                  // 0x378  aim angle, radians. TW06: direction
    f32  fPower;                // 0x37C  0..1 (up to 1.5). TW06: strength
    s32  nShotKind;             // 0x380  SHOT_TYPE_ (physics.h): 0 putt, 1 drive, 2 chip, 3 pitch,
                                //        4 punch, 5 flop. TW06: type (ShotType_t)
    s32  nTrajectory;           // 0x384  from Shot_Trajectory: 0 low, 1 normal, 2 high. TW06 has a float stance here
    f32  vLaunchA[4];           // 0x388  launch parameter blocks handed to Physics_ShotImpact. TW06: clubDirection (the face)
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
    // The ball placement point (swing state 22; speed golf's runner): stick inputs, -1..1, eased
    // back to 0 by 0.05 a frame (PlaceBall_UpdateMomentums, target.c), and its heading.
    f32  fMomentumTurn;         // 0xA7C  turns the heading
    f32  fMomentumX;            // 0xA80  moves the point sideways
    f32  fMomentumZ;            // 0xA84  moves it forward
    f32  fPlaceHeading;         // 0xA88  radians (speed golf: the run's heading)
    f32  fA8C;                  // 0xA8C  pad stick y, -1..1 (GameMode8 SpeedGolf_ReadSticks)
    Ball ball;                  // 0xA90  the player's ball
    f32  vOrient[4];            // 0xB4C  a quaternion, identity at setup. TW06: ballRot
    Ball ballBefore;            // 0xB5C  a copy of the ball: as it lay before the shot, then the look-ahead
                                //        copy launched with it (STATEFUNC_SimulateInit)
    Character* pChar;           // 0xC18  the golfer on screen
    f32  fThinkTime;            // 0xC1C  seconds a CPU has spent in state 2
    f32  fC20;                  // 0xC20
    s32  nC24;                  // 0xC24  added to PgaStatCounts.nDriveDistance at the end of a hole (GameModeDriverPGATour_EndHole)
    u8   bMulliganUsed;         // 0xC28  the one mulligan of a one-per-player mode is used (GM_PlayerTakeMulligan)
    u8   bPenaltyShot;          // 0xC29  the last shot cost a penalty stroke (GM_CheckForBallOOB;
                                //        cleared at the next hit); quarters a CPU's overconfidence
    s8   nOBCount;              // 0xC2A  penalty strokes in a row: a CPU gets 25 attribute points
                                //        each (ai_brain.c) and concedes after three. TW06: OBCount
    u8   bPlanReady;            // 0xC2B  the gimme's tap-in was solved when the camera arrived
    u8   bRehearsalDone;        // 0xC2C  the gimme's tap-in rehearsal (GS_FADE_TO_TAP_IN) has settled
    u8   bShotLimitExceeded;    // 0xC2D  the stroke limit holed the ball; no mulligan then.
                                //        TW06: shotLimitExceeded
    u8   bUsedMulligan;         // 0xC2E  a mulligan was taken (GM_PlayerTakeMulligan), until the
                                //        next swing state (STATEFUNC_SwingInit). TW06: usedMulligan
    u8   bUsedMulliganThisHole; // 0xC2F  a mulligan was taken on this hole. TW06:
                                //        usedMulliganThisHole
    s32  nRehearseState;        // 0xC30  AI_RehearseShot state machine
    u8   unkC34[4];
    s32  nC38;                  // 0xC38  a frame countdown (speed golf)
    s32  nSGFlags;              // 0xC3C  speed golf's state bits (GameMode8.c: the countdown, the
                                //        run, drive measured, tips, out / won)
    s32  nRunStartLie;          // 0xC40  the ball's lie as the run starts (SpeedGolf_RunInit);
                                //        never read
    s32  nSGPoints;             // 0xC44  speed golf mode 7: 3000 at the start of a round; 0 loses
                                //        (the winner gets 6000)
    u64  uC48;                  // 0xC48  speed golf: the events' flags (SGEvent), 64 bits
    f32  fDriveLength;          // 0xC50  speed golf: the tee shot's length from the tee (vA44),
                                //        then the replay's longer drive
    s32  nC54;                  // 0xC54  a frame countdown (speed golf's run to the ball)
    s32  nUISlot;               // 0xC58  the player's slot in the front end's in-round messages;
                                //        speed golf sets 2 for player 0, 3 for player 1
    s32  nC5C;                  // 0xC5C
    s32  nC60;                  // 0xC60  speed golf: strokes when the player holed out
    s32  nC64;                  // 0xC64  speed golf: strokes when the ball reached the green
    f32  fC68;                  // 0xC68  speed golf: the ball's distance from gpGame->pPinPos then
    s32  nSGHoleScore[18];      // 0xC6C  speed golf, per hole: mode 7 the points after it, modes 6
                                //        and 8 seconds plus 3 a stroke (SpeedGolf_SetHoleTime)
    f32  fCB4;                  // 0xCB4  speed golf: raised by a button, falls every frame
    s32  nCB8;                  // 0xCB8  speed golf: cleared by that button
    f32  vCBC[3];               // 0xCBC  a vector (the run's velocity?): the first-person camera's step is
                                //        three times the length of its x and z
    u8   unkCC8[0xCD0 - 0xCC8];
    // Mode 12 and the target games (modes 13..17, GameMode_SkillZoneBase.c): cleared per shot
    // (GameModeSkillZoneBase_ClearPerShotData) or per hole (ClearPerHoleData).
    s32  nShotSurfaceCount;     // 0xCD0  the surfaces this shot scored on, in aShotSurfaces
    s32  aShotSurfaces[20];     // 0xCD4  (mode 12 counts a surface's repeats)
    s32  nD24;                  // 0xD24  mode 12: a bonus meter, 0..100
    s32  nHolePoints[18];       // 0xD28  mode 12: the points per hole (the shots' nShotPoints)
    s32  nHoleHits[18];         // 0xD70  scoring landings per hole (modes 12..17)
    s32  nShotPoints;           // 0xDB8  mode 12: this shot's points (points x hits x multiplier)
    s32  nDBC;                  // 0xDBC  the shot's multiplier in the target games (1, 2, 3 or 5)
    s32  nBalls;                // 0xDC0  shots taken (modes 13..16; the game stops at 20), or balls
                                //        left (mode 17: 5 to start, plus the extra balls earned)
    s32  aSkillZoneStats[5];    // 0xDC4  [0] shots with a multiplier, [1] extra balls earned
                                //        (mode 17), [3] target hits, [4] time added in frames
                                //        (mode 13); [2] unused. The UI messages read them
    s32  nSkillZonePoints;      // 0xDD8  the points (money) the game has paid; a challenge medal
                                //        can ask for so many (GameMode5)
    s32  nSkillZoneLongestDrive;    // 0xDDC  the longest shot that counts as a long drive
                                //        (GameModeSkillZoneBase_IsLongDrive)
    s32  nBullseyes;            // 0xDE0  hits on a target's centre (surfaces 0x85, 0x88, 0x8C)
    s32  nTargetHits[40];       // 0xDE4  hits per target (GameModeSkillZoneBase_GetGreenIndexHit)
    s8   nTarget;               // 0xE84  the current target (set in GameMode_SkillZoneBase.c)
    u8   unkE85[3];
    s32  nHorseLetters;         // 0xE88  mode 15 (HORSE): a letter per miss; out at 5
    s32  nBestHitStreak;        // 0xE8C  the longest run of target hits
    s32  nHitStreak;            // 0xE90  the current run (a miss ends it)
    s32  nSteals;               // 0xE94  mode 14: hits on a target another player held
    s32  nE98;                  // 0xE98  shots in a row without a multiplier
    s8   nNextTarget;           // 0xE9C  the next target to hit, in order (GameMode17)
    s8   bE9D;                  // 0xE9D
    u8   bAllTargetsHit;        // 0xE9E  the hit-every-target prize was paid (its commentary
                                //        plays once)
    u8   unkE9F;
    // The long-drive contests (modes 22 and 26): kept by GameMode22_ScoreShot and
    // GameMode26_ScoreShot, cleared (all but vBestDrivePos) by their ClearPlayerStats, read by the
    // UI (GameUICommands.c IG_vGetLongDriveStat). A fair drive is one of kind 0 or 1 below.
    s32  nDrivesTaken;          // 0xEA0  every drive
    s32  nFairDrives;           // 0xEA4  the fair ones
    s32  nBestDrive;            // 0xEA8  the longest fair drive
    f32  vBestDrivePos[3];      // 0xEAC  where its ball lay
    u8   unkEB8[0xEBC - 0xEB8];
    s32  nDriveScore;           // 0xEBC  the points (never below 0)
    s32  nAverageDrive;         // 0xEC0  nFairDriveTotal / nFairDrives
    s32  nFairDriveTotal;       // 0xEC4  the fair drives' lengths added up
    s32  nFairwayDrives;        // 0xEC8  kind 0: on the fairway, fringe, green or in the cup
    s32  nBonusDrives;          // 0xECC  kind 1: on surface 0x9B (the length plus 20%)
    s32  nRoughDrives;          // 0xED0  kind 2: on the tee or in the rough (0 points)
    s32  nSandDrives;           // 0xED4  kind 3: in sand (-50)
    s32  nSurfacePenaltyDrives; // 0xED8  kind 4: on surface 0x2F or 0x68 (-100)
    s32  nPenaltyDrives;        // 0xEDC  kind 5: a penalty shot (-100)
    u8   bEE0;                  // 0xEE0
    u8   unkEE1[3];
    s32  nEE4;                  // 0xEE4  2 or 3 picks a message after a shot (GM_PlayerTookShot)
    u32  uFlags;                // 0xEE8  0x1 a scripted reaction, 0x2 set as the ball is taken out
                                //        of the hole (GS_FADE_TO_REMOVE_BALL), 0x4 the early
                                //        reaction has played, 0x8 a tap-in (STATEFUNC_TapInInit)
    f32  fEEC;                  // 0xEEC  distance to the pin when the early reaction started (GM_SimulateBallMovement)
    u32  uFlagsEF0;             // 0xEF0  0x1: the ball can be placed at vPlacement (PlaceBall_ResetMomentums);
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
    s8   a0[5];                 // 0x00  levels 0..5 from the menus (the mixer gets 0.2 x level):
                                //       [0] effects (Gaud_SetSfxLevel), [1] music
                                //       (Gaud_SetMusicLevel), [2] a menu level (FE_MessageTable.c
                                //       GM_vSetOptionLevel2), [4] commentary (Gaud_SetCommentLevel; 4 while
                                //       the lessons run)
    u8   bGimmes;               // 0x05  (gSession + 0xE7D) the Gimmes option, default on
    u8   bSkipCameras;          // 0x06  (gSession + 0xE7E) camera states end at once (inferred)
    u8   a7[5];                 // 0x07  [1] and [2] default to 1
    s32  nWeather;              // 0x0C  the weather option (fn_8006F650 picks each hole's weather
                                //       by it): 0 and 4 clear (weather bit 0), 1 a random pick per
                                //       hole, 2 a pick kept for several holes, 3 weather bit 1; the
                                //       menu sets 0, 2 or 3, several modes 4 while they run
    s32  nWind;                 // 0x10  0..3 calm..gusty, 4+ none
    s32  n14;                   // 0x14
    s32  n18;                   // 0x18  -> fn_80055C40
    s32  n1C;                   // 0x1C  -> fn_80055CD0
    s32  n20;                   // 0x20  0..2 -> fn_80055CAC (FE_MessageTable.c GM_vSetFairwaySpeedOption)
    u8   a24[8];               // 0x24  eight on/off options, default on; [7] (0xEA3) the swing trail
    u8   bBoostEnabled;         // 0x2C  (gSession + 0xEA4)
    u8   bSpinEnabled;          // 0x2D  (gSession + 0xEA5)
    u8   rows[4][19];           // 0x2E  four rows of 19 flags
    u8   abRowOn[4];            // 0x7A  per row of rows: that music row is on (Gaud_SetStreamingContext picks the
                                //       row by game mode)
    u8   b7E;                  // 0x7E
    u8   unk7F;
    s32  n80;                   // 0x80
    u8   bPuttingGrid;          // 0x84  the green grid shows with the putter (GoGreenGrid.c); the
                                //       lessons turn it on for lessons 7..9
} GameOptions;
LAYOUT_ASSERT(GameOptions, 0x88);

// A player's profile block (Session.aProfile, 0x40 bytes each).
typedef struct PlayerProfile {
    s8   n0;                    // 0x00  0..3; bumped for a CPU opponent playing the same golfer (GameMode5)
    u8   n1;                    // 0x01  cleared by Session_Init and the golfer setup
    s8   n2;                    // 0x02  a created golfer's byte 0x54C2 of its save slot, else 0; the
                                //       glove variant's number (Character_SetClubsAndClothes, read signed)
    u8   unk3[5];
    u64  aNames[6];             // 0x08  names, each packed into 64 bits (SKA_PackName)
    u8   nOutfit;               // 0x38  the golfer record's nOutfit, or the created golfer's
    u8   nBallType;             // 0x39  0..3, from the SPIN attribute for a pro
    u8   unk3A[6];
} PlayerProfile;
LAYOUT_ASSERT(PlayerProfile, 0x40);

// A course's records (the 'rcrd' stream block, Session_OnRecordsLoaded; 0x320 bytes per course).
// Like the all-time records (recA): 8 kinds, the top 5 of each (HighScoreRecords_RecordExist reads them).
// GameEffects compares kind 0's best with a player's strokes + 1, and kind 2's with three times
// Player.fA64.
typedef struct CourseRecord {
    RecordEntry aRecord[8][5];  // 0x000
} CourseRecord;
LAYOUT_ASSERT(CourseRecord, 0x320);

#define NUM_COURSE_RECORDS 21   // 0xF00..0x50A0 of the session

typedef struct Session {
    u32  uFlags;                // 0x000  bit 1: use the alternate attribute block everywhere;
                                //        bit 9: every club in the bag; 0x4000 the demo set-up
                                //        (GoEntry.c starts DEMO_Start with it): one player on
                                //        controller 9, every attribute 105, no earnings, clear
                                //        weather (fn_8006F650), caddie tips off; many tests want
                                //        0x8000 with it
    s32  nGameType;             // 0x004  4 gets a second view
    u8   bDemo;                 // 0x008  the game the menus start is the demo (set by
                                //        FE_MessageTable.c GM_vStartDemo; DEMO_Start runs as the
                                //        menus fade out): no GameBreakers, hole contests, EASBio
                                //        wins or end-of-game scorecard
    u8   unk9[3];
    s32  nC;                    // 0x00C
    u8   nSplitScreen;          // 0x010  0 single view, else split screen (2 = side by side); no luck, no caddie
    u8   b11;                   // 0x011  cleared by Session_Init
    u8   bEndLoop;              // 0x012  ends the main loop of a round (game type 6) or the
                                //        start-up (1): fn_8006D01C (gomainloop.c) checks and clears
                                //        it; set at the end of a game, by the end-of-round screen,
                                //        a replay, the lessons and the fade to black
    u8   bReplay;               // 0x013  a saved replay is playing: no luck swap, no spin, instant launch
    s32  nPaused;               // 0x014  0 running, 1 paused (GameUI GUI_OpenPauseMenu), 2 paused
                                //        for a pulled controller, until every pulled one is back
                                //        (GUI_OnControllerPresent); GameUICommands.c tests 3 too
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
    u8   aD28[5];               // 0xD28  per index, set by Character_RequestClothesUpdateIG
    u8   aD2D[5];               // 0xD2D  per index, set by Character_RequestClothesUpdateFE
    u8   unkD32[0xD38 - 0xD32];
    PlayerProfile aProfile[5];  // 0x0D38
    GameOptions options;        // 0x0E78
    CourseRecord aCourseRecord[NUM_COURSE_RECORDS];    // 0x0F00  per course
    RecordEntry recA[8][5];     // 0x50A0  all-time records: 8 kinds, top 5 each
    RecordEntry recB[3][3][5];  // 0x53C0  3 x 3 kinds, top 5 each
    RecordEntry recC[5][2][5];  // 0x5744  5 x 2 kinds, top 5 each
    u32  nSeed;                 // 0x5B2C
    char* p5B30;                // 0x5B30  while n5B34 is set: the hole file StreamManagerHole_StreamFiles
                                //         loads instead of data/<course>/<hole>/hole.hog
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
    s32  nScoringType;          // 0x004  0 strokes, 1 holes won (match play), 2 skins, 3 Stableford
                                //        (set by each mode's init; GM_GetScoringType)
    s32  nMulligans;            // 0x008  0 none, 1 any number, 2 one per player per nine (GM_PlayerTakeMulligan)
    s32  nC;                    // 0x00C  4 in the team modes
    s32  n10;                   // 0x010  4 in the team modes
    s32  nCurCourse;            // 0x014  the course of the current hole
    s32  nHoleCourse[18];       // 0x018  the round's 18 holes: which course each comes from
    s32  nCurHoleNum;           // 0x060  the current hole's number on its course
    s32  nCurHole;              // 0x064  0..17 in the round
    s32  nHoleNum[18];          // 0x068  and which hole of that course (a custom round mixes courses)
    u8   bHoleSelected[18];     // 0x0B0  holes this round plays (GM_GotoNextSelectedHole)
    u8   bHoleSaved[18];        // 0x0C2  a copy of the selection: the playoff hole pool (GM_Pick_PlayOffHole)
    u8   bInPlayoff;            // 0x0D4  a playoff is being played (the tied modes' playoff starts)
    u8   bPlayoffFullRound;     // 0x0D5  the round before the playoff played all 18 holes
                                //        (GM_FullRoundOfGolf answers it during the playoff)
    u8   unkD6[2];
    s32  nPlayoffHoles;         // 0x0D8  playoff holes started
    s32  nDC;                   // 0x0DC  the tour event's current round (GameModeDriverPGATour)
    s32  nE0;                   // 0x0E0  the event's round count (1 outside a tour event)
    s32  nPinSet[18];           // 0x0E4  per hole: which of its four pin positions (CourseInfo.pin) is used
    s32  n12C;                  // 0x12C
    f32* pPinPos;               // 0x130  the current hole's pin (GoTerrainCollision.c sets it);
                                //        speed golf measures the ball's distance to it
    u8   b134;                  // 0x134  cleared at the start of a hole
    u8   b135;                  // 0x135  set by GM_SetNeedToBuildPlayoffHoleList
    // The holes are not one course's 1..18 when any of these four is set (GM_SetCurrentCourse):
    u8   bCustomRound;          // 0x136  a custom round (nSaveSlot, nSaveCourse)
    u8   bRandom18;             // 0x137  "Random 18" (course 23, GM_BuildRandom18)
    u8   bDream18;              // 0x138  "Dream 18" (course 22, GM_BuildDream18)
    u8   nRegionalRound;        // 0x139  a regional round, 1..6 = courses 24..29
                                //        (GM_BuildRegionalRound); 0 none
    u8   unk13A[2];
    s32  nSaveSlot;             // 0x13C  the save slot (0x10600 bytes each) of a custom round
    s32  nSaveCourse;           // 0x140  the custom round in it (0x70 bytes each)
    s32  n144[5];               // 0x144  per player, cleared at the start of a hole
    s32  n158[5];               // 0x158  per player, cleared at the start of a hole
    u8   b16C[5][18];           // 0x16C  per player and hole, cleared with the hole's score
    u8   unk1C6[0x1C8 - 0x1C6];
    // The mode's callbacks (0x1C8..0x26C). GM_SetModeType sets them all to defaults (mostly empty
    // stubs), then the mode's own setup replaces the ones it needs. The names are TW06's
    // GameModeBase methods, from the modes' implementations (GameModeStroke, GameModeMatch, ...),
    // and TW07's GameModeBase.cpp, which defines its methods in the slots' order.
    void (*pfnInit)(void);      // 0x1C8  the mode's setup. TW06: Init
    void (*pfnShutdown)(void);  // 0x1CC  the mode ends. TW06: Shutdown (GameModeBattle)
    void (*pfnSetupNextGolfer)(void);   // 0x1D0  the next turn: whose shot it is, once every golfer
                                //        waits (GM_SetupGolfer_IfAllWaiting). TW06: SetupNextGolfer
    s32  (*pfnGetHonors)(int nPlayer);  // 0x1D4  who plays after nPlayer (5 = nobody). TW06: GetHonors
    u8   (*pfnHoleFinished)(int nPlayer, u8 bCheck);   // 0x1D8  the hole is over; bCheck 1 only asks
                                //        (Gimme_Allowed). TW06: HoleFinished(PlayerNumber_t, u8)
    u8   (*pfnGameFinished)(u8 bCheck);     // 0x1DC  the game is over. TW06: GameFinished(u8)
    u8   (*pfnGoToPlayoff)(u8 bCheck);      // 0x1E0  TW06: GoToPlayoff(u8). Nothing in the binary
                                //        calls it (0x800CFB88 only adds the slots up)
    void (*pfnLoadHole)(void);  // 0x1E4  a hole starts (GM_InitForHole). TW07: LoadHole (the PGA
                                //        TOUR's is TW06's PostHoleLoadInit)
    void (*pfnEndHole)(void);   // 0x1E8  hole finished. TW06: EndHole
    void (*pfnStartGamePreData)(void);  // 0x1EC  a round starts, before the course data and cameras
                                //        (GM_InitModule_PreDataStream). TW07: StartGamePreData
    void (*pfnStartGamePostData)(void); // 0x1F0  a round starts, after the players are set up
                                //        (GM_InitModule_PostDataStream). TW07: StartGamePostData
    void (*pfnEndGame)(void);   // 0x1F4  game finished. TW06: EndGame
    u8   (*pfnIsPuttForLead)(int nPlayer);  // 0x1F8  holing this ball takes the lead
                                //        (GM_IsPuttForLead). TW07: IsPuttForLead
    u8   (*pfnIsPuttForWin)(int nPlayer);   // 0x1FC  holing this ball wins; asked before the
                                //        special ball pick-up. TW07: IsPuttForWin
    s32  (*pfnGetCurrentLead)(int nPlayer); // 0x200  strokes behind the leader. TW06:
                                //        GetCurrentLead
    s32  (*pfnGetPotentialLead)(int nPlayer);   // 0x204  the same if this putt drops. TW06:
                                //        GetPotentialLead
    s32  (*pfnGetPotentialHoleResult)(int nPlayer); // 0x208  how the hole ends if this ball
                                //        drops. TW07: GetPotentialHoleResult
    void (*pfnPreShotInit)(int nPlayer);    // 0x20C  the pre-shot state starts
                                //        (STATEFUNC_PreShotInit). TW07: PreShotInit
    void (*pfnEndTurnEndHoleNotGame)(int nPlayer);  // 0x210  the hole is over, the game is not
                                //        (GM_HoleFinished_GameNotFinished). TW07:
                                //        EndTurnEndHoleNotGame
    void (*pfnScorecardClosed)(void);   // 0x214  the end-of-hole scorecard was closed
                                //        (GUI_PauseMenuClosed). TW07: ScorecardClosed
    void (*pfnHoledOut)(int nPlayer);   // 0x218  the ball was holed, or picked up at the stroke
                                //        limit (GM_PlayerTookShot); no mode sets it (our name)
    void (*pfnShotOverLimit)(int nPlayer);  // 0x21C  the stroke limit was reached
                                //        (GM_PlayerTookShot). TW07: ShotOverLimit
    void (*pfnUpdate)(void);    // 0x220  every frame of a round (GM_Update, game type 6; our name)
    void (*pfnRestartHole)(void);   // 0x224  the hole restarts (GM_RestartHole). TW07: RestartHole
    void (*pfnSwingUpdate)(int nPlayer);    // 0x228  every frame of the swing state
                                //        (STATEFUNC_SwingUpdate; our name)
    void (*pfnResetShot)(int nPlayer);  // 0x22C  after a re-plan (STATEFUNC_SwingUpdate,
                                //        STATEFUNC_GreenMorphUpdate). TW07: ResetShot
    u8   (*pfnRenderBallTarget)(int nPlayer);   // 0x230  nonzero: the ball target is drawn as in
                                //        the place-ball state (target.c, GameRound.c). TW07:
                                //        RenderBallTarget
    u8   (*pfnCheckControllerPulled)(void);    // 0x234  GM_CheckControllerPulled asks it.
                                //        TW06 / TW07: CheckControllerPulled
    u8   (*pfnOKToShoot)(int nPlayer);  // 0x238  nonzero (the default): the pre-shot state may
                                //        go on to the shot setup (STATEFUNC_PreShotUpdate). TW07:
                                //        OKToShoot
    void (*pfnBallCollision)(int nPlayer);  // 0x23C  the ball hit something (event.c, ball events
                                //        35..38). TW07: BallCollision
    s32  (*pfnTriggerSplash)(int nPlayer);  // 0x240  the ball touched a surface: returns a hit
                                //        effect to play too (PsBallFx.c). TW07: TriggerSplash
    void (*pfnCheckShotAwards)(int nPlayer);    // 0x244  a shot is over, in bounds
                                //        (GM_Earnings_PayShotGoals). TW07: CheckShotAwards
    void (*pfnEndGolferTurn)(int nPlayer); // 0x248  end of a golfer's turn. TW06: EndGolferTurn
    void (*pfnInitialFlyByDone)(int nPlayer);   // 0x24C  the hole's opening flyover ended
                                //        (STATEFUNC_InitialFlyByExit). TW07: InitialFlyByDone
    void (*pfnBallOOB)(int nPlayer);    // 0x250  the ball went out of bounds. TW07: BallOOB
    void (*pfnMulligan)(int nPlayer);   // 0x254  a mulligan was taken. TW07: Mulligan
    u8   (*pfnPickTarget)(int nPlayer); // 0x258  the re-plan button: the target modes pick the next
                                //        target; nonzero lets the re-plan run. TW07: PickTarget
    void (*pfnSetTimer)(int nPlayer, int nTime);    // 0x25C  set the time left. TW07: SetTimer
    void (*pfnHitBall)(int nPlayer);    // 0x260  the ball was hit (EVENT_HitBall). TW07: HitBall
    u8   (*pfnPickPrevTarget)(int nPlayer); // 0x264  "aim at the pin?" for a re-plan; the target
                                //        modes pick the previous target. TW07: PickPrevTarget
    void (*pfnCollisionActor)(int nPlayer, int nId);    // 0x268  the ball hit a world object nId
                                //        (a bonus, GameMode16). TW07: CollisionActor
    s32  (*pfnGreenType)(int a, int nTarget);   // 0x26C  a target's state for its marker
                                //        (GoDynObj.c, GameMode14). TW07: GreenType
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
    u8   bAllowGameBreakers;    // 0x285  the mode allows GameBreakers (GameEffects.c). TW07:
                                //        AllowGameBreakers
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
// target.c): the "tball" texture drawn at the target. Putts get a different set (TARGET_SetupTarget).
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
    f32  f24;                   // 0x24  twice this is a size (TARGET_RenderBallTarget)
    u8   b28;                   // 0x28  the marker was on screen last frame (TARGET_RenderBallTarget)
    u8   unk29[3];
} TargetMarker;
LAYOUT_ASSERT(TargetMarker, 0x2C);

extern TargetMarker lbl_801D5BF0[5];   // per player
extern TNetwork*    lbl_80281E30;       // the hole's chunk 3 (fn_8006A7A8): an outline the placed ball
                                        // must be inside, NULL when the hole has none
extern TexBank*     lbl_80281E34;       // } the "shadow" texture
extern TexEntry*    lbl_80281E38;       // }
extern TexBank*     lbl_80281E3C;       // } the "tball" texture (the aim marker)
extern TexEntry*    lbl_80281E40;       // }
extern f32          lbl_801887CC[4];    // RGBA: the placement text where the ball can go (PlaceBall_RenderBallTarget)
extern f32          lbl_801887DC[4];    // RGBA: the placement text where it can't
extern f32          lbl_801887EC[4];    // RGBA: the placement marker

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
int  Golfer_FindById(int nId);          // the gGolferTable row with that nModelID, -1 none
void fn_8002EBA4(u8* pObj, u8 nValue);  // set byte 7 of the options (a7[0]) and apply it (Golfer.c)
int  Course_GetCurHolePar(void);
s32  fn_800D2C68(int nTee);             // CourseData.c: the current hole's value for tee set nTee
int  Lessons_GetShotKind(void);                 // shot kind override, 8 = none
int  Lessons_GetClub(int nPlayer);          // club override, 26 = none
int  Game_GetCurHoleNum(void);
u8   Lessons_AllowCPUSpin(void);
f32  SW_GetSpinScale(int nSpin);         // how much spin SPIN allows: 0.15 at 0 .. 1.0 at 110 (Swing.c)
f32  SW_vGetNonPowerAttributeAffectedShotPower(int nPlayer);          // the swing's fNonPowerShotPower (Swing.c)
void SW_KillVibration(int nPlayer);      // stops the pad rumble (Swing.c)
void SW_vClearBoosts(int nPlayer);  // Swing.c

u8   Club_UsableForKind(int nPlayer, int nClub, int nKind);
int  AI_FirstUsableClub(int nPlayer, int nKind);
f32  AI_MaxDistance(int nPlayer, int nKind, int nClub);
int  AI_ShotKindForDistance(int nPlayer, f32 fDist);
int  AI_ClubForShot(int nPlayer, int nKind, u8 bUnderOnly, f32 fDist);
f32  AI_PowerForTarget(int nPlayer);
s8   AI_NearestTarget(f32* pPos, f32* pOut);
void AI_DefaultTarget(int nPlayer);
void AI_NudgeAim(int nPlayer, f32 fDelta);
void AI_NudgeDistance(int nPlayer, f32 fDelta);
int  Shot_GoverningAttribute(int nPlayer, int nClub, int nLie, int nKind);
u8   Golfer_IsLucky(int nPlayer);
s8   Caddie_GetTip(int nPlayer, f32* pOut);     // 0 no tip, 1 the aim point in pOut, 2 gave up
u8   Player_IsCPU(int nPlayer);
u8   Controller_IsCPU(int nController);
u8   fn_8002E868_HasPad(int nPlayer);
u8   fn_8002E898_IsPad(int nController);
u8   Player_IsController8(int nPlayer);
u8   Player_OnTee(int nPlayer);
u8   Player_IsHoled(int nPlayer);
u8   Team_IsAllHuman(int nTeam);        // team 0 is players 0 and 1, team 1 players 2 and 3
void fn_8002BDEC_SetTarget(int nPlayer, f32* pTarget);
u8   AI_GreenTowardPin(int nPlayer, f32 fDist);
u8   AI_RehearseShot(int nPlayer, f32* pOutDist2, u8 bFast, f32 fTolerance);
void AI_ApplyError(int nPlayer);
u8   Player_NotInSand(int nPlayer);
void Shot_FitTargetToClub(int nPlayer);
void Shot_Plan(int nPlayer, u8 bNotify);
void Shot_Prepare(int nPlayer, u8 bNotify);
int  Shot_Trajectory(int nPlayer);
void fn_8002D544_StraightDir(int nPlayer, f32* pOut);
void fn_8002D680_CpuShapeDir(int nPlayer, f32* pOut);
f32  Shot_AimAngle(int nPlayer);
void AI_ClubLonger(int nPlayer, s32* pClub, int nStep);
void AI_ClubShorter(int nPlayer, s32* pClub, int nStep);
void AI_ChooseTarget(int nPlayer);
void GOLFERSTATE_Kill(int nPlayer);      // Swing.c: pop every state
f32  AI_PowerScale(int nPlayer);
void fn_8002D560_ShapeDir(int nPlayer, f32* pOut);
void Caddie_Start(int nPlayer);
void Caddie_Stop(void);
void Caddie_Update(int nPlayer);
void Luck_TakePerfectShot(int nPlayer);
void Caddie_ApplyTip(int nPlayer);
int  Golfer_GetAttribute(Player* pPlayer, int nAttr, int nMode);
u8   fn_8002E8E4(int nController);  // Golfer.c
f32  SW_vGetHookSlice(int nPlayer);          // Swing.c
f32  SW_vGetMishitAngle(int nPlayer);          // Swing.c
int  SW_fGetBoostMagnitude(int nPlayer);          // Swing.c
int  SW_fGetSpinMagnitude(int nPlayer);          // Swing.c
u8   Player_IsHoledNotState23(int nPlayer);
u8   Team_IsAllCPU(int nTeam);
u8   fn_8002E8B4(int nPlayer);
u8   Bag_AddClub(int nPlayer, int nBit);
u8   Bag_RemoveClub(int nPlayer, int nBit);
u8   Bag_HasClub(int nPlayer, int nBit);
int  Bag_CountClubs(int nPlayer);
void Session_SetGolfer(int nGolfer, int nPlayer);
void Session_SetupProfiles(void);


#endif
