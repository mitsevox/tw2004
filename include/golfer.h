// golfer.h: the golfer record, the player struct and the attribute accessor, shared by
// Golfer.c (the CPU golfer) and Swing.c (the human swing). Layouts are from the reads in
// those files; see docs/formats/game-data.md and docs/gameplay.md.

#ifndef GOLFER_H
#define GOLFER_H

#include "game_types.h"

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
#define NUM_CLUBS            26   // clubs 0..24 are the bag, 25 the putter
#define CLUB_PUTTER          25
#define CLUB_SAND_WEDGE      21
// A CPU shot's shape, from the authored aim point. The rehearsal compensates each one when it
// fails: the curves by turning the aim 1 or 2 degrees, the trajectories by 5 yards of distance.
enum {
    SHAPE_STRAIGHT    = 0,
    SHAPE_CURVE_A     = 1,      // clubface x +0.02
    SHAPE_CURVE_B     = 2,      // clubface x -0.02
    SHAPE_LOW         = 3,      // trajectory 2 (which of 3/4 is the punch is unverified)
    SHAPE_HIGH        = 4,      // trajectory 0
    SHAPE_BIG_CURVE_A = 5,      // clubface x +0.04
    SHAPE_BIG_CURVE_B = 6       // clubface x -0.04
};

#define SHOT_PUTT            0    // shot kinds (Player.nShotKind)
#define SHOT_FULL            1
#define SHOT_CHIP            2
#define SHOT_PITCH           3
#define LIE_GREEN            9
#define NUM_AI_LINKS         10   // candidate aim points per zone
#define NUM_AI_TARGETS       25   // aim points per hole (gAITargets)

#define PI    3.14159265f
#define TWOPI 6.28318531f
#define DEG(x) ((x) * (PI / 180.0f))

#define IABS(v) (((v) ^ ((v) >> 31)) - ((v) >> 31))   // what the compiler emits for abs()

// One 320-byte row of STATS_GC.BIN as it sits in gGolferTable. The file has a 2-byte header,
// so every field is 2 bytes later than in the file; the game reuses the first byte as the
// golfer index once a row is copied into a player.
typedef struct GolferRecord {
    u8   nIndex;                // 0x000
    u8   unk1;                  // 0x001
    char szFirst[32];           // 0x002
    char szLast[32];            // 0x022
    char szNick[32];            // 0x042
    u8   unk62[6];              // 0x062
    s8   attr[NUM_ATTRS];       // 0x068  block A
    s8   attrAlt[NUM_ATTRS];    // 0x074  block B: used for CPU pros in game mode 4
    s8   tier[14];              // 0x080  equipment tiers 0..4, one per attribute
    u8   unk8E;                 // 0x08E  non-zero in gCurGolferRecord when there is one
    u8   unk8F;                 // 0x08F
    u32  uBagMask;              // 0x090  bit n set = club n is in the bag
    u8   unk94[0x140 - 0x94];
} GolferRecord;

// The swing meter's per-player state, embedded in Player at 0x3D4 (offsets below are within
// this struct; add 0x3D4 for the player offset).
typedef struct SwingData {
    u8   unk0[0x18];
    s32  nTopX;                 // 0x018  (0x3EC) stick at the top of the backswing
    s32  nTopY;                 // 0x01C  (0x3F0)
    s32  nImpactX;              // 0x020  (0x3F4) stick at impact
    s32  nImpactY;              // 0x024  (0x3F8)
    u8   unk28[0x3C - 0x28];
    f32  fSwingError;           // 0x03C  (0x410) the stick's miss, after forgiveness
    f32  fLaunchPower;          // 0x040  (0x414) Swing_ComputePower's result
    f32  fLaunchAX;             // 0x044  (0x418) copy of vLaunchA[0]
    f32  fPowerAfterError;      // 0x048  (0x41C)
    f32  fBackAngle;            // 0x04C  (0x420) the backswing's sideways angle, radians (0 on a putt)
    u8   unk50[0x378 - 0x50];
    s32  nCentreX;              // 0x378  (0x74C) stick at the start of the swing
    s32  nCentreY;              // 0x37C  (0x750)
    u8   unk380[0x460 - 0x380];
    s32  nRumbleFrames;         // 0x460  (0x834)
    u8   bRumble;               // 0x464  (0x838)
    u8   unk465[0x494 - 0x465];
    s32  nBoostLevel;           // 0x494  (0x868) power boost level pressed, 0..8
    u8   unk498[0x610 - 0x498];
    s32  nSpinAmount;           // 0x610  (0x9E4) how much spin was asked for, 0..20
    s32  nSpinStickX;           // 0x614  (0x9E8) 0..255, 128 centre
    s32  nSpinStickY;           // 0x618  (0x9EC)
    f32  fSpinX;                // 0x61C  (0x9F0)
    f32  fSpinY;                // 0x620  (0x9F4)
    u8   unk624[0x630 - 0x624];
    u8   unk630;                // 0x630  (0xA04) cleared by Player_SetGolfer
    u8   unk631[3];
} SwingData;

// A player in the current round (human or CPU). 0xEF8 bytes; only the fields read so far.
typedef struct Player {
    s32  nIndex;                // 0x000
    s32  unk4;                  // 0x004
    GolferRecord golfer;        // 0x008
    s8   attrMod[NUM_ATTRS];    // 0x148  modifiers on top of the record
    s32  nStrokes[18];          // 0x154  strokes taken per hole
    u8   unk19C[0x278 - 0x19C];
    s32  nHolesWon;             // 0x278  match play
    u8   unk27C[0x354 - 0x27C];
    s32  nClub;                 // 0x354
    s32  nClubPerKind[8];       // 0x358  the club Shot_Prepare would pick for each shot kind
    f32  fAim;                  // 0x378  aim angle, radians
    f32  fPower;                // 0x37C  0..1 (up to 1.5)
    s32  nShotKind;             // 0x380  0 putt, 2/3 approach, 5..7 recovery
    s32  nTrajectory;           // 0x384  from Shot_Trajectory
    f32  vLaunchA[4];           // 0x388  launch parameter blocks handed to Ball_Launch
    f32  vLaunchB[4];           // 0x398
    s32  nShotShape;            // 0x3A8  SHAPE_*: what the aim point (or a lesson) asks the CPU to play
    u8   bPerfect;              // 0x3AC  no error / no forgiveness when set
    u8   unk3AD[3];
    s32  nShotKind2;            // 0x3B0
    f32  fBallX;                // 0x3B4
    f32  fBallY;                // 0x3B8
    f32  fBallZ;                // 0x3BC
    f32  fBallW;                // 0x3C0
    u8   unk3C4[0x3D4 - 0x3C4];
    SwingData swing;            // 0x3D4  the swing meter's state for this player
    s32  nController;           // 0xA08  CONTROLLER_CPU for the AI
    s32  nView0;                // 0xA0C
    s32  nView1;                // 0xA10
    f32  fTargetX;              // 0xA14
    f32  fTargetY;              // 0xA18
    f32  fTargetZ;              // 0xA1C
    f32  fTargetW;              // 0xA20
    f32  vTargetCopy[4];        // 0xA24  copy of the planned target
    f32  vTarget2[4];           // 0xA34  copy of the chosen aim point
    u8   unkA44[0xA54 - 0xA44];
    f32  fDistance;             // 0xA54  to the target
    f32  fDistance2;            // 0xA58
    u8   unkA5C[0xA68 - 0xA5C];
    s32  nSurface;              // 0xA68  surface type under the target, -1 none, 16 water
    u8   unkA6C[0xA90 - 0xA6C];
    u8   ball[0x68];            // 0xA90  the player's Ball (0xBC bytes, see Ball.c) - nLie is its +0x68
    s32  nLie;                  // 0xAF8
    u8   unkAFC[0xB4C - 0xAFC];
    f32  vOrient[4];            // 0xB4C  a quaternion, identity at setup
    u8   unkB5C[0xC18 - 0xB5C];
    s32  nShotHandle;           // 0xC18
    f32  fThinkTime;            // 0xC1C  seconds a CPU has spent in state 2
    f32  fC20;                  // 0xC20
    u8   unkC24[4];
    u8   unkC28;                // 0xC28
    u8   bLowIQPenalty;         // 0xC29  quarters the IQ overconfidence term when set
    s8   nLevel;                // 0xC2A  CPU difficulty level: 25 modifier points per level
    u8   unkC2B;                // 0xC2B
    u8   unkC2C;                // 0xC2C
    u8   unkC2D;                // 0xC2D
    u8   unkC2E[2];
    s32  nRehearseState;        // 0xC30  AI_RehearseShot state machine
    u8   unkC34[0xEE8 - 0xC34];
    u32  uFlags;                // 0xEE8
    u8   unkEEC[0xEF0 - 0xEEC];
    u32  uFlagsEF0;             // 0xEF0  bit 1: target is over water
    u8   unkEF4[0xEF8 - 0xEF4];
} Player;

// Terrain surface descriptors (0x44 bytes each); only the index of one is used here.
typedef struct SurfaceType {
    u8   unk[0x2C];
    u32  nClass;                // 0x2C  3 = green
    u8   unk30[0x44 - 0x30];
} SurfaceType;

// The round / session state at gSession (0x5BD0 bytes); only what this file reads.
typedef struct Session {
    u32  uFlags;                // 0x000  bit 1: use the alternate attribute block everywhere;
                                //        bit 9: every club in the bag
    s32  nGameType;             // 0x004  4 gets a second view
    u8   unk8[4];
    s32  unkC;                  // 0x00C
    u8   nSplitScreen;          // 0x010  0 single view, else split screen (2 = side by side); no luck, no caddie
    u8   unk11[2];
    u8   bNoSpin;               // 0x013  spin control switched off
    s32  unk14;                 // 0x014
    f32  fFrameTime;            // 0x018  seconds per frame
    f32  f1C;                   // 0x01C
    s32  unk20;                 // 0x020
    s32  unk24;                 // 0x024
    s32  unk28;                 // 0x028
    s32  nNumPlayers;           // 0x02C
    s32  nController[5];        // 0x030  per player (slot 4 is the caddie / lucky-shot copy)
    s32  nGolfer[5];            // 0x044  golfer index per player
    s32  nTeeSet[5];            // 0x058
    u32  uBag[5];               // 0x06C  per player, 0 = the record's own
    u8   unk80[0x5B2C - 0x80];  // profiles at 0xD38 (PlayerProfile x 5), options at 0xE78
    u32  nSeed;                 // 0x5B2C
    u8   unk5B30[8];
    s8   unk5B38;               // 0x5B38
    u8   unk5B39;               // 0x5B39
    u8   unk5B3A[2];
    f32  f5B3C;                 // 0x5B3C
    f32  f5B40;                 // 0x5B40  150
    f32  f5B44;                 // 0x5B44  -400
    f32  f5B48;                 // 0x5B48  1
    u8   unk5B4C[0x5BD0 - 0x5B4C];
} Session;

// The game state gpGame points at; only what this file reads.
typedef struct GameState {
    s32  nMode;                 // 0x000
    u8   unk4[0x14 - 0x4];
    s32  unk14;                 // 0x014
    u8   unk18[0x64 - 0x18];
    s32  nCurHole;              // 0x064  index into holeOrder
    u8   unk68[0xE4 - 0x68];
    s32  holeOrder[18];         // 0x0E4
} GameState;

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
    s8   nHole;                 // 0x06  -1 = any
    s8   nSkillReq;             // 0x07
    s8   nAggrReq;              // 0x08
    s8   bPriority;             // 0x09  taken when nothing else qualifies (and by humans)
    s8   nType;                 // 0x0A
    s8   nPowerReq;             // 0x0B
} AITarget;

// Per-course data: only the pin positions are read here.
typedef struct PinPos {
    f32  x, y, z, w;
} PinPos;

typedef struct CourseInfo {
    u8     unk0[0x70];
    PinPos pin[18];             // 0x70
} CourseInfo;

extern GolferRecord gGolferTable[34];   // 0x801CB300  STATS_GC.BIN as loaded
extern GolferRecord gCurGolferRecord;   // 0x801CB1C0  the created golfer being edited
extern Player       gPlayers[5];        // 0x801C66E8
extern Session      gSession;           // 0x801CDD80
extern GameState*   gpGame;             // 0x80281588

extern AITarget     gAITargets[25];     // 0x801C65B8
extern s32          gNumAITargets;      // 0x80281D44
extern u8           gClubKindTable[8][NUM_CLUBS];   // 0x801874B0  which clubs each shot kind allows
extern f32          gClubDistAtPower0[NUM_CLUBS];   // 0x80187580  reach at POWER 0 (245 for the woods)
extern f32          gClubPowerStep[NUM_CLUBS];      // 0x801875E8  reach gained per POWER point over 100
extern SurfaceType  gSurfaceTypes[];    // 0x8017E9B8

int  Game_GetMode(void);                // 0x8000BED8
int  fn_800D2B08(void);
u32  Rand_Next(int nStream);            // 0x8000B130  EA's lagged-Fibonacci generator
f32  Rand_Float(int nStream);           // 0x8000B428  [0, 1)
f32  fn_800095F0(f32 fAngle);           // sin
f32  fn_80009638(f32 fAngle);           // cos
double fn_80009680(double x);           // sqrt
double fn_8015F824(double x, double y); // pow
void Vec_Copy(f32* pSrc, f32* pDst);   // 0x8000AD10
f32  Terrain_HeightAt(f32* pPos, SurfaceType** ppSurface);   // 0x800447DC
f32  fn_80050D34(f32 fDist);            // putt power for a distance
f32  fn_80050F44(int nKind, int nClub); // a club's table reach for a shot kind
f32  fn_80050F88(f32 fDist, u8* pParams, int nKind, int nClub);   // chip power
f32  fn_800510EC(u8* pParams);          // distance scale
int  fn_80100744(void);                 // shot kind override, 8 = none
int  fn_801006F0(int nPlayer);          // club override, 26 = none
CourseInfo* fn_8000C594(void);
int  fn_80015464(void);
u8   fn_80101DF4(void);
f32  fn_8005C418(int nSpin);

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
void AI_PlanShot(int nPlayer, f32* pTarget);
u8   AI_GreenTowardPin(int nPlayer, f32 fDist);
u8   AI_RehearseShot(int nPlayer, f32* pOutDist2, u8 bFast, f32 fTolerance);
u8   Lie_AllowsFullSwing(int nPlayer);
void Shot_FitTargetToClub(int nPlayer);
int  Shot_Trajectory(int nPlayer);
void Shot_DefaultSpin(int nPlayer, f32* pOut);
void Shot_FaceVector(int nPlayer, f32* pOut);
f32  Shot_AimAngle(int nPlayer);
void AI_ClubLonger(int nPlayer, s32* pClub, int nStep);
void AI_ClubShorter(int nPlayer, s32* pClub, int nStep);

#endif
